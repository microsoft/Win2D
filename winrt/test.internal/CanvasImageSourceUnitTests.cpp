// Copyright (c) Microsoft Corporation.  All rights reserved

#include "pch.h"


TEST_CLASS(CanvasImageSourceUnitTests)
{
public:
    TEST_METHOD(CanvasImageSourceConstruction)
    {
        using canvas::CanvasImageSource;

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
            [&](int32 actualWidth, int32 actualHeight, bool isOpaque, IInspectable* outer)
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
            [&]()
        {
            return mockD2DDevice;
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
        using canvas::CanvasImageSource;

        ComPtr<ICanvasDevice> expectedCanvasDevice = Make<StubCanvasDevice>();
        auto stubSurfaceImageSourceFactory = Make<StubSurfaceImageSourceFactory>();

        auto canvasImageSource = Make<CanvasImageSource>(
            expectedCanvasDevice.Get(),
            1,
            1,
            CanvasBackground::Transparent,
            stubSurfaceImageSourceFactory.Get(),
            std::make_shared<MockCanvasImageSourceDrawingSessionFactory>());

        ComPtr<ICanvasDevice> actualCanvasDevice;
        ThrowIfFailed(canvasImageSource->get_Device(&actualCanvasDevice));

        Assert::AreEqual(expectedCanvasDevice.Get(), actualCanvasDevice.Get());
    }

    TEST_METHOD(CanvasImageSourcePutDevice)
    {
        using canvas::CanvasImageSource;

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
                throw ComException(E_FAIL);
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
    }
};

TEST_CLASS(CanvasImageSourceCreateDrawingSessionTests)
{
    ComPtr<StubCanvasDevice> m_canvasDevice;
    ComPtr<MockSurfaceImageSource> m_surfaceImageSource;
    ComPtr<StubSurfaceImageSourceFactory> m_surfaceImageSourceFactory;
    std::shared_ptr<MockCanvasImageSourceDrawingSessionFactory> m_canvasImageSourceDrawingSessionFactory;
    ComPtr<canvas::CanvasImageSource> m_canvasImageSource;
    int m_imageWidth;
    int m_imageHeight;

public:
    TEST_METHOD_INITIALIZE(Init)
    {
        using canvas::CanvasImageSource;

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
            [&](ICanvasDeviceInternal* internalDevice, const Rect& updateRect)
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
            [&](ICanvasDeviceInternal* internalDevice, const Rect& updateRect)
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
        ThrowIfFailed(m_canvasImageSource->CreateDrawingSessionWithUpdateRegion(expectedRect, &drawingSession));
        Assert::IsTrue(createCalled);
        Assert::IsTrue(drawingSession);
    }
};
