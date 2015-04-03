// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may
// not use these files except in compliance with the License. You may obtain
// a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
// License for the specific language governing permissions and limitations
// under the License.

#include "pch.h"

#include "StubDWriteFontCollection.h"

using namespace Microsoft::Graphics::Canvas;

TEST_CLASS(CanvasTextFormatTests)
{
    TEST_METHOD(CanvasTextFormat_Construction)
    {
        auto format = ref new CanvasTextFormat();
    }

    TEST_METHOD(CanvasTextFormat_Interop)
    {
        ComPtr<IDWriteFactory2> factory;
        ThrowIfFailed(DWriteCreateFactory(
            DWRITE_FACTORY_TYPE_SHARED,
            __uuidof(&factory),
            static_cast<IUnknown**>(&factory)));

        //
        // Create a dwriteTextFormat with non-default values set for all the
        // properties.  An explicit font collection is set, and this is expected
        // to be thrown away if CanvasTextFormat recreates its underlying
        // IDWriteTextFormat (since custom font collections can be configured using
        // the uri#family syntax in font family.
        //
        auto fontCollection = Make<StubDWriteFontCollection>();

        ComPtr<IDWriteTextFormat> dwriteTextFormat;
        ThrowIfFailed(factory->CreateTextFormat(
            L"Arial",
            fontCollection.Get(),
            DWRITE_FONT_WEIGHT_THIN,
            DWRITE_FONT_STYLE_OBLIQUE,
            DWRITE_FONT_STRETCH_ULTRA_EXPANDED,
            12.0,
            L"locale",
            &dwriteTextFormat));

        ThrowIfFailed(dwriteTextFormat->SetReadingDirection(DWRITE_READING_DIRECTION_TOP_TO_BOTTOM));
        ThrowIfFailed(dwriteTextFormat->SetFlowDirection(DWRITE_FLOW_DIRECTION_RIGHT_TO_LEFT));
        ThrowIfFailed(dwriteTextFormat->SetIncrementalTabStop(12.0f));
        ThrowIfFailed(dwriteTextFormat->SetLineSpacing(DWRITE_LINE_SPACING_METHOD_UNIFORM, 2.0f, 4.0f));
        ThrowIfFailed(dwriteTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_FAR));
        ThrowIfFailed(dwriteTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING));
        ThrowIfFailed(dwriteTextFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_EMERGENCY_BREAK));

        auto trimming = DWRITE_TRIMMING{ DWRITE_TRIMMING_GRANULARITY_WORD, L'/', 1 };
        ThrowIfFailed(dwriteTextFormat->SetTrimming(&trimming, nullptr));

        //
        // Get a CanvasTextFormat based on the IDWriteTextFormat
        //
        auto canvasTextFormat = GetOrCreate<CanvasTextFormat>(dwriteTextFormat.Get());

        auto wrappedTextFormat = GetWrappedResource<IDWriteTextFormat>(canvasTextFormat);
        Assert::AreEqual<void*>(dwriteTextFormat.Get(), wrappedTextFormat.Get());

        //
        // Verify that it shows the right values
        //

#define CHECK(PROPERTY, EXPECTED) \
        Assert::IsTrue(canvasTextFormat->PROPERTY == EXPECTED)

        CHECK(Direction,              CanvasTextDirection::TopToBottomThenRightToLeft);
        CHECK(FontFamily,             L"Arial");
        CHECK(FontSize,               12.0f);
        CHECK(FontStretch,            Windows::UI::Text::FontStretch::UltraExpanded);
        CHECK(FontStyle,              Windows::UI::Text::FontStyle::Oblique);
        CHECK(FontWeight,             Windows::UI::Text::FontWeight { 100 } );
        CHECK(IncrementalTabStop,     12.0f);
        CHECK(LineSpacingMethod,      CanvasLineSpacingMethod::Uniform);
        CHECK(LineSpacing,            2.0f);
        CHECK(LineSpacingBaseline,    4.0f);
        CHECK(LocaleName,             L"locale");
        CHECK(VerticalAlignment,      CanvasVerticalAlignment::Bottom);
        CHECK(HorizontalAlignment,    CanvasHorizontalAlignment::Right);
        CHECK(TrimmingGranularity,    CanvasTextTrimmingGranularity::Word);
        CHECK(TrimmingDelimiter,      L"/");
        CHECK(TrimmingDelimiterCount, 1); 
        CHECK(WordWrapping,           CanvasWordWrapping::EmergencyBreak);
        CHECK(Options,                CanvasDrawTextOptions::Default);

        //
        // Set properties on the underlying dwrite object and confirm the
        // changes are shown in the reflect object.
        //
        // TODO #1764: figure out if this is the best behavior.  Maybe
        // GetOrCreate() should take a snapshot and so these updates are
        // ignored, and GetWrappedResource() always returns a unique dwrite
        // object?
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

        //
        // Set a property that would cause it to recreate.
        //

        canvasTextFormat->FontFamily = L"Comic Sans MS";

        //
        // Verify that it still shows all the right values.  At this point the
        // shadow values will be being used.
        //

        CHECK(Direction,              CanvasTextDirection::BottomToTopThenRightToLeft);
        CHECK(FontFamily,             L"Comic Sans MS");
        CHECK(FontSize,               12.0f);
        CHECK(FontStretch,            Windows::UI::Text::FontStretch::UltraExpanded);
        CHECK(FontStyle,              Windows::UI::Text::FontStyle::Oblique);
        CHECK(FontWeight,             Windows::UI::Text::FontWeight { 100 } );
        CHECK(IncrementalTabStop,     16.0f);
        CHECK(LineSpacingMethod,      CanvasLineSpacingMethod::Default);
        CHECK(LineSpacing,            5.0f);
        CHECK(LineSpacingBaseline,    7.0f);
        CHECK(LocaleName,             L"locale");
        CHECK(VerticalAlignment,      CanvasVerticalAlignment::Center);
        CHECK(HorizontalAlignment,    CanvasHorizontalAlignment::Center);
        CHECK(TrimmingGranularity,    CanvasTextTrimmingGranularity::Character);
        CHECK(TrimmingDelimiter,      L"!");
        CHECK(TrimmingDelimiterCount, 2); 
        CHECK(WordWrapping,           CanvasWordWrapping::NoWrap);
        CHECK(Options,                CanvasDrawTextOptions::Default);

        //
        // Verify that a new IDWriteTextFormat was indeed created and that it
        // has the correct values set on it.
        //

        auto newDWriteTextFormat = GetWrappedResource<IDWriteTextFormat>(canvasTextFormat);

        Assert::AreNotEqual<void*>(dwriteTextFormat.Get(), newDWriteTextFormat.Get());

        ComPtr<IDWriteFontCollection> newFontCollection;
        ThrowIfFailed(newDWriteTextFormat->GetFontCollection(&newFontCollection));
        
        Assert::AreNotEqual<void*>(fontCollection.Get(), newFontCollection.Get());

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
        ComPtr<IDWriteFactory2> factory;
        ThrowIfFailed(DWriteCreateFactory(
            DWRITE_FACTORY_TYPE_SHARED,
            __uuidof(&factory),
            static_cast<IUnknown**>(&factory)));

        ComPtr<IDWriteTextFormat> invalidDWriteTextFormat;
        ThrowIfFailed(factory->CreateTextFormat(
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
};

