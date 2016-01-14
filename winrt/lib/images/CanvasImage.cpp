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


    IFACEMETHODIMP CanvasImageFactory::ComputeHistogram(
        ICanvasImage* image,
        Rect sourceRectangle,
        ICanvasResourceCreator* resourceCreator,
        Effects::EffectChannelSelect channelSelect,
        int32_t numberOfBins,
        uint32_t* valueCount,
        float** valueElements)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(image);
                CheckInPointer(resourceCreator);
                CheckInPointer(valueCount);
                CheckAndClearOutPointer(valueElements);

                if (numberOfBins < 2 || numberOfBins > 1024)
                    ThrowHR(E_INVALIDARG);

                // Look up a device context.
                ComPtr<ICanvasDevice> device;
                ThrowIfFailed(resourceCreator->get_Device(&device));
                
                auto deviceInternal = As<ICanvasDeviceInternal>(device);

                auto deviceContext = deviceInternal->GetResourceCreationDeviceContext();
                
                // Look up a histogram effect.
                ComPtr<ID2D1Effect> histogram = deviceInternal->LeaseHistogramEffect(deviceContext.Get());

                auto releaseHistogram = MakeScopeWarden(
                    [&]
                    {
                        histogram->SetInput(0, nullptr);
                        deviceInternal->ReleaseHistogramEffect(std::move(histogram));
                    });

                // Configure the histogram effect.
                float realizedDpi;

                auto d2dImage = As<ICanvasImageInternal>(image)->GetD2DImage(device.Get(), deviceContext.Get(), GetImageFlags::None, DEFAULT_DPI, &realizedDpi);

                if (realizedDpi != 0 && realizedDpi != DEFAULT_DPI)
                {
                    ThrowIfFailed(D2D1::SetDpiCompensatedEffectInput(deviceContext.Get(), histogram.Get(), 0, As<ID2D1Bitmap>(d2dImage).Get()));
                }
                else
                {
                    histogram->SetInput(0, d2dImage.Get());
                }

                histogram->SetValue(D2D1_HISTOGRAM_PROP_CHANNEL_SELECT, channelSelect);
                histogram->SetValue(D2D1_HISTOGRAM_PROP_NUM_BINS, numberOfBins);

                // Evaluate the histogram by drawing the effect.
                deviceContext->BeginDraw();

                deviceContext->DrawImage(As<ID2D1Image>(histogram).Get(), D2D1_POINT_2F{ 0, 0 }, ToD2DRect(sourceRectangle));

                ThrowIfFailed(deviceContext->EndDraw());

                // Read back the results.
                ComArray<float> array(numberOfBins);

                ThrowIfFailed(histogram->GetValue(D2D1_HISTOGRAM_PROP_HISTOGRAM_OUTPUT,
                                                  reinterpret_cast<BYTE*>(array.GetData()),
                                                  array.GetSize() * sizeof(float)));

                array.Detach(valueCount, valueElements);
            });
    }


    IFACEMETHODIMP CanvasImageFactory::IsHistogramSupported(
        ICanvasDevice* device,
        boolean* result)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(device);
                CheckInPointer(result);

                ComPtr<ID3D11Device> d3dDevice;
                ThrowIfFailed(As<IDirect3DDxgiInterfaceAccess>(device)->GetInterface(IID_PPV_ARGS(&d3dDevice)));

                bool isSupported = false;

                if (d3dDevice->GetFeatureLevel() >= D3D_FEATURE_LEVEL_11_0)
                {
                    // Feature level 11 and above always supports compute shaders.
                    isSupported = true;
                }
                else
                {
                    // On feature level 10, compute shaders are optional.
                    D3D11_FEATURE_DATA_D3D10_X_HARDWARE_OPTIONS features = { 0 };
                    
                    if (SUCCEEDED(d3dDevice->CheckFeatureSupport(D3D11_FEATURE_D3D10_X_HARDWARE_OPTIONS, &features, sizeof(features))))
                    {
                        isSupported = !!features.ComputeShaders_Plus_RawAndStructuredBuffers_Via_Shader_4_x;
                    }
                }

                *result = isSupported;
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
