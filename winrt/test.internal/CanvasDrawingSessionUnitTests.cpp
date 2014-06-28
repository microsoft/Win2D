// Copyright (c) Microsoft Corporation.  All rights reserved

#include "pch.h"

#include "StubD2DFactoryWithCreateStrokeStyle.h"

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
                ThrowHR(DXGI_ERROR_DEVICE_REMOVED);
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

        auto manager = std::make_shared<CanvasDrawingSessionManager>();
        m_canvasDrawingSession = manager->Create(m_expectedDeviceContext.Get(), m_mockAdapter);
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

class StubD2DDeviceContextWithGetFactory : public MockD2DDeviceContext
{
public:

    ComPtr<StubD2DFactoryWithCreateStrokeStyle> m_factory;

    StubD2DDeviceContextWithGetFactory()
    {
        m_factory = Make<StubD2DFactoryWithCreateStrokeStyle>();

        CheckMakeResult(m_factory);
    }

    IFACEMETHODIMP_(void) GetFactory(ID2D1Factory** factory) const override
    {
        ThrowIfFailed(m_factory.CopyTo(factory));
    }
};

TEST_CLASS(CanvasDrawingSessionUnitTests)
{
    ComPtr<StubD2DDeviceContextWithGetFactory> m_deviceContext;
    ComPtr<canvas::CanvasDrawingSession> m_canvasDrawingSession;
    ComPtr<StubCanvasBrush> m_brush;

public:
    TEST_METHOD_INITIALIZE(Init)
    {
        using canvas::CanvasDrawingSession;

        m_deviceContext = Make<StubD2DDeviceContextWithGetFactory>();

        auto manager = std::make_shared<CanvasDrawingSessionManager>();
        m_canvasDrawingSession = manager->Create(
            m_deviceContext.Get(),
            std::make_shared<StubCanvasDrawingSessionAdapter>());

        m_brush = Make<StubCanvasBrush>();
    }

    //
    // Clear
    //

    TEST_METHOD(CanvasDrawingSession_Clear)
    {
        ABI::Windows::UI::Color expectedColor{1,2,3,4};

        bool clearCalled = false;
        m_deviceContext->MockClear =
            [&](const D2D1_COLOR_F* color)
            {
                Assert::IsFalse(clearCalled);
                Assert::AreEqual(expectedColor.A, NormalizedToUint8(color->a));
                Assert::AreEqual(expectedColor.R, NormalizedToUint8(color->r));
                Assert::AreEqual(expectedColor.G, NormalizedToUint8(color->g));
                Assert::AreEqual(expectedColor.B, NormalizedToUint8(color->b));
                clearCalled = true;
            };

        ThrowIfFailed(m_canvasDrawingSession->Clear(expectedColor));
        Assert::IsTrue(clearCalled);
    }

    //
    // DrawLine
    //

    TEST_METHOD(CanvasDrawingSession_DrawLine_NullBrush)
    {
        Assert::AreEqual(E_INVALIDARG, m_canvasDrawingSession->DrawLine(Point{}, Point{}, nullptr));
    }

    TEST_METHOD(CanvasDrawingSession_DrawLine)
    {
        Point expectedP0{1.0f,2.0f};
        Point expectedP1{3.0f,4.0f};
        float expectedStrokeWidth = 1.0f;

        bool drawLineCalled = false;
        m_deviceContext->MockDrawLine = 
            [&](D2D1_POINT_2F p0, D2D1_POINT_2F p1, ID2D1Brush* brush, float strokeWidth, ID2D1StrokeStyle* strokeStyle)
            {
                Assert::IsFalse(drawLineCalled);
                Assert::AreEqual(expectedP0.X, p0.x);
                Assert::AreEqual(expectedP0.Y, p0.y);
                Assert::AreEqual(expectedP1.X, p1.x);
                Assert::AreEqual(expectedP1.Y, p1.y);
                Assert::AreEqual(m_brush->GetD2DBrush().Get(), brush);
                Assert::AreEqual(expectedStrokeWidth, strokeWidth);
                Assert::IsNull(strokeStyle);
                drawLineCalled = true;
            };

        ThrowIfFailed(m_canvasDrawingSession->DrawLine(
            expectedP0,
            expectedP1,
            m_brush.Get()));

        Assert::IsTrue(drawLineCalled);
    }

    //
    // DrawLineWithStrokeWidth
    //

    TEST_METHOD(CanvasDrawingSession_DrawLineWithStrokeWidth_NullBrush)
    {
        Assert::AreEqual(E_INVALIDARG, m_canvasDrawingSession->DrawLineWithStrokeWidth(Point{}, Point{}, nullptr, 0.0f));
    }

    TEST_METHOD(CanvasDrawingSession_DrawLineWithStrokeWidth)
    {
        Point expectedP0{1.0f,2.0f};
        Point expectedP1{3.0f,4.0f};
        float expectedStrokeWidth = 5.0f;

        bool drawLineCalled = false;
        m_deviceContext->MockDrawLine = 
            [&](D2D1_POINT_2F p0, D2D1_POINT_2F p1, ID2D1Brush* brush, float strokeWidth, ID2D1StrokeStyle* strokeStyle)
            {
                Assert::IsFalse(drawLineCalled);
                Assert::AreEqual(ToD2DPoint(expectedP0), p0);
                Assert::AreEqual(ToD2DPoint(expectedP1), p1);
                Assert::AreEqual(m_brush->GetD2DBrush().Get(), brush);
                Assert::AreEqual(expectedStrokeWidth, strokeWidth);
                Assert::IsNull(strokeStyle);
                drawLineCalled = true;
            };

        ThrowIfFailed(m_canvasDrawingSession->DrawLineWithStrokeWidth(
            expectedP0,
            expectedP1,
            m_brush.Get(),
            expectedStrokeWidth));

        Assert::IsTrue(drawLineCalled);
    }

    //
    // DrawLineWithStrokeWidthAndStrokeStyle
    //

    TEST_METHOD(CanvasDrawingSession_DrawLineWithStrokeWidthAndStrokeStyle_NullStrokeStyle)
    {
        Assert::AreEqual(E_INVALIDARG, m_canvasDrawingSession->DrawLineWithStrokeWidthAndStrokeStyle(Point{}, Point{}, nullptr, 0.0f, nullptr));
    }

    TEST_METHOD(CanvasDrawingSession_DrawLineWithStrokeWidthAndStrokeStyle)
    {
        using canvas::CanvasStrokeStyle;

        Point expectedP0{ 1.0f, 2.0f };
        Point expectedP1{ 3.0f, 4.0f };
        float expectedStrokeWidth = 5.0f;

        bool drawLineCalled = false;
        m_deviceContext->MockDrawLine =
            [&](D2D1_POINT_2F p0, D2D1_POINT_2F p1, ID2D1Brush* brush, float strokeWidth, ID2D1StrokeStyle* strokeStyle)
        {
            Assert::IsFalse(drawLineCalled);
            Assert::AreEqual(ToD2DPoint(expectedP0), p0);
            Assert::AreEqual(ToD2DPoint(expectedP1), p1);
            Assert::AreEqual(m_brush->GetD2DBrush().Get(), brush);
            Assert::AreEqual(expectedStrokeWidth, strokeWidth);
            Assert::IsNotNull(strokeStyle);
            Assert::AreEqual(D2D1_LINE_JOIN_MITER_OR_BEVEL, m_deviceContext->m_factory->m_lineJoin);
            drawLineCalled = true;
        };

        auto canvasStrokeStyle = Make<CanvasStrokeStyle>();
        canvasStrokeStyle->put_LineJoin(CanvasLineJoin::MiterOrBevel);

        ThrowIfFailed(m_canvasDrawingSession->DrawLineWithStrokeWidthAndStrokeStyle(
            expectedP0,
            expectedP1,
            m_brush.Get(),
            expectedStrokeWidth,
            canvasStrokeStyle.Get()));

        Assert::IsTrue(drawLineCalled);
    }

    //
    // DrawRectangle
    //

    TEST_METHOD(CanvasDrawingSession_DrawRectangle_NullBrush)
    {
        Assert::AreEqual(E_INVALIDARG, m_canvasDrawingSession->DrawRectangle(Rect{}, nullptr));
    }

    TEST_METHOD(CanvasDrawingSession_DrawRectangle)
    {
        Rect expectedRect{1,2,3,4};
        float expectedStrokeWidth = 1.0f;

        bool drawRectangleCalled = false;
        m_deviceContext->MockDrawRectangle =
            [&](const D2D_RECT_F* rect, ID2D1Brush* brush, float strokeWidth, ID2D1StrokeStyle* strokeStyle)
            {
                Assert::IsFalse(drawRectangleCalled);
                Assert::AreEqual(ToD2DRect(expectedRect), *rect);
                Assert::AreEqual(m_brush->GetD2DBrush().Get(), brush);
                Assert::AreEqual(expectedStrokeWidth, strokeWidth);
                Assert::IsNull(strokeStyle);
                drawRectangleCalled = true;
            };

        ThrowIfFailed(m_canvasDrawingSession->DrawRectangle(
            expectedRect,
            m_brush.Get()));

        Assert::IsTrue(drawRectangleCalled);
    }

    //
    // DrawRectangleWithStrokeWidth
    //

    TEST_METHOD(CanvasDrawingSession_DrawRectangleWithStrokeWidth_NullBrush)
    {
        Assert::AreEqual(E_INVALIDARG, m_canvasDrawingSession->DrawRectangleWithStrokeWidth(Rect{}, nullptr, 0.0f));
    }

    TEST_METHOD(CanvasDrawingSession_DrawRectangleWithStrokeWidth)
    {
        Rect expectedRect{1,2,3,4};
        float expectedStrokeWidth = 5.0f;

        bool drawRectangleCalled = false;
        m_deviceContext->MockDrawRectangle =
            [&](const D2D_RECT_F* rect, ID2D1Brush* brush, float strokeWidth, ID2D1StrokeStyle* strokeStyle)
            {
                Assert::IsFalse(drawRectangleCalled);
                Assert::AreEqual(ToD2DRect(expectedRect), *rect);
                Assert::AreEqual(m_brush->GetD2DBrush().Get(), brush);
                Assert::AreEqual(expectedStrokeWidth, strokeWidth);
                Assert::IsNull(strokeStyle);
                drawRectangleCalled = true;
            };

        ThrowIfFailed(m_canvasDrawingSession->DrawRectangleWithStrokeWidth(
            expectedRect,
            m_brush.Get(),
            expectedStrokeWidth));

        Assert::IsTrue(drawRectangleCalled);
    }

    //
    // DrawRectangleWithStrokeWidthAndStrokeStyle
    //

    TEST_METHOD(CanvasDrawingSession_DrawRectangleWithStrokeWidthAndStrokeStyle_NullStrokeStyle)
    {
        Assert::AreEqual(E_INVALIDARG, m_canvasDrawingSession->DrawRectangleWithStrokeWidthAndStrokeStyle(Rect{}, nullptr, 0.0f, nullptr));
    }

    TEST_METHOD(CanvasDrawingSession_DrawRectangleWithStrokeWidthAndStrokeStyle)
    {
        using canvas::CanvasStrokeStyle;

        Rect expectedRect{ 1, 2, 3, 4 };
        float expectedStrokeWidth = 5.0f;

        bool drawRectangleCalled = false;
        m_deviceContext->MockDrawRectangle =
            [&](const D2D_RECT_F* rect, ID2D1Brush* brush, float strokeWidth, ID2D1StrokeStyle* strokeStyle)
        {
            Assert::IsFalse(drawRectangleCalled);
            Assert::AreEqual(ToD2DRect(expectedRect), *rect);
            Assert::AreEqual(m_brush->GetD2DBrush().Get(), brush);
            Assert::AreEqual(expectedStrokeWidth, strokeWidth);
            Assert::IsNotNull(strokeStyle);
            Assert::AreEqual(D2D1_LINE_JOIN_MITER_OR_BEVEL, m_deviceContext->m_factory->m_lineJoin);
            drawRectangleCalled = true;
        };

        auto canvasStrokeStyle = Make<CanvasStrokeStyle>();
        canvasStrokeStyle->put_LineJoin(CanvasLineJoin::MiterOrBevel);

        ThrowIfFailed(m_canvasDrawingSession->DrawRectangleWithStrokeWidthAndStrokeStyle(
            expectedRect,
            m_brush.Get(),
            expectedStrokeWidth,
            canvasStrokeStyle.Get()));

        Assert::IsTrue(drawRectangleCalled);
    }

    //
    // FillRectangle
    //

    TEST_METHOD(CanvasDrawingSession_FillRectangle_NullBrush)
    {
        Assert::AreEqual(E_INVALIDARG, m_canvasDrawingSession->FillRectangle(Rect{}, nullptr));
    }

    TEST_METHOD(CanvasDrawingSession_FillRectangle)
    {
        Rect expectedRect{1,2,3,4};

        bool fillRectangleCalled = false;
        m_deviceContext->MockFillRectangle =
            [&](const D2D1_RECT_F* rect, ID2D1Brush* brush)
            {
                Assert::IsFalse(fillRectangleCalled);
                Assert::AreEqual(ToD2DRect(expectedRect), *rect);
                Assert::AreEqual(m_brush->GetD2DBrush().Get(), brush);
                fillRectangleCalled = true;
            };

        ThrowIfFailed(m_canvasDrawingSession->FillRectangle(
            expectedRect,
            m_brush.Get()));

        Assert::IsTrue(fillRectangleCalled);
    }

    //
    // DrawRoundedRectangle
    //

    TEST_METHOD(CanvasDrawingSession_DrawRoundedRectangle_NullBrush)
    {
        Assert::AreEqual(E_INVALIDARG, m_canvasDrawingSession->DrawRoundedRectangle(CanvasRoundedRectangle{}, nullptr));
    }

    TEST_METHOD(CanvasDrawingSession_DrawRoundedRectangle)
    {
        CanvasRoundedRectangle expectedRoundedRect{Rect{1,2,3,4},5,6};
        float expectedStrokeWidth = 1.0f;

        bool drawRoundedRectangleCalled = false;
        m_deviceContext->MockDrawRoundedRectangle =
            [&](const D2D1_ROUNDED_RECT* rect, ID2D1Brush* brush, float strokeWidth, ID2D1StrokeStyle* strokeStyle)
            {
                Assert::IsFalse(drawRoundedRectangleCalled);
                Assert::AreEqual(ToD2DRoundedRect(expectedRoundedRect), *rect);
                Assert::AreEqual(m_brush->GetD2DBrush().Get(), brush);
                Assert::AreEqual(expectedStrokeWidth, strokeWidth);
                Assert::IsNull(strokeStyle);
                drawRoundedRectangleCalled = true;
            };

        ThrowIfFailed(m_canvasDrawingSession->DrawRoundedRectangle(
            expectedRoundedRect,
            m_brush.Get()));

        Assert::IsTrue(drawRoundedRectangleCalled);
    }

    //
    // DrawRoundedRectangleWithStrokeWidth
    //

    TEST_METHOD(CanvasDrawingSession_DrawRoundedRectangleWithStrokeWidth_NullBrush)
    {
        Assert::AreEqual(E_INVALIDARG, m_canvasDrawingSession->DrawRoundedRectangleWithStrokeWidth(CanvasRoundedRectangle{}, nullptr, 1.0f));
    }

    TEST_METHOD(CanvasDrawingSession_DrawRoundedRectangleWithStrokeWidth)
    {
        CanvasRoundedRectangle expectedRoundedRect{Rect{1,2,3,4},5,6};
        float expectedStrokeWidth = 5.0f;

        bool drawRoundedRectangleCalled = false;
        m_deviceContext->MockDrawRoundedRectangle =
            [&](const D2D1_ROUNDED_RECT* rect, ID2D1Brush* brush, float strokeWidth, ID2D1StrokeStyle* strokeStyle)
            {
                Assert::IsFalse(drawRoundedRectangleCalled);
                Assert::AreEqual(ToD2DRoundedRect(expectedRoundedRect), *rect);
                Assert::AreEqual(m_brush->GetD2DBrush().Get(), brush);
                Assert::AreEqual(expectedStrokeWidth, strokeWidth);
                Assert::IsNull(strokeStyle);
                drawRoundedRectangleCalled = true;
            };

        ThrowIfFailed(m_canvasDrawingSession->DrawRoundedRectangleWithStrokeWidth(
            expectedRoundedRect,
            m_brush.Get(),
            expectedStrokeWidth));

        Assert::IsTrue(drawRoundedRectangleCalled);
    }

    //
    // DrawRoundedRectangleWithStrokeWidthAndStrokeStyle
    //

    TEST_METHOD(CanvasDrawingSession_DrawRoundedRectangleWithStrokeWidthAndStrokeStyle_NullStrokeStyle)
    {
        Assert::AreEqual(E_INVALIDARG, m_canvasDrawingSession->DrawRoundedRectangleWithStrokeWidthAndStrokeStyle(CanvasRoundedRectangle{}, nullptr, 1.0f, nullptr));
    }

    TEST_METHOD(CanvasDrawingSession_DrawRoundedRectangleWithStrokeWidthAndStrokeStyle)
    {
        using canvas::CanvasStrokeStyle;

        CanvasRoundedRectangle expectedRoundedRect{ Rect{ 1, 2, 3, 4 }, 5, 6 };
        float expectedStrokeWidth = 5.0f;

        bool drawRoundedRectangleCalled = false;
        m_deviceContext->MockDrawRoundedRectangle =
            [&](const D2D1_ROUNDED_RECT* rect, ID2D1Brush* brush, float strokeWidth, ID2D1StrokeStyle* strokeStyle)
        {
            Assert::IsFalse(drawRoundedRectangleCalled);
            Assert::AreEqual(ToD2DRoundedRect(expectedRoundedRect), *rect);
            Assert::AreEqual(m_brush->GetD2DBrush().Get(), brush);
            Assert::AreEqual(expectedStrokeWidth, strokeWidth);
            Assert::IsNotNull(strokeStyle);
            Assert::AreEqual(D2D1_LINE_JOIN_MITER_OR_BEVEL, m_deviceContext->m_factory->m_lineJoin);
            drawRoundedRectangleCalled = true;
        };

        auto canvasStrokeStyle = Make<CanvasStrokeStyle>();
        canvasStrokeStyle->put_LineJoin(CanvasLineJoin::MiterOrBevel);

        ThrowIfFailed(m_canvasDrawingSession->DrawRoundedRectangleWithStrokeWidthAndStrokeStyle(
            expectedRoundedRect,
            m_brush.Get(),
            expectedStrokeWidth,
            canvasStrokeStyle.Get()));

        Assert::IsTrue(drawRoundedRectangleCalled);
    }

    //
    // FillRoundedRectangle
    //

    TEST_METHOD(CanvasDrawingSession_FillRoundedRectangle_NullBrush)
    {
        Assert::AreEqual(E_INVALIDARG, m_canvasDrawingSession->FillRoundedRectangle(CanvasRoundedRectangle{}, nullptr));
    }

    TEST_METHOD(CanvasDrawingSession_FillRoundedRectangle)
    {
        CanvasRoundedRectangle expectedRoundedRect{Rect{1,2,3,4},5,6};

        bool fillRoundedRectangleCalled = false;
        m_deviceContext->MockFillRoundedRectangle =
            [&](const D2D1_ROUNDED_RECT* rect, ID2D1Brush* brush)
            {
                Assert::IsFalse(fillRoundedRectangleCalled);
                Assert::AreEqual(ToD2DRoundedRect(expectedRoundedRect), *rect);
                Assert::AreEqual(m_brush->GetD2DBrush().Get(), brush);
                fillRoundedRectangleCalled = true;
            };

        ThrowIfFailed(m_canvasDrawingSession->FillRoundedRectangle(
            expectedRoundedRect,
            m_brush.Get()));

        Assert::IsTrue(fillRoundedRectangleCalled);
    }

    //
    // DrawEllipse
    //

    TEST_METHOD(CanvasDrawingSession_DrawEllipse_NullBrush)
    {
        Assert::AreEqual(E_INVALIDARG, m_canvasDrawingSession->DrawEllipse(CanvasEllipse{}, nullptr));
    }
    
    TEST_METHOD(CanvasDrawingSession_DrawEllipse)
    {
        CanvasEllipse expectedEllipse{Point{1,2},3,4};
        float expectedStrokeWidth = 1.0f;

        bool drawEllipseCalled = false;
        m_deviceContext->MockDrawEllipse =
            [&](const D2D1_ELLIPSE* ellipse, ID2D1Brush* brush, float strokeWidth, ID2D1StrokeStyle* strokeStyle)
            {
                Assert::IsFalse(drawEllipseCalled);
                Assert::AreEqual(*ReinterpretAs<D2D1_ELLIPSE*>(&expectedEllipse), *ellipse);
                Assert::AreEqual(m_brush->GetD2DBrush().Get(), brush);
                Assert::AreEqual(expectedStrokeWidth, strokeWidth);
                Assert::IsNull(strokeStyle);
                drawEllipseCalled = true;
            };

        ThrowIfFailed(m_canvasDrawingSession->DrawEllipse(
            expectedEllipse,
            m_brush.Get()));

        Assert::IsTrue(drawEllipseCalled);
    }

    //
    // DrawEllipseWithStrokeWidth
    //

    TEST_METHOD(CanvasDrawingSession_DrawEllipseWithStrokeWidth_NullBrush)
    {
        Assert::AreEqual(E_INVALIDARG, m_canvasDrawingSession->DrawEllipseWithStrokeWidth(CanvasEllipse{}, nullptr, 1.0f));
    }

    TEST_METHOD(CanvasDrawingSession_DrawEllipseWithStrokeWidth)
    {
        CanvasEllipse expectedEllipse{Point{1,2},3,4};
        float expectedStrokeWidth = 5.0f;

        bool drawEllipseCalled = false;
        m_deviceContext->MockDrawEllipse =
            [&](const D2D1_ELLIPSE* ellipse, ID2D1Brush* brush, float strokeWidth, ID2D1StrokeStyle* strokeStyle)
            {
                Assert::IsFalse(drawEllipseCalled);
                Assert::AreEqual(*ReinterpretAs<D2D1_ELLIPSE*>(&expectedEllipse), *ellipse);
                Assert::AreEqual(m_brush->GetD2DBrush().Get(), brush);
                Assert::AreEqual(expectedStrokeWidth, strokeWidth);
                Assert::IsNull(strokeStyle);
                drawEllipseCalled = true;
            };

        ThrowIfFailed(m_canvasDrawingSession->DrawEllipseWithStrokeWidth(
            expectedEllipse,
            m_brush.Get(),
            expectedStrokeWidth));

        Assert::IsTrue(drawEllipseCalled);
    }

    //
    // DrawEllipseWithStrokeWidthAndStrokeStyle
    //

    TEST_METHOD(CanvasDrawingSession_DrawEllipseWithStrokeWidthAndStrokeStyle_NullStrokeStyle)
    {
        Assert::AreEqual(E_INVALIDARG, m_canvasDrawingSession->DrawEllipseWithStrokeWidthAndStrokeStyle(CanvasEllipse{}, nullptr, 1.0f, nullptr));
    }

    TEST_METHOD(CanvasDrawingSession_DrawEllipseWithStrokeWidthAndStrokeStyle)
    {
        using canvas::CanvasStrokeStyle;

        CanvasEllipse expectedEllipse{ Point{ 1, 2 }, 3, 4 };
        float expectedStrokeWidth = 5.0f;

        bool drawEllipseCalled = false;
        m_deviceContext->MockDrawEllipse =
            [&](const D2D1_ELLIPSE* ellipse, ID2D1Brush* brush, float strokeWidth, ID2D1StrokeStyle* strokeStyle)
        {
            Assert::IsFalse(drawEllipseCalled);
            Assert::AreEqual(*ReinterpretAs<D2D1_ELLIPSE*>(&expectedEllipse), *ellipse);
            Assert::AreEqual(m_brush->GetD2DBrush().Get(), brush);
            Assert::AreEqual(expectedStrokeWidth, strokeWidth);
            Assert::IsNotNull(strokeStyle);
            Assert::AreEqual(D2D1_LINE_JOIN_MITER_OR_BEVEL, m_deviceContext->m_factory->m_lineJoin);
            drawEllipseCalled = true;
        };

        auto canvasStrokeStyle = Make<CanvasStrokeStyle>();
        canvasStrokeStyle->put_LineJoin(CanvasLineJoin::MiterOrBevel);

        ThrowIfFailed(m_canvasDrawingSession->DrawEllipseWithStrokeWidthAndStrokeStyle(
            expectedEllipse,
            m_brush.Get(),
            expectedStrokeWidth,
            canvasStrokeStyle.Get()));

        Assert::IsTrue(drawEllipseCalled);
    }

    //
    // FillEllipse
    //

    TEST_METHOD(CanvasDrawingSession_FillEllipse_NullBrush)
    {
        Assert::AreEqual(E_INVALIDARG, m_canvasDrawingSession->FillEllipse(CanvasEllipse{}, nullptr));
    }

    TEST_METHOD(CanvasDrawingSession_FillEllipse)
    {
        CanvasEllipse expectedEllipse{Point{1,2},3,4};

        bool fillEllipseCalled = false;
        m_deviceContext->MockFillEllipse =
            [&](const D2D1_ELLIPSE* ellipse, ID2D1Brush* brush)
            {
                Assert::IsFalse(fillEllipseCalled);
                Assert::AreEqual(*ReinterpretAs<D2D1_ELLIPSE*>(&expectedEllipse), *ellipse);
                Assert::AreEqual(m_brush->GetD2DBrush().Get(), brush);
                fillEllipseCalled = true;
            };

        ThrowIfFailed(m_canvasDrawingSession->FillEllipse(
            expectedEllipse,
            m_brush.Get()));

        Assert::IsTrue(fillEllipseCalled);
    }
};

TEST_CLASS(CanvasDrawingSession_CloseTests)
{
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
        auto manager = std::make_shared<CanvasDrawingSessionManager>();
        auto canvasDrawingSession = manager->Create(
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
        using namespace ABI::Windows::UI;
        using namespace ABI::Windows::Foundation;

#define EXPECT_OBJECT_CLOSED(CODE) Assert::AreEqual(RO_E_CLOSED, CODE)

        EXPECT_OBJECT_CLOSED(canvasDrawingSession->Clear(Color{}));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->DrawLine(Point{}, Point{}, nullptr));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->DrawLineWithStrokeWidth(Point{}, Point{}, nullptr, 0.0f));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->DrawRectangle(Rect{}, nullptr));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->DrawRectangleWithStrokeWidth(Rect{}, nullptr, 0.0f));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->FillRectangle(Rect{}, nullptr));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->DrawRoundedRectangle(CanvasRoundedRectangle{}, nullptr));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->DrawRoundedRectangleWithStrokeWidth(CanvasRoundedRectangle{}, nullptr, 0.0f));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->FillRoundedRectangle(CanvasRoundedRectangle{}, nullptr));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->DrawEllipse(CanvasEllipse{}, nullptr));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->DrawEllipseWithStrokeWidth(CanvasEllipse{}, nullptr, 0.0f));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->FillEllipse(CanvasEllipse{}, nullptr));

#undef EXPECT_OBJECT_CLOSED
    }
};

TEST_CLASS(CanvasDrawingSession_Interop)
{
    TEST_METHOD(CanvasDrawingSession_Wrapper_DoesNotAutomaticallyCallAnyMethods)
    {
        auto manager = std::make_shared<CanvasDrawingSessionManager>();
        
        // This mock device context will cause the test to fail if BeginDraw()
        // or EndDraw() is called on it.  When wrapping a drawing session like
        // this we don't want these methods to be called automatically.
        auto deviceContext = Make<MockD2DDeviceContext>();

        auto drawingSession = manager->GetOrCreate(deviceContext.Get());

        // Check one method - we assume that the previous tests are enough to
        // exercise all the methods appropriately.
        bool clearCalled = false;
        deviceContext->MockClear =
            [&](const D2D1_COLOR_F*)
            {
                Assert::IsFalse(clearCalled);
                clearCalled = true;
            };

        ThrowIfFailed(drawingSession->Clear(ABI::Windows::UI::Color{1,2,3,4}));
        Assert::IsTrue(clearCalled);

        // Closing the drawing session should not result in any methods on the
        // deviceContext getting called.
        ThrowIfFailed(drawingSession->Close());
    }
};
