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

#include "CanvasControlTestAdapter.h"

TEST_CLASS(CanvasImageSourceUnitTests)
{
public:
    TEST_METHOD_EX(CanvasImageSourceConstruction)
    {
        //
        // On construction the CanvasImageSource is expected to:
        //
        //  - create a SurfaceImageSource with the specified width/height and background mode
        //
        //  - set this as the composable base (queryable by GetComposableBase())
        //
        //  - call SetDevice on the SurfaceImageSourceNativeWith2D, passing it
        //    the d2d device associated with the CanvasDevice that was passed in
        //

        int expectedHeight = 123;
        int expectedWidth = 456;
        CanvasBackground expectedBackground = CanvasBackground::Opaque;

        auto mockD2DDevice = Make<MockD2DDevice>();
        auto mockCanvasDevice = Make<MockCanvasDevice>();
        auto mockSurfaceImageSourceFactory = Make<MockSurfaceImageSourceFactory>();
        auto mockSurfaceImageSource = Make<MockSurfaceImageSource>();

        //
        // Verify that the SurfaceImageSourceFactory is asked to create a
        // SurfaceImageSource with correct parameters.
        //
        // Note that actualOuter is a raw pointer - we don't want actualOuter to
        // ever become the last thing referencing the CanvasImageSource (this
        // could happen since the mock below is called during CanvasImageSource
        // construction; if that fails then things can blow up since actualOuter
        // exists outside normal exception cleanup)
        //
        bool mockCreateInstanceCalled = false;
        IInspectable* actualOuter;
        mockSurfaceImageSourceFactory->MockCreateInstanceWithDimensionsAndOpacity =
            [&](int32_t actualWidth, int32_t actualHeight, bool isOpaque, IInspectable* outer)
        {
            Assert::IsFalse(mockCreateInstanceCalled);
            Assert::AreEqual(expectedWidth, actualWidth);
            Assert::AreEqual(expectedHeight, actualHeight);

            auto actualBackground = isOpaque ? CanvasBackground::Opaque : CanvasBackground::Transparent;
            Assert::AreEqual(expectedBackground, actualBackground);

            actualOuter = outer;

            mockCreateInstanceCalled = true;
            return mockSurfaceImageSource;
        };

        //
        // Verify that SetDevice is called on the SurfaceImageSource with the
        // correct D2D device
        //
        mockCanvasDevice->MockGetD2DDevice =
            [&]
            {
                return mockD2DDevice;
            };

        mockCanvasDevice->Mockget_Device = 
            [&](ICanvasDevice** value)
            {
                ComPtr<ICanvasDevice> device(mockCanvasDevice.Get());

                *value = device.Detach();
            };

        mockSurfaceImageSource->SetDeviceMethod.SetExpectedCalls(1, 
            [&](IUnknown* actualDevice)
            {
                ComPtr<IUnknown> expectedDevice;
                ThrowIfFailed(mockD2DDevice.As(&expectedDevice));

                Assert::AreEqual(expectedDevice.Get(), actualDevice);
                return S_OK;
            });

        //
        // Actually create the CanvasImageSource
        //
        auto canvasImageSource = Make<CanvasImageSource>(
            mockCanvasDevice.Get(),
            (float)expectedWidth,
            (float)expectedHeight,
            DEFAULT_DPI,
            expectedBackground,
            mockSurfaceImageSourceFactory.Get(),
            std::make_shared<MockCanvasImageSourceDrawingSessionFactory>());
        
        Assert::IsTrue(mockCreateInstanceCalled);
        
        //
        // Verify the composition relationship between the CanvasImageSource and
        // the base class, SurfaceImageSource.
        //
        ComPtr<IInspectable> expectedOuter;
        ThrowIfFailed(canvasImageSource.As(&expectedOuter));
        Assert::AreEqual(expectedOuter.Get(), actualOuter);
        
        ComPtr<IInspectable> expectedComposableBase;
        ThrowIfFailed(mockSurfaceImageSource.As(&expectedComposableBase));
        Assert::AreEqual(canvasImageSource->GetComposableBase().Get(), expectedComposableBase.Get());

        CanvasBackground actualBackground;
        ThrowIfFailed(canvasImageSource->get_Background(&actualBackground));
        Assert::AreEqual(CanvasBackground::Opaque, actualBackground);
    }

    TEST_METHOD_EX(CanvasImageSourceGetDevice)
    {
        ComPtr<ICanvasDevice> expectedCanvasDevice = Make<StubCanvasDevice>();
        auto stubSurfaceImageSourceFactory = Make<StubSurfaceImageSourceFactory>();

        auto canvasImageSource = Make<CanvasImageSource>(
            static_cast<ICanvasResourceCreator*>(static_cast<StubCanvasDevice*>(expectedCanvasDevice.Get())),
            1.0f,
            1.0f,
            DEFAULT_DPI,
            CanvasBackground::Transparent,
            stubSurfaceImageSourceFactory.Get(),
            std::make_shared<MockCanvasImageSourceDrawingSessionFactory>());

        ComPtr<ICanvasDevice> actualCanvasDevice;
        ThrowIfFailed(canvasImageSource->get_Device(&actualCanvasDevice));

        Assert::AreEqual(expectedCanvasDevice.Get(), actualCanvasDevice.Get());

        // Calling get_Device with a nullptr should fail appropriately
        Assert::AreEqual(E_INVALIDARG, canvasImageSource->get_Device(nullptr));
    }

    TEST_METHOD_EX(CanvasImageSourcePutDevice)
    {
        auto firstCanvasDevice = Make<StubCanvasDevice>();

        auto mockSurfaceImageSource = Make<MockSurfaceImageSource>();
        auto stubSurfaceImageSourceFactory = Make<StubSurfaceImageSourceFactory>(mockSurfaceImageSource.Get());

        // On the initial make we know that this succeeds
        mockSurfaceImageSource->SetDeviceMethod.AllowAnyCall();

        auto canvasImageSource = Make<CanvasImageSource>(
            firstCanvasDevice.Get(),
            1.0f,
            1.0f,
            DEFAULT_DPI,
            CanvasBackground::Transparent,
            stubSurfaceImageSourceFactory.Get(),
            std::make_shared<MockCanvasImageSourceDrawingSessionFactory>());

        auto secondCanvasDevice = Make<StubCanvasDevice>();
        ComPtr<IUnknown> expectedD2DDevice = secondCanvasDevice->GetD2DDevice();

        // When the new device is set we expect SetDevice to be called with the
        // new D2D device
        mockSurfaceImageSource->SetDeviceMethod.SetExpectedCalls(1,
            [&](IUnknown* actualDevice)
            {
                Assert::AreEqual(expectedD2DDevice.Get(), actualDevice);
                return S_OK;
            });

        ThrowIfFailed(canvasImageSource->put_Device(secondCanvasDevice.Get()));

        // Verify that get_Device also returns the correct device.
        ComPtr<ICanvasDevice> expectedCanvasDevice;
        ThrowIfFailed(secondCanvasDevice.As(&expectedCanvasDevice));

        ComPtr<ICanvasDevice> actualCanvasDevice;
        ThrowIfFailed(canvasImageSource->get_Device(&actualCanvasDevice));

        Assert::AreEqual(expectedCanvasDevice.Get(), actualCanvasDevice.Get());

        //
        // Check what happens if part of the put_Device process fails
        //
        auto thirdCanvasDevice = Make<StubCanvasDevice>();

        // Simulate SetDevice failing
        mockSurfaceImageSource->SetDeviceMethod.SetExpectedCalls(1,
            [](IUnknown*)
            {
                return E_FAIL;
            });

        Assert::IsTrue(FAILED(canvasImageSource->put_Device(secondCanvasDevice.Get())));

        // Verify that get_Device returns the original device since part of
        // put_Device process failed.
        expectedCanvasDevice.Reset();
        ThrowIfFailed(secondCanvasDevice.As(&expectedCanvasDevice));

        actualCanvasDevice.Reset();
        ThrowIfFailed(canvasImageSource->get_Device(&actualCanvasDevice));

        Assert::AreEqual(expectedCanvasDevice.Get(), actualCanvasDevice.Get());   

        // Calling put_Device with nullptr should fail appropriately
        Assert::AreEqual(E_INVALIDARG, canvasImageSource->put_Device(nullptr));
    }

    TEST_METHOD_EX(CanvasImageSource_CreateFromCanvasControl)
    {
        auto canvasControlAdapter = std::make_shared<CanvasControlTestAdapter>();
        
        auto canvasControl = Make<CanvasControl>(canvasControlAdapter);

        // Get the control to a point where it has created the device.
        canvasControlAdapter->CreateCanvasImageSourceMethod.AllowAnyCall();
        auto userControl = dynamic_cast<StubUserControl*>(As<IUserControl>(canvasControl).Get());
        ThrowIfFailed(userControl->LoadedEventSource->InvokeAll(nullptr, nullptr));
        canvasControlAdapter->RaiseCompositionRenderingEvent();
        
        auto stubSurfaceImageSourceFactory = Make<StubSurfaceImageSourceFactory>();

        auto canvasImageSource = Make<CanvasImageSource>(
            canvasControl.Get(),
            123.0f,
            456.0f,
            DEFAULT_DPI,
            CanvasBackground::Opaque,
            stubSurfaceImageSourceFactory.Get(),
            std::make_shared<MockCanvasImageSourceDrawingSessionFactory>());

        //
        // Verify that the image source and the control are compatible.
        //
        ComPtr<ICanvasDevice> controlDevice;
        ThrowIfFailed(canvasControl->get_Device(&controlDevice));

        ComPtr<ICanvasDevice> sisDevice;
        ThrowIfFailed(canvasImageSource->get_Device(&sisDevice));

        Assert::AreEqual(controlDevice.Get(), sisDevice.Get());
    }

    TEST_METHOD_EX(CanvasImageSource_CreateFromDrawingSession)
    {
        ComPtr<StubCanvasDevice> canvasDevice = Make<StubCanvasDevice>();

        ComPtr<StubD2DDeviceContextWithGetFactory> d2dDeviceContext =
            Make<StubD2DDeviceContextWithGetFactory>();

        auto manager = std::make_shared<CanvasDrawingSessionManager>();
        ComPtr<CanvasDrawingSession> drawingSession = manager->Create(
            canvasDevice.Get(),
            d2dDeviceContext.Get(),
            std::make_shared<StubCanvasDrawingSessionAdapter>());

        auto stubSurfaceImageSourceFactory = Make<StubSurfaceImageSourceFactory>();

        auto canvasImageSource = Make<CanvasImageSource>(
            drawingSession.Get(),
            5.0f,
            10.0f,
            DEFAULT_DPI,
            CanvasBackground::Opaque,
            stubSurfaceImageSourceFactory.Get(),
            std::make_shared<MockCanvasImageSourceDrawingSessionFactory>());

        //
        // Verify that the image source and the drawing session are compatible.
        //
        ComPtr<ICanvasDevice> sisDevice;
        ThrowIfFailed(canvasImageSource->get_Device(&sisDevice));

        Assert::AreEqual(static_cast<ICanvasDevice*>(canvasDevice.Get()), sisDevice.Get());
    }

    TEST_METHOD_EX(CanvasImageSource_DpiProperties)
    {
        const float dpi = 144;

        auto canvasDevice = Make<StubCanvasDevice>();
        auto d2dDeviceContext = Make<StubD2DDeviceContextWithGetFactory>();
        auto manager = std::make_shared<CanvasDrawingSessionManager>();
        auto resourceCreator = manager->Create(canvasDevice.Get(), d2dDeviceContext.Get(), std::make_shared<StubCanvasDrawingSessionAdapter>());
        auto stubSurfaceImageSourceFactory = Make<StubSurfaceImageSourceFactory>();
        auto mockDrawingSessionFactory = std::make_shared<MockCanvasImageSourceDrawingSessionFactory>();
        auto canvasImageSource = Make<CanvasImageSource>(resourceCreator.Get(), 1.0f, 1.0f, dpi, CanvasBackground::Opaque, stubSurfaceImageSourceFactory.Get(), mockDrawingSessionFactory);

        float actualDpi = 0;
        ThrowIfFailed(canvasImageSource->get_Dpi(&actualDpi));
        Assert::AreEqual(dpi, actualDpi);

        const float testValue = 100;

        int pixels = 0;
        ThrowIfFailed(canvasImageSource->ConvertDipsToPixels(testValue, &pixels));
        Assert::AreEqual((int)(testValue * dpi / DEFAULT_DPI), pixels);

        float dips = 0;
        ThrowIfFailed(canvasImageSource->ConvertPixelsToDips((int)testValue, &dips));
        Assert::AreEqual(testValue * DEFAULT_DPI / dpi, dips);
    }

    TEST_METHOD_EX(CanvasImageSource_PropagatesDpiToDrawingSession)
    {
        const float expectedDpi = 144;

        auto canvasDevice = Make<StubCanvasDevice>();
        auto d2dDeviceContext = Make<StubD2DDeviceContextWithGetFactory>();
        auto manager = std::make_shared<CanvasDrawingSessionManager>();
        auto resourceCreator = manager->Create(canvasDevice.Get(), d2dDeviceContext.Get(), std::make_shared<StubCanvasDrawingSessionAdapter>());
        auto stubSurfaceImageSourceFactory = Make<StubSurfaceImageSourceFactory>();
        auto mockDrawingSessionFactory = std::make_shared<MockCanvasImageSourceDrawingSessionFactory>();
        auto canvasImageSource = Make<CanvasImageSource>(resourceCreator.Get(), 1.0f, 1.0f, expectedDpi, CanvasBackground::Opaque, stubSurfaceImageSourceFactory.Get(), mockDrawingSessionFactory);

        mockDrawingSessionFactory->CreateMethod.SetExpectedCalls(1, [&](ICanvasDevice*, ISurfaceImageSourceNativeWithD2D*, Color const&, RECT const&, float dpi)
        {
            Assert::AreEqual(expectedDpi, dpi);
            return nullptr;
        });

        ComPtr<ICanvasDrawingSession> drawingSession;
        ThrowIfFailed(canvasImageSource->CreateDrawingSession(Color{}, &drawingSession));
    }

    TEST_METHOD_EX(CanvasImageSource_SizeProperties)
    {
        const float width = 123;
        const float height = 456;
        const float dpi = 144;

        auto canvasDevice = Make<StubCanvasDevice>();
        auto d2dDeviceContext = Make<StubD2DDeviceContextWithGetFactory>();
        auto manager = std::make_shared<CanvasDrawingSessionManager>();
        auto resourceCreator = manager->Create(canvasDevice.Get(), d2dDeviceContext.Get(), std::make_shared<StubCanvasDrawingSessionAdapter>());
        auto stubSurfaceImageSourceFactory = Make<StubSurfaceImageSourceFactory>();
        auto mockDrawingSessionFactory = std::make_shared<MockCanvasImageSourceDrawingSessionFactory>();
        auto canvasImageSource = Make<CanvasImageSource>(resourceCreator.Get(), width, height, dpi, CanvasBackground::Opaque, stubSurfaceImageSourceFactory.Get(), mockDrawingSessionFactory);

        Size size = { 0, 0 };
        ThrowIfFailed(canvasImageSource->get_Size(&size));
        Assert::AreEqual(width, size.Width);
        Assert::AreEqual(height, size.Height);

        BitmapSize bitmapSize = { 0, 0 };
        ThrowIfFailed(canvasImageSource->get_SizeInPixels(&bitmapSize));
        Assert::AreEqual(static_cast<uint32_t>(round(width * dpi / DEFAULT_DPI)), bitmapSize.Width);
        Assert::AreEqual(static_cast<uint32_t>(round(height * dpi / DEFAULT_DPI)), bitmapSize.Height);
    }
};

TEST_CLASS(CanvasImageSourceCreateDrawingSessionTests)
{
    struct Fixture
    {
        ComPtr<StubCanvasDevice> m_canvasDevice;
        ComPtr<MockSurfaceImageSource> m_surfaceImageSource;
        ComPtr<StubSurfaceImageSourceFactory> m_surfaceImageSourceFactory;
        std::shared_ptr<MockCanvasImageSourceDrawingSessionFactory> m_canvasImageSourceDrawingSessionFactory;
        ComPtr<CanvasImageSource> m_canvasImageSource;
        int m_imageWidth;
        int m_imageHeight;
        Color m_anyColor;

        Fixture(float dpi = DEFAULT_DPI)
        {
            m_canvasDevice = Make<StubCanvasDevice>();
            m_surfaceImageSource = Make<MockSurfaceImageSource>();
            m_surfaceImageSourceFactory = Make<StubSurfaceImageSourceFactory>(m_surfaceImageSource.Get());
            m_canvasImageSourceDrawingSessionFactory = std::make_shared<MockCanvasImageSourceDrawingSessionFactory>();
            
            m_surfaceImageSource->SetDeviceMethod.AllowAnyCall();
            
            m_imageWidth = 123;
            m_imageHeight = 456;
            
            m_canvasImageSource = Make<CanvasImageSource>(
                m_canvasDevice.Get(),
                (float)m_imageWidth,
                (float)m_imageHeight,
                dpi,
                CanvasBackground::Transparent,
                m_surfaceImageSourceFactory.Get(),
                m_canvasImageSourceDrawingSessionFactory);
            
            m_surfaceImageSource->SetDeviceMethod.SetExpectedCalls(0);
            
            m_anyColor = Color{ 1, 2, 3, 4 };
        }
    };

    TEST_METHOD_EX(CanvasImageSource_CreateDrawingSession_PassesEntireImage)
    {
        Fixture f;

        f.m_canvasImageSourceDrawingSessionFactory->CreateMethod.SetExpectedCalls(1,
            [&](ICanvasDevice*, ISurfaceImageSourceNativeWithD2D*, Color const&, RECT const& updateRect, float)
            {
                Assert::AreEqual<int>(0, updateRect.left);
                Assert::AreEqual<int>(0, updateRect.top);
                Assert::AreEqual<int>(f.m_imageWidth, updateRect.right);
                Assert::AreEqual<int>(f.m_imageHeight, updateRect.bottom);
                return Make<MockCanvasDrawingSession>();
            });

        ComPtr<ICanvasDrawingSession> drawingSession;
        ThrowIfFailed(f.m_canvasImageSource->CreateDrawingSession(f.m_anyColor, &drawingSession));
        Assert::IsTrue(drawingSession);
    }

    TEST_METHOD_EX(CanvasImageSource_CreateDrawingSessionWithUpdateRegion_PassesSpecifiedUpdateRegion)
    {
        Fixture f;

        int32_t left = 1;
        int32_t top = 2;
        int32_t width = 3;
        int32_t height = 4;

        RECT expectedRect{ left, top, left + width, top + height };

        f.m_canvasImageSourceDrawingSessionFactory->CreateMethod.SetExpectedCalls(1, 
            [&](ICanvasDevice*, ISurfaceImageSourceNativeWithD2D*, Color const&, RECT const& updateRect, float)
            {
                Assert::AreEqual(expectedRect.left, updateRect.left);
                Assert::AreEqual(expectedRect.top, updateRect.top);
                Assert::AreEqual(expectedRect.right, updateRect.right);
                Assert::AreEqual(expectedRect.bottom, updateRect.bottom);
                return Make<MockCanvasDrawingSession>();
            });


        ComPtr<ICanvasDrawingSession> drawingSession;
        ThrowIfFailed(f.m_canvasImageSource->CreateDrawingSessionWithUpdateRectangle(f.m_anyColor, Rect{ (float)left, (float)top, (float)width, (float)height }, &drawingSession));
        Assert::IsTrue(drawingSession);
    }

    TEST_METHOD_EX(CanvasImageSource_CreateDrawingSessionWithUpdateRegion_PassesSpecifiedUpdateRegion_HighDpi)
    {
        Fixture f(DEFAULT_DPI * 2);

        int32_t left = 1;
        int32_t top = 2;
        int32_t width = 3;
        int32_t height = 4;

        f.m_canvasImageSourceDrawingSessionFactory->CreateMethod.SetExpectedCalls(1,
            [&](ICanvasDevice*, ISurfaceImageSourceNativeWithD2D*, Color const&, RECT const& updateRect, float)
        {
            Assert::AreEqual(left * 2, (int32_t)updateRect.left);
            Assert::AreEqual(top * 2, (int32_t)updateRect.top);
            Assert::AreEqual((left + width) * 2, (int32_t)updateRect.right);
            Assert::AreEqual((top + height) * 2, (int32_t)updateRect.bottom);
            return Make<MockCanvasDrawingSession>();
        });


        ComPtr<ICanvasDrawingSession> drawingSession;
        ThrowIfFailed(f.m_canvasImageSource->CreateDrawingSessionWithUpdateRectangle(f.m_anyColor, Rect{ (float)left, (float)top, (float)width, (float)height }, &drawingSession));
        Assert::IsTrue(drawingSession);
    }

    TEST_METHOD_EX(CanvasImageSource_CreateDrawingSession_PassesClearColor)
    {
        Fixture f;

        int32_t anyLeft = 1;
        int32_t anyTop = 2;
        int32_t anyWidth = 3;
        int32_t anyHeight = 4;

        Color expectedColor{ 1, 2, 3, 4 };

        f.m_canvasImageSourceDrawingSessionFactory->CreateMethod.SetExpectedCalls(2,
            [&](ICanvasDevice*, ISurfaceImageSourceNativeWithD2D*, Color const& clearColor, RECT const&, float)
            {
                Assert::AreEqual(expectedColor, clearColor);
                return Make<MockCanvasDrawingSession>();
            });

        ComPtr<ICanvasDrawingSession> ignoredDrawingSession;
        ThrowIfFailed(f.m_canvasImageSource->CreateDrawingSession(expectedColor, &ignoredDrawingSession));
        ThrowIfFailed(f.m_canvasImageSource->CreateDrawingSessionWithUpdateRectangle(expectedColor, Rect{ (float)anyLeft, (float)anyTop, (float)anyWidth, (float)anyHeight }, &ignoredDrawingSession));
    }
};

TEST_CLASS(CanvasImageSourceDrawingSessionAdapterTests)
{
public:
    TEST_METHOD_EX(CanvasImageSourceDrawingSessionAdapter_BeginEndDraw)
    {
        auto mockDeviceContext = Make<MockD2DDeviceContext>();
        auto mockSurfaceImageSource = Make<MockSurfaceImageSource>();        
        RECT expectedUpdateRect{ 1, 2, 3, 4 };
        POINT beginDrawOffset{ 5, 6 };
        D2D1_COLOR_F expectedClearColor{ 7, 8, 9, 10 };

        mockSurfaceImageSource->BeginDrawMethod.SetExpectedCalls(1,
            [&](RECT const& updateRect, IID const& iid, void** updateObject, POINT* offset)
            {
                Assert::AreEqual(expectedUpdateRect, updateRect);
                Assert::AreEqual(_uuidof(ID2D1DeviceContext), iid);

                HRESULT hr = mockDeviceContext.CopyTo(iid, updateObject);
                *offset = beginDrawOffset;
                return hr;
            });

        mockDeviceContext->ClearMethod.SetExpectedCalls(1,
            [&](D2D1_COLOR_F const* color)
            {
                Assert::AreEqual(expectedClearColor, *color);
            });

        mockDeviceContext->SetTransformMethod.SetExpectedCalls(1,
            [&](const D2D1_MATRIX_3X2_F* m)
            {
                // We expect the transform to be set to just the offset as
                // calculated from the offset returned by BeginDraw and the
                // update rectangle
                float expectedOffsetX = static_cast<float>(beginDrawOffset.x - expectedUpdateRect.left);
                float expectedOffsetY = static_cast<float>(beginDrawOffset.y - expectedUpdateRect.top);

                Assert::AreEqual(1.0f, m->_11);
                Assert::AreEqual(0.0f, m->_12);
                Assert::AreEqual(0.0f, m->_21);
                Assert::AreEqual(1.0f, m->_22);
                Assert::AreEqual(expectedOffsetX, m->_31);
                Assert::AreEqual(expectedOffsetY, m->_32);
            });

        mockDeviceContext->SetDpiMethod.SetExpectedCalls(1,
            [&](float dpiX, float dpiY)
            {
                Assert::AreEqual(DEFAULT_DPI, dpiX);
                Assert::AreEqual(DEFAULT_DPI, dpiY);
            });

        ComPtr<ID2D1DeviceContext1> actualDeviceContext;
        auto adapter = CanvasImageSourceDrawingSessionAdapter::Create(
            mockSurfaceImageSource.Get(),
            expectedClearColor,
            expectedUpdateRect,
            DEFAULT_DPI,
            &actualDeviceContext);

        Assert::AreEqual<ID2D1DeviceContext1*>(mockDeviceContext.Get(), actualDeviceContext.Get());

        mockSurfaceImageSource->EndDrawMethod.SetExpectedCalls(1);

        adapter->EndDraw();
    }

    struct DeviceContextWithRestrictedQI : public MockD2DDeviceContext
    {
        //
        // Restrict this device context implementation so that it can only be
        // QI'd for ID2D1DeviceContext.
        //
        STDMETHOD(QueryInterface)(REFIID riid, _Outptr_result_nullonfailure_ void **ppvObject)
        {
            if (riid == __uuidof(ID2D1DeviceContext))
                return RuntimeClass::QueryInterface(riid, ppvObject);

            return E_NOINTERFACE;
        }
    };

    TEST_METHOD_EX(CanvasImageSourceDrawingSessionAdapter_When_SisNative_Gives_Unusuable_DeviceContext_Then_EndDraw_Called)
    {
        auto sis = Make<MockSurfaceImageSource>();
        sis->BeginDrawMethod.SetExpectedCalls(1,
            [&](RECT const&, IID const& iid, void** obj, POINT*)
            {
                auto deviceContext = Make<DeviceContextWithRestrictedQI>();
                return deviceContext.CopyTo(iid, obj);
            });

        sis->EndDrawMethod.SetExpectedCalls(1);

        //
        // We expect creating the adapter to fail since our SurfaceImageSource
        // implementation returns a device context that only implements
        // ID2D1DeviceContext.  
        //
        // If this happens we expect BeginDraw to have been called.  Then, after
        // the failure, we expect EndDraw to have been called in order to clean
        // up properly.
        //
        ComPtr<ID2D1DeviceContext1> actualDeviceContext;
        ExpectHResultException(E_NOINTERFACE,
            [&]
            {
                CanvasImageSourceDrawingSessionAdapter::Create(
                    sis.Get(),
                    D2D1_COLOR_F{ 1, 2, 3, 4 },
                    RECT{ 1, 2, 3, 4 },
                    DEFAULT_DPI,
                    &actualDeviceContext);
            });
    }
};
