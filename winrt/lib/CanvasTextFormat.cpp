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

#include "CanvasTextFormat.h"
#include "WinStringBuilder.h"

namespace canvas
{
    //
    // Parameter validation functions
    //

    template<typename T>
    static void ThrowIfInvalid(T)
    {
        static_assert(false, "Specialization required");
    }

    template<>
    static void ThrowIfInvalid(CanvasTextDirection value)
    {
        switch (value)
        {
        case CanvasTextDirection::TopToBottom:
        case CanvasTextDirection::BottomToTop:
        case CanvasTextDirection::LeftToRight:
        case CanvasTextDirection::RightToLeft:
            return;

        default:
            ThrowHR(E_INVALIDARG);
        }
    }

    template<>
    static void ThrowIfInvalid(CanvasLineSpacingMethod value)
    {
        switch (value)
        {
        case CanvasLineSpacingMethod::Default:
        case CanvasLineSpacingMethod::Uniform:
            return;

        default:
            ThrowHR(E_INVALIDARG);
        }
    }

    template<>
    static void ThrowIfInvalid(ABI::Windows::UI::Text::FontStretch value)
    {
        switch (value)
        {
        case ABI::Windows::UI::Text::FontStretch_Undefined:
        case ABI::Windows::UI::Text::FontStretch_UltraCondensed:
        case ABI::Windows::UI::Text::FontStretch_ExtraCondensed:
        case ABI::Windows::UI::Text::FontStretch_Condensed:
        case ABI::Windows::UI::Text::FontStretch_SemiCondensed:
        case ABI::Windows::UI::Text::FontStretch_Normal:
        case ABI::Windows::UI::Text::FontStretch_SemiExpanded:
        case ABI::Windows::UI::Text::FontStretch_Expanded:
        case ABI::Windows::UI::Text::FontStretch_ExtraExpanded:
        case ABI::Windows::UI::Text::FontStretch_UltraExpanded:
            return;

        default:
            ThrowHR(E_INVALIDARG);
        }
    }

    template<>
    static void ThrowIfInvalid(ABI::Windows::UI::Text::FontStyle value)
    {
        switch (value)
        {
        case ABI::Windows::UI::Text::FontStyle_Normal:
        case ABI::Windows::UI::Text::FontStyle_Oblique:
        case ABI::Windows::UI::Text::FontStyle_Italic:
            return;

        default:
            ThrowHR(E_INVALIDARG);
        }
    }

    template<>
    static void ThrowIfInvalid(CanvasWordWrapping value)
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
    static void ThrowIfInvalid(CanvasTextTrimmingGranularity value)
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

    template<>
    static void ThrowIfInvalid(ABI::Windows::UI::Text::ParagraphAlignment value)
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
    static void ThrowIfInvalid(CanvasVerticalAlignment value)
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

    static void ThrowIfInvalidFontWeight(ABI::Windows::UI::Text::FontWeight value)
    {
        if (value.Weight >= 1 && value.Weight <= 999)
            return;

        ThrowHR(E_INVALIDARG);
    }

    static void ThrowIfInvalidTrimmingDelimiter(HSTRING value)
    {
        // The delimiter must be a single code point and so cannot be more than
        // 2 UTF-16 code units long
        if (WindowsGetStringLen(value) > 2)
            ThrowHR(E_INVALIDARG);
    }

    static void ThrowIfNan(float value)
    {
        if (isnan(value))
            ThrowHR(E_INVALIDARG);
    }

    static void ThrowIfNegativeOrNan(float value)
    {
        if (value < 0 || isnan(value))
            ThrowHR(E_INVALIDARG);
    }

    template<typename T>
    static void ThrowIfNegative(T value)
    {
        if (value < 0)
            ThrowHR(E_INVALIDARG);
    }


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
        CHECK_ENUM_MEMBER(DWRITE_FONT_STRETCH_UNDEFINED,       ABI::Windows::UI::Text::FontStretch_Undefined);
        CHECK_ENUM_MEMBER(DWRITE_FONT_STRETCH_ULTRA_CONDENSED, ABI::Windows::UI::Text::FontStretch_UltraCondensed);
        CHECK_ENUM_MEMBER(DWRITE_FONT_STRETCH_EXTRA_CONDENSED, ABI::Windows::UI::Text::FontStretch_ExtraCondensed);
        CHECK_ENUM_MEMBER(DWRITE_FONT_STRETCH_CONDENSED,       ABI::Windows::UI::Text::FontStretch_Condensed);
        CHECK_ENUM_MEMBER(DWRITE_FONT_STRETCH_SEMI_CONDENSED,  ABI::Windows::UI::Text::FontStretch_SemiCondensed);
        CHECK_ENUM_MEMBER(DWRITE_FONT_STRETCH_NORMAL,          ABI::Windows::UI::Text::FontStretch_Normal);
        CHECK_ENUM_MEMBER(DWRITE_FONT_STRETCH_MEDIUM,          ABI::Windows::UI::Text::FontStretch_Normal);
        CHECK_ENUM_MEMBER(DWRITE_FONT_STRETCH_SEMI_EXPANDED,   ABI::Windows::UI::Text::FontStretch_SemiExpanded);
        CHECK_ENUM_MEMBER(DWRITE_FONT_STRETCH_EXPANDED,        ABI::Windows::UI::Text::FontStretch_Expanded);
        CHECK_ENUM_MEMBER(DWRITE_FONT_STRETCH_EXTRA_EXPANDED,  ABI::Windows::UI::Text::FontStretch_ExtraExpanded);
        CHECK_ENUM_MEMBER(DWRITE_FONT_STRETCH_ULTRA_EXPANDED,  ABI::Windows::UI::Text::FontStretch_UltraExpanded);

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
        return ABI::Windows::UI::Text::FontWeight{value};
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


    static CanvasTextDirection ToCanvasReadingDirection(DWRITE_READING_DIRECTION value)
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


    DWRITE_TEXT_ALIGNMENT ToTextAlignment(ABI::Windows::UI::Text::ParagraphAlignment value)
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
        CHECK_ENUM_MEMBER(DWRITE_TRIMMING_GRANULARITY_NONE,      CanvasTextTrimmingGranularity::None);
        CHECK_ENUM_MEMBER(DWRITE_TRIMMING_GRANULARITY_CHARACTER, CanvasTextTrimmingGranularity::Character);
        CHECK_ENUM_MEMBER(DWRITE_TRIMMING_GRANULARITY_WORD,      CanvasTextTrimmingGranularity::Word);

        return static_cast<DWRITE_TRIMMING_GRANULARITY>(value);
    }


    static CanvasTextTrimmingGranularity ToCanvasTextTrimmingGranularity(DWRITE_TRIMMING_GRANULARITY value)
    {
        // static_asserts in ToTrimmingGranularity validate that this case is ok
        return static_cast<CanvasTextTrimmingGranularity>(value);
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
            buffer[0] = value;
        }
        else
        {
            auto buffer = builder.Allocate(2);
            *(reinterpret_cast<uint32_t*>(buffer)) = value;
        }

        return builder.Get();
    }


    static uint32_t ToTrimmingDelimiter(const WinString& value)
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
        CHECK_ENUM_MEMBER(DWRITE_WORD_WRAPPING_WRAP,            CanvasWordWrapping::Wrap);
        CHECK_ENUM_MEMBER(DWRITE_WORD_WRAPPING_NO_WRAP,         CanvasWordWrapping::NoWrap);
        CHECK_ENUM_MEMBER(DWRITE_WORD_WRAPPING_EMERGENCY_BREAK, CanvasWordWrapping::EmergencyBreak);
        CHECK_ENUM_MEMBER(DWRITE_WORD_WRAPPING_WHOLE_WORD,      CanvasWordWrapping::WholeWord);
        CHECK_ENUM_MEMBER(DWRITE_WORD_WRAPPING_CHARACTER,       CanvasWordWrapping::Character);

        return static_cast<DWRITE_WORD_WRAPPING>(value);
    }


    static CanvasWordWrapping ToCanvasWordWrapping(DWRITE_WORD_WRAPPING value)
    {
        // static_asserts in ToWordWrapping validate that this cast is ok
        return static_cast<CanvasWordWrapping>(value);
    }


    //
    // CanvasTextFormatFactory implementation
    //


    IFACEMETHODIMP CanvasTextFormatFactory::ActivateInstance(IInspectable** object)
    {
        return ExceptionBoundary(
            [&]()
            {
                auto newCanvasTextFormat = Make<CanvasTextFormat>();
                CheckMakeResult(newCanvasTextFormat);
                ThrowIfFailed(newCanvasTextFormat.CopyTo(object));
            });
    }


    IFACEMETHODIMP CanvasTextFormatFactory::GetOrCreate(
        IUnknown* resource,
        IInspectable** wrapper)
    {
        //
        // TODO #1663: CanvasTextFormat / IDWriteTextFormat interop
        //
        return E_NOTIMPL;
    }


    //
    // CanvasTextFormat implementation
    //


    CanvasTextFormat::CanvasTextFormat()
        : m_closed(false)
        , m_flowDirection(CanvasTextDirection::TopToBottom)
        , m_fontFamilyName(L"Segoe UI")
        , m_fontSize(96.0f)
        , m_fontStretch(ABI::Windows::UI::Text::FontStretch_Normal)
        , m_fontStyle(ABI::Windows::UI::Text::FontStyle_Normal)
        , m_fontWeight(ToWindowsFontWeight(DWRITE_FONT_WEIGHT_NORMAL))
        , m_incrementalTabStop(-1.0f)          
        , m_lineSpacingMethod(CanvasLineSpacingMethod::Default)
        , m_lineSpacing(0.0f)
        , m_lineSpacingBaseline(0.0f)
        , m_verticalAlignment(CanvasVerticalAlignment::Top)
        , m_paragraphAlignment(ABI::Windows::UI::Text::ParagraphAlignment_Left)
        , m_readingDirection(CanvasTextDirection::LeftToRight)
        , m_trimmingGranularity(CanvasTextTrimmingGranularity::None)
        , m_trimmingDelimiterCount(0)
        , m_wordWrapping(CanvasWordWrapping::Wrap)
        , m_drawTextOptions(CanvasDrawTextOptions::Default)
    {
    }


    IFACEMETHODIMP CanvasTextFormat::Close()
    {
        m_closed = true;
        return S_OK;
    }


    void CanvasTextFormat::ThrowIfClosed()
    {
        if (m_closed)
            ThrowHR(RO_E_CLOSED);
    }


    IFACEMETHODIMP CanvasTextFormat::GetResource(IUnknown** value)
    {
        return ExceptionBoundary(
            [&]()
            {
                CheckAndClearOutPointer(value);
                ThrowIfFailed(GetRealizedTextFormat().CopyTo(value));
            });
    }


    ComPtr<IDWriteTextFormat> CanvasTextFormat::GetRealizedTextFormat()
    {
        if (m_format)
            return m_format;

        ComPtr<IDWriteFactory2> factory;
        ThrowIfFailed(DWriteCreateFactory(
            DWRITE_FACTORY_TYPE_SHARED,
            __uuidof(&factory),
            static_cast<IUnknown**>(&factory)));

        ThrowIfFailed(factory->CreateTextFormat(
            static_cast<const wchar_t*>(m_fontFamilyName),
            nullptr,            // font collection
            ToFontWeight(m_fontWeight),
            ToFontStyle(m_fontStyle),
            ToFontStretch(m_fontStretch),
            m_fontSize,
            static_cast<const wchar_t*>(m_localeName),
            &m_format));

        RealizeFlowDirection();
        RealizeIncrementalTabStop();
        RealizeLineSpacing();
        RealizeParagraphAlignment();
        RealizeReadingDirection();
        RealizeTextAlignment();
        RealizeTrimming();
        RealizeWordWrapping();

        return m_format;
    }


    CanvasDrawTextOptions CanvasTextFormat::GetDrawTextOptions()
    {
        return m_drawTextOptions;
    }


    void CanvasTextFormat::Unrealize()
    {
        m_format.Reset();
    }


    //
    // PropertyGet/PropertyPut - these encapsulate the process of getting /
    // setting properties that might be reflected on the realized
    // IDWriteTextFormat object.
    //

    template<typename T>
    static bool SetFrom(T* outputValue, const T& value)
    {
        if ((*outputValue) == value)
            return false;

        *outputValue = value;
        return true;
    }

    template<>
    static bool SetFrom(ABI::Windows::UI::Text::FontWeight* outputValue, const ABI::Windows::UI::Text::FontWeight& value)
    {
        return SetFrom(&outputValue->Weight, value.Weight);
    }

    template<typename WinString>
    static bool SetFrom(HSTRING* outputValue, const WinString& value)
    {
        if (value.Equals(*outputValue))
            return false;

        value.CopyTo(outputValue);
        return true;
    }

    template<typename HSTRING>
    static bool SetFrom(WinString* outputValue, const HSTRING& value)
    {
        if (outputValue->Equals(value))
            return false;

        *outputValue = value;
        return true;
    }

    template<typename T, typename ST, typename FN>
    HRESULT __declspec(nothrow) CanvasTextFormat::PropertyGet(T* value, const ST& shadowValue, FN realizedGetter)
    {
        return ExceptionBoundary(
            [&]()
            {
                CheckInPointer(value);
                ThrowIfClosed();
                
                if (m_format)
                    SetFrom(value, realizedGetter());
                else
                    SetFrom(value, shadowValue);
            });
    }

    
    template<typename T, typename TT, typename FNV>
    HRESULT __declspec(nothrow) CanvasTextFormat::PropertyPut(T value, TT* dest, FNV&& validator, void(CanvasTextFormat::*realizer)())
    {
        return ExceptionBoundary(
            [&]()
            {
                validator(value);

                ThrowIfClosed();

                if (SetFrom(dest, value))
                {
                    // If the value changed then we need to call realizer().
                    if (m_format && realizer)
                        (this->*realizer)();
                }
            });
    }

    template<typename T, typename TT>
    HRESULT __declspec(nothrow) CanvasTextFormat::PropertyPut(T value, TT* dest, void(CanvasTextFormat::*realizer)())
    {
        return PropertyPut(value, dest, [](T){}, realizer);
    }

    //
    // CanvasTextFormat.FlowDirection
    //
    
    IFACEMETHODIMP CanvasTextFormat::get_FlowDirection(CanvasTextDirection* value)
    {
        return PropertyGet(
            value,
            m_flowDirection,
            [&]() { return ToCanvasTextDirection(m_format->GetFlowDirection()); });
    }


    IFACEMETHODIMP CanvasTextFormat::put_FlowDirection(CanvasTextDirection value)
    {
        return PropertyPut(
            value, 
            &m_flowDirection, 
            ThrowIfInvalid<CanvasTextDirection>,
            &CanvasTextFormat::RealizeFlowDirection);
    }


    void CanvasTextFormat::RealizeFlowDirection()
    {
        ThrowIfFailed(m_format->SetFlowDirection(ToFlowDirection(m_flowDirection)));        
    }

    //
    // CanvasTextFormat.FontFamilyName
    //

    IFACEMETHODIMP CanvasTextFormat::get_FontFamilyName(HSTRING* value)
    {
        return PropertyGet(
            value,
            m_fontFamilyName,
            [&]()
            {
                WinStringBuilder stringBuilder;
                auto length = m_format->GetFontFamilyNameLength() + 1;
                auto buffer = stringBuilder.Allocate(length);
                ThrowIfFailed(m_format->GetFontFamilyName(buffer, length));
                return stringBuilder.Get();
            });
    }


    IFACEMETHODIMP CanvasTextFormat::put_FontFamilyName(HSTRING value)
    {
        return PropertyPut(
            value,
            &m_fontFamilyName,
            &CanvasTextFormat::Unrealize);
    }

    //
    // CanvasTextFormat.FontSize
    //

    IFACEMETHODIMP CanvasTextFormat::get_FontSize(float* value)
    {
        return PropertyGet(
            value,
            m_fontSize,
            [&]() { return m_format->GetFontSize(); });
    }


    IFACEMETHODIMP CanvasTextFormat::put_FontSize(float value)
    {
        return PropertyPut(
            value, 
            &m_fontSize,
            ThrowIfNegativeOrNan,
            &CanvasTextFormat::Unrealize);
    }

    //
    // CanvasTextFormat.FontStretch
    //

    IFACEMETHODIMP CanvasTextFormat::get_FontStretch(ABI::Windows::UI::Text::FontStretch* value)
    {
        return PropertyGet(
            value,
            m_fontStretch,
            [&]() { return ToWindowsFontStretch(m_format->GetFontStretch()); });
    }


    IFACEMETHODIMP CanvasTextFormat::put_FontStretch(ABI::Windows::UI::Text::FontStretch value)
    {
        return PropertyPut(
            value, 
            &m_fontStretch, 
            ThrowIfInvalid<ABI::Windows::UI::Text::FontStretch>,
            &CanvasTextFormat::Unrealize);
    }

    //
    // CanvasTextFormat.FontStyle
    //

    IFACEMETHODIMP CanvasTextFormat::get_FontStyle(ABI::Windows::UI::Text::FontStyle* value)
    {
        return PropertyGet(
            value,
            m_fontStyle,
            [&]() { return ToWindowsFontStyle(m_format->GetFontStyle()); });
    }


    IFACEMETHODIMP CanvasTextFormat::put_FontStyle(ABI::Windows::UI::Text::FontStyle value)
    {
        return PropertyPut(
            value, 
            &m_fontStyle, 
            ThrowIfInvalid<ABI::Windows::UI::Text::FontStyle>,
            &CanvasTextFormat::Unrealize);
    }

    //
    // CanvasTextFormat.FontWeight
    // 

    IFACEMETHODIMP CanvasTextFormat::get_FontWeight(ABI::Windows::UI::Text::FontWeight* value)
    {
        return PropertyGet(
            value,
            m_fontWeight,
            [&]() { return ToWindowsFontWeight(m_format->GetFontWeight()); });
    }


    IFACEMETHODIMP CanvasTextFormat::put_FontWeight(ABI::Windows::UI::Text::FontWeight value)
    {
        return PropertyPut(
            value, 
            &m_fontWeight,
            ThrowIfInvalidFontWeight,
            &CanvasTextFormat::Unrealize);
    }

    //
    // CanvasTextFormat.IncrementalTabTop
    //

    IFACEMETHODIMP CanvasTextFormat::get_IncrementalTabStop(float* value)
    {
        return PropertyGet(
            value,
            m_incrementalTabStop,
            [&]() { return m_format->GetIncrementalTabStop(); });
    }


    IFACEMETHODIMP CanvasTextFormat::put_IncrementalTabStop(float value)
    {
        return PropertyPut(
            value, 
            &m_incrementalTabStop, 
            ThrowIfNegativeOrNan,
            &CanvasTextFormat::RealizeIncrementalTabStop);
    }


    void CanvasTextFormat::RealizeIncrementalTabStop()
    {
        // Negative value indicates that it hasn't been set yet, so we want to
        // use dwrite's default.
        if (m_incrementalTabStop >= 0.0f)
            ThrowIfFailed(m_format->SetIncrementalTabStop(m_incrementalTabStop));
    }

    //
    // CanvasTextFormat.LineSpacingMethod
    //

    IFACEMETHODIMP CanvasTextFormat::get_LineSpacingMethod(CanvasLineSpacingMethod* value)
    {
        return PropertyGet(
            value,
            m_lineSpacingMethod,
            [&]() { return ToCanvasLineSpacingMethod(DWriteLineSpacing(m_format.Get()).Method); });
    }


    IFACEMETHODIMP CanvasTextFormat::put_LineSpacingMethod(CanvasLineSpacingMethod value)
    {
        return PropertyPut(
            value, 
            &m_lineSpacingMethod, 
            ThrowIfInvalid<CanvasLineSpacingMethod>,
            &CanvasTextFormat::RealizeLineSpacing);
    }

    //
    // CanvasTextFormat.LineSpacing
    //

    IFACEMETHODIMP CanvasTextFormat::get_LineSpacing(float* value)
    {
        return PropertyGet(
            value,
            m_lineSpacing,
            [&]() { return DWriteLineSpacing(m_format.Get()).Spacing; });
    }


    IFACEMETHODIMP CanvasTextFormat::put_LineSpacing(float value)
    {
        return PropertyPut(
            value, 
            &m_lineSpacing, 
            ThrowIfNegativeOrNan,
            &CanvasTextFormat::RealizeLineSpacing);
    }

    //
    // CanvasTextFormat.LineSpacingBaseline
    //

    IFACEMETHODIMP CanvasTextFormat::get_LineSpacingBaseline(float* value)
    {
        return PropertyGet(
            value,
            m_lineSpacingBaseline,
            [&]() { return DWriteLineSpacing(m_format.Get()).Baseline; });
    }


    IFACEMETHODIMP CanvasTextFormat::put_LineSpacingBaseline(float value)
    {
        return PropertyPut(
            value, 
            &m_lineSpacingBaseline, 
            ThrowIfNan,
            &CanvasTextFormat::RealizeLineSpacing);
    }


    void CanvasTextFormat::RealizeLineSpacing()
    {
        ThrowIfFailed(m_format->SetLineSpacing(
            ToLineSpacingMethod(m_lineSpacingMethod),
            m_lineSpacing,
            m_lineSpacingBaseline));
    }

    //
    // CanvasTextFormat.LocaleName
    //

    IFACEMETHODIMP CanvasTextFormat::get_LocaleName(HSTRING* value)
    {
        return PropertyGet(
            value,
            m_localeName,
            [&]()
            {
                WinStringBuilder stringBuilder;
                auto length = m_format->GetLocaleNameLength() + 1;
                auto buffer = stringBuilder.Allocate(length);
                ThrowIfFailed(m_format->GetLocaleName(buffer, length));
                return stringBuilder.Get();
            });
    }


    IFACEMETHODIMP CanvasTextFormat::put_LocaleName(HSTRING value)
    {
        return PropertyPut(
            value, 
            &m_localeName,
            &CanvasTextFormat::Unrealize);
    }

    //
    // CanvasTextFormat.VerticalAlignment
    //

    IFACEMETHODIMP CanvasTextFormat::get_VerticalAlignment(CanvasVerticalAlignment* value)
    {
        // Canvas calls it "VerticalAlignment", but DWrite calls it "ParagraphAlignment"
        return PropertyGet(
            value,
            m_verticalAlignment,
            [&]() { return ToCanvasVerticalAlignment(m_format->GetParagraphAlignment()); });
    }


    IFACEMETHODIMP CanvasTextFormat::put_VerticalAlignment(CanvasVerticalAlignment value)
    {
        return PropertyPut(
            value, 
            &m_verticalAlignment,
            ThrowIfInvalid<CanvasVerticalAlignment>,
            &CanvasTextFormat::RealizeParagraphAlignment);
    }


    void CanvasTextFormat::RealizeParagraphAlignment()
    {
        ThrowIfFailed(m_format->SetParagraphAlignment(ToParagraphAlignment(m_verticalAlignment)));
    }

    //
    // CanvasTextFormat.ReadingDirection
    //

    IFACEMETHODIMP CanvasTextFormat::get_ReadingDirection(CanvasTextDirection* value)
    {
        return PropertyGet(
            value,
            m_readingDirection,
            [&]() { return ToCanvasReadingDirection(m_format->GetReadingDirection()); });
    }


    IFACEMETHODIMP CanvasTextFormat::put_ReadingDirection(CanvasTextDirection value)
    {
        return PropertyPut(
            value, 
            &m_readingDirection,
            ThrowIfInvalid<CanvasTextDirection>,
            &CanvasTextFormat::RealizeReadingDirection);
    }


    void CanvasTextFormat::RealizeReadingDirection()
    {
        ThrowIfFailed(m_format->SetReadingDirection(ToReadingDirection(m_readingDirection)));
    }

    //
    // CanvasTextFormat.ParagraphAlignment
    //

    IFACEMETHODIMP CanvasTextFormat::get_ParagraphAlignment(ABI::Windows::UI::Text::ParagraphAlignment* value)
    {
        // Canvas calls is "ParagraphAlignment", but DWrite calls it "TextAlignment"
        return PropertyGet(
            value,
            m_paragraphAlignment,
            [&]() { return ToWindowsParagraphAlignment(m_format->GetTextAlignment()); });
    }


    IFACEMETHODIMP CanvasTextFormat::put_ParagraphAlignment(ABI::Windows::UI::Text::ParagraphAlignment value)
    {
        return PropertyPut(
            value, 
            &m_paragraphAlignment,
            ThrowIfInvalid<ABI::Windows::UI::Text::ParagraphAlignment>,
            &CanvasTextFormat::RealizeTextAlignment);
    }

    void CanvasTextFormat::RealizeTextAlignment()
    {
        ThrowIfFailed(m_format->SetTextAlignment(ToTextAlignment(m_paragraphAlignment)));        
    }

    //
    // CanvasTextFormat.TrimmingGranulatity
    //

    IFACEMETHODIMP CanvasTextFormat::get_TrimmingGranularity(CanvasTextTrimmingGranularity* value)
    {
        return PropertyGet(
            value,
            m_trimmingGranularity,
            [&]() { return ToCanvasTextTrimmingGranularity(DWriteTrimming(m_format.Get()).Options.granularity); });
    }


    IFACEMETHODIMP CanvasTextFormat::put_TrimmingGranularity(CanvasTextTrimmingGranularity value)
    {
        return PropertyPut(
            value, 
            &m_trimmingGranularity,
            ThrowIfInvalid<CanvasTextTrimmingGranularity>,
            &CanvasTextFormat::RealizeTrimming);        
    }

    //
    // CanvasTextFormat.TrimmingDelimiter
    //

    IFACEMETHODIMP CanvasTextFormat::get_TrimmingDelimiter(HSTRING* value)
    {
        return PropertyGet(
            value,
            m_trimmingDelimiter,
            [&]() { return ToCanvasTrimmingDelimiter(DWriteTrimming(m_format.Get()).Options.delimiter); });
    }


    IFACEMETHODIMP CanvasTextFormat::put_TrimmingDelimiter(HSTRING value)
    {
        return PropertyPut(
            value, 
            &m_trimmingDelimiter,
            ThrowIfInvalidTrimmingDelimiter,
            &CanvasTextFormat::RealizeTrimming);
    }

    //
    // CanvasTextFormat.TrimmingDelimiterCount
    //

    IFACEMETHODIMP CanvasTextFormat::get_TrimmingDelimiterCount(int32_t* value)
    {
        return PropertyGet(
            value,
            m_trimmingDelimiterCount,
            [&]() { return static_cast<int32_t>(DWriteTrimming(m_format.Get()).Options.delimiterCount); });
    }


    IFACEMETHODIMP CanvasTextFormat::put_TrimmingDelimiterCount(int32_t value)
    {
        return PropertyPut(
            value, 
            &m_trimmingDelimiterCount,
            ThrowIfNegative<int32_t>,
            &CanvasTextFormat::RealizeTrimming);
    }


    void CanvasTextFormat::RealizeTrimming()
    {
        DWRITE_TRIMMING trimmingOptions{};
        trimmingOptions.granularity = ToTrimmingGranularity(m_trimmingGranularity);
        trimmingOptions.delimiter = ToTrimmingDelimiter(m_trimmingDelimiter);
        trimmingOptions.delimiterCount = m_trimmingDelimiterCount;
            
        ThrowIfFailed(m_format->SetTrimming(
            &trimmingOptions,
            nullptr));        
    }

    //
    // CanvasTextFormat.WordWrapping
    //

    IFACEMETHODIMP CanvasTextFormat::get_WordWrapping(CanvasWordWrapping* value)
    {
        return PropertyGet(
            value,
            m_wordWrapping,
            [&]() { return ToCanvasWordWrapping(m_format->GetWordWrapping()); });
    }


    IFACEMETHODIMP CanvasTextFormat::put_WordWrapping(CanvasWordWrapping value)
    {
        return PropertyPut(
            value, 
            &m_wordWrapping,
            ThrowIfInvalid<CanvasWordWrapping>,
            &CanvasTextFormat::RealizeWordWrapping);
    }


    void CanvasTextFormat::RealizeWordWrapping()
    {
        ThrowIfFailed(m_format->SetWordWrapping(ToWordWrapping(m_wordWrapping)));        
    }

    //
    // CanvasTextFormat.Options
    //

    IFACEMETHODIMP CanvasTextFormat::get_Options(CanvasDrawTextOptions* value)
    {
        return ExceptionBoundary(
            [&]()
            {
                CheckInPointer(value);
                ThrowIfClosed();
                *value = m_drawTextOptions;
            });
    }


    IFACEMETHODIMP CanvasTextFormat::put_Options(CanvasDrawTextOptions value)
    {
        return ExceptionBoundary(
            [&]()
            {
                ThrowIfClosed();

                auto validOptions = 
                    CanvasDrawTextOptions::NoSnap |
                    CanvasDrawTextOptions::Clip |
                    CanvasDrawTextOptions::EnableColorFont;

                if ((value & ~validOptions) != CanvasDrawTextOptions::Default)
                    ThrowHR(E_INVALIDARG);

                m_drawTextOptions = value;
            });
    }


    ActivatableClassWithFactory(CanvasTextFormat, CanvasTextFormatFactory);
}
