// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"
#include "mocks/MockDWriteFontFace.h"
#include "mocks/MockDWriteFontFaceReference.h"
#include "mocks/MockDWriteFont.h"
#include "stubs/LocalizedFontNames.h"
#include "stubs/StubCanvasTextLayoutAdapter.h"
#include "stubs/StubDWriteFontFaceReference.h"
#include <lib/text/CanvasFontFace.h>
#include <lib/text/CanvasTextRenderingParameters.h>

#if WINVER > _WIN32_WINNT_WINBLUE
static const int realizeOn10Only = 1;
typedef MockDWriteFontFaceReference MockDWriteFontFaceContainer;
#else
static const int realizeOn10Only = 0;
typedef MockDWriteFont MockDWriteFontFaceContainer;
#endif

TEST_CLASS(CanvasFontFaceTests)
{
    ComPtr<CanvasFontFace> CreateSimpleFontFace()
    {
        auto resource = Make<MockDWriteFontFaceContainer>();
        return Make<CanvasFontFace>(resource.Get());
    }

    TEST_METHOD_EX(CanvasFontFace_ImplementsExpectedInterfaces)
    {
        auto canvasFontFace = CreateSimpleFontFace();

        ASSERT_IMPLEMENTS_INTERFACE(canvasFontFace, ICanvasFontFace);
        ASSERT_IMPLEMENTS_INTERFACE(canvasFontFace, ABI::Windows::Foundation::IClosable);
    }

    TEST_METHOD_EX(CanvasFontFace_NullArgs)
    {
        Matrix3x2 m{};
        CanvasUnicodeRange* rangePointer;
        CanvasGlyphMetrics* glyphMetricsPointer;
        uint32_t u;
        int i;
        int* iPointer;
        auto drawingSession = Make<MockCanvasDrawingSession>();
        CanvasGlyph glyph{};
        Rect rect{};
        uint8_t* u8Pointer;

        auto canvasFontFace = CreateSimpleFontFace();

        Assert::AreEqual(E_INVALIDARG, canvasFontFace->GetRecommendedRenderingMode(0, 0, CanvasTextMeasuringMode::Natural, nullptr, nullptr));
        Assert::AreEqual(E_INVALIDARG, canvasFontFace->GetRecommendedRenderingModeWithAllOptions(0, 0, CanvasTextMeasuringMode::Natural, nullptr, m, false, CanvasAntialiasing::Antialiased, nullptr));
        Assert::AreEqual(E_INVALIDARG, canvasFontFace->GetRecommendedGridFit(0, 0, CanvasTextMeasuringMode::Natural, nullptr, m, false, CanvasAntialiasing::Antialiased, nullptr));

        Assert::AreEqual(E_INVALIDARG, canvasFontFace->get_GlyphBox(nullptr));

        Assert::AreEqual(E_INVALIDARG, canvasFontFace->get_SubscriptPosition(nullptr));
        Assert::AreEqual(E_INVALIDARG, canvasFontFace->get_SubscriptSize(nullptr));
        Assert::AreEqual(E_INVALIDARG, canvasFontFace->get_SuperscriptPosition(nullptr));
        Assert::AreEqual(E_INVALIDARG, canvasFontFace->get_SuperscriptSize(nullptr));

        Assert::AreEqual(E_INVALIDARG, canvasFontFace->get_HasTypographicMetrics(nullptr));

        Assert::AreEqual(E_INVALIDARG, canvasFontFace->get_Ascent(nullptr));
        Assert::AreEqual(E_INVALIDARG, canvasFontFace->get_Descent(nullptr));
        Assert::AreEqual(E_INVALIDARG, canvasFontFace->get_LineGap(nullptr));
        Assert::AreEqual(E_INVALIDARG, canvasFontFace->get_CapHeight(nullptr));
        Assert::AreEqual(E_INVALIDARG, canvasFontFace->get_LowercaseLetterHeight(nullptr));
        Assert::AreEqual(E_INVALIDARG, canvasFontFace->get_UnderlinePosition(nullptr));
        Assert::AreEqual(E_INVALIDARG, canvasFontFace->get_UnderlineThickness(nullptr));
        Assert::AreEqual(E_INVALIDARG, canvasFontFace->get_StrikethroughPosition(nullptr));
        Assert::AreEqual(E_INVALIDARG, canvasFontFace->get_StrikethroughThickness(nullptr));

        Assert::AreEqual(E_INVALIDARG, canvasFontFace->get_CaretSlopeRise(nullptr));
        Assert::AreEqual(E_INVALIDARG, canvasFontFace->get_CaretSlopeRun(nullptr));
        Assert::AreEqual(E_INVALIDARG, canvasFontFace->get_CaretOffset(nullptr));

        Assert::AreEqual(E_INVALIDARG, canvasFontFace->get_UnicodeRanges(nullptr, &rangePointer));
        Assert::AreEqual(E_INVALIDARG, canvasFontFace->get_UnicodeRanges(&u, nullptr));

        Assert::AreEqual(E_INVALIDARG, canvasFontFace->get_IsMonospaced(nullptr));

        Assert::AreEqual(E_INVALIDARG, canvasFontFace->GetVerticalGlyphVariants(1, &i, nullptr, &iPointer));
        Assert::AreEqual(E_INVALIDARG, canvasFontFace->GetVerticalGlyphVariants(1, &i, &u, nullptr));

        Assert::AreEqual(E_INVALIDARG, canvasFontFace->get_HasVerticalGlyphVariants(nullptr));
        Assert::AreEqual(E_INVALIDARG, canvasFontFace->get_FileFormatType(nullptr));
        Assert::AreEqual(E_INVALIDARG, canvasFontFace->get_Simulations(nullptr));
        Assert::AreEqual(E_INVALIDARG, canvasFontFace->get_IsSymbolFont(nullptr));
        Assert::AreEqual(E_INVALIDARG, canvasFontFace->get_GlyphCount(nullptr));

        Assert::AreEqual(E_INVALIDARG, canvasFontFace->GetGlyphIndices(1, &u, nullptr, &iPointer));
        Assert::AreEqual(E_INVALIDARG, canvasFontFace->GetGlyphIndices(1, &u, &u, nullptr));

        Assert::AreEqual(E_INVALIDARG, canvasFontFace->GetGlyphMetrics(1, &i, true, nullptr, &glyphMetricsPointer));
        Assert::AreEqual(E_INVALIDARG, canvasFontFace->GetGlyphMetrics(1, &i, true, &u, nullptr));

        Assert::AreEqual(E_INVALIDARG, canvasFontFace->GetGdiCompatibleGlyphMetrics(12, 96, m, true, 1, &i, true, nullptr, &glyphMetricsPointer));
        Assert::AreEqual(E_INVALIDARG, canvasFontFace->GetGdiCompatibleGlyphMetrics(12, 96, m, true, 1, &i, true, &u, nullptr));

        Assert::AreEqual(E_INVALIDARG, canvasFontFace->get_Weight(nullptr));
        Assert::AreEqual(E_INVALIDARG, canvasFontFace->get_Stretch(nullptr));
        Assert::AreEqual(E_INVALIDARG, canvasFontFace->get_Style(nullptr));

#if WINVER > _WIN32_WINNT_WINBLUE
        Assert::AreEqual(E_INVALIDARG, canvasFontFace->get_FamilyNames(nullptr));
#endif

        Assert::AreEqual(E_INVALIDARG, canvasFontFace->get_FaceNames(nullptr));

        Assert::AreEqual(E_INVALIDARG, canvasFontFace->GetInformationalStrings(CanvasFontInformation::CopyrightNotice, nullptr));

        Assert::AreEqual(E_INVALIDARG, canvasFontFace->HasCharacter(0x1234, nullptr));

        Assert::AreEqual(E_INVALIDARG, canvasFontFace->GetGlyphRunBounds(nullptr, Vector2{}, 0.0f, 0, &glyph, false, 0, &rect));
        Assert::AreEqual(E_INVALIDARG, canvasFontFace->GetGlyphRunBounds(drawingSession.Get(), Vector2{}, 0.0f, 0, nullptr, false, 0, &rect));
        Assert::AreEqual(E_INVALIDARG, canvasFontFace->GetGlyphRunBounds(drawingSession.Get(), Vector2{}, 0.0f, 0, &glyph, false, 0, nullptr));

        Assert::AreEqual(E_INVALIDARG, canvasFontFace->GetGlyphRunBoundsWithMeasuringMode(nullptr, Vector2{}, 0.0f, 0, &glyph, false, 0, CanvasTextMeasuringMode::Natural, &rect));
        Assert::AreEqual(E_INVALIDARG, canvasFontFace->GetGlyphRunBoundsWithMeasuringMode(drawingSession.Get(), Vector2{}, 0.0f, 0, nullptr, false, 0, CanvasTextMeasuringMode::Natural, &rect));
        Assert::AreEqual(E_INVALIDARG, canvasFontFace->GetGlyphRunBoundsWithMeasuringMode(drawingSession.Get(), Vector2{}, 0.0f, 0, &glyph, false, 0, CanvasTextMeasuringMode::Natural, nullptr));
        
        Assert::AreEqual(E_INVALIDARG, canvasFontFace->get_Panose(nullptr, &u8Pointer));
        Assert::AreEqual(E_INVALIDARG, canvasFontFace->get_Panose(&u, nullptr));
    }

    TEST_METHOD_EX(CanvasFontFace_Closed)
    {
        Matrix3x2 m{};
        CanvasUnicodeRange* rangePointer{};
        CanvasGlyphMetrics* glyphMetricsPointer{};
        uint32_t u{};
        int i{};
        int* iPointer{};
        Rect r{};
        Vector2 v{};
        Size s{};
        boolean b{};
        float f{};
        ComPtr<IMapView<HSTRING, HSTRING>> map;
        auto drawingSession = Make<MockCanvasDrawingSession>();
        CanvasGlyph glyph{};
        uint8_t* u8Pointer;

        auto canvasFontFace = CreateSimpleFontFace();
        Assert::AreEqual(S_OK, canvasFontFace->Close());

        CanvasTextRenderingMode renderingMode;
        CanvasTextGridFit gridFit;
        Assert::AreEqual(RO_E_CLOSED, canvasFontFace->GetRecommendedRenderingMode(0, 0, CanvasTextMeasuringMode::Natural, nullptr, &renderingMode));
        Assert::AreEqual(RO_E_CLOSED, canvasFontFace->GetRecommendedRenderingModeWithAllOptions(0, 0, CanvasTextMeasuringMode::Natural, nullptr, m, false, CanvasAntialiasing::Antialiased, &renderingMode));
        Assert::AreEqual(RO_E_CLOSED, canvasFontFace->GetRecommendedGridFit(0, 0, CanvasTextMeasuringMode::Natural, nullptr, m, false, CanvasAntialiasing::Antialiased, &gridFit));

        Assert::AreEqual(RO_E_CLOSED, canvasFontFace->get_GlyphBox(&r));

        Assert::AreEqual(RO_E_CLOSED, canvasFontFace->get_SubscriptPosition(&v));
        Assert::AreEqual(RO_E_CLOSED, canvasFontFace->get_SubscriptSize(&s));
        Assert::AreEqual(RO_E_CLOSED, canvasFontFace->get_SuperscriptPosition(&v));
        Assert::AreEqual(RO_E_CLOSED, canvasFontFace->get_SuperscriptSize(&s));

        Assert::AreEqual(RO_E_CLOSED, canvasFontFace->get_HasTypographicMetrics(&b));

        Assert::AreEqual(RO_E_CLOSED, canvasFontFace->get_Ascent(&f));
        Assert::AreEqual(RO_E_CLOSED, canvasFontFace->get_Descent(&f));
        Assert::AreEqual(RO_E_CLOSED, canvasFontFace->get_LineGap(&f));
        Assert::AreEqual(RO_E_CLOSED, canvasFontFace->get_CapHeight(&f));
        Assert::AreEqual(RO_E_CLOSED, canvasFontFace->get_LowercaseLetterHeight(&f));
        Assert::AreEqual(RO_E_CLOSED, canvasFontFace->get_UnderlinePosition(&f));
        Assert::AreEqual(RO_E_CLOSED, canvasFontFace->get_UnderlineThickness(&f));
        Assert::AreEqual(RO_E_CLOSED, canvasFontFace->get_StrikethroughPosition(&f));
        Assert::AreEqual(RO_E_CLOSED, canvasFontFace->get_StrikethroughThickness(&f));

        Assert::AreEqual(RO_E_CLOSED, canvasFontFace->get_CaretSlopeRise(&f));
        Assert::AreEqual(RO_E_CLOSED, canvasFontFace->get_CaretSlopeRun(&f));
        Assert::AreEqual(RO_E_CLOSED, canvasFontFace->get_CaretOffset(&f));

        Assert::AreEqual(RO_E_CLOSED, canvasFontFace->get_UnicodeRanges(&u, &rangePointer));

        Assert::AreEqual(RO_E_CLOSED, canvasFontFace->get_IsMonospaced(&b));

        Assert::AreEqual(RO_E_CLOSED, canvasFontFace->GetVerticalGlyphVariants(1, &i, &u, &iPointer));

        CanvasFontFileFormatType fontFileFormatType{};
        CanvasFontSimulations sim;
        Assert::AreEqual(RO_E_CLOSED, canvasFontFace->get_HasVerticalGlyphVariants(&b));
        Assert::AreEqual(RO_E_CLOSED, canvasFontFace->get_FileFormatType(&fontFileFormatType));
        Assert::AreEqual(RO_E_CLOSED, canvasFontFace->get_Simulations(&sim));
        Assert::AreEqual(RO_E_CLOSED, canvasFontFace->get_IsSymbolFont(&b));
        Assert::AreEqual(RO_E_CLOSED, canvasFontFace->get_GlyphCount(&u));

        Assert::AreEqual(RO_E_CLOSED, canvasFontFace->GetGlyphIndices(1, &u, &u, &iPointer));

        Assert::AreEqual(RO_E_CLOSED, canvasFontFace->GetGlyphMetrics(1, &i, true, &u, &glyphMetricsPointer));

        Assert::AreEqual(RO_E_CLOSED, canvasFontFace->GetGdiCompatibleGlyphMetrics(12, 96, m, true, 1, &i, true, &u, &glyphMetricsPointer));

        FontWeight fontWeight;
        FontStretch fontStretch;
        FontStyle fontStyle;
        Assert::AreEqual(RO_E_CLOSED, canvasFontFace->get_Weight(&fontWeight));
        Assert::AreEqual(RO_E_CLOSED, canvasFontFace->get_Stretch(&fontStretch));
        Assert::AreEqual(RO_E_CLOSED, canvasFontFace->get_Style(&fontStyle));

#if WINVER > _WIN32_WINNT_WINBLUE
        Assert::AreEqual(RO_E_CLOSED, canvasFontFace->get_FamilyNames(&map));
#endif

        Assert::AreEqual(RO_E_CLOSED, canvasFontFace->get_FaceNames(&map));

        Assert::AreEqual(RO_E_CLOSED, canvasFontFace->GetInformationalStrings(CanvasFontInformation::CopyrightNotice, &map));

        Assert::AreEqual(RO_E_CLOSED, canvasFontFace->HasCharacter(0x1234, &b));

        Assert::AreEqual(RO_E_CLOSED, canvasFontFace->GetGlyphRunBounds(drawingSession.Get(), Vector2{}, 0.0f, 0, &glyph, false, 0, &r));
        Assert::AreEqual(RO_E_CLOSED, canvasFontFace->GetGlyphRunBoundsWithMeasuringMode(drawingSession.Get(), Vector2{}, 0.0f, 0, &glyph, false, 0, CanvasTextMeasuringMode::Natural, &r));

        Assert::AreEqual(RO_E_CLOSED, canvasFontFace->get_Panose(&u, &u8Pointer));
    }

    struct Fixture
    {
        ComPtr<MockDWriteFontFace> RealizedDWriteFontFace;
        ComPtr<MockDWriteFont> DWriteFont;
        ComPtr<CanvasFontFace> FontFace;
        ComPtr<IDWriteRenderingParams> DWriteRenderingParameters;
        ComPtr<CanvasTextRenderingParameters> RenderingParameters;

#if WINVER > _WIN32_WINNT_WINBLUE
        ComPtr<StubDWriteFontFaceReference> DWriteFontFaceReference;
#endif

        Fixture(
            int realizationCount = 1, 
            HRESULT realizationHr = S_OK)
        {
            auto adapter = std::make_shared<StubCanvasTextLayoutAdapter>();
            CustomFontManagerAdapter::SetInstance(adapter);

#if WINVER > _WIN32_WINNT_WINBLUE
            DWriteFontFaceReference = Make<StubDWriteFontFaceReference>();
            FontFace = Make<CanvasFontFace>(DWriteFontFaceReference.Get());
#else
            DWriteFont = Make<MockDWriteFont>();
            FontFace = Make<CanvasFontFace>(DWriteFont.Get());
#endif

            RenderingParameters = CanvasTextRenderingParameters::CreateNew(CanvasTextRenderingMode::Default, CanvasTextGridFit::Default);
            DWriteRenderingParameters = GetWrappedResource<IDWriteRenderingParams>(RenderingParameters);

            RealizedDWriteFontFace = Make<MockDWriteFontFace>();

#if WINVER > _WIN32_WINNT_WINBLUE
            DWriteFontFaceReference->CreateFontFaceMethod.SetExpectedCalls(realizationCount,
                [&, realizationHr](IDWriteFontFace3** out)
                {
                    ThrowIfFailed(RealizedDWriteFontFace.CopyTo(out));
                    return realizationHr;
                });
#else
            DWriteFont->CreateFontFaceMethod.SetExpectedCalls(realizationCount,
                [&, realizationHr](IDWriteFontFace** out)
                {
                    ThrowIfFailed(RealizedDWriteFontFace.CopyTo(out));
                    return realizationHr;
                });
#endif
        }

#if WINVER > _WIN32_WINNT_WINBLUE
        ComPtr<MockDWriteFontFace> GetMockPhysicalPropertyContainer() { return RealizedDWriteFontFace; }
#else
        ComPtr<MockDWriteFont> GetMockPhysicalPropertyContainer() { return DWriteFont; }
#endif
    };

    TEST_METHOD_EX(CanvasFontFace_GetRecommendedRenderingMode)
    {
        Fixture f;

        f.RealizedDWriteFontFace->GetRecommendedRenderingModeMethod0.SetExpectedCalls(1,
            [&](FLOAT emSize, FLOAT pixelsPerDip, DWRITE_MEASURING_MODE measuringMode, IDWriteRenderingParams* renderingParams, DWRITE_RENDERING_MODE* out)
            {
                Assert::AreEqual(3.0f, emSize);
                Assert::AreEqual(100.0f / DEFAULT_DPI, pixelsPerDip);
                Assert::AreEqual(DWRITE_MEASURING_MODE_GDI_NATURAL, measuringMode);
                Assert::IsTrue(IsSameInstance(f.DWriteRenderingParameters.Get(), renderingParams));
                *out = DWRITE_RENDERING_MODE_ALIASED;
                return S_OK;
            });

        CanvasTextRenderingMode mode;
        Assert::AreEqual(S_OK, f.FontFace->GetRecommendedRenderingMode(3.0f, 100.0f, CanvasTextMeasuringMode::GdiNatural, f.RenderingParameters.Get(), &mode));
        Assert::AreEqual(CanvasTextRenderingMode::Aliased, mode);
    }

    TEST_METHOD_EX(CanvasFontFace_GetRecommendedRenderingMode_NullRenderingParamsIsOk)
    {
        Fixture f;

        f.RealizedDWriteFontFace->GetRecommendedRenderingModeMethod0.SetExpectedCalls(1,
            [&](FLOAT emSize, FLOAT pixelsPerDip, DWRITE_MEASURING_MODE measuringMode, IDWriteRenderingParams* renderingParams, DWRITE_RENDERING_MODE* out)
            {
                Assert::IsNull(renderingParams);
                *out = DWRITE_RENDERING_MODE_ALIASED;
                return S_OK;
            });

        CanvasTextRenderingMode mode;
        Assert::AreEqual(S_OK, f.FontFace->GetRecommendedRenderingMode(3.0f, 100.0f, CanvasTextMeasuringMode::GdiNatural, nullptr, &mode));
        Assert::AreEqual(CanvasTextRenderingMode::Aliased, mode);
    }

    TEST_METHOD_EX(CanvasFontFace_GetRecommendedRenderingMode_AllOptions)
    {
        Fixture f;

#if WINVER > _WIN32_WINNT_WINBLUE
        f.RealizedDWriteFontFace->GetRecommendedRenderingModeMethod3.SetExpectedCalls(1,
            [&](FLOAT emSize, FLOAT dpiX, FLOAT dpiY, DWRITE_MATRIX const* transform, BOOL isSidways, DWRITE_OUTLINE_THRESHOLD outline, DWRITE_MEASURING_MODE measuringMode, IDWriteRenderingParams* renderingParams, DWRITE_RENDERING_MODE1* renderingMode, DWRITE_GRID_FIT_MODE* gridFit)
            {
                Assert::AreEqual(3.0f, emSize);
                Assert::AreEqual(100.0f, dpiX);
                Assert::AreEqual(100.0f, dpiY);
                Assert::AreEqual(DWRITE_MATRIX{ 1, 2, 3, 4, 5, 6 }, *transform);
                Assert::IsTrue(!!isSidways);
                Assert::AreEqual(DWRITE_OUTLINE_THRESHOLD_ALIASED, outline);
                Assert::AreEqual(DWRITE_MEASURING_MODE_GDI_NATURAL, measuringMode);
                Assert::IsTrue(IsSameInstance(f.DWriteRenderingParameters.Get(), renderingParams));
                *renderingMode = DWRITE_RENDERING_MODE1_ALIASED;
                *gridFit = static_cast<DWRITE_GRID_FIT_MODE>(999);
                return S_OK;
            });
#else
        f.RealizedDWriteFontFace->GetRecommendedRenderingModeMethod2.SetExpectedCalls(1,
            [&](FLOAT emSize, FLOAT dpiX, FLOAT dpiY, DWRITE_MATRIX const* transform, BOOL isSidways, DWRITE_OUTLINE_THRESHOLD outline, DWRITE_MEASURING_MODE measuringMode, IDWriteRenderingParams* renderingParams, DWRITE_RENDERING_MODE* renderingMode, DWRITE_GRID_FIT_MODE* gridFit)
            {
                Assert::AreEqual(3.0f, emSize);
                Assert::AreEqual(100.0f, dpiX);
                Assert::AreEqual(100.0f, dpiY);
                Assert::AreEqual(DWRITE_MATRIX{ 1, 2, 3, 4, 5, 6 }, *transform);
                Assert::IsTrue(!!isSidways);
                Assert::AreEqual(DWRITE_OUTLINE_THRESHOLD_ALIASED, outline);
                Assert::AreEqual(DWRITE_MEASURING_MODE_GDI_NATURAL, measuringMode);
                Assert::IsTrue(IsSameInstance(f.DWriteRenderingParameters.Get(), renderingParams));
                *renderingMode = DWRITE_RENDERING_MODE_ALIASED;
                *gridFit = static_cast<DWRITE_GRID_FIT_MODE>(999);
                return S_OK;
            });
#endif

        CanvasTextRenderingMode mode;
        Numerics::Matrix3x2 someTransform = Numerics::Matrix3x2{ 1, 2, 3, 4, 5, 6 };
        Assert::AreEqual(S_OK, f.FontFace->GetRecommendedRenderingModeWithAllOptions(
            3.0f, 
            100.0f,
            CanvasTextMeasuringMode::GdiNatural, 
            f.RenderingParameters.Get(), 
            someTransform,
            true,
            CanvasAntialiasing::Aliased,
            &mode));
        Assert::AreEqual(CanvasTextRenderingMode::Aliased, mode);
    }

    TEST_METHOD_EX(CanvasFontFace_GetRecommendedGridFit)
    {
        Fixture f;

#if WINVER > _WIN32_WINNT_WINBLUE
        f.RealizedDWriteFontFace->GetRecommendedRenderingModeMethod3.SetExpectedCalls(1,
            [&](FLOAT emSize, FLOAT dpiX, FLOAT dpiY, DWRITE_MATRIX const* transform, BOOL isSidways, DWRITE_OUTLINE_THRESHOLD outline, DWRITE_MEASURING_MODE measuringMode, IDWriteRenderingParams* renderingParams, DWRITE_RENDERING_MODE1* renderingMode, DWRITE_GRID_FIT_MODE* gridFit)
            {
                Assert::AreEqual(3.0f, emSize);
                Assert::AreEqual(100.0f, dpiX);
                Assert::AreEqual(100.0f, dpiY);
                Assert::AreEqual(DWRITE_MATRIX{ 1, 2, 3, 4, 5, 6 }, *transform);
                Assert::IsTrue(!!isSidways);
                Assert::AreEqual(DWRITE_OUTLINE_THRESHOLD_ALIASED, outline);
                Assert::AreEqual(DWRITE_MEASURING_MODE_GDI_NATURAL, measuringMode);
                Assert::IsTrue(IsSameInstance(f.DWriteRenderingParameters.Get(), renderingParams));
                *renderingMode = static_cast<DWRITE_RENDERING_MODE1>(999);
                *gridFit = DWRITE_GRID_FIT_MODE_ENABLED;
                return S_OK;
            });
#else
        f.RealizedDWriteFontFace->GetRecommendedRenderingModeMethod2.SetExpectedCalls(1,
            [&](FLOAT emSize, FLOAT dpiX, FLOAT dpiY, DWRITE_MATRIX const* transform, BOOL isSidways, DWRITE_OUTLINE_THRESHOLD outline, DWRITE_MEASURING_MODE measuringMode, IDWriteRenderingParams* renderingParams, DWRITE_RENDERING_MODE* renderingMode, DWRITE_GRID_FIT_MODE* gridFit)
            {
                Assert::AreEqual(3.0f, emSize);
                Assert::AreEqual(100.0f, dpiX);
                Assert::AreEqual(100.0f, dpiY);
                Assert::AreEqual(DWRITE_MATRIX{ 1, 2, 3, 4, 5, 6 }, *transform);
                Assert::IsTrue(!!isSidways);
                Assert::AreEqual(DWRITE_OUTLINE_THRESHOLD_ALIASED, outline);
                Assert::AreEqual(DWRITE_MEASURING_MODE_GDI_NATURAL, measuringMode);
                Assert::IsTrue(IsSameInstance(f.DWriteRenderingParameters.Get(), renderingParams));
                *renderingMode = static_cast<DWRITE_RENDERING_MODE>(999);
                *gridFit = DWRITE_GRID_FIT_MODE_ENABLED;
                return S_OK;
            });
#endif

        CanvasTextGridFit gridFit;
        Numerics::Matrix3x2 someTransform = Numerics::Matrix3x2{ 1, 2, 3, 4, 5, 6 };
        Assert::AreEqual(S_OK, f.FontFace->GetRecommendedGridFit(
            3.0f, 
            100.0f,
            CanvasTextMeasuringMode::GdiNatural, 
            f.RenderingParameters.Get(), 
            someTransform,
            true,
            CanvasAntialiasing::Aliased,
            &gridFit));
        Assert::AreEqual(CanvasTextGridFit::Enable, gridFit);
    }

    TEST_METHOD_EX(CanvasFontFace_get_GlyphBox)
    {
        Fixture f;

        f.RealizedDWriteFontFace->GetMetricsMethod1.SetExpectedCalls(1,
            [&](DWRITE_FONT_METRICS1* out)
            {
                *out = DWRITE_FONT_METRICS1{};
                out->glyphBoxLeft = 10;
                out->glyphBoxTop = 20;
                out->glyphBoxRight = 10 + 30;
                out->glyphBoxBottom = 20 + 40;
                out->designUnitsPerEm = 10;
            });

        Rect glyphBox;
        Assert::AreEqual(S_OK, f.FontFace->get_GlyphBox(&glyphBox));
        Assert::AreEqual(Rect{ 1, 2, 3, 4 }, glyphBox);
    }

    TEST_METHOD_EX(CanvasFontFace_get_SubscriptPosition)
    {
        Fixture f;

        f.RealizedDWriteFontFace->GetMetricsMethod1.SetExpectedCalls(1,
            [&](DWRITE_FONT_METRICS1* out)
            {
                *out = DWRITE_FONT_METRICS1{};
                out->subscriptPositionX = 10;
                out->subscriptPositionY = 20;
                out->designUnitsPerEm = 10;
            });

        Vector2 subscriptPosition;
        Assert::AreEqual(S_OK, f.FontFace->get_SubscriptPosition(&subscriptPosition));
        Assert::AreEqual(Vector2{ 1, 2 }, subscriptPosition);
    }

    TEST_METHOD_EX(CanvasFontFace_get_SubscriptSize)
    {
        Fixture f;

        f.RealizedDWriteFontFace->GetMetricsMethod1.SetExpectedCalls(1,
            [&](DWRITE_FONT_METRICS1* out)
            {
                *out = DWRITE_FONT_METRICS1{};
                out->subscriptSizeX = 10;
                out->subscriptSizeY = 20;
                out->designUnitsPerEm = 10;
            });

        Size subscriptSize;
        Assert::AreEqual(S_OK, f.FontFace->get_SubscriptSize(&subscriptSize));
        Assert::AreEqual(Size{ 1, 2 }, subscriptSize);
    }

    TEST_METHOD_EX(CanvasFontFace_get_SuperscriptPosition)
    {
        Fixture f;

        f.RealizedDWriteFontFace->GetMetricsMethod1.SetExpectedCalls(1,
            [&](DWRITE_FONT_METRICS1* out)
            {
                *out = DWRITE_FONT_METRICS1{};
                out->superscriptPositionX = 10;
                out->superscriptPositionY = 20;
                out->designUnitsPerEm = 10;
            });

        Vector2 superscriptPosition;
        Assert::AreEqual(S_OK, f.FontFace->get_SuperscriptPosition(&superscriptPosition));
        Assert::AreEqual(Vector2{ 1, 2 }, superscriptPosition);
    }

    TEST_METHOD_EX(CanvasFontFace_get_SuperscriptSize)
    {
        Fixture f;

        f.RealizedDWriteFontFace->GetMetricsMethod1.SetExpectedCalls(1,
            [&](DWRITE_FONT_METRICS1* out)
            {
                *out = DWRITE_FONT_METRICS1{};
                out->superscriptSizeX = 10;
                out->superscriptSizeY = 20;
                out->designUnitsPerEm = 10;
            });

        Size superscriptSize;
        Assert::AreEqual(S_OK, f.FontFace->get_SuperscriptSize(&superscriptSize));
        Assert::AreEqual(Size{ 1, 2 }, superscriptSize);
    }

    TEST_METHOD_EX(CanvasFontFace_get_HasTypographicMetrics)
    {
        Fixture f;

        f.RealizedDWriteFontFace->GetMetricsMethod1.SetExpectedCalls(1,
            [&](DWRITE_FONT_METRICS1* out)
            {
                *out = DWRITE_FONT_METRICS1{};
                out->hasTypographicMetrics = TRUE;
            });

        boolean hasTypographicMetrics;
        Assert::AreEqual(S_OK, f.FontFace->get_HasTypographicMetrics(&hasTypographicMetrics));
        Assert::IsTrue(!!hasTypographicMetrics);
    }

#define TEST_FLOAT_PROPERTY_IN_DESIGN_METRICS(PROPERTY, METRICS_FIELD)      \
    TEST_METHOD_EX(CanvasFontFace_get_##PROPERTY)                           \
    {                                                                       \
        Fixture f;                                                          \
                                                                            \
        f.RealizedDWriteFontFace->GetMetricsMethod1.SetExpectedCalls(1,     \
            [&](DWRITE_FONT_METRICS1* out)                                  \
        {                                                                   \
            *out = DWRITE_FONT_METRICS1{};                                  \
            out->##METRICS_FIELD = 20;                                      \
            out->designUnitsPerEm = 10;                                     \
        });                                                                 \
                                                                            \
        float value;                                                        \
        Assert::AreEqual(S_OK, f.FontFace->get_##PROPERTY(&value));         \
        Assert::AreEqual(2.0f, value);                                      \
    }

    TEST_FLOAT_PROPERTY_IN_DESIGN_METRICS(Ascent, ascent);
    TEST_FLOAT_PROPERTY_IN_DESIGN_METRICS(Descent, descent);
    TEST_FLOAT_PROPERTY_IN_DESIGN_METRICS(LineGap, lineGap);
    TEST_FLOAT_PROPERTY_IN_DESIGN_METRICS(CapHeight, capHeight);
    TEST_FLOAT_PROPERTY_IN_DESIGN_METRICS(LowercaseLetterHeight, xHeight);
    TEST_FLOAT_PROPERTY_IN_DESIGN_METRICS(UnderlinePosition, underlinePosition);
    TEST_FLOAT_PROPERTY_IN_DESIGN_METRICS(UnderlineThickness, underlineThickness);
    TEST_FLOAT_PROPERTY_IN_DESIGN_METRICS(StrikethroughPosition, strikethroughPosition);
    TEST_FLOAT_PROPERTY_IN_DESIGN_METRICS(StrikethroughThickness, strikethroughThickness);    

    TEST_METHOD_EX(CanvasFontFace_get_CaretSlopeRise)
    {
        Fixture f;

        f.RealizedDWriteFontFace->GetCaretMetricsMethod.SetExpectedCalls(1,
            [&](DWRITE_CARET_METRICS* out)
            {
                *out = DWRITE_CARET_METRICS{};
                out->slopeRise = 99;
            });

        float slopeRise;
        Assert::AreEqual(S_OK, f.FontFace->get_CaretSlopeRise(&slopeRise));
        Assert::AreEqual(99.0f, slopeRise);
    }

    TEST_METHOD_EX(CanvasFontFace_get_CaretSlopeRun)
    {
        Fixture f;

        f.RealizedDWriteFontFace->GetCaretMetricsMethod.SetExpectedCalls(1,
            [&](DWRITE_CARET_METRICS* out)
            {
                *out = DWRITE_CARET_METRICS{};
                out->slopeRun = 99;
            });

        float slopeRun;
        Assert::AreEqual(S_OK, f.FontFace->get_CaretSlopeRun(&slopeRun));
        Assert::AreEqual(99.0f, slopeRun);
    }

    TEST_METHOD_EX(CanvasFontFace_get_CaretOffset)
    {
        Fixture f;

        f.RealizedDWriteFontFace->GetCaretMetricsMethod.SetExpectedCalls(1,
            [&](DWRITE_CARET_METRICS* out)
            {
                *out = DWRITE_CARET_METRICS{};
                out->offset = 90;
            });
        
        f.RealizedDWriteFontFace->GetMetricsMethod1.SetExpectedCalls(1,
            [&](DWRITE_FONT_METRICS1* out)
            {
                *out = DWRITE_FONT_METRICS1{}; 
                out->designUnitsPerEm = 10;
            });

        float offset;
        Assert::AreEqual(S_OK, f.FontFace->get_CaretOffset(&offset));
        Assert::AreEqual(9.0f, offset);
    }

    TEST_METHOD_EX(CanvasFontFace_get_UnicodeRanges)
    {
        Fixture f;

        int callNumber = 0;

        f.RealizedDWriteFontFace->GetUnicodeRangesMethod.SetExpectedCalls(2,
            [&](uint32_t maxRangeCount, DWRITE_UNICODE_RANGE* unicodeRanges, uint32_t* actualRangeCount)
            {
                if (callNumber == 0)
                {
                    Assert::AreEqual(0u, maxRangeCount);
                    Assert::IsNull(unicodeRanges);
                    *actualRangeCount = 2;
                    callNumber++;
                    return E_NOT_SUFFICIENT_BUFFER;
                }
                else
                {
                    Assert::AreEqual(2u, maxRangeCount);
                    unicodeRanges[0].first = 0x1000;
                    unicodeRanges[0].last = 0x1100;
                    unicodeRanges[1].first = 0x2000;
                    unicodeRanges[1].last = 0x2200;
                    *actualRangeCount = 2;
                    return S_OK;
                }
            });
        
        uint32_t valueCount;
        CanvasUnicodeRange* ranges;
        Assert::AreEqual(S_OK, f.FontFace->get_UnicodeRanges(&valueCount, &ranges));

        Assert::AreEqual(2u, valueCount);
        Assert::AreEqual(0x1000u, ranges[0].First);
        Assert::AreEqual(0x1100u, ranges[0].Last);
        Assert::AreEqual(0x2000u, ranges[1].First);
        Assert::AreEqual(0x2200u, ranges[1].Last);
    }

    TEST_METHOD_EX(CanvasFontFace_get_IsMonospaced)
    {
        Fixture f;

        f.RealizedDWriteFontFace->IsMonospacedFontMethod.SetExpectedCalls(1,
            [&]
            {
                return TRUE;
            });

        boolean isMonospaced;
        Assert::AreEqual(S_OK, f.FontFace->get_IsMonospaced(&isMonospaced));
        Assert::IsTrue(!!isMonospaced);
    }

    TEST_METHOD_EX(CanvasFontFace_GetVerticalGlyphVariants_BadArgs)
    {
        Fixture f(0);
        uint32_t outputCount;
        int* outputElements;

        int inputElement = -1;
        Assert::AreEqual(E_INVALIDARG, f.FontFace->GetVerticalGlyphVariants(1, &inputElement, &outputCount, &outputElements));

        inputElement = USHORT_MAX+1;
        Assert::AreEqual(E_INVALIDARG, f.FontFace->GetVerticalGlyphVariants(1, &inputElement, &outputCount, &outputElements));
    }

    TEST_METHOD_EX(CanvasFontFace_GetVerticalGlyphVariants)
    {
        Fixture f;        

        f.RealizedDWriteFontFace->GetVerticalGlyphVariantsMethod.SetExpectedCalls(1,
            [&](uint32_t glyphCount, UINT16 const* nominalGlyphIndices, UINT16* verticalGlyphIndices)
            {
                Assert::AreEqual(3u, glyphCount);

                Assert::AreEqual(4ui16, nominalGlyphIndices[0]);
                Assert::AreEqual(5ui16, nominalGlyphIndices[1]);
                Assert::AreEqual(6ui16, nominalGlyphIndices[2]);

                verticalGlyphIndices[0] = 7u;
                verticalGlyphIndices[1] = 8u;
                verticalGlyphIndices[2] = 9u;

                return S_OK;
            });

        int inputElements[]{ 4, 5, 6 };
        uint32_t outputCount;
        int* outputElements;
        Assert::AreEqual(S_OK, f.FontFace->GetVerticalGlyphVariants(3u, inputElements, &outputCount, &outputElements));

        Assert::AreEqual(3u, outputCount);
        Assert::AreEqual(7, outputElements[0]);
        Assert::AreEqual(8, outputElements[1]);
        Assert::AreEqual(9, outputElements[2]);
    }

    TEST_METHOD_EX(CanvasFontFace_get_HasVerticalGlyphVariants)
    {
        Fixture f;

        f.RealizedDWriteFontFace->HasVerticalGlyphVariantsMethod.SetExpectedCalls(1,
            [&]
            {
                return TRUE;
            });

        boolean hasVerticalGlyphVariants;
        Assert::AreEqual(S_OK, f.FontFace->get_HasVerticalGlyphVariants(&hasVerticalGlyphVariants));
        Assert::IsTrue(!!hasVerticalGlyphVariants);
    }

    TEST_METHOD_EX(CanvasFontFace_get_FileFormatType)
    {
        Fixture f;

        f.RealizedDWriteFontFace->GetTypeMethod.SetExpectedCalls(1,
            [&]
            {
                return DWRITE_FONT_FACE_TYPE_RAW_CFF;
            });

        CanvasFontFileFormatType fileFormatType;
        Assert::AreEqual(S_OK, f.FontFace->get_FileFormatType(&fileFormatType));
        Assert::AreEqual(CanvasFontFileFormatType::RawCff, fileFormatType);
    }

    TEST_METHOD_EX(CanvasFontFace_get_Simulations)
    {
        Fixture f;

        f.RealizedDWriteFontFace->GetSimulationsMethod.SetExpectedCalls(1,
            [&]
            {
                return DWRITE_FONT_SIMULATIONS_BOLD | DWRITE_FONT_SIMULATIONS_OBLIQUE;
            });

        CanvasFontSimulations fontSimulations;
        Assert::AreEqual(S_OK, f.FontFace->get_Simulations(&fontSimulations));
        Assert::AreEqual(CanvasFontSimulations::Bold | CanvasFontSimulations::Oblique, fontSimulations);
    }

    TEST_METHOD_EX(CanvasFontFace_get_IsSymbolFont)
    {
        Fixture f;

        f.RealizedDWriteFontFace->IsSymbolFontMethod.SetExpectedCalls(1,
            [&]
            {
                return TRUE;
            });

        boolean isSymbolFont;
        Assert::AreEqual(S_OK, f.FontFace->get_IsSymbolFont(&isSymbolFont));
        Assert::IsTrue(!!isSymbolFont);
    }

    TEST_METHOD_EX(CanvasFontFace_get_GlyphCount)
    {
        Fixture f;

        f.RealizedDWriteFontFace->GetGlyphCountMethod.SetExpectedCalls(1,
            [&]
            {
                return 123ui16;
            });

        uint32_t glyphCount;
        Assert::AreEqual(S_OK, f.FontFace->get_GlyphCount(&glyphCount));
        Assert::AreEqual(123u, glyphCount);
    }

    TEST_METHOD_EX(CanvasFontFace_GetGlyphIndices)
    {
        Fixture f;

        f.RealizedDWriteFontFace->GetGlyphIndicesMethod.SetExpectedCalls(1,
            [&](uint32_t const* codePoints, uint32_t codePointCount, UINT16* glyphIndices)
            {
                Assert::AreEqual(4u, codePoints[0]);
                Assert::AreEqual(5u, codePoints[1]);
                Assert::AreEqual(6u, codePoints[2]);

                Assert::AreEqual(3u, codePointCount);

                glyphIndices[0] = 7ui16;
                glyphIndices[1] = 8ui16;
                glyphIndices[2] = 9ui16;

                return S_OK;
            });

        uint32_t inputGlyphs[] {4u, 5u, 6u};
        uint32_t outputCount;
        int* outputElements;
        Assert::AreEqual(S_OK, f.FontFace->GetGlyphIndices(3, inputGlyphs, &outputCount, &outputElements));
        Assert::AreEqual(3u, outputCount);
        Assert::AreEqual(7, outputElements[0]);
        Assert::AreEqual(8, outputElements[1]);
        Assert::AreEqual(9, outputElements[2]);
    }

    TEST_METHOD_EX(CanvasFontFace_GetGlyphMetrics_BadArgs)
    {
        Fixture f(0);
        uint32_t outputCount;
        CanvasGlyphMetrics* outputElements;

        int inputElement = -1;
        Assert::AreEqual(E_INVALIDARG, f.FontFace->GetGlyphMetrics(1, &inputElement, true, &outputCount, &outputElements));

        inputElement = USHORT_MAX + 1;
        Assert::AreEqual(E_INVALIDARG, f.FontFace->GetGlyphMetrics(1, &inputElement, true, &outputCount, &outputElements));
    }

    TEST_METHOD_EX(CanvasFontFace_GetGlyphMetrics)
    {
        Fixture f;

        f.RealizedDWriteFontFace->GetMetricsMethod1.SetExpectedCalls(1,
            [&](DWRITE_FONT_METRICS1* out)
            {
                *out = DWRITE_FONT_METRICS1{};
                out->designUnitsPerEm = 10;
                out->lineGap = 10;
                out->ascent = 110;
            });

        f.RealizedDWriteFontFace->GetDesignGlyphMetricsMethod.SetExpectedCalls(1,
            [&](UINT16 const* glyphIndices, UINT32 glyphCount, DWRITE_GLYPH_METRICS* glyphMetrics, BOOL isSideways)
            {
                Assert::AreEqual(TRUE, isSideways);

                Assert::AreEqual(61ui16, glyphIndices[0]);
                Assert::AreEqual(62ui16, glyphIndices[1]);
                Assert::AreEqual(63ui16, glyphIndices[2]);

                Assert::AreEqual(3u, glyphCount);

                glyphMetrics[0] = DWRITE_GLYPH_METRICS{ 20, 90u, -10, 10, 70u, -20, 60 };
                glyphMetrics[1] = DWRITE_GLYPH_METRICS{ 20, 90u, -10, 10, 70u, -20, 60 };
                glyphMetrics[2] = DWRITE_GLYPH_METRICS{ 20, 90u, -10, 10, 70u, -20, 60 };

                return S_OK;
            });

        int inputGlyphs[]{ 61u, 62u, 63u };
        uint32_t outputCount;
        CanvasGlyphMetrics* outputElements;
        Assert::AreEqual(S_OK, f.FontFace->GetGlyphMetrics(3, inputGlyphs, true, &outputCount, &outputElements));
        Assert::AreEqual(3u, outputCount);
        Assert::AreEqual(2.f, outputElements[0].LeftSideBearing);
        Assert::AreEqual(9.f, outputElements[0].AdvanceWidth);
        Assert::AreEqual(-1.f, outputElements[0].RightSideBearing);
        Assert::AreEqual(1.f, outputElements[0].TopSideBearing);
        Assert::AreEqual(7.f, outputElements[0].AdvanceHeight);
        Assert::AreEqual(-2.f, outputElements[0].BottomSideBearing);
        Assert::AreEqual(6.f, outputElements[0].VerticalOrigin);
        Assert::AreEqual(Rect{ 2.f, 7.f, 8.f, 8.f }, outputElements[0].DrawBounds);
    }

    TEST_METHOD_EX(CanvasFontFace_GetGdiCompatibleGlyphMetrics_BadArgs)
    {
      Fixture f(0);
      uint32_t outputCount;
      CanvasGlyphMetrics* outputElements;
      Matrix3x2 someTransform = Matrix3x2{ 1, 2, 3, 4, 5, 6 };

      int inputElement = -1;
      Assert::AreEqual(E_INVALIDARG, f.FontFace->GetGdiCompatibleGlyphMetrics(12, 96, someTransform, true, 1, &inputElement, true, &outputCount, &outputElements));

      inputElement = USHORT_MAX + 1;
      Assert::AreEqual(E_INVALIDARG, f.FontFace->GetGdiCompatibleGlyphMetrics(12, 96, someTransform, true, 1, &inputElement, true, &outputCount, &outputElements));
    }

    TEST_METHOD_EX(CanvasFontFace_GetGdiCompatibleGlyphMetrics)
    {
        Fixture f;

        f.RealizedDWriteFontFace->GetMetricsMethod1.SetExpectedCalls(1,
            [&](DWRITE_FONT_METRICS1* out)
            {
                *out = DWRITE_FONT_METRICS1{};
                out->designUnitsPerEm = 10;
                out->lineGap = 10;
                out->ascent = 110;
            });
        f.RealizedDWriteFontFace->GetGdiCompatibleGlyphMetricsMethod.SetExpectedCalls(1,
            [&](FLOAT emSize, FLOAT pixelsPerDip, DWRITE_MATRIX const* transform, BOOL useGdiNatural, UINT16 const* glyphIndices, uint32_t glyphCount, DWRITE_GLYPH_METRICS* glyphMetrics, BOOL isSideways)
            {
                Assert::AreEqual(20.f, emSize);
                Assert::AreEqual(1.f, pixelsPerDip);
                Assert::AreEqual(TRUE, useGdiNatural);
                Assert::AreEqual(TRUE, isSideways);

                Assert::AreEqual(61ui16, glyphIndices[0]);
                Assert::AreEqual(62ui16, glyphIndices[1]);
                Assert::AreEqual(63ui16, glyphIndices[2]);

                Assert::AreEqual(3u, glyphCount);

                glyphMetrics[0] = DWRITE_GLYPH_METRICS{ 20, 90u, -10, 10, 70u, -20, 60 };
                glyphMetrics[1] = DWRITE_GLYPH_METRICS{ 20, 90u, -10, 10, 70u, -20, 60 };
                glyphMetrics[2] = DWRITE_GLYPH_METRICS{ 20, 90u, -10, 10, 70u, -20, 60 };

                return S_OK;
            });

        int inputGlyphs[]{ 61u, 62u, 63u };
        uint32_t outputCount;
        CanvasGlyphMetrics* outputElements;
        Matrix3x2 someTransform = Matrix3x2{ 1, 2, 3, 4, 5, 6 };

        Assert::AreEqual(S_OK, f.FontFace->GetGdiCompatibleGlyphMetrics(20.f, 96.f, someTransform, true, 3, inputGlyphs, true, &outputCount, &outputElements));
        Assert::AreEqual(3u, outputCount);
        Assert::AreEqual(2.f, outputElements[0].LeftSideBearing);
        Assert::AreEqual(9.f, outputElements[0].AdvanceWidth);
        Assert::AreEqual(-1.f, outputElements[0].RightSideBearing);
        Assert::AreEqual(1.f, outputElements[0].TopSideBearing);
        Assert::AreEqual(7.f, outputElements[0].AdvanceHeight);
        Assert::AreEqual(-2.f, outputElements[0].BottomSideBearing);
        Assert::AreEqual(6.f, outputElements[0].VerticalOrigin);
        Assert::AreEqual(Rect{ 2.f, 7.f, 8.f, 8.f }, outputElements[0].DrawBounds);
    }

    TEST_METHOD_EX(CanvasFontFace_get_Weight)
    {
        Fixture f(realizeOn10Only);

        f.GetMockPhysicalPropertyContainer()->GetWeightMethod.SetExpectedCalls(1,
            [&]
            {
                return DWRITE_FONT_WEIGHT_ULTRA_BOLD;
            });

        FontWeight value;
        Assert::AreEqual(S_OK, f.FontFace->get_Weight(&value));
        Assert::AreEqual(FontWeight{ 800 }, value);
    }

    TEST_METHOD_EX(CanvasFontFace_get_Stretch)
    {
        Fixture f(realizeOn10Only);

        f.GetMockPhysicalPropertyContainer()->GetStretchMethod.SetExpectedCalls(1,
            [&]
            {
                return DWRITE_FONT_STRETCH_EXPANDED;
            });

        FontStretch value;
        Assert::AreEqual(S_OK, f.FontFace->get_Stretch(&value));
        Assert::AreEqual(FontStretch_Expanded, value);
    }

    TEST_METHOD_EX(CanvasFontFace_get_Style)
    {
        Fixture f(realizeOn10Only);

        f.GetMockPhysicalPropertyContainer()->GetStyleMethod.SetExpectedCalls(1,
            [&]
            {
                return DWRITE_FONT_STYLE_ITALIC;
            });

        FontStyle value;
        Assert::AreEqual(S_OK, f.FontFace->get_Style(&value));
        Assert::AreEqual(FontStyle_Italic, value);
    }

    TEST_METHOD_EX(CanvasFontFace_get_FaceNames)
    {
        Fixture f(realizeOn10Only);

        f.GetMockPhysicalPropertyContainer()->GetFaceNamesMethod.SetExpectedCalls(1,
            [&](IDWriteLocalizedStrings** out)
            {
                auto localizedNames = Make<LocalizedFontNames>(
                    L"SomeName1", L"xx-aa",
                    L"SomeName2", L"xx-bb");
                ThrowIfFailed(localizedNames.CopyTo(out));
                return S_OK;
            });

        ComPtr<IMapView<HSTRING, HSTRING>> faceNames;
        Assert::AreEqual(S_OK, f.FontFace->get_FaceNames(&faceNames));

        uint32_t size;
        ThrowIfFailed(faceNames->get_Size(&size));
        Assert::AreEqual(2u, size);

        VerifyStringMapContainsKeyValue(faceNames, L"xx-aa", L"SomeName1");
        VerifyStringMapContainsKeyValue(faceNames, L"xx-bb", L"SomeName2");
    }

    TEST_METHOD_EX(CanvasFontFace_GetInformationalStrings)
    {
        Fixture f(realizeOn10Only);

        f.GetMockPhysicalPropertyContainer()->GetInformationalStringsMethod.SetExpectedCalls(1,
            [&](DWRITE_INFORMATIONAL_STRING_ID stringId, IDWriteLocalizedStrings** out, BOOL* exists)
            {
                Assert::AreEqual(DWRITE_INFORMATIONAL_STRING_TRADEMARK, stringId);

                auto localizedNames = Make<LocalizedFontNames>(
                    L"SomeName1", L"xx-aa",
                    L"SomeName2", L"xx-bb");
                ThrowIfFailed(localizedNames.CopyTo(out));

                *exists = true;

                return S_OK;
            });

        ComPtr<IMapView<HSTRING, HSTRING>> informationStrings;
        Assert::AreEqual(S_OK, f.FontFace->GetInformationalStrings(CanvasFontInformation::Trademark, &informationStrings));

        uint32_t size;
        ThrowIfFailed(informationStrings->get_Size(&size));
        Assert::AreEqual(2u, size);

        VerifyStringMapContainsKeyValue(informationStrings, L"xx-aa", L"SomeName1");
        VerifyStringMapContainsKeyValue(informationStrings, L"xx-bb", L"SomeName2");
    }

    TEST_METHOD_EX(CanvasFontFace_HasCharacter)
    {
        Fixture f(realizeOn10Only);

#if WINVER > _WIN32_WINNT_WINBLUE
        f.GetMockPhysicalPropertyContainer()->HasCharacterMethod.SetExpectedCalls(2,
            [&](uint32_t unicodeValue)
            {
                return (unicodeValue == 0x1234u);
            });
#else
        f.GetMockPhysicalPropertyContainer()->HasCharacterMethod.SetExpectedCalls(2,
            [&](uint32_t unicodeValue, BOOL* exists)
            {
                *exists = (unicodeValue == 0x1234u);
                return S_OK;
            });
#endif

        boolean value;
        Assert::AreEqual(S_OK, f.FontFace->HasCharacter(0x1234u, &value));
        Assert::IsTrue(!!value);

        Assert::AreEqual(S_OK, f.FontFace->HasCharacter(0x4000u, &value));
        Assert::IsFalse(!!value);
    }

#if WINVER > _WIN32_WINNT_WINBLUE
    TEST_METHOD_EX(CanvasFontFace_RealizingRemoteFontThrowsException)
    {
        Fixture f(1, DWRITE_E_REMOTEFONT);

        boolean value;
        Assert::AreEqual(DWRITE_E_REMOTEFONT, f.FontFace->HasCharacter(0x1234u, &value));
    }
#endif

    struct GetGlyphRunBoundsFixture : public Fixture
    {
        ComPtr<MockD2DDeviceContext> m_d2dDeviceContext;
    public:
        ComPtr<CanvasDrawingSession> DrawingSession;
        std::vector<CanvasGlyph> Glyphs;

        GetGlyphRunBoundsFixture()
            : Fixture(1)
        {
            m_d2dDeviceContext = Make<MockD2DDeviceContext>();
            DrawingSession = Make<CanvasDrawingSession>(m_d2dDeviceContext.Get());

            Glyphs.push_back(CanvasGlyph{ 1, 2.0f, 3.0f, 4.0f });
            Glyphs.push_back(CanvasGlyph{ 5, 6.0f, 7.0f, 8.0f });
            Glyphs.push_back(CanvasGlyph{ 9, 10.0f, 11.0f, 12.0f });
        }

        void ExpectGetGlyphRunWorldBounds(DWRITE_MEASURING_MODE expectedMeasuringMode)
        {
            m_d2dDeviceContext->GetGlyphRunWorldBoundsMethod.SetExpectedCalls(1,
                [=](D2D1_POINT_2F baselineOrigin, const DWRITE_GLYPH_RUN* glyphRun, DWRITE_MEASURING_MODE measuringMode, D2D1_RECT_F* out)
                {
                    Assert::AreEqual(1.2f, baselineOrigin.x);
                    Assert::AreEqual(3.4f, baselineOrigin.y);

                    Assert::IsTrue(IsSameInstance(RealizedDWriteFontFace.Get(), glyphRun->fontFace));

                    Assert::AreEqual(11.0f, glyphRun->fontEmSize);
                    Assert::AreEqual(3u, glyphRun->glyphCount);

                    for (int i = 0; i < 3; ++i)
                    {
                        int k = i * 4 + 1;
                        Assert::AreEqual(static_cast<uint16_t>(k), glyphRun->glyphIndices[i]);
                        Assert::AreEqual(k + 1.0f, glyphRun->glyphAdvances[i]);
                        Assert::AreEqual(k + 2.0f, glyphRun->glyphOffsets[i].advanceOffset);
                        Assert::AreEqual(k + 3.0f, glyphRun->glyphOffsets[i].ascenderOffset);
                    }

                    Assert::IsTrue(!!glyphRun->isSideways);
                    Assert::AreEqual(5u, glyphRun->bidiLevel);

                    Assert::AreEqual(expectedMeasuringMode, measuringMode);

                    *out = D2D1::RectF(100, 200, 104, 205);

                    return S_OK;
                });
        }
    };

    TEST_METHOD_EX(CanvasFontFace_GetGlyphRunBounds)
    {
        GetGlyphRunBoundsFixture f;

        f.ExpectGetGlyphRunWorldBounds(DWRITE_MEASURING_MODE_NATURAL);

        Rect bounds;
        Assert::AreEqual(S_OK, f.FontFace->GetGlyphRunBounds(
            f.DrawingSession.Get(),
            Vector2{ 1.2f, 3.4f },
            11.0f,
            static_cast<uint32_t>(f.Glyphs.size()),
            f.Glyphs.data(),
            true,
            5u,
            &bounds));

        Assert::AreEqual(Rect{ 100, 200, 4, 5 }, bounds);
    }

    TEST_METHOD_EX(CanvasFontFace_GetGlyphRunBoundsWithMeasuringMode)
    {
        GetGlyphRunBoundsFixture f;

        f.ExpectGetGlyphRunWorldBounds(DWRITE_MEASURING_MODE_GDI_CLASSIC);

        Rect bounds;
        Assert::AreEqual(S_OK, f.FontFace->GetGlyphRunBoundsWithMeasuringMode(
            f.DrawingSession.Get(),
            Vector2{ 1.2f, 3.4f },
            11.0f,
            static_cast<uint32_t>(f.Glyphs.size()),
            f.Glyphs.data(),
            true,
            5u,
            CanvasTextMeasuringMode::GdiClassic,
            &bounds));

        Assert::AreEqual(Rect{ 100, 200, 4, 5 }, bounds);
    }


    TEST_METHOD_EX(CanvasFontFace_get_Panose)
    {
        Fixture f(realizeOn10Only);
        
        f.GetMockPhysicalPropertyContainer()->GetPanoseMethod.SetExpectedCalls(1,
            [&](DWRITE_PANOSE* panose)
            {
                for (uint8_t i = 0; i < 10; ++i)
                    panose->values[i] = i;
            });

        uint32_t valueCount;
        uint8_t* values;

        Assert::AreEqual(S_OK, f.FontFace->get_Panose(&valueCount, &values));
        Assert::AreEqual(10u, valueCount);

        for (uint8_t i = 0; i < 10; ++i)
            Assert::AreEqual(i, values[i]);
    }
};

//
// These workarounds are (unfortunately) needed due to a bug in DWrite's headers,
// where a few virtual methods are not marked as 'pure'. This causes a link error
// when defining our mocks, when compiling for 8.1.
//

#if WINVER <= _WIN32_WINNT_WINBLUE
IFACEMETHODIMP IDWriteFactory1::GetEudcFontCollection(IDWriteFontCollection**, BOOL )
{
    return E_NOTIMPL;
}

IFACEMETHODIMP_(BOOL) IDWriteFontFace1::HasKerningPairs()
{
    return FALSE;
}

IFACEMETHODIMP IDWriteFontFace1::GetVerticalGlyphVariants(uint32_t, uint16_t const*, uint16_t*)
{
    return E_NOTIMPL;
}

IFACEMETHODIMP_(BOOL) IDWriteFontFace1::HasVerticalGlyphVariants()
{
    return FALSE;
}

IFACEMETHODIMP IDWriteFontFallback::MapCharacters(IDWriteTextAnalysisSource*, unsigned int, unsigned int, IDWriteFontCollection *, wchar_t const *, DWRITE_FONT_WEIGHT, DWRITE_FONT_STYLE, DWRITE_FONT_STRETCH, unsigned int *, IDWriteFont**, float*)
{
    return E_NOTIMPL;
}
#endif