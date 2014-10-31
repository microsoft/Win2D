// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may
// not use these files except in compliance with the License. You may obtain
// a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
// License for the specific language governing permissions and limitations
// under the License.

#include "pch.h"

#include "CanvasRenderTarget.h"
#include "TestBitmapResourceCreationAdapter.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace ABI::Windows::Foundation;

TEST_CLASS(CanvasRenderTargetTests)
{
    std::shared_ptr<TestBitmapResourceCreationAdapter> m_adapter;
    std::shared_ptr<CanvasRenderTargetManager> m_manager;
    ComPtr<MockD2DDevice> m_d2dDevice;
    ComPtr<StubCanvasDevice> m_canvasDevice;

public:
    TEST_METHOD_INITIALIZE(Reset)
    {
        auto converter = Make<MockWICFormatConverter>();
        auto adapter = std::make_shared<TestBitmapResourceCreationAdapter>(converter);
        m_manager = std::make_shared<CanvasRenderTargetManager>(adapter);
        m_d2dDevice = Make<MockD2DDevice>();
        m_canvasDevice = Make<StubCanvasDevice>(m_d2dDevice);

        // Make the D2D device return StubD2DDeviceContexts
        m_d2dDevice->MockCreateDeviceContext = 
            [&](D2D1_DEVICE_CONTEXT_OPTIONS, ID2D1DeviceContext1** value)
            {
                auto deviceContext = Make<StubD2DDeviceContext>(m_d2dDevice.Get());
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
            [&](float, float, DirectXPixelFormat, CanvasAlphaBehavior, float)
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
            DirectXPixelFormat::B8G8R8A8UIntNormalized,
            CanvasAlphaBehavior::Premultiplied,
            DEFAULT_DPI);

        m_canvasDevice->MockCreateRenderTargetBitmap = nullptr;

        return renderTarget;
    }

    ComPtr<CanvasRenderTarget> CreateRenderTargetAsWrapper(ICanvasDevice* device, ComPtr<ID2D1Bitmap1> bitmap)
    {
        return m_manager->GetOrCreate(device, bitmap.Get());
    }

    TEST_METHOD(CanvasRenderTarget_Implements_Expected_Interfaces)
    {
        auto renderTarget = CreateRenderTarget();

        ASSERT_IMPLEMENTS_INTERFACE(renderTarget, ICanvasRenderTarget);
        ASSERT_IMPLEMENTS_INTERFACE(renderTarget, ICanvasBitmap);
        ASSERT_IMPLEMENTS_INTERFACE(renderTarget, ICanvasImage);
        ASSERT_IMPLEMENTS_INTERFACE(renderTarget, ABI::Windows::Foundation::IClosable);
        ASSERT_IMPLEMENTS_INTERFACE(renderTarget, ICanvasImageInternal);
    }

    TEST_METHOD(CanvasRenderTarget_InterfacesAreTransitive)
    {
        auto renderTarget = CreateRenderTarget();

        ComPtr<ICanvasBitmap> bitmap;
        ThrowIfFailed(renderTarget.As(&bitmap));

        ComPtr<ICanvasRenderTarget> irenderTarget;
        Assert::AreEqual(S_OK, bitmap.As(&irenderTarget));
    }

    TEST_METHOD(CanvasRenderTarget_Close)
    {
        auto renderTarget = CreateRenderTarget();

        ComPtr<IClosable> renderTargetClosable;
        ThrowIfFailed(renderTarget.As(&renderTargetClosable));

        Assert::AreEqual(S_OK, renderTargetClosable->Close());

        ComPtr<ICanvasDrawingSession> drawingSession;
        Assert::AreEqual(RO_E_CLOSED, renderTarget->CreateDrawingSession(&drawingSession));
    }


    TEST_METHOD(CanvasRenderTarget_DrawingSession)
    {        
        auto d2dBitmap = Make<StubD2DBitmap>(D2D1_BITMAP_OPTIONS_TARGET);    
        auto renderTarget = CreateRenderTarget(d2dBitmap);

        ComPtr<ICanvasDrawingSession> drawingSession;
        ThrowIfFailed(renderTarget->CreateDrawingSession(&drawingSession));

        ValidateDrawingSession(drawingSession.Get(), m_d2dDevice.Get(), d2dBitmap.Get());
    }

    TEST_METHOD(CanvasRenderTarget_Wrapped_CreatesDrawingSession)
    {
        auto d2dBitmap = Make<StubD2DBitmap>(D2D1_BITMAP_OPTIONS_TARGET);
        auto renderTarget = CreateRenderTargetAsWrapper(m_canvasDevice.Get(), d2dBitmap);

        ComPtr<ICanvasDrawingSession> drawingSession;
        ThrowIfFailed(renderTarget->CreateDrawingSession(&drawingSession));

        ValidateDrawingSession(drawingSession.Get(), m_d2dDevice.Get(), d2dBitmap.Get());
    }

    TEST_METHOD(CanvasRenderTarget_Wrapped_WrongDevice_Fails)
    {
        auto d2dBitmap = Make<StubD2DBitmap>(D2D1_BITMAP_OPTIONS_TARGET);
        auto renderTarget = CreateRenderTarget(d2dBitmap);

        auto otherCanvasDevice = Make<StubCanvasDevice>();

        ExpectHResultException(E_INVALIDARG, [&]{ CreateRenderTargetAsWrapper(otherCanvasDevice.Get(), d2dBitmap.Get()); });
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
        ComPtr<ICanvasResourceWrapperNative> drawingSessionResourceWrapper;
        ThrowIfFailed(drawingSession->QueryInterface(drawingSessionResourceWrapper.GetAddressOf()));

        ComPtr<ID2D1DeviceContext1> deviceContext;
        ThrowIfFailed(drawingSessionResourceWrapper->GetResource(IID_PPV_ARGS(&deviceContext)));

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


    TEST_METHOD(CanvasRenderTarget_InheritanceFromBitmap)
    {
        // This exercises some of the Bitmap methods on CanvasRenderTarget.

        Size expectedSize = { 33, 44 };

        auto d2dBitmap = Make<StubD2DBitmap>(D2D1_BITMAP_OPTIONS_TARGET);
        d2dBitmap->GetSizeMethod.AllowAnyCall(
            [&] { return D2D1_SIZE_F{expectedSize.Width, expectedSize.Height}; });

        d2dBitmap->GetPixelSizeMethod.AllowAnyCall(
            [&] { return ToD2DSizeU(expectedSize.Width, expectedSize.Height); });

        auto renderTarget = CreateRenderTarget(d2dBitmap, expectedSize);

        ComPtr<ICanvasBitmap> renderTargetAsBitmap;
        ThrowIfFailed(renderTarget.As(&renderTargetAsBitmap));

        Size retrievedSize;

        ThrowIfFailed(renderTargetAsBitmap->get_Size(&retrievedSize));
        Assert::AreEqual(expectedSize.Width, retrievedSize.Width);
        Assert::AreEqual(expectedSize.Height, retrievedSize.Height);

        // Bitmaps are constructed against default DPI, currently, so the pixel 
        // size and dips size should be equal.
        ThrowIfFailed(renderTargetAsBitmap->get_SizeInPixels(&retrievedSize));
        Assert::AreEqual(expectedSize.Width, retrievedSize.Width);
        Assert::AreEqual(expectedSize.Height, retrievedSize.Height);
    }
};
