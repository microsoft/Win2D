// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace canvas
{
    class MockDWriteFontFace
        : public RuntimeClass<
        RuntimeClassFlags<ClassicCom>,
        ChainInterfaces<IDWriteFontFace3, IDWriteFontFace2, IDWriteFontFace1, IDWriteFontFace>>
    {
    public:

        CALL_COUNTER_WITH_MOCK(GetTypeMethod, DWRITE_FONT_FACE_TYPE());
        CALL_COUNTER_WITH_MOCK(GetFilesMethod, HRESULT(uint32_t*, IDWriteFontFile**));
        CALL_COUNTER_WITH_MOCK(GetIndexMethod, uint32_t());
        CALL_COUNTER_WITH_MOCK(GetSimulationsMethod, DWRITE_FONT_SIMULATIONS());
        CALL_COUNTER_WITH_MOCK(IsSymbolFontMethod, BOOL());
        CALL_COUNTER_WITH_MOCK(GetMetricsMethod0, void(DWRITE_FONT_METRICS*));
        CALL_COUNTER_WITH_MOCK(GetGlyphCountMethod, UINT16());
        CALL_COUNTER_WITH_MOCK(GetDesignGlyphMetricsMethod, HRESULT(UINT16 const*, uint32_t, DWRITE_GLYPH_METRICS*, BOOL));
        CALL_COUNTER_WITH_MOCK(GetGlyphIndicesMethod, HRESULT(uint32_t const*, uint32_t, UINT16*));
        CALL_COUNTER_WITH_MOCK(TryGetFontTableMethod, HRESULT(uint32_t, void const**, uint32_t*, void**, BOOL*));
        CALL_COUNTER_WITH_MOCK(ReleaseFontTableMethod, HRESULT(void*));
        CALL_COUNTER_WITH_MOCK(GetGlyphRunOutlineMethod, HRESULT(FLOAT, UINT16 const*, FLOAT const*, DWRITE_GLYPH_OFFSET const*, uint32_t, BOOL, BOOL, IDWriteGeometrySink*));
        CALL_COUNTER_WITH_MOCK(GetRecommendedRenderingModeMethod0, HRESULT(FLOAT, FLOAT, DWRITE_MEASURING_MODE, IDWriteRenderingParams*, DWRITE_RENDERING_MODE*));
        CALL_COUNTER_WITH_MOCK(GetGdiCompatibleMetricsMethod0, HRESULT(FLOAT, FLOAT, DWRITE_MATRIX const*, DWRITE_FONT_METRICS*));
        CALL_COUNTER_WITH_MOCK(GetGdiCompatibleGlyphMetricsMethod, HRESULT(FLOAT, FLOAT, DWRITE_MATRIX const*, BOOL, UINT16 const*, uint32_t, DWRITE_GLYPH_METRICS*, BOOL));

        CALL_COUNTER_WITH_MOCK(GetMetricsMethod1, void(DWRITE_FONT_METRICS1*));
        CALL_COUNTER_WITH_MOCK(GetGdiCompatibleMetricsMethod1, HRESULT(FLOAT, FLOAT, DWRITE_MATRIX const*, DWRITE_FONT_METRICS1*));
        CALL_COUNTER_WITH_MOCK(GetCaretMetricsMethod, void(DWRITE_CARET_METRICS*));
        CALL_COUNTER_WITH_MOCK(GetUnicodeRangesMethod, HRESULT(uint32_t, DWRITE_UNICODE_RANGE*, uint32_t*));
        CALL_COUNTER_WITH_MOCK(IsMonospacedFontMethod, BOOL());
        CALL_COUNTER_WITH_MOCK(GetDesignGlyphAdvancesMethod, HRESULT(uint32_t, UINT16 const*, INT32*, BOOL));
        CALL_COUNTER_WITH_MOCK(GetGdiCompatibleGlyphAdvancesMethod, HRESULT(FLOAT, FLOAT, DWRITE_MATRIX const*, BOOL, BOOL, uint32_t, UINT16 const*, INT32*));
        CALL_COUNTER_WITH_MOCK(GetKerningPairAdjustmentsMethod, HRESULT(uint32_t, UINT16 const*, INT32*));
        CALL_COUNTER_WITH_MOCK(HasKerningPairsMethod, BOOL());
        CALL_COUNTER_WITH_MOCK(GetVerticalGlyphVariantsMethod, HRESULT(uint32_t, UINT16 const*, UINT16*));
        CALL_COUNTER_WITH_MOCK(HasVerticalGlyphVariantsMethod, BOOL());

        CALL_COUNTER_WITH_MOCK(IsColorFontMethod, BOOL());
        CALL_COUNTER_WITH_MOCK(GetColorPaletteCountMethod, uint32_t());
        CALL_COUNTER_WITH_MOCK(GetPaletteEntryCountMethod, uint32_t());
        CALL_COUNTER_WITH_MOCK(GetPaletteEntriesMethod, HRESULT(uint32_t, uint32_t, uint32_t, DWRITE_COLOR_F*));
        CALL_COUNTER_WITH_MOCK(GetRecommendedRenderingModeMethod1, HRESULT(FLOAT, FLOAT, FLOAT, DWRITE_MATRIX const*, BOOL, DWRITE_OUTLINE_THRESHOLD, DWRITE_MEASURING_MODE, DWRITE_RENDERING_MODE*));
        CALL_COUNTER_WITH_MOCK(GetRecommendedRenderingModeMethod2, HRESULT(FLOAT, FLOAT, FLOAT, DWRITE_MATRIX const*, BOOL, DWRITE_OUTLINE_THRESHOLD, DWRITE_MEASURING_MODE, IDWriteRenderingParams*, DWRITE_RENDERING_MODE*, DWRITE_GRID_FIT_MODE*));

        CALL_COUNTER_WITH_MOCK(GetFontFaceReferenceMethod, HRESULT(IDWriteFontFaceReference**));
        CALL_COUNTER_WITH_MOCK(GetPanoseMethod, void(DWRITE_PANOSE*));
        CALL_COUNTER_WITH_MOCK(GetWeightMethod, DWRITE_FONT_WEIGHT());
        CALL_COUNTER_WITH_MOCK(GetStretchMethod, DWRITE_FONT_STRETCH());
        CALL_COUNTER_WITH_MOCK(GetStyleMethod, DWRITE_FONT_STYLE());
        CALL_COUNTER_WITH_MOCK(GetFamilyNamesMethod, HRESULT(IDWriteLocalizedStrings**));
        CALL_COUNTER_WITH_MOCK(GetFaceNamesMethod, HRESULT(IDWriteLocalizedStrings**));
        CALL_COUNTER_WITH_MOCK(GetInformationalStringsMethod, HRESULT(DWRITE_INFORMATIONAL_STRING_ID, IDWriteLocalizedStrings**, BOOL*));
        CALL_COUNTER_WITH_MOCK(HasCharacterMethod, BOOL(uint32_t));
        CALL_COUNTER_WITH_MOCK(IsCharacterLocalMethod, BOOL(uint32_t));
        CALL_COUNTER_WITH_MOCK(IsGlyphLocalMethod, BOOL(uint32_t));
        CALL_COUNTER_WITH_MOCK(AreCharactersLocalMethod, HRESULT(WCHAR const*, uint32_t, BOOL, BOOL*));
        CALL_COUNTER_WITH_MOCK(AreGlyphsLocalMethod, HRESULT(UINT16 const*, uint32_t, BOOL, BOOL*));
        CALL_COUNTER_WITH_MOCK(GetRecommendedRenderingModeMethod3, HRESULT(FLOAT, FLOAT, FLOAT, DWRITE_MATRIX const*, BOOL, DWRITE_OUTLINE_THRESHOLD, DWRITE_MEASURING_MODE, IDWriteRenderingParams*, DWRITE_RENDERING_MODE1*, DWRITE_GRID_FIT_MODE*));

        //
        // IDWriteFontFace
        //

        STDMETHOD_(DWRITE_FONT_FACE_TYPE, GetType)()
        {
            return GetTypeMethod.WasCalled();
        }

        IFACEMETHODIMP GetFiles(
            uint32_t* numberOfFiles,
            IDWriteFontFile** fontFiles)
        {
            return GetFilesMethod.WasCalled(numberOfFiles, fontFiles);
        }

        STDMETHOD_(uint32_t, GetIndex)()
        {
            return GetIndexMethod.WasCalled();
        }

        STDMETHOD_(DWRITE_FONT_SIMULATIONS, GetSimulations)()
        {
            return GetSimulationsMethod.WasCalled();
        }

        STDMETHOD_(BOOL, IsSymbolFont)()
        {
            return IsSymbolFontMethod.WasCalled();
        }

        STDMETHOD_(void, GetMetrics)(
            DWRITE_FONT_METRICS* fontFaceMetrics)
        {
             GetMetricsMethod0.WasCalled(fontFaceMetrics);
        }

        STDMETHOD_(UINT16, GetGlyphCount)()
        {
            return GetGlyphCountMethod.WasCalled();
        }

        IFACEMETHODIMP GetDesignGlyphMetrics(
            UINT16 const* glyphIndices,
            uint32_t glyphCount,
            DWRITE_GLYPH_METRICS* glyphMetrics,
            BOOL isSideways)
        {
            return GetDesignGlyphMetricsMethod.WasCalled(glyphIndices, glyphCount, glyphMetrics, isSideways);
        }

        IFACEMETHODIMP GetGlyphIndices(
            uint32_t const* codePoints,
            uint32_t codePointCount,
            UINT16* glyphIndices)
        {
            return GetGlyphIndicesMethod.WasCalled(codePoints, codePointCount, glyphIndices);
        }

        IFACEMETHODIMP TryGetFontTable(
            uint32_t openTypeTableTag,
            const void** tableData,
            uint32_t* tableSize,
            void** tableContext,
            BOOL* exists)
        {
            return TryGetFontTableMethod.WasCalled(openTypeTableTag, tableData, tableSize, tableContext, exists);
        }

        STDMETHOD_(void, ReleaseFontTable)(
            void* tableContext)
        {
            ReleaseFontTableMethod.WasCalled(tableContext);
        }

        IFACEMETHODIMP GetGlyphRunOutline(
            FLOAT emSize,
            UINT16 const* glyphIndices,
            FLOAT const* glyphAdvances,
            DWRITE_GLYPH_OFFSET const* glyphOffsets,
            uint32_t glyphCount,
            BOOL isSideways,
            BOOL isRightToLeft,
            IDWriteGeometrySink* geometrySink)
        {
            return GetGlyphRunOutlineMethod.WasCalled(emSize, glyphIndices, glyphAdvances, glyphOffsets, glyphCount, isSideways, isRightToLeft, geometrySink);
        }

        IFACEMETHODIMP GetRecommendedRenderingMode(
            FLOAT emSize,
            FLOAT pixelsPerDip,
            DWRITE_MEASURING_MODE measuringMode,
            IDWriteRenderingParams* renderingParams,
            DWRITE_RENDERING_MODE* renderingMode)
        {
            return GetRecommendedRenderingModeMethod0.WasCalled(emSize, pixelsPerDip, measuringMode, renderingParams, renderingMode);
        }

        IFACEMETHODIMP GetGdiCompatibleMetrics(
            FLOAT emSize,
            FLOAT pixelsPerDip,
            DWRITE_MATRIX const* transform,
            DWRITE_FONT_METRICS* fontFaceMetrics)
        {
            return GetGdiCompatibleMetricsMethod0.WasCalled(emSize, pixelsPerDip, transform, fontFaceMetrics);
        }

        IFACEMETHODIMP GetGdiCompatibleGlyphMetrics(
            FLOAT emSize,
            FLOAT pixelsPerDip,
            DWRITE_MATRIX const* transform,
            BOOL useGdiNatural,
            UINT16 const* glyphIndices,
            uint32_t glyphCount,
            DWRITE_GLYPH_METRICS* glyphMetrics,
            BOOL isSideways)
        {
            return GetGdiCompatibleGlyphMetricsMethod.WasCalled(emSize, pixelsPerDip, transform, useGdiNatural, glyphIndices, glyphCount, glyphMetrics, isSideways);
        }

        //
        // IDWriteFontFace1
        //

        IFACEMETHODIMP_(void) GetMetrics(
            DWRITE_FONT_METRICS1* fontMetrics)
        {
            GetMetricsMethod1.WasCalled(fontMetrics);
        }

        IFACEMETHODIMP GetGdiCompatibleMetrics(
            FLOAT emSize,
            FLOAT pixelsPerDip,
            DWRITE_MATRIX const* transform,
            DWRITE_FONT_METRICS1* fontMetrics
            )
        {
            return GetGdiCompatibleMetricsMethod1.WasCalled(emSize, pixelsPerDip, transform, fontMetrics);
        }

        IFACEMETHODIMP_(void) GetCaretMetrics(
            DWRITE_CARET_METRICS* caretMetrics
            )
        {
            return GetCaretMetricsMethod.WasCalled(caretMetrics);
        }

        IFACEMETHODIMP GetUnicodeRanges(
            uint32_t maxRangeCount,
            DWRITE_UNICODE_RANGE* unicodeRanges,
            uint32_t* actualRangeCount
            )
        {
            return GetUnicodeRangesMethod.WasCalled(maxRangeCount, unicodeRanges, actualRangeCount);
        }

        IFACEMETHODIMP_(BOOL) IsMonospacedFont()
        {
            return IsMonospacedFontMethod.WasCalled();
        }

        IFACEMETHODIMP GetDesignGlyphAdvances(
            uint32_t glyphCount,
            UINT16 const* glyphIndices,
            INT32* glyphAdvances,
            BOOL isSideways
            )
        {
            return GetDesignGlyphAdvancesMethod.WasCalled(glyphCount, glyphIndices, glyphAdvances, isSideways);
        }

        IFACEMETHODIMP GetGdiCompatibleGlyphAdvances(
            FLOAT emSize,
            FLOAT pixelsPerDip,
            DWRITE_MATRIX const* transform,
            BOOL useGdiNatural,
            BOOL isSideways,
            uint32_t glyphCount,
            UINT16 const* glyphIndices,
            INT32* glyphAdvances)
        {
            return GetGdiCompatibleGlyphAdvancesMethod.WasCalled(emSize, pixelsPerDip, transform, useGdiNatural, isSideways, glyphCount, glyphIndices, glyphAdvances);
        }

        IFACEMETHODIMP GetKerningPairAdjustments(
            uint32_t glyphCount,
            UINT16 const* glyphIndices,
            INT32* glyphAdvanceAdjustments)
        {
            return GetKerningPairAdjustmentsMethod.WasCalled(glyphCount, glyphIndices, glyphAdvanceAdjustments);
        }


        IFACEMETHODIMP_(BOOL) HasKerningPairs(void) override
        {
            return HasKerningPairsMethod.WasCalled();
        }

        IFACEMETHODIMP GetRecommendedRenderingMode(
            FLOAT fontEmSize,
            FLOAT dpiX,
            FLOAT dpiY,
            DWRITE_MATRIX const* transform,
            BOOL isSideways,
            DWRITE_OUTLINE_THRESHOLD outlineThreshold,
            DWRITE_MEASURING_MODE measuringMode,
            DWRITE_RENDERING_MODE* renderingMode)
        {
            return GetRecommendedRenderingModeMethod1.WasCalled(fontEmSize, dpiX, dpiY, transform, isSideways, outlineThreshold, measuringMode, renderingMode);
        }

        IFACEMETHODIMP GetVerticalGlyphVariants(
            uint32_t glyphCount,
            UINT16 const* nominalGlyphIndices,
            UINT16* verticalGlyphIndices) override
        {
            return GetVerticalGlyphVariantsMethod.WasCalled(glyphCount, nominalGlyphIndices, verticalGlyphIndices);
        }

        IFACEMETHODIMP_(BOOL) HasVerticalGlyphVariants() override
        {
            return HasVerticalGlyphVariantsMethod.WasCalled();
        }


        //
        // IDWriteFontFace2
        //

        IFACEMETHODIMP_(BOOL) IsColorFont()
        {
            return IsColorFontMethod.WasCalled();
        }

        IFACEMETHODIMP_(uint32_t) GetColorPaletteCount()
        {
            return GetColorPaletteCountMethod.WasCalled();
        }

        IFACEMETHODIMP_(uint32_t) GetPaletteEntryCount()
        {
            return GetPaletteEntryCountMethod.WasCalled();
        }

        IFACEMETHODIMP GetPaletteEntries(
            uint32_t colorPaletteIndex,
            uint32_t firstEntryIndex,
            uint32_t entryCount,
            DWRITE_COLOR_F* paletteEntries)
        {
            return GetPaletteEntriesMethod.WasCalled(colorPaletteIndex, firstEntryIndex, entryCount, paletteEntries);
        }

        IFACEMETHODIMP GetRecommendedRenderingMode(
            FLOAT fontEmSize,
            FLOAT dpiX,
            FLOAT dpiY,
            DWRITE_MATRIX const* transform,
            BOOL isSideways,
            DWRITE_OUTLINE_THRESHOLD outlineThreshold,
            DWRITE_MEASURING_MODE measuringMode,
            IDWriteRenderingParams* renderingParams,
            DWRITE_RENDERING_MODE* renderingMode,
            DWRITE_GRID_FIT_MODE* gridFitMode)
        {
            return GetRecommendedRenderingModeMethod2.WasCalled(fontEmSize, dpiX, dpiY, transform, isSideways, outlineThreshold, measuringMode, renderingParams, renderingMode, gridFitMode);
        }

        //
        // IDWriteFontFace3
        //
        IFACEMETHODIMP GetFontFaceReference(
            IDWriteFontFaceReference** fontFaceReference)
        {
            return GetFontFaceReferenceMethod.WasCalled(fontFaceReference);
        }

        IFACEMETHODIMP_(void) GetPanose(
            DWRITE_PANOSE* panose)
        {
            return GetPanoseMethod.WasCalled(panose);
        }

        IFACEMETHODIMP_(DWRITE_FONT_WEIGHT) GetWeight()
        {
            return GetWeightMethod.WasCalled();
        }

        IFACEMETHODIMP_(DWRITE_FONT_STRETCH) GetStretch()
        {
            return GetStretchMethod.WasCalled();
        }

        IFACEMETHODIMP_(DWRITE_FONT_STYLE) GetStyle()
        {
            return GetStyleMethod.WasCalled();
        }

        IFACEMETHODIMP GetFamilyNames(
            IDWriteLocalizedStrings** names)
        {
            return GetFamilyNamesMethod.WasCalled(names);
        }

        IFACEMETHODIMP GetFaceNames(
            IDWriteLocalizedStrings** names
            )
        {
            return GetFaceNamesMethod.WasCalled(names);
        }

        IFACEMETHODIMP GetInformationalStrings(
            DWRITE_INFORMATIONAL_STRING_ID informationalStringID,
            IDWriteLocalizedStrings** informationalStrings,
            BOOL* exists)
        {
            return GetInformationalStringsMethod.WasCalled(informationalStringID, informationalStrings, exists);
        }

        IFACEMETHODIMP_(BOOL) HasCharacter(
            uint32_t unicodeValue)
        {
            return HasCharacterMethod.WasCalled(unicodeValue);
        }

        IFACEMETHODIMP GetRecommendedRenderingMode(
            FLOAT fontEmSize,
            FLOAT dpiX,
            FLOAT dpiY,
            DWRITE_MATRIX const* transform,
            BOOL isSideways,
            DWRITE_OUTLINE_THRESHOLD outlineThreshold,
            DWRITE_MEASURING_MODE measuringMode,
            IDWriteRenderingParams* renderingParams,
            DWRITE_RENDERING_MODE1* renderingMode,
            DWRITE_GRID_FIT_MODE* gridFitMode)
        {
            return GetRecommendedRenderingModeMethod3.WasCalled(fontEmSize, dpiX, dpiY, transform, isSideways, outlineThreshold, measuringMode, renderingParams, renderingMode, gridFitMode);
        }

        IFACEMETHODIMP_(BOOL) IsCharacterLocal(
            uint32_t unicodeValue)
        {
            return IsCharacterLocalMethod.WasCalled(unicodeValue);
        }

        IFACEMETHODIMP_(BOOL) IsGlyphLocal(
            UINT16 glyphId)
        {
            return IsGlyphLocalMethod.WasCalled(glyphId);
        }

        IFACEMETHODIMP AreCharactersLocal(
            WCHAR const* characters,
            uint32_t characterCount,
            BOOL enqueueIfNotLocal,
            BOOL* isLocal)
        {
            return AreCharactersLocalMethod.WasCalled(characters, characterCount, enqueueIfNotLocal, isLocal);
        }

        IFACEMETHODIMP AreGlyphsLocal(
            UINT16 const* glyphIndices,
            uint32_t glyphCount,
            BOOL enqueueIfNotLocal,
            BOOL* isLocal)
        {
            return AreGlyphsLocalMethod.WasCalled(glyphIndices, glyphCount, enqueueIfNotLocal, isLocal);
        }
    };
}
