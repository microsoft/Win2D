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

#include "StubCanvasBrush.h"
#include "effects\generated\GaussianBlurEffect.h"
#include "MockD2DRectangleGeometry.h"
#include "CanvasCachedGeometry.h"
#include "MockD2DGeometryRealization.h"

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

    struct Fixture
    {
        ComPtr<ID2D1DeviceContext1> m_expectedDeviceContext;
        std::shared_ptr<MockCanvasDrawingSessionAdapter> m_mockAdapter;
        ComPtr<CanvasDrawingSession> m_canvasDrawingSession;

        Fixture()
        {
            m_expectedDeviceContext = Make<MockD2DDeviceContext>();
            m_mockAdapter = std::make_shared<MockCanvasDrawingSessionAdapter>();
            
            auto manager = std::make_shared<CanvasDrawingSessionManager>();
            m_canvasDrawingSession = manager->Create(m_expectedDeviceContext.Get(), m_mockAdapter);
        }
    };

    TEST_METHOD_EX(CanvasDrawingSession_Calls_Adapter_BeginDraw_EndDraw_When_Closed)
    {
        Fixture f;

        ThrowIfFailed(f.m_canvasDrawingSession->Close());
        f.m_mockAdapter->AssertEndDrawCalled();
        
        // EndDraw should only be called once even if we call Close again
        ThrowIfFailed(f.m_canvasDrawingSession->Close());
    }

    TEST_METHOD_EX(CanvasDrawingSession_Calls_Adapter_BeginDraw_EndDraw_When_Destroyed)
    {
        Fixture f;

        f.m_canvasDrawingSession.Reset();
        f.m_mockAdapter->AssertEndDrawCalled();
    }

    TEST_METHOD_EX(CanvasDrawingSession_Close_EndDrawErrorsReturned)
    {
        Fixture f;

        f.m_mockAdapter->SetEndDrawToThrow();
        Assert::AreEqual(DXGI_ERROR_DEVICE_REMOVED, f.m_canvasDrawingSession->Close());
    }

    TEST_METHOD_EX(CanvasDrawingSession_Destroy_EndDrawErrorsSwallowed)
    {
        Fixture f;

        f.m_mockAdapter->SetEndDrawToThrow();
        f.m_canvasDrawingSession.Reset();
        f.m_mockAdapter->AssertEndDrawCalled();
    }    
};

class CanvasDrawingSessionFixture
{
public:
    ComPtr<StubD2DDeviceContextWithGetFactory> DeviceContext;
    ComPtr<CanvasDrawingSession> DS;
    ComPtr<StubCanvasBrush> Brush;
    ComPtr<CanvasGeometry> Geometry;
    ComPtr<CanvasCachedGeometry> CachedGeometry;

    CanvasDrawingSessionFixture()
        : DeviceContext(Make<StubD2DDeviceContextWithGetFactory>())
        , DS(MakeDrawingSession(DeviceContext.Get()))
        , Brush(Make<StubCanvasBrush>())
    {
        ComPtr<StubCanvasDevice> canvasDevice = Make<StubCanvasDevice>();

        auto geometryManager = std::make_shared<CanvasGeometryManager>();
        Geometry = geometryManager->Create(canvasDevice.Get(), Rect{ 1, 2, 3, 4 });

        auto cachedGeometryManager = std::make_shared<CanvasCachedGeometryManager>();
        CachedGeometry = cachedGeometryManager->Create(canvasDevice.Get(), Geometry.Get(), D2D1_DEFAULT_FLATTENING_TOLERANCE);
    }

private:
    static ComPtr<CanvasDrawingSession> MakeDrawingSession(ID2D1DeviceContext1* deviceContext)
    {
        auto manager = std::make_shared<CanvasDrawingSessionManager>();
        return manager->Create(
            deviceContext,
            std::make_shared<StubCanvasDrawingSessionAdapter>());
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

static Color ArbitraryMarkerColor1{ 1, 23, 3, 42 };
static Color ArbitraryMarkerColor2{ 0xDE, 0xAD, 0xBE, 0xEF };

class BrushValidator
{
    bool m_isColorOverload;
    ID2D1Brush* m_expectedBrush;
    bool m_gotColor2;
    int m_checkCount;

public:
    BrushValidator(CanvasDrawingSessionFixture const& f, bool isColorOverload)
        : m_isColorOverload(isColorOverload),
          m_expectedBrush(nullptr),
          m_gotColor2(false),
          m_checkCount(0)
    {
        if (isColorOverload)
        {
            // When testing a WithColor overload, we expect to get two draw calls.
            // The first draw uses ArbitraryMarkerColor1 and should trigger a call to CreateSolidColorBrush.
            // The second uses ArbitraryMarkerColor2 and should call SetColor on the previously created brush.

            f.DeviceContext->CreateSolidColorBrushMethod.AllowAnyCall(
                [&](const D2D1_COLOR_F* color, const D2D1_BRUSH_PROPERTIES* brushProperties, ID2D1SolidColorBrush** solidColorBrush)
                {
                    // First we should see a brush created using ArbitraryMarkerColor1.
                    Assert::IsNull(m_expectedBrush);
                    Assert::AreEqual(ToD2DColor(ArbitraryMarkerColor1), *color);

                    auto brush = Make<MockD2DSolidColorBrush>();
                    m_expectedBrush = brush.Get();
                    brush.CopyTo(solidColorBrush);

                    // Then we should see SetColor called with ArbitraryMarkerColor2.
                    brush->MockSetColor =
                        [&](const D2D1_COLOR_F* color)
                        {
                            Assert::IsFalse(m_gotColor2);
                            Assert::AreEqual(ToD2DColor(ArbitraryMarkerColor2), *color);
                            m_gotColor2 = true;
                        };

                    return S_OK;
                });
        }
        else
        {
            // When testing a WithBrush overload, we expect to see the stub brush provided by CanvasDrawingSessionFixture.
            m_expectedBrush = f.Brush->GetD2DBrush(nullptr).Get();
        }
    }

    void Check(ID2D1Brush* brush)
    {
        if (m_isColorOverload)
        {
            switch (m_checkCount)
            {
            case 0:
                // During the first draw call, our brush should have been created but its color not changed yet.
                Assert::IsNotNull(m_expectedBrush);
                Assert::IsFalse(m_gotColor2);
                break;

            case 1:
                // During the second draw call, the brush color should have been changed.
                Assert::IsTrue(m_gotColor2);
                break;

            default:
                Assert::Fail();
            }
        }

        Assert::AreEqual(m_expectedBrush, brush);

        m_checkCount++;
    }
};

TEST_CLASS(CanvasDrawingSessionUnitTests)
{
public:
    //
    // Clear
    //

    TEST_METHOD_EX(CanvasDrawingSession_Clear)
    {
        CanvasDrawingSessionFixture f;

        ABI::Windows::UI::Color expectedColor{ 1, 2, 3, 4 };

        f.DeviceContext->ClearMethod.SetExpectedCalls(1,
            [&](const D2D1_COLOR_F* color)
            {
                Assert::AreEqual(expectedColor.A, NormalizedToUint8(color->a));
                Assert::AreEqual(expectedColor.R, NormalizedToUint8(color->r));
                Assert::AreEqual(expectedColor.G, NormalizedToUint8(color->g));
                Assert::AreEqual(expectedColor.B, NormalizedToUint8(color->b));
            });

        ThrowIfFailed(f.DS->Clear(expectedColor));
    }


    //
    // DrawImage
    //

    class BitmapFixture : public CanvasDrawingSessionFixture
    {
    public:
        ComPtr<ID2D1Image> Image;
        ComPtr<CanvasBitmap> Bitmap;
        ComPtr<StubD2DBitmap> StubBitmap;

        BitmapFixture()
        {
            StubBitmap = Make<StubD2DBitmap>();
            Image = As<ID2D1Image>(StubBitmap);
            Bitmap = MakeBitmapManager()->GetOrCreate(nullptr, StubBitmap.Get());
        }

    private:
        static std::shared_ptr<CanvasBitmapManager> MakeBitmapManager()
        {
            auto converter = Make<MockWICFormatConverter>();
            auto adapter = std::make_shared<TestBitmapResourceCreationAdapter>(converter);
            return std::make_shared<CanvasBitmapManager>(adapter);
        }
    };


    TEST_METHOD_EX(CanvasDrawingSession_DrawImage_NullImage)
    {
        BitmapFixture f;

        Assert::AreEqual(E_INVALIDARG, f.DS->DrawImageAtOrigin(nullptr));

        Assert::AreEqual(E_INVALIDARG, f.DS->DrawImageWithSourceRect(nullptr, Vector2{}, Rect{}));
        Assert::AreEqual(E_INVALIDARG, f.DS->DrawImageWithSourceRectAndInterpolation(nullptr, Vector2{}, Rect{}, CanvasImageInterpolation::NearestNeighbor));
        Assert::AreEqual(E_INVALIDARG, f.DS->DrawImageWithSourceRectAndInterpolationAndComposite(nullptr, Vector2{}, Rect{}, CanvasImageInterpolation::NearestNeighbor, CanvasComposite::SourceOver));

        Assert::AreEqual(E_INVALIDARG, f.DS->DrawImageAtCoordsWithSourceRect(nullptr, 0, 0, Rect{}));
        Assert::AreEqual(E_INVALIDARG, f.DS->DrawImageAtCoordsWithSourceRectAndInterpolation(nullptr, 0, 0, Rect{}, CanvasImageInterpolation::NearestNeighbor));
        Assert::AreEqual(E_INVALIDARG, f.DS->DrawImageAtCoordsWithSourceRectAndInterpolationAndComposite(nullptr, 0, 0, Rect{}, CanvasImageInterpolation::NearestNeighbor, CanvasComposite::SourceOver));
    }


    TEST_METHOD_EX(CanvasDrawingSession_DrawImage_NullBitmapImage)
    {
        BitmapFixture f;
        Assert::AreEqual(E_INVALIDARG, f.DS->DrawBitmapWithDestRect(nullptr, Rect{}));
        Assert::AreEqual(E_INVALIDARG, f.DS->DrawBitmapWithDestRectAndSourceRect(nullptr, Rect{}, Rect{}));

        Assert::AreEqual(E_INVALIDARG, f.DS->DrawBitmapWithDestRectAndSourceRectAndOpacity(nullptr, Rect{}, Rect{}, 0));
        Assert::AreEqual(E_INVALIDARG, f.DS->DrawBitmapWithDestRectAndSourceRectAndOpacityAndInterpolation(nullptr, Rect{}, Rect{}, 0, CanvasImageInterpolation::NearestNeighbor));
        Assert::AreEqual(E_INVALIDARG, f.DS->DrawBitmapWithDestRectAndSourceRectAndOpacityAndInterpolationAndPerspective(nullptr, Rect{}, Rect{}, 0, CanvasImageInterpolation::NearestNeighbor, Matrix4x4{}));
    }

    TEST_METHOD_EX(CanvasDrawingSession_DrawImage_BitmapInterpolationModes)
    {
        struct
        {
            CanvasImageInterpolation InterpolationMode;
            bool ExpectDrawBitmapFastPath;
        }
        testPasses[] =
        {
            { CanvasImageInterpolation::NearestNeighbor,   true  },
            { CanvasImageInterpolation::Linear,            true  },
            { CanvasImageInterpolation::Cubic,             false },
            { CanvasImageInterpolation::MultiSampleLinear, false },
            { CanvasImageInterpolation::Anisotropic,       false },
            { CanvasImageInterpolation::HighQualityCubic,  false },
        };

        for (auto& testPass : testPasses)
        {
            BitmapFixture f;

            f.DeviceContext->GetPrimitiveBlendMethod.SetExpectedCalls(1, [] { return D2D1_PRIMITIVE_BLEND_SOURCE_OVER; });

            if (testPass.ExpectDrawBitmapFastPath)
            {
                f.DeviceContext->DrawBitmapMethod.SetExpectedCalls(1,
                    [&](ID2D1Bitmap* bitmap, const D2D1_RECT_F*, FLOAT, D2D1_INTERPOLATION_MODE interpolationMode, const D2D1_RECT_F*, const D2D1_MATRIX_4X4_F*)
                    {
                        Assert::IsTrue(IsSameInstance(f.Image.Get(), bitmap));
                        Assert::AreEqual(testPass.InterpolationMode, (CanvasImageInterpolation)interpolationMode);
                    });
            }
            else
            {
                f.DeviceContext->DrawImageMethod.SetExpectedCalls(1,
                    [&](ID2D1Image* image, D2D1_POINT_2F const*, D2D1_RECT_F const*, D2D1_INTERPOLATION_MODE interpolationMode, D2D1_COMPOSITE_MODE)
                    {
                        Assert::AreEqual(f.Image.Get(), image);
                        Assert::AreEqual(testPass.InterpolationMode, (CanvasImageInterpolation)interpolationMode);
                    });
            }

            ThrowIfFailed(f.DS->DrawImageAtCoordsWithSourceRectAndInterpolationAndComposite(f.Bitmap.Get(), 0, 0, Rect{}, testPass.InterpolationMode, CanvasComposite::SourceOver));
        }
    }

    TEST_METHOD_EX(CanvasDrawingSession_DrawImage_BitmapCompositeModes)
    {
        struct
        {
            CanvasComposite CompositeMode;
            D2D1_PRIMITIVE_BLEND PrimitiveBlend;
            bool ExpectDrawBitmapFastPath;
        }
        testPasses[] =
        {
            { CanvasComposite::SourceOver,      D2D1_PRIMITIVE_BLEND_SOURCE_OVER, true  },
            { CanvasComposite::SourceOver,      D2D1_PRIMITIVE_BLEND_COPY,        false },
            { CanvasComposite::SourceOver,      D2D1_PRIMITIVE_BLEND_MIN,         false },
            { CanvasComposite::SourceOver,      D2D1_PRIMITIVE_BLEND_ADD,         false },

            { CanvasComposite::DestinationOver, D2D1_PRIMITIVE_BLEND_SOURCE_OVER, false },
            { CanvasComposite::DestinationOver, D2D1_PRIMITIVE_BLEND_COPY,        false },
            { CanvasComposite::DestinationOver, D2D1_PRIMITIVE_BLEND_MIN,         false },
            { CanvasComposite::DestinationOver, D2D1_PRIMITIVE_BLEND_ADD,         false },

            { CanvasComposite::SourceIn,        D2D1_PRIMITIVE_BLEND_SOURCE_OVER, false },
            { CanvasComposite::SourceIn,        D2D1_PRIMITIVE_BLEND_COPY,        false },
            { CanvasComposite::SourceIn,        D2D1_PRIMITIVE_BLEND_MIN,         false },
            { CanvasComposite::SourceIn,        D2D1_PRIMITIVE_BLEND_ADD,         false },

            { CanvasComposite::DestinationIn,   D2D1_PRIMITIVE_BLEND_SOURCE_OVER, false },
            { CanvasComposite::DestinationIn,   D2D1_PRIMITIVE_BLEND_COPY,        false },
            { CanvasComposite::DestinationIn,   D2D1_PRIMITIVE_BLEND_MIN,         false },
            { CanvasComposite::DestinationIn,   D2D1_PRIMITIVE_BLEND_ADD,         false },

            { CanvasComposite::SourceOut,       D2D1_PRIMITIVE_BLEND_SOURCE_OVER, false },
            { CanvasComposite::SourceOut,       D2D1_PRIMITIVE_BLEND_COPY,        false },
            { CanvasComposite::SourceOut,       D2D1_PRIMITIVE_BLEND_MIN,         false },
            { CanvasComposite::SourceOut,       D2D1_PRIMITIVE_BLEND_ADD,         false },

            { CanvasComposite::DestinationOut,  D2D1_PRIMITIVE_BLEND_SOURCE_OVER, false },
            { CanvasComposite::DestinationOut,  D2D1_PRIMITIVE_BLEND_COPY,        false },
            { CanvasComposite::DestinationOut,  D2D1_PRIMITIVE_BLEND_MIN,         false },
            { CanvasComposite::DestinationOut,  D2D1_PRIMITIVE_BLEND_ADD,         false },

            { CanvasComposite::SourceAtop,      D2D1_PRIMITIVE_BLEND_SOURCE_OVER, false },
            { CanvasComposite::SourceAtop,      D2D1_PRIMITIVE_BLEND_COPY,        false },
            { CanvasComposite::SourceAtop,      D2D1_PRIMITIVE_BLEND_MIN,         false },
            { CanvasComposite::SourceAtop,      D2D1_PRIMITIVE_BLEND_ADD,         false },

            { CanvasComposite::DestinationAtop, D2D1_PRIMITIVE_BLEND_SOURCE_OVER, false },
            { CanvasComposite::DestinationAtop, D2D1_PRIMITIVE_BLEND_COPY,        false },
            { CanvasComposite::DestinationAtop, D2D1_PRIMITIVE_BLEND_MIN,         false },
            { CanvasComposite::DestinationAtop, D2D1_PRIMITIVE_BLEND_ADD,         false },

            { CanvasComposite::Xor,             D2D1_PRIMITIVE_BLEND_SOURCE_OVER, false },
            { CanvasComposite::Xor,             D2D1_PRIMITIVE_BLEND_COPY,        false },
            { CanvasComposite::Xor,             D2D1_PRIMITIVE_BLEND_MIN,         false },
            { CanvasComposite::Xor,             D2D1_PRIMITIVE_BLEND_ADD,         false },

            { CanvasComposite::Add,             D2D1_PRIMITIVE_BLEND_SOURCE_OVER, false },
            { CanvasComposite::Add,             D2D1_PRIMITIVE_BLEND_COPY,        false },
            { CanvasComposite::Add,             D2D1_PRIMITIVE_BLEND_MIN,         false },
            { CanvasComposite::Add,             D2D1_PRIMITIVE_BLEND_ADD,         true  },

            { CanvasComposite::Copy,            D2D1_PRIMITIVE_BLEND_SOURCE_OVER, false },
            { CanvasComposite::Copy,            D2D1_PRIMITIVE_BLEND_COPY,        true  },
            { CanvasComposite::Copy,            D2D1_PRIMITIVE_BLEND_MIN,         false },
            { CanvasComposite::Copy,            D2D1_PRIMITIVE_BLEND_ADD,         false },

            { CanvasComposite::BoundedCopy,     D2D1_PRIMITIVE_BLEND_SOURCE_OVER, false },
            { CanvasComposite::BoundedCopy,     D2D1_PRIMITIVE_BLEND_COPY,        false },
            { CanvasComposite::BoundedCopy,     D2D1_PRIMITIVE_BLEND_MIN,         false },
            { CanvasComposite::BoundedCopy,     D2D1_PRIMITIVE_BLEND_ADD,         false },

            { CanvasComposite::MaskInvert,      D2D1_PRIMITIVE_BLEND_SOURCE_OVER, false },
            { CanvasComposite::MaskInvert,      D2D1_PRIMITIVE_BLEND_COPY,        false },
            { CanvasComposite::MaskInvert,      D2D1_PRIMITIVE_BLEND_MIN,         false },
            { CanvasComposite::MaskInvert,      D2D1_PRIMITIVE_BLEND_ADD,         false },
        };

        for (auto& testPass : testPasses)
        {
            BitmapFixture f;

            f.DeviceContext->GetPrimitiveBlendMethod.SetExpectedCalls(1, [&] { return testPass.PrimitiveBlend; });

            if (testPass.ExpectDrawBitmapFastPath)
            {
                f.DeviceContext->DrawBitmapMethod.SetExpectedCalls(1,
                    [&](ID2D1Bitmap* bitmap, const D2D1_RECT_F*, FLOAT, D2D1_INTERPOLATION_MODE, const D2D1_RECT_F*, const D2D1_MATRIX_4X4_F*)
                    {
                        Assert::IsTrue(IsSameInstance(f.Image.Get(), bitmap));
                    });
            }
            else
            {
                f.DeviceContext->DrawImageMethod.SetExpectedCalls(1,
                    [&](ID2D1Image* image, D2D1_POINT_2F const*, D2D1_RECT_F const*, D2D1_INTERPOLATION_MODE, D2D1_COMPOSITE_MODE compositeMode)
                    {
                        Assert::AreEqual(f.Image.Get(), image);
                        Assert::AreEqual((D2D1_COMPOSITE_MODE)testPass.CompositeMode, compositeMode);
                    });
            }

            ThrowIfFailed(f.DS->DrawImageAtCoordsWithSourceRectAndInterpolationAndComposite(f.Bitmap.Get(), 0, 0, Rect{}, CanvasImageInterpolation::Linear, testPass.CompositeMode));
        }
    }

    TEST_METHOD_EX(CanvasDrawingSession_DrawImage_InheritsCompositeModeFromPrimitiveBlend)
    {
        struct
        {
            D2D1_PRIMITIVE_BLEND PrimitiveBlend;
            D2D1_COMPOSITE_MODE ExpectedCompositeMode;
        }
        testPasses[] =
        {
            { D2D1_PRIMITIVE_BLEND_SOURCE_OVER, D2D1_COMPOSITE_MODE_SOURCE_OVER },
            { D2D1_PRIMITIVE_BLEND_COPY,        D2D1_COMPOSITE_MODE_SOURCE_COPY },
            { D2D1_PRIMITIVE_BLEND_ADD,         D2D1_COMPOSITE_MODE_PLUS        },
        };

        for (auto& testPass : testPasses)
        {
            BitmapFixture f;

            f.DeviceContext->GetPrimitiveBlendMethod.AllowAnyCall([&] { return testPass.PrimitiveBlend; });

            f.DeviceContext->DrawImageMethod.SetExpectedCalls(1,
                [&](ID2D1Image* image, D2D1_POINT_2F const*, D2D1_RECT_F const*, D2D1_INTERPOLATION_MODE, D2D1_COMPOSITE_MODE compositeMode)
                {
                    Assert::AreEqual(testPass.ExpectedCompositeMode, compositeMode);
                });

            ThrowIfFailed(f.DS->DrawImageAtCoordsWithSourceRectAndInterpolation(f.Bitmap.Get(), 0, 0, Rect{}, CanvasImageInterpolation::Cubic));
        }
    }

    TEST_METHOD_EX(CanvasDrawingSession_DrawImage_ErrorForPrimitiveBlendMin)
    {
        BitmapFixture f;

        f.DeviceContext->GetPrimitiveBlendMethod.AllowAnyCall([&] { return D2D1_PRIMITIVE_BLEND_MIN; });

        Assert::AreEqual(E_FAIL, f.DS->DrawImageAtCoordsWithSourceRectAndInterpolation(f.Bitmap.Get(), 0, 0, Rect{}, CanvasImageInterpolation::Cubic));
        ValidateStoredErrorState(E_FAIL, Strings::DrawImageMinBlendNotSupported);
    }

    TEST_METHOD_EX(CanvasDrawingSession_DrawImage_BitmapComputesCorrectDestinationSize)
    {
        BitmapFixture f;

        const float x = 23;
        const float y = 42;
        const float w = 12;
        const float h = 34;

        f.DeviceContext->GetPrimitiveBlendMethod.AllowAnyCall();

        // When passing an explicit source size to DrawImage, that should be used to compute the DrawBitmap destination rect.
        Rect sourceRect{ 1, 2, 3, 4 };

        f.DeviceContext->DrawBitmapMethod.SetExpectedCalls(1,
            [&](ID2D1Bitmap*, const D2D1_RECT_F* dest, FLOAT, D2D1_INTERPOLATION_MODE, const D2D1_RECT_F* source, const D2D1_MATRIX_4X4_F*)
            {
                Assert::AreEqual(ToD2DRect(sourceRect), *source);

                Assert::AreEqual(x, dest->left);
                Assert::AreEqual(y, dest->top);
                Assert::AreEqual(x + sourceRect.Width, dest->right);
                Assert::AreEqual(y + sourceRect.Height, dest->bottom);
            });

        ThrowIfFailed(f.DS->DrawImageAtCoordsWithSourceRect(f.Bitmap.Get(), x, y, sourceRect));

        // When drawing with unit mode = dips, the bitmap size in dips should be used.
        f.DeviceContext->GetUnitModeMethod.SetExpectedCalls(1, [] { return D2D1_UNIT_MODE_DIPS; });
        f.StubBitmap->GetSizeMethod.SetExpectedCalls(1, [&] { return D2D1_SIZE_F{ w, h }; });

        f.DeviceContext->DrawBitmapMethod.SetExpectedCalls(1,
            [&](ID2D1Bitmap*, const D2D1_RECT_F* dest, FLOAT, D2D1_INTERPOLATION_MODE, const D2D1_RECT_F* source, const D2D1_MATRIX_4X4_F*)
            {
                Assert::IsNull(source);

                Assert::AreEqual(x, dest->left);
                Assert::AreEqual(y, dest->top);
                Assert::AreEqual(x + w, dest->right);
                Assert::AreEqual(y + h, dest->bottom);
            });

        ThrowIfFailed(f.DS->DrawImageAtCoords(f.Bitmap.Get(), x, y));

        // Or when drawing with unit mode = pixels, the bitmap size should be queried in pixel format instead.
        f.DeviceContext->GetUnitModeMethod.SetExpectedCalls(1, [] { return D2D1_UNIT_MODE_PIXELS; });
        f.StubBitmap->GetPixelSizeMethod.SetExpectedCalls(1, [&] { return D2D1_SIZE_U{ static_cast<uint32_t>(w), static_cast<uint32_t>(h) }; });

        f.DeviceContext->DrawBitmapMethod.SetExpectedCalls(1,
            [&](ID2D1Bitmap*, const D2D1_RECT_F* dest, FLOAT, D2D1_INTERPOLATION_MODE, const D2D1_RECT_F* source, const D2D1_MATRIX_4X4_F*)
            {
                Assert::IsNull(source);

                Assert::AreEqual(x, dest->left);
                Assert::AreEqual(y, dest->top);
                Assert::AreEqual(x + w, dest->right);
                Assert::AreEqual(y + h, dest->bottom);
            });

        ThrowIfFailed(f.DS->DrawImageAtCoords(f.Bitmap.Get(), x, y));
    }

    class BitmapFixtureWithDrawImageVerification : public BitmapFixture
    {
    public:
        BitmapFixtureWithDrawImageVerification(
            Rect const& sourceRect,
            CanvasImageInterpolation interpolation = CanvasImageInterpolation::Linear,
            CanvasComposite composite = CanvasComposite::Copy)
        {
            D2D1_RECT_F expectedSourceRect = ToD2DRect(sourceRect);
            D2D1_INTERPOLATION_MODE expectedInterpolation = static_cast<D2D1_INTERPOLATION_MODE>(interpolation);
            D2D1_COMPOSITE_MODE expectedComposite = static_cast<D2D1_COMPOSITE_MODE>(composite);
            
            DeviceContext->GetPrimitiveBlendMethod.AllowAnyCall([] { return D2D1_PRIMITIVE_BLEND_COPY; });

            DeviceContext->DrawImageMethod.SetExpectedCalls(1,
                [=](ID2D1Image* image, CONST D2D1_POINT_2F *targetOffset, CONST D2D1_RECT_F *imageRectangle, D2D1_INTERPOLATION_MODE interpolationMode, D2D1_COMPOSITE_MODE compositeMode)
                {
                    Assert::IsNotNull(image);
                    Assert::AreEqual(Image.Get(), image);
                    Assert::AreEqual(expectedSourceRect, *imageRectangle);
                    Assert::AreEqual(expectedInterpolation, interpolationMode);
                    Assert::AreEqual(expectedComposite, compositeMode);
                });
        }
    };

    class BitmapFixtureWithDrawBitmapVerification : public BitmapFixture
    {
    public:
        BitmapFixtureWithDrawBitmapVerification(
            Rect const& destRect,
            Rect* sourceRect = nullptr,
            float opacity = 1.0f,
            CanvasImageInterpolation interpolation = CanvasImageInterpolation::Linear,
            Matrix4x4* perspective = nullptr)
        {
            D2D1_RECT_F expectedDestRect = ToD2DRect(destRect);

            D2D1_RECT_F expectedSourceRect;
            if (sourceRect) expectedSourceRect = ToD2DRect(*sourceRect);
            bool expectSourceRect = sourceRect != nullptr;

            D2D1_INTERPOLATION_MODE expectedInterpolation = static_cast<D2D1_INTERPOLATION_MODE>(interpolation);

            D2D1_MATRIX_4X4_F expectedPerspective;
            if (perspective) expectedPerspective = *(ReinterpretAs<D2D1_MATRIX_4X4_F*>(perspective));
            bool expectPerspective = perspective != nullptr;

            DeviceContext->GetPrimitiveBlendMethod.AllowAnyCall();

            DeviceContext->DrawBitmapMethod.SetExpectedCalls(1,
                [=](ID2D1Bitmap* bitmap, const D2D1_RECT_F* destRect, FLOAT opacity, D2D1_INTERPOLATION_MODE interpolation, const D2D1_RECT_F* sourceRect, const D2D1_MATRIX_4X4_F* perspective)
                {
                    Assert::IsNotNull(bitmap);
                    auto image = As<ID2D1Image>(bitmap);
                    Assert::AreEqual(Image.Get(), image.Get());
                    Assert::AreEqual(expectedDestRect, *destRect);

                    if (expectSourceRect)
                    {
                        Assert::IsNotNull(sourceRect);
                        Assert::AreEqual(expectedSourceRect, *sourceRect);
                    }
                    else
                    {
                        Assert::IsNull(sourceRect);
                    }

                    Assert::AreEqual(expectedInterpolation, interpolation);

                    if (expectPerspective)
                    {
                        Assert::IsNotNull(perspective);
                        Assert::AreEqual(expectedPerspective, *perspective);
                    }
                    else
                    {
                        Assert::IsNull(perspective);
                    }
                });
        }
    };

    TEST_METHOD_EX(CanvasDrawingSession_DrawImage_Overloads)
    {
        Rect destRect{ 33, 12, 4, 2 };
        Rect sourceRect{ 3, 4, 7, 9 };
        CanvasImageInterpolation interpolation = CanvasImageInterpolation::MultiSampleLinear;
        CanvasComposite composite = CanvasComposite::Add;
        const float opacity = 0.45f;
        Matrix4x4 perspective = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};

        {
            BitmapFixtureWithDrawBitmapVerification f(Rect{ 123, 456, sourceRect.Width, sourceRect.Height }, &sourceRect);
            ThrowIfFailed(f.DS->DrawImageWithSourceRect(f.Bitmap.Get(), Vector2{ 123, 456 }, sourceRect));
        }
        {
            BitmapFixtureWithDrawImageVerification f(sourceRect, interpolation);
            ThrowIfFailed(f.DS->DrawImageWithSourceRectAndInterpolation(f.Bitmap.Get(), Vector2{ 123, 456 }, sourceRect, interpolation));
        }
        {
            BitmapFixtureWithDrawImageVerification f(sourceRect, interpolation, composite);
            ThrowIfFailed(f.DS->DrawImageWithSourceRectAndInterpolationAndComposite(f.Bitmap.Get(), Vector2{ 123, 456 }, sourceRect, interpolation, composite));
        }
        {
            BitmapFixtureWithDrawBitmapVerification f(Rect{ 123, 456, sourceRect.Width, sourceRect.Height }, &sourceRect);
            ThrowIfFailed(f.DS->DrawImageAtCoordsWithSourceRect(f.Bitmap.Get(), 123, 456, sourceRect));
        }
        {
            BitmapFixtureWithDrawImageVerification f(sourceRect, interpolation);
            ThrowIfFailed(f.DS->DrawImageAtCoordsWithSourceRectAndInterpolation(f.Bitmap.Get(), 123, 456, sourceRect, interpolation));
        }
        {
            BitmapFixtureWithDrawImageVerification f(sourceRect, interpolation, composite);
            ThrowIfFailed(f.DS->DrawImageAtCoordsWithSourceRectAndInterpolationAndComposite(f.Bitmap.Get(), 123, 456, sourceRect, interpolation, composite));
        }
        {
            BitmapFixtureWithDrawBitmapVerification f(destRect, &sourceRect, opacity);
            ThrowIfFailed(f.DS->DrawBitmapWithDestRectAndSourceRectAndOpacity(f.Bitmap.Get(), destRect, sourceRect, opacity));
        }
        {
            BitmapFixtureWithDrawBitmapVerification f(destRect, &sourceRect, opacity, interpolation);
            ThrowIfFailed(f.DS->DrawBitmapWithDestRectAndSourceRectAndOpacityAndInterpolation(f.Bitmap.Get(), destRect, sourceRect, opacity, interpolation));
        }
        {
            BitmapFixtureWithDrawBitmapVerification f(destRect, &sourceRect, opacity, interpolation, &perspective);
            ThrowIfFailed(f.DS->DrawBitmapWithDestRectAndSourceRectAndOpacityAndInterpolationAndPerspective(f.Bitmap.Get(), destRect, sourceRect, opacity, interpolation, perspective));
        }
        {
            BitmapFixtureWithDrawBitmapVerification f(destRect, &sourceRect);
            ThrowIfFailed(f.DS->DrawBitmapWithDestRectAndSourceRect(f.Bitmap.Get(), destRect, sourceRect));
        }
        {
            BitmapFixtureWithDrawBitmapVerification f(destRect);
            ThrowIfFailed(f.DS->DrawBitmapWithDestRect(f.Bitmap.Get(), destRect));
        }
    }

    TEST_METHOD_EX(CanvasDrawingSession_DrawImage_GaussianBlurEffect)
    {
        BitmapFixture f;

        ComPtr<MockD2DEffect> mockEffect = Make<MockD2DEffect>();
        
        bool setInputCalled = false;
        mockEffect->MockSetInput = 
            [&](UINT32, ID2D1Image*)
            {
                Assert::IsFalse(setInputCalled);
                setInputCalled = true;
            };

        bool setValueCalled = false;
        mockEffect->MockSetValue = 
            [&](UINT32, D2D1_PROPERTY_TYPE, CONST BYTE*, UINT32)
            {
                setValueCalled = true;
                return S_OK;
            };

        bool setInputCountCalled = false;
        mockEffect->MockSetInputCount =
            [&]
        {
            setInputCountCalled = true;
            return S_OK;
        };

        ComPtr<Effects::GaussianBlurEffect> blurEffect = Make<Effects::GaussianBlurEffect>();
        
        ThrowIfFailed(blurEffect->put_Source(f.Bitmap.Get()));

        ComPtr<StubCanvasDevice> canvasDevice = Make<StubCanvasDevice>();

        f.DeviceContext->GetDeviceMethod.AllowAnyCallAlwaysCopyValueToParam(canvasDevice->GetD2DDevice());
        f.DeviceContext->GetPrimitiveBlendMethod.AllowAnyCall();

        f.DeviceContext->GetDpiMethod.SetExpectedCalls(1,
            [&](float* dpiX, float* dpiY)
            {
                *dpiX = DEFAULT_DPI;
                *dpiY = DEFAULT_DPI;
            });

        f.DeviceContext->GetTargetMethod.SetExpectedCalls(1,
            [&](ID2D1Image** target)
            {
                *target = nullptr;
            });

        f.DeviceContext->CreateEffectMethod.SetExpectedCalls(1,
            [&](IID const&, ID2D1Effect** effect)
            {
                return mockEffect.CopyTo(effect);
            });

        f.DeviceContext->DrawImageMethod.SetExpectedCalls(1,
            [](ID2D1Image* image, D2D1_POINT_2F const*, D2D1_RECT_F const*, D2D1_INTERPOLATION_MODE, D2D1_COMPOSITE_MODE)
            {
                Assert::IsNotNull(image);
            });

        ThrowIfFailed(blurEffect->put_BlurAmount(5.0f));

        ThrowIfFailed(f.DS->DrawImageAtOrigin(blurEffect.Get()));
        Assert::IsTrue(setInputCalled);
        Assert::IsTrue(setValueCalled);
        Assert::IsTrue(setInputCountCalled);
    }


    //
    // DrawLine
    //

    template<typename TDraw>
    void TestDrawLine(bool isColorOverload, float expectedStrokeWidth, bool expectStrokeStyle, TDraw const& callDrawFunction)
    {
        CanvasDrawingSessionFixture f;
        BrushValidator brushValidator(f, isColorOverload);

        Vector2 expectedP0{ 1.0f, 2.0f };
        Vector2 expectedP1{ 3.0f, 4.0f };
        
        auto canvasStrokeStyle = Make<CanvasStrokeStyle>();
        canvasStrokeStyle->put_LineJoin(CanvasLineJoin::MiterOrBevel);

        int expectedDrawLineCount = isColorOverload ? 2 : 1;

        f.DeviceContext->DrawLineMethod.SetExpectedCalls(expectedDrawLineCount,
            [&](D2D1_POINT_2F p0, D2D1_POINT_2F p1, ID2D1Brush* brush, float strokeWidth, ID2D1StrokeStyle* strokeStyle)
            {
                Assert::AreEqual(expectedP0.X, p0.x);
                Assert::AreEqual(expectedP0.Y, p0.y);
                Assert::AreEqual(expectedP1.X, p1.x);
                Assert::AreEqual(expectedP1.Y, p1.y);
                brushValidator.Check(brush);
                Assert::AreEqual(expectedStrokeWidth, strokeWidth);
                if (expectStrokeStyle)
                {
                    Assert::IsNotNull(strokeStyle);
                    Assert::AreEqual(D2D1_LINE_JOIN_MITER_OR_BEVEL, f.DeviceContext->m_factory->m_lineJoin);
                }
                else
                {
                    Assert::IsNull(strokeStyle);
                }
            });

        callDrawFunction(f, expectedP0, expectedP1, canvasStrokeStyle.Get());
    }

    TEST_METHOD_EX(CanvasDrawingSession_DrawLineWithBrush)
    {
        TestDrawLine(false, 1, false,
            [](CanvasDrawingSessionFixture const& f, Vector2 p0, Vector2 p1, CanvasStrokeStyle* strokeStyle)
        {
            ThrowIfFailed(f.DS->DrawLineWithBrush(p0, p1, f.Brush.Get()));
        });

        // Null brush.
        Assert::AreEqual(E_INVALIDARG, CanvasDrawingSessionFixture().DS->DrawLineWithBrush(Vector2{}, Vector2{}, nullptr));
    }

    TEST_METHOD_EX(CanvasDrawingSession_DrawLineAtCoordsWithBrush)
    {
        TestDrawLine(false, 1, false,
            [](CanvasDrawingSessionFixture const& f, Vector2 p0, Vector2 p1, CanvasStrokeStyle* strokeStyle)
        {
            ThrowIfFailed(f.DS->DrawLineAtCoordsWithBrush(p0.X, p0.Y, p1.X, p1.Y, f.Brush.Get()));
        });

        // Null brush.
        Assert::AreEqual(E_INVALIDARG, CanvasDrawingSessionFixture().DS->DrawLineAtCoordsWithBrush(0, 0, 0, 0, nullptr));
    }

    TEST_METHOD_EX(CanvasDrawingSession_DrawLineWithColor)
    {
        TestDrawLine(true, 1, false,
            [](CanvasDrawingSessionFixture const& f, Vector2 p0, Vector2 p1, CanvasStrokeStyle* strokeStyle)
        {
            ThrowIfFailed(f.DS->DrawLineWithColor(p0, p1, ArbitraryMarkerColor1));
            ThrowIfFailed(f.DS->DrawLineWithColor(p0, p1, ArbitraryMarkerColor2));
        });
    }

    TEST_METHOD_EX(CanvasDrawingSession_DrawLineAtCoordsWithColor)
    {
        TestDrawLine(true, 1, false,
            [](CanvasDrawingSessionFixture const& f, Vector2 p0, Vector2 p1, CanvasStrokeStyle* strokeStyle)
        {
            ThrowIfFailed(f.DS->DrawLineAtCoordsWithColor(p0.X, p0.Y, p1.X, p1.Y, ArbitraryMarkerColor1));
            ThrowIfFailed(f.DS->DrawLineAtCoordsWithColor(p0.X, p0.Y, p1.X, p1.Y, ArbitraryMarkerColor2));
        });
    }

    TEST_METHOD_EX(CanvasDrawingSession_DrawLineWithBrushAndStrokeWidth)
    {
        TestDrawLine(false, 123, false,
            [](CanvasDrawingSessionFixture const& f, Vector2 p0, Vector2 p1, CanvasStrokeStyle* strokeStyle)
        {
            ThrowIfFailed(f.DS->DrawLineWithBrushAndStrokeWidth(p0, p1, f.Brush.Get(), 123));
        });

        // Null brush.
        Assert::AreEqual(E_INVALIDARG, CanvasDrawingSessionFixture().DS->DrawLineWithBrushAndStrokeWidth(Vector2{}, Vector2{}, nullptr, 0));
    }

    TEST_METHOD_EX(CanvasDrawingSession_DrawLineAtCoordsWithBrushAndStrokeWidth)
    {
        TestDrawLine(false, 123, false,
            [](CanvasDrawingSessionFixture const& f, Vector2 p0, Vector2 p1, CanvasStrokeStyle* strokeStyle)
        {
            ThrowIfFailed(f.DS->DrawLineAtCoordsWithBrushAndStrokeWidth(p0.X, p0.Y, p1.X, p1.Y, f.Brush.Get(), 123));
        });

        // Null brush.
        Assert::AreEqual(E_INVALIDARG, CanvasDrawingSessionFixture().DS->DrawLineAtCoordsWithBrushAndStrokeWidth(0, 0, 0, 0, nullptr, 0));
    }

    TEST_METHOD_EX(CanvasDrawingSession_DrawLineWithColorAndStrokeWidth)
    {
        TestDrawLine(true, 123, false,
            [](CanvasDrawingSessionFixture const& f, Vector2 p0, Vector2 p1, CanvasStrokeStyle* strokeStyle)
        {
            ThrowIfFailed(f.DS->DrawLineWithColorAndStrokeWidth(p0, p1, ArbitraryMarkerColor1, 123));
            ThrowIfFailed(f.DS->DrawLineWithColorAndStrokeWidth(p0, p1, ArbitraryMarkerColor2, 123));
        });
    }

    TEST_METHOD_EX(CanvasDrawingSession_DrawLineAtCoordsWithColorAndStrokeWidth)
    {
        TestDrawLine(true, 123, false,
            [](CanvasDrawingSessionFixture const& f, Vector2 p0, Vector2 p1, CanvasStrokeStyle* strokeStyle)
        {
            ThrowIfFailed(f.DS->DrawLineAtCoordsWithColorAndStrokeWidth(p0.X, p0.Y, p1.X, p1.Y, ArbitraryMarkerColor1, 123));
            ThrowIfFailed(f.DS->DrawLineAtCoordsWithColorAndStrokeWidth(p0.X, p0.Y, p1.X, p1.Y, ArbitraryMarkerColor2, 123));
        });
    }

    TEST_METHOD_EX(CanvasDrawingSession_DrawLineWithBrushAndStrokeWidthAndStrokeStyle)
    {
        TestDrawLine(false, 123, true,
            [](CanvasDrawingSessionFixture const& f, Vector2 p0, Vector2 p1, CanvasStrokeStyle* strokeStyle)
        {
            ThrowIfFailed(f.DS->DrawLineWithBrushAndStrokeWidthAndStrokeStyle(p0, p1, f.Brush.Get(), 123, strokeStyle));
        });

        // Null brush.
        Assert::AreEqual(E_INVALIDARG, CanvasDrawingSessionFixture().DS->DrawLineWithBrushAndStrokeWidthAndStrokeStyle(Vector2{}, Vector2{}, nullptr, 0, nullptr));
    }

    TEST_METHOD_EX(CanvasDrawingSession_DrawLineAtCoordsWithBrushAndStrokeWidthAndStrokeStyle)
    {
        TestDrawLine(false, 123, true,
            [](CanvasDrawingSessionFixture const& f, Vector2 p0, Vector2 p1, CanvasStrokeStyle* strokeStyle)
        {
            ThrowIfFailed(f.DS->DrawLineAtCoordsWithBrushAndStrokeWidthAndStrokeStyle(p0.X, p0.Y, p1.X, p1.Y, f.Brush.Get(), 123, strokeStyle));
        });

        // Null brush.
        Assert::AreEqual(E_INVALIDARG, CanvasDrawingSessionFixture().DS->DrawLineAtCoordsWithBrushAndStrokeWidthAndStrokeStyle(0, 0, 0, 0, nullptr, 0, nullptr));
    }

    TEST_METHOD_EX(CanvasDrawingSession_DrawLineWithColorAndStrokeWidthAndStrokeStyle)
    {
        TestDrawLine(true, 123, true,
            [](CanvasDrawingSessionFixture const& f, Vector2 p0, Vector2 p1, CanvasStrokeStyle* strokeStyle)
        {
            ThrowIfFailed(f.DS->DrawLineWithColorAndStrokeWidthAndStrokeStyle(p0, p1, ArbitraryMarkerColor1, 123, strokeStyle));
            ThrowIfFailed(f.DS->DrawLineWithColorAndStrokeWidthAndStrokeStyle(p0, p1, ArbitraryMarkerColor2, 123, strokeStyle));
        });
    }

    TEST_METHOD_EX(CanvasDrawingSession_DrawLineAtCoordsWithColorAndStrokeWidthAndStrokeStyle)
    {
        TestDrawLine(true, 123, true,
            [](CanvasDrawingSessionFixture const& f, Vector2 p0, Vector2 p1, CanvasStrokeStyle* strokeStyle)
        {
            ThrowIfFailed(f.DS->DrawLineAtCoordsWithColorAndStrokeWidthAndStrokeStyle(p0.X, p0.Y, p1.X, p1.Y, ArbitraryMarkerColor1, 123, strokeStyle));
            ThrowIfFailed(f.DS->DrawLineAtCoordsWithColorAndStrokeWidthAndStrokeStyle(p0.X, p0.Y, p1.X, p1.Y, ArbitraryMarkerColor2, 123, strokeStyle));
        });
    }


    //
    // DrawRectangle
    //

    template<typename TDraw>
    void TestDrawRectangle(bool isColorOverload, float expectedStrokeWidth, bool expectStrokeStyle, TDraw const& callDrawFunction)
    {
        CanvasDrawingSessionFixture f;
        BrushValidator brushValidator(f, isColorOverload);

        Rect expectedRect{ 1.0f, 2.0f, 3.0f, 4.0f };

        auto canvasStrokeStyle = Make<CanvasStrokeStyle>();
        canvasStrokeStyle->put_LineJoin(CanvasLineJoin::MiterOrBevel);

        int expectedDrawRectangleCount = isColorOverload ? 2 : 1;

        f.DeviceContext->DrawRectangleMethod.SetExpectedCalls(expectedDrawRectangleCount,
            [&](D2D1_RECT_F const* rect, ID2D1Brush* brush, float strokeWidth, ID2D1StrokeStyle* strokeStyle)
            {
                Assert::AreEqual(expectedRect.X, rect->left);
                Assert::AreEqual(expectedRect.Y, rect->top);
                Assert::AreEqual(expectedRect.X + expectedRect.Width, rect->right);
                Assert::AreEqual(expectedRect.Y + expectedRect.Height, rect->bottom);
                brushValidator.Check(brush);
                Assert::AreEqual(expectedStrokeWidth, strokeWidth);
                if (expectStrokeStyle)
                {
                    Assert::IsNotNull(strokeStyle);
                    Assert::AreEqual(D2D1_LINE_JOIN_MITER_OR_BEVEL, f.DeviceContext->m_factory->m_lineJoin);
                }
                else
                {
                    Assert::IsNull(strokeStyle);
                }
            });

        callDrawFunction(f, expectedRect, canvasStrokeStyle.Get());
    }

    TEST_METHOD_EX(CanvasDrawingSession_DrawRectangleWithBrush)
    {
        TestDrawRectangle(false, 1, false,
            [](CanvasDrawingSessionFixture const& f, Rect rect, CanvasStrokeStyle* strokeStyle)
        {
            ThrowIfFailed(f.DS->DrawRectangleWithBrush(rect, f.Brush.Get()));
        });

        // Null brush.
        Assert::AreEqual(E_INVALIDARG, CanvasDrawingSessionFixture().DS->DrawRectangleWithBrush(Rect{}, nullptr));
    }

    TEST_METHOD_EX(CanvasDrawingSession_DrawRectangleAtCoordsWithBrush)
    {
        TestDrawRectangle(false, 1, false,
            [](CanvasDrawingSessionFixture const& f, Rect rect, CanvasStrokeStyle* strokeStyle)
        {
            ThrowIfFailed(f.DS->DrawRectangleAtCoordsWithBrush(rect.X, rect.Y, rect.Width, rect.Height, f.Brush.Get()));
        });

        // Null brush.
        Assert::AreEqual(E_INVALIDARG, CanvasDrawingSessionFixture().DS->DrawRectangleAtCoordsWithBrush(0, 0, 0, 0, nullptr));
    }

    TEST_METHOD_EX(CanvasDrawingSession_DrawRectangleWithColor)
    {
        TestDrawRectangle(true, 1, false,
            [](CanvasDrawingSessionFixture const& f, Rect rect, CanvasStrokeStyle* strokeStyle)
        {
            ThrowIfFailed(f.DS->DrawRectangleWithColor(rect, ArbitraryMarkerColor1));
            ThrowIfFailed(f.DS->DrawRectangleWithColor(rect, ArbitraryMarkerColor2));
        });
    }

    TEST_METHOD_EX(CanvasDrawingSession_DrawRectangleAtCoordsWithColor)
    {
        TestDrawRectangle(true, 1, false,
            [](CanvasDrawingSessionFixture const& f, Rect rect, CanvasStrokeStyle* strokeStyle)
        {
            ThrowIfFailed(f.DS->DrawRectangleAtCoordsWithColor(rect.X, rect.Y, rect.Width, rect.Height, ArbitraryMarkerColor1));
            ThrowIfFailed(f.DS->DrawRectangleAtCoordsWithColor(rect.X, rect.Y, rect.Width, rect.Height, ArbitraryMarkerColor2));
        });
    }

    TEST_METHOD_EX(CanvasDrawingSession_DrawRectangleWithBrushAndStrokeWidth)
    {
        TestDrawRectangle(false, 123, false,
            [](CanvasDrawingSessionFixture const& f, Rect rect, CanvasStrokeStyle* strokeStyle)
        {
            ThrowIfFailed(f.DS->DrawRectangleWithBrushAndStrokeWidth(rect, f.Brush.Get(), 123));
        });

        // Null brush.
        Assert::AreEqual(E_INVALIDARG, CanvasDrawingSessionFixture().DS->DrawRectangleWithBrushAndStrokeWidth(Rect{}, nullptr, 0));
    }

    TEST_METHOD_EX(CanvasDrawingSession_DrawRectangleAtCoordsWithBrushAndStrokeWidth)
    {
        TestDrawRectangle(false, 123, false,
            [](CanvasDrawingSessionFixture const& f, Rect rect, CanvasStrokeStyle* strokeStyle)
        {
            ThrowIfFailed(f.DS->DrawRectangleAtCoordsWithBrushAndStrokeWidth(rect.X, rect.Y, rect.Width, rect.Height, f.Brush.Get(), 123));
        });

        // Null brush.
        Assert::AreEqual(E_INVALIDARG, CanvasDrawingSessionFixture().DS->DrawRectangleAtCoordsWithBrushAndStrokeWidth(0, 0, 0, 0, nullptr, 0));
    }

    TEST_METHOD_EX(CanvasDrawingSession_DrawRectangleWithColorAndStrokeWidth)
    {
        TestDrawRectangle(true, 123, false,
            [](CanvasDrawingSessionFixture const& f, Rect rect, CanvasStrokeStyle* strokeStyle)
        {
            ThrowIfFailed(f.DS->DrawRectangleWithColorAndStrokeWidth(rect, ArbitraryMarkerColor1, 123));
            ThrowIfFailed(f.DS->DrawRectangleWithColorAndStrokeWidth(rect, ArbitraryMarkerColor2, 123));
        });
    }

    TEST_METHOD_EX(CanvasDrawingSession_DrawRectangleAtCoordsWithColorAndStrokeWidth)
    {
        TestDrawRectangle(true, 123, false,
            [](CanvasDrawingSessionFixture const& f, Rect rect, CanvasStrokeStyle* strokeStyle)
        {
            ThrowIfFailed(f.DS->DrawRectangleAtCoordsWithColorAndStrokeWidth(rect.X, rect.Y, rect.Width, rect.Height, ArbitraryMarkerColor1, 123));
            ThrowIfFailed(f.DS->DrawRectangleAtCoordsWithColorAndStrokeWidth(rect.X, rect.Y, rect.Width, rect.Height, ArbitraryMarkerColor2, 123));
        });
    }

    TEST_METHOD_EX(CanvasDrawingSession_DrawRectangleWithBrushAndStrokeWidthAndStrokeStyle)
    {
        TestDrawRectangle(false, 123, true,
            [](CanvasDrawingSessionFixture const& f, Rect rect, CanvasStrokeStyle* strokeStyle)
        {
            ThrowIfFailed(f.DS->DrawRectangleWithBrushAndStrokeWidthAndStrokeStyle(rect, f.Brush.Get(), 123, strokeStyle));
        });

        // Null brush.
        Assert::AreEqual(E_INVALIDARG, CanvasDrawingSessionFixture().DS->DrawRectangleWithBrushAndStrokeWidthAndStrokeStyle(Rect{}, nullptr, 0, nullptr));
    }

    TEST_METHOD_EX(CanvasDrawingSession_DrawRectangleAtCoordsWithBrushAndStrokeWidthAndStrokeStyle)
    {
        TestDrawRectangle(false, 123, true,
            [](CanvasDrawingSessionFixture const& f, Rect rect, CanvasStrokeStyle* strokeStyle)
        {
            ThrowIfFailed(f.DS->DrawRectangleAtCoordsWithBrushAndStrokeWidthAndStrokeStyle(rect.X, rect.Y, rect.Width, rect.Height, f.Brush.Get(), 123, strokeStyle));
        });

        // Null brush.
        Assert::AreEqual(E_INVALIDARG, CanvasDrawingSessionFixture().DS->DrawRectangleAtCoordsWithBrushAndStrokeWidthAndStrokeStyle(0, 0, 0, 0, nullptr, 0, nullptr));
    }

    TEST_METHOD_EX(CanvasDrawingSession_DrawRectangleWithColorAndStrokeWidthAndStrokeStyle)
    {
        TestDrawRectangle(true, 123, true,
            [](CanvasDrawingSessionFixture const& f, Rect rect, CanvasStrokeStyle* strokeStyle)
        {
            ThrowIfFailed(f.DS->DrawRectangleWithColorAndStrokeWidthAndStrokeStyle(rect, ArbitraryMarkerColor1, 123, strokeStyle));
            ThrowIfFailed(f.DS->DrawRectangleWithColorAndStrokeWidthAndStrokeStyle(rect, ArbitraryMarkerColor2, 123, strokeStyle));
        });
    }

    TEST_METHOD_EX(CanvasDrawingSession_DrawRectangleAtCoordsWithColorAndStrokeWidthAndStrokeStyle)
    {
        TestDrawRectangle(true, 123, true,
            [](CanvasDrawingSessionFixture const& f, Rect rect, CanvasStrokeStyle* strokeStyle)
        {
            ThrowIfFailed(f.DS->DrawRectangleAtCoordsWithColorAndStrokeWidthAndStrokeStyle(rect.X, rect.Y, rect.Width, rect.Height, ArbitraryMarkerColor1, 123, strokeStyle));
            ThrowIfFailed(f.DS->DrawRectangleAtCoordsWithColorAndStrokeWidthAndStrokeStyle(rect.X, rect.Y, rect.Width, rect.Height, ArbitraryMarkerColor2, 123, strokeStyle));
        });
    }


    //
    // FillRectangle
    //

    template<typename TDraw>
    void TestFillRectangle(bool isColorOverload, TDraw const& callDrawFunction)
    {
        CanvasDrawingSessionFixture f;
        BrushValidator brushValidator(f, isColorOverload);

        Rect expectedRect{ 1.0f, 2.0f, 3.0f, 4.0f };

        int expectedFillRectangleCount = isColorOverload ? 2 : 1;

        f.DeviceContext->FillRectangleMethod.SetExpectedCalls(expectedFillRectangleCount,
            [&](D2D1_RECT_F const* rect, ID2D1Brush* brush)
            {
                Assert::AreEqual(expectedRect.X, rect->left);
                Assert::AreEqual(expectedRect.Y, rect->top);
                Assert::AreEqual(expectedRect.X + expectedRect.Width, rect->right);
                Assert::AreEqual(expectedRect.Y + expectedRect.Height, rect->bottom);
                brushValidator.Check(brush);
            });

        callDrawFunction(f, expectedRect);
    }

    TEST_METHOD_EX(CanvasDrawingSession_FillRectangleWithBrush)
    {
        TestFillRectangle(false,
            [](CanvasDrawingSessionFixture const& f, Rect rect)
        {
            ThrowIfFailed(f.DS->FillRectangleWithBrush(rect, f.Brush.Get()));
        });

        // Null brush.
        Assert::AreEqual(E_INVALIDARG, CanvasDrawingSessionFixture().DS->FillRectangleWithBrush(Rect{}, nullptr));
    }

    TEST_METHOD_EX(CanvasDrawingSession_FillRectangleAtCoordsWithBrush)
    {
        TestFillRectangle(false,
            [](CanvasDrawingSessionFixture const& f, Rect rect)
        {
            ThrowIfFailed(f.DS->FillRectangleAtCoordsWithBrush(rect.X, rect.Y, rect.Width, rect.Height, f.Brush.Get()));
        });

        // Null brush.
        Assert::AreEqual(E_INVALIDARG, CanvasDrawingSessionFixture().DS->FillRectangleAtCoordsWithBrush(0, 0, 0, 0, nullptr));
    }

    TEST_METHOD_EX(CanvasDrawingSession_FillRectangleWithColor)
    {
        TestFillRectangle(true,
            [](CanvasDrawingSessionFixture const& f, Rect rect)
        {
            ThrowIfFailed(f.DS->FillRectangleWithColor(rect, ArbitraryMarkerColor1));
            ThrowIfFailed(f.DS->FillRectangleWithColor(rect, ArbitraryMarkerColor2));
        });
    }

    TEST_METHOD_EX(CanvasDrawingSession_FillRectangleAtCoordsWithColor)
    {
        TestFillRectangle(true,
            [](CanvasDrawingSessionFixture const& f, Rect rect)
        {
            ThrowIfFailed(f.DS->FillRectangleAtCoordsWithColor(rect.X, rect.Y, rect.Width, rect.Height, ArbitraryMarkerColor1));
            ThrowIfFailed(f.DS->FillRectangleAtCoordsWithColor(rect.X, rect.Y, rect.Width, rect.Height, ArbitraryMarkerColor2));
        });
    }


    //
    // DrawRoundedRectangle
    //

    template<typename TDraw>
    void TestDrawRoundedRectangle(bool isColorOverload, float expectedStrokeWidth, bool expectStrokeStyle, TDraw const& callDrawFunction)
    {
        CanvasDrawingSessionFixture f;
        BrushValidator brushValidator(f, isColorOverload);

        Rect expectedRect{ 1.0f, 2.0f, 3.0f, 4.0f };
        float expectedRadiusX = 5;
        float expectedRadiusY = 6;

        auto canvasStrokeStyle = Make<CanvasStrokeStyle>();
        canvasStrokeStyle->put_LineJoin(CanvasLineJoin::MiterOrBevel);

        int expectedDrawRoundedRectangleCount = isColorOverload ? 2 : 1;

        f.DeviceContext->DrawRoundedRectangleMethod.SetExpectedCalls(expectedDrawRoundedRectangleCount,
            [&](D2D1_ROUNDED_RECT const* rect, ID2D1Brush* brush, float strokeWidth, ID2D1StrokeStyle* strokeStyle)
            {
                Assert::AreEqual(expectedRect.X, rect->rect.left);
                Assert::AreEqual(expectedRect.Y, rect->rect.top);
                Assert::AreEqual(expectedRect.X + expectedRect.Width, rect->rect.right);
                Assert::AreEqual(expectedRect.Y + expectedRect.Height, rect->rect.bottom);
                Assert::AreEqual(expectedRadiusX, rect->radiusX);
                Assert::AreEqual(expectedRadiusY, rect->radiusY);
                brushValidator.Check(brush);
                Assert::AreEqual(expectedStrokeWidth, strokeWidth);
                if (expectStrokeStyle)
                {
                    Assert::IsNotNull(strokeStyle);
                    Assert::AreEqual(D2D1_LINE_JOIN_MITER_OR_BEVEL, f.DeviceContext->m_factory->m_lineJoin);
                }
                else
                {
                    Assert::IsNull(strokeStyle);
                }
            });

        callDrawFunction(f, expectedRect, expectedRadiusX, expectedRadiusY, canvasStrokeStyle.Get());
    }

    TEST_METHOD_EX(CanvasDrawingSession_DrawRoundedRectangleWithBrush)
    {
        TestDrawRoundedRectangle(false, 1, false,
            [](CanvasDrawingSessionFixture const& f, Rect rect, float rx, float ry, CanvasStrokeStyle* strokeStyle)
        {
            ThrowIfFailed(f.DS->DrawRoundedRectangleWithBrush(rect, rx, ry, f.Brush.Get()));
        });

        // Null brush.
        Assert::AreEqual(E_INVALIDARG, CanvasDrawingSessionFixture().DS->DrawRoundedRectangleWithBrush(Rect{}, 0, 0, nullptr));
    }

    TEST_METHOD_EX(CanvasDrawingSession_DrawRoundedRectangleAtCoordsWithBrush)
    {
        TestDrawRoundedRectangle(false, 1, false,
            [](CanvasDrawingSessionFixture const& f, Rect rect, float rx, float ry, CanvasStrokeStyle* strokeStyle)
        {
            ThrowIfFailed(f.DS->DrawRoundedRectangleAtCoordsWithBrush(rect.X, rect.Y, rect.Width, rect.Height, rx, ry, f.Brush.Get()));
        });

        // Null brush.
        Assert::AreEqual(E_INVALIDARG, CanvasDrawingSessionFixture().DS->DrawRoundedRectangleAtCoordsWithBrush(0, 0, 0, 0, 0, 0, nullptr));
    }

    TEST_METHOD_EX(CanvasDrawingSession_DrawRoundedRectangleWithColor)
    {
        TestDrawRoundedRectangle(true, 1, false,
            [](CanvasDrawingSessionFixture const& f, Rect rect, float rx, float ry, CanvasStrokeStyle* strokeStyle)
        {
            ThrowIfFailed(f.DS->DrawRoundedRectangleWithColor(rect, rx, ry, ArbitraryMarkerColor1));
            ThrowIfFailed(f.DS->DrawRoundedRectangleWithColor(rect, rx, ry, ArbitraryMarkerColor2));
        });
    }

    TEST_METHOD_EX(CanvasDrawingSession_DrawRoundedRectangleAtCoordsWithColor)
    {
        TestDrawRoundedRectangle(true, 1, false,
            [](CanvasDrawingSessionFixture const& f, Rect rect, float rx, float ry, CanvasStrokeStyle* strokeStyle)
        {
            ThrowIfFailed(f.DS->DrawRoundedRectangleAtCoordsWithColor(rect.X, rect.Y, rect.Width, rect.Height, rx, ry, ArbitraryMarkerColor1));
            ThrowIfFailed(f.DS->DrawRoundedRectangleAtCoordsWithColor(rect.X, rect.Y, rect.Width, rect.Height, rx, ry, ArbitraryMarkerColor2));
        });
    }

    TEST_METHOD_EX(CanvasDrawingSession_DrawRoundedRectangleWithBrushAndStrokeWidth)
    {
        TestDrawRoundedRectangle(false, 123, false,
            [](CanvasDrawingSessionFixture const& f, Rect rect, float rx, float ry, CanvasStrokeStyle* strokeStyle)
        {
            ThrowIfFailed(f.DS->DrawRoundedRectangleWithBrushAndStrokeWidth(rect, rx, ry, f.Brush.Get(), 123));
        });

        // Null brush.
        Assert::AreEqual(E_INVALIDARG, CanvasDrawingSessionFixture().DS->DrawRoundedRectangleWithBrushAndStrokeWidth(Rect{}, 0, 0, nullptr, 0));
    }

    TEST_METHOD_EX(CanvasDrawingSession_DrawRoundedRectangleAtCoordsWithBrushAndStrokeWidth)
    {
        TestDrawRoundedRectangle(false, 123, false,
            [](CanvasDrawingSessionFixture const& f, Rect rect, float rx, float ry, CanvasStrokeStyle* strokeStyle)
        {
            ThrowIfFailed(f.DS->DrawRoundedRectangleAtCoordsWithBrushAndStrokeWidth(rect.X, rect.Y, rect.Width, rect.Height, rx, ry, f.Brush.Get(), 123));
        });

        // Null brush.
        Assert::AreEqual(E_INVALIDARG, CanvasDrawingSessionFixture().DS->DrawRoundedRectangleAtCoordsWithBrushAndStrokeWidth(0, 0, 0, 0, 0, 0, nullptr, 0));
    }

    TEST_METHOD_EX(CanvasDrawingSession_DrawRoundedRectangleWithColorAndStrokeWidth)
    {
        TestDrawRoundedRectangle(true, 123, false,
            [](CanvasDrawingSessionFixture const& f, Rect rect, float rx, float ry, CanvasStrokeStyle* strokeStyle)
        {
            ThrowIfFailed(f.DS->DrawRoundedRectangleWithColorAndStrokeWidth(rect, rx, ry, ArbitraryMarkerColor1, 123));
            ThrowIfFailed(f.DS->DrawRoundedRectangleWithColorAndStrokeWidth(rect, rx, ry, ArbitraryMarkerColor2, 123));
        });
    }

    TEST_METHOD_EX(CanvasDrawingSession_DrawRoundedRectangleAtCoordsWithColorAndStrokeWidth)
    {
        TestDrawRoundedRectangle(true, 123, false,
            [](CanvasDrawingSessionFixture const& f, Rect rect, float rx, float ry, CanvasStrokeStyle* strokeStyle)
        {
            ThrowIfFailed(f.DS->DrawRoundedRectangleAtCoordsWithColorAndStrokeWidth(rect.X, rect.Y, rect.Width, rect.Height, rx, ry, ArbitraryMarkerColor1, 123));
            ThrowIfFailed(f.DS->DrawRoundedRectangleAtCoordsWithColorAndStrokeWidth(rect.X, rect.Y, rect.Width, rect.Height, rx, ry, ArbitraryMarkerColor2, 123));
        });
    }

    TEST_METHOD_EX(CanvasDrawingSession_DrawRoundedRectangleWithBrushAndStrokeWidthAndStrokeStyle)
    {
        TestDrawRoundedRectangle(false, 123, true,
            [](CanvasDrawingSessionFixture const& f, Rect rect, float rx, float ry, CanvasStrokeStyle* strokeStyle)
        {
            ThrowIfFailed(f.DS->DrawRoundedRectangleWithBrushAndStrokeWidthAndStrokeStyle(rect, rx, ry, f.Brush.Get(), 123, strokeStyle));
        });

        // Null brush.
        Assert::AreEqual(E_INVALIDARG, CanvasDrawingSessionFixture().DS->DrawRoundedRectangleWithBrushAndStrokeWidthAndStrokeStyle(Rect{}, 0, 0, nullptr, 0, nullptr));
    }

    TEST_METHOD_EX(CanvasDrawingSession_DrawRoundedRectangleAtCoordsWithBrushAndStrokeWidthAndStrokeStyle)
    {
        TestDrawRoundedRectangle(false, 123, true,
            [](CanvasDrawingSessionFixture const& f, Rect rect, float rx, float ry, CanvasStrokeStyle* strokeStyle)
        {
            ThrowIfFailed(f.DS->DrawRoundedRectangleAtCoordsWithBrushAndStrokeWidthAndStrokeStyle(rect.X, rect.Y, rect.Width, rect.Height, rx, ry, f.Brush.Get(), 123, strokeStyle));
        });

        // Null brush.
        Assert::AreEqual(E_INVALIDARG, CanvasDrawingSessionFixture().DS->DrawRoundedRectangleAtCoordsWithBrushAndStrokeWidthAndStrokeStyle(0, 0, 0, 0, 0, 0, nullptr, 0, nullptr));
    }

    TEST_METHOD_EX(CanvasDrawingSession_DrawRoundedRectangleWithColorAndStrokeWidthAndStrokeStyle)
    {
        TestDrawRoundedRectangle(true, 123, true,
            [](CanvasDrawingSessionFixture const& f, Rect rect, float rx, float ry, CanvasStrokeStyle* strokeStyle)
        {
            ThrowIfFailed(f.DS->DrawRoundedRectangleWithColorAndStrokeWidthAndStrokeStyle(rect, rx, ry, ArbitraryMarkerColor1, 123, strokeStyle));
            ThrowIfFailed(f.DS->DrawRoundedRectangleWithColorAndStrokeWidthAndStrokeStyle(rect, rx, ry, ArbitraryMarkerColor2, 123, strokeStyle));
        });
    }

    TEST_METHOD_EX(CanvasDrawingSession_DrawRoundedRectangleAtCoordsWithColorAndStrokeWidthAndStrokeStyle)
    {
        TestDrawRoundedRectangle(true, 123, true,
            [](CanvasDrawingSessionFixture const& f, Rect rect, float rx, float ry, CanvasStrokeStyle* strokeStyle)
        {
            ThrowIfFailed(f.DS->DrawRoundedRectangleAtCoordsWithColorAndStrokeWidthAndStrokeStyle(rect.X, rect.Y, rect.Width, rect.Height, rx, ry, ArbitraryMarkerColor1, 123, strokeStyle));
            ThrowIfFailed(f.DS->DrawRoundedRectangleAtCoordsWithColorAndStrokeWidthAndStrokeStyle(rect.X, rect.Y, rect.Width, rect.Height, rx, ry, ArbitraryMarkerColor2, 123, strokeStyle));
        });
    }


    //
    // FillRoundedRectangle
    //

    template<typename TDraw>
    void TestFillRoundedRectangle(bool isColorOverload, TDraw const& callDrawFunction)
    {
        CanvasDrawingSessionFixture f;
        BrushValidator brushValidator(f, isColorOverload);

        Rect expectedRect{ 1.0f, 2.0f, 3.0f, 4.0f };
        float expectedRadiusX = 5;
        float expectedRadiusY = 6;

        int expectedFillRoundedRectangleCount = isColorOverload ? 2 : 1;

        f.DeviceContext->FillRoundedRectangleMethod.SetExpectedCalls(expectedFillRoundedRectangleCount,
            [&](D2D1_ROUNDED_RECT const* rect, ID2D1Brush* brush)
            {
                Assert::AreEqual(expectedRect.X, rect->rect.left);
                Assert::AreEqual(expectedRect.Y, rect->rect.top);
                Assert::AreEqual(expectedRect.X + expectedRect.Width, rect->rect.right);
                Assert::AreEqual(expectedRect.Y + expectedRect.Height, rect->rect.bottom);
                Assert::AreEqual(expectedRadiusX, rect->radiusX);
                Assert::AreEqual(expectedRadiusY, rect->radiusY);
                brushValidator.Check(brush);
            });

        callDrawFunction(f, expectedRect, expectedRadiusX, expectedRadiusY);
    }

    TEST_METHOD_EX(CanvasDrawingSession_FillRoundedRectangleWithBrush)
    {
        TestFillRoundedRectangle(false,
            [](CanvasDrawingSessionFixture const& f, Rect rect, float rx, float ry)
        {
            ThrowIfFailed(f.DS->FillRoundedRectangleWithBrush(rect, rx, ry, f.Brush.Get()));
        });

        // Null brush.
        Assert::AreEqual(E_INVALIDARG, CanvasDrawingSessionFixture().DS->FillRoundedRectangleWithBrush(Rect{}, 0, 0, nullptr));
    }

    TEST_METHOD_EX(CanvasDrawingSession_FillRoundedRectangleAtCoordsWithBrush)
    {
        TestFillRoundedRectangle(false,
            [](CanvasDrawingSessionFixture const& f, Rect rect, float rx, float ry)
        {
            ThrowIfFailed(f.DS->FillRoundedRectangleAtCoordsWithBrush(rect.X, rect.Y, rect.Width, rect.Height, rx, ry, f.Brush.Get()));
        });

        // Null brush.
        Assert::AreEqual(E_INVALIDARG, CanvasDrawingSessionFixture().DS->FillRoundedRectangleAtCoordsWithBrush(0, 0, 0, 0, 0, 0, nullptr));
    }

    TEST_METHOD_EX(CanvasDrawingSession_FillRoundedRectangleWithColor)
    {
        TestFillRoundedRectangle(true,
            [](CanvasDrawingSessionFixture const& f, Rect rect, float rx, float ry)
        {
            ThrowIfFailed(f.DS->FillRoundedRectangleWithColor(rect, rx, ry, ArbitraryMarkerColor1));
            ThrowIfFailed(f.DS->FillRoundedRectangleWithColor(rect, rx, ry, ArbitraryMarkerColor2));
        });
    }

    TEST_METHOD_EX(CanvasDrawingSession_FillRoundedRectangleAtCoordsWithColor)
    {
        TestFillRoundedRectangle(true,
            [](CanvasDrawingSessionFixture const& f, Rect rect, float rx, float ry)
        {
            ThrowIfFailed(f.DS->FillRoundedRectangleAtCoordsWithColor(rect.X, rect.Y, rect.Width, rect.Height, rx, ry, ArbitraryMarkerColor1));
            ThrowIfFailed(f.DS->FillRoundedRectangleAtCoordsWithColor(rect.X, rect.Y, rect.Width, rect.Height, rx, ry, ArbitraryMarkerColor2));
        });
    }


    //
    // DrawEllipse
    //

    template<typename TDraw>
    void TestDrawEllipse(bool isColorOverload, float expectedRadiusX, float expectedRadiusY, float expectedStrokeWidth, bool expectStrokeStyle, TDraw const& callDrawFunction)
    {
        CanvasDrawingSessionFixture f;
        BrushValidator brushValidator(f, isColorOverload);

        Vector2 expectedPoint{ 1.0f, 2.0f };

        auto canvasStrokeStyle = Make<CanvasStrokeStyle>();
        canvasStrokeStyle->put_LineJoin(CanvasLineJoin::MiterOrBevel);

        int expectedDrawEllipseCount = isColorOverload ? 2 : 1;

        f.DeviceContext->DrawEllipseMethod.SetExpectedCalls(expectedDrawEllipseCount,
            [&](D2D1_ELLIPSE const* ellipse, ID2D1Brush* brush, float strokeWidth, ID2D1StrokeStyle* strokeStyle)
            {
                Assert::AreEqual(expectedPoint.X, ellipse->point.x);
                Assert::AreEqual(expectedPoint.Y, ellipse->point.y);
                Assert::AreEqual(expectedRadiusX, ellipse->radiusX);
                Assert::AreEqual(expectedRadiusY, ellipse->radiusY);
                brushValidator.Check(brush);
                Assert::AreEqual(expectedStrokeWidth, strokeWidth);
                if (expectStrokeStyle)
                {
                    Assert::IsNotNull(strokeStyle);
                    Assert::AreEqual(D2D1_LINE_JOIN_MITER_OR_BEVEL, f.DeviceContext->m_factory->m_lineJoin);
                }
                else
                {
                    Assert::IsNull(strokeStyle);
                }
            });

        callDrawFunction(f, expectedPoint, canvasStrokeStyle.Get());
    }

    TEST_METHOD_EX(CanvasDrawingSession_DrawEllipseWithBrush)
    {
        TestDrawEllipse(false, 23, 42, 1, false,
            [](CanvasDrawingSessionFixture const& f, Vector2 point, CanvasStrokeStyle* strokeStyle)
        {
            ThrowIfFailed(f.DS->DrawEllipseWithBrush(point, 23, 42, f.Brush.Get()));
        });

        // Null brush.
        Assert::AreEqual(E_INVALIDARG, CanvasDrawingSessionFixture().DS->DrawEllipseWithBrush(Vector2{}, 0, 0, nullptr));
    }

    TEST_METHOD_EX(CanvasDrawingSession_DrawEllipseAtCoordsWithBrush)
    {
        TestDrawEllipse(false, 23, 42, 1, false,
            [](CanvasDrawingSessionFixture const& f, Vector2 point, CanvasStrokeStyle* strokeStyle)
        {
            ThrowIfFailed(f.DS->DrawEllipseAtCoordsWithBrush(point.X, point.Y, 23, 42, f.Brush.Get()));
        });

        // Null brush.
        Assert::AreEqual(E_INVALIDARG, CanvasDrawingSessionFixture().DS->DrawEllipseAtCoordsWithBrush(0, 0, 0, 0, nullptr));
    }

    TEST_METHOD_EX(CanvasDrawingSession_DrawEllipseWithColor)
    {
        TestDrawEllipse(true, 23, 42, 1, false,
            [](CanvasDrawingSessionFixture const& f, Vector2 point, CanvasStrokeStyle* strokeStyle)
        {
            ThrowIfFailed(f.DS->DrawEllipseWithColor(point, 23, 42, ArbitraryMarkerColor1));
            ThrowIfFailed(f.DS->DrawEllipseWithColor(point, 23, 42, ArbitraryMarkerColor2));
        });
    }

    TEST_METHOD_EX(CanvasDrawingSession_DrawEllipseAtCoordsWithColor)
    {
        TestDrawEllipse(true, 23, 42, 1, false,
            [](CanvasDrawingSessionFixture const& f, Vector2 point, CanvasStrokeStyle* strokeStyle)
        {
            ThrowIfFailed(f.DS->DrawEllipseAtCoordsWithColor(point.X, point.Y, 23, 42, ArbitraryMarkerColor1));
            ThrowIfFailed(f.DS->DrawEllipseAtCoordsWithColor(point.X, point.Y, 23, 42, ArbitraryMarkerColor2));
        });
    }

    TEST_METHOD_EX(CanvasDrawingSession_DrawEllipseWithBrushAndStrokeWidth)
    {
        TestDrawEllipse(false, 23, 42, 123, false,
            [](CanvasDrawingSessionFixture const& f, Vector2 point, CanvasStrokeStyle* strokeStyle)
        {
            ThrowIfFailed(f.DS->DrawEllipseWithBrushAndStrokeWidth(point, 23, 42, f.Brush.Get(), 123));
        });

        // Null brush.
        Assert::AreEqual(E_INVALIDARG, CanvasDrawingSessionFixture().DS->DrawEllipseWithBrushAndStrokeWidth(Vector2{}, 0, 0, nullptr, 0));
    }

    TEST_METHOD_EX(CanvasDrawingSession_DrawEllipseAtCoordsWithBrushAndStrokeWidth)
    {
        TestDrawEllipse(false, 23, 42, 123, false,
            [](CanvasDrawingSessionFixture const& f, Vector2 point, CanvasStrokeStyle* strokeStyle)
        {
            ThrowIfFailed(f.DS->DrawEllipseAtCoordsWithBrushAndStrokeWidth(point.X, point.Y, 23, 42, f.Brush.Get(), 123));
        });

        // Null brush.
        Assert::AreEqual(E_INVALIDARG, CanvasDrawingSessionFixture().DS->DrawEllipseAtCoordsWithBrushAndStrokeWidth(0, 0, 0, 0, nullptr, 0));
    }

    TEST_METHOD_EX(CanvasDrawingSession_DrawEllipseWithColorAndStrokeWidth)
    {
        TestDrawEllipse(true, 23, 42, 123, false,
            [](CanvasDrawingSessionFixture const& f, Vector2 point, CanvasStrokeStyle* strokeStyle)
        {
            ThrowIfFailed(f.DS->DrawEllipseWithColorAndStrokeWidth(point, 23, 42, ArbitraryMarkerColor1, 123));
            ThrowIfFailed(f.DS->DrawEllipseWithColorAndStrokeWidth(point, 23, 42, ArbitraryMarkerColor2, 123));
        });
    }

    TEST_METHOD_EX(CanvasDrawingSession_DrawEllipseAtCoordsWithColorAndStrokeWidth)
    {
        TestDrawEllipse(true, 23, 42, 123, false,
            [](CanvasDrawingSessionFixture const& f, Vector2 point, CanvasStrokeStyle* strokeStyle)
        {
            ThrowIfFailed(f.DS->DrawEllipseAtCoordsWithColorAndStrokeWidth(point.X, point.Y, 23, 42, ArbitraryMarkerColor1, 123));
            ThrowIfFailed(f.DS->DrawEllipseAtCoordsWithColorAndStrokeWidth(point.X, point.Y, 23, 42, ArbitraryMarkerColor2, 123));
        });
    }

    TEST_METHOD_EX(CanvasDrawingSession_DrawEllipseWithBrushAndStrokeWidthAndStrokeStyle)
    {
        TestDrawEllipse(false, 23, 42, 123, true,
            [](CanvasDrawingSessionFixture const& f, Vector2 point, CanvasStrokeStyle* strokeStyle)
        {
            ThrowIfFailed(f.DS->DrawEllipseWithBrushAndStrokeWidthAndStrokeStyle(point, 23, 42, f.Brush.Get(), 123, strokeStyle));
        });

        // Null brush.
        Assert::AreEqual(E_INVALIDARG, CanvasDrawingSessionFixture().DS->DrawEllipseWithBrushAndStrokeWidthAndStrokeStyle(Vector2{}, 0, 0, nullptr, 0, nullptr));
    }

    TEST_METHOD_EX(CanvasDrawingSession_DrawEllipseAtCoordsWithBrushAndStrokeWidthAndStrokeStyle)
    {
        TestDrawEllipse(false, 23, 42, 123, true,
            [](CanvasDrawingSessionFixture const& f, Vector2 point, CanvasStrokeStyle* strokeStyle)
        {
            ThrowIfFailed(f.DS->DrawEllipseAtCoordsWithBrushAndStrokeWidthAndStrokeStyle(point.X, point.Y, 23, 42, f.Brush.Get(), 123, strokeStyle));
        });

        // Null brush.
        Assert::AreEqual(E_INVALIDARG, CanvasDrawingSessionFixture().DS->DrawEllipseAtCoordsWithBrushAndStrokeWidthAndStrokeStyle(0, 0, 0, 0, nullptr, 0, nullptr));
    }

    TEST_METHOD_EX(CanvasDrawingSession_DrawEllipseWithColorAndStrokeWidthAndStrokeStyle)
    {
        TestDrawEllipse(true, 23, 42, 123, true,
            [](CanvasDrawingSessionFixture const& f, Vector2 point, CanvasStrokeStyle* strokeStyle)
        {
            ThrowIfFailed(f.DS->DrawEllipseWithColorAndStrokeWidthAndStrokeStyle(point, 23, 42, ArbitraryMarkerColor1, 123, strokeStyle));
            ThrowIfFailed(f.DS->DrawEllipseWithColorAndStrokeWidthAndStrokeStyle(point, 23, 42, ArbitraryMarkerColor2, 123, strokeStyle));
        });
    }

    TEST_METHOD_EX(CanvasDrawingSession_DrawEllipseAtCoordsWithColorAndStrokeWidthAndStrokeStyle)
    {
        TestDrawEllipse(true, 23, 42, 123, true,
            [](CanvasDrawingSessionFixture const& f, Vector2 point, CanvasStrokeStyle* strokeStyle)
        {
            ThrowIfFailed(f.DS->DrawEllipseAtCoordsWithColorAndStrokeWidthAndStrokeStyle(point.X, point.Y, 23, 42, ArbitraryMarkerColor1, 123, strokeStyle));
            ThrowIfFailed(f.DS->DrawEllipseAtCoordsWithColorAndStrokeWidthAndStrokeStyle(point.X, point.Y, 23, 42, ArbitraryMarkerColor2, 123, strokeStyle));
        });
    }


    //
    // FillEllipse
    //

    template<typename TDraw>
    void TestFillEllipse(bool isColorOverload, float expectedRadiusX, float expectedRadiusY, TDraw const& callDrawFunction)
    {
        CanvasDrawingSessionFixture f;
        BrushValidator brushValidator(f, isColorOverload);

        Vector2 expectedPoint{ 1.0f, 2.0f };

        int expectedFillEllipseCount = isColorOverload ? 2 : 1;

        f.DeviceContext->FillEllipseMethod.SetExpectedCalls(expectedFillEllipseCount,
            [&](D2D1_ELLIPSE const* ellipse, ID2D1Brush* brush)
            {
                Assert::AreEqual(expectedPoint.X, ellipse->point.x);
                Assert::AreEqual(expectedPoint.Y, ellipse->point.y);
                Assert::AreEqual(expectedRadiusX, ellipse->radiusX);
                Assert::AreEqual(expectedRadiusY, ellipse->radiusY);
                brushValidator.Check(brush);
            });

        callDrawFunction(f, expectedPoint);
    }

    TEST_METHOD_EX(CanvasDrawingSession_FillEllipseWithBrush)
    {
        TestFillEllipse(false, 23, 42,
            [](CanvasDrawingSessionFixture const& f, Vector2 point)
        {
            ThrowIfFailed(f.DS->FillEllipseWithBrush(point, 23, 42, f.Brush.Get()));
        });

        // Null brush.
        Assert::AreEqual(E_INVALIDARG, CanvasDrawingSessionFixture().DS->FillEllipseWithBrush(Vector2{}, 0, 0, nullptr));
    }

    TEST_METHOD_EX(CanvasDrawingSession_FillEllipseAtCoordsWithBrush)
    {
        TestFillEllipse(false, 23, 42,
            [](CanvasDrawingSessionFixture const& f, Vector2 point)
        {
            ThrowIfFailed(f.DS->FillEllipseAtCoordsWithBrush(point.X, point.Y, 23, 42, f.Brush.Get()));
        });

        // Null brush.
        Assert::AreEqual(E_INVALIDARG, CanvasDrawingSessionFixture().DS->FillEllipseAtCoordsWithBrush(0, 0, 0, 0, nullptr));
    }

    TEST_METHOD_EX(CanvasDrawingSession_FillEllipseWithColor)
    {
        TestFillEllipse(true, 23, 42,
            [](CanvasDrawingSessionFixture const& f, Vector2 point)
        {
            ThrowIfFailed(f.DS->FillEllipseWithColor(point, 23, 42, ArbitraryMarkerColor1));
            ThrowIfFailed(f.DS->FillEllipseWithColor(point, 23, 42, ArbitraryMarkerColor2));
        });
    }

    TEST_METHOD_EX(CanvasDrawingSession_FillEllipseAtCoordsWithColor)
    {
        TestFillEllipse(true, 23, 42,
            [](CanvasDrawingSessionFixture const& f, Vector2 point)
        {
            ThrowIfFailed(f.DS->FillEllipseAtCoordsWithColor(point.X, point.Y, 23, 42, ArbitraryMarkerColor1));
            ThrowIfFailed(f.DS->FillEllipseAtCoordsWithColor(point.X, point.Y, 23, 42, ArbitraryMarkerColor2));
        });
    }


    //
    // DrawCircle
    //

    TEST_METHOD_EX(CanvasDrawingSession_DrawCircleWithBrush)
    {
        TestDrawEllipse(false, 23, 23, 1, false,
            [](CanvasDrawingSessionFixture const& f, Vector2 point, CanvasStrokeStyle* strokeStyle)
        {
            ThrowIfFailed(f.DS->DrawCircleWithBrush(point, 23, f.Brush.Get()));
        });

        // Null brush.
        Assert::AreEqual(E_INVALIDARG, CanvasDrawingSessionFixture().DS->DrawCircleWithBrush(Vector2{}, 0, nullptr));
    }

    TEST_METHOD_EX(CanvasDrawingSession_DrawCircleAtCoordsWithBrush)
    {
        TestDrawEllipse(false, 23, 23, 1, false,
            [](CanvasDrawingSessionFixture const& f, Vector2 point, CanvasStrokeStyle* strokeStyle)
        {
            ThrowIfFailed(f.DS->DrawCircleAtCoordsWithBrush(point.X, point.Y, 23, f.Brush.Get()));
        });

        // Null brush.
        Assert::AreEqual(E_INVALIDARG, CanvasDrawingSessionFixture().DS->DrawCircleAtCoordsWithBrush(0, 0, 0, nullptr));
    }

    TEST_METHOD_EX(CanvasDrawingSession_DrawCircleWithColor)
    {
        TestDrawEllipse(true, 23, 23, 1, false,
            [](CanvasDrawingSessionFixture const& f, Vector2 point, CanvasStrokeStyle* strokeStyle)
        {
            ThrowIfFailed(f.DS->DrawCircleWithColor(point, 23, ArbitraryMarkerColor1));
            ThrowIfFailed(f.DS->DrawCircleWithColor(point, 23, ArbitraryMarkerColor2));
        });
    }

    TEST_METHOD_EX(CanvasDrawingSession_DrawCircleAtCoordsWithColor)
    {
        TestDrawEllipse(true, 23, 23, 1, false,
            [](CanvasDrawingSessionFixture const& f, Vector2 point, CanvasStrokeStyle* strokeStyle)
        {
            ThrowIfFailed(f.DS->DrawCircleAtCoordsWithColor(point.X, point.Y, 23, ArbitraryMarkerColor1));
            ThrowIfFailed(f.DS->DrawCircleAtCoordsWithColor(point.X, point.Y, 23, ArbitraryMarkerColor2));
        });
    }

    TEST_METHOD_EX(CanvasDrawingSession_DrawCircleWithBrushAndStrokeWidth)
    {
        TestDrawEllipse(false, 23, 23, 123, false,
            [](CanvasDrawingSessionFixture const& f, Vector2 point, CanvasStrokeStyle* strokeStyle)
        {
            ThrowIfFailed(f.DS->DrawCircleWithBrushAndStrokeWidth(point, 23, f.Brush.Get(), 123));
        });

        // Null brush.
        Assert::AreEqual(E_INVALIDARG, CanvasDrawingSessionFixture().DS->DrawCircleWithBrushAndStrokeWidth(Vector2{}, 0, nullptr, 0));
    }

    TEST_METHOD_EX(CanvasDrawingSession_DrawCircleAtCoordsWithBrushAndStrokeWidth)
    {
        TestDrawEllipse(false, 23, 23, 123, false,
            [](CanvasDrawingSessionFixture const& f, Vector2 point, CanvasStrokeStyle* strokeStyle)
        {
            ThrowIfFailed(f.DS->DrawCircleAtCoordsWithBrushAndStrokeWidth(point.X, point.Y, 23, f.Brush.Get(), 123));
        });

        // Null brush.
        Assert::AreEqual(E_INVALIDARG, CanvasDrawingSessionFixture().DS->DrawCircleAtCoordsWithBrushAndStrokeWidth(0, 0, 0, nullptr, 0));
    }

    TEST_METHOD_EX(CanvasDrawingSession_DrawCircleWithColorAndStrokeWidth)
    {
        TestDrawEllipse(true, 23, 23, 123, false,
            [](CanvasDrawingSessionFixture const& f, Vector2 point, CanvasStrokeStyle* strokeStyle)
        {
            ThrowIfFailed(f.DS->DrawCircleWithColorAndStrokeWidth(point, 23, ArbitraryMarkerColor1, 123));
            ThrowIfFailed(f.DS->DrawCircleWithColorAndStrokeWidth(point, 23, ArbitraryMarkerColor2, 123));
        });
    }

    TEST_METHOD_EX(CanvasDrawingSession_DrawCircleAtCoordsWithColorAndStrokeWidth)
    {
        TestDrawEllipse(true, 23, 23, 123, false,
            [](CanvasDrawingSessionFixture const& f, Vector2 point, CanvasStrokeStyle* strokeStyle)
        {
            ThrowIfFailed(f.DS->DrawCircleAtCoordsWithColorAndStrokeWidth(point.X, point.Y, 23, ArbitraryMarkerColor1, 123));
            ThrowIfFailed(f.DS->DrawCircleAtCoordsWithColorAndStrokeWidth(point.X, point.Y, 23, ArbitraryMarkerColor2, 123));
        });
    }

    TEST_METHOD_EX(CanvasDrawingSession_DrawCircleWithBrushAndStrokeWidthAndStrokeStyle)
    {
        TestDrawEllipse(false, 23, 23, 123, true,
            [](CanvasDrawingSessionFixture const& f, Vector2 point, CanvasStrokeStyle* strokeStyle)
        {
            ThrowIfFailed(f.DS->DrawCircleWithBrushAndStrokeWidthAndStrokeStyle(point, 23, f.Brush.Get(), 123, strokeStyle));
        });

        // Null brush.
        Assert::AreEqual(E_INVALIDARG, CanvasDrawingSessionFixture().DS->DrawCircleWithBrushAndStrokeWidthAndStrokeStyle(Vector2{}, 0, nullptr, 0, nullptr));
    }

    TEST_METHOD_EX(CanvasDrawingSession_DrawCircleAtCoordsWithBrushAndStrokeWidthAndStrokeStyle)
    {
        TestDrawEllipse(false, 23, 23, 123, true,
            [](CanvasDrawingSessionFixture const& f, Vector2 point, CanvasStrokeStyle* strokeStyle)
        {
            ThrowIfFailed(f.DS->DrawCircleAtCoordsWithBrushAndStrokeWidthAndStrokeStyle(point.X, point.Y, 23, f.Brush.Get(), 123, strokeStyle));
        });

        // Null brush.
        Assert::AreEqual(E_INVALIDARG, CanvasDrawingSessionFixture().DS->DrawCircleAtCoordsWithBrushAndStrokeWidthAndStrokeStyle(0, 0, 0, nullptr, 0, nullptr));
    }

    TEST_METHOD_EX(CanvasDrawingSession_DrawCircleWithColorAndStrokeWidthAndStrokeStyle)
    {
        TestDrawEllipse(true, 23, 23, 123, true,
            [](CanvasDrawingSessionFixture const& f, Vector2 point, CanvasStrokeStyle* strokeStyle)
        {
            ThrowIfFailed(f.DS->DrawCircleWithColorAndStrokeWidthAndStrokeStyle(point, 23, ArbitraryMarkerColor1, 123, strokeStyle));
            ThrowIfFailed(f.DS->DrawCircleWithColorAndStrokeWidthAndStrokeStyle(point, 23, ArbitraryMarkerColor2, 123, strokeStyle));
        });
    }

    TEST_METHOD_EX(CanvasDrawingSession_DrawCircleAtCoordsWithColorAndStrokeWidthAndStrokeStyle)
    {
        TestDrawEllipse(true, 23, 23, 123, true,
            [](CanvasDrawingSessionFixture const& f, Vector2 point, CanvasStrokeStyle* strokeStyle)
        {
            ThrowIfFailed(f.DS->DrawCircleAtCoordsWithColorAndStrokeWidthAndStrokeStyle(point.X, point.Y, 23, ArbitraryMarkerColor1, 123, strokeStyle));
            ThrowIfFailed(f.DS->DrawCircleAtCoordsWithColorAndStrokeWidthAndStrokeStyle(point.X, point.Y, 23, ArbitraryMarkerColor2, 123, strokeStyle));
        });
    }


    //
    // FillCircle
    //

    TEST_METHOD_EX(CanvasDrawingSession_FillCircleWithBrush)
    {
        TestFillEllipse(false, 23, 23,
            [](CanvasDrawingSessionFixture const& f, Vector2 point)
        {
            ThrowIfFailed(f.DS->FillCircleWithBrush(point, 23, f.Brush.Get()));
        });

        // Null brush.
        Assert::AreEqual(E_INVALIDARG, CanvasDrawingSessionFixture().DS->FillCircleWithBrush(Vector2{}, 0, nullptr));
    }

    TEST_METHOD_EX(CanvasDrawingSession_FillCircleAtCoordsWithBrush)
    {
        TestFillEllipse(false, 23, 23,
            [](CanvasDrawingSessionFixture const& f, Vector2 point)
        {
            ThrowIfFailed(f.DS->FillCircleAtCoordsWithBrush(point.X, point.Y, 23, f.Brush.Get()));
        });

        // Null brush.
        Assert::AreEqual(E_INVALIDARG, CanvasDrawingSessionFixture().DS->FillCircleAtCoordsWithBrush(0, 0, 0, nullptr));
    }

    TEST_METHOD_EX(CanvasDrawingSession_FillCircleWithColor)
    {
        TestFillEllipse(true, 23, 23,
            [](CanvasDrawingSessionFixture const& f, Vector2 point)
        {
            ThrowIfFailed(f.DS->FillCircleWithColor(point, 23, ArbitraryMarkerColor1));
            ThrowIfFailed(f.DS->FillCircleWithColor(point, 23, ArbitraryMarkerColor2));
        });
    }

    TEST_METHOD_EX(CanvasDrawingSession_FillCircleAtCoordsWithColor)
    {
        TestFillEllipse(true, 23, 23,
            [](CanvasDrawingSessionFixture const& f, Vector2 point)
        {
            ThrowIfFailed(f.DS->FillCircleAtCoordsWithColor(point.X, point.Y, 23, ArbitraryMarkerColor1));
            ThrowIfFailed(f.DS->FillCircleAtCoordsWithColor(point.X, point.Y, 23, ArbitraryMarkerColor2));
        });
    }

    //
    // DrawGeometry
    //

    template<typename TDraw>
    void TestDrawGeometry(bool isColorOverload, float expectedStrokeWidth, bool expectStrokeStyle, TDraw const& callDrawFunction)
    {
        CanvasDrawingSessionFixture f;
        BrushValidator brushValidator(f, isColorOverload);

        int expectedDrawGeometryCalls = isColorOverload ? 2 : 1;

        ComPtr<ID2D1Geometry> nativeGeometryResource = f.Geometry->GetResource();

        auto canvasStrokeStyle = Make<CanvasStrokeStyle>();
        canvasStrokeStyle->put_LineJoin(CanvasLineJoin::MiterOrBevel);
        
        f.DeviceContext->DrawGeometryMethod.SetExpectedCalls(expectedDrawGeometryCalls,
            [&](ID2D1Geometry* geometry, ID2D1Brush* brush, float strokeWidth, ID2D1StrokeStyle* strokeStyle)
            {
                Assert::AreEqual(nativeGeometryResource.Get(), geometry);

                brushValidator.Check(brush);
                Assert::AreEqual(expectedStrokeWidth, strokeWidth);
                if (expectStrokeStyle)
                {
                    Assert::IsNotNull(strokeStyle);
                    Assert::AreEqual(D2D1_LINE_JOIN_MITER_OR_BEVEL, f.DeviceContext->m_factory->m_lineJoin);
                }
                else
                {
                    Assert::IsNull(strokeStyle);
                }
            });

        callDrawFunction(f, f.Geometry.Get(), canvasStrokeStyle.Get());
    }

    TEST_METHOD_EX(CanvasDrawingSession_DrawGeometryWithBrush)
    {
        TestDrawGeometry(false, 1, false,
            [](CanvasDrawingSessionFixture const& f, CanvasGeometry* geometry, CanvasStrokeStyle* strokeStyle)
            {
                ThrowIfFailed(f.DS->DrawGeometryWithBrush(geometry, f.Brush.Get()));
            });

        // Null brush or geometry.
        CanvasDrawingSessionFixture f;
        Assert::AreEqual(E_INVALIDARG, f.DS->DrawGeometryWithBrush(f.Geometry.Get(), nullptr));
        Assert::AreEqual(E_INVALIDARG, f.DS->DrawGeometryWithBrush(nullptr, f.Brush.Get()));
    }

    TEST_METHOD_EX(CanvasDrawingSession_DrawGeometryWithColor)
    {
        TestDrawGeometry(true, 1, false,
            [](CanvasDrawingSessionFixture const& f, CanvasGeometry* geometry, CanvasStrokeStyle* strokeStyle)
            {
                ThrowIfFailed(f.DS->DrawGeometryWithColor(geometry, ArbitraryMarkerColor1));
                ThrowIfFailed(f.DS->DrawGeometryWithColor(geometry, ArbitraryMarkerColor2));
            });
    }

    TEST_METHOD_EX(CanvasDrawingSession_DrawGeometryWithBrushAndStrokeWidth)
    {        
        TestDrawGeometry(false, 123, false,
            [](CanvasDrawingSessionFixture const& f, CanvasGeometry* geometry, CanvasStrokeStyle* strokeStyle)
            {
                ThrowIfFailed(f.DS->DrawGeometryWithBrushAndStrokeWidth(geometry, f.Brush.Get(), 123));
            });

        // Null brush or geometry.
        CanvasDrawingSessionFixture f;
        Assert::AreEqual(E_INVALIDARG, f.DS->DrawGeometryWithBrushAndStrokeWidth(f.Geometry.Get(), nullptr, 0));
        Assert::AreEqual(E_INVALIDARG, f.DS->DrawGeometryWithBrushAndStrokeWidth(nullptr, f.Brush.Get(), 0));
    }

    TEST_METHOD_EX(CanvasDrawingSession_DrawGeometryWithColorAndStrokeWidth)
    {        
        TestDrawGeometry(true, 123, false,
            [](CanvasDrawingSessionFixture const& f, CanvasGeometry* geometry, CanvasStrokeStyle* strokeStyle)
            {
                ThrowIfFailed(f.DS->DrawGeometryWithColorAndStrokeWidth(geometry, ArbitraryMarkerColor1, 123));
                ThrowIfFailed(f.DS->DrawGeometryWithColorAndStrokeWidth(geometry, ArbitraryMarkerColor2, 123));
            });
    }

    TEST_METHOD_EX(CanvasDrawingSession_DrawGeometryWithBrushAndStrokeWidthAndStrokeStyle)
    {
        TestDrawGeometry(false, 123, true,
            [](CanvasDrawingSessionFixture const& f, CanvasGeometry* geometry, CanvasStrokeStyle* strokeStyle)
            {
                ThrowIfFailed(f.DS->DrawGeometryWithBrushAndStrokeWidthAndStrokeStyle(geometry, f.Brush.Get(), 123, strokeStyle));
            });

        // Null brush or geometry.
        CanvasDrawingSessionFixture f;
        Assert::AreEqual(E_INVALIDARG, f.DS->DrawGeometryWithBrushAndStrokeWidthAndStrokeStyle(f.Geometry.Get(), nullptr, 0, nullptr));
        Assert::AreEqual(E_INVALIDARG, f.DS->DrawGeometryWithBrushAndStrokeWidthAndStrokeStyle(nullptr, f.Brush.Get(), 0, nullptr));
    }

    TEST_METHOD_EX(CanvasDrawingSession_DrawGeometryWithColorAndStrokeWidthAndStrokeStyle)
    {
        TestDrawGeometry(true, 123, true,
            [](CanvasDrawingSessionFixture const& f, CanvasGeometry* geometry, CanvasStrokeStyle* strokeStyle)
            {
                ThrowIfFailed(f.DS->DrawGeometryWithColorAndStrokeWidthAndStrokeStyle(geometry, ArbitraryMarkerColor1, 123, strokeStyle));
                ThrowIfFailed(f.DS->DrawGeometryWithColorAndStrokeWidthAndStrokeStyle(geometry, ArbitraryMarkerColor2, 123, strokeStyle));
            });
    }

    //
    // FillGeometry
    //    
    
    template<typename TDraw>
    void TestFillGeometry(bool isColorOverload, TDraw const& callDrawFunction)
    {
        CanvasDrawingSessionFixture f;
        BrushValidator brushValidator(f, isColorOverload);

        int expectedFillGeometryCount = isColorOverload ? 2 : 1;

        ComPtr<ID2D1Geometry> nativeGeometryResource = f.Geometry->GetResource();

        f.DeviceContext->FillGeometryMethod.SetExpectedCalls(expectedFillGeometryCount,
            [&](ID2D1Geometry* geometry, ID2D1Brush* brush, ID2D1Brush* opacityBrush)
            {
                Assert::AreEqual(nativeGeometryResource.Get(), geometry);
                brushValidator.Check(brush);

                Assert::IsNull(opacityBrush);
            });

        callDrawFunction(f, f.Geometry.Get());
    }

    TEST_METHOD_EX(CanvasDrawingSession_FillGeometryWithBrush)
    {        
        TestFillGeometry(false,
            [](CanvasDrawingSessionFixture const& f, CanvasGeometry* geometry)
            {
                ThrowIfFailed(f.DS->FillGeometryWithBrush(geometry, f.Brush.Get()));
            });

        // Null brush or geometry.
        CanvasDrawingSessionFixture f;
        Assert::AreEqual(E_INVALIDARG, f.DS->FillGeometryWithBrush(f.Geometry.Get(), nullptr));
        Assert::AreEqual(E_INVALIDARG, f.DS->FillGeometryWithBrush(nullptr, f.Brush.Get()));
    }

    TEST_METHOD_EX(CanvasDrawingSession_FillGeometryWithColor)
    {
        TestFillGeometry(true,
            [](CanvasDrawingSessionFixture const& f, CanvasGeometry* geometry)
            {
                ThrowIfFailed(f.DS->FillGeometryWithColor(geometry, ArbitraryMarkerColor1));
                ThrowIfFailed(f.DS->FillGeometryWithColor(geometry, ArbitraryMarkerColor2));
            });
    }

    //
    // DrawGeometryRealization
    //    

    template<typename TDraw>
    void TestDrawGeometryRealization(bool isColorOverload, TDraw const& callDrawFunction)
    {
        CanvasDrawingSessionFixture f;
        BrushValidator brushValidator(f, isColorOverload);

        int expectedDrawGeometryRealizationCount = isColorOverload ? 2 : 1;

        ComPtr<ID2D1GeometryRealization> nativeGeometryRealizationResource = f.CachedGeometry->GetResource();

        f.DeviceContext->DrawGeometryRealizationMethod.SetExpectedCalls(expectedDrawGeometryRealizationCount,
            [&](ID2D1GeometryRealization* geometryRealization, ID2D1Brush* brush)
            {
                Assert::AreEqual(nativeGeometryRealizationResource.Get(), geometryRealization);
                brushValidator.Check(brush);
            });

        callDrawFunction(f, f.CachedGeometry.Get());
    }

    TEST_METHOD_EX(CanvasDrawingSession_DrawGeometryRealizationWithBrush)
    {
        TestDrawGeometryRealization(false,
            [](CanvasDrawingSessionFixture const& f, CanvasCachedGeometry* cachedGeometry)
            {
                ThrowIfFailed(f.DS->DrawCachedGeometryWithBrush(cachedGeometry, f.Brush.Get()));
            });

        // Null brush or cached geometry.
        CanvasDrawingSessionFixture f;
        Assert::AreEqual(E_INVALIDARG, f.DS->DrawCachedGeometryWithBrush(f.CachedGeometry.Get(), nullptr));
        Assert::AreEqual(E_INVALIDARG, f.DS->DrawCachedGeometryWithBrush(nullptr, f.Brush.Get()));
    }

    TEST_METHOD_EX(CanvasDrawingSession_DrawGeometryRealizationWithColor)
    {
        TestDrawGeometryRealization(true,
            [](CanvasDrawingSessionFixture const& f, CanvasCachedGeometry* cachedGeometry)
            {
                ThrowIfFailed(f.DS->DrawCachedGeometryWithColor(cachedGeometry, ArbitraryMarkerColor1));
                ThrowIfFailed(f.DS->DrawCachedGeometryWithColor(cachedGeometry, ArbitraryMarkerColor2));
            });
    }

    TEST_METHOD_EX(CanvasDrawingSession_StateGettersWithNull)
    {
        CanvasDrawingSessionFixture f;

        Assert::AreEqual(E_INVALIDARG, f.DS->get_Antialiasing(nullptr));
        Assert::AreEqual(E_INVALIDARG, f.DS->get_Blend(nullptr));
        Assert::AreEqual(E_INVALIDARG, f.DS->get_TextAntialiasing(nullptr));
        Assert::AreEqual(E_INVALIDARG, f.DS->get_Transform(nullptr));
        Assert::AreEqual(E_INVALIDARG, f.DS->get_Units(nullptr));
    }

    TEST_METHOD_EX(CanvasDrawingSession_StateProperties)
    {
        CanvasDrawingSessionFixture f;

        {
            f.DeviceContext->SetAntialiasModeMethod.SetExpectedCalls(1,
                [](const D2D1_ANTIALIAS_MODE m)
                {
                    Assert::AreEqual(m, D2D1_ANTIALIAS_MODE_ALIASED);
                });
            ThrowIfFailed(f.DS->put_Antialiasing(CanvasAntialiasing_Aliased));
        }

        {
            f.DeviceContext->GetAntialiasModeMethod.SetExpectedCalls(1,
                [] { return D2D1_ANTIALIAS_MODE_ALIASED; });

            CanvasAntialiasing antialiasMode;
            ThrowIfFailed(f.DS->get_Antialiasing(&antialiasMode));
            Assert::AreEqual(CanvasAntialiasing_Aliased, antialiasMode);
        }

        {
            f.DeviceContext->SetPrimitiveBlendMethod.SetExpectedCalls(1,
                [](const D2D1_PRIMITIVE_BLEND b)
                {
                    Assert::AreEqual(b, D2D1_PRIMITIVE_BLEND_MIN);
                });

            ThrowIfFailed(f.DS->put_Blend(CanvasBlend_Min));
        }

        {
            f.DeviceContext->GetPrimitiveBlendMethod.SetExpectedCalls(1,
                [] { return D2D1_PRIMITIVE_BLEND_COPY; });

            CanvasBlend blend;
            ThrowIfFailed(f.DS->get_Blend(&blend));
            Assert::AreEqual(CanvasBlend_Copy, blend);
        }

        {
            f.DeviceContext->SetTextAntialiasModeMethod.SetExpectedCalls(1,
                [&](const D2D1_TEXT_ANTIALIAS_MODE m)
                {
                    Assert::AreEqual(m, D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE);
                });

            ThrowIfFailed(f.DS->put_TextAntialiasing(CanvasTextAntialiasing_ClearType));
        }

        {
            f.DeviceContext->GetTextAntialiasModeMethod.SetExpectedCalls(1,
                [] { return D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE; });

            CanvasTextAntialiasing textAntialiasing;
            ThrowIfFailed(f.DS->get_TextAntialiasing(&textAntialiasing));
            Assert::AreEqual(CanvasTextAntialiasing_Grayscale, textAntialiasing);
        }

        {
            Numerics::Matrix3x2 matrix = { 1, 2, 3, 5, 8, 13 };
            f.DeviceContext->SetTransformMethod.SetExpectedCalls(1,
                [&](const D2D1_MATRIX_3X2_F* m)
                {
                    const D2D1_MATRIX_3X2_F* asD2DMatrix = reinterpret_cast<D2D1_MATRIX_3X2_F*>(&matrix);
                    Assert::AreEqual(*asD2DMatrix, *m);
                });

            ThrowIfFailed(f.DS->put_Transform(matrix));
        }

        {
            Numerics::Matrix3x2 matrix = { 13, 8, 5, 3, 2, 1 };
            f.DeviceContext->GetTransformMethod.SetExpectedCalls(1,
                [&](D2D1_MATRIX_3X2_F* m)
                {
                    const D2D1_MATRIX_3X2_F* asD2DMatrix = reinterpret_cast<D2D1_MATRIX_3X2_F*>(&matrix);
                    *m = *asD2DMatrix;
                });

            Numerics::Matrix3x2 transform;
            ThrowIfFailed(f.DS->get_Transform(&transform));
            Assert::AreEqual(matrix, transform);
        }

        {
            f.DeviceContext->SetUnitModeMethod.SetExpectedCalls(1,
                [](const D2D1_UNIT_MODE m)
                {
                    Assert::AreEqual(m, D2D1_UNIT_MODE_PIXELS);
                });

            ThrowIfFailed(f.DS->put_Units(CanvasUnits_Pixels));
        }

        {
            f.DeviceContext->GetUnitModeMethod.SetExpectedCalls(1,
                [] { return D2D1_UNIT_MODE_PIXELS; });

            CanvasUnits units;
            ThrowIfFailed(f.DS->get_Units(&units));
            Assert::AreEqual(CanvasUnits_Pixels, units);
        }
    }

    TEST_METHOD_EX(CanvasDrawingSession_SiSOffsetIsHiddenFromTransformProperty)
    {
        auto deviceContext = Make<StubD2DDeviceContextWithGetFactory>();

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

        deviceContext->SetTransformMethod.AllowAnyCall(
            [&](const D2D1_MATRIX_3X2_F* m)
            {
                nativeTransform = *m;
            });

        deviceContext->GetTransformMethod.AllowAnyCall(
            [&](D2D1_MATRIX_3X2_F* m)
            {
                *m = nativeTransform;
            });
        
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

    TEST_METHOD_EX(CanvasDrawingSession_get_Device)
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

        for (auto& testCase : testCases)
        {
            if (testCase.ExpectGetDeviceCalled)
            {
                d2dDeviceContext->GetDeviceMethod.SetExpectedCalls(1,
                    [&](ID2D1Device** d2dDevice)
                    {
                        ComPtr<StubD2DDevice> stubDevice = Make<StubD2DDevice>();
                        stubDevice.CopyTo(d2dDevice);
                    });
            }
            else
            {
                d2dDeviceContext->GetDeviceMethod.SetExpectedCalls(0);
            }

            auto manager = std::make_shared<CanvasDrawingSessionManager>();
            auto drawingSession = manager->Create(
                testCase.ManagerDevice,
                d2dDeviceContext.Get(),
                std::make_shared<StubCanvasDrawingSessionAdapter>());

            ComPtr<ICanvasDevice> deviceVerify;
            ThrowIfFailed(drawingSession->get_Device(&deviceVerify));

            Assert::IsNotNull(deviceVerify.Get());

            if (testCase.ManagerDevice)
            {
                Assert::AreEqual(testCase.ManagerDevice, deviceVerify.Get());
            }

            // Do get_Device again. Even in the interop case, it should never 
            // cause GetDevice to be called again.
            ComPtr<ICanvasDevice> deviceReverify;
            ThrowIfFailed(drawingSession->get_Device(&deviceReverify));

            Assert::AreEqual(deviceVerify.Get(), deviceReverify.Get());
        }
    }

    TEST_METHOD_EX(CanvasDrawingSession_DpiProperties)
    {
        const float dpi = 144;

        auto deviceContext = Make<StubD2DDeviceContextWithGetFactory>();
        auto manager = std::make_shared<CanvasDrawingSessionManager>();
        auto adapter = std::make_shared<CanvasDrawingSessionAdapter_ChangeableOffset>();
        auto drawingSession = manager->Create(deviceContext.Get(), adapter);

        deviceContext->GetDpiMethod.SetExpectedCalls(3, [&](float* dpiX, float* dpiY)
        {
            *dpiX = dpi;
            *dpiY = dpi;
        });

        float actualDpi = 0;
        ThrowIfFailed(drawingSession->get_Dpi(&actualDpi));
        Assert::AreEqual(dpi, actualDpi);

        const float testValue = 100;

        int pixels = 0;
        ThrowIfFailed(drawingSession->ConvertDipsToPixels(testValue, &pixels));
        Assert::AreEqual((int)(testValue * dpi / DEFAULT_DPI), pixels);

        float dips = 0;
        ThrowIfFailed(drawingSession->ConvertPixelsToDips((int)testValue, &dips));
        Assert::AreEqual(testValue * DEFAULT_DPI / dpi, dips);
    }
};

TEST_CLASS(CanvasDrawingSession_DrawTextTests)
{
    class Fixture : public CanvasDrawingSessionFixture
    {
    public:
        ComPtr<CanvasTextFormat> Format;

        Fixture()
        {
            Format = Make<CanvasTextFormat>();
        }

        template<typename FORMAT_VALIDATOR>
        void Expect(int numCalls, std::wstring expectedText, D2D1_RECT_F expectedRect, D2D1_DRAW_TEXT_OPTIONS expectedOptions, FORMAT_VALIDATOR&& formatValidator)
        {            
            DeviceContext->DrawTextMethod.SetExpectedCalls(numCalls,
                [=](wchar_t const* actualText,
                    uint32_t actualTextLength,
                    IDWriteTextFormat* format,
                    D2D1_RECT_F const* actualRect,
                    ID2D1Brush* actualBrush,
                    D2D1_DRAW_TEXT_OPTIONS actualOptions,
                    DWRITE_MEASURING_MODE)
                {
                    Assert::AreEqual(expectedText.c_str(), actualText);
                    Assert::AreEqual<size_t>(expectedText.size(), actualTextLength);
                    Assert::IsNotNull(format);
                    Assert::AreEqual(expectedRect, *actualRect);
                    Assert::AreEqual(expectedOptions, actualOptions);

                    formatValidator(format, actualBrush);
                });
        }
    };

    template<typename TDraw>
    void TestDrawText(bool hasTextFormat, bool isRectVersion, bool isColorOverload, D2D1_RECT_F expectedRect, TDraw const& callDrawFunction)
    {
        // Try first with a real test string, then again drawing the empty string.
        for (int pass = 0; pass < 2; pass++)
        {
            auto expectedText = (pass == 0) ? L"how can you have any pudding?" : L"";

            Fixture f;
            BrushValidator brushValidator(f, isColorOverload);

            CanvasWordWrapping expectedWordWrapping = hasTextFormat ? CanvasWordWrapping::WholeWord : CanvasWordWrapping::Wrap;
            auto expectedFontFamilyName = hasTextFormat ? L"Ariel" : L"Segoe UI";
            float expectedFontSize = hasTextFormat ? 123.0f : 20.0f;

            if (hasTextFormat)
            {
                ThrowIfFailed(f.Format->put_WordWrapping(expectedWordWrapping));
                ThrowIfFailed(f.Format->put_FontFamily(WinString(expectedFontFamilyName)));
                ThrowIfFailed(f.Format->put_FontSize(expectedFontSize));
                ThrowIfFailed(f.Format->put_Options(CanvasDrawTextOptions::Clip));
            }

            int expectedDrawTextCalls = isColorOverload ? 2 : 1;

            f.Expect(
                expectedDrawTextCalls,
                expectedText,
                expectedRect,
                hasTextFormat ? D2D1_DRAW_TEXT_OPTIONS_CLIP : D2D1_DRAW_TEXT_OPTIONS_NONE,
                [&](IDWriteTextFormat* format, ID2D1Brush* brush)
                {
                    Assert::AreEqual(isRectVersion ? (DWRITE_WORD_WRAPPING)expectedWordWrapping : DWRITE_WORD_WRAPPING_NO_WRAP, format->GetWordWrapping());

                    auto length = format->GetFontFamilyNameLength() + 1; // + 1 for NULL terminator
                    std::vector<wchar_t> buf(length);
                    ThrowIfFailed(format->GetFontFamilyName(&buf.front(), length));
                    auto actualFontFamilyName = std::wstring(buf.begin(), buf.end() - 1); // - 1 since we don't want NULL terminator in string

                    Assert::AreEqual(expectedFontFamilyName, actualFontFamilyName.c_str());
                    Assert::AreEqual(expectedFontSize, format->GetFontSize());

                    brushValidator.Check(brush);
                });

            callDrawFunction(f, WinString(expectedText));

            // After the call we expect the word wrapping to be how we left it
            CanvasWordWrapping actualWordWrapping{};
            ThrowIfFailed(f.Format->get_WordWrapping(&actualWordWrapping));
            Assert::AreEqual(expectedWordWrapping, actualWordWrapping);
        }
    }

    TEST_METHOD_EX(CanvasDrawingSession_DrawTextAtPointWithColor)
    {
        TestDrawText(false, false, true, D2D1_RECT_F{ 23, 42, 23, 42 },
            [](Fixture const& f, HSTRING text)
        {
            ThrowIfFailed(f.DS->DrawTextAtPointWithColor(text, Vector2{ 23, 42 }, ArbitraryMarkerColor1));
            ThrowIfFailed(f.DS->DrawTextAtPointWithColor(text, Vector2{ 23, 42 }, ArbitraryMarkerColor2));
        });
    }
    
    TEST_METHOD_EX(CanvasDrawingSession_DrawTextAtPointCoordsWithColor)
    {
        TestDrawText(false, false, true, D2D1_RECT_F{ 23, 42, 23, 42 },
            [](Fixture const& f, HSTRING text)
        {
            ThrowIfFailed(f.DS->DrawTextAtPointCoordsWithColor(text, 23, 42, ArbitraryMarkerColor1));
            ThrowIfFailed(f.DS->DrawTextAtPointCoordsWithColor(text, 23, 42, ArbitraryMarkerColor2));
        });
    }
    
    TEST_METHOD_EX(CanvasDrawingSession_DrawTextAtPointWithBrushAndFormat)
    {
        // Full version.
        TestDrawText(true, false, false, D2D1_RECT_F{ 23, 42, 23, 42 },
            [](Fixture const& f, HSTRING text)
        {
            ThrowIfFailed(f.DS->DrawTextAtPointWithBrushAndFormat(text, Vector2{ 23, 42 }, f.Brush.Get(), f.Format.Get()));
        });

        // Null text format.
        TestDrawText(false, false, false, D2D1_RECT_F{ 23, 42, 23, 42 },
            [](Fixture const& f, HSTRING text)
        {
            ThrowIfFailed(f.DS->DrawTextAtPointWithBrushAndFormat(text, Vector2{ 23, 42 }, f.Brush.Get(), nullptr));
        });
        
        // Null brush.
        Assert::AreEqual(E_INVALIDARG, CanvasDrawingSessionFixture().DS->DrawTextAtPointWithBrushAndFormat(WinString(), Vector2{}, nullptr, nullptr));
    }

    TEST_METHOD_EX(CanvasDrawingSession_DrawTextAtRectWithBrushAndFormat)
    {
        // Full version.
        TestDrawText(true, true, false, D2D1_RECT_F{ 1, 2, 4, 6 },
            [](Fixture const& f, HSTRING text)
        {
            ThrowIfFailed(f.DS->DrawTextAtRectWithBrushAndFormat(text, Rect{ 1, 2, 3, 4 }, f.Brush.Get(), f.Format.Get()));
        });

        // Null text format.
        TestDrawText(false, true, false, D2D1_RECT_F{ 1, 2, 4, 6 },
            [](Fixture const& f, HSTRING text)
        {
            ThrowIfFailed(f.DS->DrawTextAtRectWithBrushAndFormat(text, Rect{ 1, 2, 3, 4 }, f.Brush.Get(), nullptr));
        });
        
        // Null brush.
        Assert::AreEqual(E_INVALIDARG, CanvasDrawingSessionFixture().DS->DrawTextAtRectWithBrushAndFormat(WinString(), Rect{}, nullptr, nullptr));
    }
    
    TEST_METHOD_EX(CanvasDrawingSession_DrawTextAtPointCoordsWithBrushAndFormat)
    {
        // Full version.
        TestDrawText(true, false, false, D2D1_RECT_F{ 23, 42, 23, 42 },
            [](Fixture const& f, HSTRING text)
        {
            ThrowIfFailed(f.DS->DrawTextAtPointCoordsWithBrushAndFormat(text, 23, 42, f.Brush.Get(), f.Format.Get()));
        });

        // Null text format.
        TestDrawText(false, false, false, D2D1_RECT_F{ 23, 42, 23, 42 },
            [](Fixture const& f, HSTRING text)
        {
            ThrowIfFailed(f.DS->DrawTextAtPointCoordsWithBrushAndFormat(text, 23, 42, f.Brush.Get(), nullptr));
        });
        
        // Null brush.
        Assert::AreEqual(E_INVALIDARG, CanvasDrawingSessionFixture().DS->DrawTextAtPointCoordsWithBrushAndFormat(WinString(), 0, 0, nullptr, nullptr));
    }
    
    TEST_METHOD_EX(CanvasDrawingSession_DrawTextAtRectCoordsWithBrushAndFormat)
    {
        // Full version.
        TestDrawText(true, true, false, D2D1_RECT_F{ 1, 2, 4, 6 },
            [](Fixture const& f, HSTRING text)
        {
            ThrowIfFailed(f.DS->DrawTextAtRectCoordsWithBrushAndFormat(text, 1, 2, 3, 4, f.Brush.Get(), f.Format.Get()));
        });

        // Null text format.
        TestDrawText(false, true, false, D2D1_RECT_F{ 1, 2, 4, 6 },
            [](Fixture const& f, HSTRING text)
        {
            ThrowIfFailed(f.DS->DrawTextAtRectCoordsWithBrushAndFormat(text, 1, 2, 3, 4, f.Brush.Get(), nullptr));
        });
        
        // Null brush.
        Assert::AreEqual(E_INVALIDARG, CanvasDrawingSessionFixture().DS->DrawTextAtRectCoordsWithBrushAndFormat(WinString(), 0, 0, 0, 0, nullptr, nullptr));
    }
    
    TEST_METHOD_EX(CanvasDrawingSession_DrawTextAtPointWithColorAndFormat)
    {
        // Full version.
        TestDrawText(true, false, true, D2D1_RECT_F{ 23, 42, 23, 42 },
            [](Fixture const& f, HSTRING text)
        {
            ThrowIfFailed(f.DS->DrawTextAtPointWithColorAndFormat(text, Vector2{ 23, 42 }, ArbitraryMarkerColor1, f.Format.Get()));
            ThrowIfFailed(f.DS->DrawTextAtPointWithColorAndFormat(text, Vector2{ 23, 42 }, ArbitraryMarkerColor2, f.Format.Get()));
        });

        // Null text format.
        TestDrawText(false, false, true, D2D1_RECT_F{ 23, 42, 23, 42 },
            [](Fixture const& f, HSTRING text)
        {
            ThrowIfFailed(f.DS->DrawTextAtPointWithColorAndFormat(text, Vector2{ 23, 42 }, ArbitraryMarkerColor1, nullptr));
            ThrowIfFailed(f.DS->DrawTextAtPointWithColorAndFormat(text, Vector2{ 23, 42 }, ArbitraryMarkerColor2, nullptr));
        });
    }
    
    TEST_METHOD_EX(CanvasDrawingSession_DrawTextAtRectWithColorAndFormat)
    {
        // Full version.
        TestDrawText(true, true, true, D2D1_RECT_F{ 1, 2, 4, 6 },
            [](Fixture const& f, HSTRING text)
        {
            ThrowIfFailed(f.DS->DrawTextAtRectWithColorAndFormat(text, Rect{ 1, 2, 3, 4 }, ArbitraryMarkerColor1, f.Format.Get()));
            ThrowIfFailed(f.DS->DrawTextAtRectWithColorAndFormat(text, Rect{ 1, 2, 3, 4 }, ArbitraryMarkerColor2, f.Format.Get()));
        });

        // Null text format.
        TestDrawText(false, true, true, D2D1_RECT_F{ 1, 2, 4, 6 },
            [](Fixture const& f, HSTRING text)
        {
            ThrowIfFailed(f.DS->DrawTextAtRectWithColorAndFormat(text, Rect{ 1, 2, 3, 4 }, ArbitraryMarkerColor1, nullptr));
            ThrowIfFailed(f.DS->DrawTextAtRectWithColorAndFormat(text, Rect{ 1, 2, 3, 4 }, ArbitraryMarkerColor2, nullptr));
        });
    }
    
    TEST_METHOD_EX(CanvasDrawingSession_DrawTextAtPointCoordsWithColorAndFormat)
    {
        // Full version.
        TestDrawText(true, false, true, D2D1_RECT_F{ 23, 42, 23, 42 },
            [](Fixture const& f, HSTRING text)
        {
            ThrowIfFailed(f.DS->DrawTextAtPointCoordsWithColorAndFormat(text, 23, 42, ArbitraryMarkerColor1, f.Format.Get()));
            ThrowIfFailed(f.DS->DrawTextAtPointCoordsWithColorAndFormat(text, 23, 42, ArbitraryMarkerColor2, f.Format.Get()));
        });

        // Null text format.
        TestDrawText(false, false, true, D2D1_RECT_F{ 23, 42, 23, 42 },
            [](Fixture const& f, HSTRING text)
        {
            ThrowIfFailed(f.DS->DrawTextAtPointCoordsWithColorAndFormat(text, 23, 42, ArbitraryMarkerColor1, nullptr));
            ThrowIfFailed(f.DS->DrawTextAtPointCoordsWithColorAndFormat(text, 23, 42, ArbitraryMarkerColor2, nullptr));
        });
    }
    
    TEST_METHOD_EX(CanvasDrawingSession_DrawTextAtRectCoordsWithColorAndFormat)
    {
        // Full version.
        TestDrawText(true, true, true, D2D1_RECT_F{ 1, 2, 4, 6 },
            [](Fixture const& f, HSTRING text)
        {
            ThrowIfFailed(f.DS->DrawTextAtRectCoordsWithColorAndFormat(text, 1, 2, 3, 4, ArbitraryMarkerColor1, f.Format.Get()));
            ThrowIfFailed(f.DS->DrawTextAtRectCoordsWithColorAndFormat(text, 1, 2, 3, 4, ArbitraryMarkerColor2, f.Format.Get()));
        });

        // Null text format.
        TestDrawText(false, true, true, D2D1_RECT_F{ 1, 2, 4, 6 },
            [](Fixture const& f, HSTRING text)
        {
            ThrowIfFailed(f.DS->DrawTextAtRectCoordsWithColorAndFormat(text, 1, 2, 3, 4, ArbitraryMarkerColor1, nullptr));
            ThrowIfFailed(f.DS->DrawTextAtRectCoordsWithColorAndFormat(text, 1, 2, 3, 4, ArbitraryMarkerColor2, nullptr));
        });
    }
};


TEST_CLASS(CanvasDrawingSession_CloseTests)
{
    TEST_METHOD_EX(CanvasDrawingSession_Close_ReleasesDeviceContextAndOtherMethodsFail)
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

        EXPECT_OBJECT_CLOSED(canvasDrawingSession->DrawImage(nullptr, Vector2{}));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->DrawImageAtCoords(nullptr, 0, 0));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->DrawImageAtOrigin(nullptr));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->DrawImageWithSourceRect(nullptr, Vector2{}, Rect{}));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->DrawImageWithSourceRectAndInterpolation(nullptr, Vector2{}, Rect{}, CanvasImageInterpolation::NearestNeighbor));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->DrawImageWithSourceRectAndInterpolationAndComposite(nullptr, Vector2{}, Rect{}, CanvasImageInterpolation::NearestNeighbor, CanvasComposite::SourceOver));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->DrawImageAtCoordsWithSourceRect(nullptr, 0, 0, Rect{}));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->DrawImageAtCoordsWithSourceRectAndInterpolation(nullptr, 0, 0, Rect{}, CanvasImageInterpolation::NearestNeighbor));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->DrawImageAtCoordsWithSourceRectAndInterpolationAndComposite(nullptr, 0, 0, Rect{}, CanvasImageInterpolation::NearestNeighbor, CanvasComposite::SourceOver));

        EXPECT_OBJECT_CLOSED(canvasDrawingSession->DrawBitmapWithDestRect(nullptr, Rect{}));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->DrawBitmapWithDestRectAndSourceRect(nullptr, Rect{}, Rect{}));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->DrawBitmapWithDestRectAndSourceRectAndOpacity(nullptr, Rect{}, Rect{}, 0));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->DrawBitmapWithDestRectAndSourceRectAndOpacityAndInterpolation(nullptr, Rect{}, Rect{}, 0, CanvasImageInterpolation::NearestNeighbor));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->DrawBitmapWithDestRectAndSourceRectAndOpacityAndInterpolationAndPerspective(nullptr, Rect{}, Rect{}, 0, CanvasImageInterpolation::NearestNeighbor, Numerics::Matrix4x4{}));

        EXPECT_OBJECT_CLOSED(canvasDrawingSession->DrawLineWithBrush(Vector2{}, Vector2{}, nullptr));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->DrawLineAtCoordsWithBrush(0, 0, 0, 0, nullptr));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->DrawLineWithColor(Vector2{}, Vector2{}, Color{}));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->DrawLineAtCoordsWithColor(0, 0, 0, 0, Color{}));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->DrawLineWithBrushAndStrokeWidth(Vector2{}, Vector2{}, nullptr, 0));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->DrawLineAtCoordsWithBrushAndStrokeWidth(0, 0, 0, 0, nullptr, 0));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->DrawLineWithColorAndStrokeWidth(Vector2{}, Vector2{}, Color{}, 0));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->DrawLineAtCoordsWithColorAndStrokeWidth(0, 0, 0, 0, Color{}, 0));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->DrawLineWithBrushAndStrokeWidthAndStrokeStyle(Vector2{}, Vector2{}, nullptr, 0, nullptr));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->DrawLineAtCoordsWithBrushAndStrokeWidthAndStrokeStyle(0, 0, 0, 0, nullptr, 0, nullptr));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->DrawLineWithColorAndStrokeWidthAndStrokeStyle(Vector2{}, Vector2{}, Color{}, 0, nullptr));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->DrawLineAtCoordsWithColorAndStrokeWidthAndStrokeStyle(0, 0, 0, 0, Color{}, 0, nullptr));

        EXPECT_OBJECT_CLOSED(canvasDrawingSession->DrawRectangleWithBrush(Rect{}, nullptr));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->DrawRectangleAtCoordsWithBrush(0, 0, 0, 0, nullptr));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->DrawRectangleWithColor(Rect{}, Color{}));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->DrawRectangleAtCoordsWithColor(0, 0, 0, 0, Color{}));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->DrawRectangleWithBrushAndStrokeWidth(Rect{}, nullptr, 0));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->DrawRectangleAtCoordsWithBrushAndStrokeWidth(0, 0, 0, 0, nullptr, 0));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->DrawRectangleWithColorAndStrokeWidth(Rect{}, Color{}, 0));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->DrawRectangleAtCoordsWithColorAndStrokeWidth(0, 0, 0, 0, Color{}, 0));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->DrawRectangleWithBrushAndStrokeWidthAndStrokeStyle(Rect{}, nullptr, 0, nullptr));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->DrawRectangleAtCoordsWithBrushAndStrokeWidthAndStrokeStyle(0, 0, 0, 0, nullptr, 0, nullptr));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->DrawRectangleWithColorAndStrokeWidthAndStrokeStyle(Rect{}, Color{}, 0, nullptr));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->DrawRectangleAtCoordsWithColorAndStrokeWidthAndStrokeStyle(0, 0, 0, 0, Color{}, 0, nullptr));

        EXPECT_OBJECT_CLOSED(canvasDrawingSession->FillRectangleWithBrush(Rect{}, nullptr));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->FillRectangleAtCoordsWithBrush(0, 0, 0, 0, nullptr));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->FillRectangleWithColor(Rect{}, Color{}));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->FillRectangleAtCoordsWithColor(0, 0, 0, 0, Color{}));

        EXPECT_OBJECT_CLOSED(canvasDrawingSession->DrawRoundedRectangleWithBrush(Rect{}, 0, 0, nullptr));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->DrawRoundedRectangleAtCoordsWithBrush(0, 0, 0, 0, 0, 0, nullptr));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->DrawRoundedRectangleWithColor(Rect{}, 0, 0, Color{}));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->DrawRoundedRectangleAtCoordsWithColor(0, 0, 0, 0, 0, 0, Color{}));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->DrawRoundedRectangleWithBrushAndStrokeWidth(Rect{}, 0, 0, nullptr, 0));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->DrawRoundedRectangleAtCoordsWithBrushAndStrokeWidth(0, 0, 0, 0, 0, 0, nullptr, 0));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->DrawRoundedRectangleWithColorAndStrokeWidth(Rect{}, 0, 0, Color{}, 0));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->DrawRoundedRectangleAtCoordsWithColorAndStrokeWidth(0, 0, 0, 0, 0, 0, Color{}, 0));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->DrawRoundedRectangleWithBrushAndStrokeWidthAndStrokeStyle(Rect{}, 0, 0, nullptr, 0, nullptr));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->DrawRoundedRectangleAtCoordsWithBrushAndStrokeWidthAndStrokeStyle(0, 0, 0, 0, 0, 0, nullptr, 0, nullptr));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->DrawRoundedRectangleWithColorAndStrokeWidthAndStrokeStyle(Rect{}, 0, 0, Color{}, 0, nullptr));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->DrawRoundedRectangleAtCoordsWithColorAndStrokeWidthAndStrokeStyle(0, 0, 0, 0, 0, 0, Color{}, 0, nullptr));
        
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->FillRoundedRectangleWithBrush(Rect{}, 0, 0, nullptr));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->FillRoundedRectangleAtCoordsWithBrush(0, 0, 0, 0, 0, 0, nullptr));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->FillRoundedRectangleWithColor(Rect{}, 0, 0, Color{}));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->FillRoundedRectangleAtCoordsWithColor(0, 0, 0, 0, 0, 0, Color{}));
        
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->DrawEllipseWithBrush(Vector2{}, 0, 0, nullptr));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->DrawEllipseAtCoordsWithBrush(0, 0, 0, 0, nullptr));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->DrawEllipseWithColor(Vector2{}, 0, 0, Color{}));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->DrawEllipseAtCoordsWithColor(0, 0, 0, 0, Color{}));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->DrawEllipseWithBrushAndStrokeWidth(Vector2{}, 0, 0, nullptr, 0));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->DrawEllipseAtCoordsWithBrushAndStrokeWidth(0, 0, 0, 0, nullptr, 0));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->DrawEllipseWithColorAndStrokeWidth(Vector2{}, 0, 0, Color{}, 0));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->DrawEllipseAtCoordsWithColorAndStrokeWidth(0, 0, 0, 0, Color{}, 0));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->DrawEllipseWithBrushAndStrokeWidthAndStrokeStyle(Vector2{}, 0, 0, nullptr, 0, nullptr));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->DrawEllipseAtCoordsWithBrushAndStrokeWidthAndStrokeStyle(0, 0, 0, 0, nullptr, 0, nullptr));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->DrawEllipseWithColorAndStrokeWidthAndStrokeStyle(Vector2{}, 0, 0, Color{}, 0, nullptr));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->DrawEllipseAtCoordsWithColorAndStrokeWidthAndStrokeStyle(0, 0, 0, 0, Color{}, 0, nullptr));

        EXPECT_OBJECT_CLOSED(canvasDrawingSession->FillEllipseWithBrush(Vector2{}, 0, 0, nullptr));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->FillEllipseAtCoordsWithBrush(0, 0, 0, 0, nullptr));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->FillEllipseWithColor(Vector2{}, 0, 0, Color{}));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->FillEllipseAtCoordsWithColor(0, 0, 0, 0, Color{}));
        
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->DrawCircleWithBrush(Vector2{}, 0, nullptr));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->DrawCircleAtCoordsWithBrush(0, 0, 0, nullptr));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->DrawCircleWithColor(Vector2{}, 0, Color{}));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->DrawCircleAtCoordsWithColor(0, 0, 0, Color{}));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->DrawCircleWithBrushAndStrokeWidth(Vector2{}, 0, nullptr, 0));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->DrawCircleAtCoordsWithBrushAndStrokeWidth(0, 0, 0, nullptr, 0));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->DrawCircleWithColorAndStrokeWidth(Vector2{}, 0, Color{}, 0));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->DrawCircleAtCoordsWithColorAndStrokeWidth(0, 0, 0, Color{}, 0));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->DrawCircleWithBrushAndStrokeWidthAndStrokeStyle(Vector2{}, 0, nullptr, 0, nullptr));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->DrawCircleAtCoordsWithBrushAndStrokeWidthAndStrokeStyle(0, 0, 0, nullptr, 0, nullptr));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->DrawCircleWithColorAndStrokeWidthAndStrokeStyle(Vector2{}, 0, Color{}, 0, nullptr));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->DrawCircleAtCoordsWithColorAndStrokeWidthAndStrokeStyle(0, 0, 0, Color{}, 0, nullptr));

        EXPECT_OBJECT_CLOSED(canvasDrawingSession->FillCircleWithBrush(Vector2{}, 0, nullptr));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->FillCircleAtCoordsWithBrush(0, 0, 0, nullptr));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->FillCircleWithColor(Vector2{}, 0, Color{}));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->FillCircleAtCoordsWithColor(0, 0, 0, Color{}));

        EXPECT_OBJECT_CLOSED(canvasDrawingSession->DrawTextAtPointWithColor(nullptr, Vector2{}, Color{}));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->DrawTextAtPointCoordsWithColor(nullptr, 0, 0, Color{}));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->DrawTextAtPointWithBrushAndFormat(nullptr, Vector2{}, nullptr, nullptr));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->DrawTextAtRectWithBrushAndFormat(nullptr, Rect{}, nullptr, nullptr));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->DrawTextAtPointCoordsWithBrushAndFormat(nullptr, 0, 0, nullptr, nullptr));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->DrawTextAtRectCoordsWithBrushAndFormat(nullptr, 0, 0, 0, 0, nullptr, nullptr));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->DrawTextAtPointWithColorAndFormat(nullptr, Vector2{}, Color{}, nullptr));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->DrawTextAtRectWithColorAndFormat(nullptr, Rect{}, Color{}, nullptr));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->DrawTextAtPointCoordsWithColorAndFormat(nullptr, 0, 0, Color{}, nullptr));
        EXPECT_OBJECT_CLOSED(canvasDrawingSession->DrawTextAtRectCoordsWithColorAndFormat(nullptr, 0, 0, 0, 0, Color{}, nullptr));

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

    TEST_METHOD_EX(CanvasDrawingSession_Implements_ExpectedInterfaces)
    {
        CanvasDrawingSessionFixture f;

        ASSERT_IMPLEMENTS_INTERFACE(f.DS, ICanvasResourceCreator);
    }

    TEST_METHOD_EX(CanvasDrawingSession_get_Device_NullArg)
    {
        CanvasDrawingSessionFixture f;

        Assert::AreEqual(E_INVALIDARG, f.DS->get_Device(nullptr));
    }
};

TEST_CLASS(CanvasDrawingSession_Interop)
{
    TEST_METHOD_EX(CanvasDrawingSession_Wrapper_DoesNotAutomaticallyCallAnyMethods)
    {
        auto manager = std::make_shared<CanvasDrawingSessionManager>();
        
        // This mock device context will cause the test to fail if BeginDraw()
        // or EndDraw() is called on it.  When wrapping a drawing session like
        // this we don't want these methods to be called automatically.
        auto deviceContext = Make<MockD2DDeviceContext>();

        auto drawingSession = manager->GetOrCreate(deviceContext.Get());

        // Check one method - we assume that the previous tests are enough to
        // exercise all the methods appropriately.

        deviceContext->ClearMethod.SetExpectedCalls(1);

        ThrowIfFailed(drawingSession->Clear(ABI::Windows::UI::Color{ 1, 2, 3, 4 }));

        // Closing the drawing session should not result in any methods on the
        // deviceContext getting called.
        ThrowIfFailed(drawingSession->Close());
    }
};
