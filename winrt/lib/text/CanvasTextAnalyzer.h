// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Text
{
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

        IFACEMETHOD(AnalyzeScript)(
            IVectorView<IKeyValuePair<CanvasCharacterRange, CanvasAnalyzedScript>*>** values) override;

        IFACEMETHOD(AnalyzeScriptWithLocale)(
            HSTRING locale,
            IVectorView<IKeyValuePair<CanvasCharacterRange, CanvasAnalyzedScript>*>** values) override;

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
        ComPtr<IDWriteTextAnalysisSource> CreateTextAnalysisSource(
            WinString localeNameString);

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
