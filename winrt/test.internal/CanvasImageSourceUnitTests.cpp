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


TEST_CLASS(CanvasImageSourceUnitTests)
{
public:
    TEST_METHOD(CanvasImageSourceConstruction)
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

        bool mockSetDeviceCalled = false;
        mockSurfaceImageSource->MockSetDevice =
            [&](IUnknown* actualDevice)
        {
            Assert::IsFalse(mockSetDeviceCalled);

            ComPtr<IUnknown> expectedDevice;
            ThrowIfFailed(mockD2DDevice.As(&expectedDevice));

            Assert::AreEqual(expectedDevice.Get(), actualDevice);
            mockSetDeviceCalled = true;
        };

        //
        // Actually create the CanvasImageSource
        //
        auto canvasImageSource = Make<CanvasImageSource>(
            mockCanvasDevice.Get(),
            expectedWidth,
            expectedHeight,
            expectedBackground,
            mockSurfaceImageSourceFactory.Get(),
            std::make_shared<MockCanvasImageSourceDrawingSessionFactory>());
        
        Assert::IsTrue(mockCreateInstanceCalled);
        Assert::IsTrue(mockSetDeviceCalled);
        
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
    }

    TEST_METHOD(CanvasImageSourceGetDevice)
    {
        ComPtr<ICanvasDevice> expectedCanvasDevice = Make<StubCanvasDevice>();
        auto stubSurfaceImageSourceFactory = Make<StubSurfaceImageSourceFactory>();

        auto canvasImageSource = Make<CanvasImageSource>(
            static_cast<ICanvasResourceCreator*>(static_cast<StubCanvasDevice*>(expectedCanvasDevice.Get())),
            1,
            1,
            CanvasBackground::Transparent,
            stubSurfaceImageSourceFactory.Get(),
            std::make_shared<MockCanvasImageSourceDrawingSessionFactory>());

        ComPtr<ICanvasDevice> actualCanvasDevice;
        ThrowIfFailed(canvasImageSource->get_Device(&actualCanvasDevice));

        Assert::AreEqual(expectedCanvasDevice.Get(), actualCanvasDevice.Get());

        // Calling get_Device with a nullptr should fail appropriately
        Assert::AreEqual(E_INVALIDARG, canvasImageSource->get_Device(nullptr));
    }

    TEST_METHOD(CanvasImageSourcePutDevice)
    {
        auto firstCanvasDevice = Make<StubCanvasDevice>();

        auto mockSurfaceImageSource = Make<MockSurfaceImageSource>();
        auto stubSurfaceImageSourceFactory = Make<StubSurfaceImageSourceFactory>(mockSurfaceImageSource.Get());

        // On the initial make we know that this succeeds
        mockSurfaceImageSource->MockSetDevice = [&](IUnknown*) {};

        auto canvasImageSource = Make<CanvasImageSource>(
            firstCanvasDevice.Get(),
            1,
            1,
            CanvasBackground::Transparent,
            stubSurfaceImageSourceFactory.Get(),
            std::make_shared<MockCanvasImageSourceDrawingSessionFactory>());

        auto secondCanvasDevice = Make<StubCanvasDevice>();
        ComPtr<IUnknown> expectedD2DDevice = secondCanvasDevice->GetD2DDevice();

        // When the new device is set we expect SetDevice to be called with the
        // new D2D device
        bool setDeviceCalled = false;
        mockSurfaceImageSource->MockSetDevice = 
            [&](IUnknown* actualDevice)
            {
                Assert::IsFalse(setDeviceCalled);
                Assert::AreEqual(expectedD2DDevice.Get(), actualDevice);
                setDeviceCalled = true;
            };

        ThrowIfFailed(canvasImageSource->put_Device(secondCanvasDevice.Get()));

        Assert::IsTrue(setDeviceCalled);

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
        setDeviceCalled = false;
        mockSurfaceImageSource->MockSetDevice = 
            [&](IUnknown* actualDevice)
            {
                Assert::IsFalse(setDeviceCalled);
                setDeviceCalled = true;
                ThrowHR(E_FAIL);
            };

        Assert::IsTrue(FAILED(canvasImageSource->put_Device(secondCanvasDevice.Get())));

        Assert::IsTrue(setDeviceCalled);

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
};

TEST_CLASS(CanvasImageSourceCreateDrawingSessionTests)
{
    ComPtr<StubCanvasDevice> m_canvasDevice;
    ComPtr<MockSurfaceImageSource> m_surfaceImageSource;
    ComPtr<StubSurfaceImageSourceFactory> m_surfaceImageSourceFactory;
    std::shared_ptr<MockCanvasImageSourceDrawingSessionFactory> m_canvasImageSourceDrawingSessionFactory;
    ComPtr<CanvasImageSource> m_canvasImageSource;
    int m_imageWidth;
    int m_imageHeight;

public:
    TEST_METHOD_INITIALIZE(Init)
    {
        m_canvasDevice = Make<StubCanvasDevice>();
        m_surfaceImageSource = Make<MockSurfaceImageSource>();
        m_surfaceImageSourceFactory = Make<StubSurfaceImageSourceFactory>(m_surfaceImageSource.Get());
        m_canvasImageSourceDrawingSessionFactory = std::make_shared<MockCanvasImageSourceDrawingSessionFactory>();

        m_surfaceImageSource->MockSetDevice = [&](IUnknown*) {};

        m_imageWidth = 123;
        m_imageHeight = 456;

        m_canvasImageSource = Make<CanvasImageSource>(
            m_canvasDevice.Get(),
            m_imageWidth,
            m_imageHeight,
            CanvasBackground::Transparent,
            m_surfaceImageSourceFactory.Get(),
            m_canvasImageSourceDrawingSessionFactory);

        m_surfaceImageSource->MockSetDevice = nullptr;
    }

    TEST_METHOD(CanvasImageSource_CreateDrawingSession_PassesEntireImage)
    {
        bool createCalled = false;
        m_canvasImageSourceDrawingSessionFactory->MockCreate =
            [&](ICanvasDevice* owner, ISurfaceImageSourceNativeWithD2D* sisNative, Rect const& updateRect, float dpi)
            {
                Assert::IsFalse(createCalled);
                Assert::AreEqual<float>(0, updateRect.X);
                Assert::AreEqual<float>(0, updateRect.Y);
                Assert::AreEqual<float>(static_cast<float>(m_imageWidth), updateRect.Width);
                Assert::AreEqual<float>(static_cast<float>(m_imageHeight), updateRect.Height);
                createCalled=true;
                return Make<MockCanvasDrawingSession>();
            };

        ComPtr<ICanvasDrawingSession> drawingSession;
        ThrowIfFailed(m_canvasImageSource->CreateDrawingSession(&drawingSession));
        Assert::IsTrue(createCalled);
        Assert::IsTrue(drawingSession);
    }

    TEST_METHOD(CanvasImageSource_CreateDrawingSessionWithUpdateRegion_PassesSpecifiedUpdateRegion)
    {
        Rect expectedRect = {};
        expectedRect.X = 1;
        expectedRect.Y = 2;
        expectedRect.Width = 3;
        expectedRect.Height = 4;

        bool createCalled = false;
        m_canvasImageSourceDrawingSessionFactory->MockCreate = 
            [&](ICanvasDevice* owner, ISurfaceImageSourceNativeWithD2D* sisNative, Rect const& updateRect, float dpi)
            {
                Assert::IsFalse(createCalled);
                Assert::AreEqual(expectedRect.X, updateRect.X);
                Assert::AreEqual(expectedRect.Y, updateRect.Y);
                Assert::AreEqual(expectedRect.Width, updateRect.Width);
                Assert::AreEqual(expectedRect.Height, updateRect.Height);
                createCalled = true;
                return Make<MockCanvasDrawingSession>();
            };


        ComPtr<ICanvasDrawingSession> drawingSession;
        ThrowIfFailed(m_canvasImageSource->CreateDrawingSessionWithUpdateRectangle(expectedRect, &drawingSession));
        Assert::IsTrue(createCalled);
        Assert::IsTrue(drawingSession);
    }
};

TEST_CLASS(CanvasImageSourceDrawingSessionAdapterTests)
{
public:
    TEST_METHOD(CanvasImageSourceDrawingSessionAdapter_BeginEndDraw)
    {
        auto mockDeviceContext = Make<MockD2DDeviceContext>();
        auto mockSurfaceImageSource = Make<MockSurfaceImageSource>();        
        RECT expectedUpdateRect{1, 2, 3, 4};
        POINT expectedOffset{5, 6};

        bool beginDrawCalled = false;
        mockSurfaceImageSource->MockBeginDraw =
            [&](RECT const& updateRect, IID const& iid, void** updateObject, POINT* offset)
            {
                Assert::IsFalse(beginDrawCalled);
                Assert::AreEqual(expectedUpdateRect, updateRect);
                Assert::AreEqual(_uuidof(ID2D1DeviceContext), iid);

                ThrowIfFailed(mockDeviceContext.CopyTo(iid, updateObject));
                *offset = expectedOffset;
                beginDrawCalled = true;
            };

        bool setTransformCalled = false;
        mockDeviceContext->MockSetTransform =
            [&](const D2D1_MATRIX_3X2_F* m)
            {
                Assert::IsFalse(setTransformCalled);

                // We expect the transform to be set to just the offset
                Assert::AreEqual(1.0f, m->_11);
                Assert::AreEqual(0.0f, m->_12);
                Assert::AreEqual(0.0f, m->_21);
                Assert::AreEqual(1.0f, m->_22);
                Assert::AreEqual(static_cast<float>(expectedOffset.x), m->_31);
                Assert::AreEqual(static_cast<float>(expectedOffset.y), m->_32);

                setTransformCalled = true;
            };

        bool setDpiCalled = false;
        mockDeviceContext->MockSetDpi =
            [&](float dpiX, float dpiY)
            {
                Assert::AreEqual(DEFAULT_DPI, dpiX);
                Assert::AreEqual(DEFAULT_DPI, dpiY);
                setDpiCalled = true;
            };

        ComPtr<ID2D1DeviceContext1> actualDeviceContext;
        auto adapter = CanvasImageSourceDrawingSessionAdapter::Create(
            mockSurfaceImageSource.Get(),
            expectedUpdateRect,
            DEFAULT_DPI,
            &actualDeviceContext);

        Assert::AreEqual<ID2D1DeviceContext1*>(mockDeviceContext.Get(), actualDeviceContext.Get());
        Assert::IsTrue(beginDrawCalled);
        Assert::IsTrue(setTransformCalled);
        Assert::IsTrue(setDpiCalled);
        
        bool endDrawCalled = false;
        mockSurfaceImageSource->MockEndDraw =
            [&]
            {
                Assert::IsFalse(endDrawCalled);
                endDrawCalled = true;
            };

        adapter->EndDraw();
        Assert::IsTrue(endDrawCalled);
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

    TEST_METHOD(CanvasImageSourceDrawingSessionAdapter_When_SisNative_Gives_Unusuable_DeviceContext_Then_EndDraw_Called)
    {
        auto sis = Make<MockSurfaceImageSource>();
        bool beginDrawCalled = false;
        sis->MockBeginDraw = 
            [&](RECT const&, IID const& iid, void** obj, POINT*)
            {
                Assert::IsFalse(beginDrawCalled);
                beginDrawCalled = true;
                auto deviceContext = Make<DeviceContextWithRestrictedQI>();
                ThrowIfFailed(deviceContext.CopyTo(iid, obj));
            };

        bool endDrawCalled = false;
        sis->MockEndDraw =
            [&]
            {
                Assert::IsFalse(endDrawCalled);
                endDrawCalled = true;
            };

        //
        // We expect creating the adapter to fail since our SurfaceImageSource
        // implementation returns a device context that only implements
        // ID2D1DeviceContext.  We require ID2D1DeviceContext1.
        //
        ComPtr<ID2D1DeviceContext1> actualDeviceContext;
        Assert::ExpectException<NoSuchInterfaceException>(
            [&]
            {
                CanvasImageSourceDrawingSessionAdapter::Create(
                    sis.Get(),
                    RECT{1,2,3,4},
                    DEFAULT_DPI,
                    &actualDeviceContext);
            });

        //
        // If this happens we expect BeginDraw to have been called.  Then, after
        // the failure, we expect EndDraw to have been called in order to clean
        // up properly.
        //
        Assert::IsTrue(beginDrawCalled);
        Assert::IsTrue(endDrawCalled);
    }

    TEST_METHOD(CanvasImageSource_CreateFromCanvasControl)
    {
        std::shared_ptr<CanvasControlTestAdapter> canvasControlAdapter = std::make_shared<CanvasControlTestAdapter>();
        
        ComPtr<CanvasControl> canvasControl = Make<CanvasControl>(canvasControlAdapter);

        auto stubSurfaceImageSourceFactory = Make<StubSurfaceImageSourceFactory>();

        auto canvasImageSource = Make<CanvasImageSource>(
            canvasControl.Get(),
            123,
            456,
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

    TEST_METHOD(CanvasImageSource_CreateFromDrawingSession)
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
            5,
            10,
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
};
