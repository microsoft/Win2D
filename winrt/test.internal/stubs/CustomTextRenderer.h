// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace canvas
{
    class CustomTextRenderer : public RuntimeClass <
        RuntimeClassFlags<WinRtClassicComMix>,
        ICanvasTextRenderer >
    {
    public:

        CALL_COUNTER_WITH_MOCK(DrawGlyphRunMethod, HRESULT(Vector2, ICanvasFontFace*, float, uint32_t, CanvasGlyph*, boolean, uint32_t, IInspectable*, CanvasTextMeasuringMode, HSTRING, HSTRING, uint32_t, int*, unsigned int, CanvasGlyphOrientation));
        CALL_COUNTER_WITH_MOCK(DrawStrikethroughMethod, HRESULT(Vector2, float, float, float, CanvasTextDirection, IInspectable*, CanvasTextMeasuringMode, HSTRING, CanvasGlyphOrientation));
        CALL_COUNTER_WITH_MOCK(DrawUnderlineMethod, HRESULT(Vector2, float, float, float, float, CanvasTextDirection, IInspectable*, CanvasTextMeasuringMode, HSTRING, CanvasGlyphOrientation));
        CALL_COUNTER_WITH_MOCK(DrawInlineObjectMethod, HRESULT(Vector2, ICanvasTextInlineObject*, boolean, boolean, IInspectable*, CanvasGlyphOrientation));
        CALL_COUNTER_WITH_MOCK(get_DpiMethod, HRESULT(float*));
        CALL_COUNTER_WITH_MOCK(get_PixelSnappingDisabledMethod, HRESULT(boolean*));
        CALL_COUNTER_WITH_MOCK(get_TransformMethod, HRESULT(Matrix3x2*));

        CustomTextRenderer()
        {
            DrawGlyphRunMethod.AllowAnyCall();
            DrawStrikethroughMethod.AllowAnyCall();
            DrawUnderlineMethod.AllowAnyCall();
            DrawInlineObjectMethod.AllowAnyCall();

            get_DpiMethod.AllowAnyCall([&](float* out){*out = DEFAULT_DPI;  return S_OK; });
            get_PixelSnappingDisabledMethod.AllowAnyCall([&](boolean* out){ *out = true;  return S_OK; });
            get_TransformMethod.AllowAnyCall([&](Matrix3x2* out){ *out = { 1, 0, 0, 1, 0, 0 };  return S_OK; });
        }

        IFACEMETHODIMP DrawGlyphRun(
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
            CanvasGlyphOrientation glyphOrientation) override
        {
            return DrawGlyphRunMethod.WasCalled(baselineOrigin, fontFace, fontSize, glyphCount, glyphs, isSideways, bidiLevel, brush, measuringMode, locale, text, clusterMapIndicesCount, clusterMapIndices, characterIndex, glyphOrientation);
        }

        IFACEMETHODIMP DrawStrikethrough(
            Vector2 baselineOrigin,
            float width,
            float thickness,
            float offset,
            CanvasTextDirection textDirection,
            IInspectable* brush,
            CanvasTextMeasuringMode measuringMode,
            HSTRING text,
            CanvasGlyphOrientation glyphOrientation)
        {
            return DrawStrikethroughMethod.WasCalled(baselineOrigin, width, thickness, offset, textDirection, brush, measuringMode, text, glyphOrientation);
        }

        IFACEMETHODIMP DrawUnderline(
            Vector2 baselineOrigin,
            float width,
            float thickness,
            float offset,
            float runHeight,
            CanvasTextDirection textDirection,
            IInspectable* brush,
            CanvasTextMeasuringMode measuringMode,
            HSTRING text,
            CanvasGlyphOrientation glyphOrientation)
        {
            return DrawUnderlineMethod.WasCalled(baselineOrigin, width, thickness, offset, runHeight, textDirection, brush, measuringMode, text, glyphOrientation);
        }

        IFACEMETHODIMP DrawInlineObject(
            Vector2 baselineOrigin,
            ICanvasTextInlineObject* inlineObject,
            boolean isSideways,
            boolean isRightToLeft,
            IInspectable* brush,
            CanvasGlyphOrientation glyphOrientation) override
        {
            return DrawInlineObjectMethod.WasCalled(baselineOrigin, inlineObject, isSideways, isRightToLeft, brush, glyphOrientation);
        }

        virtual IFACEMETHODIMP get_Dpi(float* value)
        {
            return get_DpiMethod.WasCalled(value);
        }

        virtual IFACEMETHODIMP get_PixelSnappingDisabled(boolean* value)
        {
            return get_PixelSnappingDisabledMethod.WasCalled(value);
        }

        virtual IFACEMETHODIMP get_Transform(Matrix3x2* value)
        {
            return get_TransformMethod.WasCalled(value);
        }
    };
}