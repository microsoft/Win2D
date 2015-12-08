// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas
{
    using namespace ABI::Windows::Foundation;
    
    static Rect GetImageBoundsImpl(
        ICanvasImageInternal* imageInternal,
        ICanvasDrawingSession *drawingSession,
        Numerics::Matrix3x2 const* transform)
    {
        ComPtr<ICanvasDevice> canvasDevice;
        ThrowIfFailed(As<ICanvasResourceCreator>(drawingSession)->get_Device(&canvasDevice));

        auto drawingSessionResourceWrapper = As<ICanvasResourceWrapperNative>(drawingSession);

        ComPtr<ID2D1DeviceContext1> d2dDeviceContext;
        ThrowIfFailed(drawingSessionResourceWrapper->GetNativeResource(nullptr, 0, IID_PPV_ARGS(&d2dDeviceContext)));

        auto d2dImage = imageInternal->GetD2DImage(canvasDevice.Get(), d2dDeviceContext.Get());

        D2D1_RECT_F d2dBounds;
        
        D2D1_MATRIX_3X2_F previousTransform;
        d2dDeviceContext->GetTransform(&previousTransform);

        auto restoreTransformWarden = MakeScopeWarden([&] { d2dDeviceContext->SetTransform(previousTransform); });

        d2dDeviceContext->SetTransform(ReinterpretAs<D2D1_MATRIX_3X2_F const*>(transform));

        ThrowIfFailed(d2dDeviceContext->GetImageWorldBounds(d2dImage.Get(), &d2dBounds));

        return FromD2DRect(d2dBounds);
    }

    HRESULT GetImageBoundsImpl(
        ICanvasImageInternal* imageInternal,
        ICanvasDrawingSession* drawingSession,
        Numerics::Matrix3x2 const* transform,
        Rect* bounds)
    {
        static Numerics::Matrix3x2 identity = { 1, 0, 0, 1, 0, 0 };

        if (!transform)
            transform = &identity;

        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(drawingSession);
                CheckInPointer(bounds);

                *bounds = GetImageBoundsImpl(imageInternal, drawingSession, transform);
            });
    }


    //
    // CanvasImageFactory
    //
    

    ActivatableStaticOnlyFactory(CanvasImageFactory);

    
    CanvasImageFactory::CanvasImageFactory()
        : m_adapter(CanvasImageAdapter::GetInstance())
    {
    }

    
    IFACEMETHODIMP CanvasImageFactory::SaveAsync(
        ICanvasImage* image,
        Rect sourceRectangle,
        float dpi,
        ICanvasResourceCreator* resourceCreator,
        IRandomAccessStream* stream,
        CanvasBitmapFileFormat fileFormat,
        IAsyncAction** action)
    {
        return SaveWithQualityAsync(
            image,
            sourceRectangle,
            dpi,
            resourceCreator,
            stream,
            fileFormat,
            DEFAULT_CANVASBITMAP_QUALITY,
            action);
    }
    

    IFACEMETHODIMP CanvasImageFactory::SaveWithQualityAsync(
        ICanvasImage* image,
        Rect sourceRectangle,
        float dpi,
        ICanvasResourceCreator* resourceCreator,
        IRandomAccessStream* stream,
        CanvasBitmapFileFormat fileFormat,
        float quality,
        IAsyncAction** action)
    {
        return SaveWithQualityAndBufferPrecisionAsync(
            image,
            sourceRectangle,
            dpi,
            resourceCreator,
            stream,
            fileFormat,
            quality,
            CanvasBufferPrecision::Precision8UIntNormalized,
            action);
    }


    static DXGI_FORMAT GetFormat(CanvasBufferPrecision precision)
    {
        switch (precision)
        {
        case CanvasBufferPrecision::Precision8UIntNormalized: return DXGI_FORMAT_B8G8R8A8_UNORM;
        case CanvasBufferPrecision::Precision8UIntNormalizedSrgb: return DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
        case CanvasBufferPrecision::Precision16UIntNormalized: return DXGI_FORMAT_R16G16B16A16_UNORM;
        case CanvasBufferPrecision::Precision16Float: return DXGI_FORMAT_R16G16B16A16_FLOAT;
        case CanvasBufferPrecision::Precision32Float: return DXGI_FORMAT_R32G32B32A32_FLOAT;
        default:
            ThrowHR(E_INVALIDARG);
        }
    }
    

    IFACEMETHODIMP CanvasImageFactory::SaveWithQualityAndBufferPrecisionAsync(
        ICanvasImage* image,
        Rect sourceRectangle,
        float dpi,
        ICanvasResourceCreator* resourceCreator,
        IRandomAccessStream* stream,
        CanvasBitmapFileFormat fileFormat,
        float quality,
        CanvasBufferPrecision bufferPrecision,
        IAsyncAction** action)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(image);
                CheckInPointer(resourceCreator);
                CheckInPointer(stream);
                CheckAndClearOutPointer(action);

                if (fileFormat == CanvasBitmapFileFormat::Auto)
                    ThrowHR(E_INVALIDARG, Strings::AutoFileFormatNotAllowed);

                if (quality < 0.0f || quality > 1.0f)
                    ThrowHR(E_INVALIDARG);

                WICImageParameters wicImageParameters{};
                wicImageParameters.PixelFormat.format = GetFormat(bufferPrecision);
                wicImageParameters.PixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
                wicImageParameters.DpiX = dpi;
                wicImageParameters.DpiY = dpi;
                wicImageParameters.Top = sourceRectangle.Y * dpi / DEFAULT_DPI;
                wicImageParameters.Left = sourceRectangle.X * dpi / DEFAULT_DPI;
                wicImageParameters.PixelWidth = static_cast<uint32_t>(SizeDipsToPixels(sourceRectangle.Width, dpi));
                wicImageParameters.PixelHeight = static_cast<uint32_t>(SizeDipsToPixels(sourceRectangle.Height, dpi));

                auto canvasDevice = GetCanvasDevice(resourceCreator);
                auto d2dDevice = GetWrappedResource<ID2D1Device>(canvasDevice);

                auto d2dImage = GetWrappedResource<ID2D1Image>(image, canvasDevice.Get());

                auto adapter = m_adapter;
                auto istream = adapter->CreateStreamOverRandomAccessStream(stream);

                auto newAction = adapter->RunAsync(
                    [=]
                    {
                        adapter->SaveImage(
                            d2dImage.Get(),
                            wicImageParameters,
                            d2dDevice.Get(),
                            istream.Get(),
                            GetGUIDForFileFormat(fileFormat),
                            quality);
                    });
                ThrowIfFailed(newAction.CopyTo(action));
            });
    }


    ComPtr<IAsyncAction> DefaultCanvasImageAdapter::RunAsync(
        std::function<void()>&& fn)
    {
        auto action = Make<AsyncAction>(std::move(fn));
        CheckMakeResult(action);
        return action;
    }


    ComPtr<IStream> DefaultCanvasImageAdapter::CreateStreamOverRandomAccessStream(
        IRandomAccessStream* stream)
    {
        ComPtr<IStream> istream;
        ThrowIfFailed(::CreateStreamOverRandomAccessStream(stream, IID_PPV_ARGS(&istream)));
        return istream;
    }

    
    void DefaultCanvasImageAdapter::SaveImage(
        ID2D1Image* image,
        WICImageParameters const& parameters,
        ID2D1Device* device,
        IStream* stream,
        GUID const& containerFormat,
        float quality)
    {        
        auto factory = GetFactory();

        ComPtr<IWICBitmapEncoder> encoder;
        ThrowIfFailed(factory->CreateEncoder(containerFormat, nullptr, &encoder));
        ThrowIfFailed(encoder->Initialize(stream, WICBitmapEncoderNoCache));

        ComPtr<IWICBitmapFrameEncode> frame;
        ComPtr<IPropertyBag2> frameProperties;
        ThrowIfFailed(encoder->CreateNewFrame(&frame, &frameProperties));

        bool supportsQuality =
            containerFormat == GUID_ContainerFormatJpeg ||
            containerFormat == GUID_ContainerFormatWmp;

        if (supportsQuality)
        {
            PROPBAG2 option{};
            option.pstrName = L"ImageQuality";
            VARIANT value{};
            value.vt = VT_R4;
            value.fltVal = quality;
            ThrowIfFailed(frameProperties->Write(1, &option, &value));
        }
        
        ThrowIfFailed(frame->Initialize(frameProperties.Get()));

        ComPtr<IWICImageEncoder> imageEncoder;
        ThrowIfFailed(factory->CreateImageEncoder(device, &imageEncoder));

        ThrowIfFailed(imageEncoder->WriteFrame(image, frame.Get(), &parameters));

        ThrowIfFailed(frame->Commit());
        ThrowIfFailed(encoder->Commit());
    }


    ComPtr<IWICImagingFactory2> const& DefaultCanvasImageAdapter::GetFactory()
    {
        if (!m_wicAdapter)
        {
            // Hold on to the wic adapter so it doesn't keep getting destroyed
            // and recreated.
            m_wicAdapter = WicAdapter::GetInstance();
        }

        return m_wicAdapter->GetFactory();
    }
       
        
}}}}
