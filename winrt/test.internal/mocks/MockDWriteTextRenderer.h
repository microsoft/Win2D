// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace canvas
{
    class MockDWriteTextRenderer : public RuntimeClass <
        RuntimeClassFlags<ClassicCom>,
        ChainInterfaces < IDWriteTextRenderer1, IDWriteTextRenderer >>
    {
    public:

        MOCK_METHOD8(DrawGlyphRun, HRESULT(void*, FLOAT, FLOAT, DWRITE_GLYPH_ORIENTATION_ANGLE, DWRITE_MEASURING_MODE, DWRITE_GLYPH_RUN const*, DWRITE_GLYPH_RUN_DESCRIPTION const*, IUnknown*));
        MOCK_METHOD6(DrawUnderline, HRESULT(void*, FLOAT, FLOAT, DWRITE_GLYPH_ORIENTATION_ANGLE, DWRITE_UNDERLINE const*, IUnknown*));
        MOCK_METHOD6(DrawStrikethrough, HRESULT(void*, FLOAT, FLOAT, DWRITE_GLYPH_ORIENTATION_ANGLE, DWRITE_STRIKETHROUGH const*, IUnknown*));
        MOCK_METHOD8(DrawInlineObject, HRESULT(void*, FLOAT, FLOAT, DWRITE_GLYPH_ORIENTATION_ANGLE, IDWriteInlineObject*, BOOL, BOOL, IUnknown*));
        MOCK_METHOD2(IsPixelSnappingDisabled, HRESULT(void*, BOOL*));
        MOCK_METHOD2(GetCurrentTransform, HRESULT(void*, DWRITE_MATRIX*));
        MOCK_METHOD2(GetPixelsPerDip, HRESULT(void*, FLOAT*));

        IFACEMETHODIMP DrawGlyphRun(
            void* clientDrawingContext,
            FLOAT baselineOriginX,
            FLOAT baselineOriginY,
            DWRITE_MEASURING_MODE measuringMode,
            DWRITE_GLYPH_RUN const* glyphRun,
            DWRITE_GLYPH_RUN_DESCRIPTION const* glyphRunDescription,
            IUnknown* clientDrawingEffect)
        {
            return DrawGlyphRun(
                clientDrawingContext,
                baselineOriginX,
                baselineOriginY,
                DWRITE_GLYPH_ORIENTATION_ANGLE_0_DEGREES,
                measuringMode,
                glyphRun,
                glyphRunDescription,
                clientDrawingEffect);
        }

        IFACEMETHODIMP DrawUnderline(
            void* clientDrawingContext,
            FLOAT baselineOriginX,
            FLOAT baselineOriginY,
            DWRITE_UNDERLINE const* underline,
            IUnknown* clientDrawingEffect)
        {
            return DrawUnderline(
                clientDrawingContext,
                baselineOriginX,
                baselineOriginY,
                DWRITE_GLYPH_ORIENTATION_ANGLE_0_DEGREES,
                underline,
                clientDrawingEffect);
        }

        IFACEMETHODIMP DrawStrikethrough(
            void* clientDrawingContext,
            FLOAT baselineOriginX,
            FLOAT baselineOriginY,
            DWRITE_STRIKETHROUGH const* strikethrough,
            IUnknown* clientDrawingEffect)
        {
            return DrawStrikethrough(
                clientDrawingContext,
                baselineOriginX,
                baselineOriginY,
                DWRITE_GLYPH_ORIENTATION_ANGLE_0_DEGREES,
                strikethrough,
                clientDrawingEffect);
        }

        IFACEMETHODIMP DrawInlineObject(
            void* clientDrawingContext,
            FLOAT baselineOriginX,
            FLOAT baselineOriginY,
            IDWriteInlineObject* inlineObject,
            BOOL isSideways,
            BOOL isRightToLeft,
            IUnknown* brush)
        {
            return DrawInlineObject(
                clientDrawingContext,
                baselineOriginX,
                baselineOriginY,
                inlineObject,
                isSideways,
                isRightToLeft,
                brush);
        }
    };
}