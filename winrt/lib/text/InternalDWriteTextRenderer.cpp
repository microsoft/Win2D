// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#include "pch.h"

#include "InternalDWriteTextRenderer.h"
#include "CanvasFontFace.h"

IFACEMETHODIMP InternalDWriteTextRenderer::DrawGlyphRun(
    void*,
    FLOAT baselineOriginX,
    FLOAT baselineOriginY,
    DWRITE_GLYPH_ORIENTATION_ANGLE orientationAngle,
    DWRITE_MEASURING_MODE measuringMode,
    DWRITE_GLYPH_RUN const* glyphRun,
    DWRITE_GLYPH_RUN_DESCRIPTION const* glyphRunDescription,
    IUnknown* customDrawingObject)
{
    return ExceptionBoundary(
        [&]
        {
            auto customDrawingObjectInspectable = GetCustomDrawingObjectInspectable(m_device.Get(), customDrawingObject);

            auto canvasFontFace = CanvasFontFace::GetOrCreate(As<IDWriteFontFace2>(glyphRun->fontFace).Get());

            std::vector<CanvasGlyph> glyphs;
            glyphs.reserve(glyphRun->glyphCount);
            for (uint32_t i = 0; i < glyphRun->glyphCount; ++i)
            {
                CanvasGlyph glyph{};
                glyph.Advance = glyphRun->glyphAdvances[i];
                glyph.Index = glyphRun->glyphIndices[i];
                if (glyphRun->glyphOffsets)
                {
                    glyph.AdvanceOffset = glyphRun->glyphOffsets[i].advanceOffset;
                    glyph.AscenderOffset = glyphRun->glyphOffsets[i].ascenderOffset;
                }
                glyphs.push_back(glyph);
            }

            WinString localeNameString;
            WinString textString;
            std::vector<int> clusterMapIndices;

            if (glyphRunDescription)
            {
                localeNameString = WinString(glyphRunDescription->localeName);
                textString = WinString(glyphRunDescription->string);
                clusterMapIndices.reserve(glyphRunDescription->stringLength);
                for (uint32_t i = 0; i < glyphRunDescription->stringLength; ++i)
                {
                    clusterMapIndices.push_back(glyphRunDescription->clusterMap[i]);
                }
            }

            ThrowIfFailed(m_textRenderer->DrawGlyphRun(
                Vector2{ baselineOriginX, baselineOriginY },
                canvasFontFace.Get(),
                glyphRun->fontEmSize,
                glyphRun->glyphCount,
                glyphs.data(),
                static_cast<boolean>(glyphRun->isSideways),
                glyphRun->bidiLevel,
                customDrawingObjectInspectable.Get(),
                ToCanvasTextMeasuringMode(measuringMode),
                localeNameString,
                textString,
                glyphRunDescription ? glyphRunDescription->stringLength : 0u,
                glyphRunDescription ? clusterMapIndices.data() : nullptr,
                glyphRunDescription ? glyphRunDescription->textPosition : 0u,
                ToCanvasGlyphOrientation(orientationAngle)));
        });
}

IFACEMETHODIMP InternalDWriteTextRenderer::DrawUnderline(
    void*,
    FLOAT baselineOriginX,
    FLOAT baselineOriginY,
    DWRITE_GLYPH_ORIENTATION_ANGLE orientationAngle,
    DWRITE_UNDERLINE const* underline,
    IUnknown* customDrawingObject)
{
    return ExceptionBoundary(
        [&]
        {
            //
            // The renderer isn't required to specify a locale name.
            //
            WinString localeName;
            if (underline->localeName)
                localeName = WinString(underline->localeName);

            auto customDrawingObjectInspectable = GetCustomDrawingObjectInspectable(m_device.Get(), customDrawingObject);

            ThrowIfFailed(m_textRenderer->DrawUnderline(
                Vector2{ baselineOriginX, baselineOriginY },
                underline->width,
                underline->thickness,
                underline->offset,
                underline->runHeight,
                DWriteToCanvasTextDirection::Lookup(underline->readingDirection, underline->flowDirection)->TextDirection,
                customDrawingObjectInspectable.Get(),
                ToCanvasTextMeasuringMode(underline->measuringMode),
                localeName,
                ToCanvasGlyphOrientation(orientationAngle)));
        });
}


IFACEMETHODIMP InternalDWriteTextRenderer::DrawStrikethrough(
    void*,
    FLOAT baselineOriginX,
    FLOAT baselineOriginY,
    DWRITE_GLYPH_ORIENTATION_ANGLE orientationAngle,
    DWRITE_STRIKETHROUGH const* strikethrough,
    IUnknown* customDrawingObject)
{
    return ExceptionBoundary(
        [&]
        {
            WinString localeName;
            if (strikethrough->localeName)
                localeName = WinString(strikethrough->localeName);

            auto customDrawingObjectInspectable = GetCustomDrawingObjectInspectable(m_device.Get(), customDrawingObject);

            ThrowIfFailed(m_textRenderer->DrawStrikethrough(
                Vector2{ baselineOriginX, baselineOriginY },
                strikethrough->width,
                strikethrough->thickness,
                strikethrough->offset,
                DWriteToCanvasTextDirection::Lookup(strikethrough->readingDirection, strikethrough->flowDirection)->TextDirection,
                customDrawingObjectInspectable.Get(),
                ToCanvasTextMeasuringMode(strikethrough->measuringMode),
                localeName,
                ToCanvasGlyphOrientation(orientationAngle)));
        });
}


IFACEMETHODIMP InternalDWriteTextRenderer::DrawInlineObject(
    void*,
    FLOAT baselineOriginX,
    FLOAT baselineOriginY,
    DWRITE_GLYPH_ORIENTATION_ANGLE orientationAngle,
    IDWriteInlineObject* inlineObject,
    BOOL isSideways,
    BOOL isRightToLeft,
    IUnknown* brush)
{
    return ExceptionBoundary(
        [&]
        {
            auto canvasInlineObject = GetCanvasInlineObjectFromDWriteInlineObject(inlineObject, false);

            auto customDrawingObjectInspectable = GetCustomDrawingObjectInspectable(m_device.Get(), brush);

            if (canvasInlineObject)
            {
                ThrowIfFailed(m_textRenderer->DrawInlineObject(
                    Vector2{ baselineOriginX, baselineOriginY },
                    canvasInlineObject.Get(),
                    !!isSideways,
                    !!isRightToLeft,
                    customDrawingObjectInspectable.Get(),
                    ToCanvasGlyphOrientation(orientationAngle)));
            }
            else
            {
                //
                // Inline objects may be implemented by the app. Or, they may be implemented by
                // DWrite, an important example of which being an ellipsis trimming sign. 
                // We've reached this spot if it's the latter.
                // 
                // In this case, we don't call into the app's custom renderer callback since
                // it doesn't know anything about the DWrite-implemented inline object.
                // We forward the Draw call along.
                //
                ThrowIfFailed(inlineObject->Draw(nullptr, this, baselineOriginX, baselineOriginY, isSideways, isRightToLeft, brush));
            }
        });
}