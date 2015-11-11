// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Text
{
    struct DrawGlyphRunHelper
    {
        DWRITE_GLYPH_RUN DWriteGlyphRun;
        DWRITE_GLYPH_RUN_DESCRIPTION DWriteGlyphRunDescription;
        std::vector<unsigned short> ClusterMapElements;
        std::vector<float> GlyphAdvances;
        std::vector<unsigned short> GlyphIndices;
        std::vector<DWRITE_GLYPH_OFFSET> GlyphOffsets;
        ComPtr<IUnknown> ClientDrawingEffect;
        DWRITE_MEASURING_MODE MeasuringMode;

        DrawGlyphRunHelper(
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
            ComPtr<ID2D1DeviceContext> const& deviceContext);

        static ComPtr<IUnknown> GetClientDrawingEffect(
            ComPtr<IInspectable> const& inspectable,
            ComPtr<ID2D1DeviceContext> const& deviceContext);
    };

}}}}}
