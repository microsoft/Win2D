// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace canvas
{
    class MockDWriteTextAnalyzer : public RuntimeClass<
        RuntimeClassFlags<ClassicCom>,
        ChainInterfaces<IDWriteTextAnalyzer2, IDWriteTextAnalyzer1, IDWriteTextAnalyzer>>
    {
    public:
        MOCK_METHOD4(AnalyzeScript, HRESULT(IDWriteTextAnalysisSource*, UINT32, UINT32, IDWriteTextAnalysisSink*));
        MOCK_METHOD4(AnalyzeBidi, HRESULT(IDWriteTextAnalysisSource*, UINT32, UINT32, IDWriteTextAnalysisSink*));
        MOCK_METHOD4(AnalyzeNumberSubstitution, HRESULT(IDWriteTextAnalysisSource*, UINT32, UINT32, IDWriteTextAnalysisSink*));
        MOCK_METHOD4(AnalyzeLineBreakpoints, HRESULT(IDWriteTextAnalysisSource*, UINT32, UINT32, IDWriteTextAnalysisSink*));
        MOCK_METHOD17(GetGlyphs, HRESULT(const WCHAR *, UINT32, IDWriteFontFace *, BOOL, BOOL, const DWRITE_SCRIPT_ANALYSIS *, const WCHAR *, IDWriteNumberSubstitution *, const DWRITE_TYPOGRAPHIC_FEATURES **, const UINT32 *, UINT32, UINT32, UINT16 *, DWRITE_SHAPING_TEXT_PROPERTIES *, UINT16 *, DWRITE_SHAPING_GLYPH_PROPERTIES *, UINT32 *));
        MOCK_METHOD18(GetGlyphPlacements, HRESULT(WCHAR const*,UINT16 const*,DWRITE_SHAPING_TEXT_PROPERTIES*,UINT32,UINT16 const*,DWRITE_SHAPING_GLYPH_PROPERTIES const*,UINT32,IDWriteFontFace *,FLOAT,BOOL,BOOL,DWRITE_SCRIPT_ANALYSIS const*,WCHAR const*,DWRITE_TYPOGRAPHIC_FEATURES const**,UINT32 const*,UINT32,FLOAT*,DWRITE_GLYPH_OFFSET* glyphOffsets));
        MOCK_METHOD21(GetGdiCompatibleGlyphPlacements, HRESULT(const WCHAR *, const UINT16 *, DWRITE_SHAPING_TEXT_PROPERTIES *, UINT32, const UINT16 *, const DWRITE_SHAPING_GLYPH_PROPERTIES *, UINT32, IDWriteFontFace *, FLOAT, FLOAT, const DWRITE_MATRIX *, BOOL, BOOL, BOOL, const DWRITE_SCRIPT_ANALYSIS *, const WCHAR *, const DWRITE_TYPOGRAPHIC_FEATURES **, const UINT32 *, UINT32, FLOAT *, DWRITE_GLYPH_OFFSET *));

        MOCK_METHOD11(ApplyCharacterSpacing, HRESULT(FLOAT, FLOAT, FLOAT, UINT32, UINT32, UINT16 const*, FLOAT const*, DWRITE_GLYPH_OFFSET const*, DWRITE_SHAPING_GLYPH_PROPERTIES const*, FLOAT*, DWRITE_GLYPH_OFFSET*));
        MOCK_METHOD8(GetBaseline, HRESULT(IDWriteFontFace*, DWRITE_BASELINE, BOOL, BOOL, DWRITE_SCRIPT_ANALYSIS, WCHAR const*, INT32*, BOOL*));
        MOCK_METHOD4(AnalyzeVerticalGlyphOrientation, HRESULT(IDWriteTextAnalysisSource1*, UINT32, UINT32, IDWriteTextAnalysisSink1*));
        MOCK_METHOD2(GetScriptProperties, HRESULT(DWRITE_SCRIPT_ANALYSIS, DWRITE_SCRIPT_PROPERTIES*));
        MOCK_METHOD6(GetTextComplexity,HRESULT(WCHAR const*,UINT32,IDWriteFontFace*,BOOL*,UINT32*,UINT16*));
        MOCK_METHOD9(GetJustificationOpportunities,HRESULT(IDWriteFontFace*,FLOAT,DWRITE_SCRIPT_ANALYSIS,UINT32,UINT32,WCHAR const*,UINT16 const*,DWRITE_SHAPING_GLYPH_PROPERTIES const*,DWRITE_JUSTIFICATION_OPPORTUNITY*));
        MOCK_METHOD7(JustifyGlyphAdvances,HRESULT(FLOAT,UINT32,DWRITE_JUSTIFICATION_OPPORTUNITY const*,FLOAT const*, DWRITE_GLYPH_OFFSET const*,FLOAT*,DWRITE_GLYPH_OFFSET*));
        MOCK_METHOD17(GetJustifiedGlyphs, HRESULT(IDWriteFontFace *, FLOAT, DWRITE_SCRIPT_ANALYSIS, UINT32, UINT32, UINT32, const UINT16 *, const UINT16 *, const FLOAT *, const FLOAT *, const DWRITE_GLYPH_OFFSET *, const DWRITE_SHAPING_GLYPH_PROPERTIES *, UINT32 *, UINT16 *, UINT16 *, FLOAT *, DWRITE_GLYPH_OFFSET *));

        MOCK_METHOD5(GetGlyphOrientationTransform, HRESULT(DWRITE_GLYPH_ORIENTATION_ANGLE, BOOL, FLOAT, FLOAT, DWRITE_MATRIX*));
        MOCK_METHOD6(GetTypographicFeatures, HRESULT(IDWriteFontFace*, DWRITE_SCRIPT_ANALYSIS, WCHAR const*, UINT32, UINT32*, DWRITE_FONT_FEATURE_TAG*));
        MOCK_METHOD7(CheckTypographicFeature, HRESULT(IDWriteFontFace*, DWRITE_SCRIPT_ANALYSIS, WCHAR const*, DWRITE_FONT_FEATURE_TAG, UINT32, UINT16 const*, UINT8*));

        STDMETHODIMP GetGlyphOrientationTransform(
            DWRITE_GLYPH_ORIENTATION_ANGLE,
            BOOL,
            DWRITE_MATRIX*)
        {
            Assert::Fail(L"Unexpected call to GetGlyphOrientationTransform");
            return E_NOTIMPL;
        }
    };
}
