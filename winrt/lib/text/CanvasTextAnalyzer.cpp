// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

#include "CanvasTextAnalyzer.h"
#include "CanvasFontSet.h"
#include "CanvasCharacterRangeFont.h"

using namespace ABI::Microsoft::Graphics::Canvas;
using namespace ABI::Microsoft::Graphics::Canvas::Text;

class DWriteTextAnalysisSource : public RuntimeClass<RuntimeClassFlags<ClassicCom>, IDWriteTextAnalysisSource1>,
    private LifespanTracker<DWriteTextAnalysisSource>
{
    WinString m_text;
    uint32_t m_textLength;

    std::wstring m_localeName;
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
        std::wstring&& localeName,
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
                    *localeName = m_localeName.c_str();

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
    IVectorView<CanvasCharacterRangeFont*>** result)
{
    return ChooseFonts(textFormat, nullptr, result);
}


IFACEMETHODIMP CanvasTextAnalyzer::ChooseFonts(
    ICanvasTextFormat* textFormat,
    ICanvasFontSet* requestedFontSet,
    IVectorView<CanvasCharacterRangeFont*>** result)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(textFormat);
            CheckAndClearOutPointer(result);

            auto dwriteTextFormat = GetWrappedResource<IDWriteTextFormat>(textFormat);

            std::wstring localeNameString;
            localeNameString.resize(dwriteTextFormat->GetLocaleNameLength() + 1);
            ThrowIfFailed(dwriteTextFormat->GetLocaleName(&localeNameString[0], static_cast<uint32_t>(localeNameString.size())));

            uint32_t textLength;
            WindowsGetStringRawBuffer(m_text, &textLength);

            auto dwriteTextAnalysisSource = Make<DWriteTextAnalysisSource>(
                m_text,
                textLength,
                std::move(localeNameString),
                m_textDirection,
                m_source,
                m_defaultNumberSubstitution,
                m_defaultVerticalGlyphOrientation,
                m_defaultBidiLevel);
            CheckMakeResult(dwriteTextAnalysisSource);

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

            auto vector = Make<Vector<CanvasCharacterRangeFont*>>();

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
                    auto characterRangeFont = Make<CanvasCharacterRangeFont>(characterIndex, mappedLength, canvasFontFace.Get(), scaleFactor);

                    ThrowIfFailed(vector->Append(characterRangeFont.Get()));
                }

                characterIndex += mappedLength;
                charactersLeft -= mappedLength;
            }

            ThrowIfFailed(vector->GetView(result));
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