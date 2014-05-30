// Copyright (c) Microsoft Corporation.  All rights reserved

#include "pch.h"

TEST_CLASS(CanvasDrawingSession_CallsAdapter)
{
    class MockCanvasDrawingSessionAdapter : public ICanvasDrawingSessionAdapter
    {
        bool m_endDrawCalled;
        bool m_endDrawShouldThrow;
        
    public:
        MockCanvasDrawingSessionAdapter()
            : m_endDrawCalled(false)
            , m_endDrawShouldThrow(false)
        {
        }

        virtual ~MockCanvasDrawingSessionAdapter()
        {
            Assert::IsTrue(m_endDrawCalled);
        }

        void SetEndDrawToThrow()
        {
            m_endDrawShouldThrow = true;
        }

        void AssertEndDrawCalled()
        {
            Assert::IsTrue(m_endDrawCalled);
        }

        virtual void EndDraw() override
        {
            Assert::IsFalse(m_endDrawCalled);
            m_endDrawCalled = true;

            if (m_endDrawShouldThrow)
                throw ComException(DXGI_ERROR_DEVICE_REMOVED);
        }
    };

    ComPtr<ID2D1DeviceContext1> m_expectedDeviceContext;
    std::shared_ptr<MockCanvasDrawingSessionAdapter> m_mockAdapter;
    ComPtr<canvas::CanvasDrawingSession> m_canvasDrawingSession;

public:
    TEST_METHOD_INITIALIZE(Init)
    {
        using canvas::CanvasDrawingSession;
        m_expectedDeviceContext = Make<MockD2DDeviceContext>();
        m_mockAdapter = std::make_shared<MockCanvasDrawingSessionAdapter>();
        m_canvasDrawingSession = Make<CanvasDrawingSession>(m_expectedDeviceContext.Get(), m_mockAdapter);
    }

    TEST_METHOD(CanvasDrawingSession_Calls_Adapter_BeginDraw_EndDraw_When_Closed)
    {
        ThrowIfFailed(m_canvasDrawingSession->Close());
        m_mockAdapter->AssertEndDrawCalled();
        
        // EndDraw should only be called once even if we call Close again
        ThrowIfFailed(m_canvasDrawingSession->Close());
    }

    TEST_METHOD(CanvasDrawingSession_Calls_Adapter_BeginDraw_EndDraw_When_Destroyed)
    {
        m_canvasDrawingSession.Reset();
        m_mockAdapter->AssertEndDrawCalled();
    }

    TEST_METHOD(CanvasDrawingSession_Close_EndDrawErrorsReturned)
    {
        m_mockAdapter->SetEndDrawToThrow();
        Assert::AreEqual(DXGI_ERROR_DEVICE_REMOVED, m_canvasDrawingSession->Close());
    }

    TEST_METHOD(CanvasDrawingSession_Destroy_EndDrawErrorsSwallowed)
    {
        m_mockAdapter->SetEndDrawToThrow();
        m_canvasDrawingSession.Reset();
        m_mockAdapter->AssertEndDrawCalled();
    }    
};

TEST_CLASS(CanvasDrawingSessionUnitTests)
{
public:
    TEST_METHOD(CanvasDrawingSession_Clear)
    {
        using canvas::CanvasDrawingSession;

        ABI::Windows::UI::Color expectedColor{1,2,3,4};

        auto mockDeviceContext = Make<MockD2DDeviceContext>();
        
        bool clearCalled = false;
        mockDeviceContext->MockClear =
            [&](const D2D1_COLOR_F* color)
            {
                Assert::IsFalse(clearCalled);
                Assert::AreEqual(expectedColor.A, NormalizedToUint8(color->a));
                Assert::AreEqual(expectedColor.R, NormalizedToUint8(color->r));
                Assert::AreEqual(expectedColor.G, NormalizedToUint8(color->g));
                Assert::AreEqual(expectedColor.B, NormalizedToUint8(color->b));
                clearCalled = true;
            };

        auto canvasDrawingSession = Make<CanvasDrawingSession>(
            mockDeviceContext.Get(),
            std::make_shared<StubCanvasDrawingSessionAdapter>());

        ThrowIfFailed(canvasDrawingSession->Clear(expectedColor));
        Assert::IsTrue(clearCalled);
    }

    TEST_METHOD(CanvasDrawingSession_Close_ReleasesDeviceContextAndOtherMethodsFail)
    {
        using canvas::CanvasDrawingSession;

        //
        // Set up a device context that tracks when it was deleted (via the
        // token)
        //
        class D2DDeviceContextWithToken : public MockD2DDeviceContext
        {
            std::shared_ptr<bool> m_token;

        public:
            D2DDeviceContextWithToken(std::shared_ptr<bool> token)
                : m_token(token)
            {}
        };

        std::shared_ptr<bool> strongToken = std::make_shared<bool>(true);
        std::weak_ptr<bool> weakToken(strongToken);

        auto deviceContext = Make<D2DDeviceContextWithToken>(strongToken);
        strongToken.reset();

        Assert::IsFalse(weakToken.expired());

        //
        // Create a drawing session using this device context, verifying that
        // this has taken ownership of the token.
        //
        auto canvasDrawingSession = Make<CanvasDrawingSession>(
            deviceContext.Get(),
            std::make_shared<StubCanvasDrawingSessionAdapter>());

        deviceContext.Reset();
        Assert::IsFalse(weakToken.expired());

        //
        // Closing the drawing session will destroy the device context
        //
        ThrowIfFailed(canvasDrawingSession->Close());
        Assert::IsTrue(weakToken.expired());

        //
        // Closing a second time is a no-op and will succeed.
        //
        ThrowIfFailed(canvasDrawingSession->Close());

        //
        // Calling any other method will return RO_E_CLOSED
        //
#define EXPECT_OBJECT_CLOSED(CODE) Assert::AreEqual(RO_E_CLOSED, CODE)

        EXPECT_OBJECT_CLOSED(canvasDrawingSession->Clear(ABI::Windows::UI::Color{0,0,0,0}));

#undef EXPECT_OBJECT_CLOSED
    }
};

