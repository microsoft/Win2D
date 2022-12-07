// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#include "WicAdapter.h"

class DeviceContextLease;

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas
{
    using namespace ::Microsoft::WRL;
    using namespace ABI::Windows::Foundation;
    using namespace ABI::Windows::Storage::Streams;


    class __declspec(uuid("2F434224-053C-4978-87C4-CFAAFA2F4FAC"))
    ICanvasImageInternal : public ICanvasImageInterop
    {
    public:
        IFACEMETHODIMP GetD2DImage(
            ICanvasDevice* device,
            ID2D1DeviceContext* deviceContext,
            WIN2D_GET_D2D_IMAGE_FLAGS flags,
            float targetDpi,
            float* realizeDpi,
            ID2D1Image** ppImage) override
        {
            // GetD2DImage is exposed as a COM interface to external users, so make sure exceptions never cross the ABI boundary.
            return ExceptionBoundary([&]
                {
                    auto d2dImage = GetD2DImage(device, deviceContext, flags, targetDpi, realizeDpi);

                    ThrowIfFailed(d2dImage.CopyTo(ppImage));
                });
        }

        // For bitmaps and command lists, GetD2DImage is a trivial getter.
        // For effects it triggers on-demand realization, which requires extra information.
        //
        // The device parameter is required, while deviceContext is optional (but recommended)
        // except when the WIN2D_GET_D2D_IMAGE_FLAGS_READ_DPI_FROM_DEVICE_CONTEXT flag is specified.
        // This is because not all callers of GetD2DImage have easy access to a context. It is always
        // possible to get a resource creation context from the device, but the context is only actually
        // necessary if a new effect realization needs to be created, so it is more efficient to have the
        // implementation do this lookup only if/when it turns out to be needed.
        //
        // targetDpi passes in the DPI of the target device context. This is used to
        // determine when a D2D1DpiCompensation effect needs to be inserted. Behavior of
        // this parameter can be overridden by the flag values WIN2D_GET_D2D_IMAGE_FLAGS_READ_DPI_FROM_DEVICE_CONTEXT,
        // WIN2D_GET_D2D_IMAGE_FLAGS_ALWAYS_INSERT_DPI_COMPENSATION, or WIN2D_GET_D2D_IMAGE_FLAGS_NEVER_INSERT_DPI_COMPENSATION.
        //
        // realizedDpi returns the DPI of a source bitmap, or zero if the image does not
        // have a fixed DPI. A D2D1DpiCompensation effect will be inserted if targetDpi
        // and realizedDpi are different (flags permitting).

        virtual ComPtr<ID2D1Image> GetD2DImage(
            ICanvasDevice* device,
            ID2D1DeviceContext* deviceContext,
            WIN2D_GET_D2D_IMAGE_FLAGS flags = WIN2D_GET_D2D_IMAGE_FLAGS_READ_DPI_FROM_DEVICE_CONTEXT,
            float targetDpi = 0,
            float* realizedDpi = nullptr) = 0;

        // Static helper for internal callers to invoke GetD2DImage on multiple input types. That is, this method
        // handles not just ICanvasImageInternal sources, but ICanvasImageInterop source as well. This way, callers
        // don't have to worry about the second interop interface, and can just get an image from a single code path.

        static ComPtr<ID2D1Image> GetD2DImageFromInternalOrInteropSource(
            IUnknown* canvasImage,
            ICanvasDevice* device,
            ID2D1DeviceContext* deviceContext,
            WIN2D_GET_D2D_IMAGE_FLAGS flags = WIN2D_GET_D2D_IMAGE_FLAGS_READ_DPI_FROM_DEVICE_CONTEXT,
            float targetDpi = 0,
            float* realizedDpi = nullptr)
        {
            // The input is an IUnknown* object, where callers pass in either ICanvasImage* or IGraphicsEffectSource.
            // There are two possible cases for the input image being retrieved:
            //   - It is one of the built-in Win2D effects, so it will implement ICanvasImageInternal.
            //   - It is a custom Win2D-compatible effect, so it will implement ICanvasImageInterop.
            ComPtr<ID2D1Image> d2dImage;
            ComPtr<ICanvasImageInternal> internalSource;

            // Check if the specified source is an ICanvasImageInternal (this is the most common case).
            HRESULT hr = canvasImage->QueryInterface(IID_PPV_ARGS(&internalSource));

            // If QueryInterface failed in any way other than E_NOINTERFACE, we just rethrow (just like in CanvasEffect::SetD2DInput).
            if (FAILED(hr) && hr != E_NOINTERFACE)
                ThrowHR(hr);

            // If the input was indeed an ICanvasImageInternal, invoke its GetD2DImage method normally.
            if (SUCCEEDED(hr))
            {
                d2dImage = internalSource->GetD2DImage(device, deviceContext, flags, targetDpi, realizedDpi);
            }
            else
            {
                // If the source is not an ICanvasImageInternal, it must be an ICanvasImageInterop object.
                hr = As<ICanvasImageInterop>(canvasImage)->GetD2DImage(device, deviceContext, flags, targetDpi, realizedDpi, &d2dImage);

                // To match the behavior of ICanvasImageInternal::GetD2DImage in case of failure, check if the flags being used did have the flag
                // WIN2D_GET_D2D_IMAGE_FLAGS_UNREALIZE_ON_FAILURE set. If not, and the call failed, then we explicitly throw from the returned HRESULT.
                if ((flags & WIN2D_GET_D2D_IMAGE_FLAGS_UNREALIZE_ON_FAILURE) == WIN2D_GET_D2D_IMAGE_FLAGS_NONE)
                    ThrowIfFailed(hr);
            }

            return d2dImage;
        }
    };

    HRESULT GetImageBoundsImpl(
        ICanvasImageInternal* imageInternal,
        ICanvasResourceCreator* resourceCreator,
        Numerics::Matrix3x2 const* optionalTransform,
        Rect* bounds);

    DeviceContextLease GetDeviceContextForGetBounds(ICanvasDevice* device, ICanvasResourceCreator* resourceCreator);

    class DefaultCanvasImageAdapter;
    
    class CanvasImageAdapter : public Singleton<CanvasImageAdapter, DefaultCanvasImageAdapter>
    {
    public:
        virtual ~CanvasImageAdapter() = default;

        virtual ComPtr<IAsyncAction> RunAsync(std::function<void()>&& fn) = 0;

        virtual ComPtr<IStream> CreateStreamOverRandomAccessStream(IRandomAccessStream* stream) = 0;

        virtual void SaveImage(
            ID2D1Image* d2dImage,
            WICImageParameters const& wicImageParameters,
            ID2D1Device* device,
            IStream* stream,
            GUID const& containerFormat,
            float quality) = 0;
    };


    class DefaultCanvasImageAdapter : public CanvasImageAdapter
    {
        std::shared_ptr<WicAdapter> m_wicAdapter;
        
    public:
        virtual ComPtr<IAsyncAction> RunAsync(std::function<void()>&& fn) override;
        
        virtual ComPtr<IStream> CreateStreamOverRandomAccessStream(IRandomAccessStream* stream) override;

        virtual void SaveImage(
            ID2D1Image* d2dImage,
            WICImageParameters const& wicImageParameters,
            ID2D1Device* device,
            IStream* stream,
            GUID const& containerFormat,
            float quality) override;

    private:
        ComPtr<IWICImagingFactory2> const& GetFactory();
    };


    class CanvasImageFactory
        : public AgileActivationFactory<ICanvasImageStatics>
        , private LifespanTracker<CanvasImageFactory>
    {
        InspectableClassStatic(RuntimeClass_Microsoft_Graphics_Canvas_CanvasImage, BaseTrust);

        std::shared_ptr<CanvasImageAdapter> m_adapter;
        
    public:
        CanvasImageFactory();
        
        IFACEMETHODIMP SaveAsync(
            ICanvasImage* image,
            Rect sourceRectangle,
            float dpi,
            ICanvasResourceCreator* resourceCreator,
            IRandomAccessStream* stream,
            CanvasBitmapFileFormat fileFormat,
            IAsyncAction** action) override;

        IFACEMETHODIMP SaveWithQualityAsync(
            ICanvasImage* image,
            Rect sourceRectangle,
            float dpi,
            ICanvasResourceCreator* resourceCreator,
            IRandomAccessStream* stream,
            CanvasBitmapFileFormat fileFormat,
            float quality,
            IAsyncAction** action) override;

        IFACEMETHODIMP SaveWithQualityAndBufferPrecisionAsync(
            ICanvasImage* image,
            Rect sourceRectangle,
            float dpi,
            ICanvasResourceCreator* resourceCreator,
            IRandomAccessStream* stream,
            CanvasBitmapFileFormat fileFormat,
            float quality,
            CanvasBufferPrecision bufferPrecision,
            IAsyncAction** action) override;

        IFACEMETHODIMP ComputeHistogram(
            ICanvasImage* image,
            Rect sourceRectangle,
            ICanvasResourceCreator* resourceCreator,
            Effects::EffectChannelSelect channelSelect,
            int32_t numberOfBins,
            uint32_t* valueCount,
            float** valueElements) override;

        IFACEMETHODIMP IsHistogramSupported(
            ICanvasDevice* device,
            boolean* result) override;
    };
}}}}
