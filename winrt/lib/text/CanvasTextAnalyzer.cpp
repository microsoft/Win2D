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


IFACEMETHODIMP CanvasTextAnalyzer::ChooseFontsUsingSystemFontSet(
    ICanvasTextFormat* textFormat,
    IVectorView<IKeyValuePair<CanvasCharacterRange, CanvasScaledFont*>*>** result)
{
    return ChooseFonts(textFormat, nullptr, result);
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

            m_dwriteTextAnalysisSource->SetLocaleName(localeNameString);

            ComPtr<IDWriteFontCollection> dwriteFontCollection;

            if (requestedFontSet)
            {
#if WINVER > _WIN32_WINNT_WINBLUE
                auto dwriteFontSet = GetWrappedResource<IDWriteFontSet>(requestedFontSet);
                auto factory = As<IDWriteFactory3>(m_customFontManager->GetSharedFactory());
                ComPtr<IDWriteFontCollection1> dwriteFontCollection1;
                ThrowIfFailed(factory->CreateFontCollectionFromFontSet(dwriteFontSet.Get(), &dwriteFontCollection1));

                dwriteFontCollection = As<IDWriteFontCollection>(dwriteFontCollection1.Get());
#else
                dwriteFontCollection = GetWrappedResource<IDWriteFontCollection>(requestedFontSet);
#endif
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

IFACEMETHODIMP CanvasTextAnalyzer::AnalyzeBidi(
    IVectorView<IKeyValuePair<CanvasCharacterRange, CanvasAnalyzedBidi>*>** values)
{
    return AnalyzeBidiWithLocale(nullptr, values);
}

IFACEMETHODIMP CanvasTextAnalyzer::AnalyzeBidiWithLocale(
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

IFACEMETHODIMP CanvasTextAnalyzer::AnalyzeBreakpoints(
    uint32_t* valueCount,
    CanvasAnalyzedBreakpoint** valueElements)
{
    return AnalyzeBreakpointsWithLocale(nullptr, valueCount, valueElements);
}

IFACEMETHODIMP CanvasTextAnalyzer::AnalyzeBreakpointsWithLocale(
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

IFACEMETHODIMP CanvasTextAnalyzer::AnalyzeNumberSubstitutions(
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
            m_dwriteTextAnalysisSource->SetLocaleName(localeString);

            uint32_t textLength;
            WindowsGetStringRawBuffer(m_text, &textLength);

            ThrowIfFailed(m_customFontManager->GetTextAnalyzer()->AnalyzeScript(m_dwriteTextAnalysisSource.Get(), 0, textLength, m_dwriteTextAnalysisSink.Get()));

            ThrowIfFailed(m_dwriteTextAnalysisSink->GetAnalyzedScript()->GetView(values));
        });
}

IFACEMETHODIMP CanvasTextAnalyzer::AnalyzeGlyphOrientations(
    IVectorView<IKeyValuePair<CanvasCharacterRange, CanvasAnalyzedGlyphOrientation>*>** values)
{
    return AnalyzeGlyphOrientationsWithLocale(nullptr, values);
}
    
IFACEMETHODIMP CanvasTextAnalyzer::AnalyzeGlyphOrientationsWithLocale(
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

                getGlyphsResult = m_customFontManager->GetTextAnalyzer()->GetGlyphs(
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