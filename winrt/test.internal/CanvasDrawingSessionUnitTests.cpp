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

#include "StubD2DResources.h"

#include "CanvasBitmap.h"
#include "effects\GaussianBlurEffect.h"
#include "TestBitmapResourceCreationAdapter.h"
#include "MockWICFormatConverter.h"

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

        virtual D2D1_POINT_2F GetRenderingSurfaceOffset() override
        {
            Assert::IsFalse(m_endDrawCalled);
            return D2D1::Point2F(0, 0);
        }
    };

    ComPtr<ID2D1DeviceContext1> m_expectedDeviceContext;
    std::shared_ptr<MockCanvasDrawingSessionAdapter> m_mockAdapter;
    ComPtr<CanvasDrawingSession> m_canvasDrawingSession;

public:
    TEST_METHOD_INITIALIZE(Init)
    {
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

class CanvasDrawingSessionFixture
{
public:
    ComPtr<StubD2DDeviceContextWithGetFactory> DeviceContext;
    ComPtr<CanvasDrawingSession> DS;
    ComPtr<StubCanvasBrush> Brush;

    CanvasDrawingSessionFixture()
    {
        DeviceContext = Make<StubD2DDeviceContextWithGetFactory>();

        auto manager = std::make_shared<CanvasDrawingSessionManager>();
        DS = manager->Create(
            DeviceContext.Get(),
            std::make_shared<StubCanvasDrawingSessionAdapter>());

        Brush = Make<StubCanvasBrush>();
    }
};

class CanvasDrawingSessionAdapter_ChangeableOffset : public StubCanvasDrawingSessionAdapter
{
public:
    D2D1_POINT_2F m_offset;

    CanvasDrawingSessionAdapter_ChangeableOffset()
        : m_offset(D2D1::Point2F(0, 0)) {}

    virtual D2D1_POINT_2F GetRenderingSurfaceOffset() override
    {
        return m_offset;
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
    // DrawImage
    //
    TEST_METHOD(CanvasDrawingSession_DrawImage_NullImage)
    {
        CanvasDrawingSessionFixture f;

        Assert::AreEqual(E_INVALIDARG, f.DS->DrawImage(nullptr));
    }

    TEST_METHOD(CanvasDrawingSession_DrawImage_Bitmap)
    {
        CanvasDrawingSessionFixture f;

        int testWidth = 16;
        int testHeight = 32;

        WinString testFileName(L"fakeFileName.jpg");

        auto converter = Make<MockWICFormatConverter>();
        auto adapter = std::make_shared<TestBitmapResourceCreationAdapter>(converter);

        ComPtr<MockD2DBitmap>  bitmap = Make<MockD2DBitmap>();

        ComPtr<StubCanvasDevice> canvasDevice = Make<StubCanvasDevice>();
        canvasDevice->MockCreateBitmap = 
            [&]() -> ComPtr<ID2D1Bitmap1>
            {
                return bitmap;
            };

        ComPtr<CanvasBitmap> canvasBitmap = Make<CanvasBitmap>(canvasDevice.Get(), testFileName, adapter);

        ComPtr<ICanvasImageInternal> internalImage;
        ThrowIfFailed(canvasBitmap.As(&internalImage));

        bool drawImageCalled = false;
        f.DeviceContext->MockDrawImage = 
            [&](ID2D1Image* image)
            {
                Assert::IsFalse(drawImageCalled);
                Assert::IsNotNull(image);
                Assert::AreEqual(internalImage->GetD2DImage(f.DeviceContext.Get()).Get(), image);
                drawImageCalled = true;
            };

        ThrowIfFailed(f.DS->DrawImage(canvasBitmap.Get()));
        Assert::IsTrue(drawImageCalled);
    }

    TEST_METHOD(CanvasDrawingSession_DrawImage_GaussianBlurEffect)
    {
        CanvasDrawingSessionFixture f;

        WinString testFileName(L"fakeFileName.jpg");

        auto converter = Make<MockWICFormatConverter>();
        auto adapter = std::make_shared<TestBitmapResourceCreationAdapter>(converter);

        ComPtr<MockD2DBitmap>  mockBitmap = Make<MockD2DBitmap>();
        ComPtr<MockD2DEffect> mockEffect = Make<MockD2DEffect>();
        
        bool setInputCalled = false;
        mockEffect->MockSetInput = 
            [&]
            {
                Assert::IsFalse(setInputCalled);
                setInputCalled = true;
            };

        bool setValueCalled = false;
        mockEffect->MockSetValue = 
            [&]
            {
                setValueCalled = true;
                return S_OK;
            };

        ComPtr<StubCanvasDevice> canvasDevice = Make<StubCanvasDevice>();
        bool createBitmapCalled = false;
        canvasDevice->MockCreateBitmap =
            [&]() -> ComPtr<ID2D1Bitmap1>
            {
                Assert::IsFalse(createBitmapCalled);
                createBitmapCalled = true;
                return mockBitmap;
            };


        ComPtr<CanvasBitmap> canvasBitmap = Make<CanvasBitmap>(canvasDevice.Get(), testFileName, adapter);
        ComPtr<Effects::GaussianBlurEffect> blurEffect = Make<Effects::GaussianBlurEffect>();
        
        ThrowIfFailed(blurEffect->put_Source(canvasBitmap.Get()));

        bool drawImageCalled = false;
        f.DeviceContext->MockDrawImage =
            [&](ID2D1Image* image)
            {
                Assert::IsFalse(drawImageCalled);
                Assert::IsNotNull(image);
                drawImageCalled = true;
            };

        f.DeviceContext->MockCreateEffect = 
            [&](ID2D1Effect** effect)
            {
                return mockEffect.CopyTo(effect);
            };

        ThrowIfFailed(blurEffect->put_StandardDeviation(5.0f));

        ThrowIfFailed(f.DS->DrawImage(blurEffect.Get()));
        Assert::IsTrue(drawImageCalled);
        Assert::IsTrue(setInputCalled);
        Assert::IsTrue(setValueCalled);
        // Make sure mock bitmap created as input for blur effect
        Assert::IsTrue(createBitmapCalled);
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


    TEST_METHOD(CanvasDrawingSession_Circle_NullArgs)
    {
        CanvasDrawingSessionFixture f;

        Assert::AreEqual(E_INVALIDARG, f.DS->DrawCircle(Point{}, 0.0f, nullptr));

        Assert::AreEqual(E_INVALIDARG, f.DS->DrawCircleWithStrokeWidth(Point{}, 0.0f, nullptr, 0.0f));

        Assert::AreEqual(E_INVALIDARG, f.DS->DrawCircleWithStrokeWidthAndStrokeStyle(Point{}, 0.0f, nullptr, 0.0f, nullptr));

        Assert::AreEqual(E_INVALIDARG, f.DS->FillCircle(Point{}, 0.0f, nullptr));
    }

    void VerifyD2DEllipseMatchesCircle(
        const D2D1_ELLIPSE* ellipse,
        const Point& expectedCenterPoint,
        float expectedRadius)
    {
        Assert::AreEqual(ellipse->point.x, expectedCenterPoint.X);
        Assert::AreEqual(ellipse->point.y, expectedCenterPoint.Y);
        Assert::AreEqual(ellipse->radiusX, expectedRadius);
        Assert::AreEqual(ellipse->radiusY, expectedRadius);
    }

    TEST_METHOD(CanvasDrawingSession_FillCircle)
    {
        CanvasDrawingSessionFixture f;
        const Point expectedCenterPoint = { 33, 44 };
        const float expectedRadius = 5;

        bool fillEllipseCalled = false;
        f.DeviceContext->MockFillEllipse =
            [&](const D2D1_ELLIPSE* ellipse, ID2D1Brush* brush)
            {
                Assert::IsFalse(fillEllipseCalled);
                VerifyD2DEllipseMatchesCircle(ellipse, expectedCenterPoint, expectedRadius);
                Assert::AreEqual(f.Brush->GetD2DBrush().Get(), brush);
                fillEllipseCalled = true;
            };

        ThrowIfFailed(f.DS->FillCircle(
            expectedCenterPoint,
            expectedRadius,
            f.Brush.Get()));

        Assert::IsTrue(fillEllipseCalled);
    }

    TEST_METHOD(CanvasDrawingSession_DrawCircle)
    {
        CanvasDrawingSessionFixture f;
        const Point expectedCenterPoint = { 33, 44 };
        const float expectedRadius = 5;

        bool drawEllipseCalled = false;
        f.DeviceContext->MockDrawEllipse =
            [&](const D2D1_ELLIPSE* ellipse, ID2D1Brush* brush, float strokeWidth, ID2D1StrokeStyle* strokeStyle)
            {
                Assert::IsFalse(drawEllipseCalled);
                VerifyD2DEllipseMatchesCircle(ellipse, expectedCenterPoint, expectedRadius);
                Assert::AreEqual(f.Brush->GetD2DBrush().Get(), brush);
                Assert::AreEqual(1.0f, strokeWidth);
                Assert::IsNull(strokeStyle);
                drawEllipseCalled = true;
            };

        ThrowIfFailed(f.DS->DrawCircle(expectedCenterPoint, expectedRadius,f.Brush.Get()));

        Assert::IsTrue(drawEllipseCalled);
    }

    TEST_METHOD(CanvasDrawingSession_DrawCircleWithStrokeWidth)
    {
        CanvasDrawingSessionFixture f;
        const Point expectedCenterPoint = { 33, 44 };
        const float expectedRadius = 5;
        const float expectedStrokeWidth = 11.11f;

        bool drawEllipseCalled = false;
        f.DeviceContext->MockDrawEllipse =
            [&](const D2D1_ELLIPSE* ellipse, ID2D1Brush* brush, float strokeWidth, ID2D1StrokeStyle* strokeStyle)
        {
            Assert::IsFalse(drawEllipseCalled);
            VerifyD2DEllipseMatchesCircle(ellipse, expectedCenterPoint, expectedRadius);
            Assert::AreEqual(f.Brush->GetD2DBrush().Get(), brush);
            Assert::AreEqual(expectedStrokeWidth, strokeWidth);
            Assert::IsNull(strokeStyle);
            drawEllipseCalled = true;
        };

        ThrowIfFailed(f.DS->DrawCircleWithStrokeWidth(expectedCenterPoint, expectedRadius, f.Brush.Get(), expectedStrokeWidth));

        Assert::IsTrue(drawEllipseCalled);
    }

    TEST_METHOD(CanvasDrawingSession_DrawCircleWithStrokeWidthAndStrokeStyle)
    {
        CanvasDrawingSessionFixture f;
        const Point expectedCenterPoint = { 33, 44 };
        const float expectedRadius = 5;
        const float expectedStrokeWidth = 11.11f;

        bool drawEllipseCalled = false;
        f.DeviceContext->MockDrawEllipse =
            [&](const D2D1_ELLIPSE* ellipse, ID2D1Brush* brush, float strokeWidth, ID2D1StrokeStyle* strokeStyle)
        {
            Assert::IsFalse(drawEllipseCalled);
            VerifyD2DEllipseMatchesCircle(ellipse, expectedCenterPoint, expectedRadius);
            Assert::AreEqual(f.Brush->GetD2DBrush().Get(), brush);
            Assert::AreEqual(expectedStrokeWidth, strokeWidth);
            Assert::IsNotNull(strokeStyle);
            Assert::AreEqual(D2D1_LINE_JOIN_MITER_OR_BEVEL, f.DeviceContext->m_factory->m_lineJoin);
            drawEllipseCalled = true;
        };

        auto canvasStrokeStyle = Make<CanvasStrokeStyle>();
        canvasStrokeStyle->put_LineJoin(CanvasLineJoin::MiterOrBevel);

        ThrowIfFailed(f.DS->DrawCircleWithStrokeWidthAndStrokeStyle(
            expectedCenterPoint, 
            expectedRadius, 
            f.Brush.Get(), 
            expectedStrokeWidth,
            canvasStrokeStyle.Get()));

        Assert::IsTrue(drawEllipseCalled);
    }

    TEST_METHOD(CanvasDrawingSession_StateGettersWithNull)
    {
        CanvasDrawingSessionFixture f;

        Assert::AreEqual(E_INVALIDARG, f.DS->get_Antialiasing(nullptr));
        Assert::AreEqual(E_INVALIDARG, f.DS->get_Blend(nullptr));
        Assert::AreEqual(E_INVALIDARG, f.DS->get_TextAntialiasing(nullptr));
        Assert::AreEqual(E_INVALIDARG, f.DS->get_Transform(nullptr));
        Assert::AreEqual(E_INVALIDARG, f.DS->get_Units(nullptr));
    }

    TEST_METHOD(CanvasDrawingSession_StateProperties)
    {
        CanvasDrawingSessionFixture f;

        {
            bool propertyFunctionCalled = false;
            f.DeviceContext->MockSetAntialiasMode =
                [&](const D2D1_ANTIALIAS_MODE m)
            {
                Assert::IsFalse(propertyFunctionCalled);
                propertyFunctionCalled = true;
                Assert::AreEqual(m, D2D1_ANTIALIAS_MODE_ALIASED);
            };
            ThrowIfFailed(f.DS->put_Antialiasing(CanvasAntialiasing_Aliased));
            Assert::IsTrue(propertyFunctionCalled);
        }

        {
            bool propertyFunctionCalled = false;
            f.DeviceContext->MockGetAntialiasMode =
                [&]
            {
                Assert::IsFalse(propertyFunctionCalled);
                propertyFunctionCalled = true;
                return D2D1_ANTIALIAS_MODE_ALIASED;
            };
            CanvasAntialiasing antialiasMode;
            ThrowIfFailed(f.DS->get_Antialiasing(&antialiasMode));
            Assert::IsTrue(propertyFunctionCalled);
            Assert::AreEqual(CanvasAntialiasing_Aliased, antialiasMode);
        }

        {
            bool propertyFunctionCalled = false;
            f.DeviceContext->MockSetPrimitiveBlend =
                [&](const D2D1_PRIMITIVE_BLEND b)
            {
                Assert::IsFalse(propertyFunctionCalled);
                propertyFunctionCalled = true;
                Assert::AreEqual(b, D2D1_PRIMITIVE_BLEND_MIN);
            };
            ThrowIfFailed(f.DS->put_Blend(CanvasBlend_Min));
            Assert::IsTrue(propertyFunctionCalled);
        }

        {
            bool propertyFunctionCalled = false;
            f.DeviceContext->MockGetPrimitiveBlend =
                [&]
            {
                Assert::IsFalse(propertyFunctionCalled);
                propertyFunctionCalled = true;
                return D2D1_PRIMITIVE_BLEND_COPY;
            };
            CanvasBlend blend;
            ThrowIfFailed(f.DS->get_Blend(&blend));
            Assert::IsTrue(propertyFunctionCalled);
            Assert::AreEqual(CanvasBlend_Copy, blend);
        }

        {
            bool propertyFunctionCalled = false;
            f.DeviceContext->MockSetTextAntialiasMode =
                [&](const D2D1_TEXT_ANTIALIAS_MODE m)
            {
                Assert::IsFalse(propertyFunctionCalled);
                propertyFunctionCalled = true;
                Assert::AreEqual(m, D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE);
            };
            ThrowIfFailed(f.DS->put_TextAntialiasing(CanvasTextAntialiasing_ClearType));
            Assert::IsTrue(propertyFunctionCalled);
        }

        {
            bool propertyFunctionCalled = false;
            f.DeviceContext->MockGetTextAntialiasMode =
                [&]
            {
                Assert::IsFalse(propertyFunctionCalled);
                propertyFunctionCalled = true;
                return D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE;
            };
            CanvasTextAntialiasing textAntialiasing;
            ThrowIfFailed(f.DS->get_TextAntialiasing(&textAntialiasing));
            Assert::IsTrue(propertyFunctionCalled);
            Assert::AreEqual(CanvasTextAntialiasing_Grayscale, textAntialiasing);
        }

        {
            bool propertyFunctionCalled = false;
            Numerics::Matrix3x2 matrix = { 1, 2, 3, 5, 8, 13 };
            f.DeviceContext->MockSetTransform =
                [&](const D2D1_MATRIX_3X2_F* m)
            {
                Assert::IsFalse(propertyFunctionCalled);
                propertyFunctionCalled = true;

                const D2D1_MATRIX_3X2_F* asD2DMatrix = reinterpret_cast<D2D1_MATRIX_3X2_F*>(&matrix);
                Assert::AreEqual(*asD2DMatrix, *m);
            };
            ThrowIfFailed(f.DS->put_Transform(matrix));
            Assert::IsTrue(propertyFunctionCalled);
        }

        {
            bool propertyFunctionCalled = false;
            Numerics::Matrix3x2 matrix = { 13, 8, 5, 3, 2, 1 };
            f.DeviceContext->MockGetTransform =
                [&](D2D1_MATRIX_3X2_F* m)
            {
                Assert::IsFalse(propertyFunctionCalled);
                propertyFunctionCalled = true;
                const D2D1_MATRIX_3X2_F* asD2DMatrix = reinterpret_cast<D2D1_MATRIX_3X2_F*>(&matrix);
                *m = *asD2DMatrix;
            };
            Numerics::Matrix3x2 transform;
            ThrowIfFailed(f.DS->get_Transform(&transform));
            Assert::IsTrue(propertyFunctionCalled);
            Assert::AreEqual(matrix, transform);
        }

        {
            bool propertyFunctionCalled = false;
            f.DeviceContext->MockSetUnitMode =
                [&](const D2D1_UNIT_MODE m)
            {
                Assert::IsFalse(propertyFunctionCalled);
                propertyFunctionCalled = true;
                Assert::AreEqual(m, D2D1_UNIT_MODE_PIXELS);
            };
            ThrowIfFailed(f.DS->put_Units(CanvasUnits_Pixels));
            Assert::IsTrue(propertyFunctionCalled);
        }

        {
            bool propertyFunctionCalled = false;
            f.DeviceContext->MockGetUnitMode =
                [&]
            {
                Assert::IsFalse(propertyFunctionCalled);
                propertyFunctionCalled = true;
                return D2D1_UNIT_MODE_PIXELS;
            };
            CanvasUnits units;
            ThrowIfFailed(f.DS->get_Units(&units));
            Assert::IsTrue(propertyFunctionCalled);
            Assert::AreEqual(CanvasUnits_Pixels, units);
        }
    }

    TEST_METHOD(CanvasDrawingSession_SiSOffsetIsHiddenFromTransformProperty)
    {
        ComPtr<StubD2DDeviceContextWithGetFactory> deviceContext = 
            Make<StubD2DDeviceContextWithGetFactory>();

        auto manager = std::make_shared<CanvasDrawingSessionManager>();
        auto adapter = std::make_shared<CanvasDrawingSessionAdapter_ChangeableOffset>();
        auto drawingSession = manager->Create(deviceContext.Get(), adapter);

        //
        // The adapter sets the native transform to the offset on BeginDraw. 
        // This test doesn't verify that - the test CanvasImageSourceDrawingSessionAdapter_BeginEndDraw 
        // does. This test just replicates the behavior.
        //
        adapter->m_offset = D2D1::Point2F(1, 1);
        D2D1_MATRIX_3X2_F nativeTransform = D2D1::Matrix3x2F::Translation(
            adapter->m_offset.x,
            adapter->m_offset.y);

        deviceContext->MockSetTransform =
            [&](const D2D1_MATRIX_3X2_F* m)
            {
                nativeTransform = *m;
            };

        deviceContext->MockGetTransform =
            [&](D2D1_MATRIX_3X2_F* m)
            {
                *m = nativeTransform;
            };
        
        Numerics::Matrix3x2 transform;
        ThrowIfFailed(drawingSession->get_Transform(&transform));
        Assert::AreEqual(0.0f, transform.M31);
        Assert::AreEqual(0.0f, transform.M32);

        Numerics::Matrix3x2 interestingTranslation = { 1, 0, 0, 1, 123, 456 };
        ThrowIfFailed(drawingSession->put_Transform(interestingTranslation));

        ThrowIfFailed(drawingSession->get_Transform(&transform));
        Assert::AreEqual(interestingTranslation, transform);

        // Verify that the native resource out of interop, too, reflects the SiS offset.
        ComPtr<ID2D1DeviceContext> nativeResource = drawingSession->GetResource();
        D2D1_MATRIX_3X2_F wrappedResourceTransform;
        nativeResource->GetTransform(&wrappedResourceTransform);
        D2D1_MATRIX_3X2_F expectedTransform = D2D1::Matrix3x2F(1, 0, 0, 1, 123 + adapter->m_offset.x, 456 + adapter->m_offset.y);
        Assert::AreEqual(expectedTransform, wrappedResourceTransform);
    }

    TEST_METHOD(CanvasDrawingSession_get_Device)
    {
        //
        // In the interop case, where NULL is passed to the drawing session
        // manager, get_Device should create a new D2D device, and a new 
        // CanvasDevice on the fly. 
        //
        // In the non-interop case, the device is passed to the manager and
        // the drawing session keeps a reference to it.
        //
        // That resource domains of of get_Device is not verified here for
        // the interop case. They are verified in the external test 
        // CanvasDrawingSession_Interop_get_Device.
        //

        ComPtr<StubD2DDeviceContextWithGetFactory> d2dDeviceContext =
            Make<StubD2DDeviceContextWithGetFactory>();

        ComPtr<StubD2DFactoryWithCreateStrokeStyle> d2dFactory = Make<StubD2DFactoryWithCreateStrokeStyle>();
        d2dDeviceContext->m_factory = d2dFactory;

        bool getDeviceCalled = false;
        d2dDeviceContext->MockGetDevice =
            [&](ID2D1Device** d2dDevice)
            {
                getDeviceCalled = true;
                ComPtr<StubD2DDevice> stubDevice = Make<StubD2DDevice>();
                stubDevice.CopyTo(d2dDevice);
            };

        ComPtr<StubCanvasDevice> stubCanvasDevice = Make<StubCanvasDevice>();

        struct TestCase
        {
            ICanvasDevice* ManagerDevice;
            bool ExpectGetDeviceCalled;

        } testCases[] =
        {
            { nullptr, true },
            { stubCanvasDevice.Get(), false }
        };

        for (size_t i = 0; i < _countof(testCases); ++i)
        {
            getDeviceCalled = false;

            auto manager = std::make_shared<CanvasDrawingSessionManager>();
            ComPtr<CanvasDrawingSession> drawingSession = manager->Create(
                testCases[i].ManagerDevice,
                d2dDeviceContext.Get(),
                std::make_shared<StubCanvasDrawingSessionAdapter>());

            ComPtr<ICanvasDevice> deviceVerify;
            ThrowIfFailed(drawingSession->get_Device(&deviceVerify));

            Assert::IsNotNull(deviceVerify.Get());
            Assert::AreEqual(testCases[i].ExpectGetDeviceCalled, getDeviceCalled);

            if (testCases[i].ManagerDevice)
            {
                Assert::AreEqual(testCases[i].ManagerDevice, deviceVerify.Get());
            }

            // Do get_Device again. Even in the interop case, it should never 
            // cause GetDevice to be called again.
            getDeviceCalled = false;
            ComPtr<ICanvasDevice> deviceReverify;
            ThrowIfFailed(drawingSession->get_Device(&deviceReverify));

            Assert::IsFalse(getDeviceCalled);
            Assert::AreEqual(deviceVerify.Get(), deviceReverify.Get());
        }
    }
};

TEST_CLASS(CanvasDrawingSession_DrawTextTests)
{
    class Fixture : public CanvasDrawingSessionFixture
    {
        bool m_drawTextCalled;

    public:
        ComPtr<CanvasTextFormat> Format;

        Fixture()
            : m_drawTextCalled(false)
        {
            Format = Make<CanvasTextFormat>();
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
        ThrowIfFailed(f.Format->put_FontFamily(WinString(expectedFontFamilyName)));

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
        ThrowIfFailed(f.Format->put_FontFamily(WinString(expectedFontFamilyName)));

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

        ComPtr<ICanvasDevice> deviceVerify;

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
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->DrawEllipseWithStrokeWidthAndStrokeStyle(CanvasEllipse{}, nullptr, 0.0f, nullptr));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->FillEllipse(CanvasEllipse{}, nullptr));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->DrawCircle(Point{}, 0.0f, nullptr));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->DrawCircleWithStrokeWidth(Point{}, 0.0f, nullptr, 0.0f));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->DrawCircleWithStrokeWidthAndStrokeStyle(Point{}, 0.0f, nullptr, 0.0f, nullptr));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->FillCircle(Point{}, 0.0f, nullptr));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->DrawTextAtPoint(nullptr, Point{}, nullptr));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->DrawTextAtPointWithFormat(nullptr, Point{}, nullptr, nullptr));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->DrawText(nullptr, Rect{}, nullptr));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->DrawTextWithFormat(nullptr, Rect{}, nullptr, nullptr));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->get_Antialiasing(nullptr));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->put_Antialiasing(CanvasAntialiasing::Aliased));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->get_Blend(nullptr));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->put_Blend(CanvasBlend::SourceOver));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->get_TextAntialiasing(nullptr));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->put_TextAntialiasing(CanvasTextAntialiasing::Default));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->get_Transform(nullptr));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->put_Transform(Numerics::Matrix3x2()));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->get_Units(nullptr));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->put_Units(CanvasUnits::Dips));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->get_Device(&deviceVerify));


#undef EXPECT_OBJECT_CLOSED
    }

    TEST_METHOD(CanvasDrawingSession_Implements_ExpectedInterfaces)
    {
        CanvasDrawingSessionFixture f;

        ASSERT_IMPLEMENTS_INTERFACE(f.DS, ICanvasResourceCreator);
    }

    TEST_METHOD(CanvasDrawingSession_get_Device_NullArg)
    {
        CanvasDrawingSessionFixture f;

        Assert::AreEqual(E_INVALIDARG, f.DS->get_Device(nullptr));
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
