// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Text
{    
    class InternalDWriteTextRenderer : 
        public RuntimeClass<RuntimeClassFlags<ClassicCom>, IDWriteTextRenderer1>,
        private LifespanTracker<InternalDWriteTextRenderer>
    {
        ComPtr<ICanvasDevice> m_device;
        ComPtr<ICanvasTextRenderer> m_textRenderer;
    public:
        InternalDWriteTextRenderer(ComPtr<ICanvasDevice> const& device, ICanvasTextRenderer* textRenderer)
            : m_device(device)
            , m_textRenderer(textRenderer)
        {
        }

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

        IFACEMETHODIMP DrawGlyphRun(
            void*,
            FLOAT baselineOriginX,
            FLOAT baselineOriginY,
            DWRITE_GLYPH_ORIENTATION_ANGLE orientationAngle,
            DWRITE_MEASURING_MODE measuringMode,
            DWRITE_GLYPH_RUN const* glyphRun,
            DWRITE_GLYPH_RUN_DESCRIPTION const* glyphRunDescription,
            IUnknown* customDrawingObject);

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

        IFACEMETHODIMP DrawUnderline(
            void*,
            FLOAT baselineOriginX,
            FLOAT baselineOriginY,
            DWRITE_GLYPH_ORIENTATION_ANGLE orientationAngle,
            DWRITE_UNDERLINE const* underline,
            IUnknown* customDrawingObject);

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

        IFACEMETHODIMP DrawStrikethrough(
            void*,
            FLOAT baselineOriginX,
            FLOAT baselineOriginY,
            DWRITE_GLYPH_ORIENTATION_ANGLE orientationAngle,
            DWRITE_STRIKETHROUGH const* strikethrough,
            IUnknown* customDrawingObject);

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
                DWRITE_GLYPH_ORIENTATION_ANGLE_0_DEGREES,
                inlineObject,
                isSideways,
                isRightToLeft,
                brush);
        }

        IFACEMETHODIMP DrawInlineObject(
            void* clientDrawingContext,
            FLOAT baselineOriginX,
            FLOAT baselineOriginY,
            DWRITE_GLYPH_ORIENTATION_ANGLE orientationAngle,
            IDWriteInlineObject* inlineObject,
            BOOL isSideways,
            BOOL isRightToLeft,
            IUnknown* brush);

        IFACEMETHODIMP IsPixelSnappingDisabled(
            void*,
            BOOL* isDisabled)
        {
            return ExceptionBoundary(
                [&]
            {
                boolean value;
                ThrowIfFailed(m_textRenderer->get_PixelSnappingDisabled(&value));

                *isDisabled = value;
            });
        }

        IFACEMETHODIMP GetCurrentTransform(
            void*,
            DWRITE_MATRIX* transform)
        {
            return ExceptionBoundary(
                [&]
            {
                Matrix3x2 value;
                ThrowIfFailed(m_textRenderer->get_Transform(&value));

                *transform = *ReinterpretAs<DWRITE_MATRIX*>(&value);
            });
        }

        IFACEMETHODIMP GetPixelsPerDip(
            void*,
            FLOAT* pixelsPerDip)
        {
            return ExceptionBoundary(
                [&]
            {
                float value;
                ThrowIfFailed(m_textRenderer->get_Dpi(&value));

                *pixelsPerDip = value / DEFAULT_DPI;
            });
        }
    };

}}}}}
