// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

using namespace Platform::Collections;
using namespace Platform::Collections::Details;
using namespace Windows::Foundation::Collections;
using namespace Microsoft::Graphics::Canvas;
using namespace Microsoft::Graphics::Canvas::Text;

TEST_CLASS(CanvasNumberSubstitutionTests)
{
public:
    TEST_METHOD(CanvasNumberSubstitution_Construction)
    {
        auto numberSubstitution = ref new CanvasNumberSubstitution(CanvasNumberSubstitutionMethod::FromCulture, "", false);
    }

    TEST_METHOD(CanvasNumberSubstitution_Interop)
    {
        ComPtr<IDWriteFactory2> factory;
        ThrowIfFailed(DWriteCreateFactory(
            DWRITE_FACTORY_TYPE_SHARED,
            __uuidof(&factory),
            static_cast<IUnknown**>(&factory)));

        ComPtr<IDWriteNumberSubstitution> dwriteNumberSubstitution;
        ThrowIfFailed(factory->CreateNumberSubstitution(
            DWRITE_NUMBER_SUBSTITUTION_METHOD_FROM_CULTURE,
            L"",
            false,
            & dwriteNumberSubstitution));

        auto canvasNumberSubstitution = GetOrCreate<CanvasNumberSubstitution>(dwriteNumberSubstitution.Get());

        auto wrappedNumberSubstitution = GetWrappedResource<IDWriteNumberSubstitution>(canvasNumberSubstitution);

        Assert::AreEqual<void*>(dwriteNumberSubstitution.Get(), wrappedNumberSubstitution.Get());
    }
};

ref class AnalyzerOptions sealed : public ICanvasTextAnalyzerOptions
{
public:
    virtual Platform::String^ GetLocaleName(int characterIndex, int* characterCount)
    {
        *characterCount = 0;
        return "en-us";
    }

    virtual CanvasNumberSubstitution^ GetNumberSubstitution(int characterIndex, int* characterCount)
    {
        *characterCount = 0;
        return nullptr;
    }

    virtual CanvasVerticalGlyphOrientation GetVerticalGlyphOrientation(int characterIndex, int* characterCount)
    {
        *characterCount = 0;
        return CanvasVerticalGlyphOrientation::Default;
    }

    virtual uint32_t GetBidiLevel(int characterIndex, int* characterCount)
    {
        *characterCount = 0;
        return 0;
    }
};

TEST_CLASS(CanvasTextAnalyzerTests)
{
    CanvasTextFormat^ m_defaultTextFormat;
    CanvasFontFace^ m_defaultFontFace;
    CanvasAnalyzedScript m_defaultScript;

public:
    CanvasTextAnalyzerTests()
    {
        m_defaultTextFormat = ref new CanvasTextFormat();

        auto analyzer = ref new CanvasTextAnalyzer(L"text", CanvasTextDirection::LeftToRightThenTopToBottom);

        auto fontResult = analyzer->ChooseFonts(m_defaultTextFormat);
        m_defaultFontFace = fontResult->GetAt(0)->Value->FontFace;

        auto scriptResult = analyzer->AnalyzeScript();
        m_defaultScript = scriptResult->GetAt(0)->Value;
    }

    TEST_METHOD(CanvasTextAnalyzer_ChooseFonts_ZeroLengthString_NothingBadHappens)
    {
        auto analyzer1 = ref new CanvasTextAnalyzer(L"", CanvasTextDirection::LeftToRightThenTopToBottom);

        auto analyzer2 = ref new CanvasTextAnalyzer(L"", CanvasTextDirection::LeftToRightThenTopToBottom, nullptr, CanvasVerticalGlyphOrientation::Default, 0);

        auto analyzer3 = ref new CanvasTextAnalyzer(L"", CanvasTextDirection::LeftToRightThenTopToBottom, ref new AnalyzerOptions);

        auto result = analyzer1->ChooseFonts(m_defaultTextFormat);
        result = analyzer2->ChooseFonts(m_defaultTextFormat);
        result = analyzer3->ChooseFonts(m_defaultTextFormat);
    }

    TEST_METHOD(CanvasTextAnalyzer_ChooseFonts_DividesTextIntoSpans)
    {
        auto textFormat = ref new CanvasTextFormat();
        textFormat->FontFamily = "Arial";

        //
        // Each of Latin, Chinese and symbolic script are matched to different 
        // font faces when matching with Arial family.
        //
        auto analyzer = ref new CanvasTextAnalyzer(L"Some text-文字📖📖📖", CanvasTextDirection::LeftToRightThenTopToBottom);

        auto result = analyzer->ChooseFonts(textFormat);

        Assert::AreEqual(3u, result->Size);

        Assert::AreEqual(0, result->GetAt(0)->Key.CharacterIndex);
        Assert::AreEqual(10, result->GetAt(0)->Key.CharacterCount);

        Assert::AreEqual(10, result->GetAt(1)->Key.CharacterIndex);
        Assert::AreEqual(2, result->GetAt(1)->Key.CharacterCount);

        Assert::AreEqual(12, result->GetAt(2)->Key.CharacterIndex);
        Assert::AreEqual(6, result->GetAt(2)->Key.CharacterCount); // Each symbolic glyph is two code units
    }

    TEST_METHOD(CanvasTextAnalyzer_ChooseFonts_GlyphDoesntMatchAnyFont)
    {
        // 
        // Choose a very obscure glyph (U+1F984) which isn't supported by built-in fonts.
        //
        auto analyzer = ref new CanvasTextAnalyzer(L"🦄", CanvasTextDirection::LeftToRightThenTopToBottom);

        auto result = analyzer->ChooseFonts(m_defaultTextFormat);

        Assert::AreEqual(0u, result->Size);
    }

    TEST_METHOD(CanvasTextAnalyzer_ChooseFonts_TextContainsUnmatchableRange)
    {
        auto analyzer = ref new CanvasTextAnalyzer(L"abc🦄🦄def", CanvasTextDirection::LeftToRightThenTopToBottom);

        auto result = analyzer->ChooseFonts(m_defaultTextFormat);

        Assert::AreEqual(2u, result->Size);

        Assert::AreEqual(0, result->GetAt(0)->Key.CharacterIndex);
        Assert::AreEqual(3, result->GetAt(0)->Key.CharacterCount);

        // Each glyph in the unmatchable range is two code units.

        Assert::AreEqual(7, result->GetAt(1)->Key.CharacterIndex);
        Assert::AreEqual(3, result->GetAt(1)->Key.CharacterCount);
    }

    TEST_METHOD(CanvasTextAnalyzer_AnalyzeScript_DefaultFormatting)
    {
        auto analyzer = ref new CanvasTextAnalyzer(L"abcdef", CanvasTextDirection::LeftToRightThenTopToBottom);

        auto result = analyzer->AnalyzeScript();
        Assert::AreEqual(1u, result->Size);        

        auto scriptProperties = analyzer->GetScriptProperties(result->GetAt(0)->Value);

        Assert::AreEqual("Latn", scriptProperties.IsoScriptCode);
        Assert::AreEqual(215, scriptProperties.IsoScriptNumber);
        Assert::AreEqual(1, scriptProperties.ClusterLookahead);
        Assert::AreEqual(" ", scriptProperties.JustificationCharacter);
        Assert::IsFalse(scriptProperties.RestrictCaretToClusters);
        Assert::IsTrue(scriptProperties.UsesWordDividers);
        Assert::IsTrue(scriptProperties.IsDiscreteWriting);
        Assert::IsFalse(scriptProperties.IsBlockWriting);
        Assert::IsFalse(scriptProperties.IsDistributedWithinCluster);
        Assert::IsFalse(scriptProperties.IsConnectedWriting);
        Assert::IsFalse(scriptProperties.IsCursiveWriting);
    }

    TEST_METHOD(CanvasTextAnalyzer_AnalyzeScript_MultipleSpans)
    {
        auto analyzer = ref new CanvasTextAnalyzer(L"abc文字テクスト", CanvasTextDirection::LeftToRightThenTopToBottom);

        auto result = analyzer->AnalyzeScript();
        Assert::AreEqual(3u, result->Size);

        Assert::AreEqual(0, result->GetAt(0)->Key.CharacterIndex);
        Assert::AreEqual(3, result->GetAt(0)->Key.CharacterCount);
        auto properties0 = analyzer->GetScriptProperties(result->GetAt(0)->Value);
        Assert::AreEqual("Latn", properties0.IsoScriptCode);

        Assert::AreEqual(3, result->GetAt(1)->Key.CharacterIndex);
        Assert::AreEqual(2, result->GetAt(1)->Key.CharacterCount);
        auto properties1 = analyzer->GetScriptProperties(result->GetAt(1)->Value);
        Assert::AreEqual("Hani", properties1.IsoScriptCode);

        Assert::AreEqual(5, result->GetAt(2)->Key.CharacterIndex);
        Assert::AreEqual(4, result->GetAt(2)->Key.CharacterCount);
        auto properties2 = analyzer->GetScriptProperties(result->GetAt(2)->Value);
        Assert::AreEqual("Kana", properties2.IsoScriptCode);
    }

    TEST_METHOD(CanvasTextAnalyzer_AnalyzeScript_ZeroLengthText)
    {
        auto analyzer = ref new CanvasTextAnalyzer(L"", CanvasTextDirection::LeftToRightThenTopToBottom);

        auto result = analyzer->AnalyzeScript();
        Assert::AreEqual(0u, result->Size);
    }

    TEST_METHOD(CanvasTextAnalyzer_GetGlyphs_ZeroLengthText)
    {
        auto zeroLengthAnalyzer = ref new CanvasTextAnalyzer(L"", CanvasTextDirection::LeftToRightThenTopToBottom);

        CanvasCharacterRange characterRange{ 0, 0 };
        auto glyphs = zeroLengthAnalyzer->GetGlyphs(characterRange, m_defaultFontFace, 10.0f, false, false, m_defaultScript);

        Assert::AreEqual(0u, glyphs->Length);
    }

    TEST_METHOD(CanvasTextAnalyzer_GetGlyphs_TypographySpanWithNoFeatures)
    {
        auto analyzer = ref new CanvasTextAnalyzer(L"abc", CanvasTextDirection::LeftToRightThenTopToBottom);

        auto typographyVector = ref new Vector<IKeyValuePair<CanvasCharacterRange, CanvasTypography^>^>;
        typographyVector->Append(ref new KeyValuePair<CanvasCharacterRange, CanvasTypography^>(CanvasCharacterRange{ 0, 3 }, ref new CanvasTypography()));

        Platform::Array<int>^ clusterMap;
        Platform::Array<bool>^ isShapedAloneGlyphs;
        Platform::Array<CanvasGlyphShaping>^ glyphShaping;

        CanvasCharacterRange characterRange{0, 3};

        auto glyphs = analyzer->GetGlyphs(characterRange, m_defaultFontFace, 10.0f, false, false, m_defaultScript, "", nullptr, typographyVector->GetView(), &clusterMap, &isShapedAloneGlyphs, &glyphShaping);
        Assert::AreEqual(3u, glyphs->Length);
    }

    TEST_METHOD(CanvasTextAnalyzer_GetGlyphs_BadTypographySpans)
    {
        auto analyzer = ref new CanvasTextAnalyzer(L"abcd", CanvasTextDirection::LeftToRightThenTopToBottom);

        Platform::Array<int>^ clusterMap;
        Platform::Array<bool>^ isShapedAloneGlyphs;
        Platform::Array<CanvasGlyphShaping>^ glyphShaping;

        CanvasCharacterRange spans1[] =
        {
            { 0, -1 }, // Contains negative number
            { -1, 0 },
            { -2, -2 },
            { 1, 0 }, // Zero-length
            { 1, 4 }, // Starts too late
            { 0, 3 }, // Ends too early
        };

        for (auto span : spans1)
        {     
            Assert::ExpectException<Platform::InvalidArgumentException^>(
                [&]
                {
                    auto typographyVector = ref new Vector<IKeyValuePair<CanvasCharacterRange, CanvasTypography^>^>;
                    typographyVector->Append(ref new KeyValuePair<CanvasCharacterRange, CanvasTypography^>(span, ref new CanvasTypography()));

                    CanvasCharacterRange characterRange{ 0, 4 };
                    analyzer->GetGlyphs(characterRange, m_defaultFontFace, 10.0f, false, false, m_defaultScript, "", nullptr, typographyVector->GetView(), &clusterMap, &isShapedAloneGlyphs, &glyphShaping);
                });
        }

        CanvasCharacterRange spans2[][2] =
        {
            { { 0, 1 }, { -1, 3 } }, // Contains negative number
            { { 0, 2 }, { 1, 3 } }, // Overlap
            { { 2, 2 }, { 0, 2 } }, // Not in order
        };

        for (auto spans : spans2)
        {
            Assert::ExpectException<Platform::InvalidArgumentException^>(
                [&]
                {
                    auto typographyVector = ref new Vector<IKeyValuePair<CanvasCharacterRange, CanvasTypography^>^>;
                    typographyVector->Append(ref new KeyValuePair<CanvasCharacterRange, CanvasTypography^>(spans[0], ref new CanvasTypography()));
                    typographyVector->Append(ref new KeyValuePair<CanvasCharacterRange, CanvasTypography^>(spans[1], ref new CanvasTypography()));
                    
                    CanvasCharacterRange characterRange{0, 4};
                    analyzer->GetGlyphs(characterRange, m_defaultFontFace, 10.0f, false, false, m_defaultScript, "", nullptr, typographyVector->GetView(), &clusterMap, &isShapedAloneGlyphs, &glyphShaping);
                });
        }
    }

    TEST_METHOD(CanvasTextAnalyzer_GetGlyphs_ValidTypographySpans)
    {
        auto analyzer = ref new CanvasTextAnalyzer(L"abcd", CanvasTextDirection::LeftToRightThenTopToBottom);

        Platform::Array<int>^ clusterMap;
        Platform::Array<bool>^ isShapedAloneGlyphs;
        Platform::Array<CanvasGlyphShaping>^ glyphShaping;

        CanvasCharacterRange spans[][2] =
        {
            { { 0, 1 }, { 1, 3 } },
            { { 0, 2 }, { 2, 2 } },
            { { 0, 3 }, { 3, 1 } },
        };

        CanvasCharacterRange characterRange{ 0, 4 };

        for (auto spanCouple : spans)
        {
            auto typographyVector = ref new Vector<IKeyValuePair<CanvasCharacterRange, CanvasTypography^>^>;
            typographyVector->Append(ref new KeyValuePair<CanvasCharacterRange, CanvasTypography^>(spanCouple[0], ref new CanvasTypography()));
            typographyVector->Append(ref new KeyValuePair<CanvasCharacterRange, CanvasTypography^>(spanCouple[1], ref new CanvasTypography()));

            analyzer->GetGlyphs(characterRange, m_defaultFontFace, 10.0f, false, false, m_defaultScript, "", nullptr, typographyVector->GetView(), &clusterMap, &isShapedAloneGlyphs, &glyphShaping);
        }

        CanvasCharacterRange singleCharacterSpans[4] =
        {
            { 0, 1 }, { 1, 1 }, { 2, 1 }, {3, 1}
        };
        {
            auto typographyVector = ref new Vector<IKeyValuePair<CanvasCharacterRange, CanvasTypography^>^>;
            for (int i = 0; i < 4; ++i)
                typographyVector->Append(ref new KeyValuePair<CanvasCharacterRange, CanvasTypography^>(singleCharacterSpans[i], ref new CanvasTypography()));

            analyzer->GetGlyphs(characterRange, m_defaultFontFace, 10.0f, false, false, m_defaultScript, "", nullptr, typographyVector->GetView(), &clusterMap, &isShapedAloneGlyphs, &glyphShaping);
        }
    }
    TEST_METHOD(CanvasTextAnalyzer_GetGlyphs_TypographySpansCanLieOutsideCharacterRange)
    {
        auto analyzer = ref new CanvasTextAnalyzer(L"abcd", CanvasTextDirection::LeftToRightThenTopToBottom);

        Platform::Array<int>^ clusterMap;
        Platform::Array<bool>^ isShapedAloneGlyphs;
        Platform::Array<CanvasGlyphShaping>^ glyphShaping;

        CanvasCharacterRange spans[] =
        {
            { 1, 6 }, { 0, 4 }, { 0, 3 },
        };

        CanvasCharacterRange characterRange{ 1, 2 };

        for (auto span : spans)
        {
            auto typographyVector = ref new Vector<IKeyValuePair<CanvasCharacterRange, CanvasTypography^>^>;
            typographyVector->Append(ref new KeyValuePair<CanvasCharacterRange, CanvasTypography^>(span, ref new CanvasTypography()));

            analyzer->GetGlyphs(characterRange, m_defaultFontFace, 10.0f, false, false, m_defaultScript, "", nullptr, typographyVector->GetView(), &clusterMap, &isShapedAloneGlyphs, &glyphShaping);
        }
    }

    TEST_METHOD(CanvasTextAnalyzer_GetGlyphs_SpanWithCharacterRange)
    {
        //
        // These glyphs with the default script expand to one glyph per text position, so we can use
        // that to verify that the length of the returned glyph array is correct.
        //
        // Note that zero-length spans are valid and return zero glyphs.
        //
        auto analyzer = ref new CanvasTextAnalyzer(L"abcd", CanvasTextDirection::LeftToRightThenTopToBottom);

        CanvasCharacterRange spans[] =
        {
            { 0, 1 }, { 0, 2 }, { 1, 2 }, { 3, 1 }, { 0, 0 }, { 1, 0 }
        };

        for (auto span : spans)
        {
            auto glyphs = analyzer->GetGlyphs(span, m_defaultFontFace, 10.0f, false, false, m_defaultScript);
            Assert::AreEqual(span.CharacterCount, static_cast<int>(glyphs->Length));
        }
    }

    TEST_METHOD(CanvasTextAnalyzer_GetGlyphs_InvalidCharacterRanges)
    {
        auto analyzer = ref new CanvasTextAnalyzer(L"abcd", CanvasTextDirection::LeftToRightThenTopToBottom);

        CanvasCharacterRange spans[] =
        {
            { 0, -1 }, // Contains negative number
            { -1, 0 }, // 
            { 0, 5 }, // Goes past the end
            { 4, 1 }, // Starts past the end
        };

        for (auto span : spans)
        {     
            Assert::ExpectException<Platform::InvalidArgumentException^>(
                [&]
                {
                    analyzer->GetGlyphs(span, m_defaultFontFace, 10.0f, false, false, m_defaultScript);
                });
        }
    }
};
