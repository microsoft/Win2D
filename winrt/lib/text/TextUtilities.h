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

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Text
{

    //
    // Conversion between DWite and Canvas types.  Many of the enums are
    // blittable; this is confirmed through static_asserts before casting.
    //

#define CHECK_ENUM_MEMBER(DWRITE, CANVAS)                                           \
        static_assert(static_cast<int>(DWRITE) == static_cast<int>(CANVAS), #CANVAS " is assumed to match " #DWRITE)


    inline DWRITE_FONT_STRETCH ToFontStretch(ABI::Windows::UI::Text::FontStretch value)
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


    inline ABI::Windows::UI::Text::FontStretch ToWindowsFontStretch(DWRITE_FONT_STRETCH value)
    {
        // static_asserts in ToFontStretch validate that this cast is ok
        return static_cast<ABI::Windows::UI::Text::FontStretch>(value);
    }


    inline DWRITE_FONT_STYLE ToFontStyle(ABI::Windows::UI::Text::FontStyle value)
    {
        CHECK_ENUM_MEMBER(DWRITE_FONT_STYLE_NORMAL, ABI::Windows::UI::Text::FontStyle_Normal);
        CHECK_ENUM_MEMBER(DWRITE_FONT_STYLE_OBLIQUE, ABI::Windows::UI::Text::FontStyle_Oblique);
        CHECK_ENUM_MEMBER(DWRITE_FONT_STYLE_ITALIC, ABI::Windows::UI::Text::FontStyle_Italic);

        return static_cast<DWRITE_FONT_STYLE>(value);
    }


    inline ABI::Windows::UI::Text::FontStyle ToWindowsFontStyle(DWRITE_FONT_STYLE value)
    {
        // static_asserts in ToFontStyle validate that this cast is ok
        return static_cast<ABI::Windows::UI::Text::FontStyle>(value);
    }


    inline DWRITE_FONT_WEIGHT ToFontWeight(ABI::Windows::UI::Text::FontWeight value)
    {
        return static_cast<DWRITE_FONT_WEIGHT>(value.Weight);
    }


    inline ABI::Windows::UI::Text::FontWeight ToWindowsFontWeight(DWRITE_FONT_WEIGHT value)
    {
        return ABI::Windows::UI::Text::FontWeight{ static_cast<uint16_t>(value) };
    }


    inline DWRITE_PARAGRAPH_ALIGNMENT ToParagraphAlignment(CanvasVerticalAlignment value)
    {
        CHECK_ENUM_MEMBER(DWRITE_PARAGRAPH_ALIGNMENT_NEAR, CanvasVerticalAlignment::Top);
        CHECK_ENUM_MEMBER(DWRITE_PARAGRAPH_ALIGNMENT_FAR, CanvasVerticalAlignment::Bottom);
        CHECK_ENUM_MEMBER(DWRITE_PARAGRAPH_ALIGNMENT_CENTER, CanvasVerticalAlignment::Center);

        return static_cast<DWRITE_PARAGRAPH_ALIGNMENT>(value);
    }


    inline CanvasVerticalAlignment ToCanvasVerticalAlignment(DWRITE_PARAGRAPH_ALIGNMENT value)
    {
        // static_asserts in ToParagraphAlignment validate that this cast is ok
        return static_cast<CanvasVerticalAlignment>(value);
    }


    inline DWRITE_TEXT_ALIGNMENT ToTextAlignment(CanvasHorizontalAlignment value)
    {
        CHECK_ENUM_MEMBER(DWRITE_TEXT_ALIGNMENT_LEADING, CanvasHorizontalAlignment::Left);
        CHECK_ENUM_MEMBER(DWRITE_TEXT_ALIGNMENT_TRAILING, CanvasHorizontalAlignment::Right);
        CHECK_ENUM_MEMBER(DWRITE_TEXT_ALIGNMENT_CENTER, CanvasHorizontalAlignment::Center);
        CHECK_ENUM_MEMBER(DWRITE_TEXT_ALIGNMENT_JUSTIFIED, CanvasHorizontalAlignment::Justified);

        return static_cast<DWRITE_TEXT_ALIGNMENT>(value);
    }


    inline CanvasHorizontalAlignment ToCanvasHorizontalAlignment(DWRITE_TEXT_ALIGNMENT value)
    {
        // static_asserts in ToTextAlignment validate that this case is ok
        return static_cast<CanvasHorizontalAlignment>(value);
    }


    inline DWRITE_TRIMMING_GRANULARITY ToTrimmingGranularity(CanvasTextTrimmingGranularity value)
    {
        CHECK_ENUM_MEMBER(DWRITE_TRIMMING_GRANULARITY_NONE, CanvasTextTrimmingGranularity::None);
        CHECK_ENUM_MEMBER(DWRITE_TRIMMING_GRANULARITY_CHARACTER, CanvasTextTrimmingGranularity::Character);
        CHECK_ENUM_MEMBER(DWRITE_TRIMMING_GRANULARITY_WORD, CanvasTextTrimmingGranularity::Word);

        return static_cast<DWRITE_TRIMMING_GRANULARITY>(value);
    }


    inline CanvasTextTrimmingGranularity ToCanvasTextTrimmingGranularity(DWRITE_TRIMMING_GRANULARITY value)
    {
        // static_asserts in ToTrimmingGranularity validate that this case is ok
        return static_cast<CanvasTextTrimmingGranularity>(value);
    }


    inline DWRITE_VERTICAL_GLYPH_ORIENTATION ToVerticalGlyphOrientation(CanvasVerticalGlyphOrientation value)
    {
        CHECK_ENUM_MEMBER(DWRITE_VERTICAL_GLYPH_ORIENTATION_DEFAULT, CanvasVerticalGlyphOrientation::Default);
        CHECK_ENUM_MEMBER(DWRITE_VERTICAL_GLYPH_ORIENTATION_STACKED, CanvasVerticalGlyphOrientation::Stacked);

        return static_cast<DWRITE_VERTICAL_GLYPH_ORIENTATION>(value);
    }


    inline CanvasVerticalGlyphOrientation ToCanvasVerticalGlyphOrientation(DWRITE_VERTICAL_GLYPH_ORIENTATION value)
    {
        // static_asserts in ToVerticalGlyphOrientation validate that this case is ok
        return static_cast<CanvasVerticalGlyphOrientation>(value);
    }


    inline DWRITE_OPTICAL_ALIGNMENT ToOpticalAlignment(CanvasOpticalAlignment value)
    {
        CHECK_ENUM_MEMBER(DWRITE_OPTICAL_ALIGNMENT_NONE, CanvasOpticalAlignment::Default);
        CHECK_ENUM_MEMBER(DWRITE_OPTICAL_ALIGNMENT_NO_SIDE_BEARINGS, CanvasOpticalAlignment::NoSideBearings);

        return static_cast<DWRITE_OPTICAL_ALIGNMENT>(value);
    }


    inline CanvasOpticalAlignment ToCanvasOpticalAlignment(DWRITE_OPTICAL_ALIGNMENT value)
    {
        // static_asserts in ToOpticalAlignment validate that this case is ok
        return static_cast<CanvasOpticalAlignment>(value);
    }


    inline WinString ToCanvasTrimmingDelimiter(uint32_t value)
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


    inline uint32_t ToTrimmingDelimiter(WinString const& value)
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


    inline DWRITE_WORD_WRAPPING ToWordWrapping(CanvasWordWrapping value)
    {
        CHECK_ENUM_MEMBER(DWRITE_WORD_WRAPPING_WRAP, CanvasWordWrapping::Wrap);
        CHECK_ENUM_MEMBER(DWRITE_WORD_WRAPPING_NO_WRAP, CanvasWordWrapping::NoWrap);
        CHECK_ENUM_MEMBER(DWRITE_WORD_WRAPPING_EMERGENCY_BREAK, CanvasWordWrapping::EmergencyBreak);
        CHECK_ENUM_MEMBER(DWRITE_WORD_WRAPPING_WHOLE_WORD, CanvasWordWrapping::WholeWord);
        CHECK_ENUM_MEMBER(DWRITE_WORD_WRAPPING_CHARACTER, CanvasWordWrapping::Character);

        return static_cast<DWRITE_WORD_WRAPPING>(value);
    }


    inline CanvasWordWrapping ToCanvasWordWrapping(DWRITE_WORD_WRAPPING value)
    {
        // static_asserts in ToWordWrapping validate that this cast is ok
        return static_cast<CanvasWordWrapping>(value);
    }

    //
    // Parameter validation functions
    //

    template<typename T>
    inline void ThrowIfInvalid(T)
    {
        static_assert(false, "Specialization required");
    }

    template<>
    inline void ThrowIfInvalid(CanvasTextDirection value)
    {
        switch (value)
        {
        case CanvasTextDirection::LeftToRightThenTopToBottom:
        case CanvasTextDirection::RightToLeftThenTopToBottom:
        case CanvasTextDirection::LeftToRightThenBottomToTop:
        case CanvasTextDirection::RightToLeftThenBottomToTop:
        case CanvasTextDirection::TopToBottomThenLeftToRight:
        case CanvasTextDirection::BottomToTopThenLeftToRight:
        case CanvasTextDirection::TopToBottomThenRightToLeft:
        case CanvasTextDirection::BottomToTopThenRightToLeft:
            return;
        default:
            ThrowHR(E_INVALIDARG);
        }
    }

    template<>
    inline void ThrowIfInvalid(CanvasVerticalAlignment value)
    {
        switch (value)
        {
        case CanvasVerticalAlignment::Top:
        case CanvasVerticalAlignment::Bottom:
        case CanvasVerticalAlignment::Center:
            return;

        default:
            ThrowHR(E_INVALIDARG);
        }
    }

    template<>
    inline void ThrowIfInvalid(CanvasHorizontalAlignment value)
    {
        switch (value)
        {
        case CanvasHorizontalAlignment::Left:
        case CanvasHorizontalAlignment::Right:
        case CanvasHorizontalAlignment::Center:
        case CanvasHorizontalAlignment::Justified:
            return;

        default:
            ThrowHR(E_INVALIDARG);
        }
    }

    template<>
    inline void ThrowIfInvalid(ABI::Windows::UI::Text::ParagraphAlignment value)
    {
        switch (value)
        {
        case ABI::Windows::UI::Text::ParagraphAlignment_Undefined:
        case ABI::Windows::UI::Text::ParagraphAlignment_Left:
        case ABI::Windows::UI::Text::ParagraphAlignment_Center:
        case ABI::Windows::UI::Text::ParagraphAlignment_Right:
        case ABI::Windows::UI::Text::ParagraphAlignment_Justify:
            return;

        default:
            ThrowHR(E_INVALIDARG);
        }
    }

    template<>
    inline void ThrowIfInvalid(CanvasWordWrapping value)
    {
        switch (value)
        {
        case CanvasWordWrapping::Wrap:
        case CanvasWordWrapping::NoWrap:
        case CanvasWordWrapping::EmergencyBreak:
        case CanvasWordWrapping::WholeWord:
        case CanvasWordWrapping::Character:
            return;

        default:
            ThrowHR(E_INVALIDARG);
        }
    }

    template<>
    inline void ThrowIfInvalid(CanvasTextTrimmingGranularity value)
    {
        switch (value)
        {
        case CanvasTextTrimmingGranularity::None:
        case CanvasTextTrimmingGranularity::Character:
        case CanvasTextTrimmingGranularity::Word:
            return;

        default:
            ThrowHR(E_INVALIDARG);
        }
    }

    inline void ThrowIfInvalidTrimmingDelimiter(HSTRING value)
    {
        // The delimiter must be a single code point and so cannot be more than
        // 2 UTF-16 code units long
        if (WindowsGetStringLen(value) > 2)
            ThrowHR(E_INVALIDARG);
    }
    
    template <class GET_ATTRIBUTE_LENGTH_METHOD, class GET_ATTRIBUTE_METHOD>
    inline WinString GetStringAttributeFromTextFormat(
        GET_ATTRIBUTE_LENGTH_METHOD fnGetLength,
        GET_ATTRIBUTE_METHOD fnGet,
        IDWriteTextFormat* format)
    {
        WinStringBuilder stringBuilder;
        uint32_t attributeLength = (format->*fnGetLength)();
        attributeLength++; // Account for null terminator

        auto buffer = stringBuilder.Allocate(attributeLength);
        ThrowIfFailed((format->*fnGet)(buffer, attributeLength));
        return stringBuilder.Get();
    }

    inline WinString GetFontFamilyName(IDWriteTextFormat* format)
    {
        return GetStringAttributeFromTextFormat(
            &IDWriteTextFormat::GetFontFamilyNameLength,
            &IDWriteTextFormat::GetFontFamilyName,
            format);
    }

    inline WinString GetLocaleName(IDWriteTextFormat* format)
    {
        return GetStringAttributeFromTextFormat(
            &IDWriteTextFormat::GetLocaleNameLength,
            &IDWriteTextFormat::GetLocaleName,
            format);
    }

    inline std::pair<WinString, WinString> GetUriAndFontFamily(WinString const& fontFamilyName)
    {
        auto beginIt = begin(fontFamilyName);
        auto endIt = end(fontFamilyName);

        auto hashPos = std::find(beginIt, endIt, L'#');

        if (hashPos == endIt)
            return std::make_pair(WinString(), fontFamilyName);
        else
            return std::make_pair(WinString(beginIt, hashPos), WinString(hashPos + 1, endIt));
    }
   
}}}}}