// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

#include "StubDWriteFontCollection.h"

using namespace Microsoft::Graphics::Canvas;

TEST_CLASS(CanvasTextFormatTests)
{
    ComPtr<IDWriteFactory> m_dwriteFactory;

public:
    CanvasTextFormatTests()
    {
        ThrowIfFailed(DWriteCreateFactory(
            DWRITE_FACTORY_TYPE_SHARED,
            __uuidof(&m_dwriteFactory),
            static_cast<IUnknown**>(&m_dwriteFactory)));
    }

    TEST_METHOD(CanvasTextFormat_Construction)
    {
        auto format = ref new CanvasTextFormat();
    }

    TEST_METHOD(CanvasTextFormat_Interop)
    {
        //
        // Create a dwriteTextFormat with non-default values set for all the
        // properties.  An explicit font collection is set, and this is expected
        // to be thrown away if CanvasTextFormat recreates its underlying
        // IDWriteTextFormat (since custom font collections can be configured using
        // the uri#family syntax in font family.
        //
        auto fontCollection = Make<StubDWriteFontCollection>();

        ComPtr<IDWriteTextFormat> dwriteTextFormatBase;
        ThrowIfFailed(m_dwriteFactory->CreateTextFormat(
            L"Arial",
            fontCollection.Get(),
            DWRITE_FONT_WEIGHT_THIN,
            DWRITE_FONT_STYLE_OBLIQUE,
            DWRITE_FONT_STRETCH_ULTRA_EXPANDED,
            12.0,
            L"locale",
            &dwriteTextFormatBase));
        auto dwriteTextFormat = As<IDWriteTextFormat1>(dwriteTextFormatBase);

        ThrowIfFailed(dwriteTextFormat->SetReadingDirection(DWRITE_READING_DIRECTION_TOP_TO_BOTTOM));
        ThrowIfFailed(dwriteTextFormat->SetFlowDirection(DWRITE_FLOW_DIRECTION_RIGHT_TO_LEFT));
        ThrowIfFailed(dwriteTextFormat->SetIncrementalTabStop(12.0f));
        ThrowIfFailed(dwriteTextFormat->SetLineSpacing(DWRITE_LINE_SPACING_METHOD_UNIFORM, 2.0f, 4.0f));
        ThrowIfFailed(dwriteTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_FAR));
        ThrowIfFailed(dwriteTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING));
        ThrowIfFailed(dwriteTextFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_EMERGENCY_BREAK));

        auto trimming = DWRITE_TRIMMING{ DWRITE_TRIMMING_GRANULARITY_WORD, L'/', 1 };
        ThrowIfFailed(dwriteTextFormat->SetTrimming(&trimming, nullptr));

        ThrowIfFailed(dwriteTextFormat->SetVerticalGlyphOrientation(DWRITE_VERTICAL_GLYPH_ORIENTATION_STACKED));
        ThrowIfFailed(dwriteTextFormat->SetOpticalAlignment(DWRITE_OPTICAL_ALIGNMENT_NO_SIDE_BEARINGS));
        ThrowIfFailed(dwriteTextFormat->SetLastLineWrapping(false));

        //
        // Get a CanvasTextFormat based on the IDWriteTextFormat
        //
        auto canvasTextFormat = GetOrCreate<CanvasTextFormat>(dwriteTextFormat.Get());

        auto wrappedTextFormat = GetWrappedResource<IDWriteTextFormat>(canvasTextFormat);
        Assert::AreEqual<void*>(dwriteTextFormat.Get(), wrappedTextFormat.Get());

        //
        // Repeated lookups should be idempotent
        //
        auto canvasTextFormat2 = GetOrCreate<CanvasTextFormat>(dwriteTextFormat.Get());

        Assert::AreEqual(canvasTextFormat, canvasTextFormat2);

        //
        // The Options property is special - this is stored in the Win2D object rather
        // than as part of the underlying IDWriteTextFormat. To test this properly,
        // we must set it to something other than the default value.
        //
        canvasTextFormat->Options = CanvasDrawTextOptions::EnableColorFont;

        //
        // Verify that it shows the right values
        //

#define CHECK(PROPERTY, EXPECTED) \
        Assert::IsTrue(canvasTextFormat->PROPERTY == EXPECTED)

        CHECK(Direction,                CanvasTextDirection::TopToBottomThenRightToLeft);
        CHECK(FontFamily,               L"Arial");
        CHECK(FontSize,                 12.0f);
        CHECK(FontStretch,              Windows::UI::Text::FontStretch::UltraExpanded);
        CHECK(FontStyle,                Windows::UI::Text::FontStyle::Oblique);
        CHECK(FontWeight,               Windows::UI::Text::FontWeight { 100 } );
        CHECK(IncrementalTabStop,       12.0f);
        CHECK(LineSpacing,              2.0f);
        CHECK(LineSpacingBaseline,      4.0f);
        CHECK(LocaleName,               L"locale");
        CHECK(VerticalAlignment,        CanvasVerticalAlignment::Bottom);
        CHECK(HorizontalAlignment,      CanvasHorizontalAlignment::Right);
        CHECK(TrimmingGranularity,      CanvasTextTrimmingGranularity::Word);
        CHECK(TrimmingDelimiter,        L"/");
        CHECK(TrimmingDelimiterCount,   1); 
        CHECK(WordWrapping,             CanvasWordWrapping::EmergencyBreak);
        CHECK(Options,                  CanvasDrawTextOptions::EnableColorFont);
        CHECK(VerticalGlyphOrientation, CanvasVerticalGlyphOrientation::Stacked);
        CHECK(OpticalAlignment,         CanvasOpticalAlignment::NoSideBearings);
        CHECK(LastLineWrapping,         false);

        //
        // Set properties on the underlying dwrite object and confirm the
        // changes are shown in the reflected object.
        //

        ThrowIfFailed(dwriteTextFormat->SetReadingDirection(DWRITE_READING_DIRECTION_BOTTOM_TO_TOP));
        ThrowIfFailed(dwriteTextFormat->SetFlowDirection(DWRITE_FLOW_DIRECTION_RIGHT_TO_LEFT));
        ThrowIfFailed(dwriteTextFormat->SetIncrementalTabStop(16.0f));
        ThrowIfFailed(dwriteTextFormat->SetLineSpacing(DWRITE_LINE_SPACING_METHOD_DEFAULT, 5.0f, 7.0f));
        ThrowIfFailed(dwriteTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER));
        ThrowIfFailed(dwriteTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER));
        ThrowIfFailed(dwriteTextFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP));

        trimming = DWRITE_TRIMMING{ DWRITE_TRIMMING_GRANULARITY_CHARACTER, L'!', 2 };
        ThrowIfFailed(dwriteTextFormat->SetTrimming(&trimming, nullptr));

        ThrowIfFailed(dwriteTextFormat->SetVerticalGlyphOrientation(DWRITE_VERTICAL_GLYPH_ORIENTATION_DEFAULT));
        ThrowIfFailed(dwriteTextFormat->SetOpticalAlignment(DWRITE_OPTICAL_ALIGNMENT_NONE));
        ThrowIfFailed(dwriteTextFormat->SetLastLineWrapping(true));

        CHECK(Direction, CanvasTextDirection::BottomToTopThenRightToLeft);
        CHECK(IncrementalTabStop, 16.0f);
        CHECK(LineSpacing, -5.0f);
        CHECK(LineSpacingBaseline, 7.0f);
        CHECK(VerticalAlignment, CanvasVerticalAlignment::Center);
        CHECK(HorizontalAlignment, CanvasHorizontalAlignment::Center);
        CHECK(WordWrapping, CanvasWordWrapping::NoWrap);
        CHECK(TrimmingGranularity, CanvasTextTrimmingGranularity::Character);
        CHECK(TrimmingDelimiter, L"!");
        CHECK(TrimmingDelimiterCount, 2);

        CHECK(VerticalGlyphOrientation, CanvasVerticalGlyphOrientation::Default);
        CHECK(OpticalAlignment,         CanvasOpticalAlignment::Default);
        CHECK(LastLineWrapping,         true);

        //
        // Set a Win2D property back to its original value from before the underlying
        // dwrite change, and confirm this is reflected both via Win2D and DWrite.
        //

        canvasTextFormat->IncrementalTabStop = 12.0f;

        CHECK(IncrementalTabStop, 12.0f);

        Assert::AreEqual(12.0f, dwriteTextFormat->GetIncrementalTabStop());

        //
        // Set a property that would cause it to recreate.
        //

        canvasTextFormat->FontFamily = L"Comic Sans MS";

        //
        // Verify that it still shows all the right values.  At this point the
        // shadow values will be being used.
        //

        CHECK(Direction,                CanvasTextDirection::BottomToTopThenRightToLeft);
        CHECK(FontFamily,               L"Comic Sans MS");
        CHECK(FontSize,                 12.0f);
        CHECK(FontStretch,              Windows::UI::Text::FontStretch::UltraExpanded);
        CHECK(FontStyle,                Windows::UI::Text::FontStyle::Oblique);
        CHECK(FontWeight,               Windows::UI::Text::FontWeight { 100 } );
        CHECK(IncrementalTabStop,       12.0f);
        CHECK(LineSpacing,              -5.0f);
        CHECK(LineSpacingBaseline,      7.0f);
        CHECK(LocaleName,               L"locale");
        CHECK(VerticalAlignment,        CanvasVerticalAlignment::Center);
        CHECK(HorizontalAlignment,      CanvasHorizontalAlignment::Center);
        CHECK(TrimmingGranularity,      CanvasTextTrimmingGranularity::Character);
        CHECK(TrimmingDelimiter,        L"!");
        CHECK(TrimmingDelimiterCount,   2); 
        CHECK(WordWrapping,             CanvasWordWrapping::NoWrap);
        CHECK(Options,                  CanvasDrawTextOptions::EnableColorFont);  
        CHECK(VerticalGlyphOrientation, CanvasVerticalGlyphOrientation::Default);
        CHECK(OpticalAlignment,         CanvasOpticalAlignment::Default);
        CHECK(LastLineWrapping,         true);
        CHECK(TrimmingSign,             CanvasTrimmingSign::None);

        //
        // Verify that a new IDWriteTextFormat was indeed created and that it
        // has the correct values set on it.
        //

        auto newDWriteTextFormat = GetWrappedResource<IDWriteTextFormat>(canvasTextFormat);

        Assert::AreNotEqual<void*>(dwriteTextFormat.Get(), newDWriteTextFormat.Get());

        ComPtr<IDWriteFontCollection> newFontCollection;
        ThrowIfFailed(newDWriteTextFormat->GetFontCollection(&newFontCollection));
        
        Assert::AreNotEqual<void*>(fontCollection.Get(), newFontCollection.Get());

        //
        // Verify that after the Win2D object has created a new IDWriteTextFormat,
        // looking up a Win2D wrapper for the original DWrite instance should create
        // a new CanvasTextFormat.
        //

        auto newWrapper = GetOrCreate<CanvasTextFormat>(dwriteTextFormat.Get());

        Assert::AreNotEqual(newWrapper, canvasTextFormat);

#undef CHECK
    }

    TEST_METHOD(CanvasTextFormat_InteropWithInvalidReadingAndFlowDirectionFails)
    {
        AssertIsInvalidCombination(DWRITE_READING_DIRECTION_LEFT_TO_RIGHT, DWRITE_FLOW_DIRECTION_LEFT_TO_RIGHT);
        AssertIsInvalidCombination(DWRITE_READING_DIRECTION_LEFT_TO_RIGHT, DWRITE_FLOW_DIRECTION_RIGHT_TO_LEFT);

        AssertIsInvalidCombination(DWRITE_READING_DIRECTION_RIGHT_TO_LEFT, DWRITE_FLOW_DIRECTION_LEFT_TO_RIGHT);
        AssertIsInvalidCombination(DWRITE_READING_DIRECTION_RIGHT_TO_LEFT, DWRITE_FLOW_DIRECTION_RIGHT_TO_LEFT);

        AssertIsInvalidCombination(DWRITE_READING_DIRECTION_TOP_TO_BOTTOM, DWRITE_FLOW_DIRECTION_TOP_TO_BOTTOM);
        AssertIsInvalidCombination(DWRITE_READING_DIRECTION_TOP_TO_BOTTOM, DWRITE_FLOW_DIRECTION_BOTTOM_TO_TOP);

        AssertIsInvalidCombination(DWRITE_READING_DIRECTION_BOTTOM_TO_TOP, DWRITE_FLOW_DIRECTION_TOP_TO_BOTTOM);
        AssertIsInvalidCombination(DWRITE_READING_DIRECTION_BOTTOM_TO_TOP, DWRITE_FLOW_DIRECTION_BOTTOM_TO_TOP);
    }

    void AssertIsInvalidCombination(DWRITE_READING_DIRECTION readingDirection, DWRITE_FLOW_DIRECTION flowDirection)
    {
        ComPtr<IDWriteTextFormat> invalidDWriteTextFormat;
        ThrowIfFailed(m_dwriteFactory->CreateTextFormat(
            L"Arial",
            nullptr,
            DWRITE_FONT_WEIGHT_THIN,
            DWRITE_FONT_STYLE_OBLIQUE,
            DWRITE_FONT_STRETCH_ULTRA_EXPANDED,
            12.0,
            L"locale",
            &invalidDWriteTextFormat));

        ThrowIfFailed(invalidDWriteTextFormat->SetReadingDirection(readingDirection));            
        ThrowIfFailed(invalidDWriteTextFormat->SetFlowDirection(flowDirection));

        try
        {
            GetOrCreate<CanvasTextFormat>(invalidDWriteTextFormat.Get());
        }
        catch (Platform::Exception^ e)
        {
            Assert::AreEqual(DWRITE_E_FLOWDIRECTIONCONFLICTS, static_cast<HRESULT>(e->HResult));
        }
    }

    TEST_METHOD(CanvasTextFormat_GetSystemFontFamilies_ZeroLocaleArray_DoesntDoAnythingBad)
    {
        auto zeroArray = ref new Platform::Collections::Vector<Platform::String^>();
        CanvasTextFormat::GetSystemFontFamilies(zeroArray->GetView());
    }

    TEST_METHOD(CanvasTextFormat_GetSystemFontFamilies_NullLocaleList)
    {
        auto l1 = CanvasTextFormat::GetSystemFontFamilies(nullptr);
        auto l2 = CanvasTextFormat::GetSystemFontFamilies();

        Assert::AreEqual(l1->Length, l2->Length);
        for (uint32_t i = 0; i < l1->Length; ++i)
        {
            Assert::AreEqual(l1[i], l2[i]);
        }
    }

    TEST_METHOD(CanvasTextFormat_GetSystemFontFamilies_TypicalCaseWorks)
    {
        Platform::Array<Platform::String^>^ families = CanvasTextFormat::GetSystemFontFamilies();
        Assert::IsTrue(families->Length > 0);

        bool arialFound = false;
        for(Platform::String^ name : families)
        {
            if (name == L"Arial")
            {
                arialFound = true;
                break;
            }
        }
        Assert::IsTrue(arialFound);
    }

    TEST_METHOD(CanvasTextFormat_GetSystemFontFamilies_RequestNonEnglishName)
    {
        auto preferZhCn = ref new Platform::Collections::Vector<Platform::String^>(1);
        preferZhCn->SetAt(0, L"zh-cn");

        Platform::Array<Platform::String^>^ families = CanvasTextFormat::GetSystemFontFamilies(preferZhCn->GetView());
        Assert::IsTrue(families->Length > 0);

        bool simSunFound = false;
        for (Platform::String^ name : families)
        {
            if (name == L"宋体")
            {
                simSunFound = true;
                break;
            }
        }
        Assert::IsTrue(simSunFound);
    }

    void VerifyTrimmingState(CanvasTextFormat^ canvasTextFormat)
    {
        Assert::AreEqual(CanvasTextTrimmingGranularity::Character, canvasTextFormat->TrimmingGranularity);
        Assert::AreEqual(L"!", canvasTextFormat->TrimmingDelimiter);
        Assert::AreEqual(2, canvasTextFormat->TrimmingDelimiterCount);
    }

    TEST_METHOD(CanvasTextFormat_Interop_TrimmingSign_None)
    {
        //
        // This is organized out separately from the main interop test, since
        // trimming signs need to use an actual (non-mock) font collection.
        //

        ComPtr<IDWriteFontCollection> fontCollection;
        ThrowIfFailed(m_dwriteFactory->GetSystemFontCollection(&fontCollection));

        ComPtr<IDWriteTextFormat> dwriteTextFormat;
        ThrowIfFailed(m_dwriteFactory->CreateTextFormat(
            L"Arial",
            fontCollection.Get(),
            DWRITE_FONT_WEIGHT_THIN,
            DWRITE_FONT_STYLE_OBLIQUE,
            DWRITE_FONT_STRETCH_ULTRA_EXPANDED,
            12.0,
            L"locale",
            &dwriteTextFormat));

        ComPtr<IDWriteInlineObject> ellipsisTrimmingSign;
        ThrowIfFailed(m_dwriteFactory->CreateEllipsisTrimmingSign(dwriteTextFormat.Get(), &ellipsisTrimmingSign));

        auto trimming = DWRITE_TRIMMING{ DWRITE_TRIMMING_GRANULARITY_CHARACTER, L'!', 2 };
        ThrowIfFailed(dwriteTextFormat->SetTrimming(&trimming, ellipsisTrimmingSign.Get()));

        auto canvasTextFormat = GetOrCreate<CanvasTextFormat>(dwriteTextFormat.Get());

        auto wrappedTextFormat = GetWrappedResource<IDWriteTextFormat>(canvasTextFormat);
        Assert::AreEqual<void*>(dwriteTextFormat.Get(), wrappedTextFormat.Get());

        //
        // Even though we manually set a trimming sign, the getter should still report None,
        // and nothing should clobber the trimming state.
        //
        Assert::AreEqual(CanvasTrimmingSign::None, canvasTextFormat->TrimmingSign);
        VerifyTrimmingState(canvasTextFormat);

        //
        // Null out the trimming sign; shouldn't cause any misbehavior.
        //
        ThrowIfFailed(dwriteTextFormat->SetTrimming(&trimming, nullptr));
        Assert::AreEqual(CanvasTrimmingSign::None, canvasTextFormat->TrimmingSign);
        VerifyTrimmingState(canvasTextFormat);

        //
        // Set a property that would cause re-realization.
        //
        canvasTextFormat->FontFamily = L"Comic Sans MS";
        Assert::AreEqual(CanvasTrimmingSign::None, canvasTextFormat->TrimmingSign);
        VerifyTrimmingState(canvasTextFormat);
    }

    TEST_METHOD(CanvasTextFormat_Interop_TrimmingSign_Ellipsis)
    {
        ComPtr<IDWriteFontCollection> fontCollection;
        ThrowIfFailed(m_dwriteFactory->GetSystemFontCollection(&fontCollection));

        ComPtr<IDWriteTextFormat> dwriteTextFormat;
        ThrowIfFailed(m_dwriteFactory->CreateTextFormat(
            L"Arial",
            fontCollection.Get(),
            DWRITE_FONT_WEIGHT_THIN,
            DWRITE_FONT_STYLE_OBLIQUE,
            DWRITE_FONT_STRETCH_ULTRA_EXPANDED,
            12.0,
            L"locale",
            &dwriteTextFormat));

        auto trimming = DWRITE_TRIMMING{ DWRITE_TRIMMING_GRANULARITY_CHARACTER, L'!', 2 };
        ThrowIfFailed(dwriteTextFormat->SetTrimming(&trimming, nullptr));

        auto canvasTextFormat = GetOrCreate<CanvasTextFormat>(dwriteTextFormat.Get());

        auto wrappedTextFormat = GetWrappedResource<IDWriteTextFormat>(canvasTextFormat);
        Assert::AreEqual<void*>(dwriteTextFormat.Get(), wrappedTextFormat.Get());

        //
        // Set the trimming sign to ellipsis. Nnothing should clobber the trimming state.
        //
        canvasTextFormat->TrimmingSign = CanvasTrimmingSign::Ellipsis;
        Assert::AreEqual(CanvasTrimmingSign::Ellipsis, canvasTextFormat->TrimmingSign);
        VerifyTrimmingState(canvasTextFormat);

        //
        // Nulling out the interopped trimming sign should reset the trimming sign property.
        //
        ThrowIfFailed(dwriteTextFormat->SetTrimming(&trimming, nullptr));
        Assert::AreEqual(CanvasTrimmingSign::None, canvasTextFormat->TrimmingSign);
        VerifyTrimmingState(canvasTextFormat);
    }

    TEST_METHOD(CanvasTextFormat_Interop_TrimmingSign_MaintainsShadowPropertyAfterUnrealize)
    {
        ComPtr<IDWriteFontCollection> fontCollection;
        ThrowIfFailed(m_dwriteFactory->GetSystemFontCollection(&fontCollection));

        ComPtr<IDWriteTextFormat> dwriteTextFormat;
        ThrowIfFailed(m_dwriteFactory->CreateTextFormat(
            L"Arial",
            fontCollection.Get(),
            DWRITE_FONT_WEIGHT_THIN,
            DWRITE_FONT_STYLE_OBLIQUE,
            DWRITE_FONT_STRETCH_ULTRA_EXPANDED,
            12.0,
            L"locale",
            &dwriteTextFormat));

        //
        // Set the trimming sign on the wrapper to Ellipsis.
        //
        auto canvasTextFormat = GetOrCreate<CanvasTextFormat>(dwriteTextFormat.Get());
        canvasTextFormat->TrimmingSign = CanvasTrimmingSign::Ellipsis;

        //
        // Null out the trimming sign on the DWrite object.
        //
        auto trimming = DWRITE_TRIMMING{ DWRITE_TRIMMING_GRANULARITY_CHARACTER, L'!', 2 };
        ThrowIfFailed(dwriteTextFormat->SetTrimming(&trimming, nullptr));

        //
        // Do something that unrealizes the text format. In this case, we change the font family
        //
        canvasTextFormat->FontFamily = "Times New Roman";

        //
        // The trimming sign property should 'know' that we nulled it out, despite being unrealized.
        //
        Assert::AreEqual(CanvasTrimmingSign::None, canvasTextFormat->TrimmingSign);
    }

    TEST_METHOD(CanvasTextFormat_Interop_TrimmingSign_Ellipsis_IntropAssignsCustomTrimmingSign)
    {
        ComPtr<IDWriteFontCollection> fontCollection;
        ThrowIfFailed(m_dwriteFactory->GetSystemFontCollection(&fontCollection));

        ComPtr<IDWriteTextFormat> dwriteTextFormat;
        ThrowIfFailed(m_dwriteFactory->CreateTextFormat(
            L"Arial",
            fontCollection.Get(),
            DWRITE_FONT_WEIGHT_THIN,
            DWRITE_FONT_STYLE_OBLIQUE,
            DWRITE_FONT_STRETCH_ULTRA_EXPANDED,
            12.0,
            L"locale",
            &dwriteTextFormat));

        auto canvasTextFormat = GetOrCreate<CanvasTextFormat>(dwriteTextFormat.Get());
        canvasTextFormat->TrimmingSign = CanvasTrimmingSign::Ellipsis;

        ComPtr<IDWriteInlineObject> ellipsisTrimmingSign;
        ThrowIfFailed(m_dwriteFactory->CreateEllipsisTrimmingSign(dwriteTextFormat.Get(), &ellipsisTrimmingSign));

        auto trimming = DWRITE_TRIMMING{ DWRITE_TRIMMING_GRANULARITY_CHARACTER, L'!', 2 };
        ThrowIfFailed(dwriteTextFormat->SetTrimming(&trimming, ellipsisTrimmingSign.Get()));

        //
        // Do something that unrealizes the text format. In this case, we change the font family
        //
        canvasTextFormat->FontFamily = "Times New Roman";

        //
        // The custom sign should still be set to the DWrite object.
        //
        ComPtr<IDWriteInlineObject> actualDWriteTrimmingSign;
        ThrowIfFailed(dwriteTextFormat->GetTrimming(&trimming, &actualDWriteTrimmingSign));
        Assert::IsTrue(IsSameInstance(ellipsisTrimmingSign.Get(), actualDWriteTrimmingSign.Get()));

        //
        // The trimming sign property should honor the fact that we assigned a custom sign, and it isn't ellipsis anymore
        //
        Assert::AreEqual(CanvasTrimmingSign::None, canvasTextFormat->TrimmingSign);
    }
};

