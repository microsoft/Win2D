// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

#include "CanvasTextAnalyzer.h"
#include "CanvasFontSet.h"
#include "CanvasScaledFont.h"
#include "CanvasFontFace.h"
#include "CanvasTypography.h"

using namespace ABI::Microsoft::Graphics::Canvas;
using namespace ABI::Microsoft::Graphics::Canvas::Text;

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
        WinString localeName,
        CanvasTextDirection textDirection,
        ComPtr<ICanvasTextAnalyzerOptions> const& analyzerOptions,
        ComPtr<ICanvasNumberSubstitution> defaultNumberSubstitution,
        CanvasVerticalGlyphOrientation defaultVerticalGlyphOrientation,
        uint32_t defaultBidiLevel)
        : m_text(text)
        , m_textLength(textLength)
        , m_localeName(localeName)
        , m_dwriteReadingDirection(DWriteToCanvasTextDirection::Lookup(textDirection)->ReadingDirection)
        , m_analyzerOptions(analyzerOptions)
        , m_defaultVerticalGlyphOrientation(ToVerticalGlyphOrientation(defaultVerticalGlyphOrientation))
        , m_defaultBidiLevel(static_cast<uint8_t>(defaultBidiLevel))
    {

        if (defaultNumberSubstitution)
        {
            m_defaultNumberSubstitution = GetWrappedResource<IDWriteNumberSubstitution>(defaultNumberSubstitution);
        }
    }

    IFACEMETHODIMP GetTextAtPosition(
        uint32_t textPosition,
        WCHAR const** textString,
        uint32_t* textLengthRemaining)
    {
        if (textPosition >= m_textLength)
        {
            *textString = nullptr;
            *textLengthRemaining = 0;
        }
        else
        {
            *textString = static_cast<wchar_t const*>(m_text) + textPosition;
            *textLengthRemaining = m_textLength - textPosition;
        }
        return S_OK;
    }

    IFACEMETHODIMP GetTextBeforePosition(
        uint32_t textPosition,
        WCHAR const** textString,
        uint32_t* textLengthRemaining)
    {
        if (textPosition == 0 || textPosition > m_textLength)
        {
            *textString = nullptr;
            *textLengthRemaining = 0;
        }
        else
        {
            *textString = WindowsGetStringRawBuffer(m_text, nullptr);
            *textLengthRemaining = textPosition;
        }
        return S_OK;
    }

    IFACEMETHODIMP_(DWRITE_READING_DIRECTION) GetParagraphReadingDirection()
    {
        return m_dwriteReadingDirection;
    }

    IFACEMETHODIMP GetLocaleName(
        uint32_t textPosition,
        uint32_t* characterCountWithSameFormatting,
        WCHAR const** localeName)
    {
        return ExceptionBoundary(
            [&]
            {
                if (m_analyzerOptions)
                {
                    WinString localeString;
                    int characterCount;
                    ThrowIfFailed(m_analyzerOptions->GetLocaleName(textPosition, &characterCount, localeString.GetAddressOf()));

                    m_localeNames.push_back(localeString);

                    *localeName = static_cast<wchar_t const*>(m_localeNames.back());

                    *characterCountWithSameFormatting = characterCount;
                }
                else
                {
                    *localeName = static_cast<wchar_t const*>(m_localeName);

                    *characterCountWithSameFormatting = GetCharacterCountForRemainderOfText(textPosition);
                }
            });
    }

    IFACEMETHODIMP GetNumberSubstitution(
        uint32_t textPosition,
        uint32_t* characterCountWithSameFormatting,
        IDWriteNumberSubstitution** numberSubstitution)
    {    
        return ExceptionBoundary(
            [&]
            {
                if (m_analyzerOptions)
                {
                    ComPtr<ICanvasNumberSubstitution> substitution;
                    int characterCount;
                    ThrowIfFailed(m_analyzerOptions->GetNumberSubstitution(textPosition, &characterCount, &substitution));

                    ComPtr<IDWriteNumberSubstitution> dwriteNumberSubstitution;
                    if (substitution)
                        dwriteNumberSubstitution = GetWrappedResource<IDWriteNumberSubstitution>(substitution);

                    dwriteNumberSubstitution.CopyTo(numberSubstitution);

                    *characterCountWithSameFormatting = characterCount;
                }
                else
                {
                    m_defaultNumberSubstitution.CopyTo(numberSubstitution);

                    *characterCountWithSameFormatting = GetCharacterCountForRemainderOfText(textPosition);
                }
            });
    }

    IFACEMETHODIMP GetVerticalGlyphOrientation(
        uint32_t textPosition,
        uint32_t* characterCountWithSameFormatting,
        DWRITE_VERTICAL_GLYPH_ORIENTATION* verticalGlyphOrientation,
        uint8_t* bidiLevel)
    {
        return ExceptionBoundary(
            [&]
            {
                if (m_analyzerOptions)
                {
                    CanvasVerticalGlyphOrientation glyphOrientation;
                    int glyphOrientationCharacterCount;
                    ThrowIfFailed(m_analyzerOptions->GetVerticalGlyphOrientation(textPosition, &glyphOrientationCharacterCount, &glyphOrientation));

                    *verticalGlyphOrientation = ToVerticalGlyphOrientation(glyphOrientation);

                    uint32_t bidi;
                    int bidiCharacterCount;
                    ThrowIfFailed(m_analyzerOptions->GetBidiLevel(textPosition, &bidiCharacterCount, &bidi));

                    if (bidi > UINT8_MAX)
                    {
                        ThrowHR(E_INVALIDARG);
                    }

                    *bidiLevel = static_cast<uint8_t>(bidi);

                    *characterCountWithSameFormatting = std::min(glyphOrientationCharacterCount, bidiCharacterCount);

                }
                else
                {
                    *verticalGlyphOrientation = m_defaultVerticalGlyphOrientation;

                    *bidiLevel = m_defaultBidiLevel;

                    *characterCountWithSameFormatting = GetCharacterCountForRemainderOfText(textPosition);
                }
            });
    }

private:
    uint32_t GetCharacterCountForRemainderOfText(uint32_t textPosition)
    {
        //
        // Returned as a character count when we have uniform formatting.
        //
        return textPosition < m_textLength ? m_textLength - textPosition : 0;
    }
};

template<typename ValueType, typename ValueInterface=ValueType>
static ComPtr<IKeyValuePair<CanvasCharacterRange, ValueType>> MakeCharacterRangeKeyValue(int textPosition, int textLength, ValueType value)
{
    typedef KeyValuePair<CanvasCharacterRange, ValueType, DefaultMapTraits<CanvasCharacterRange, ValueInterface>> KeyValuePairImplementation;

    auto newPair = Make<KeyValuePairImplementation>(CanvasCharacterRange{ textPosition, textLength }, value);
    CheckMakeResult(newPair);

    return newPair;
}

static CanvasAnalyzedScript ToCanvasAnalyzedScript(DWRITE_SCRIPT_ANALYSIS const* scriptAnalysis)
{
    CanvasAnalyzedScript analyzedScript{};

    analyzedScript.ScriptIdentifier = static_cast<int>(scriptAnalysis->script);
    analyzedScript.Shape = ToCanvasScriptShape(scriptAnalysis->shapes);

    return analyzedScript;
}

static DWRITE_SCRIPT_ANALYSIS ToDWriteScriptAnalysis(CanvasAnalyzedScript const& analyzedScript)
{
    DWRITE_SCRIPT_ANALYSIS scriptAnalysis{};

    if (analyzedScript.ScriptIdentifier < 0 || analyzedScript.ScriptIdentifier > UINT16_MAX)
    {
        ThrowHR(E_INVALIDARG);
    }
    scriptAnalysis.script = static_cast<uint16_t>(analyzedScript.ScriptIdentifier);
    scriptAnalysis.shapes = ToDWriteScriptShapes(analyzedScript.Shape);

    return scriptAnalysis;
}

class DWriteTextAnalysisSink : public RuntimeClass<RuntimeClassFlags<ClassicCom>, IDWriteTextAnalysisSink1>,
    private LifespanTracker<DWriteTextAnalysisSink>
{
    ComPtr<Vector<IKeyValuePair<CanvasCharacterRange, CanvasAnalyzedScript>*>> m_analyzedScript;

public:

    STDMETHOD(SetScriptAnalysis)(
        uint32_t textPosition,
        uint32_t textLength,
        DWRITE_SCRIPT_ANALYSIS const* scriptAnalysis) override
    {
        return ExceptionBoundary(
            [&]
            {
                EnsureAnalyzedScript();

                auto analyzedScript = ToCanvasAnalyzedScript(scriptAnalysis);

                auto newPair = MakeCharacterRangeKeyValue(textPosition, textLength, analyzedScript);

                ThrowIfFailed(m_analyzedScript->Append(newPair.Get()));
            });
    }

    STDMETHOD(SetLineBreakpoints)(
        uint32_t,
        uint32_t,
        DWRITE_LINE_BREAKPOINT const*) override
    {
        return E_UNEXPECTED; // TODO: #6142 expose analysis results other than script.
    }

    STDMETHOD(SetBidiLevel)(
        uint32_t,
        uint32_t,
        UINT8,
        UINT8) override
    {
        return E_UNEXPECTED;
    }

    STDMETHOD(SetNumberSubstitution)(
        uint32_t,
        uint32_t,
        IDWriteNumberSubstitution*) override
    {
        return E_UNEXPECTED;
    }

    IFACEMETHODIMP SetGlyphOrientation(
        uint32_t,
        uint32_t,
        DWRITE_GLYPH_ORIENTATION_ANGLE,
        UINT8,
        BOOL,
        BOOL) override
    {
        return E_UNEXPECTED;
    }

    ComPtr<Vector<IKeyValuePair<CanvasCharacterRange, CanvasAnalyzedScript>*>> const& GetAnalyzedScript()
    {
        EnsureAnalyzedScript();
        return m_analyzedScript;
    }

private:

    void EnsureAnalyzedScript()
    {
        if (!m_analyzedScript)
        {
            m_analyzedScript = Make<Vector<IKeyValuePair<CanvasCharacterRange, CanvasAnalyzedScript>*>>();
            CheckMakeResult(m_analyzedScript);
        }
    }

};

CanvasTextAnalyzer::CanvasTextAnalyzer(
    HSTRING text,
    CanvasTextDirection textDirection,
    ICanvasTextAnalyzerOptions* source)
    : m_text(text)
    , m_textDirection(textDirection)
    , m_source(source)
    , m_defaultVerticalGlyphOrientation(CanvasVerticalGlyphOrientation::Default)
    , m_defaultBidiLevel(0)
{
}


CanvasTextAnalyzer::CanvasTextAnalyzer(
    HSTRING text,
    CanvasTextDirection textDirection,
    ICanvasNumberSubstitution* numberSubstitution,
    CanvasVerticalGlyphOrientation verticalGlyphOrientation,
    uint32_t bidiLevel)
    : m_text(text)
    , m_textDirection(textDirection)
    , m_defaultNumberSubstitution(numberSubstitution)
    , m_defaultVerticalGlyphOrientation(verticalGlyphOrientation)
{
    if (bidiLevel > UINT8_MAX)
        ThrowHR(E_INVALIDARG);

    m_defaultBidiLevel = bidiLevel;
}


IFACEMETHODIMP CanvasTextAnalyzer::ChooseFontsUsingSystemFontSet(
    ICanvasTextFormat* textFormat,
    IVectorView<IKeyValuePair<CanvasCharacterRange, CanvasScaledFont*>*>** result)
{
    return ChooseFonts(textFormat, nullptr, result);
}

ComPtr<IDWriteTextAnalysisSource> CanvasTextAnalyzer::CreateTextAnalysisSource(
    WinString localeNameString)
{
    uint32_t textLength;
    WindowsGetStringRawBuffer(m_text, &textLength);

    ComPtr<IDWriteTextAnalysisSource> dwriteTextAnalysisSource = Make<DWriteTextAnalysisSource>(
        m_text,
        textLength,
        localeNameString,
        m_textDirection,
        m_source,
        m_defaultNumberSubstitution,
        m_defaultVerticalGlyphOrientation,
        m_defaultBidiLevel);
    CheckMakeResult(dwriteTextAnalysisSource);

    return dwriteTextAnalysisSource;
}

IFACEMETHODIMP CanvasTextAnalyzer::ChooseFonts(
    ICanvasTextFormat* textFormat,
    ICanvasFontSet* requestedFontSet,
    IVectorView<IKeyValuePair<CanvasCharacterRange, CanvasScaledFont*>*>** result)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(textFormat);
            CheckAndClearOutPointer(result);

            auto dwriteTextFormat = GetWrappedResource<IDWriteTextFormat>(textFormat);

            WinString localeNameString = GetLocaleName(dwriteTextFormat.Get());

            uint32_t textLength;
            WindowsGetStringRawBuffer(m_text, &textLength);

            auto dwriteTextAnalysisSource = CreateTextAnalysisSource(localeNameString);

            ComPtr<IDWriteFontCollection> dwriteFontCollection;

            if (requestedFontSet)
            {
#if WINVER > _WIN32_WINNT_WINBLUE
                auto dwriteFontSet = GetWrappedResource<IDWriteFontSet>(requestedFontSet);
                auto factory = As<IDWriteFactory3>(CustomFontManager::GetInstance()->GetSharedFactory());
                ComPtr<IDWriteFontCollection1> dwriteFontCollection1;
                ThrowIfFailed(factory->CreateFontCollectionFromFontSet(dwriteFontSet.Get(), &dwriteFontCollection1));

                dwriteFontCollection = As<IDWriteFontCollection>(dwriteFontCollection1.Get());
#else
                dwriteFontCollection = GetWrappedResource<IDWriteFontCollection>(requestedFontSet);
#endif
            }
            else
            {
                auto factory = CustomFontManager::GetInstance()->GetSharedFactory();
                ThrowIfFailed(factory->GetSystemFontCollection(&dwriteFontCollection));
            }

            std::wstring familyNameString;
            familyNameString.resize(dwriteTextFormat->GetFontFamilyNameLength() + 1);
            ThrowIfFailed(dwriteTextFormat->GetFontFamilyName(&familyNameString[0], static_cast<uint32_t>(familyNameString.size())));

            uint32_t characterIndex = 0;
            uint32_t charactersLeft = textLength;

            uint32_t mappedLength;
            ComPtr<IDWriteFont> mappedFont;
            float scaleFactor;

            auto vector = Make<Vector<IKeyValuePair<CanvasCharacterRange, CanvasScaledFont*>*>>();

            while (charactersLeft > 0)
            {
                ThrowIfFailed(CustomFontManager::GetInstance()->GetSystemFontFallback()->MapCharacters(
                    dwriteTextAnalysisSource.Get(),
                    characterIndex,
                    charactersLeft,
                    dwriteFontCollection.Get(),
                    &familyNameString[0],
                    dwriteTextFormat->GetFontWeight(),
                    dwriteTextFormat->GetFontStyle(),
                    dwriteTextFormat->GetFontStretch(),
                    &mappedLength,
                    &mappedFont,
                    &scaleFactor));

                if (mappedFont)
                {
#if WINVER > _WIN32_WINNT_WINBLUE
                    ComPtr<IDWriteFontFaceReference> fontFaceReference;
                    ThrowIfFailed(As<IDWriteFont3>(mappedFont)->GetFontFaceReference(&fontFaceReference));
                    auto canvasFontFace = ResourceManager::GetOrCreate<ICanvasFontFace>(fontFaceReference.Get());
#else
                    auto canvasFontFace = ResourceManager::GetOrCreate<ICanvasFontFace>(mappedFont.Get());
#endif
                    auto canvasScaledFont = Make<CanvasScaledFont>(canvasFontFace.Get(), scaleFactor);
                    CheckMakeResult(canvasScaledFont);

                    auto newPair = MakeCharacterRangeKeyValue<CanvasScaledFont*, ICanvasScaledFont*>(characterIndex, mappedLength, canvasScaledFont.Get());

                    ThrowIfFailed(vector->Append(newPair.Get()));
                }

                characterIndex += mappedLength;
                charactersLeft -= mappedLength;
            }

            ThrowIfFailed(vector->GetView(result));
        });
}


IFACEMETHODIMP CanvasTextAnalyzer::AnalyzeScript(
    IVectorView<IKeyValuePair<CanvasCharacterRange, CanvasAnalyzedScript>*>** values)
{
    return AnalyzeScriptWithLocale(nullptr, values);
}
    
IFACEMETHODIMP CanvasTextAnalyzer::AnalyzeScriptWithLocale(
    HSTRING locale,
    IVectorView<IKeyValuePair<CanvasCharacterRange, CanvasAnalyzedScript>*>** values)
{
    return ExceptionBoundary(
        [&]
        {
            CheckAndClearOutPointer(values);

            WinString localeString(locale);
            auto dwriteTextAnalysisSource = CreateTextAnalysisSource(localeString);

            auto dwriteTextAnalysisSink = Make<DWriteTextAnalysisSink>();

            uint32_t textLength;
            WindowsGetStringRawBuffer(m_text, &textLength);

            ThrowIfFailed(CustomFontManager::GetInstance()->GetTextAnalyzer()->AnalyzeScript(dwriteTextAnalysisSource.Get(), 0, textLength, dwriteTextAnalysisSink.Get()));

            ThrowIfFailed(dwriteTextAnalysisSink->GetAnalyzedScript()->GetView(values));
        });
}

WinString ToStringIsoCode(uint32_t code)
{
    WinStringBuilder builder;
    auto buffer = builder.Allocate(4);

    buffer[0] = (code >> 0) & 0xFF;
    buffer[1] = (code >> 8) & 0xFF;
    buffer[2] = (code >> 16) & 0xFF;
    buffer[3] = (code >> 24) & 0xFF;

    return builder.Get();
}

IFACEMETHODIMP CanvasTextAnalyzer::GetScriptProperties(
    CanvasAnalyzedScript analyzedScript,
    CanvasScriptProperties* scriptProperties)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(scriptProperties);

            auto dwriteScriptAnalysis = ToDWriteScriptAnalysis(analyzedScript);

            DWRITE_SCRIPT_PROPERTIES dwriteScriptProperties;

            ThrowIfFailed(CustomFontManager::GetInstance()->GetTextAnalyzer()->GetScriptProperties(dwriteScriptAnalysis, &dwriteScriptProperties));

            WinString isoCodeString = ToStringIsoCode(dwriteScriptProperties.isoScriptCode);
            isoCodeString.CopyTo(&scriptProperties->IsoScriptCode);

            scriptProperties->IsoScriptNumber = static_cast<int>(dwriteScriptProperties.isoScriptNumber);
            scriptProperties->ClusterLookahead = static_cast<int>(dwriteScriptProperties.clusterLookahead);

            WinString justificationCharacterString = ConvertCharacterCodepointToString(dwriteScriptProperties.justificationCharacter);
            justificationCharacterString.CopyTo(&scriptProperties->JustificationCharacter);

            scriptProperties->RestrictCaretToClusters = dwriteScriptProperties.restrictCaretToClusters;
            scriptProperties->UsesWordDividers = dwriteScriptProperties.usesWordDividers;
            scriptProperties->IsDiscreteWriting = dwriteScriptProperties.isDiscreteWriting;
            scriptProperties->IsBlockWriting = dwriteScriptProperties.isBlockWriting;
            scriptProperties->IsDistributedWithinCluster = dwriteScriptProperties.isDistributedWithinCluster;
            scriptProperties->IsConnectedWriting = dwriteScriptProperties.isConnectedWriting;
            scriptProperties->IsCursiveWriting = dwriteScriptProperties.isCursiveWriting;
        });
}

IFACEMETHODIMP CanvasTextAnalyzer::GetGlyphs(
    CanvasCharacterRange characterRange,
    ICanvasFontFace* fontFace,
    float fontSize,
    boolean isSideways,
    boolean isRightToLeft,
    CanvasAnalyzedScript script,
    uint32_t* valueCount,
    CanvasGlyph** valueElements)
{
    return GetGlyphsWithAllOptions(characterRange, fontFace, fontSize, isSideways, isRightToLeft, script, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, valueCount, valueElements);
}

IFACEMETHODIMP CanvasTextAnalyzer::GetGlyphsWithAllOptions(
    CanvasCharacterRange characterRange,
    ICanvasFontFace* fontFace,
    float fontSize,
    boolean isSideways,
    boolean isRightToLeft,
    CanvasAnalyzedScript script,
    HSTRING locale,
    ICanvasNumberSubstitution* numberSubstitution,
    IVectorView<IKeyValuePair<CanvasCharacterRange, CanvasTypography*>*>* typographyRanges,
    uint32_t* clusterMapIndexCount,
    int** clusterMapIndexElements,
    uint32_t* isShapedAloneCount,
    boolean** isShapedAloneElements,
    uint32_t* glyphShapingCount,
    CanvasGlyphShaping** glyphShapingElements,
    uint32_t* valueCount,
    CanvasGlyph** valueElements)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(fontFace);
            CheckAndClearOutPointer(valueElements);

            ThrowIfNegative(characterRange.CharacterIndex);
            ThrowIfNegative(characterRange.CharacterCount);

            wchar_t const* text;
            uint32_t textLength;

            text = WindowsGetStringRawBuffer(m_text, &textLength);

            if (textLength > 0 && characterRange.CharacterIndex >= static_cast<int>(textLength))
                ThrowHR(E_INVALIDARG);

            if (characterRange.CharacterIndex + characterRange.CharacterCount > static_cast<int>(textLength))
                ThrowHR(E_INVALIDARG);

            text += characterRange.CharacterIndex;
            textLength = characterRange.CharacterCount;

            auto dwriteScriptAnalysis = ToDWriteScriptAnalysis(script);

            std::vector<uint16_t> clusterMap;
            clusterMap.resize(textLength);

            std::vector<DWRITE_SHAPING_TEXT_PROPERTIES> shapingTextProperties;
            shapingTextProperties.resize(textLength);

            auto dwriteFontFace = As<ICanvasFontFaceInternal>(fontFace)->GetRealizedFontFace();
            
            ComPtr<IDWriteNumberSubstitution> dwriteNumberSubstitution;
            if (numberSubstitution)
                dwriteNumberSubstitution = GetWrappedResource<IDWriteNumberSubstitution>(numberSubstitution);

            uint32_t typographyRangeCount = 0;
            std::vector<uint32_t> featureRangeLengths;

            struct DWriteTypographicFeatureData
            {
                DWRITE_TYPOGRAPHIC_FEATURES Features;
                std::vector<DWRITE_FONT_FEATURE> Data;
            };
            std::vector<DWriteTypographicFeatureData> featureData;
            std::vector<DWRITE_TYPOGRAPHIC_FEATURES const*> featureDataPointers;
            if (typographyRanges)
            {
                ThrowIfFailed(typographyRanges->get_Size(&typographyRangeCount));

                featureRangeLengths.resize(typographyRangeCount);
                featureData.resize(typographyRangeCount);
                featureDataPointers.resize(typographyRangeCount);

                int characterIndex = characterRange.CharacterIndex; // For validating the sizes of the spans.
                int characterRangeEnd = characterRange.CharacterIndex + characterRange.CharacterCount;

                for (uint32_t i = 0; i < typographyRangeCount; ++i)
                {
                    ComPtr<IKeyValuePair<CanvasCharacterRange, CanvasTypography*>> typographyRangeElement;
                    ThrowIfFailed(typographyRanges->GetAt(i, &typographyRangeElement));

                    CanvasCharacterRange typographyCharacterRange;
                    ThrowIfFailed(typographyRangeElement->get_Key(&typographyCharacterRange));

                    ComPtr<ICanvasTypography> typography;
                    ThrowIfFailed(typographyRangeElement->get_Value(&typography));

                    //
                    // Perform some validation, ensuring spans are sequential and don't overlap.
                    //
                    ThrowIfNegative(typographyCharacterRange.CharacterIndex);
                    ThrowIfNegative(typographyCharacterRange.CharacterCount);
                    
                    //
                    // Clamp the typography span character range to the character range passed
                    // to GetGlyphs.
                    //
                    int typographyRangeEnd = typographyCharacterRange.CharacterIndex + typographyCharacterRange.CharacterCount;
                    bool rangesOverlap =
                        !(typographyRangeEnd <= characterRange.CharacterIndex ||
                        characterRangeEnd <= typographyCharacterRange.CharacterIndex);

                    if (rangesOverlap)
                    {
                        typographyCharacterRange.CharacterIndex = std::max(
                            typographyCharacterRange.CharacterIndex,
                            characterRange.CharacterIndex);

                        typographyRangeEnd = std::min(typographyRangeEnd, characterRangeEnd);
                        typographyCharacterRange.CharacterCount = typographyRangeEnd - typographyCharacterRange.CharacterIndex;

                        auto features = As<ICanvasTypographyInternal>(typography)->GetFeatureData();
                        const uint32_t featureCount = static_cast<uint32_t>(features.size());

                        featureData[i].Data = std::move(features);
                        featureData[i].Features.featureCount = featureCount;
                        featureData[i].Features.features = featureCount > 0 ? &featureData[i].Data[0] : nullptr;
                        featureDataPointers[i] = &featureData[i].Features;
                        featureRangeLengths[i] = typographyCharacterRange.CharacterCount;

                        if (characterIndex != typographyCharacterRange.CharacterIndex)
                            ThrowHR(E_INVALIDARG);
                        characterIndex += typographyCharacterRange.CharacterCount;
                    }
                    else
                    {
                        featureData[i].Features.featureCount = 0;
                        featureData[i].Features.features = nullptr;
                        featureDataPointers[i] = nullptr;
                        featureRangeLengths[i] = 0;
                    }
                }

                if (characterIndex != characterRangeEnd)
                    ThrowHR(E_INVALIDARG);
            }

            std::vector<DWRITE_TYPOGRAPHIC_FEATURES> featuresPerSpan;

            std::vector<uint16_t> glyphIndices;

            std::vector<DWRITE_SHAPING_GLYPH_PROPERTIES> shapingGlyphProperties;

            //
            // A text span can map to a (theoretically) unbounded number of glyphs, and there
            // isn't a way to obtain the count of the glyphs without evaluating the glyphs
            // themselves. We size a buffer and expand it according to DWrite's 
            // recommended heuristic.
            //
            uint32_t maxGlyphCount = (3 * textLength / 2 + 16);

            HRESULT getGlyphsResult;

            uint32_t actualGlyphCount{};

            for (int resizeAttempt = 0; resizeAttempt < 3; resizeAttempt++)
            {
                glyphIndices.resize(maxGlyphCount);

                shapingGlyphProperties.resize(maxGlyphCount);

                getGlyphsResult = CustomFontManager::GetInstance()->GetTextAnalyzer()->GetGlyphs(
                    text,
                    textLength,
                    dwriteFontFace.Get(),
                    isSideways,
                    isRightToLeft,
                    &dwriteScriptAnalysis,
                    WindowsGetStringRawBuffer(locale, nullptr),
                    dwriteNumberSubstitution.Get(),
                    typographyRanges ? featureDataPointers.data() : nullptr,
                    typographyRanges ? featureRangeLengths.data() : nullptr,
                    typographyRangeCount,
                    maxGlyphCount,
                    clusterMap.data(),
                    shapingTextProperties.data(),
                    glyphIndices.data(),
                    shapingGlyphProperties.data(),
                    &actualGlyphCount);

                if (SUCCEEDED(getGlyphsResult))
                {
                    break;
                }
                else if (getGlyphsResult == HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER))
                {
                    maxGlyphCount *= 3;
                }
                else
                {
                    ThrowHR(getGlyphsResult);
                }
            }
            ThrowIfFailed(getGlyphsResult);

            std::vector<float> glyphAdvances;
            glyphAdvances.resize(actualGlyphCount);

            std::vector<DWRITE_GLYPH_OFFSET> glyphOffsets;
            glyphOffsets.resize(actualGlyphCount);

            ThrowIfFailed(CustomFontManager::GetInstance()->GetTextAnalyzer()->GetGlyphPlacements(
                text,
                clusterMap.data(),
                shapingTextProperties.data(),
                textLength,
                glyphIndices.data(),
                shapingGlyphProperties.data(),
                actualGlyphCount,
                dwriteFontFace.Get(),
                fontSize,
                isSideways,
                isRightToLeft,
                &dwriteScriptAnalysis,
                WindowsGetStringRawBuffer(locale, nullptr),
                typographyRanges ? featureDataPointers.data() : nullptr,
                typographyRanges ? featureRangeLengths.data() : nullptr,
                typographyRangeCount,
                glyphAdvances.data(),
                glyphOffsets.data()));

            ComArray<CanvasGlyph> glyphs(actualGlyphCount);
            for (uint32_t i = 0; i < actualGlyphCount; ++i)
            {
                glyphs[i].Index = glyphIndices[i];
                glyphs[i].Advance = glyphAdvances[i];
                glyphs[i].AdvanceOffset = glyphOffsets[i].advanceOffset;
                glyphs[i].AscenderOffset = glyphOffsets[i].ascenderOffset;
            }
            glyphs.Detach(valueCount, valueElements);

            if (clusterMapIndexElements)
            {
                auto clusterMapResult = TransformToComArray<int>(clusterMap.begin(), clusterMap.end(), 
                    [](uint16_t value)
                    {
                        return static_cast<int>(value);
                    });
                clusterMapResult.Detach(clusterMapIndexCount, clusterMapIndexElements);
            }

            if (isShapedAloneElements)
            {
                auto isShapedAloneResult = TransformToComArray<boolean>(shapingTextProperties.begin(), shapingTextProperties.end(),
                    [](DWRITE_SHAPING_TEXT_PROPERTIES const& value)
                    {
                        return !!value.isShapedAlone;
                    });
                isShapedAloneResult.Detach(isShapedAloneCount, isShapedAloneElements);
            }

            if (glyphShapingElements)
            {
                auto glyphShaping = TransformToComArray<CanvasGlyphShaping>(shapingGlyphProperties.begin(), shapingGlyphProperties.begin() + actualGlyphCount,
                    [](DWRITE_SHAPING_GLYPH_PROPERTIES const& dwriteValue)
                    {
                        CanvasGlyphShaping result{};
                        result.Justification = ToCanvasGlyphJustification(dwriteValue.justification);
                        result.IsClusterStart = dwriteValue.isClusterStart;
                        result.IsDiacritic = dwriteValue.isDiacritic;
                        result.IsZeroWidthSpace = dwriteValue.isZeroWidthSpace;
                        return result;
                    });
                glyphShaping.Detach(glyphShapingCount, glyphShapingElements);
            }
        });
}

HRESULT CanvasTextAnalyzerFactory::Create(
    HSTRING text,
    CanvasTextDirection textDirection,
    ICanvasTextAnalyzer** out)
{
    return CreateWithOptions(text, textDirection, nullptr, out);
}


HRESULT CanvasTextAnalyzerFactory::CreateWithNumberSubstitutionAndVerticalGlyphOrientationAndBidiLevel(
    HSTRING text,
    CanvasTextDirection textDirection,
    ICanvasNumberSubstitution* numberSubstitution,
    CanvasVerticalGlyphOrientation verticalGlyphOrientation,
    uint32_t bidiLevel,
    ICanvasTextAnalyzer** textAnalyzer)
{
    return ExceptionBoundary(
        [&]
        {
            auto newTextAnalyzer = Make<CanvasTextAnalyzer>(text, textDirection, numberSubstitution, verticalGlyphOrientation, bidiLevel);

            ThrowIfFailed(newTextAnalyzer.CopyTo(textAnalyzer));
        });
}


HRESULT CanvasTextAnalyzerFactory::CreateWithOptions(
    HSTRING text,
    CanvasTextDirection textDirection,
    ICanvasTextAnalyzerOptions* source,
    ICanvasTextAnalyzer** textAnalyzer)
{
    return ExceptionBoundary(
        [&]
        {
            auto newTextAnalyzer = Make<CanvasTextAnalyzer>(text, textDirection, source);

            ThrowIfFailed(newTextAnalyzer.CopyTo(textAnalyzer));
        });
}


ActivatableClassWithFactory(CanvasTextAnalyzer, CanvasTextAnalyzerFactory);