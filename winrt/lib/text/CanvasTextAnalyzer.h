// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Text
{
    class DWriteTextAnalysisSource : public RuntimeClass<RuntimeClassFlags<ClassicCom>, IDWriteTextAnalysisSource1>,
        private LifespanTracker<DWriteTextAnalysisSource>
    {
        WinString m_text;
        uint32_t m_textLength;

        WinString m_localeName;
        DWRITE_READING_DIRECTION m_dwriteReadingDirection;
        ComPtr<ICanvasTextAnalyzerOptions> m_analyzerOptions;
        std::vector<WinString> m_localeNames;
        ComPtr<IDWriteNumberSubstitution> m_defaultNumberSubstitution;
        DWRITE_VERTICAL_GLYPH_ORIENTATION m_defaultVerticalGlyphOrientation;
        uint8_t m_defaultBidiLevel;

    public:
        DWriteTextAnalysisSource(
            WinString text,
            uint32_t textLength,
            CanvasTextDirection textDirection,
            ComPtr<ICanvasTextAnalyzerOptions> const& analyzerOptions,
            ComPtr<ICanvasNumberSubstitution> defaultNumberSubstitution,
            CanvasVerticalGlyphOrientation defaultVerticalGlyphOrientation,
            uint32_t defaultBidiLevel);

        void SetLocaleName(WinString localeName);

        IFACEMETHODIMP GetTextAtPosition(
            uint32_t textPosition,
            WCHAR const** textString,
            uint32_t* textLengthRemaining) override;

        IFACEMETHODIMP GetTextBeforePosition(
            uint32_t textPosition,
            WCHAR const** textString,
            uint32_t* textLengthRemaining) override;

        IFACEMETHODIMP_(DWRITE_READING_DIRECTION) GetParagraphReadingDirection() override;

        IFACEMETHODIMP GetLocaleName(
            uint32_t textPosition,
            uint32_t* characterCountWithSameFormatting,
            WCHAR const** localeName) override;

        IFACEMETHODIMP GetNumberSubstitution(
            uint32_t textPosition,
            uint32_t* characterCountWithSameFormatting,
            IDWriteNumberSubstitution** numberSubstitution) override;

        IFACEMETHODIMP GetVerticalGlyphOrientation(
            uint32_t textPosition,
            uint32_t* characterCountWithSameFormatting,
            DWRITE_VERTICAL_GLYPH_ORIENTATION* verticalGlyphOrientation,
            uint8_t* bidiLevel) override;

    private:
        uint32_t GetCharacterCountForRemainderOfText(uint32_t textPosition);
    };
    
    class DWriteTextAnalysisSink : public RuntimeClass<RuntimeClassFlags<ClassicCom>, IDWriteTextAnalysisSink1>,
        private LifespanTracker<DWriteTextAnalysisSink>
    {
        uint32_t m_textLength;

        ComPtr<Vector<IKeyValuePair<CanvasCharacterRange, CanvasAnalyzedBidi>*>> m_analyzedBidi;
        ComArray<CanvasAnalyzedBreakpoint> m_analyzedLineBreakpoints;
        ComPtr<Vector<IKeyValuePair<CanvasCharacterRange, CanvasNumberSubstitution*>*>> m_analyzedNumberSubstitution;
        ComPtr<Vector<IKeyValuePair<CanvasCharacterRange, CanvasAnalyzedScript>*>> m_analyzedScript;
        ComPtr<Vector<IKeyValuePair<CanvasCharacterRange, CanvasAnalyzedGlyphOrientation>*>> m_analyzedGlyphOrientation;

    public:
        DWriteTextAnalysisSink(uint32_t textLength) : m_textLength(textLength) {}

        STDMETHOD(SetBidiLevel)(
            uint32_t textPosition,
            uint32_t textLength,
            uint8_t explicitLevel,
            uint8_t resolvedLevel) override;

        STDMETHOD(SetLineBreakpoints)(
            uint32_t textPosition,
            uint32_t textLength,
            DWRITE_LINE_BREAKPOINT const* dwriteLineBreakpoint) override;

        STDMETHOD(SetNumberSubstitution)(
            uint32_t textPosition,
            uint32_t textLength,
            IDWriteNumberSubstitution* dwriteNumberSubstitution) override;

        STDMETHOD(SetScriptAnalysis)(
            uint32_t textPosition,
            uint32_t textLength,
            DWRITE_SCRIPT_ANALYSIS const* scriptAnalysis) override;

        IFACEMETHODIMP SetGlyphOrientation(
            uint32_t textPosition,
            uint32_t textLength,
            DWRITE_GLYPH_ORIENTATION_ANGLE dwriteGlyphOrientationAngle,
            uint8_t adjustedBidiLevel,
            BOOL isSideways,
            BOOL isRightToLeft) override;

        ComPtr<Vector<IKeyValuePair<CanvasCharacterRange, CanvasAnalyzedScript>*>> GetAnalyzedScript();

        ComPtr<Vector<IKeyValuePair<CanvasCharacterRange, CanvasAnalyzedBidi>*>> GetAnalyzedBidi();

        ComArray<CanvasAnalyzedBreakpoint> GetAnalyzedLineBreakpoints();

        ComPtr<Vector<IKeyValuePair<CanvasCharacterRange, CanvasNumberSubstitution*>*>> GetAnalyzedNumberSubstitution();

        ComPtr<Vector<IKeyValuePair<CanvasCharacterRange, CanvasAnalyzedGlyphOrientation>*>> GetAnalyzedGlyphOrientation();

    private:

        void EnsureAnalyzedBidi();

        void EnsureAnalyzedLineBreakpoints();

        void EnsureAnalyzedNumberSubstitution();

        void EnsureAnalyzedScript();

        void EnsureAnalyzedGlyphOrientation();

    };

    class CanvasTextAnalyzer : public RuntimeClass<
        RuntimeClassFlags<WinRtClassicComMix>,
        ICanvasTextAnalyzer>,
        private LifespanTracker<CanvasTextAnalyzer>
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_Text_CanvasTextAnalyzer, BaseTrust);

        WinString m_text;
        CanvasTextDirection m_textDirection;
        ComPtr<ICanvasTextAnalyzerOptions> m_source;
        ComPtr<ICanvasNumberSubstitution> m_defaultNumberSubstitution;
        CanvasVerticalGlyphOrientation m_defaultVerticalGlyphOrientation;
        uint32_t m_defaultBidiLevel;

        std::shared_ptr<CustomFontManager> m_customFontManager;

        ComPtr<DWriteTextAnalysisSource> m_dwriteTextAnalysisSource;
        ComPtr<DWriteTextAnalysisSink> m_dwriteTextAnalysisSink;

    public:
        CanvasTextAnalyzer(
            HSTRING text,
            CanvasTextDirection textDirection,
            ICanvasTextAnalyzerOptions* source);

        CanvasTextAnalyzer(
            HSTRING text,
            CanvasTextDirection textDirection,
            ICanvasNumberSubstitution* numberSubstitution,
            CanvasVerticalGlyphOrientation verticalGlyphOrientation,
            uint32_t bidiLevel);

        IFACEMETHOD(ChooseFontsUsingSystemFontSet)(
            ICanvasTextFormat* textFormat,
            IVectorView<IKeyValuePair<CanvasCharacterRange, CanvasScaledFont*>*>** result);

        IFACEMETHOD(ChooseFonts)(
            ICanvasTextFormat* textFormat,
            ICanvasFontSet* fontSet,
            IVectorView<IKeyValuePair<CanvasCharacterRange, CanvasScaledFont*>*>** result);

        IFACEMETHOD(AnalyzeBidi)(
            IVectorView<IKeyValuePair<CanvasCharacterRange, CanvasAnalyzedBidi>*>** values) override;

        IFACEMETHOD(AnalyzeBidiWithLocale)(
            HSTRING locale,
            IVectorView<IKeyValuePair<CanvasCharacterRange, CanvasAnalyzedBidi>*>** values) override;

        IFACEMETHOD(AnalyzeBreakpoints)(
            uint32_t* valueCount,
            CanvasAnalyzedBreakpoint** valueElements) override;

        IFACEMETHOD(AnalyzeBreakpointsWithLocale)(
            HSTRING locale,
            uint32_t* valueCount,
            CanvasAnalyzedBreakpoint** valueElements) override;

        IFACEMETHOD(AnalyzeNumberSubstitutions)(
            IVectorView<IKeyValuePair<CanvasCharacterRange, CanvasNumberSubstitution*>*>** values) override;

        IFACEMETHOD(AnalyzeScript)(
            IVectorView<IKeyValuePair<CanvasCharacterRange, CanvasAnalyzedScript>*>** values) override;

        IFACEMETHOD(AnalyzeScriptWithLocale)(
            HSTRING locale,
            IVectorView<IKeyValuePair<CanvasCharacterRange, CanvasAnalyzedScript>*>** values) override;

        IFACEMETHOD(AnalyzeGlyphOrientations)(
            IVectorView<IKeyValuePair<CanvasCharacterRange, CanvasAnalyzedGlyphOrientation>*>** values) override;

        IFACEMETHOD(AnalyzeGlyphOrientationsWithLocale)(
            HSTRING locale,
            IVectorView<IKeyValuePair<CanvasCharacterRange, CanvasAnalyzedGlyphOrientation>*>** values) override;

        IFACEMETHOD(GetScriptProperties)(
            CanvasAnalyzedScript analyzedScript,
            CanvasScriptProperties* scriptProperties) override;

        IFACEMETHOD(GetGlyphs)(
            CanvasCharacterRange characterRange,
            ICanvasFontFace* fontFace,
            float fontSize,
            boolean isSideways,
            boolean isRightToLeft,
            CanvasAnalyzedScript script,
            uint32_t* valueCount,
            CanvasGlyph** valueElements) override;

        IFACEMETHOD(GetGlyphsWithAllOptions)(
            CanvasCharacterRange characterRange,
            ICanvasFontFace* fontFace,
            float fontSize,
            boolean isSideways,
            boolean isRightToLeft,
            CanvasAnalyzedScript script,
            HSTRING locale,
            ICanvasNumberSubstitution* numberSubstitution,
            IVectorView<IKeyValuePair<CanvasCharacterRange, CanvasTypography*>*>* typographyRanges,
            uint32_t* clusterMapIndicesCount,
            int** clusterMapIndicesElements,
            uint32_t* isShapedAloneGlyphsCount,
            boolean** isShapedAloneGlyphsElements,
            uint32_t* glyphShapingResultsCount,
            CanvasGlyphShaping** glyphShapingResultsElements,
            uint32_t* valueCount,
            CanvasGlyph** valueElements) override;

    private:
        void CreateTextAnalysisSourceAndSink();

    };


    //
    // CanvasTextTextAnalyzerFactory
    //

    class CanvasTextAnalyzerFactory
        : public ActivationFactory<ICanvasTextAnalyzerFactory>
        , private LifespanTracker<CanvasTextAnalyzerFactory>
    {
        InspectableClassStatic(RuntimeClass_Microsoft_Graphics_Canvas_Text_CanvasTextAnalyzer, BaseTrust);

    public:

        IFACEMETHOD(Create)(
            HSTRING text,
            CanvasTextDirection textDirection,
            ICanvasTextAnalyzer** textAnalyzer);

        IFACEMETHOD(CreateWithNumberSubstitutionAndVerticalGlyphOrientationAndBidiLevel)(
            HSTRING text,
            CanvasTextDirection textDirection,
            ICanvasNumberSubstitution* numberSubstitution,
            CanvasVerticalGlyphOrientation verticalGlyphOrientation,
            uint32_t bidiLevel,
            ICanvasTextAnalyzer** textAnalyzer);

        IFACEMETHOD(CreateWithOptions)(
            HSTRING text,
            CanvasTextDirection textDirection,
            ICanvasTextAnalyzerOptions* source,
            ICanvasTextAnalyzer** textAnalyzer);
    };
    
}}}}}
