// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

#include <lib/effects/generated/GaussianBlurEffect.h>

#include "../mocks/MockPropertyBag.h"
#include "../mocks/MockRandomAccessStream.h"
#include "../mocks/MockStream.h"
#include "../mocks/MockWICBitmapEncoder.h"
#include "../mocks/MockWICBitmapFrameEncode.h"
#include "../mocks/MockWICFactory.h"
#include "../mocks/MockWICImageEncoder.h"

TEST_CLASS(CanvasImageUnitTests)
{
    struct Fixture
    {
        ComPtr<StubCanvasDevice> m_canvasDevice;

        Fixture()
        {
            m_canvasDevice = Make<StubCanvasDevice>();
            
            m_canvasDevice->CreateRenderTargetBitmapMethod.AllowAnyCall(
                [&](float, float, float, DirectXPixelFormat, CanvasAlphaMode)
                {
                    return Make<StubD2DBitmap>(D2D1_BITMAP_OPTIONS_TARGET);
                });
        }
    };

    TEST_METHOD_EX(CanvasImage_GetBounds_CorrectContext)
    {
        Fixture f;

        ABI::Windows::Foundation::Rect bounds;

        D2D1_MATRIX_3X2_F someTransform = D2D1::Matrix3x2F(1, 2, 3, 4, 5, 6);
        D2D1_MATRIX_3X2_F currentTransform = someTransform;

        auto d2dDeviceContext = Make<MockD2DDeviceContext>();

        d2dDeviceContext->GetDeviceMethod.AllowAnyCallAlwaysCopyValueToParam(Make<StubD2DDevice>());

        d2dDeviceContext->GetTransformMethod.AllowAnyCall(
            [&](D2D1_MATRIX_3X2_F* matrix)
            {
                *matrix = currentTransform;
            });        

        d2dDeviceContext->SetTransformMethod.AllowAnyCall(
            [&](const D2D1_MATRIX_3X2_F* matrix)
            {
                currentTransform = *matrix;
            });

        d2dDeviceContext->GetImageWorldBoundsMethod.SetExpectedCalls(1,
            [&](ID2D1Image* image, D2D1_RECT_F* bounds)
            {
                return S_OK;
            });

        auto drawingSession = Make<CanvasDrawingSession>(d2dDeviceContext.Get());

        auto canvasBitmap = CanvasRenderTarget::CreateNew(
            f.m_canvasDevice.Get(),
            1.0f,
            1.0f,
            DEFAULT_DPI,
            PIXEL_FORMAT(B8G8R8A8UIntNormalized),
            CanvasAlphaMode::Premultiplied);
        canvasBitmap->GetBounds(drawingSession.Get(), &bounds);

        Assert::AreEqual(someTransform, currentTransform); 
    }
};


class CanvasImageTestAdapter : public CanvasImageAdapter
{
public:
    virtual ComPtr<IAsyncAction> RunAsync(std::function<void()>&& fn) override
    {
        HRESULT hr = ExceptionBoundary([&] { fn(); });
        auto action = Make<MockAsyncAction>();
        action->SetResult(hr);
        return action;
    }

    CALL_COUNTER_WITH_MOCK(CreateStreamOverRandomAccessStreamMethod, ComPtr<IStream>(IRandomAccessStream*));
        
    virtual ComPtr<IStream> CreateStreamOverRandomAccessStream(IRandomAccessStream* stream) override
    {
        return CreateStreamOverRandomAccessStreamMethod.WasCalled(stream);
    }

    CALL_COUNTER_WITH_MOCK(SaveImageMethod, void(ID2D1Image*, WICImageParameters const&, ID2D1Device*, IStream*, GUID const&, float));
    
    virtual void SaveImage(
        ID2D1Image* d2dImage,
        WICImageParameters const& wicImageParameters,
        ID2D1Device* device,
        IStream* stream,
        GUID const& containerFormat,
        float quality) override
    {
        return SaveImageMethod.WasCalled(d2dImage, wicImageParameters, device, stream, containerFormat, quality);
    }
};


TEST_CLASS(CanvasImageSaveUnitTests)
{
    struct Fixture
    {
        std::shared_ptr<CanvasImageTestAdapter> Adapter;
        ComPtr<CanvasImageFactory> CanvasImage;
        ComPtr<MockD2DDevice> D2DDevice;
        ComPtr<CanvasDevice> Device;

        Fixture()
        {
            Adapter = std::make_shared<CanvasImageTestAdapter>();
            CanvasImageAdapter::SetInstance(Adapter);

            CanvasImage = Make<CanvasImageFactory>();

            D2DDevice = Make<MockD2DDevice>();
            auto deviceContext = Make<StubD2DDeviceContext>(D2DDevice.Get());

            D2DDevice->MockCreateDeviceContext =
                [=] (D2D1_DEVICE_CONTEXT_OPTIONS, ID2D1DeviceContext1** dc)
                {
                    ThrowIfFailed(deviceContext.CopyTo(dc));
                };

            deviceContext->CreateEffectMethod.AllowAnyCall(
                [] (IID const& iid, ID2D1Effect** effect)
                {
                    return Make<StubD2DEffect>(iid).CopyTo(effect);
                });

            auto d3dDevice = Make<StubD3D11Device>();
        
            Device = Make<CanvasDevice>(D2DDevice.Get(), d3dDevice.Get());
        }        
    };


    struct InvalidParamsFixture : public Fixture
    {
        // These pointers are never expected to be dereferenced when the params
        // are invalid
        ICanvasImage* AnyImage;
        ICanvasResourceCreator* AnyResourceCreator;
        IRandomAccessStream* AnyRandomAccessStream;

        Rect AnyRect;
        float AnyDpi;
        CanvasBitmapFileFormat AnyFormat;
        float AnyQuality;
        CanvasBufferPrecision AnyPrecision;

        InvalidParamsFixture()
            : AnyImage(reinterpret_cast<ICanvasImage*>(1))
            , AnyResourceCreator(reinterpret_cast<ICanvasResourceCreator*>(2))
            , AnyRandomAccessStream(reinterpret_cast<IRandomAccessStream*>(3))
            , AnyRect{}
            , AnyDpi(DEFAULT_DPI)
            , AnyFormat(CanvasBitmapFileFormat::Jpeg)
            , AnyQuality(DEFAULT_CANVASBITMAP_QUALITY)
            , AnyPrecision{}
        {
        }
    };

    TEST_METHOD_EX(CanvasImage_SaveAsync_FailsWhenPassedNullParameters)
    {
        InvalidParamsFixture f;

        ComPtr<IAsyncAction> action;

        Assert::AreEqual(E_INVALIDARG, f.CanvasImage->SaveAsync(nullptr,    f.AnyRect, f.AnyDpi, f.AnyResourceCreator, f.AnyRandomAccessStream,  f.AnyFormat, &action));
        Assert::AreEqual(E_INVALIDARG, f.CanvasImage->SaveAsync(f.AnyImage, f.AnyRect, f.AnyDpi, nullptr,              f.AnyRandomAccessStream,  f.AnyFormat, &action));
        Assert::AreEqual(E_INVALIDARG, f.CanvasImage->SaveAsync(f.AnyImage, f.AnyRect, f.AnyDpi, f.AnyResourceCreator, nullptr,                  f.AnyFormat, &action));
        Assert::AreEqual(E_INVALIDARG, f.CanvasImage->SaveAsync(f.AnyImage, f.AnyRect, f.AnyDpi, f.AnyResourceCreator, f.AnyRandomAccessStream,  f.AnyFormat, nullptr));

        Assert::AreEqual(E_INVALIDARG, f.CanvasImage->SaveWithQualityAsync(nullptr,    f.AnyRect, f.AnyDpi, f.AnyResourceCreator, f.AnyRandomAccessStream,  f.AnyFormat, f.AnyQuality, &action));
        Assert::AreEqual(E_INVALIDARG, f.CanvasImage->SaveWithQualityAsync(f.AnyImage, f.AnyRect, f.AnyDpi, nullptr,              f.AnyRandomAccessStream,  f.AnyFormat, f.AnyQuality, &action));
        Assert::AreEqual(E_INVALIDARG, f.CanvasImage->SaveWithQualityAsync(f.AnyImage, f.AnyRect, f.AnyDpi, f.AnyResourceCreator, nullptr,                  f.AnyFormat, f.AnyQuality, &action));
        Assert::AreEqual(E_INVALIDARG, f.CanvasImage->SaveWithQualityAsync(f.AnyImage, f.AnyRect, f.AnyDpi, f.AnyResourceCreator, f.AnyRandomAccessStream,  f.AnyFormat, f.AnyQuality, nullptr));
        
        Assert::AreEqual(E_INVALIDARG, f.CanvasImage->SaveWithQualityAndBufferPrecisionAsync(nullptr,    f.AnyRect, f.AnyDpi, f.AnyResourceCreator, f.AnyRandomAccessStream,  f.AnyFormat, f.AnyQuality, f.AnyPrecision, &action));
        Assert::AreEqual(E_INVALIDARG, f.CanvasImage->SaveWithQualityAndBufferPrecisionAsync(f.AnyImage, f.AnyRect, f.AnyDpi, nullptr,              f.AnyRandomAccessStream,  f.AnyFormat, f.AnyQuality, f.AnyPrecision, &action));
        Assert::AreEqual(E_INVALIDARG, f.CanvasImage->SaveWithQualityAndBufferPrecisionAsync(f.AnyImage, f.AnyRect, f.AnyDpi, f.AnyResourceCreator, nullptr,                  f.AnyFormat, f.AnyQuality, f.AnyPrecision, &action));
        Assert::AreEqual(E_INVALIDARG, f.CanvasImage->SaveWithQualityAndBufferPrecisionAsync(f.AnyImage, f.AnyRect, f.AnyDpi, f.AnyResourceCreator, f.AnyRandomAccessStream,  f.AnyFormat, f.AnyQuality, f.AnyPrecision, nullptr));
    }

    TEST_METHOD_EX(CanvasImage_SaveAsync_FailsWhenPassedAutoFileFormat)
    {
        InvalidParamsFixture f;

        ComPtr<IAsyncAction> action;

        Assert::AreEqual(E_INVALIDARG, f.CanvasImage->SaveAsync(f.AnyImage, f.AnyRect, f.AnyDpi, f.AnyResourceCreator, f.AnyRandomAccessStream, CanvasBitmapFileFormat::Auto, &action));
        ValidateStoredErrorState(E_INVALIDARG, Strings::AutoFileFormatNotAllowed);

        Assert::AreEqual(E_INVALIDARG, f.CanvasImage->SaveWithQualityAsync(f.AnyImage, f.AnyRect, f.AnyDpi, f.AnyResourceCreator, f.AnyRandomAccessStream, CanvasBitmapFileFormat::Auto, f.AnyQuality, &action));
        ValidateStoredErrorState(E_INVALIDARG, Strings::AutoFileFormatNotAllowed);

        Assert::AreEqual(E_INVALIDARG, f.CanvasImage->SaveWithQualityAndBufferPrecisionAsync(f.AnyImage, f.AnyRect, f.AnyDpi, f.AnyResourceCreator, f.AnyRandomAccessStream, CanvasBitmapFileFormat::Auto, f.AnyQuality, f.AnyPrecision, &action));
        ValidateStoredErrorState(E_INVALIDARG, Strings::AutoFileFormatNotAllowed);
    }

    TEST_METHOD_EX(CanvasImage_SaveAsync_FailsWhenPassedInvalidQuality)
    {
        InvalidParamsFixture f;

        ComPtr<IAsyncAction> action;

        for (auto invalidQuality : { -1.0f, 1.1f })
        {
            Assert::AreEqual(E_INVALIDARG, f.CanvasImage->SaveWithQualityAsync(f.AnyImage, f.AnyRect, f.AnyDpi, f.AnyResourceCreator, f.AnyRandomAccessStream, f.AnyFormat, invalidQuality, &action));
            Assert::AreEqual(E_INVALIDARG, f.CanvasImage->SaveWithQualityAndBufferPrecisionAsync(f.AnyImage, f.AnyRect, f.AnyDpi, f.AnyResourceCreator, f.AnyRandomAccessStream, f.AnyFormat, invalidQuality, f.AnyPrecision, &action));
        }
    }

    
    struct ImageFixture : public Fixture
    {
        ComPtr<ICanvasImage> AnyCanvasImage;
        ComPtr<ID2D1Image> D2DImage;
        ComPtr<IRandomAccessStream> RandomAccessStream;
        ComPtr<IStream> Stream;

        ImageFixture()
        {
            AnyCanvasImage = Make<Effects::GaussianBlurEffect>();
            D2DImage = GetWrappedResource<ID2D1Image>(AnyCanvasImage, Device.Get(), DEFAULT_DPI);
            RandomAccessStream = Make<MockRandomAccessStream>();
            Stream = Make<MockStream>();

            Adapter->CreateStreamOverRandomAccessStreamMethod.SetExpectedCalls(1,
                [&] (IRandomAccessStream* r)
                {
                    Assert::IsTrue(IsSameInstance(RandomAccessStream.Get(), r));
                    return Stream;
                });            
        }
    };
       

    TEST_METHOD_EX(CanvasImage_SaveAsync_PassesThroughCorrectDefaults)
    {
        ImageFixture f;

        f.Adapter->SaveImageMethod.SetExpectedCalls(1,
            [&] (ID2D1Image* image, WICImageParameters const& params, ID2D1Device* device, IStream* s, GUID const& formatGuid, float quality)
            {
                Assert::IsTrue(IsSameInstance(f.D2DImage.Get(), image), L"Image");
                Assert::IsTrue(IsSameInstance(f.D2DDevice.Get(), device), L"Device");
                Assert::IsTrue(IsSameInstance(f.Stream.Get(), s), L"Stream");
                Assert::AreEqual(GUID_ContainerFormatJpeg, formatGuid);
                Assert::AreEqual(DEFAULT_CANVASBITMAP_QUALITY, quality);

                Assert::AreEqual(DXGI_FORMAT_B8G8R8A8_UNORM, params.PixelFormat.format);
                Assert::AreEqual(D2D1_ALPHA_MODE_PREMULTIPLIED, params.PixelFormat.alphaMode);
                Assert::AreEqual(DEFAULT_DPI, params.DpiX);
                Assert::AreEqual(DEFAULT_DPI, params.DpiY);
                Assert::AreEqual(1.0f, params.Left);
                Assert::AreEqual(2.0f, params.Top);
                Assert::AreEqual(3u, params.PixelWidth);
                Assert::AreEqual(4u, params.PixelHeight);
            });
        
        ComPtr<IAsyncAction> action;
        ThrowIfFailed(f.CanvasImage->SaveAsync(
            f.AnyCanvasImage.Get(),
            Rect{1, 2, 3, 4},
            DEFAULT_DPI,
            f.Device.Get(),
            f.RandomAccessStream.Get(),
            CanvasBitmapFileFormat::Jpeg,
            &action));
        
        ValidateActionSucceeded(action);
    }

    TEST_METHOD_EX(CanvasImage_SaveAsync_ConvertsRectToPixels)
    {
        ImageFixture f;

        float dpi = DEFAULT_DPI * 1.5f;

        f.Adapter->SaveImageMethod.SetExpectedCalls(1,
            [&] (ID2D1Image*, WICImageParameters const& params, ID2D1Device*, IStream*, GUID const&, float)
            {
                Assert::AreEqual(dpi, params.DpiX);
                Assert::AreEqual(dpi, params.DpiY);
                Assert::AreEqual(1.5f, params.Left);
                Assert::AreEqual(3.0f, params.Top);
                Assert::AreEqual(5u, params.PixelWidth);
                Assert::AreEqual(6u, params.PixelHeight);
            });
        
        ComPtr<IAsyncAction> action;
        ThrowIfFailed(f.CanvasImage->SaveAsync(
            f.AnyCanvasImage.Get(),
            Rect{1, 2, 3, 4},
            dpi,
            f.Device.Get(),
            f.RandomAccessStream.Get(),
            CanvasBitmapFileFormat::Jpeg,
            &action));

        ValidateActionSucceeded(action);
    }

    TEST_METHOD_EX(CanvasImage_SaveAsync_BufferPrecision)
    {
        std::pair<CanvasBufferPrecision, DXGI_FORMAT> precisionAndFormats[] =
        {
            { CanvasBufferPrecision::Precision8UIntNormalized     , DXGI_FORMAT_B8G8R8A8_UNORM },
            { CanvasBufferPrecision::Precision8UIntNormalizedSrgb , DXGI_FORMAT_B8G8R8A8_UNORM_SRGB },
            { CanvasBufferPrecision::Precision16UIntNormalized    , DXGI_FORMAT_R16G16B16A16_UNORM },
            { CanvasBufferPrecision::Precision16Float             , DXGI_FORMAT_R16G16B16A16_FLOAT },
            { CanvasBufferPrecision::Precision32Float             , DXGI_FORMAT_R32G32B32A32_FLOAT },
        };

        for (auto precisionAndFormat : precisionAndFormats)
        {
            auto precision = precisionAndFormat.first;
            auto format    = precisionAndFormat.second;
            
            ImageFixture f;

            f.Adapter->SaveImageMethod.SetExpectedCalls(1,
                [&] (ID2D1Image*, WICImageParameters const& params, ID2D1Device*, IStream*, GUID const&, float)
                {
                    Assert::AreEqual(params.PixelFormat.format, format);
                    Assert::AreEqual(params.PixelFormat.alphaMode, D2D1_ALPHA_MODE_PREMULTIPLIED);
                });

            ComPtr<IAsyncAction> action;
            float anyQuality = 1;
            
            ThrowIfFailed(f.CanvasImage->SaveWithQualityAndBufferPrecisionAsync(
                f.AnyCanvasImage.Get(),
                Rect{1, 2, 3, 4},
                DEFAULT_DPI,
                f.Device.Get(),
                f.RandomAccessStream.Get(),
                CanvasBitmapFileFormat::Jpeg,
                anyQuality,
                precision,
                &action));

            ValidateActionSucceeded(action);
        }
    }

    TEST_METHOD_EX(CanvasImage_SaveAsync_FileFormats)
    {
        std::pair<CanvasBitmapFileFormat, GUID> bitmapFileFormatAndGuids[] =
        {
            { CanvasBitmapFileFormat::Bmp    , GUID_ContainerFormatBmp },
            { CanvasBitmapFileFormat::Png    , GUID_ContainerFormatPng },
            { CanvasBitmapFileFormat::Jpeg   , GUID_ContainerFormatJpeg },
            { CanvasBitmapFileFormat::Tiff   , GUID_ContainerFormatTiff },
            { CanvasBitmapFileFormat::Gif    , GUID_ContainerFormatGif },
            { CanvasBitmapFileFormat::JpegXR , GUID_ContainerFormatWmp },
        };

        for (auto bitmapFileFormatAndGuid : bitmapFileFormatAndGuids)
        {
            auto bitmapFileFormat = bitmapFileFormatAndGuid.first;
            auto guid = bitmapFileFormatAndGuid.second;

            ImageFixture f;
            
            f.Adapter->SaveImageMethod.SetExpectedCalls(1,
                [&] (ID2D1Image*, WICImageParameters const&, ID2D1Device*, IStream*, GUID const& g, float)
                {
                    Assert::AreEqual(guid, g);
                });
            
            ComPtr<IAsyncAction> action;
            
            ThrowIfFailed(f.CanvasImage->SaveAsync(
                f.AnyCanvasImage.Get(),
                Rect{1, 2, 3, 4},
                DEFAULT_DPI,
                f.Device.Get(),
                f.RandomAccessStream.Get(),
                bitmapFileFormat,
                &action));

            ValidateActionSucceeded(action);            
        }
    }

    void ValidateActionSucceeded(ComPtr<IAsyncAction> action)
    {
        HRESULT errorCode;
        ThrowIfFailed(As<IAsyncInfo>(action)->get_ErrorCode(&errorCode));
        Assert::AreEqual(S_OK, errorCode);
    }    
};

class WicTestAdapter : public WicAdapter
{
    ComPtr<IWICImagingFactory2> m_factory;

public:
    ComPtr<MockWICImagingFactory> WICFactory;
    

    WicTestAdapter()
        : WICFactory(Make<MockWICImagingFactory>())
    {
        m_factory = As<IWICImagingFactory2>(WICFactory);
    }
    
    virtual ComPtr<IWICImagingFactory2> const& GetFactory() override
    {
        return m_factory;
    }
};

TEST_CLASS(DefaultCanvasImageAdapter_Tests)
{
    struct Fixture
    {
        std::shared_ptr<WicTestAdapter> Adapter;

        ID2D1Image* AnyD2DImage;
        ID2D1Device* AnyD2DDevice;
        IStream* AnyStream;
        WICImageParameters AnyParameters;
        float AnyQuality;
        
        Fixture()
            : Adapter(std::make_shared<WicTestAdapter>())
            , AnyD2DImage(reinterpret_cast<ID2D1Image*>(1))
            , AnyD2DDevice(reinterpret_cast<ID2D1Device*>(2))
            , AnyStream(reinterpret_cast<IStream*>(3))
            , AnyQuality(0.4f)
        {
            WicTestAdapter::SetInstance(Adapter);

            AnyParameters = WICImageParameters
                {
                    { (DXGI_FORMAT)4, (D2D1_ALPHA_MODE)5 }, // pixel format
                    6, 7,                                   // dpix, dpiy
                    8, 9,                                   // top, left
                    10, 11                                  // width, height
                };            
        }

        void Expect(GUID containerFormat, bool qualitySupported)
        {
            auto encoder = Make<MockWICBitmapEncoder>();
            auto frame = Make<MockWICBitmapFrameEncode>();
            auto frameProperties = Make<MockPropertyBag>();
            auto imageEncoder = Make<MockWICImageEncoder>();

            Adapter->WICFactory->CreateEncoderMethod.SetExpectedCalls(1,
                [=] (const GUID& cf, const GUID* vendor, IWICBitmapEncoder** value)
                {
                    Assert::AreEqual(containerFormat, cf);
                    Assert::IsNull(vendor);
                    return encoder.CopyTo(value);
                });

            encoder->InitializeMethod.SetExpectedCalls(1,
                [=] (IStream* stream, WICBitmapEncoderCacheOption cacheOption)
                {
                    Assert::AreEqual<void*>(AnyStream, stream);
                    Assert::AreEqual<uint32_t>(WICBitmapEncoderNoCache, cacheOption);

                    // CreateNewFrame must be called after Initialize
                    encoder->CreateNewFrameMethod.SetExpectedCalls(1,
                        [&] (IWICBitmapFrameEncode** f, IPropertyBag2** b)
                        {
                            frameProperties.CopyTo(b);
                            return frame.CopyTo(f);
                        });
                
                    return S_OK;
                });

            frame->InitializeMethod.SetExpectedCalls(1,
                [=] (IPropertyBag2* p)
                {
                    Assert::IsTrue(IsSameInstance(frameProperties.Get(), p));
                    return S_OK;
                });

            if (qualitySupported)
            {
                frameProperties->WriteMethod.SetExpectedCalls(1,
                    [&] (ULONG count, PROPBAG2* p, VARIANT* v)
                    {
                        Assert::AreEqual(1ul, count);
                        Assert::AreEqual<std::wstring>(L"ImageQuality", p->pstrName);
                        Assert::AreEqual<uint32_t>(VT_R4, v->vt);
                        Assert::AreEqual(AnyQuality, v->fltVal);
                        return S_OK;
                    });
            }

            Adapter->WICFactory->CreateImageEncoderMethod.SetExpectedCalls(1,
                [=] (ID2D1Device* d, IWICImageEncoder** e)
                {
                    Assert::AreEqual<void*>(AnyD2DDevice, d);
                    return imageEncoder.CopyTo(e);
                });

            imageEncoder->WriteFrameMethod.SetExpectedCalls(1,
                [=] (ID2D1Image* i, IWICBitmapFrameEncode* e, WICImageParameters const* p)
                {
                    Assert::AreEqual<void*>(AnyD2DImage, i);
                    Assert::AreEqual<void*>(frame.Get(), e);
                    Assert::AreEqual(0, memcmp(p, &AnyParameters, sizeof(AnyParameters)));

                    frame->CommitMethod.SetExpectedCalls(1,
                        [&]
                        {
                            encoder->CommitMethod.SetExpectedCalls(1);
                            return S_OK;
                        });
                
                    return S_OK;
                });
        }
    };
    
    TEST_METHOD_EX(CanvasImage_Adapter_SaveImage_WhenFormatSupportQualityItIsSet)
    {
        std::pair<GUID, bool> formatAndQualitySupported[] =
        {
            { GUID_ContainerFormatBmp  , false },
            { GUID_ContainerFormatPng  , false },
            { GUID_ContainerFormatJpeg , true },
            { GUID_ContainerFormatTiff , false },
            { GUID_ContainerFormatGif  , false },
            { GUID_ContainerFormatWmp  , true }
        };

        for (auto v : formatAndQualitySupported)
        {
            auto containerFormat = v.first;
            auto qualitySupported = v.second;

            Fixture f;

            f.Expect(containerFormat, qualitySupported);

            CanvasImageAdapter::GetInstance()->SaveImage(
                f.AnyD2DImage,
                f.AnyParameters,
                f.AnyD2DDevice,
                f.AnyStream,
                containerFormat,
                f.AnyQuality);
        }
    }
};
