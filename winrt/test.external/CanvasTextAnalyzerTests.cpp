// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

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
    TEST_METHOD(CanvasTextAnalyzer_ChooseFonts_ZeroLengthString_NothingBadHappens)
    {
        auto textFormat = ref new CanvasTextFormat();

        auto analyzer1 = ref new CanvasTextAnalyzer(L"", CanvasTextDirection::LeftToRightThenTopToBottom);

        auto analyzer2 = ref new CanvasTextAnalyzer(L"", CanvasTextDirection::LeftToRightThenTopToBottom, nullptr, CanvasVerticalGlyphOrientation::Default, 0);

        auto analyzer3 = ref new CanvasTextAnalyzer(L"", CanvasTextDirection::LeftToRightThenTopToBottom, ref new AnalyzerOptions);

        auto result = analyzer1->ChooseFonts(textFormat);
        result = analyzer2->ChooseFonts(textFormat);
        result = analyzer3->ChooseFonts(textFormat);
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

        Assert::AreEqual(0, result->GetAt(0)->CharacterIndex);
        Assert::AreEqual(10, result->GetAt(0)->CharacterCount);

        Assert::AreEqual(10, result->GetAt(1)->CharacterIndex);
        Assert::AreEqual(2, result->GetAt(1)->CharacterCount);

        Assert::AreEqual(12, result->GetAt(2)->CharacterIndex);
        Assert::AreEqual(6, result->GetAt(2)->CharacterCount); // Each symbolic glyph is two code units
    }

    TEST_METHOD(CanvasTextAnalyzer_ChooseFonts_GlyphDoesntMatchAnyFont)
    {
        auto textFormat = ref new CanvasTextFormat();

        // 
        // Choose a very obscure glyph (U+1F984) which isn't supported by built-in fonts.
        //
        auto analyzer = ref new CanvasTextAnalyzer(L"🦄", CanvasTextDirection::LeftToRightThenTopToBottom);

        auto result = analyzer->ChooseFonts(textFormat);

        Assert::AreEqual(0u, result->Size);
    }

    TEST_METHOD(CanvasTextAnalyzer_ChooseFonts_TextContainsUnmatchableRange)
    {
        auto textFormat = ref new CanvasTextFormat();

        auto analyzer = ref new CanvasTextAnalyzer(L"abc🦄🦄def", CanvasTextDirection::LeftToRightThenTopToBottom);

        auto result = analyzer->ChooseFonts(textFormat);

        Assert::AreEqual(2u, result->Size);

        Assert::AreEqual(0, result->GetAt(0)->CharacterIndex);
        Assert::AreEqual(3, result->GetAt(0)->CharacterCount);

        // Each glyph in the unmatchable range is two code units.

        Assert::AreEqual(7, result->GetAt(1)->CharacterIndex);
        Assert::AreEqual(3, result->GetAt(1)->CharacterCount);
    }
};
