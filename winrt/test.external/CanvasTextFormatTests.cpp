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
        // to be preserved even if CanvasTextFormat recreates its underlying
        // IDWriteTextFormat.
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

        ThrowIfFailed(dwriteTextFormat->SetFlowDirection(DWRITE_FLOW_DIRECTION_RIGHT_TO_LEFT));
        ThrowIfFailed(dwriteTextFormat->SetIncrementalTabStop(12.0f));
        ThrowIfFailed(dwriteTextFormat->SetLineSpacing(DWRITE_LINE_SPACING_METHOD_UNIFORM, 2.0f, 4.0f));
        ThrowIfFailed(dwriteTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_FAR));
        ThrowIfFailed(dwriteTextFormat->SetReadingDirection(DWRITE_READING_DIRECTION_TOP_TO_BOTTOM));
        ThrowIfFailed(dwriteTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING));
        ThrowIfFailed(dwriteTextFormat->SetTrimming(&DWRITE_TRIMMING{ DWRITE_TRIMMING_GRANULARITY_WORD, L'/', 1 }, nullptr));
        ThrowIfFailed(dwriteTextFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_EMERGENCY_BREAK));

        //
        // Get a CanvasTextFormat based on the IDWriteTextFormat
        //
        auto canvasTextFormat = GetOrCreate<CanvasTextFormat>(dwriteTextFormat.Get());

        auto wrappedTextFormat = GetWrappedResource<IDWriteTextFormat>(canvasTextFormat);
        Assert::AreEqual<void*>(dwriteTextFormat.Get(), wrappedTextFormat.Get());

        //
        // Verify that it shows the right values
        //

#define CHECK(PROPERTY, EXPECTED)                       \
        do {                                            \
            auto value = canvasTextFormat->PROPERTY;    \
            Assert::IsTrue(value == EXPECTED);          \
        } while(false)

        CHECK(FlowDirection,          CanvasTextDirection::RightToLeft);
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
        CHECK(ReadingDirection,       CanvasTextDirection::TopToBottom);
        CHECK(ParagraphAlignment,     Windows::UI::Text::ParagraphAlignment::Right);
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

        ThrowIfFailed(dwriteTextFormat->SetFlowDirection(DWRITE_FLOW_DIRECTION_BOTTOM_TO_TOP));
        ThrowIfFailed(dwriteTextFormat->SetIncrementalTabStop(16.0f));
        ThrowIfFailed(dwriteTextFormat->SetLineSpacing(DWRITE_LINE_SPACING_METHOD_DEFAULT, 5.0f, 7.0f));
        ThrowIfFailed(dwriteTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER));
        ThrowIfFailed(dwriteTextFormat->SetReadingDirection(DWRITE_READING_DIRECTION_BOTTOM_TO_TOP));
        ThrowIfFailed(dwriteTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER));
        ThrowIfFailed(dwriteTextFormat->SetTrimming(&DWRITE_TRIMMING{ DWRITE_TRIMMING_GRANULARITY_CHARACTER, L'!', 2 }, nullptr));
        ThrowIfFailed(dwriteTextFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP));

        //
        // Set a property that would cause it to recreate.
        //

        canvasTextFormat->FontFamily = L"Comic Sans MS";

        //
        // Verify that it still shows all the right values.  At this point the
        // shadow values will be being used.
        //

        CHECK(FlowDirection,          CanvasTextDirection::BottomToTop);
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
        CHECK(ReadingDirection,       CanvasTextDirection::BottomToTop);
        CHECK(ParagraphAlignment,     Windows::UI::Text::ParagraphAlignment::Center);
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
        
        Assert::AreEqual<void*>(fontCollection.Get(), newFontCollection.Get());

#undef CHECK
    }
};

