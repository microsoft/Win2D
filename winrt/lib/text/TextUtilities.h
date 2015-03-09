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

#pragma once

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas
{

    //
    // Conversion between DWite and Canvas types.  Many of the enums are
    // blittable; this is confirmed through static_asserts before casting.
    //

#define CHECK_ENUM_MEMBER(DWRITE, CANVAS)                                           \
        static_assert(static_cast<int>(DWRITE) == static_cast<int>(CANVAS), #CANVAS " is assumed to match " #DWRITE)


    static DWRITE_FLOW_DIRECTION ToFlowDirection(CanvasTextDirection value)
    {
        CHECK_ENUM_MEMBER(DWRITE_FLOW_DIRECTION_TOP_TO_BOTTOM, CanvasTextDirection::TopToBottom);
        CHECK_ENUM_MEMBER(DWRITE_FLOW_DIRECTION_BOTTOM_TO_TOP, CanvasTextDirection::BottomToTop);
        CHECK_ENUM_MEMBER(DWRITE_FLOW_DIRECTION_LEFT_TO_RIGHT, CanvasTextDirection::LeftToRight);
        CHECK_ENUM_MEMBER(DWRITE_FLOW_DIRECTION_RIGHT_TO_LEFT, CanvasTextDirection::RightToLeft);

        return static_cast<DWRITE_FLOW_DIRECTION>(value);
    }


    static CanvasTextDirection ToCanvasTextDirection(DWRITE_FLOW_DIRECTION value)
    {
        // static_asserts in ToFlowDirection validate that this cast is ok.
        return static_cast<CanvasTextDirection>(value);
    }


    static DWRITE_LINE_SPACING_METHOD ToLineSpacingMethod(CanvasLineSpacingMethod value)
    {
        CHECK_ENUM_MEMBER(DWRITE_LINE_SPACING_METHOD_DEFAULT, CanvasLineSpacingMethod::Default);
        CHECK_ENUM_MEMBER(DWRITE_LINE_SPACING_METHOD_UNIFORM, CanvasLineSpacingMethod::Uniform);

        return static_cast<DWRITE_LINE_SPACING_METHOD>(value);
    }


    static CanvasLineSpacingMethod ToCanvasLineSpacingMethod(DWRITE_LINE_SPACING_METHOD value)
    {
        // static_asserts in ToLineSpacingMethod validate that this cast is ok
        return static_cast<CanvasLineSpacingMethod>(value);
    }


    static DWRITE_FONT_STRETCH ToFontStretch(ABI::Windows::UI::Text::FontStretch value)
    {
        CHECK_ENUM_MEMBER(DWRITE_FONT_STRETCH_UNDEFINED, ABI::Windows::UI::Text::FontStretch_Undefined);
        CHECK_ENUM_MEMBER(DWRITE_FONT_STRETCH_ULTRA_CONDENSED, ABI::Windows::UI::Text::FontStretch_UltraCondensed);
        CHECK_ENUM_MEMBER(DWRITE_FONT_STRETCH_EXTRA_CONDENSED, ABI::Windows::UI::Text::FontStretch_ExtraCondensed);
        CHECK_ENUM_MEMBER(DWRITE_FONT_STRETCH_CONDENSED, ABI::Windows::UI::Text::FontStretch_Condensed);
        CHECK_ENUM_MEMBER(DWRITE_FONT_STRETCH_SEMI_CONDENSED, ABI::Windows::UI::Text::FontStretch_SemiCondensed);
        CHECK_ENUM_MEMBER(DWRITE_FONT_STRETCH_NORMAL, ABI::Windows::UI::Text::FontStretch_Normal);
        CHECK_ENUM_MEMBER(DWRITE_FONT_STRETCH_MEDIUM, ABI::Windows::UI::Text::FontStretch_Normal);
        CHECK_ENUM_MEMBER(DWRITE_FONT_STRETCH_SEMI_EXPANDED, ABI::Windows::UI::Text::FontStretch_SemiExpanded);
        CHECK_ENUM_MEMBER(DWRITE_FONT_STRETCH_EXPANDED, ABI::Windows::UI::Text::FontStretch_Expanded);
        CHECK_ENUM_MEMBER(DWRITE_FONT_STRETCH_EXTRA_EXPANDED, ABI::Windows::UI::Text::FontStretch_ExtraExpanded);
        CHECK_ENUM_MEMBER(DWRITE_FONT_STRETCH_ULTRA_EXPANDED, ABI::Windows::UI::Text::FontStretch_UltraExpanded);

        return static_cast<DWRITE_FONT_STRETCH>(value);
    }


    static ABI::Windows::UI::Text::FontStretch ToWindowsFontStretch(DWRITE_FONT_STRETCH value)
    {
        // static_asserts in ToFontStretch validate that this cast is ok
        return static_cast<ABI::Windows::UI::Text::FontStretch>(value);
    }


    static DWRITE_FONT_STYLE ToFontStyle(ABI::Windows::UI::Text::FontStyle value)
    {
        CHECK_ENUM_MEMBER(DWRITE_FONT_STYLE_NORMAL, ABI::Windows::UI::Text::FontStyle_Normal);
        CHECK_ENUM_MEMBER(DWRITE_FONT_STYLE_OBLIQUE, ABI::Windows::UI::Text::FontStyle_Oblique);
        CHECK_ENUM_MEMBER(DWRITE_FONT_STYLE_ITALIC, ABI::Windows::UI::Text::FontStyle_Italic);

        return static_cast<DWRITE_FONT_STYLE>(value);
    }


    static ABI::Windows::UI::Text::FontStyle ToWindowsFontStyle(DWRITE_FONT_STYLE value)
    {
        // static_asserts in ToFontStyle validate that this cast is ok
        return static_cast<ABI::Windows::UI::Text::FontStyle>(value);
    }


    static DWRITE_FONT_WEIGHT ToFontWeight(ABI::Windows::UI::Text::FontWeight value)
    {
        return static_cast<DWRITE_FONT_WEIGHT>(value.Weight);
    }


    static ABI::Windows::UI::Text::FontWeight ToWindowsFontWeight(DWRITE_FONT_WEIGHT value)
    {
        return ABI::Windows::UI::Text::FontWeight{ static_cast<uint16_t>(value) };
    }


    static DWRITE_PARAGRAPH_ALIGNMENT ToParagraphAlignment(CanvasVerticalAlignment value)
    {
        CHECK_ENUM_MEMBER(DWRITE_PARAGRAPH_ALIGNMENT_NEAR, CanvasVerticalAlignment::Top);
        CHECK_ENUM_MEMBER(DWRITE_PARAGRAPH_ALIGNMENT_FAR, CanvasVerticalAlignment::Bottom);
        CHECK_ENUM_MEMBER(DWRITE_PARAGRAPH_ALIGNMENT_CENTER, CanvasVerticalAlignment::Center);

        return static_cast<DWRITE_PARAGRAPH_ALIGNMENT>(value);
    }


    static CanvasVerticalAlignment ToCanvasVerticalAlignment(DWRITE_PARAGRAPH_ALIGNMENT value)
    {
        // static_asserts in ToParagraphAlignment validate that this cast is ok
        return static_cast<CanvasVerticalAlignment>(value);
    }


    static DWRITE_READING_DIRECTION ToReadingDirection(CanvasTextDirection value)
    {
        switch (value)
        {
        case CanvasTextDirection::LeftToRight: return DWRITE_READING_DIRECTION_LEFT_TO_RIGHT;
        case CanvasTextDirection::RightToLeft: return DWRITE_READING_DIRECTION_RIGHT_TO_LEFT;
        case CanvasTextDirection::TopToBottom: return DWRITE_READING_DIRECTION_TOP_TO_BOTTOM;
        case CanvasTextDirection::BottomToTop: return DWRITE_READING_DIRECTION_BOTTOM_TO_TOP;
        default: ThrowHR(E_INVALIDARG);
        }
    }


    static CanvasTextDirection ToCanvasTextDirection(DWRITE_READING_DIRECTION value)
    {
        switch (value)
        {
        case DWRITE_READING_DIRECTION_LEFT_TO_RIGHT: return CanvasTextDirection::LeftToRight;
        case DWRITE_READING_DIRECTION_RIGHT_TO_LEFT: return CanvasTextDirection::RightToLeft;
        case DWRITE_READING_DIRECTION_TOP_TO_BOTTOM: return CanvasTextDirection::TopToBottom;
        case DWRITE_READING_DIRECTION_BOTTOM_TO_TOP: return CanvasTextDirection::BottomToTop;
        default: ThrowHR(E_INVALIDARG);
        }
    }


    static DWRITE_TEXT_ALIGNMENT ToTextAlignment(ABI::Windows::UI::Text::ParagraphAlignment value)
    {
        switch (value)
        {
        case ABI::Windows::UI::Text::ParagraphAlignment_Left: return DWRITE_TEXT_ALIGNMENT_LEADING;
        case ABI::Windows::UI::Text::ParagraphAlignment_Center: return DWRITE_TEXT_ALIGNMENT_CENTER;
        case ABI::Windows::UI::Text::ParagraphAlignment_Right: return DWRITE_TEXT_ALIGNMENT_TRAILING;
        case ABI::Windows::UI::Text::ParagraphAlignment_Justify: return DWRITE_TEXT_ALIGNMENT_JUSTIFIED;
        default: ThrowHR(E_INVALIDARG);
        }
    }


    static ABI::Windows::UI::Text::ParagraphAlignment ToWindowsParagraphAlignment(DWRITE_TEXT_ALIGNMENT value)
    {
        switch (value)
        {
        case DWRITE_TEXT_ALIGNMENT_LEADING: return ABI::Windows::UI::Text::ParagraphAlignment_Left;
        case DWRITE_TEXT_ALIGNMENT_CENTER: return ABI::Windows::UI::Text::ParagraphAlignment_Center;
        case DWRITE_TEXT_ALIGNMENT_TRAILING: return ABI::Windows::UI::Text::ParagraphAlignment_Right;
        case DWRITE_TEXT_ALIGNMENT_JUSTIFIED: return ABI::Windows::UI::Text::ParagraphAlignment_Justify;
        default: ThrowHR(E_INVALIDARG);
        }
    }


    static DWRITE_TRIMMING_GRANULARITY ToTrimmingGranularity(CanvasTextTrimmingGranularity value)
    {
        CHECK_ENUM_MEMBER(DWRITE_TRIMMING_GRANULARITY_NONE, CanvasTextTrimmingGranularity::None);
        CHECK_ENUM_MEMBER(DWRITE_TRIMMING_GRANULARITY_CHARACTER, CanvasTextTrimmingGranularity::Character);
        CHECK_ENUM_MEMBER(DWRITE_TRIMMING_GRANULARITY_WORD, CanvasTextTrimmingGranularity::Word);

        return static_cast<DWRITE_TRIMMING_GRANULARITY>(value);
    }


    static CanvasTextTrimmingGranularity ToCanvasTextTrimmingGranularity(DWRITE_TRIMMING_GRANULARITY value)
    {
        // static_asserts in ToTrimmingGranularity validate that this case is ok
        return static_cast<CanvasTextTrimmingGranularity>(value);
    }


    static DWRITE_VERTICAL_GLYPH_ORIENTATION ToVerticalGlyphOrientation(CanvasVerticalGlyphOrientation value)
    {
        CHECK_ENUM_MEMBER(DWRITE_VERTICAL_GLYPH_ORIENTATION_DEFAULT, CanvasVerticalGlyphOrientation::Default);
        CHECK_ENUM_MEMBER(DWRITE_VERTICAL_GLYPH_ORIENTATION_STACKED, CanvasVerticalGlyphOrientation::Stacked);

        return static_cast<DWRITE_VERTICAL_GLYPH_ORIENTATION>(value);
    }


    static CanvasVerticalGlyphOrientation ToCanvasVerticalGlyphOrientation(DWRITE_VERTICAL_GLYPH_ORIENTATION value)
    {
        // static_asserts in ToVerticalGlyphOrientation validate that this case is ok
        return static_cast<CanvasVerticalGlyphOrientation>(value);
    }


    static DWRITE_OPTICAL_ALIGNMENT ToOpticalAlignment(CanvasOpticalAlignment value)
    {
        CHECK_ENUM_MEMBER(DWRITE_OPTICAL_ALIGNMENT_NONE, CanvasOpticalAlignment::Default);
        CHECK_ENUM_MEMBER(DWRITE_OPTICAL_ALIGNMENT_NO_SIDE_BEARINGS, CanvasOpticalAlignment::NoSideBearings);

        return static_cast<DWRITE_OPTICAL_ALIGNMENT>(value);
    }


    static CanvasOpticalAlignment ToCanvasOpticalAlignment(DWRITE_OPTICAL_ALIGNMENT value)
    {
        // static_asserts in ToOpticalAlignment validate that this case is ok
        return static_cast<CanvasOpticalAlignment>(value);
    }


    static WinString ToCanvasTrimmingDelimiter(uint32_t value)
    {
        // TODO #1658: Do the unicode conversion properly.
        // http://www.unicode.org/faq/utf_bom.html#utf16-3.  This code needs its
        // own set of tests.

        WinStringBuilder builder;

        if (value == 0)
        {
            return WinString();
        }
        else if (value <= 0x0000FFFF)
        {
            auto buffer = builder.Allocate(1);
            buffer[0] = static_cast<wchar_t>(value);
        }
        else
        {
            auto buffer = builder.Allocate(2);
            *(reinterpret_cast<uint32_t*>(buffer)) = value;
        }

        return builder.Get();
    }


    static uint32_t ToTrimmingDelimiter(WinString const& value)
    {
        // TODO #1658: Do the unicode conversion properly.
        // http://www.unicode.org/faq/utf_bom.html#utf16-3.  This code needs its
        // own set of tests.

        uint32_t sourceStringLength = 0;
        auto sourceString = WindowsGetStringRawBuffer(value, &sourceStringLength);

        if (sourceStringLength == 0)
        {
            return 0;
        }
        else if (sourceStringLength == 1)
        {
            return sourceString[0];
        }
        else
        {
            return (sourceString[0] << 16) | sourceString[1];
        }
    }


    static DWRITE_WORD_WRAPPING ToWordWrapping(CanvasWordWrapping value)
    {
        CHECK_ENUM_MEMBER(DWRITE_WORD_WRAPPING_WRAP, CanvasWordWrapping::Wrap);
        CHECK_ENUM_MEMBER(DWRITE_WORD_WRAPPING_NO_WRAP, CanvasWordWrapping::NoWrap);
        CHECK_ENUM_MEMBER(DWRITE_WORD_WRAPPING_EMERGENCY_BREAK, CanvasWordWrapping::EmergencyBreak);
        CHECK_ENUM_MEMBER(DWRITE_WORD_WRAPPING_WHOLE_WORD, CanvasWordWrapping::WholeWord);
        CHECK_ENUM_MEMBER(DWRITE_WORD_WRAPPING_CHARACTER, CanvasWordWrapping::Character);

        return static_cast<DWRITE_WORD_WRAPPING>(value);
    }


    static CanvasWordWrapping ToCanvasWordWrapping(DWRITE_WORD_WRAPPING value)
    {
        // static_asserts in ToWordWrapping validate that this cast is ok
        return static_cast<CanvasWordWrapping>(value);
    }
   
}}}}