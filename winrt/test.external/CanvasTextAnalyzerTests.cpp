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

ref class SimpleAnalyzerOptions sealed : public ICanvasTextAnalyzerOptions
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

        auto analyzer3 = ref new CanvasTextAnalyzer(L"", CanvasTextDirection::LeftToRightThenTopToBottom, ref new SimpleAnalyzerOptions);

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

    TEST_METHOD(CanvasTextAnalyzer_InvalidCharacterRanges)
    {
        auto analyzer = ref new CanvasTextAnalyzer(L"abcd", CanvasTextDirection::LeftToRightThenTopToBottom);

        CanvasCharacterRange spans[] =
        {
            { 0, -1 }, // Contains negative number
            { -1, 0 }, // 
            { 0, 5 }, // Goes past the end
            { 4, 1 }, // Starts past the end
        };

        Platform::Array<int>^ clusterMap = { 0, 1, 2, 3 };
        Platform::Array<CanvasGlyphShaping>^ glyphShaping = { CanvasGlyphShaping{}, CanvasGlyphShaping{}, CanvasGlyphShaping{}, CanvasGlyphShaping{} }; 

        for (auto span : spans)
        {     
            Assert::ExpectException<Platform::InvalidArgumentException^>(
                [&]
                {
                    analyzer->GetGlyphs(span, m_defaultFontFace, 10.0f, false, false, m_defaultScript);
                });
            
            Assert::ExpectException<Platform::InvalidArgumentException^>(
                [&]
                {
                    analyzer->GetJustificationOpportunities(span, m_defaultFontFace, 1.0f, m_defaultScript, clusterMap, glyphShaping);
                });
        }
    }


    TEST_METHOD(CanvasTextAnalyzer_AnalyzeBidi_ZeroLengthText)
    {
        auto analyzer = ref new CanvasTextAnalyzer(L"", CanvasTextDirection::LeftToRightThenTopToBottom);

        auto result = analyzer->AnalyzeBidi();
        Assert::AreEqual(0u, result->Size);
    }

    TEST_METHOD(CanvasTextAnalyzer_AnalyzeBidi_MultipleSpans)
    {
        auto analyzer = ref new CanvasTextAnalyzer(L"abcنصj", CanvasTextDirection::LeftToRightThenTopToBottom);

        auto result = analyzer->AnalyzeBidi();
        Assert::AreEqual(3u, result->Size);
        
        Assert::AreEqual(0, result->GetAt(0)->Key.CharacterIndex);
        Assert::AreEqual(3, result->GetAt(0)->Key.CharacterCount);
        Assert::AreEqual(0u, result->GetAt(0)->Value.ResolvedLevel);

        Assert::AreEqual(3, result->GetAt(1)->Key.CharacterIndex);
        Assert::AreEqual(2, result->GetAt(1)->Key.CharacterCount);
        Assert::AreEqual(1u, result->GetAt(1)->Value.ResolvedLevel);

        Assert::AreEqual(5, result->GetAt(2)->Key.CharacterIndex);
        Assert::AreEqual(1, result->GetAt(2)->Key.CharacterCount);
        Assert::AreEqual(0u, result->GetAt(2)->Value.ResolvedLevel);
    }

    TEST_METHOD(CanvasTextAnalyzer_AnalyzeBidi_RepeatedCalls)
    {
        auto analyzer = ref new CanvasTextAnalyzer(L"aنbن", CanvasTextDirection::LeftToRightThenTopToBottom);

        auto result = analyzer->AnalyzeBidi();
        Assert::AreEqual(4u, result->Size);

        for (int i = 0; i < 4; i++)
        {
            Assert::AreEqual(i, result->GetAt(i)->Key.CharacterIndex);
            Assert::AreEqual(1, result->GetAt(i)->Key.CharacterCount);
            Assert::AreEqual(static_cast<uint32_t>(i % 2), result->GetAt(i)->Value.ResolvedLevel);
        }

        result = analyzer->AnalyzeBidi();
        Assert::AreEqual(4u, result->Size);

        for (int i = 0; i < 4; i++)
        {
            Assert::AreEqual(i, result->GetAt(i)->Key.CharacterIndex);
            Assert::AreEqual(1, result->GetAt(i)->Key.CharacterCount);
            Assert::AreEqual(static_cast<uint32_t>(i % 2), result->GetAt(i)->Value.ResolvedLevel);
        }
    }


    TEST_METHOD(CanvasTextAnalyzer_AnalyzeBreakpoints_ZeroLengthText)
    {
        auto analyzer = ref new CanvasTextAnalyzer(L"", CanvasTextDirection::LeftToRightThenTopToBottom);

        auto result = analyzer->AnalyzeBreakpoints();
        Assert::AreEqual(0u, result->Length);
    }

    TEST_METHOD(CanvasTextAnalyzer_AnalyzeBreakpoints_SingleGlyph)
    {
        struct TestCase
        {
            Platform::String^ TestString;
            CanvasLineBreakCondition ExpectedBreakBefore;
            CanvasLineBreakCondition ExpectedBreakAfter;
            bool ExpectedIsWhitespace;
            bool ExpectedIsSoftHyphen;
        } testCases[] {
            { L"a", CanvasLineBreakCondition::CannotBreak, CanvasLineBreakCondition::CanBreak, false, false },
            { L" ", CanvasLineBreakCondition::CannotBreak, CanvasLineBreakCondition::CanBreak, true, false },
            { L"\x00AD", CanvasLineBreakCondition::CannotBreak, CanvasLineBreakCondition::CanBreak, false, true }, // Soft hyphen
            { L"\n", CanvasLineBreakCondition::CannotBreak, CanvasLineBreakCondition::MustBreak, true, false },
        };

        for (auto testCase : testCases)
        {
            auto analyzer = ref new CanvasTextAnalyzer(testCase.TestString, CanvasTextDirection::LeftToRightThenTopToBottom);
            auto result = analyzer->AnalyzeBreakpoints();

            Assert::AreEqual(1u, result->Length);
            Assert::AreEqual(testCase.ExpectedBreakBefore, result[0].BreakBefore);
            Assert::AreEqual(testCase.ExpectedBreakAfter, result[0].BreakAfter);
            Assert::AreEqual(testCase.ExpectedIsWhitespace, result[0].IsWhitespace);
            Assert::AreEqual(testCase.ExpectedIsSoftHyphen, result[0].IsSoftHyphen);
        }
    }

    TEST_METHOD(CanvasTextAnalyzer_AnalyzeBreakpoints_TypicalBreaks)
    {
        auto analyzer = ref new CanvasTextAnalyzer(L"a bc\nd", CanvasTextDirection::LeftToRightThenTopToBottom);
        auto result = analyzer->AnalyzeBreakpoints();

        Assert::AreEqual(6u, result->Length);
        Assert::AreEqual(CanvasLineBreakCondition::CannotBreak, result[0].BreakBefore);
        Assert::AreEqual(CanvasLineBreakCondition::CannotBreak, result[0].BreakAfter);

        Assert::AreEqual(CanvasLineBreakCondition::CannotBreak, result[1].BreakBefore);
        Assert::AreEqual(CanvasLineBreakCondition::CanBreak, result[1].BreakAfter); // Space causes a break opportunity

        Assert::AreEqual(CanvasLineBreakCondition::CanBreak, result[2].BreakBefore);
        Assert::AreEqual(CanvasLineBreakCondition::CannotBreak, result[2].BreakAfter);

        Assert::AreEqual(CanvasLineBreakCondition::CannotBreak, result[3].BreakBefore);
        Assert::AreEqual(CanvasLineBreakCondition::CannotBreak, result[3].BreakAfter);

        Assert::AreEqual(CanvasLineBreakCondition::CannotBreak, result[4].BreakBefore);
        Assert::AreEqual(CanvasLineBreakCondition::MustBreak, result[4].BreakAfter); // From newline

        Assert::AreEqual(CanvasLineBreakCondition::MustBreak, result[5].BreakBefore);
        Assert::AreEqual(CanvasLineBreakCondition::CanBreak, result[5].BreakAfter);
    }

    TEST_METHOD(CanvasTextAnalyzer_AnalyzeBreakpoints_RepeatedCalls)
    {
        auto analyzer = ref new CanvasTextAnalyzer(L"a b ", CanvasTextDirection::LeftToRightThenTopToBottom);
        auto result = analyzer->AnalyzeBreakpoints();

        Assert::AreEqual(4u, result->Length);
        for (int i = 0; i < 4; ++i)
        {
            Assert::AreEqual(i % 2 == 0 ? CanvasLineBreakCondition::CannotBreak : CanvasLineBreakCondition::CanBreak, result[i].BreakAfter);
        }

        result = analyzer->AnalyzeBreakpoints();

        Assert::AreEqual(4u, result->Length);
        for (int i = 0; i < 4; ++i)
        {
            Assert::AreEqual(i % 2 == 0 ? CanvasLineBreakCondition::CannotBreak : CanvasLineBreakCondition::CanBreak, result[i].BreakAfter);
        }
    }

    TEST_METHOD(CanvasTextAnalyzer_AnalyzeNumberSubstitutions_ZeroLengthText)
    {
        auto analyzer = ref new CanvasTextAnalyzer(L"", CanvasTextDirection::LeftToRightThenTopToBottom);

        auto result = analyzer->AnalyzeNumberSubstitutions();
        Assert::AreEqual(0u, result->Size);
    }

    ref class CustomNumberSubstitutions sealed : public ICanvasTextAnalyzerOptions
    {
        CanvasNumberSubstitution^ m_substitutions[2];
    public:
        CustomNumberSubstitutions(Platform::String^ locale)
        {
            m_substitutions[0] = ref new CanvasNumberSubstitution(CanvasNumberSubstitutionMethod::National, locale, false);
            m_substitutions[1] = ref new CanvasNumberSubstitution(CanvasNumberSubstitutionMethod::Traditional, locale, false);
        }
        virtual Platform::String^ GetLocaleName(int characterIndex, int* characterCount)
        {
            //
            // Number substitution analysis doesn't look here for locale. 
            // The locale assigned to the number substitution *object* decides which locale affects
            // substitution for that character, and it takes precedent.
            //
            *characterCount = 0;
            return "";
        }

        virtual CanvasNumberSubstitution^ GetNumberSubstitution(int characterIndex, int* characterCount)
        {
            //
            // This is designed to return a different substitution method for
            // every character- Traditional and National, alternating.
            //
            *characterCount = 1;

            return GetNumberSubstitution(characterIndex);
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

        CanvasNumberSubstitution^ GetNumberSubstitution(int characterIndex)
        {
            return m_substitutions[characterIndex % 2];
        }
    };

    TEST_METHOD(CanvasTextAnalyzer_AnalyzeNumberSubstitutions_AlternatingPerGlyph_RepeatedCalls)
    {
        auto analysisOptions = ref new CustomNumberSubstitutions("ar-eg");

        auto analyzer = ref new CanvasTextAnalyzer(
            L"123456789", 
            CanvasTextDirection::LeftToRightThenTopToBottom, 
            analysisOptions);

        auto result = analyzer->AnalyzeNumberSubstitutions();
        Assert::AreEqual(9u, result->Size);

        for (int i = 0; i < 9; ++i)
        {
            Assert::AreEqual(i, result->GetAt(i)->Key.CharacterIndex);
            Assert::AreEqual(1, result->GetAt(i)->Key.CharacterCount);
            
            auto expected = analysisOptions->GetNumberSubstitution(i);
            auto actual = result->GetAt(i)->Value;
            Assert::AreEqual(expected, actual);
        }

        result = analyzer->AnalyzeNumberSubstitutions();
        Assert::AreEqual(9u, result->Size);

        for (int i = 0; i < 9; ++i)
        {
            Assert::AreEqual(i, result->GetAt(i)->Key.CharacterIndex);
            Assert::AreEqual(1, result->GetAt(i)->Key.CharacterCount);

            auto expected = analysisOptions->GetNumberSubstitution(i);
            auto actual = result->GetAt(i)->Value;
            Assert::AreEqual(expected, actual);
        }
    }

    TEST_METHOD(CanvasTextAnalyzer_AnalyzeGlyphOrientations_ZeroLengthText)
    {
        auto analyzer = ref new CanvasTextAnalyzer(L"", CanvasTextDirection::LeftToRightThenTopToBottom);

        auto result = analyzer->AnalyzeGlyphOrientations();
        Assert::AreEqual(0u, result->Size);
    }


    ref class CustomGlyphOrientations sealed : public ICanvasTextAnalyzerOptions
    {
    public:
        virtual Platform::String^ GetLocaleName(int characterIndex, int* characterCount)
        {
            *characterCount = 1;
            return "";
        }

        virtual CanvasNumberSubstitution^ GetNumberSubstitution(int characterIndex, int* characterCount)
        {
            *characterCount = 1;
            return nullptr;
        }

        virtual CanvasVerticalGlyphOrientation GetVerticalGlyphOrientation(int characterIndex, int* characterCount)
        {
            *characterCount = 1;
            return GetVerticalOrientation(characterIndex);
        }

        virtual uint32_t GetBidiLevel(int characterIndex, int* characterCount)
        {
            *characterCount = 1;
            return 0;
        }

        CanvasVerticalGlyphOrientation GetVerticalOrientation(int index)
        {
            return index % 2 == 0 ? CanvasVerticalGlyphOrientation::Default : CanvasVerticalGlyphOrientation::Stacked;
        }

        CanvasGlyphOrientation GetAngleOrientation(int index)
        {
            auto vertical = GetVerticalOrientation(index);

            if (vertical == CanvasVerticalGlyphOrientation::Default)
                return CanvasGlyphOrientation::Clockwise90Degrees;
            else
                return CanvasGlyphOrientation::Upright; // Stacked vertical Latin text doesn't get rotated.
        }
    };

    TEST_METHOD(CanvasTextAnalyzer_AnalyzeGlyphOrientations_AlternatingPerGlyph_RepeatedCalls)
    {
        auto analysisOptions = ref new CustomGlyphOrientations();

        auto analyzer = ref new CanvasTextAnalyzer(
            L"abcd",
            CanvasTextDirection::TopToBottomThenRightToLeft,
            analysisOptions);

        auto result = analyzer->AnalyzeGlyphOrientations();
        Assert::AreEqual(4u, result->Size);

        for (int i = 0; i < 4; ++i)
        {
            Assert::AreEqual(i, result->GetAt(i)->Key.CharacterIndex);
            Assert::AreEqual(1, result->GetAt(i)->Key.CharacterCount);

            auto expected = analysisOptions->GetAngleOrientation(i);
            auto actual = result->GetAt(i)->Value.GlyphOrientation;
            Assert::AreEqual(expected, actual);
        }

        result = analyzer->AnalyzeGlyphOrientations();
        Assert::AreEqual(4u, result->Size);

        for (int i = 0; i < 4; ++i)
        {
            Assert::AreEqual(i, result->GetAt(i)->Key.CharacterIndex);
            Assert::AreEqual(1, result->GetAt(i)->Key.CharacterCount);

            auto expected = analysisOptions->GetAngleOrientation(i);
            auto actual = result->GetAt(i)->Value.GlyphOrientation;
            Assert::AreEqual(expected, actual);
        }
    }

    TEST_METHOD(CanvasTextAnalyzer_GetJustificationOpportunities_EmptyArray_ZeroLengthText)
    {
        auto analyzer = ref new CanvasTextAnalyzer(L"", CanvasTextDirection::LeftToRightThenTopToBottom);
        Platform::Array<int>^ clusterMap = {};
        Platform::Array<CanvasGlyphShaping>^ glyphShaping = {};

        Assert::ExpectException<Platform::InvalidArgumentException^>(
            [&]
            {
                // Empty arrays are sent across as null pointers. Ensure expected failure
                analyzer->GetJustificationOpportunities(CanvasCharacterRange{ 0, 0 }, m_defaultFontFace, 1.0f, m_defaultScript, clusterMap, glyphShaping);
            });
    }

    TEST_METHOD(CanvasTextAnalyzer_GetJustificationOpportunities_ClusterMapMustBeCorrectLength)
    {
        auto analyzer = ref new CanvasTextAnalyzer(L"A", CanvasTextDirection::LeftToRightThenTopToBottom);
        Platform::Array<int>^ tooLongClusterMap = {0, 1, 2};
        Platform::Array<CanvasGlyphShaping>^ glyphShaping = { CanvasGlyphShaping{} }; // Zeroed-out CanvasGlyphShaping is okay here

        Assert::ExpectException<Platform::InvalidArgumentException^>(
            [&]
            {
                analyzer->GetJustificationOpportunities(CanvasCharacterRange{ 0, 1 }, m_defaultFontFace, 1.0f, m_defaultScript, tooLongClusterMap, glyphShaping);
            });
    }

    TEST_METHOD(CanvasTextAnalyzer_GetJustificationOpportunities_ClusterMapHasABadValue)
    {
        auto analyzer = ref new CanvasTextAnalyzer(L"Ab", CanvasTextDirection::LeftToRightThenTopToBottom);
        Platform::Array<int>^ clusterMap = { 0, -1 };
        Platform::Array<CanvasGlyphShaping>^ glyphShaping = { CanvasGlyphShaping{}, CanvasGlyphShaping{} };

        Assert::ExpectException<Platform::InvalidArgumentException^>(
            [&]
            {
                analyzer->GetJustificationOpportunities(CanvasCharacterRange{ 0, 2 }, m_defaultFontFace, 1.0f, m_defaultScript, clusterMap, glyphShaping);
            });
    }
};
