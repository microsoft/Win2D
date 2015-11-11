// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#include "pch.h"

#include "DrawGlyphRunHelper.h"
#include "CanvasFontFace.h"

DrawGlyphRunHelper::DrawGlyphRunHelper(
    ICanvasFontFace* fontFace,
    float fontSize,
    uint32_t glyphCount,
    CanvasGlyph* glyphs,
    boolean isSideways,
    uint32_t bidiLevel,
    IInspectable* brush,
    CanvasTextMeasuringMode textMeasuringMode,
    HSTRING localeName,
    HSTRING text,
    uint32_t clusterMapIndicesCount,
    int* clusterMapIndices,
    uint32_t textPosition,
    ComPtr<ID2D1DeviceContext> const& deviceContext)
    : DWriteGlyphRun{}
    , DWriteGlyphRunDescription{}
{
    DWriteGlyphRun.bidiLevel = bidiLevel;
    DWriteGlyphRun.fontEmSize = fontSize;
    DWriteGlyphRun.fontFace = As<ICanvasFontFaceInternal>(fontFace)->GetRealizedFontFace().Get();

    GlyphAdvances.reserve(glyphCount);
    GlyphIndices.reserve(glyphCount);
    GlyphOffsets.reserve(glyphCount);
    for (uint32_t i = 0; i < glyphCount; ++i)
    {
        GlyphAdvances.push_back(glyphs[i].Advance);

        GlyphIndices.push_back(CheckCastAsUShort(glyphs[i].Index));

        DWRITE_GLYPH_OFFSET offset;
        offset.advanceOffset = glyphs[i].AdvanceOffset;
        offset.ascenderOffset = glyphs[i].AscenderOffset;
        GlyphOffsets.push_back(offset);
    }
    DWriteGlyphRun.glyphCount = glyphCount;
    DWriteGlyphRun.glyphAdvances = GlyphAdvances.data();
    DWriteGlyphRun.glyphIndices = GlyphIndices.data();
    DWriteGlyphRun.glyphOffsets = GlyphOffsets.data();
    DWriteGlyphRun.isSideways = isSideways;

    uint32_t textStringLength;
    wchar_t const* textString = WindowsGetStringRawBuffer(text, &textStringLength);

    ClusterMapElements.reserve(textStringLength);

    if (clusterMapIndices)
    {
        for (uint32_t i = 0; i < clusterMapIndicesCount; ++i)
        {
            ClusterMapElements.push_back(CheckCastAsUShort(clusterMapIndices[i]));
        }
    }
    DWriteGlyphRunDescription.clusterMap = ClusterMapElements.data();

    // This helper structure isn't intended to outlive the arguments used to create it.
    DWriteGlyphRunDescription.localeName = WindowsGetStringRawBuffer(localeName, nullptr);
    DWriteGlyphRunDescription.string = textString;
    DWriteGlyphRunDescription.stringLength = textStringLength;

    DWriteGlyphRunDescription.textPosition = textPosition;

    ClientDrawingEffect = GetClientDrawingEffect(brush, deviceContext);

    MeasuringMode = ToDWriteMeasuringMode(textMeasuringMode);
}

ComPtr<IUnknown> DrawGlyphRunHelper::GetClientDrawingEffect(
    ComPtr<IInspectable> const& inspectable, 
    ComPtr<ID2D1DeviceContext> const& deviceContext)
{
    ComPtr<IUnknown> result = inspectable;
    if (inspectable)
    {
        auto canvasBrushInternal = MaybeAs<ICanvasBrushInternal>(inspectable);
        if (canvasBrushInternal)
        {
            result = canvasBrushInternal->GetD2DBrush(deviceContext.Get(), GetBrushFlags::None);
        }
    }
    return result;
}