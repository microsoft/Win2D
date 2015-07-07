// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace ABI::Windows::Foundation;

TEST_CLASS(CanvasRenderTargetTests)
{
    struct Fixture
    {
        std::shared_ptr<TestBitmapResourceCreationAdapter> m_adapter;
        std::shared_ptr<CanvasRenderTargetManager> m_manager;
        ComPtr<MockD2DDevice> m_d2dDevice;
        ComPtr<StubCanvasDevice> m_canvasDevice;
        
        Fixture(float expectedDpi = DEFAULT_DPI)
        {
            auto converter = Make<MockWICFormatConverter>();
            auto adapter = std::make_shared<TestBitmapResourceCreationAdapter>(converter);
            m_manager = std::make_shared<CanvasRenderTargetManager>(adapter);
            m_d2dDevice = Make<MockD2DDevice>();
            m_canvasDevice = Make<StubCanvasDevice>(m_d2dDevice);

            // Make the D2D device return StubD2DDeviceContexts
            m_d2dDevice->MockCreateDeviceContext = 
                [&, expectedDpi](D2D1_DEVICE_CONTEXT_OPTIONS, ID2D1DeviceContext1** value)
                {
                    auto deviceContext = Make<StubD2DDeviceContext>(m_d2dDevice.Get());

                    deviceContext->SetDpiMethod.SetExpectedCalls(1, [=](float dpiX, float dpiY)
                    {
                        Assert::AreEqual(expectedDpi, dpiX);
                        Assert::AreEqual(expectedDpi, dpiY);
                    });

                    deviceContext->SetTextAntialiasModeMethod.SetExpectedCalls(1, [](D2D1_TEXT_ANTIALIAS_MODE mode)
                    {
                        Assert::AreEqual(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE, mode);
                    });

                    ThrowIfFailed(deviceContext.CopyTo(value));
                };
        }

        ComPtr<CanvasRenderTarget> CreateRenderTarget(
            ComPtr<ID2D1Bitmap1> bitmap = nullptr,
            Size size = Size{ 1, 1 })
        {
            if (!bitmap)
                bitmap = Make<StubD2DBitmap>(D2D1_BITMAP_OPTIONS_TARGET);

            m_canvasDevice->MockCreateRenderTargetBitmap =
                [&](float, float, DirectXPixelFormat, CanvasAlphaMode, float)
                {
                    Assert::IsNotNull(bitmap.Get());
                    auto result = bitmap;
                    bitmap.Reset();
                    return result;
                };

            auto renderTarget = m_manager->Create(
                m_canvasDevice.Get(), 
                size.Width, 
                size.Height, 
                DEFAULT_DPI,
                PIXEL_FORMAT(B8G8R8A8UIntNormalized),
                CanvasAlphaMode::Premultiplied);

            m_canvasDevice->MockCreateRenderTargetBitmap = nullptr;

            return renderTarget;
        }

        ComPtr<CanvasRenderTarget> CreateRenderTargetAsWrapper(ICanvasDevice* device, ComPtr<ID2D1Bitmap1> bitmap)
        {
            return m_manager->GetOrCreate(device, bitmap.Get());
        }
    };

    TEST_METHOD_EX(CanvasRenderTarget_Implements_Expected_Interfaces)
    {
        Fixture f;
        auto renderTarget = f.CreateRenderTarget();

        ASSERT_IMPLEMENTS_INTERFACE(renderTarget, ICanvasRenderTarget);
        ASSERT_IMPLEMENTS_INTERFACE(renderTarget, ICanvasBitmap);
        ASSERT_IMPLEMENTS_INTERFACE(renderTarget, ICanvasImage);
        ASSERT_IMPLEMENTS_INTERFACE(renderTarget, ABI::Windows::Foundation::IClosable);
        ASSERT_IMPLEMENTS_INTERFACE(renderTarget, ICanvasImageInternal);
    }

    TEST_METHOD_EX(CanvasRenderTarget_InterfacesAreTransitive)
    {
        Fixture f;
        auto renderTarget = f.CreateRenderTarget();

        ComPtr<ICanvasBitmap> bitmap;
        ThrowIfFailed(renderTarget.As(&bitmap));

        ComPtr<ICanvasRenderTarget> irenderTarget;
        Assert::AreEqual(S_OK, bitmap.As(&irenderTarget));
    }

    TEST_METHOD_EX(CanvasRenderTarget_Close)
    {
        Fixture f;
        auto renderTarget = f.CreateRenderTarget();

        ComPtr<IClosable> renderTargetClosable;
        ThrowIfFailed(renderTarget.As(&renderTargetClosable));

        Assert::AreEqual(S_OK, renderTargetClosable->Close());

        ComPtr<ICanvasDrawingSession> drawingSession;
        Assert::AreEqual(RO_E_CLOSED, renderTarget->CreateDrawingSession(&drawingSession));
    }

    TEST_METHOD_EX(CanvasRenderTarget_DrawingSession)
    {        
        Fixture f;
        auto d2dBitmap = Make<StubD2DBitmap>(D2D1_BITMAP_OPTIONS_TARGET);    
        auto renderTarget = f.CreateRenderTarget(d2dBitmap);

        ComPtr<ICanvasDrawingSession> drawingSession;
        ThrowIfFailed(renderTarget->CreateDrawingSession(&drawingSession));

        ValidateDrawingSession(drawingSession.Get(), f.m_d2dDevice.Get(), d2dBitmap.Get());
    }

    TEST_METHOD_EX(CanvasRenderTarget_PropagatesDpiToDrawingSession)
    {
        const float expectedDpi = 123;
        
        Fixture f(expectedDpi);
        auto d2dBitmap = Make<StubD2DBitmap>(D2D1_BITMAP_OPTIONS_TARGET, expectedDpi);
        auto renderTarget = f.CreateRenderTarget(d2dBitmap);

        ComPtr<ICanvasDrawingSession> drawingSession;
        ThrowIfFailed(renderTarget->CreateDrawingSession(&drawingSession));
    }

    TEST_METHOD_EX(CanvasRenderTarget_Wrapped_CreatesDrawingSession)
    {
        Fixture f;
        auto d2dBitmap = Make<StubD2DBitmap>(D2D1_BITMAP_OPTIONS_TARGET);
        auto renderTarget = f.CreateRenderTargetAsWrapper(f.m_canvasDevice.Get(), d2dBitmap);

        ComPtr<ICanvasDrawingSession> drawingSession;
        ThrowIfFailed(renderTarget->CreateDrawingSession(&drawingSession));

        ValidateDrawingSession(drawingSession.Get(), f.m_d2dDevice.Get(), d2dBitmap.Get());
    }

    TEST_METHOD_EX(CanvasRenderTarget_Wrapped_WrongDevice_Fails)
    {
        Fixture f;
        auto d2dBitmap = Make<StubD2DBitmap>(D2D1_BITMAP_OPTIONS_TARGET);
        auto renderTarget = f.CreateRenderTarget(d2dBitmap);

        auto otherCanvasDevice = Make<StubCanvasDevice>();

        ExpectHResultException(E_INVALIDARG, [&]{ f.CreateRenderTargetAsWrapper(otherCanvasDevice.Get(), d2dBitmap.Get()); });
    }

    //
    // Validates the drawing session.  We do this using the underlying D2D
    // resources.
    //
    void ValidateDrawingSession(
        ICanvasDrawingSession* drawingSession,
        ID2D1Device* expectedDevice,
        ID2D1Image* expectedTarget)
    {
        //
        // Pull the ID2D1DeviceContext1 out of the drawing session
        //
        auto deviceContext = GetWrappedResource<ID2D1DeviceContext1>(drawingSession);

        //
        // Check the device
        //
        ComPtr<ID2D1Device> deviceContextDevice;
        deviceContext->GetDevice(&deviceContextDevice);

        Assert::AreEqual(expectedDevice, deviceContextDevice.Get());

        //
        // Check the currently set target
        //
        ComPtr<ID2D1Image> deviceContextTarget;
        deviceContext->GetTarget(&deviceContextTarget);

        Assert::AreEqual(expectedTarget, deviceContextTarget.Get());
    }


    TEST_METHOD_EX(CanvasRenderTarget_InheritanceFromBitmap)
    {
        Fixture f;

        // This exercises some of the Bitmap methods on CanvasRenderTarget.

        Size expectedSize = { 33, 44 };

        auto d2dBitmap = Make<StubD2DBitmap>(D2D1_BITMAP_OPTIONS_TARGET);
        d2dBitmap->GetSizeMethod.AllowAnyCall(
            [&] { return D2D1_SIZE_F{expectedSize.Width, expectedSize.Height}; });

        d2dBitmap->GetPixelSizeMethod.AllowAnyCall(
            [&] { return D2D_SIZE_U{ (unsigned)expectedSize.Width, (unsigned)expectedSize.Height }; });

        auto renderTarget = f.CreateRenderTarget(d2dBitmap, expectedSize);

        ComPtr<ICanvasBitmap> renderTargetAsBitmap;
        ThrowIfFailed(renderTarget.As(&renderTargetAsBitmap));

        Size retrievedSize;
        BitmapSize retrievedBitmapSize;

        ThrowIfFailed(renderTargetAsBitmap->get_Size(&retrievedSize));
        Assert::AreEqual(expectedSize.Width, retrievedSize.Width);
        Assert::AreEqual(expectedSize.Height, retrievedSize.Height);

        // Bitmaps are constructed against default DPI, currently, so the pixel 
        // size and dips size should be equal.
        ThrowIfFailed(renderTargetAsBitmap->get_SizeInPixels(&retrievedBitmapSize));
        Assert::AreEqual(static_cast<uint32_t>(expectedSize.Width), retrievedBitmapSize.Width);
        Assert::AreEqual(static_cast<uint32_t>(expectedSize.Height), retrievedBitmapSize.Height);
    }
};
