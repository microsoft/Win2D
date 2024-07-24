// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

#include "CanvasTextAnalyzer.h"
#include "CanvasFontSet.h"
#include "CanvasScaledFont.h"
#include "CanvasFontFace.h"
#include "CanvasTypography.h"
#include "CanvasNumberSubstitution.h"

using namespace ABI::Microsoft::Graphics::Canvas;
using namespace ABI::Microsoft::Graphics::Canvas::Text;

DWriteTextAnalysisSource::DWriteTextAnalysisSource(
    WinString text,
    uint32_t textLength,
    CanvasTextDirection textDirection,
    ComPtr<ICanvasTextAnalyzerOptions> const& analyzerOptions,
    ComPtr<ICanvasNumberSubstitution> defaultNumberSubstitution,
    CanvasVerticalGlyphOrientation defaultVerticalGlyphOrientation,
    uint32_t defaultBidiLevel)
    : m_text(text)
    , m_textLength(textLength)
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

void DWriteTextAnalysisSource::SetLocaleName(WinString localeName)
{
    m_localeName = localeName;
}

IFACEMETHODIMP DWriteTextAnalysisSource::GetTextAtPosition(
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


IFACEMETHODIMP DWriteTextAnalysisSource::GetTextBeforePosition(
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

IFACEMETHODIMP_(DWRITE_READING_DIRECTION) DWriteTextAnalysisSource::GetParagraphReadingDirection()
{
    return m_dwriteReadingDirection;
}

IFACEMETHODIMP DWriteTextAnalysisSource::GetLocaleName(
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

IFACEMETHODIMP DWriteTextAnalysisSource::GetNumberSubstitution(
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

IFACEMETHODIMP DWriteTextAnalysisSource::GetVerticalGlyphOrientation(
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

uint32_t DWriteTextAnalysisSource::GetCharacterCountForRemainderOfText(uint32_t textPosition)
{
    //
    // Returned as a character count when we have uniform formatting.
    //
    return textPosition < m_textLength ? m_textLength - textPosition : 0;
}

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

STDMETHODIMP DWriteTextAnalysisSink::SetBidiLevel(
    uint32_t textPosition,
    uint32_t textLength,
    uint8_t explicitLevel,
    uint8_t resolvedLevel)
{
    return ExceptionBoundary(
        [&]
        {
            EnsureAnalyzedBidi();

            CanvasAnalyzedBidi analyzedBidi{};
            analyzedBidi.ExplicitLevel = explicitLevel;
            analyzedBidi.ResolvedLevel = resolvedLevel;

            auto newPair = MakeCharacterRangeKeyValue(textPosition, textLength, analyzedBidi);

            ThrowIfFailed(m_analyzedBidi->Append(newPair.Get()));
        });
}

STDMETHODIMP DWriteTextAnalysisSink::SetLineBreakpoints(
    uint32_t textPosition,
    uint32_t textLength,
    DWRITE_LINE_BREAKPOINT const* dwriteLineBreakpoint)
{
    return ExceptionBoundary(
        [&]
        {
            EnsureAnalyzedLineBreakpoints();

            for (uint32_t i = 0; i < textLength; ++i)
            {
                auto& b = m_analyzedLineBreakpoints[textPosition + i];

                b.BreakBefore = ToCanvasLineBreakCondition(dwriteLineBreakpoint[i].breakConditionBefore);
                b.BreakAfter = ToCanvasLineBreakCondition(dwriteLineBreakpoint[i].breakConditionAfter);
                b.IsWhitespace = dwriteLineBreakpoint[i].isWhitespace;
                b.IsSoftHyphen = dwriteLineBreakpoint[i].isSoftHyphen;
            }
        });
}

STDMETHODIMP DWriteTextAnalysisSink::SetNumberSubstitution(
    uint32_t textPosition,
    uint32_t textLength,
    IDWriteNumberSubstitution* dwriteNumberSubstitution)
{
    return ExceptionBoundary(
        [&]
        {
            EnsureAnalyzedNumberSubstitution();

            auto numberSubstitution = ResourceManager::GetOrCreate<ICanvasNumberSubstitution>(dwriteNumberSubstitution);

            auto newPair = MakeCharacterRangeKeyValue<CanvasNumberSubstitution*, ICanvasNumberSubstitution*>(
                textPosition, 
                textLength, 
                static_cast<CanvasNumberSubstitution*>(numberSubstitution.Get()));

            ThrowIfFailed(m_analyzedNumberSubstitution->Append(newPair.Get()));
        });
}

STDMETHODIMP DWriteTextAnalysisSink::SetScriptAnalysis(
    uint32_t textPosition,
    uint32_t textLength,
    DWRITE_SCRIPT_ANALYSIS const* scriptAnalysis)
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

STDMETHODIMP DWriteTextAnalysisSink::SetGlyphOrientation(
    uint32_t textPosition,
    uint32_t textLength,
    DWRITE_GLYPH_ORIENTATION_ANGLE dwriteGlyphOrientationAngle,
    uint8_t adjustedBidiLevel,
    BOOL isSideways,
    BOOL isRightToLeft)
{
    return ExceptionBoundary(
        [&]
        {
            EnsureAnalyzedGlyphOrientation();

            CanvasAnalyzedGlyphOrientation analyzedGlyphOrientation{};
            analyzedGlyphOrientation.GlyphOrientation = ToCanvasGlyphOrientation(dwriteGlyphOrientationAngle);
            analyzedGlyphOrientation.AdjustedBidiLevel = adjustedBidiLevel;
            analyzedGlyphOrientation.IsSideways = !!isSideways;
            analyzedGlyphOrientation.IsRightToLeft = !!isRightToLeft;

            auto newPair = MakeCharacterRangeKeyValue(textPosition, textLength, analyzedGlyphOrientation);

            ThrowIfFailed(m_analyzedGlyphOrientation->Append(newPair.Get()));
        });
}

ComPtr<Vector<IKeyValuePair<CanvasCharacterRange, CanvasAnalyzedScript>*>> DWriteTextAnalysisSink::GetAnalyzedScript()
{
    EnsureAnalyzedScript();

    auto result = m_analyzedScript;
    
    m_analyzedScript.Reset();

    return result;
}

ComPtr<Vector<IKeyValuePair<CanvasCharacterRange, CanvasAnalyzedBidi>*>> DWriteTextAnalysisSink::GetAnalyzedBidi()
{
    EnsureAnalyzedBidi();

    auto result = m_analyzedBidi;

    m_analyzedBidi.Reset();

    return result;
}

ComArray<CanvasAnalyzedBreakpoint> DWriteTextAnalysisSink::GetAnalyzedLineBreakpoints()
{
    EnsureAnalyzedLineBreakpoints();

    ComArray<CanvasAnalyzedBreakpoint> result = std::move(m_analyzedLineBreakpoints);

    return result;
}

ComPtr<Vector<IKeyValuePair<CanvasCharacterRange, CanvasNumberSubstitution*>*>> DWriteTextAnalysisSink::GetAnalyzedNumberSubstitution()
{
    EnsureAnalyzedNumberSubstitution();

    auto result = m_analyzedNumberSubstitution;

    m_analyzedNumberSubstitution.Reset();

    return result;
}

ComPtr<Vector<IKeyValuePair<CanvasCharacterRange, CanvasAnalyzedGlyphOrientation>*>> DWriteTextAnalysisSink::GetAnalyzedGlyphOrientation()
{
    EnsureAnalyzedGlyphOrientation();

    auto result = m_analyzedGlyphOrientation;

    m_analyzedGlyphOrientation.Reset();

    return result;
}

void DWriteTextAnalysisSink::EnsureAnalyzedBidi()
{
    if (!m_analyzedBidi)
    {
        m_analyzedBidi = Make<Vector<IKeyValuePair<CanvasCharacterRange, CanvasAnalyzedBidi>*>>();
        CheckMakeResult(m_analyzedBidi);
    }
}

void DWriteTextAnalysisSink::EnsureAnalyzedLineBreakpoints()
{
    if (!m_analyzedLineBreakpoints.GetData())
    {
        m_analyzedLineBreakpoints = ComArray<CanvasAnalyzedBreakpoint>(m_textLength);
    }
}

void DWriteTextAnalysisSink::EnsureAnalyzedNumberSubstitution()
{
    if (!m_analyzedNumberSubstitution)
    {
        m_analyzedNumberSubstitution = Make<Vector<IKeyValuePair<CanvasCharacterRange, CanvasNumberSubstitution*>*>>();
        CheckMakeResult(m_analyzedNumberSubstitution);
    }
}

void DWriteTextAnalysisSink::EnsureAnalyzedScript()
{
    if (!m_analyzedScript)
    {
        m_analyzedScript = Make<Vector<IKeyValuePair<CanvasCharacterRange, CanvasAnalyzedScript>*>>();
        CheckMakeResult(m_analyzedScript);
    }
}

void DWriteTextAnalysisSink::EnsureAnalyzedGlyphOrientation()
{
    if (!m_analyzedGlyphOrientation)
    {
        m_analyzedGlyphOrientation = Make<Vector<IKeyValuePair<CanvasCharacterRange, CanvasAnalyzedGlyphOrientation>*>>();
        CheckMakeResult(m_analyzedGlyphOrientation);
    }
}

CanvasTextAnalyzer::CanvasTextAnalyzer(
    HSTRING text,
    CanvasTextDirection textDirection,
    ICanvasTextAnalyzerOptions* source)
    : m_text(text)
    , m_textDirection(textDirection)
    , m_source(source)
    , m_defaultVerticalGlyphOrientation(CanvasVerticalGlyphOrientation::Default)
    , m_defaultBidiLevel(0)
    , m_customFontManager(CustomFontManager::GetInstance())
{
    CreateTextAnalysisSourceAndSink();
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
    , m_customFontManager(CustomFontManager::GetInstance())
{
    if (bidiLevel > UINT8_MAX)
        ThrowHR(E_INVALIDARG);

    m_defaultBidiLevel = bidiLevel;

    CreateTextAnalysisSourceAndSink();
}


IFACEMETHODIMP CanvasTextAnalyzer::GetFontsUsingSystemFontSet(
    ICanvasTextFormat* textFormat,
    IVectorView<IKeyValuePair<CanvasCharacterRange, CanvasScaledFont*>*>** result)
{
    return GetFonts(textFormat, nullptr, result);
}

void CanvasTextAnalyzer::CreateTextAnalysisSourceAndSink()
{
    uint32_t textLength;
    WindowsGetStringRawBuffer(m_text, &textLength);

    m_dwriteTextAnalysisSource = Make<DWriteTextAnalysisSource>(
        m_text,
        textLength,
        m_textDirection,
        m_source,
        m_defaultNumberSubstitution,
        m_defaultVerticalGlyphOrientation,
        m_defaultBidiLevel);
    CheckMakeResult(m_dwriteTextAnalysisSource);

    m_dwriteTextAnalysisSink = Make<DWriteTextAnalysisSink>(textLength);
    CheckMakeResult(m_dwriteTextAnalysisSink);
}

IFACEMETHODIMP CanvasTextAnalyzer::GetFonts(
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

            m_dwriteTextAnalysisSource->SetLocaleName(localeNameString);

            ComPtr<IDWriteFontCollection> dwriteFontCollection;

            if (requestedFontSet)
            {
                auto dwriteFontSet = GetWrappedResource<IDWriteFontSet>(requestedFontSet);
                auto factory = As<IDWriteFactory3>(m_customFontManager->GetSharedFactory());
                ComPtr<IDWriteFontCollection1> dwriteFontCollection1;
                ThrowIfFailed(factory->CreateFontCollectionFromFontSet(dwriteFontSet.Get(), &dwriteFontCollection1));

                dwriteFontCollection = As<IDWriteFontCollection>(dwriteFontCollection1.Get());
            }
            else
            {
                auto factory = m_customFontManager->GetSharedFactory();
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
                ThrowIfFailed(m_customFontManager->GetSystemFontFallback()->MapCharacters(
                    m_dwriteTextAnalysisSource.Get(),
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
                    ComPtr<IDWriteFontFaceReference> fontFaceReference;
                    ThrowIfFailed(As<IDWriteFont3>(mappedFont)->GetFontFaceReference(&fontFaceReference));
                    auto canvasFontFace = ResourceManager::GetOrCreate<ICanvasFontFace>(fontFaceReference.Get());

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

IFACEMETHODIMP CanvasTextAnalyzer::GetBidi(
    IVectorView<IKeyValuePair<CanvasCharacterRange, CanvasAnalyzedBidi>*>** values)
{
    return GetBidiWithLocale(nullptr, values);
}

IFACEMETHODIMP CanvasTextAnalyzer::GetBidiWithLocale(
    HSTRING locale,
    IVectorView<IKeyValuePair<CanvasCharacterRange, CanvasAnalyzedBidi>*>** values)
{
    return ExceptionBoundary(
        [&]
        {
            CheckAndClearOutPointer(values);

            WinString localeString(locale);
            m_dwriteTextAnalysisSource->SetLocaleName(localeString);

            uint32_t textLength;
            WindowsGetStringRawBuffer(m_text, &textLength);
            
            ThrowIfFailed(m_customFontManager->GetTextAnalyzer()->AnalyzeBidi(m_dwriteTextAnalysisSource.Get(), 0, textLength, m_dwriteTextAnalysisSink.Get()));

            ThrowIfFailed(m_dwriteTextAnalysisSink->GetAnalyzedBidi()->GetView(values));
        });
}

IFACEMETHODIMP CanvasTextAnalyzer::GetBreakpoints(
    uint32_t* valueCount,
    CanvasAnalyzedBreakpoint** valueElements)
{
    return GetBreakpointsWithLocale(nullptr, valueCount, valueElements);
}

IFACEMETHODIMP CanvasTextAnalyzer::GetBreakpointsWithLocale(
    HSTRING locale,
    uint32_t* valueCount,
    CanvasAnalyzedBreakpoint** valueElements)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(valueCount);
            CheckAndClearOutPointer(valueElements);

            WinString localeString(locale);
            m_dwriteTextAnalysisSource->SetLocaleName(localeString);

            uint32_t textLength;
            WindowsGetStringRawBuffer(m_text, &textLength);

            ThrowIfFailed(m_customFontManager->GetTextAnalyzer()->AnalyzeLineBreakpoints(m_dwriteTextAnalysisSource.Get(), 0, textLength, m_dwriteTextAnalysisSink.Get()));

            auto analyzedBreakpoints = m_dwriteTextAnalysisSink->GetAnalyzedLineBreakpoints();

            analyzedBreakpoints.Detach(valueCount, valueElements);
        });
}

IFACEMETHODIMP CanvasTextAnalyzer::GetNumberSubstitutions(
    IVectorView<IKeyValuePair<CanvasCharacterRange, CanvasNumberSubstitution*>*>** values)
{
    return ExceptionBoundary(
        [&]
        {
            CheckAndClearOutPointer(values);

            uint32_t textLength;
            WindowsGetStringRawBuffer(m_text, &textLength);

            ThrowIfFailed(m_customFontManager->GetTextAnalyzer()->AnalyzeNumberSubstitution(m_dwriteTextAnalysisSource.Get(), 0, textLength, m_dwriteTextAnalysisSink.Get()));

            ThrowIfFailed(m_dwriteTextAnalysisSink->GetAnalyzedNumberSubstitution()->GetView(values));
        });
}

IFACEMETHODIMP CanvasTextAnalyzer::GetScript(
    IVectorView<IKeyValuePair<CanvasCharacterRange, CanvasAnalyzedScript>*>** values)
{
    return GetScriptWithLocale(nullptr, values);
}
    
IFACEMETHODIMP CanvasTextAnalyzer::GetScriptWithLocale(
    HSTRING locale,
    IVectorView<IKeyValuePair<CanvasCharacterRange, CanvasAnalyzedScript>*>** values)
{
    return ExceptionBoundary(
        [&]
        {
            CheckAndClearOutPointer(values);

            WinString localeString(locale);
            m_dwriteTextAnalysisSource->SetLocaleName(localeString);

            uint32_t textLength;
            WindowsGetStringRawBuffer(m_text, &textLength);

            ThrowIfFailed(m_customFontManager->GetTextAnalyzer()->AnalyzeScript(m_dwriteTextAnalysisSource.Get(), 0, textLength, m_dwriteTextAnalysisSink.Get()));

            ThrowIfFailed(m_dwriteTextAnalysisSink->GetAnalyzedScript()->GetView(values));
        });
}

IFACEMETHODIMP CanvasTextAnalyzer::GetGlyphOrientations(
    IVectorView<IKeyValuePair<CanvasCharacterRange, CanvasAnalyzedGlyphOrientation>*>** values)
{
    return GetGlyphOrientationsWithLocale(nullptr, values);
}
    
IFACEMETHODIMP CanvasTextAnalyzer::GetGlyphOrientationsWithLocale(
    HSTRING locale,
    IVectorView<IKeyValuePair<CanvasCharacterRange, CanvasAnalyzedGlyphOrientation>*>** values)
{
    return ExceptionBoundary(
        [&]
        {
            CheckAndClearOutPointer(values);

            WinString localeString(locale);
            m_dwriteTextAnalysisSource->SetLocaleName(localeString);

            uint32_t textLength;
            WindowsGetStringRawBuffer(m_text, &textLength);

            ThrowIfFailed(m_customFontManager->GetTextAnalyzer()->AnalyzeVerticalGlyphOrientation(m_dwriteTextAnalysisSource.Get(), 0, textLength, m_dwriteTextAnalysisSink.Get()));

            ThrowIfFailed(m_dwriteTextAnalysisSink->GetAnalyzedGlyphOrientation()->GetView(values));
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

            ThrowIfFailed(m_customFontManager->GetTextAnalyzer()->GetScriptProperties(dwriteScriptAnalysis, &dwriteScriptProperties));

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

struct DWriteTypographicFeatureData
{
    DWRITE_TYPOGRAPHIC_FEATURES Features;
    std::vector<DWRITE_FONT_FEATURE> Data;
};

struct DWriteTypographyRangeData
{
    std::vector<uint32_t> FeatureRangeLengths;
    std::vector<DWriteTypographicFeatureData> FeatureData;
    std::vector<DWRITE_TYPOGRAPHIC_FEATURES const*> FeatureDataPointers;
};

static void GetDWriteTypographyRanges(
    CanvasCharacterRange characterRange,
    IVectorView<IKeyValuePair<CanvasCharacterRange, CanvasTypography*>*>* typographyRanges,
    uint32_t* typographyRangeCountResult,
    DWriteTypographyRangeData* result)
{
    uint32_t typographyRangeCount;
    ThrowIfFailed(typographyRanges->get_Size(&typographyRangeCount));

    result->FeatureRangeLengths.resize(typographyRangeCount);
    result->FeatureData.resize(typographyRangeCount);
    result->FeatureDataPointers.resize(typographyRangeCount);

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

            result->FeatureData[i].Data = std::move(features);
            result->FeatureData[i].Features.featureCount = featureCount;
            result->FeatureData[i].Features.features = featureCount > 0 ? &result->FeatureData[i].Data[0] : nullptr;
            result->FeatureDataPointers[i] = &result->FeatureData[i].Features;
            result->FeatureRangeLengths[i] = typographyCharacterRange.CharacterCount;

            if (characterIndex != typographyCharacterRange.CharacterIndex)
                ThrowHR(E_INVALIDARG);
            characterIndex += typographyCharacterRange.CharacterCount;
        }
        else
        {
            result->FeatureData[i].Features.featureCount = 0;
            result->FeatureData[i].Features.features = nullptr;
            result->FeatureDataPointers[i] = nullptr;
            result->FeatureRangeLengths[i] = 0;
        }
    }

    if (characterIndex != characterRangeEnd)
        ThrowHR(E_INVALIDARG);

    *typographyRangeCountResult = typographyRangeCount;
}

static void RetryWithIncreasingGlyphCount(
    uint32_t textLength, 
    std::function<HRESULT(uint32_t)> fn)
{
    //
    // A text span can map to a (theoretically) unbounded number of glyphs, and there
    // isn't a way to obtain the count of the glyphs without evaluating the glyphs
    // themselves. We allow the opportunity to size a buffer and expand it according to DWrite's 
    // recommended heuristic.
    //

    uint32_t maxGlyphCount = (3 * textLength / 2 + 16);

    HRESULT result;

    // 
    // DWrite's heuristic recommends 3 resizing attempts before giving up. Looping forever is
    // a bad idea, since a malformed font can indeed cause unbounded glyph expansion.
    //
    for (int resizeAttempt = 0; resizeAttempt < 3; resizeAttempt++)
    {
        result = fn(maxGlyphCount);

        if (SUCCEEDED(result))
        {
            break;
        }
        else if (result == HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER))
        {
            maxGlyphCount *= 3;
        }
        else
        {
            ThrowHR(result);
        }
    }
    ThrowIfFailed(result);
}

static void ThrowIfInvalidCharacterRange(uint32_t textLength, CanvasCharacterRange characterRange)
{
    if (textLength > 0 && characterRange.CharacterIndex >= static_cast<int>(textLength))
        ThrowHR(E_INVALIDARG);

    if (characterRange.CharacterIndex + characterRange.CharacterCount > static_cast<int>(textLength))
        ThrowHR(E_INVALIDARG);
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

            ThrowIfInvalidCharacterRange(textLength, characterRange);

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
            DWriteTypographyRangeData dwriteTypographyRangeData;
            if (typographyRanges)
            {
                GetDWriteTypographyRanges(characterRange, typographyRanges, &typographyRangeCount, &dwriteTypographyRangeData);
            }

            std::vector<DWRITE_TYPOGRAPHIC_FEATURES> featuresPerSpan;

            std::vector<uint16_t> glyphIndices;

            std::vector<DWRITE_SHAPING_GLYPH_PROPERTIES> shapingGlyphProperties;

            uint32_t actualGlyphCount{};    
            RetryWithIncreasingGlyphCount(
                textLength,
                [&](uint32_t maxGlyphCount)
                {
                    glyphIndices.resize(maxGlyphCount);

                    shapingGlyphProperties.resize(maxGlyphCount);

                    return m_customFontManager->GetTextAnalyzer()->GetGlyphs(
                        text,
                        textLength,
                        dwriteFontFace.Get(),
                        isSideways,
                        isRightToLeft,
                        &dwriteScriptAnalysis,
                        WindowsGetStringRawBuffer(locale, nullptr),
                        dwriteNumberSubstitution.Get(),
                        typographyRanges ? dwriteTypographyRangeData.FeatureDataPointers.data() : nullptr,
                        typographyRanges ? dwriteTypographyRangeData.FeatureRangeLengths.data() : nullptr,
                        typographyRangeCount,
                        maxGlyphCount,
                        clusterMap.data(),
                        shapingTextProperties.data(),
                        glyphIndices.data(),
                        shapingGlyphProperties.data(),
                        &actualGlyphCount);
                });

            std::vector<float> glyphAdvances;
            glyphAdvances.resize(actualGlyphCount);

            std::vector<DWRITE_GLYPH_OFFSET> glyphOffsets;
            glyphOffsets.resize(actualGlyphCount);

            ThrowIfFailed(m_customFontManager->GetTextAnalyzer()->GetGlyphPlacements(
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
                typographyRanges ? dwriteTypographyRangeData.FeatureDataPointers.data() : nullptr,
                typographyRanges ? dwriteTypographyRangeData.FeatureRangeLengths.data() : nullptr,
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

static std::vector<uint16_t> GetDWriteClusterMap(
    uint32_t clusterMapIndicesCount,
    int* clusterMapIndicesElements)
{
    std::vector<uint16_t> dwriteClusterMap;
    dwriteClusterMap.reserve(clusterMapIndicesCount);
    for (uint32_t i = 0; i < clusterMapIndicesCount; ++i)
    {
        ThrowIfNegative(clusterMapIndicesElements[i]);
        dwriteClusterMap.push_back(static_cast<uint16_t>(clusterMapIndicesElements[i]));
    }

    return dwriteClusterMap;
}

static std::vector<DWRITE_SHAPING_GLYPH_PROPERTIES> GetDWriteGlyphShapingProperties(
    uint32_t glyphShapingResultsCount,
    CanvasGlyphShaping* glyphShapingResultsElements)
{
    std::vector<DWRITE_SHAPING_GLYPH_PROPERTIES> dwriteShapingGlyphProperties;
    dwriteShapingGlyphProperties.reserve(glyphShapingResultsCount);
    for (uint32_t i = 0; i < glyphShapingResultsCount; ++i)
    {
        DWRITE_SHAPING_GLYPH_PROPERTIES d;
        d.justification = static_cast<uint32_t>(glyphShapingResultsElements[i].Justification);
        d.isClusterStart = glyphShapingResultsElements[i].IsClusterStart;
        d.isDiacritic = glyphShapingResultsElements[i].IsDiacritic;
        d.isZeroWidthSpace = glyphShapingResultsElements[i].IsZeroWidthSpace;

        dwriteShapingGlyphProperties.push_back(d);
    }

    return dwriteShapingGlyphProperties;
}

IFACEMETHODIMP CanvasTextAnalyzer::GetJustificationOpportunities(
    CanvasCharacterRange characterRange,
    ICanvasFontFace* fontFace,
    float fontSize,
    CanvasAnalyzedScript script,
    uint32_t clusterMapIndicesCount,
    int* clusterMapIndicesElements,
    uint32_t glyphShapingResultsCount,
    CanvasGlyphShaping* glyphShapingResultsElements,
    uint32_t* valueCount,
    CanvasJustificationOpportunity** valueElements)
{
    return ExceptionBoundary(
        [&]
        {
            // Null font face is allowed. It means, use the default font face.

            CheckInPointer(clusterMapIndicesElements);
            CheckInPointer(glyphShapingResultsElements);
            CheckAndClearOutPointer(valueElements);

            ThrowIfNegative(characterRange.CharacterIndex);
            ThrowIfNegative(characterRange.CharacterCount);

            ComPtr<IDWriteFontFace> dwriteFontFace;
            if (fontFace)
                dwriteFontFace = As<ICanvasFontFaceInternal>(fontFace)->GetRealizedFontFace();

            uint32_t textLength;
            wchar_t const* text = WindowsGetStringRawBuffer(m_text, &textLength);

            ThrowIfInvalidCharacterRange(textLength, characterRange);

            text += characterRange.CharacterIndex;

            auto dwriteClusterMap = GetDWriteClusterMap(clusterMapIndicesCount, clusterMapIndicesElements);
            if (static_cast<int>(clusterMapIndicesCount) != characterRange.CharacterCount)
                ThrowHR(E_INVALIDARG);

            auto dwriteShapingGlyphProperties = GetDWriteGlyphShapingProperties(glyphShapingResultsCount, glyphShapingResultsElements);

            //
            // We infer the glyph count from the size of the glyph shaping array.
            // This is consistent with what DWrite expects.
            //
            // DWrite does not expand text positions into glyphs with this
            // method; rather, it takes your word that the shaping array was
            // produced from valid glyph data. 
            //
            // It does, however, validate that the cluster map is well-formed.
            //
            const uint32_t glyphCount = glyphShapingResultsCount;

            std::vector<DWRITE_JUSTIFICATION_OPPORTUNITY> dwriteJustificationOpportunities;
            dwriteJustificationOpportunities.resize(glyphCount);

            ThrowIfFailed(m_customFontManager->GetTextAnalyzer()->GetJustificationOpportunities(
                dwriteFontFace.Get(),
                fontSize,
                ToDWriteScriptAnalysis(script),
                characterRange.CharacterCount,
                glyphCount,
                text,
                dwriteClusterMap.data(),
                dwriteShapingGlyphProperties.data(),
                dwriteJustificationOpportunities.data()));

            auto output = TransformToComArray<CanvasJustificationOpportunity>(dwriteJustificationOpportunities.begin(), dwriteJustificationOpportunities.end(),
                [](DWRITE_JUSTIFICATION_OPPORTUNITY value)
                {
                    CanvasJustificationOpportunity o;
                    o.AllowResidualCompression = value.allowResidualCompression;
                    o.AllowResidualExpansion = value.allowResidualExpansion;
                    o.ApplyToLeadingEdge = value.applyToLeadingEdge;
                    o.ApplyToTrailingEdge = value.applyToTrailingEdge;
                    o.CompressionMaximum = value.compressionMaximum;
                    o.CompressionPriority = value.compressionPriority;
                    o.ExpansionMaximum = value.expansionMaximum;
                    o.ExpansionMinimum = value.expansionMinimum;
                    o.ExpansionPriority = value.expansionPriority;

                    return o;
                });
            output.Detach(valueCount, valueElements);
        });
}

IFACEMETHODIMP CanvasTextAnalyzer::ApplyJustificationOpportunities(
    float lineWidth,
    uint32_t justificationOpportunitiesCount,
    CanvasJustificationOpportunity* justificationOpportunitiesElements,
    uint32_t sourceGlyphsElementsCount,
    CanvasGlyph* sourceGlyphsElements,
    uint32_t* valueCount,
    CanvasGlyph** valueElements)
{
    return ExceptionBoundary(
        [&]
        {
            const uint32_t glyphCount = sourceGlyphsElementsCount;

            std::vector<DWRITE_JUSTIFICATION_OPPORTUNITY> dwriteJustificationOpportunities;
            dwriteJustificationOpportunities.resize(justificationOpportunitiesCount);
            for (uint32_t i = 0; i < justificationOpportunitiesCount; i++)
            {
                dwriteJustificationOpportunities[i].allowResidualCompression = justificationOpportunitiesElements[i].AllowResidualCompression;
                dwriteJustificationOpportunities[i].allowResidualExpansion = justificationOpportunitiesElements[i].AllowResidualExpansion;
                dwriteJustificationOpportunities[i].applyToLeadingEdge = justificationOpportunitiesElements[i].ApplyToLeadingEdge;
                dwriteJustificationOpportunities[i].applyToTrailingEdge = justificationOpportunitiesElements[i].ApplyToTrailingEdge;
                dwriteJustificationOpportunities[i].compressionMaximum = justificationOpportunitiesElements[i].CompressionMaximum;
                dwriteJustificationOpportunities[i].compressionPriority = justificationOpportunitiesElements[i].CompressionPriority;
                dwriteJustificationOpportunities[i].expansionMaximum = justificationOpportunitiesElements[i].ExpansionMaximum;
                dwriteJustificationOpportunities[i].expansionMinimum = justificationOpportunitiesElements[i].ExpansionMinimum;
                dwriteJustificationOpportunities[i].expansionPriority = justificationOpportunitiesElements[i].ExpansionPriority;
            }

            auto dwriteGlyphData = GetDWriteGlyphData(
                glyphCount, 
                sourceGlyphsElements, 
                static_cast<int>(DWriteGlyphField::Advances) | static_cast<int>(DWriteGlyphField::Offsets));

            std::vector<float> newDwriteGlyphAdvances;
            std::vector<DWRITE_GLYPH_OFFSET> newDwriteGlyphOffsets;
            newDwriteGlyphAdvances.resize(glyphCount);
            newDwriteGlyphOffsets.resize(glyphCount);

            ThrowIfFailed(m_customFontManager->GetTextAnalyzer()->JustifyGlyphAdvances(
                lineWidth,
                glyphCount,
                dwriteJustificationOpportunities.data(),
                dwriteGlyphData.Advances.data(),
                dwriteGlyphData.Offsets.data(),
                newDwriteGlyphAdvances.data(),
                newDwriteGlyphOffsets.data()));

            ComArray<CanvasGlyph> newGlyphs(glyphCount);
            for (uint32_t i = 0; i < glyphCount; ++i)
            {
                newGlyphs[i].Advance = newDwriteGlyphAdvances[i];
                newGlyphs[i].AdvanceOffset = newDwriteGlyphOffsets[i].advanceOffset;
                newGlyphs[i].AscenderOffset = newDwriteGlyphOffsets[i].ascenderOffset;
                newGlyphs[i].Index = sourceGlyphsElements[i].Index;
            }
            newGlyphs.Detach(valueCount, valueElements);
        });
}

IFACEMETHODIMP CanvasTextAnalyzer::AddGlyphsAfterJustification(
    ICanvasFontFace* fontFace,
    float fontSize,
    CanvasAnalyzedScript script,
    uint32_t clusterMapIndicesCount,
    int* clusterMapIndicesElements,
    uint32_t originalGlyphsCount,
    CanvasGlyph* originalGlyphsElements,
    uint32_t justifiedGlyphsCount,
    CanvasGlyph* justifiedGlyphsElements,
    uint32_t glyphShapingResultsCount,
    CanvasGlyphShaping* glyphShapingResultsElements,
    uint32_t* valueCount,
    CanvasGlyph** valueElements)
{
    return AddGlyphsAfterJustificationWithClusterMap(
        fontFace, 
        fontSize, 
        script, 
        clusterMapIndicesCount, 
        clusterMapIndicesElements,
        originalGlyphsCount, 
        originalGlyphsElements, 
        justifiedGlyphsCount, 
        justifiedGlyphsElements, 
        glyphShapingResultsCount, 
        glyphShapingResultsElements, 
        nullptr, 
        nullptr, 
        valueCount, 
        valueElements);
}

IFACEMETHODIMP CanvasTextAnalyzer::AddGlyphsAfterJustificationWithClusterMap(
    ICanvasFontFace* fontFace,
    float fontSize,
    CanvasAnalyzedScript script,
    uint32_t clusterMapIndicesCount,
    int* clusterMapIndicesElements,
    uint32_t originalGlyphsCount,
    CanvasGlyph* originalGlyphsElements,
    uint32_t justifiedGlyphsCount,
    CanvasGlyph* justifiedGlyphsElements,
    uint32_t glyphShapingResultsCount,
    CanvasGlyphShaping* glyphShapingResultsElements,
    uint32_t* outputClusterMapIndicesCount,
    int** outputClusterMapIndicesElements,
    uint32_t* valueCount,
    CanvasGlyph** valueElements)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(fontFace);

            auto dwriteFontFace = As<ICanvasFontFaceInternal>(fontFace)->GetRealizedFontFace();

            uint32_t textLength = clusterMapIndicesCount;

            auto dwriteClusterMap = GetDWriteClusterMap(clusterMapIndicesCount, clusterMapIndicesElements);

            auto dwriteGlyphData = GetDWriteGlyphData(
                originalGlyphsCount, 
                originalGlyphsElements, 
                static_cast<int>(DWriteGlyphField::Advances) | static_cast<int>(DWriteGlyphField::Indices));

            auto dwriteJustifiedGlyphData = GetDWriteGlyphData(
                justifiedGlyphsCount, 
                justifiedGlyphsElements, 
                static_cast<int>(DWriteGlyphField::Advances) | static_cast<int>(DWriteGlyphField::Offsets));

            auto dwriteShapingGlyphProperties = GetDWriteGlyphShapingProperties(glyphShapingResultsCount, glyphShapingResultsElements);

            std::vector<uint16_t> modifiedClusterMap;
            modifiedClusterMap.resize(textLength);

            std::vector<uint16_t> modifiedGlyphIndices;
            std::vector<float> modifiedGlyphAdvances;
            std::vector<DWRITE_GLYPH_OFFSET> modifiedGlyphOffsets;

            uint32_t actualGlyphCount{};
            RetryWithIncreasingGlyphCount(
                textLength,
                [&](uint32_t maxGlyphCount)
                {
                    modifiedGlyphIndices.resize(maxGlyphCount);
                    modifiedGlyphAdvances.resize(maxGlyphCount);
                    modifiedGlyphOffsets.resize(maxGlyphCount);

                    return m_customFontManager->GetTextAnalyzer()->GetJustifiedGlyphs(
                        dwriteFontFace.Get(),
                        fontSize,
                        ToDWriteScriptAnalysis(script),
                        textLength,
                        originalGlyphsCount,
                        maxGlyphCount,
                        dwriteClusterMap.data(),
                        dwriteGlyphData.Indices.data(),
                        dwriteGlyphData.Advances.data(),
                        dwriteJustifiedGlyphData.Advances.data(),
                        dwriteJustifiedGlyphData.Offsets.data(),
                        dwriteShapingGlyphProperties.data(),
                        &actualGlyphCount,
                        modifiedClusterMap.data(),
                        modifiedGlyphIndices.data(),
                        modifiedGlyphAdvances.data(),
                        modifiedGlyphOffsets.data());
                });

            if (outputClusterMapIndicesElements)
            {
                auto clusterMapResult = TransformToComArray<int>(modifiedClusterMap.begin(), modifiedClusterMap.end(),
                    [](uint16_t value)
                    {
                        return static_cast<int>(value);
                    });
                clusterMapResult.Detach(outputClusterMapIndicesCount, outputClusterMapIndicesElements);
            }

            ComArray<CanvasGlyph> newGlyphs(actualGlyphCount);
            for (uint32_t i = 0; i < actualGlyphCount; ++i)
            {
                newGlyphs[i].Advance = modifiedGlyphAdvances[i];
                newGlyphs[i].AdvanceOffset = modifiedGlyphOffsets[i].advanceOffset;
                newGlyphs[i].AscenderOffset = modifiedGlyphOffsets[i].ascenderOffset;
                newGlyphs[i].Index = modifiedGlyphIndices[i];
            }
            newGlyphs.Detach(valueCount, valueElements);
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