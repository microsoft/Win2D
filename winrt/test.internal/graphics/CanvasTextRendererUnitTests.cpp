// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

#include <lib/text/CanvasFontFace.h>
#include <lib/text/CanvasTextLayout.h>
#include <lib/brushes/CanvasSolidColorBrush.h>

#include "stubs/CustomTextRenderer.h"
#include "stubs/StubCanvasTextLayoutAdapter.h"
#include "stubs/CustomInlineObject.h"

#include "mocks/MockDWriteFont.h"
#include "mocks/MockDWriteFontFace.h"
#include "mocks/MockDWriteFontFaceReference.h"
#include "mocks/MockDWriteFontFile.h"

namespace canvas
{
    using namespace ABI::Windows::UI::Text;

    static const HRESULT sc_someFailureHr = D2DERR_PRINT_JOB_CLOSED;

#if WINVER > _WIN32_WINNT_WINBLUE
    typedef MockDWriteFontFaceReference MockDWriteFontFaceContainer;
    typedef IDWriteFontFace3 FontFaceType;
#else
    typedef MockDWriteFont MockDWriteFontFaceContainer;
    typedef IDWriteFontFace FontFaceType;
#endif

    TEST_CLASS(CanvasTextRenderer)
    {
        struct Fixture
        {
            ComPtr<ICanvasTextFormat> Format;
            std::shared_ptr<StubCanvasTextLayoutAdapter> Adapter;
            ComPtr<StubCanvasDevice> Device;

            ComPtr<CustomTextRenderer> TextRenderer;

            ComPtr<CustomInlineObject> InlineObject;
            ComPtr<InternalDWriteInlineObject> DWriteInlineObject;

            ComPtr<MockDWriteFont> DWriteFont;
#if WINVER > _WIN32_WINNT_WINBLUE
            ComPtr<MockDWriteFontFaceReference> DWriteFontFaceReference;
#endif
            ComPtr<MockDWriteFontFace> RealizedDWriteFontFace;
            ComPtr<CanvasFontFace> FontFace;

            ComPtr<MockD2DSolidColorBrush> D2DBrush;
            ComPtr<CanvasSolidColorBrush> SolidColorBrush;

            Fixture()
                : Adapter(std::make_shared<StubCanvasTextLayoutAdapter>())
                , Device(Make<StubCanvasDevice>())
                , TextRenderer(Make<CustomTextRenderer>())
                , DWriteFont(Make<MockDWriteFont>())
#if WINVER > _WIN32_WINNT_WINBLUE
                , DWriteFontFaceReference(Make<MockDWriteFontFaceReference>())
#endif
                , RealizedDWriteFontFace(Make<MockDWriteFontFace>())
                , D2DBrush(Make<MockD2DSolidColorBrush>())
            {
                InlineObject = Make<CustomInlineObject>();
                DWriteInlineObject = Make<InternalDWriteInlineObject>(InlineObject.Get(), Device.Get());

                RealizedDWriteFontFace->GetFilesMethod.AllowAnyCall(
                    [&](uint32_t* numberOfFiles, IDWriteFontFile** fontFiles)
                    {
                        *numberOfFiles = 1;
                        auto mockFontFile = Make<MockDWriteFontFile>();
                        if (fontFiles)
                        {
                            ThrowIfFailed(mockFontFile.CopyTo(&fontFiles[0]));
                        }
                        return S_OK;
                    });

#if WINVER > _WIN32_WINNT_WINBLUE
                DWriteFontFaceReference->CreateFontFaceMethod.AllowAnyCall(
                    [&](IDWriteFontFace3** out)
                    {
                        ThrowIfFailed(RealizedDWriteFontFace.CopyTo(out));
                        return S_OK;
                    });

                DWriteFont->GetFontFaceReferenceMethod.AllowAnyCall(
                    [&](IDWriteFontFaceReference** out)
                    {
                        ThrowIfFailed(DWriteFontFaceReference.CopyTo(out));
                        return S_OK;
                    });
#else
                DWriteFont->CreateFontFaceMethod.AllowAnyCall(
                    [&](IDWriteFontFace** out)
                    {
                        ThrowIfFailed(RealizedDWriteFontFace.CopyTo(out));
                        return S_OK;
                    });
#endif

#if WINVER > _WIN32_WINNT_WINBLUE
                FontFace = Make<CanvasFontFace>(DWriteFontFaceReference.Get());
#else
                FontFace = Make<CanvasFontFace>(DWriteFont.Get());
#endif

                Adapter->GetMockDWriteFactory()->CreateCustomFontCollectionMethod.AllowAnyCall(
                    [&](
                    IDWriteFontCollectionLoader* collectionLoader,
                    void const* collectionKey,
                    uint32_t collectionKeySize,
                    IDWriteFontCollection** fontCollection)
                {
                    auto mockFontCollection = Make<MockDWriteFontCollection>();

                    mockFontCollection->GetFontFromFontFaceMethod.AllowAnyCall(
                        [&](IDWriteFontFace* fontFace, IDWriteFont** font)
                        {
                            return DWriteFont.CopyTo(font);
                        });

                    return mockFontCollection.CopyTo(fontCollection);
                });

                SolidColorBrush = Make<CanvasSolidColorBrush>(Device.Get(), D2DBrush.Get());

                CustomFontManagerAdapter::SetInstance(Adapter);

                Format = Make<CanvasTextFormat>();
            }

            ComPtr<CanvasTextLayout> CreateSimpleTextLayout()
            {
                return CanvasTextLayout::CreateNew(Device.Get(), WinString(L"A string"), Format.Get(), 0.0f, 0.0f);
            }
        };

        TEST_METHOD_EX(CanvasTextRenderer_NullArg)
        {
            Fixture f;

            auto textLayout = f.CreateSimpleTextLayout();

            Assert::AreEqual(E_INVALIDARG, textLayout->DrawToTextRenderer(nullptr, Vector2{ 0, 0 }));

            Assert::AreEqual(E_INVALIDARG, textLayout->DrawToTextRendererWithCoords(nullptr,  0, 0 ));
        }

        TEST_METHOD_EX(CanvasTextRenderer_Draw_CallsThrough)
        {
            for (int i = 0; i < 2; ++i)
            {
                Fixture f;

                f.Adapter->MockTextLayout->DrawMethod.SetExpectedCalls(1,
                    [&](void* context, IDWriteTextRenderer* renderer, FLOAT originX, FLOAT originY)
                    {
                        Assert::IsNull(context); // Not used
                        Assert::IsNotNull(renderer);
                        Assert::AreEqual(1.0f, originX);
                        Assert::AreEqual(2.0f, originY);
                        return S_OK;
                    });

                auto textLayout = f.CreateSimpleTextLayout();

                if (i==0)
                    Assert::AreEqual(S_OK, textLayout->DrawToTextRenderer(f.TextRenderer.Get(), Vector2{ 1, 2 }));
                else
                    Assert::AreEqual(S_OK, textLayout->DrawToTextRendererWithCoords(f.TextRenderer.Get(), 1, 2));
            }
        }

        bool StringEquals(std::wstring const& a, HSTRING const& b)
        {
            std::wstring bString(WindowsGetStringRawBuffer(b, nullptr));
            return a == bString;
        }

        void DrawGlyphRunTestCase(bool useBrush, bool useDescription)
        {
            Fixture f;

            std::wstring localeString = L"xa-yb";
            std::wstring textString = L"Some text";

            unsigned short clusterMapElements[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
            Assert::AreEqual(textString.size(), _countof(clusterMapElements));

            f.TextRenderer->DrawGlyphRunMethod.SetExpectedCalls(1,
                [&](
                Vector2 baselineOrigin,
                ICanvasFontFace* fontFace,
                float fontSize,
                uint32_t glyphCount,
                CanvasGlyph* glyphs,
                boolean isSideways,
                uint32_t bidiLevel,
                IInspectable* brush,
                CanvasTextMeasuringMode measuringMode,
                HSTRING locale,
                HSTRING text,
                uint32_t clusterMapIndicesCount,
                int* clusterMapIndices,
                unsigned int characterIndex,
                CanvasGlyphOrientation glyphOrientation)
            {
                Assert::AreEqual(1.2f, baselineOrigin.X);
                Assert::AreEqual(3.4f, baselineOrigin.Y);
                Assert::IsTrue(IsSameInstance(f.FontFace.Get(), fontFace));
                Assert::AreEqual(11.0f, fontSize);
                Assert::AreEqual(3u, glyphCount);

                for (int i = 0; i < 3; ++i)
                {
                    int inc = i * 4;
                    Assert::AreEqual(inc + 1, glyphs[i].Index);
                    Assert::AreEqual(static_cast<float>(inc)+2.0f, glyphs[i].Advance);
                    Assert::AreEqual(static_cast<float>(inc)+3.0f, glyphs[i].AdvanceOffset);
                    Assert::AreEqual(static_cast<float>(inc)+4.0f, glyphs[i].AscenderOffset);
                }

                Assert::AreEqual(static_cast<boolean>(true), isSideways);
                Assert::AreEqual(5u, bidiLevel);

                if (useBrush)
                    Assert::IsTrue(IsSameInstance(f.SolidColorBrush.Get(), brush));
                else
                    Assert::IsNull(brush);

                Assert::AreEqual(CanvasTextMeasuringMode::GdiNatural, measuringMode);

                if (useDescription)
                {
                    Assert::IsTrue(StringEquals(localeString, locale));
                    Assert::IsTrue(StringEquals(textString, text));

                    Assert::AreEqual(clusterMapIndicesCount, static_cast<uint32_t>(_countof(clusterMapElements)));
                    for (int i = 0; i < _countof(clusterMapElements); ++i)
                    {
                        Assert::AreEqual(clusterMapIndices[i], static_cast<int>(clusterMapElements[i]));
                    }
                    Assert::AreEqual(123u, characterIndex);
                }
                else
                {
                    Assert::IsNull(locale);
                    Assert::IsNull(text);
                    Assert::AreEqual(0u, clusterMapIndicesCount);
                    Assert::IsNull(clusterMapIndices);
                    Assert::AreEqual(0u, characterIndex);                    
                }

                Assert::AreEqual(CanvasGlyphOrientation::Upright, glyphOrientation);

                return S_OK;
            });

            UINT16 glyphIndices[] = { 1, 5, 9 };
            float glyphAdvances[] = { 2.0f, 6.0f, 10.0f };
            DWRITE_GLYPH_OFFSET glyphOffsets[] = { { 3.0f, 4.0f }, { 7.0f, 8.0f }, { 11.0f, 12.0f } };

            f.Adapter->MockTextLayout->DrawMethod.SetExpectedCalls(1,
                [&](void* context, IDWriteTextRenderer* renderer, FLOAT originX, FLOAT originY)
                {
                    DWRITE_GLYPH_RUN glyphRun;
                    glyphRun.fontFace = f.RealizedDWriteFontFace.Get();
                    glyphRun.fontEmSize = 11.0f;
                    glyphRun.glyphCount = 3;
                    glyphRun.glyphIndices = glyphIndices;
                    glyphRun.glyphAdvances = glyphAdvances;
                    glyphRun.glyphOffsets = glyphOffsets;
                    glyphRun.isSideways = true;
                    glyphRun.bidiLevel = 5;

                    DWRITE_GLYPH_RUN_DESCRIPTION glyphRunDescription;
                    if (useDescription)
                    {
                        glyphRunDescription.localeName = localeString.c_str();
                        glyphRunDescription.string = textString.c_str();
                        glyphRunDescription.stringLength = static_cast<uint32_t>(textString.size());
                        glyphRunDescription.clusterMap = clusterMapElements;
                        glyphRunDescription.textPosition = 123u;
                    }

                    renderer->DrawGlyphRun(
                        nullptr,
                        1.2f,
                        3.4f,
                        DWRITE_MEASURING_MODE_GDI_NATURAL,
                        &glyphRun,
                        useDescription? &glyphRunDescription : nullptr,
                        useBrush? f.D2DBrush.Get() : nullptr);

                    return S_OK;
                });

            auto textLayout = f.CreateSimpleTextLayout();

            Assert::AreEqual(S_OK, textLayout->DrawToTextRenderer(f.TextRenderer.Get(), Vector2{ 0, 0 }));
        }

        TEST_METHOD_EX(CanvasTextRenderer_DrawGlyphRun_CallsThrough)
        {
            DrawGlyphRunTestCase(false, false);
            DrawGlyphRunTestCase(false, true);
            DrawGlyphRunTestCase(true, false);
            DrawGlyphRunTestCase(true, true);
        }        

        void DrawStrikethroughTestCase(bool useBrush, bool useLocale)
        {
            Fixture f;

            std::wstring localeString = L"xa-yb";

            f.TextRenderer->DrawStrikethroughMethod.SetExpectedCalls(1,
                [&](
                Vector2 baselineOrigin,
                float width,
                float thickness,
                float offset,
                CanvasTextDirection textDirection,
                IInspectable* brush,
                CanvasTextMeasuringMode measuringMode,
                HSTRING locale,
                CanvasGlyphOrientation glyphOrientation)
            {
                Assert::AreEqual(1.2f, baselineOrigin.X);
                Assert::AreEqual(3.4f, baselineOrigin.Y);

                Assert::AreEqual(5.6f, width);
                Assert::AreEqual(7.8f, thickness);
                Assert::AreEqual(9.0f, offset);

                Assert::AreEqual(CanvasTextDirection::RightToLeftThenBottomToTop, textDirection);

                if (useBrush)
                    Assert::IsTrue(IsSameInstance(f.SolidColorBrush.Get(), brush));
                else
                    Assert::IsNull(brush);

                Assert::AreEqual(CanvasTextMeasuringMode::GdiNatural, measuringMode);

                if (useLocale)
                    Assert::IsTrue(StringEquals(localeString, locale));
                else
                    Assert::IsNull(locale);

                Assert::AreEqual(CanvasGlyphOrientation::Upright, glyphOrientation);

                return S_OK;
            });
            
            f.Adapter->MockTextLayout->DrawMethod.SetExpectedCalls(1,
                [&](void* context, IDWriteTextRenderer* renderer, FLOAT originX, FLOAT originY)
                {
                    DWRITE_STRIKETHROUGH strikethrough;
                    strikethrough.width = 5.6f;
                    strikethrough.thickness = 7.8f;
                    strikethrough.offset = 9.0f;
                    strikethrough.readingDirection = DWRITE_READING_DIRECTION_RIGHT_TO_LEFT;
                    strikethrough.flowDirection = DWRITE_FLOW_DIRECTION_BOTTOM_TO_TOP;
                    strikethrough.localeName = useLocale? localeString.c_str() : nullptr;
                    strikethrough.measuringMode = DWRITE_MEASURING_MODE_GDI_NATURAL;

                    renderer->DrawStrikethrough(
                        nullptr,
                        1.2f,
                        3.4f,
                        &strikethrough,
                        useBrush? f.D2DBrush.Get() : nullptr);

                    return S_OK;
                });

            auto textLayout = f.CreateSimpleTextLayout();

            Assert::AreEqual(S_OK, textLayout->DrawToTextRenderer(f.TextRenderer.Get(), Vector2{ 0, 0 }));
        }

        TEST_METHOD_EX(CanvasTextRenderer_DrawStrikethrough_CallsThrough)
        {
            DrawStrikethroughTestCase(false, false);
            DrawStrikethroughTestCase(false, true);
            DrawStrikethroughTestCase(true, false);
            DrawStrikethroughTestCase(true, true);
        }

        void DrawUnderlineTestCase(bool useBrush, bool useLocale)
        {
            Fixture f;

            std::wstring localeString = L"xa-yb";

            f.TextRenderer->DrawUnderlineMethod.SetExpectedCalls(1,
                [&](
                Vector2 baselineOrigin,
                float width,
                float thickness,
                float offset,
                float runHeight,
                CanvasTextDirection textDirection,
                IInspectable* brush,
                CanvasTextMeasuringMode measuringMode,
                HSTRING locale,
                CanvasGlyphOrientation glyphOrientation)
            {
                Assert::AreEqual(1.2f, baselineOrigin.X);
                Assert::AreEqual(3.4f, baselineOrigin.Y);

                Assert::AreEqual(5.6f, width);
                Assert::AreEqual(7.8f, thickness);
                Assert::AreEqual(9.0f, offset);
                Assert::AreEqual(1.02f, runHeight);

                Assert::AreEqual(CanvasTextDirection::RightToLeftThenBottomToTop, textDirection);

                if (useBrush)
                    Assert::IsTrue(IsSameInstance(f.SolidColorBrush.Get(), brush));
                else
                    Assert::IsNull(brush);

                Assert::AreEqual(CanvasTextMeasuringMode::GdiNatural, measuringMode);

                if (useLocale)
                    Assert::IsTrue(StringEquals(localeString, locale));
                else
                    Assert::IsNull(locale);

                Assert::AreEqual(CanvasGlyphOrientation::Upright, glyphOrientation);

                return S_OK;
            });

            f.Adapter->MockTextLayout->DrawMethod.SetExpectedCalls(1,
                [&](void* context, IDWriteTextRenderer* renderer, FLOAT originX, FLOAT originY)
                {
                    DWRITE_UNDERLINE underline;
                    underline.width = 5.6f;
                    underline.thickness = 7.8f;
                    underline.offset = 9.0f;
                    underline.runHeight = 1.02f;
                    underline.readingDirection = DWRITE_READING_DIRECTION_RIGHT_TO_LEFT;
                    underline.flowDirection = DWRITE_FLOW_DIRECTION_BOTTOM_TO_TOP;
                    underline.localeName = useLocale? localeString.c_str() : nullptr;
                    underline.measuringMode = DWRITE_MEASURING_MODE_GDI_NATURAL;

                    renderer->DrawUnderline(
                        nullptr,
                        1.2f,
                        3.4f,
                        &underline,
                        useBrush ? f.D2DBrush.Get() : nullptr);

                    return S_OK;
                });

            auto textLayout = f.CreateSimpleTextLayout();

            Assert::AreEqual(S_OK, textLayout->DrawToTextRenderer(f.TextRenderer.Get(), Vector2{ 0, 0 }));
        }

        TEST_METHOD_EX(CanvasTextRenderer_DrawUnderline_CallsThrough)
        {
            DrawUnderlineTestCase(false, false);
            DrawUnderlineTestCase(false, true);
            DrawUnderlineTestCase(true, false);
            DrawUnderlineTestCase(true, true);
        }

        TEST_METHOD_EX(CanvasTextRenderer_DrawGlyphRun_SinkReturnsError_ErrorGetsPropagated)
        {
            Fixture f;

            f.TextRenderer->DrawGlyphRunMethod.SetExpectedCalls(1,
                [&](
                    Vector2 baselineOrigin,
                    ICanvasFontFace* fontFace,
                    float fontSize,
                    uint32_t glyphCount,
                    CanvasGlyph* glyphs,
                    boolean isSideways,
                    uint32_t bidiLevel,
                    IInspectable* brush,
                    CanvasTextMeasuringMode measuringMode,
                    HSTRING locale,
                    HSTRING text,
                    uint32_t clusterMapIndicesCount,
                    int* clusterMapIndices,
                    unsigned int characterIndex,
                    CanvasGlyphOrientation glyphOrientation)
                {
                    return sc_someFailureHr;
                });                

            f.Adapter->MockTextLayout->DrawMethod.SetExpectedCalls(1,
                [&](void* context, IDWriteTextRenderer* renderer, FLOAT originX, FLOAT originY)
                {
                    DWRITE_GLYPH_RUN glyphRun{};
                    glyphRun.fontFace = f.RealizedDWriteFontFace.Get();

                    return renderer->DrawGlyphRun(nullptr, 0.0f, 0.0f,  DWRITE_MEASURING_MODE_NATURAL, &glyphRun, nullptr, nullptr);

                });

            auto textLayout = f.CreateSimpleTextLayout();

            Assert::AreEqual(sc_someFailureHr, textLayout->DrawToTextRenderer(f.TextRenderer.Get(), Vector2{ 0, 0 }));
        }

        TEST_METHOD_EX(CanvasTextRenderer_DrawStrikethrough_SinkReturnsError_ErrorGetsPropagated)
        {
            Fixture f;

            f.TextRenderer->DrawStrikethroughMethod.SetExpectedCalls(1,
                [&](
                Vector2 baselineOrigin,
                float width,
                float thickness,
                float offset,
                CanvasTextDirection textDirection,
                IInspectable* brush,
                CanvasTextMeasuringMode measuringMode,
                HSTRING locale,
                CanvasGlyphOrientation glyphOrientation)
                {
                    return sc_someFailureHr;
                });
                

            f.Adapter->MockTextLayout->DrawMethod.SetExpectedCalls(1,
                [&](void* context, IDWriteTextRenderer* renderer, FLOAT originX, FLOAT originY)
                {
                    DWRITE_STRIKETHROUGH strikethrough{};
                    return renderer->DrawStrikethrough(nullptr, 0.0f, 0.0f, &strikethrough, nullptr);
                });

            auto textLayout = f.CreateSimpleTextLayout();

            Assert::AreEqual(sc_someFailureHr, textLayout->DrawToTextRenderer(f.TextRenderer.Get(), Vector2{ 0, 0 }));
        }

        TEST_METHOD_EX(CanvasTextRenderer_DrawUnderline_SinkReturnsError_ErrorGetsPropagated)
        {
            Fixture f;

            std::wstring localeString = L"xa-yb";

            f.TextRenderer->DrawUnderlineMethod.SetExpectedCalls(1,
                [&](
                Vector2 baselineOrigin,
                float width,
                float thickness,
                float offset,
                float runHeight,
                CanvasTextDirection textDirection,
                IInspectable* brush,
                CanvasTextMeasuringMode measuringMode,
                HSTRING locale,
                CanvasGlyphOrientation glyphOrientation)
            {
                return sc_someFailureHr;
            });
                

            f.Adapter->MockTextLayout->DrawMethod.SetExpectedCalls(1,
                [&](void* context, IDWriteTextRenderer* renderer, FLOAT originX, FLOAT originY)
                {
                    DWRITE_UNDERLINE underline{};
                    return renderer->DrawUnderline(nullptr, 0.0f, 0.0f, &underline, nullptr);
                });

            auto textLayout = f.CreateSimpleTextLayout();

            Assert::AreEqual(sc_someFailureHr, textLayout->DrawToTextRenderer(f.TextRenderer.Get(), Vector2{ 0, 0 }));
        }

        TEST_METHOD_EX(CanvasTextRenderer_get_Dpi)
        {
            Fixture f;

            f.TextRenderer->get_DpiMethod.SetExpectedCalls(1,
                [&](float* out)
                {
                    *out = DEFAULT_DPI * 2;
                    return S_OK;
                });
                

            f.Adapter->MockTextLayout->DrawMethod.SetExpectedCalls(1,
                [&](void* context, IDWriteTextRenderer* renderer, FLOAT originX, FLOAT originY)
                {
                    float value;
                    ThrowIfFailed(renderer->GetPixelsPerDip(nullptr, &value));
                    Assert::AreEqual(2.0f, value);
                    return S_OK;
                });

            auto textLayout = f.CreateSimpleTextLayout();

            Assert::AreEqual(S_OK, textLayout->DrawToTextRenderer(f.TextRenderer.Get(), Vector2{ 0, 0 }));
        }

        TEST_METHOD_EX(CanvasTextRenderer_get_Dpi_ErrorPropagates)
        {
            Fixture f;

            f.TextRenderer->get_DpiMethod.SetExpectedCalls(1,
                [&](float* out)
                {
                    return sc_someFailureHr;
                });
                

            f.Adapter->MockTextLayout->DrawMethod.SetExpectedCalls(1,
                [&](void* context, IDWriteTextRenderer* renderer, FLOAT originX, FLOAT originY)
                {
                    float value;
                    return renderer->GetPixelsPerDip(nullptr, &value);
                });

            auto textLayout = f.CreateSimpleTextLayout();
            Assert::AreEqual(sc_someFailureHr, textLayout->DrawToTextRenderer(f.TextRenderer.Get(), Vector2{ 0, 0 }));
        }

        TEST_METHOD_EX(CanvasTextRenderer_get_PixelSnappingDisabled)
        {
            Fixture f;

            f.TextRenderer->get_PixelSnappingDisabledMethod.SetExpectedCalls(1,
                [&](boolean* out)
                {
                    *out = true;
                    return S_OK;
                });
                

            f.Adapter->MockTextLayout->DrawMethod.SetExpectedCalls(1,
                [&](void* context, IDWriteTextRenderer* renderer, FLOAT originX, FLOAT originY)
                {
                    BOOL value;
                    ThrowIfFailed(renderer->IsPixelSnappingDisabled(nullptr, &value));
                    Assert::AreEqual(TRUE, value);
                    return S_OK;
                });

            auto textLayout = f.CreateSimpleTextLayout();

            Assert::AreEqual(S_OK, textLayout->DrawToTextRenderer(f.TextRenderer.Get(), Vector2{ 0, 0 }));
        }

        TEST_METHOD_EX(CanvasTextRenderer_get_PixelSnappingDisabled_ErrorPropagates)
        {
            Fixture f;

            f.TextRenderer->get_PixelSnappingDisabledMethod.SetExpectedCalls(1,
                [&](boolean* out)
                {
                    return sc_someFailureHr;
                });
                

            f.Adapter->MockTextLayout->DrawMethod.SetExpectedCalls(1,
                [&](void* context, IDWriteTextRenderer* renderer, FLOAT originX, FLOAT originY)
                {
                    BOOL value;
                    return renderer->IsPixelSnappingDisabled(nullptr, &value);
                });

            auto textLayout = f.CreateSimpleTextLayout();

            Assert::AreEqual(sc_someFailureHr, textLayout->DrawToTextRenderer(f.TextRenderer.Get(), Vector2{ 0, 0 }));
        }

        TEST_METHOD_EX(CanvasTextRenderer_get_Transform)
        {
            Fixture f;

            f.TextRenderer->get_TransformMethod.SetExpectedCalls(1,
                [&](Matrix3x2* out)
                {
                    *out = Matrix3x2{ 1, 2, 3, 4, 5, 6 };
                    return S_OK;
                });
                

            f.Adapter->MockTextLayout->DrawMethod.SetExpectedCalls(1,
                [&](void* context, IDWriteTextRenderer* renderer, FLOAT originX, FLOAT originY)
                {
                    DWRITE_MATRIX value;
                    ThrowIfFailed(renderer->GetCurrentTransform(nullptr, &value));

                    DWRITE_MATRIX expected = { 1, 2, 3, 4, 5, 6 };
                    Assert::AreEqual(expected, value);
                    return S_OK;
                });

            auto textLayout = f.CreateSimpleTextLayout();

            Assert::AreEqual(S_OK, textLayout->DrawToTextRenderer(f.TextRenderer.Get(), Vector2{ 0, 0 }));
        }

        TEST_METHOD_EX(CanvasTextRenderer_get_Transform_ErrorPropagates)
        {
            Fixture f;

            f.TextRenderer->get_TransformMethod.SetExpectedCalls(1,
                [&](Matrix3x2* out)
                {
                    return sc_someFailureHr;
                });
                

            f.Adapter->MockTextLayout->DrawMethod.SetExpectedCalls(1,
                [&](void* context, IDWriteTextRenderer* renderer, FLOAT originX, FLOAT originY)
                {
                    DWRITE_MATRIX value;
                    return renderer->GetCurrentTransform(nullptr, &value);
                });

            auto textLayout = f.CreateSimpleTextLayout();

            Assert::AreEqual(sc_someFailureHr, textLayout->DrawToTextRenderer(f.TextRenderer.Get(), Vector2{ 0, 0 }));
        }        

        void DrawInlineObjectTestCase(bool useBrush)
        {
            Fixture f;

            f.TextRenderer->DrawInlineObjectMethod.SetExpectedCalls(1,
                [&](
                Vector2 baselineOrigin,
                ICanvasTextInlineObject* inlineObject,
                boolean isSideways,
                boolean isRightToLeft,
                IInspectable* brush,
                CanvasGlyphOrientation glyphOrientation)
            {
                Assert::AreEqual(1.2f, baselineOrigin.X);
                Assert::AreEqual(3.4f, baselineOrigin.Y);
                Assert::IsTrue(IsSameInstance(f.InlineObject.Get(), inlineObject));
                Assert::IsTrue(!!isSideways);
                Assert::IsTrue(!!isRightToLeft);

                if (useBrush)
                    Assert::IsTrue(IsSameInstance(f.SolidColorBrush.Get(), brush));
                else
                    Assert::IsNull(brush);

                Assert::AreEqual(CanvasGlyphOrientation::Upright, glyphOrientation);

                return S_OK;
            });

            f.Adapter->MockTextLayout->DrawMethod.SetExpectedCalls(1,
                [&](void* context, IDWriteTextRenderer* renderer, FLOAT originX, FLOAT originY)
                {
                    renderer->DrawInlineObject(
                        nullptr,
                        1.2f,
                        3.4f,
                        f.DWriteInlineObject.Get(),
                        true,
                        true,
                        useBrush? f.D2DBrush.Get() : nullptr);

                    return S_OK;
                });

            auto textLayout = f.CreateSimpleTextLayout();

            Assert::AreEqual(S_OK, textLayout->DrawToTextRenderer(f.TextRenderer.Get(), Vector2{ 0, 0 }));
        }

        TEST_METHOD_EX(CanvasTextRenderer_DrawInlineObject_CallsThrough)
        {
            DrawInlineObjectTestCase(false);
            DrawInlineObjectTestCase(true);
        }
    };
}