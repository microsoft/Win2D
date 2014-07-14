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


class CanvasDrawingSessionFixture
{
public:
    ComPtr<StubD2DDeviceContextWithGetFactory> DeviceContext;
    ComPtr<canvas::CanvasDrawingSession> DS;
    ComPtr<StubCanvasBrush> Brush;

    CanvasDrawingSessionFixture()
    {
        using canvas::CanvasDrawingSession;

        DeviceContext = Make<StubD2DDeviceContextWithGetFactory>();

        auto manager = std::make_shared<CanvasDrawingSessionManager>();
        DS = manager->Create(
            DeviceContext.Get(),
            std::make_shared<StubCanvasDrawingSessionAdapter>());

        Brush = Make<StubCanvasBrush>();
    }
};

TEST_CLASS(CanvasDrawingSessionUnitTests)
{
public:
    //
    // Clear
    //

    TEST_METHOD(CanvasDrawingSession_Clear)
    {
        CanvasDrawingSessionFixture f;

        ABI::Windows::UI::Color expectedColor{1,2,3,4};

        bool clearCalled = false;
        f.DeviceContext->MockClear =
            [&](const D2D1_COLOR_F* color)
            {
                Assert::IsFalse(clearCalled);
                Assert::AreEqual(expectedColor.A, NormalizedToUint8(color->a));
                Assert::AreEqual(expectedColor.R, NormalizedToUint8(color->r));
                Assert::AreEqual(expectedColor.G, NormalizedToUint8(color->g));
                Assert::AreEqual(expectedColor.B, NormalizedToUint8(color->b));
                clearCalled = true;
            };

        ThrowIfFailed(f.DS->Clear(expectedColor));
        Assert::IsTrue(clearCalled);
    }

    //
    // DrawLine
    //

    TEST_METHOD(CanvasDrawingSession_DrawLine_NullBrush)
    {
        CanvasDrawingSessionFixture f;

        Assert::AreEqual(E_INVALIDARG, f.DS->DrawLine(Point{}, Point{}, nullptr));
    }

    TEST_METHOD(CanvasDrawingSession_DrawLine)
    {
        CanvasDrawingSessionFixture f;

        Point expectedP0{1.0f,2.0f};
        Point expectedP1{3.0f,4.0f};
        float expectedStrokeWidth = 1.0f;

        bool drawLineCalled = false;
        f.DeviceContext->MockDrawLine = 
            [&](D2D1_POINT_2F p0, D2D1_POINT_2F p1, ID2D1Brush* brush, float strokeWidth, ID2D1StrokeStyle* strokeStyle)
            {
                Assert::IsFalse(drawLineCalled);
                Assert::AreEqual(expectedP0.X, p0.x);
                Assert::AreEqual(expectedP0.Y, p0.y);
                Assert::AreEqual(expectedP1.X, p1.x);
                Assert::AreEqual(expectedP1.Y, p1.y);
                Assert::AreEqual(f.Brush->GetD2DBrush().Get(), brush);
                Assert::AreEqual(expectedStrokeWidth, strokeWidth);
                Assert::IsNull(strokeStyle);
                drawLineCalled = true;
            };

        ThrowIfFailed(f.DS->DrawLine(
            expectedP0,
            expectedP1,
            f.Brush.Get()));

        Assert::IsTrue(drawLineCalled);
    }

    //
    // DrawLineWithStrokeWidth
    //

    TEST_METHOD(CanvasDrawingSession_DrawLineWithStrokeWidth_NullBrush)
    {
        CanvasDrawingSessionFixture f;

        Assert::AreEqual(E_INVALIDARG, f.DS->DrawLineWithStrokeWidth(Point{}, Point{}, nullptr, 0.0f));
    }

    TEST_METHOD(CanvasDrawingSession_DrawLineWithStrokeWidth)
    {
        CanvasDrawingSessionFixture f;

        Point expectedP0{1.0f,2.0f};
        Point expectedP1{3.0f,4.0f};
        float expectedStrokeWidth = 5.0f;

        bool drawLineCalled = false;
        f.DeviceContext->MockDrawLine = 
            [&](D2D1_POINT_2F p0, D2D1_POINT_2F p1, ID2D1Brush* brush, float strokeWidth, ID2D1StrokeStyle* strokeStyle)
            {
                Assert::IsFalse(drawLineCalled);
                Assert::AreEqual(ToD2DPoint(expectedP0), p0);
                Assert::AreEqual(ToD2DPoint(expectedP1), p1);
                Assert::AreEqual(f.Brush->GetD2DBrush().Get(), brush);
                Assert::AreEqual(expectedStrokeWidth, strokeWidth);
                Assert::IsNull(strokeStyle);
                drawLineCalled = true;
            };

        ThrowIfFailed(f.DS->DrawLineWithStrokeWidth(
            expectedP0,
            expectedP1,
            f.Brush.Get(),
            expectedStrokeWidth));

        Assert::IsTrue(drawLineCalled);
    }

    //
    // DrawLineWithStrokeWidthAndStrokeStyle
    //

    TEST_METHOD(CanvasDrawingSession_DrawLineWithStrokeWidthAndStrokeStyle_NullStrokeStyle)
    {
        CanvasDrawingSessionFixture f;

        Assert::AreEqual(E_INVALIDARG, f.DS->DrawLineWithStrokeWidthAndStrokeStyle(Point{}, Point{}, nullptr, 0.0f, nullptr));
    }

    TEST_METHOD(CanvasDrawingSession_DrawLineWithStrokeWidthAndStrokeStyle)
    {
        CanvasDrawingSessionFixture f;

        using canvas::CanvasStrokeStyle;

        Point expectedP0{ 1.0f, 2.0f };
        Point expectedP1{ 3.0f, 4.0f };
        float expectedStrokeWidth = 5.0f;

        bool drawLineCalled = false;
        f.DeviceContext->MockDrawLine =
            [&](D2D1_POINT_2F p0, D2D1_POINT_2F p1, ID2D1Brush* brush, float strokeWidth, ID2D1StrokeStyle* strokeStyle)
        {
            Assert::IsFalse(drawLineCalled);
            Assert::AreEqual(ToD2DPoint(expectedP0), p0);
            Assert::AreEqual(ToD2DPoint(expectedP1), p1);
            Assert::AreEqual(f.Brush->GetD2DBrush().Get(), brush);
            Assert::AreEqual(expectedStrokeWidth, strokeWidth);
            Assert::IsNotNull(strokeStyle);
            Assert::AreEqual(D2D1_LINE_JOIN_MITER_OR_BEVEL, f.DeviceContext->m_factory->m_lineJoin);
            drawLineCalled = true;
        };

        auto canvasStrokeStyle = Make<CanvasStrokeStyle>();
        canvasStrokeStyle->put_LineJoin(CanvasLineJoin::MiterOrBevel);

        ThrowIfFailed(f.DS->DrawLineWithStrokeWidthAndStrokeStyle(
            expectedP0,
            expectedP1,
            f.Brush.Get(),
            expectedStrokeWidth,
            canvasStrokeStyle.Get()));

        Assert::IsTrue(drawLineCalled);
    }

    //
    // DrawRectangle
    //

    TEST_METHOD(CanvasDrawingSession_DrawRectangle_NullBrush)
    {
        CanvasDrawingSessionFixture f;

        Assert::AreEqual(E_INVALIDARG, f.DS->DrawRectangle(Rect{}, nullptr));
    }

    TEST_METHOD(CanvasDrawingSession_DrawRectangle)
    {
        CanvasDrawingSessionFixture f;

        Rect expectedRect{1,2,3,4};
        float expectedStrokeWidth = 1.0f;

        bool drawRectangleCalled = false;
        f.DeviceContext->MockDrawRectangle =
            [&](const D2D_RECT_F* rect, ID2D1Brush* brush, float strokeWidth, ID2D1StrokeStyle* strokeStyle)
            {
                Assert::IsFalse(drawRectangleCalled);
                Assert::AreEqual(ToD2DRect(expectedRect), *rect);
                Assert::AreEqual(f.Brush->GetD2DBrush().Get(), brush);
                Assert::AreEqual(expectedStrokeWidth, strokeWidth);
                Assert::IsNull(strokeStyle);
                drawRectangleCalled = true;
            };

        ThrowIfFailed(f.DS->DrawRectangle(
            expectedRect,
            f.Brush.Get()));

        Assert::IsTrue(drawRectangleCalled);
    }

    //
    // DrawRectangleWithStrokeWidth
    //

    TEST_METHOD(CanvasDrawingSession_DrawRectangleWithStrokeWidth_NullBrush)
    {
        CanvasDrawingSessionFixture f;

        Assert::AreEqual(E_INVALIDARG, f.DS->DrawRectangleWithStrokeWidth(Rect{}, nullptr, 0.0f));
    }

    TEST_METHOD(CanvasDrawingSession_DrawRectangleWithStrokeWidth)
    {
        CanvasDrawingSessionFixture f;

        Rect expectedRect{1,2,3,4};
        float expectedStrokeWidth = 5.0f;

        bool drawRectangleCalled = false;
        f.DeviceContext->MockDrawRectangle =
            [&](const D2D_RECT_F* rect, ID2D1Brush* brush, float strokeWidth, ID2D1StrokeStyle* strokeStyle)
            {
                Assert::IsFalse(drawRectangleCalled);
                Assert::AreEqual(ToD2DRect(expectedRect), *rect);
                Assert::AreEqual(f.Brush->GetD2DBrush().Get(), brush);
                Assert::AreEqual(expectedStrokeWidth, strokeWidth);
                Assert::IsNull(strokeStyle);
                drawRectangleCalled = true;
            };

        ThrowIfFailed(f.DS->DrawRectangleWithStrokeWidth(
            expectedRect,
            f.Brush.Get(),
            expectedStrokeWidth));

        Assert::IsTrue(drawRectangleCalled);
    }

    //
    // DrawRectangleWithStrokeWidthAndStrokeStyle
    //

    TEST_METHOD(CanvasDrawingSession_DrawRectangleWithStrokeWidthAndStrokeStyle_NullStrokeStyle)
    {
        CanvasDrawingSessionFixture f;

        Assert::AreEqual(E_INVALIDARG, f.DS->DrawRectangleWithStrokeWidthAndStrokeStyle(Rect{}, nullptr, 0.0f, nullptr));
    }

    TEST_METHOD(CanvasDrawingSession_DrawRectangleWithStrokeWidthAndStrokeStyle)
    {
        CanvasDrawingSessionFixture f;

        using canvas::CanvasStrokeStyle;

        Rect expectedRect{ 1, 2, 3, 4 };
        float expectedStrokeWidth = 5.0f;

        bool drawRectangleCalled = false;
        f.DeviceContext->MockDrawRectangle =
            [&](const D2D_RECT_F* rect, ID2D1Brush* brush, float strokeWidth, ID2D1StrokeStyle* strokeStyle)
        {
            Assert::IsFalse(drawRectangleCalled);
            Assert::AreEqual(ToD2DRect(expectedRect), *rect);
            Assert::AreEqual(f.Brush->GetD2DBrush().Get(), brush);
            Assert::AreEqual(expectedStrokeWidth, strokeWidth);
            Assert::IsNotNull(strokeStyle);
            Assert::AreEqual(D2D1_LINE_JOIN_MITER_OR_BEVEL, f.DeviceContext->m_factory->m_lineJoin);
            drawRectangleCalled = true;
        };

        auto canvasStrokeStyle = Make<CanvasStrokeStyle>();
        canvasStrokeStyle->put_LineJoin(CanvasLineJoin::MiterOrBevel);

        ThrowIfFailed(f.DS->DrawRectangleWithStrokeWidthAndStrokeStyle(
            expectedRect,
            f.Brush.Get(),
            expectedStrokeWidth,
            canvasStrokeStyle.Get()));

        Assert::IsTrue(drawRectangleCalled);
    }

    //
    // FillRectangle
    //

    TEST_METHOD(CanvasDrawingSession_FillRectangle_NullBrush)
    {
        CanvasDrawingSessionFixture f;

        Assert::AreEqual(E_INVALIDARG, f.DS->FillRectangle(Rect{}, nullptr));
    }

    TEST_METHOD(CanvasDrawingSession_FillRectangle)
    {
        CanvasDrawingSessionFixture f;

        Rect expectedRect{1,2,3,4};

        bool fillRectangleCalled = false;
        f.DeviceContext->MockFillRectangle =
            [&](const D2D1_RECT_F* rect, ID2D1Brush* brush)
            {
                Assert::IsFalse(fillRectangleCalled);
                Assert::AreEqual(ToD2DRect(expectedRect), *rect);
                Assert::AreEqual(f.Brush->GetD2DBrush().Get(), brush);
                fillRectangleCalled = true;
            };

        ThrowIfFailed(f.DS->FillRectangle(
            expectedRect,
            f.Brush.Get()));

        Assert::IsTrue(fillRectangleCalled);
    }

    //
    // DrawRoundedRectangle
    //

    TEST_METHOD(CanvasDrawingSession_DrawRoundedRectangle_NullBrush)
    {
        CanvasDrawingSessionFixture f;

        Assert::AreEqual(E_INVALIDARG, f.DS->DrawRoundedRectangle(CanvasRoundedRectangle{}, nullptr));
    }

    TEST_METHOD(CanvasDrawingSession_DrawRoundedRectangle)
    {
        CanvasDrawingSessionFixture f;

        CanvasRoundedRectangle expectedRoundedRect{Rect{1,2,3,4},5,6};
        float expectedStrokeWidth = 1.0f;

        bool drawRoundedRectangleCalled = false;
        f.DeviceContext->MockDrawRoundedRectangle =
            [&](const D2D1_ROUNDED_RECT* rect, ID2D1Brush* brush, float strokeWidth, ID2D1StrokeStyle* strokeStyle)
            {
                Assert::IsFalse(drawRoundedRectangleCalled);
                Assert::AreEqual(ToD2DRoundedRect(expectedRoundedRect), *rect);
                Assert::AreEqual(f.Brush->GetD2DBrush().Get(), brush);
                Assert::AreEqual(expectedStrokeWidth, strokeWidth);
                Assert::IsNull(strokeStyle);
                drawRoundedRectangleCalled = true;
            };

        ThrowIfFailed(f.DS->DrawRoundedRectangle(
            expectedRoundedRect,
            f.Brush.Get()));

        Assert::IsTrue(drawRoundedRectangleCalled);
    }

    //
    // DrawRoundedRectangleWithStrokeWidth
    //

    TEST_METHOD(CanvasDrawingSession_DrawRoundedRectangleWithStrokeWidth_NullBrush)
    {
        CanvasDrawingSessionFixture f;

        Assert::AreEqual(E_INVALIDARG, f.DS->DrawRoundedRectangleWithStrokeWidth(CanvasRoundedRectangle{}, nullptr, 1.0f));
    }

    TEST_METHOD(CanvasDrawingSession_DrawRoundedRectangleWithStrokeWidth)
    {
        CanvasDrawingSessionFixture f;

        CanvasRoundedRectangle expectedRoundedRect{Rect{1,2,3,4},5,6};
        float expectedStrokeWidth = 5.0f;

        bool drawRoundedRectangleCalled = false;
        f.DeviceContext->MockDrawRoundedRectangle =
            [&](const D2D1_ROUNDED_RECT* rect, ID2D1Brush* brush, float strokeWidth, ID2D1StrokeStyle* strokeStyle)
            {
                Assert::IsFalse(drawRoundedRectangleCalled);
                Assert::AreEqual(ToD2DRoundedRect(expectedRoundedRect), *rect);
                Assert::AreEqual(f.Brush->GetD2DBrush().Get(), brush);
                Assert::AreEqual(expectedStrokeWidth, strokeWidth);
                Assert::IsNull(strokeStyle);
                drawRoundedRectangleCalled = true;
            };

        ThrowIfFailed(f.DS->DrawRoundedRectangleWithStrokeWidth(
            expectedRoundedRect,
            f.Brush.Get(),
            expectedStrokeWidth));

        Assert::IsTrue(drawRoundedRectangleCalled);
    }

    //
    // DrawRoundedRectangleWithStrokeWidthAndStrokeStyle
    //

    TEST_METHOD(CanvasDrawingSession_DrawRoundedRectangleWithStrokeWidthAndStrokeStyle_NullStrokeStyle)
    {
        CanvasDrawingSessionFixture f;

        Assert::AreEqual(E_INVALIDARG, f.DS->DrawRoundedRectangleWithStrokeWidthAndStrokeStyle(CanvasRoundedRectangle{}, nullptr, 1.0f, nullptr));
    }

    TEST_METHOD(CanvasDrawingSession_DrawRoundedRectangleWithStrokeWidthAndStrokeStyle)
    {
        CanvasDrawingSessionFixture f;

        using canvas::CanvasStrokeStyle;

        CanvasRoundedRectangle expectedRoundedRect{ Rect{ 1, 2, 3, 4 }, 5, 6 };
        float expectedStrokeWidth = 5.0f;

        bool drawRoundedRectangleCalled = false;
        f.DeviceContext->MockDrawRoundedRectangle =
            [&](const D2D1_ROUNDED_RECT* rect, ID2D1Brush* brush, float strokeWidth, ID2D1StrokeStyle* strokeStyle)
        {
            Assert::IsFalse(drawRoundedRectangleCalled);
            Assert::AreEqual(ToD2DRoundedRect(expectedRoundedRect), *rect);
            Assert::AreEqual(f.Brush->GetD2DBrush().Get(), brush);
            Assert::AreEqual(expectedStrokeWidth, strokeWidth);
            Assert::IsNotNull(strokeStyle);
            Assert::AreEqual(D2D1_LINE_JOIN_MITER_OR_BEVEL, f.DeviceContext->m_factory->m_lineJoin);
            drawRoundedRectangleCalled = true;
        };

        auto canvasStrokeStyle = Make<CanvasStrokeStyle>();
        canvasStrokeStyle->put_LineJoin(CanvasLineJoin::MiterOrBevel);

        ThrowIfFailed(f.DS->DrawRoundedRectangleWithStrokeWidthAndStrokeStyle(
            expectedRoundedRect,
            f.Brush.Get(),
            expectedStrokeWidth,
            canvasStrokeStyle.Get()));

        Assert::IsTrue(drawRoundedRectangleCalled);
    }

    //
    // FillRoundedRectangle
    //

    TEST_METHOD(CanvasDrawingSession_FillRoundedRectangle_NullBrush)
    {
        CanvasDrawingSessionFixture f;

        Assert::AreEqual(E_INVALIDARG, f.DS->FillRoundedRectangle(CanvasRoundedRectangle{}, nullptr));
    }

    TEST_METHOD(CanvasDrawingSession_FillRoundedRectangle)
    {
        CanvasDrawingSessionFixture f;

        CanvasRoundedRectangle expectedRoundedRect{Rect{1,2,3,4},5,6};

        bool fillRoundedRectangleCalled = false;
        f.DeviceContext->MockFillRoundedRectangle =
            [&](const D2D1_ROUNDED_RECT* rect, ID2D1Brush* brush)
            {
                Assert::IsFalse(fillRoundedRectangleCalled);
                Assert::AreEqual(ToD2DRoundedRect(expectedRoundedRect), *rect);
                Assert::AreEqual(f.Brush->GetD2DBrush().Get(), brush);
                fillRoundedRectangleCalled = true;
            };

        ThrowIfFailed(f.DS->FillRoundedRectangle(
            expectedRoundedRect,
            f.Brush.Get()));

        Assert::IsTrue(fillRoundedRectangleCalled);
    }

    //
    // DrawEllipse
    //

    TEST_METHOD(CanvasDrawingSession_DrawEllipse_NullBrush)
    {
        CanvasDrawingSessionFixture f;

        Assert::AreEqual(E_INVALIDARG, f.DS->DrawEllipse(CanvasEllipse{}, nullptr));
    }
    
    TEST_METHOD(CanvasDrawingSession_DrawEllipse)
    {
        CanvasDrawingSessionFixture f;

        CanvasEllipse expectedEllipse{Point{1,2},3,4};
        float expectedStrokeWidth = 1.0f;

        bool drawEllipseCalled = false;
        f.DeviceContext->MockDrawEllipse =
            [&](const D2D1_ELLIPSE* ellipse, ID2D1Brush* brush, float strokeWidth, ID2D1StrokeStyle* strokeStyle)
            {
                Assert::IsFalse(drawEllipseCalled);
                Assert::AreEqual(*ReinterpretAs<D2D1_ELLIPSE*>(&expectedEllipse), *ellipse);
                Assert::AreEqual(f.Brush->GetD2DBrush().Get(), brush);
                Assert::AreEqual(expectedStrokeWidth, strokeWidth);
                Assert::IsNull(strokeStyle);
                drawEllipseCalled = true;
            };

        ThrowIfFailed(f.DS->DrawEllipse(
            expectedEllipse,
            f.Brush.Get()));

        Assert::IsTrue(drawEllipseCalled);
    }

    //
    // DrawEllipseWithStrokeWidth
    //

    TEST_METHOD(CanvasDrawingSession_DrawEllipseWithStrokeWidth_NullBrush)
    {
        CanvasDrawingSessionFixture f;

        Assert::AreEqual(E_INVALIDARG, f.DS->DrawEllipseWithStrokeWidth(CanvasEllipse{}, nullptr, 1.0f));
    }

    TEST_METHOD(CanvasDrawingSession_DrawEllipseWithStrokeWidth)
    {
        CanvasDrawingSessionFixture f;

        CanvasEllipse expectedEllipse{Point{1,2},3,4};
        float expectedStrokeWidth = 5.0f;

        bool drawEllipseCalled = false;
        f.DeviceContext->MockDrawEllipse =
            [&](const D2D1_ELLIPSE* ellipse, ID2D1Brush* brush, float strokeWidth, ID2D1StrokeStyle* strokeStyle)
            {
                Assert::IsFalse(drawEllipseCalled);
                Assert::AreEqual(*ReinterpretAs<D2D1_ELLIPSE*>(&expectedEllipse), *ellipse);
                Assert::AreEqual(f.Brush->GetD2DBrush().Get(), brush);
                Assert::AreEqual(expectedStrokeWidth, strokeWidth);
                Assert::IsNull(strokeStyle);
                drawEllipseCalled = true;
            };

        ThrowIfFailed(f.DS->DrawEllipseWithStrokeWidth(
            expectedEllipse,
            f.Brush.Get(),
            expectedStrokeWidth));

        Assert::IsTrue(drawEllipseCalled);
    }

    //
    // DrawEllipseWithStrokeWidthAndStrokeStyle
    //

    TEST_METHOD(CanvasDrawingSession_DrawEllipseWithStrokeWidthAndStrokeStyle_NullStrokeStyle)
    {
        CanvasDrawingSessionFixture f;

        Assert::AreEqual(E_INVALIDARG, f.DS->DrawEllipseWithStrokeWidthAndStrokeStyle(CanvasEllipse{}, nullptr, 1.0f, nullptr));
    }

    TEST_METHOD(CanvasDrawingSession_DrawEllipseWithStrokeWidthAndStrokeStyle)
    {
        CanvasDrawingSessionFixture f;

        using canvas::CanvasStrokeStyle;

        CanvasEllipse expectedEllipse{ Point{ 1, 2 }, 3, 4 };
        float expectedStrokeWidth = 5.0f;

        bool drawEllipseCalled = false;
        f.DeviceContext->MockDrawEllipse =
            [&](const D2D1_ELLIPSE* ellipse, ID2D1Brush* brush, float strokeWidth, ID2D1StrokeStyle* strokeStyle)
        {
            Assert::IsFalse(drawEllipseCalled);
            Assert::AreEqual(*ReinterpretAs<D2D1_ELLIPSE*>(&expectedEllipse), *ellipse);
            Assert::AreEqual(f.Brush->GetD2DBrush().Get(), brush);
            Assert::AreEqual(expectedStrokeWidth, strokeWidth);
            Assert::IsNotNull(strokeStyle);
            Assert::AreEqual(D2D1_LINE_JOIN_MITER_OR_BEVEL, f.DeviceContext->m_factory->m_lineJoin);
            drawEllipseCalled = true;
        };

        auto canvasStrokeStyle = Make<CanvasStrokeStyle>();
        canvasStrokeStyle->put_LineJoin(CanvasLineJoin::MiterOrBevel);

        ThrowIfFailed(f.DS->DrawEllipseWithStrokeWidthAndStrokeStyle(
            expectedEllipse,
            f.Brush.Get(),
            expectedStrokeWidth,
            canvasStrokeStyle.Get()));

        Assert::IsTrue(drawEllipseCalled);
    }

    //
    // FillEllipse
    //

    TEST_METHOD(CanvasDrawingSession_FillEllipse_NullBrush)
    {
        CanvasDrawingSessionFixture f;

        Assert::AreEqual(E_INVALIDARG, f.DS->FillEllipse(CanvasEllipse{}, nullptr));
    }

    TEST_METHOD(CanvasDrawingSession_FillEllipse)
    {
        CanvasDrawingSessionFixture f;

        CanvasEllipse expectedEllipse{Point{1,2},3,4};

        bool fillEllipseCalled = false;
        f.DeviceContext->MockFillEllipse =
            [&](const D2D1_ELLIPSE* ellipse, ID2D1Brush* brush)
            {
                Assert::IsFalse(fillEllipseCalled);
                Assert::AreEqual(*ReinterpretAs<D2D1_ELLIPSE*>(&expectedEllipse), *ellipse);
                Assert::AreEqual(f.Brush->GetD2DBrush().Get(), brush);
                fillEllipseCalled = true;
            };

        ThrowIfFailed(f.DS->FillEllipse(
            expectedEllipse,
            f.Brush.Get()));

        Assert::IsTrue(fillEllipseCalled);
    }
};

TEST_CLASS(CanvasDrawingSession_DrawTextTests)
{
    class Fixture : public CanvasDrawingSessionFixture
    {
        bool m_drawTextCalled;

    public:
        ComPtr<canvas::CanvasTextFormat> Format;

        Fixture()
            : m_drawTextCalled(false)
        {
            Format = Make<canvas::CanvasTextFormat>();
        }

        template<typename FORMAT_VALIDATOR>
        void ExpectOnce(std::wstring expectedText, D2D1_RECT_F expectedRect, D2D1_DRAW_TEXT_OPTIONS expectedOptions, FORMAT_VALIDATOR&& formatValidator)
        {            
            DeviceContext->MockDrawText =
                [=](const wchar_t* actualText,
                    uint32_t actualTextLength,
                    IDWriteTextFormat* format,
                    D2D1_RECT_F actualRect,
                    ID2D1Brush* actualBrush,
                    D2D1_DRAW_TEXT_OPTIONS actualOptions,
                    DWRITE_MEASURING_MODE)
                {
                    Assert::IsFalse(m_drawTextCalled);
                    m_drawTextCalled = true;

                    Assert::AreEqual(expectedText.c_str(), actualText);
                    Assert::AreEqual<uint32_t>(expectedText.size(), actualTextLength);
                    Assert::IsNotNull(format);
                    Assert::AreEqual(expectedRect, actualRect);
                    Assert::AreEqual(expectedOptions, actualOptions);
                    formatValidator(format);
                };
        }

        void Check()
        {
            Assert::IsTrue(m_drawTextCalled);
        }
    };

    //
    // DrawTextAtPoint
    //

    TEST_METHOD(CanvasDrawingSession_DrawTextAtPoint_NullParameters)
    {
        Fixture f;

        Assert::AreEqual(E_INVALIDARG, f.DS->DrawTextAtPoint(nullptr, Point{}, f.Brush.Get()));
        Assert::AreEqual(E_INVALIDARG, f.DS->DrawTextAtPoint(WinString(L"hello"), Point{}, nullptr));
    }

    TEST_METHOD(CanvasDrawingSession_DrawTextAtPoint)
    {
        Fixture f;

        auto expectedText = L"hello, world";
        Point expectedPoint{12,34};
        f.ExpectOnce(
            expectedText,
            D2D1_RECT_F{expectedPoint.X, expectedPoint.Y, expectedPoint.X, expectedPoint.Y},
            D2D1_DRAW_TEXT_OPTIONS_NONE, 
            [](IDWriteTextFormat* format)
            {
                Assert::AreEqual(DWRITE_WORD_WRAPPING_NO_WRAP, format->GetWordWrapping());
            });

        ThrowIfFailed(f.DS->DrawTextAtPoint(
            WinString(expectedText),
            expectedPoint,
            f.Brush.Get()));

        f.Check();
    }

    //
    // DrawTextAtPointWithFormat
    //

    TEST_METHOD(CanvasDrawingSession_DrawTextAtPointWithFormat_NullParameters)
    {
        Fixture f;

        Assert::AreEqual(E_INVALIDARG, f.DS->DrawTextAtPointWithFormat(nullptr, Point{}, f.Brush.Get(), f.Format.Get()));
        Assert::AreEqual(E_INVALIDARG, f.DS->DrawTextAtPointWithFormat(WinString(), Point{}, nullptr, f.Format.Get()));
        Assert::AreEqual(E_INVALIDARG, f.DS->DrawTextAtPointWithFormat(WinString(), Point{}, f.Brush.Get(), nullptr));
    }

    TEST_METHOD(CanvasDrawingSession_DrawTextAtPointWithFormat)
    {
        Fixture f;
        
        CanvasWordWrapping expectedWordWrapping = CanvasWordWrapping::WholeWord; 
        ThrowIfFailed(f.Format->put_WordWrapping(expectedWordWrapping));

        auto expectedFontFamilyName = L"Ariel";
        ThrowIfFailed(f.Format->put_FontFamilyName(WinString(expectedFontFamilyName)));

        ThrowIfFailed(f.Format->put_Options(CanvasDrawTextOptions::Clip));

        auto expectedText = L"goodbye cruel world";
        Point expectedPoint{12,34};
        f.ExpectOnce(
            expectedText,
            D2D1_RECT_F{expectedPoint.X, expectedPoint.Y, expectedPoint.X, expectedPoint.Y},
            D2D1_DRAW_TEXT_OPTIONS_CLIP,
            [&](IDWriteTextFormat* format)
            {
                Assert::AreEqual(DWRITE_WORD_WRAPPING_NO_WRAP, format->GetWordWrapping());
                
                auto length = format->GetFontFamilyNameLength() + 1; // + 1 for NULL terminator
                std::vector<wchar_t> buf(length);
                ThrowIfFailed(format->GetFontFamilyName(&buf.front(), length));
                auto actualFontFamilyName = std::wstring(buf.begin(), buf.end() - 1); // - 1 since we don't want NULL terminator in string

                Assert::AreEqual(expectedFontFamilyName, actualFontFamilyName.c_str());
            });

        ThrowIfFailed(f.DS->DrawTextAtPointWithFormat(
            WinString(expectedText),
            expectedPoint,
            f.Brush.Get(),
            f.Format.Get()));

        f.Check();

        // After the call we expect the word wrapping to be how we left it
        CanvasWordWrapping actualWordWrapping{};
        ThrowIfFailed(f.Format->get_WordWrapping(&actualWordWrapping));
        Assert::AreEqual(expectedWordWrapping, actualWordWrapping);
    }

    //
    // DrawText
    //

    TEST_METHOD(CanvasDrawingSession_DrawText_NullParameters)
    {
        Fixture f;

        Assert::AreEqual(E_INVALIDARG, f.DS->DrawText(nullptr, Rect{}, f.Brush.Get()));
        Assert::AreEqual(E_INVALIDARG, f.DS->DrawText(WinString(), Rect{}, nullptr));
    }

    TEST_METHOD(CanvasDrawingSession_DrawText)
    {
        Fixture f;

        auto expectedText = L"If you don't eat your meat";
        Rect expectedRect{1,2,3,4};
        f.ExpectOnce(
            expectedText,
            ToD2DRect(expectedRect),
            D2D1_DRAW_TEXT_OPTIONS_NONE,
            [&](IDWriteTextFormat*){});

        ThrowIfFailed(f.DS->DrawText(
            WinString(expectedText),
            expectedRect,
            f.Brush.Get()));

        f.Check();
    }

    //
    // DrawTextWithFormat
    //

    TEST_METHOD(CanvasDrawingSession_DrawTextWithFormat_NullParameters)
    {
        Fixture f;

        Assert::AreEqual(E_INVALIDARG, f.DS->DrawTextWithFormat(nullptr, Rect{}, f.Brush.Get(), f.Format.Get()));
        Assert::AreEqual(E_INVALIDARG, f.DS->DrawTextWithFormat(WinString(), Rect{}, nullptr, f.Format.Get()));
        Assert::AreEqual(E_INVALIDARG, f.DS->DrawTextWithFormat(WinString(), Rect{}, f.Brush.Get(), nullptr));
    }

    TEST_METHOD(CanvasDrawingSession_DrawTextWithFormat)
    {
        Fixture f;
        
        CanvasWordWrapping expectedWordWrapping = CanvasWordWrapping::WholeWord; 
        ThrowIfFailed(f.Format->put_WordWrapping(expectedWordWrapping));

        auto expectedFontFamilyName = L"Ariel";
        ThrowIfFailed(f.Format->put_FontFamilyName(WinString(expectedFontFamilyName)));

        ThrowIfFailed(f.Format->put_Options(CanvasDrawTextOptions::Clip));

        auto expectedText = L"how can you have any pudding?";
        Rect expectedRect{1,2,3,4};
        f.ExpectOnce(
            expectedText,
            ToD2DRect(expectedRect),
            D2D1_DRAW_TEXT_OPTIONS_CLIP,
            [&](IDWriteTextFormat* format)
            {
                Assert::AreEqual(DWRITE_WORD_WRAPPING_WHOLE_WORD, format->GetWordWrapping());
                
                auto length = format->GetFontFamilyNameLength() + 1; // + 1 for NULL terminator
                std::vector<wchar_t> buf(length);
                ThrowIfFailed(format->GetFontFamilyName(&buf.front(), length));
                auto actualFontFamilyName = std::wstring(buf.begin(), buf.end() - 1); // - 1 since we don't want NULL terminator in string

                Assert::AreEqual(expectedFontFamilyName, actualFontFamilyName.c_str());
            });

        ThrowIfFailed(f.DS->DrawTextWithFormat(
            WinString(expectedText),
            expectedRect,
            f.Brush.Get(),
            f.Format.Get()));

        f.Check();

        // After the call we expect the word wrapping to be how we left it
        CanvasWordWrapping actualWordWrapping{};
        ThrowIfFailed(f.Format->get_WordWrapping(&actualWordWrapping));
        Assert::AreEqual(expectedWordWrapping, actualWordWrapping);
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
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->DrawTextAtPoint(nullptr, Point{}, nullptr));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->DrawTextAtPointWithFormat(nullptr, Point{}, nullptr, nullptr));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->DrawText(nullptr, Rect{}, nullptr));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->DrawTextWithFormat(nullptr, Rect{}, nullptr, nullptr));

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
