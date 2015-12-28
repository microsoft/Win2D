// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

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
        CanvasAlphaMode expectedAlphaMode = CanvasAlphaMode::Ignore;

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

            auto actualAlphaMode = isOpaque ? CanvasAlphaMode::Ignore : CanvasAlphaMode::Premultiplied;
            Assert::AreEqual(expectedAlphaMode, actualAlphaMode);

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
            expectedAlphaMode,
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

        CanvasAlphaMode actualAlphaMode;
        ThrowIfFailed(canvasImageSource->get_AlphaMode(&actualAlphaMode));
        Assert::AreEqual(CanvasAlphaMode::Ignore, actualAlphaMode);
    }

    struct AlphaModeFixture
    {
        bool ExpectedIsOpaque;
        
        AlphaModeFixture()
            : ExpectedIsOpaque(false)
        {}

        void CreateImageSource(CanvasAlphaMode alphaMode)
        {
            auto factory = Make<MockSurfaceImageSourceFactory>();

            bool createWasCalled = false;
            factory->MockCreateInstanceWithDimensionsAndOpacity =
                [&] (int32_t,int32_t,bool opaque,IInspectable*)
                {
                    createWasCalled = true;
                    Assert::AreEqual(ExpectedIsOpaque, opaque);
                    return Make<StubSurfaceImageSource>();
                };

            Make<CanvasImageSource>(
                As<ICanvasResourceCreator>(Make<StubCanvasDevice>()).Get(),
                1.0f,
                1.0f,
                DEFAULT_DPI,
                alphaMode,
                factory.Get(),
                std::make_shared<MockCanvasImageSourceDrawingSessionFactory>());

            Assert::IsTrue(createWasCalled, L"SurfaceImageSourceFactory::Create was called");
        }
    };

    TEST_METHOD_EX(CanvasImageSource_WhenConstructedWithAlphaModeIgnore_ThenUnderlyingImageSourceIsOpaque)
    {
        AlphaModeFixture f;

        f.ExpectedIsOpaque = true;
        f.CreateImageSource(CanvasAlphaMode::Ignore);
    }

    TEST_METHOD_EX(CanvasImageSource_WhenConstructedWithAlphaModePremultiplied_ThenUnderlyingImageSourceIsTransparent)
    {
        AlphaModeFixture f;

        f.ExpectedIsOpaque = false;
        f.CreateImageSource(CanvasAlphaMode::Premultiplied);
    }

    TEST_METHOD_EX(CanvasImageSource_WhenConstructedWithAlphaModeStraight_ThenFailsWithInvalidArg)
    {
        AlphaModeFixture f;
        ExpectHResultException(E_INVALIDARG, [&] { f.CreateImageSource(CanvasAlphaMode::Straight); });
        ValidateStoredErrorState(E_INVALIDARG, Strings::InvalidAlphaModeForImageSource);
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
            CanvasAlphaMode::Premultiplied,
            stubSurfaceImageSourceFactory.Get(),
            std::make_shared<MockCanvasImageSourceDrawingSessionFactory>());

        ComPtr<ICanvasDevice> actualCanvasDevice;
        ThrowIfFailed(canvasImageSource->get_Device(&actualCanvasDevice));

        Assert::AreEqual(expectedCanvasDevice.Get(), actualCanvasDevice.Get());

        // Calling get_Device with a nullptr should fail appropriately
        Assert::AreEqual(E_INVALIDARG, canvasImageSource->get_Device(nullptr));
    }

    struct RecreateFixture
    {
        ComPtr<StubCanvasDevice> FirstDevice;
        ComPtr<MockSurfaceImageSource> SurfaceImageSource;
        ComPtr<CanvasImageSource> ImageSource;

        RecreateFixture()
            : FirstDevice(Make<StubCanvasDevice>())
            , SurfaceImageSource(Make<MockSurfaceImageSource>())
        {
            auto surfaceImageSourceFactory = Make<StubSurfaceImageSourceFactory>(SurfaceImageSource.Get());

            // On the initial make we know that this succeeds
            SurfaceImageSource->SetDeviceMethod.AllowAnyCall();

            ImageSource = Make<CanvasImageSource>(
                FirstDevice.Get(),
                1.0f,
                1.0f,
                DEFAULT_DPI,
                CanvasAlphaMode::Premultiplied,
                surfaceImageSourceFactory.Get(),
                std::make_shared<MockCanvasImageSourceDrawingSessionFactory>());            
        }


        void ExpectSetDeviceWithNullAndThen(ComPtr<IUnknown> expected)
        {
            SurfaceImageSource->SetDeviceMethod.SetExpectedCalls(2,
                [=] (IUnknown* actualDevice)
                {
                    if (this->SurfaceImageSource->SetDeviceMethod.GetCurrentCallCount() == 1)
                    {
                        Assert::IsNull(actualDevice);
                    }
                    else
                    {
                        Assert::IsTrue(IsSameInstance(actualDevice, expected.Get()));
                    }
                    return S_OK;
                });
        }
    };

    TEST_METHOD_EX(CanvasImageSource_Recreate_WhenPassedNull_ReturnsInvalidArg)
    {
        RecreateFixture f;

        Assert::AreEqual(E_INVALIDARG, f.ImageSource->Recreate(nullptr));
    }

    TEST_METHOD_EX(CanvasImageSource_Recreate_WhenPassedNewDevice_SetsDevice)
    {
        RecreateFixture f;

        auto secondDevice = Make<StubCanvasDevice>();

        f.ExpectSetDeviceWithNullAndThen(secondDevice->GetD2DDevice());
        ThrowIfFailed(f.ImageSource->Recreate(secondDevice.Get()));

        ComPtr<ICanvasDevice> retrievedDevice;
        ThrowIfFailed(f.ImageSource->get_Device(&retrievedDevice));
        Assert::IsTrue(IsSameInstance(secondDevice.Get(), retrievedDevice.Get()));
    }

    TEST_METHOD_EX(CanvasImageSource_Recreated_WhenPassedOriginalDevice_SetsDevice)
    {
        RecreateFixture f;

        f.ExpectSetDeviceWithNullAndThen(f.FirstDevice->GetD2DDevice());
        ThrowIfFailed(f.ImageSource->Recreate(f.FirstDevice.Get()));

        ComPtr<ICanvasDevice> retrievedDevice;
        ThrowIfFailed(f.ImageSource->get_Device(&retrievedDevice));
        Assert::IsTrue(IsSameInstance(f.FirstDevice.Get(), retrievedDevice.Get()));
    }

    TEST_METHOD_EX(CanvasImageSource_CreateFromCanvasControl)
    {
        auto canvasControlAdapter = std::make_shared<CanvasControlTestAdapter>();
        
        auto canvasControl = Make<CanvasControl>(canvasControlAdapter);

        // Get the control to a point where it has created the device.
        canvasControlAdapter->CreateCanvasImageSourceMethod.AllowAnyCall();
        auto userControl = static_cast<StubUserControl*>(As<IUserControl>(canvasControl).Get());
        ThrowIfFailed(userControl->LoadedEventSource->InvokeAll(nullptr, nullptr));
        canvasControlAdapter->RaiseCompositionRenderingEvent();
        
        auto stubSurfaceImageSourceFactory = Make<StubSurfaceImageSourceFactory>();

        auto canvasImageSource = Make<CanvasImageSource>(
            canvasControl.Get(),
            123.0f,
            456.0f,
            DEFAULT_DPI,
            CanvasAlphaMode::Ignore,
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

        d2dDeviceContext->SetTextAntialiasModeMethod.SetExpectedCalls(1,
            [](D2D1_TEXT_ANTIALIAS_MODE mode)
            {
                Assert::AreEqual(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE, mode);
            });

        ComPtr<CanvasDrawingSession> drawingSession = CanvasDrawingSession::CreateNew(
            d2dDeviceContext.Get(),
            std::make_shared<StubCanvasDrawingSessionAdapter>(),
            canvasDevice.Get());

        auto stubSurfaceImageSourceFactory = Make<StubSurfaceImageSourceFactory>();

        auto canvasImageSource = Make<CanvasImageSource>(
            drawingSession.Get(),
            5.0f,
            10.0f,
            DEFAULT_DPI,
            CanvasAlphaMode::Ignore,
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
        auto resourceCreator = CanvasDrawingSession::CreateNew(d2dDeviceContext.Get(), std::make_shared<StubCanvasDrawingSessionAdapter>(), canvasDevice.Get());
        auto stubSurfaceImageSourceFactory = Make<StubSurfaceImageSourceFactory>();
        auto mockDrawingSessionFactory = std::make_shared<MockCanvasImageSourceDrawingSessionFactory>();
        auto canvasImageSource = Make<CanvasImageSource>(resourceCreator.Get(), 1.0f, 1.0f, dpi, CanvasAlphaMode::Ignore, stubSurfaceImageSourceFactory.Get(), mockDrawingSessionFactory);

        float actualDpi = 0;
        ThrowIfFailed(canvasImageSource->get_Dpi(&actualDpi));
        Assert::AreEqual(dpi, actualDpi);

        VerifyConvertDipsToPixels(dpi, canvasImageSource);

        const float testValue = 100;
        float dips = 0;
        ThrowIfFailed(canvasImageSource->ConvertPixelsToDips((int)testValue, &dips));
        Assert::AreEqual(testValue * DEFAULT_DPI / dpi, dips);
    }

    TEST_METHOD_EX(CanvasImageSource_PropagatesDpiToDrawingSession)
    {
        const float expectedDpi = 144;

        auto canvasDevice = Make<StubCanvasDevice>();
        auto d2dDeviceContext = Make<StubD2DDeviceContextWithGetFactory>();
        auto resourceCreator = CanvasDrawingSession::CreateNew(d2dDeviceContext.Get(), std::make_shared<StubCanvasDrawingSessionAdapter>(), canvasDevice.Get());
        auto stubSurfaceImageSourceFactory = Make<StubSurfaceImageSourceFactory>();
        auto mockDrawingSessionFactory = std::make_shared<MockCanvasImageSourceDrawingSessionFactory>();
        auto canvasImageSource = Make<CanvasImageSource>(resourceCreator.Get(), 1.0f, 1.0f, expectedDpi, CanvasAlphaMode::Ignore, stubSurfaceImageSourceFactory.Get(), mockDrawingSessionFactory);

        mockDrawingSessionFactory->CreateMethod.SetExpectedCalls(1, [&](ICanvasDevice*, ISurfaceImageSourceNativeWithD2D*, Color const&, Rect const&, float dpi)
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
        auto resourceCreator = CanvasDrawingSession::CreateNew(d2dDeviceContext.Get(), std::make_shared<StubCanvasDrawingSessionAdapter>(), canvasDevice.Get());
        auto stubSurfaceImageSourceFactory = Make<StubSurfaceImageSourceFactory>();
        auto mockDrawingSessionFactory = std::make_shared<MockCanvasImageSourceDrawingSessionFactory>();
        auto canvasImageSource = Make<CanvasImageSource>(resourceCreator.Get(), width, height, dpi, CanvasAlphaMode::Ignore, stubSurfaceImageSourceFactory.Get(), mockDrawingSessionFactory);

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
                CanvasAlphaMode::Premultiplied,
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
            [&](ICanvasDevice*, ISurfaceImageSourceNativeWithD2D*, Color const&, Rect const& updateRect, float)
            {
                Rect expectedRect{ 0, 0, static_cast<float>(f.m_imageWidth), static_cast<float>(f.m_imageHeight) };
                Assert::AreEqual(expectedRect, updateRect);
                return Make<MockCanvasDrawingSession>();
            });

        ComPtr<ICanvasDrawingSession> drawingSession;
        ThrowIfFailed(f.m_canvasImageSource->CreateDrawingSession(f.m_anyColor, &drawingSession));
        Assert::IsTrue(drawingSession);
    }

    TEST_METHOD_EX(CanvasImageSource_CreateDrawingSessionWithUpdateRegion_PassesSpecifiedUpdateRegion)
    {
        Fixture f;

        Rect expectedRect{ 1, 2, 3, 4 };

        f.m_canvasImageSourceDrawingSessionFactory->CreateMethod.SetExpectedCalls(1, 
            [&](ICanvasDevice*, ISurfaceImageSourceNativeWithD2D*, Color const&, Rect const& updateRect, float)
            {
                Assert::AreEqual(expectedRect, updateRect);
                return Make<MockCanvasDrawingSession>();
            });


        ComPtr<ICanvasDrawingSession> drawingSession;
        ThrowIfFailed(f.m_canvasImageSource->CreateDrawingSessionWithUpdateRectangle(f.m_anyColor, expectedRect, &drawingSession));
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
            [&](ICanvasDevice*, ISurfaceImageSourceNativeWithD2D*, Color const& clearColor, Rect const&, float)
            {
                Assert::AreEqual(expectedColor, clearColor);
                return Make<MockCanvasDrawingSession>();
            });

        ComPtr<ICanvasDrawingSession> ignoredDrawingSession;
        ThrowIfFailed(f.m_canvasImageSource->CreateDrawingSession(expectedColor, &ignoredDrawingSession));
        ThrowIfFailed(f.m_canvasImageSource->CreateDrawingSessionWithUpdateRectangle(expectedColor, Rect{ (float)anyLeft, (float)anyTop, (float)anyWidth, (float)anyHeight }, &ignoredDrawingSession));
    }
};

TEST_CLASS(CanvasImageSourceDrawingSessionFactoryTests)
{
public:
    TEST_METHOD_EX(CanvasImageSourceDrawingSessionFactory_CallsBeginDrawWithCorrectUpdateRectangle)
    {
        auto drawingSessionFactory = std::make_shared<CanvasImageSourceDrawingSessionFactory>();
        auto surfaceImageSource = Make<MockSurfaceImageSource>();

        float dpi = 123.0f;
        Rect updateRectangleInDips{ 10, 20, 30, 40 };

        RECT expectedUpdateRectangle = ToRECT(updateRectangleInDips, dpi);

        surfaceImageSource->BeginDrawMethod.SetExpectedCalls(1,
            [&] (RECT const& updateRectangle, IID const& iid, void** updateObject, POINT*)
            {
                Assert::AreEqual(expectedUpdateRectangle, updateRectangle);

                auto dc = Make<StubD2DDeviceContext>(nullptr);
                dc->SetTransformMethod.AllowAnyCall();

                return dc.CopyTo(iid, updateObject);
            });

        surfaceImageSource->EndDrawMethod.SetExpectedCalls(1);

        Color anyClearColor{ 1,2,3,4 };

        drawingSessionFactory->Create(
            Make<MockCanvasDevice>().Get(),
            As<ISurfaceImageSourceNativeWithD2D>(surfaceImageSource).Get(),
            std::make_shared<bool>(),
            anyClearColor,
            updateRectangleInDips,
            dpi);
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

        D2D1_POINT_2F expectedOffset{
            static_cast<float>(beginDrawOffset.x - expectedUpdateRect.left),
            static_cast<float>(beginDrawOffset.y - expectedUpdateRect.top) };

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
                Assert::AreEqual(1.0f, m->_11);
                Assert::AreEqual(0.0f, m->_12);
                Assert::AreEqual(0.0f, m->_21);
                Assert::AreEqual(1.0f, m->_22);
                Assert::AreEqual(expectedOffset.x, m->_31);
                Assert::AreEqual(expectedOffset.y, m->_32);
            });

        mockDeviceContext->SetDpiMethod.SetExpectedCalls(1,
            [&](float dpiX, float dpiY)
            {
                Assert::AreEqual(DEFAULT_DPI, dpiX);
                Assert::AreEqual(DEFAULT_DPI, dpiY);
            });

        ComPtr<ID2D1DeviceContext1> actualDeviceContext;
        D2D1_POINT_2F actualOffset;
        auto adapter = CanvasImageSourceDrawingSessionAdapter::Create(
            mockSurfaceImageSource.Get(),
            expectedClearColor,
            expectedUpdateRect,
            DEFAULT_DPI,
            &actualDeviceContext,
            &actualOffset);

        Assert::AreEqual<ID2D1DeviceContext1*>(mockDeviceContext.Get(), actualDeviceContext.Get());
        Assert::AreEqual(expectedOffset, actualOffset);
        
        mockSurfaceImageSource->EndDrawMethod.SetExpectedCalls(1);

        adapter->EndDraw(actualDeviceContext.Get());
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
        D2D1_POINT_2F actualOffset;
        ExpectHResultException(E_NOINTERFACE,
            [&]
            {
                CanvasImageSourceDrawingSessionAdapter::Create(
                    sis.Get(),
                    D2D1_COLOR_F{ 1, 2, 3, 4 },
                    RECT{ 1, 2, 3, 4 },
                    DEFAULT_DPI,
                    &actualDeviceContext,
                    &actualOffset);
            });
    }
};
