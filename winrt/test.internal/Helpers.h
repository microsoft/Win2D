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

namespace Microsoft
{
    namespace VisualStudio
    {
        namespace CppUnitTestFramework
        {

            // TODO #1716: update rest of file to use the ENUM_TO_STRING macro below

            template<>
            static inline std::wstring ToString<CanvasBackground>(const CanvasBackground& value)
            {
                switch (value)
                {
                case CanvasBackground::Transparent: return L"CanvasBackground::Transparent";
                case CanvasBackground::Opaque: return L"CanvasBackground::Opaque";
                default: return L"<<invalid CanvasBackground>>";
                }
            }

            template<>
            static inline std::wstring ToString<RECT>(const RECT& value)
            {
                wchar_t buf[256];
                ThrowIfFailed(StringCchPrintf(
                    buf,
                    _countof(buf),
                    L"RECT{%d,%d,%d,%d}",
                    value.left,
                    value.top,
                    value.right,
                    value.bottom));
                return buf;
            }

            template<>
            static inline std::wstring ToString<D2D1_POINT_2F>(const D2D1_POINT_2F& value)
            {
                wchar_t buf[256];
                ThrowIfFailed(StringCchPrintf(
                    buf,
                    _countof(buf),
                    L"D2D_POINT_2F{%f,%f}",
                    value.x,
                    value.y));
                return buf;
            }

            template<>
            static inline std::wstring ToString<D2D1_RECT_F>(const D2D1_RECT_F& value)
            {
                wchar_t buf[256];
                ThrowIfFailed(StringCchPrintf(
                    buf,
                    _countof(buf),
                    L"D2D_RECT_F{%f,%f,%f,%f}",
                    value.left,
                    value.top,
                    value.right,
                    value.bottom));
                return buf;
            }

            template<>
            static inline std::wstring ToString<D2D1_ROUNDED_RECT>(const D2D1_ROUNDED_RECT& roundedRect)
            {
                wchar_t buf[256];
                ThrowIfFailed(StringCchPrintf(
                    buf,
                    _countof(buf),
                    L"D2D_ROUNDED_RECT{%s,%f,%f}",
                    ToString(roundedRect.rect).c_str(),
                    roundedRect.radiusX,
                    roundedRect.radiusY));
                return buf;
            }

            template<>
            static inline std::wstring ToString<D2D1_ELLIPSE>(const D2D1_ELLIPSE& ellipse)
            {
                wchar_t buf[256];
                ThrowIfFailed(StringCchPrintf(
                    buf,
                    _countof(buf),
                    L"D2D_ELLIPSE{%s,%f,%f}",
                    ToString(ellipse.point).c_str(),
                    ellipse.radiusX,
                    ellipse.radiusY));
                return buf;
            }

            template<>
            static inline std::wstring ToString<IID>(const IID& iid)
            {
                wchar_t* iidString = nullptr;
                ThrowIfFailed(StringFromIID(iid, &iidString));
                std::wstring value(iidString);
                CoTaskMemFree(iidString);
                return value;
            }

            static inline std::wstring PointerToString(const wchar_t* name, void* value)
            {
                wchar_t buf[64];
                ThrowIfFailed(StringCchPrintf(
                    buf,
                    _countof(buf),
                    L"%s(%x)",
                    name,
                    value));
                return std::wstring(buf);
            }

#define TO_STRING(T)                                            \
            template<>                                          \
            static inline std::wstring ToString<T>(T* value)    \
            {                                                   \
                return PointerToString(L#T, value);             \
            }

            TO_STRING(ICanvasDevice);
            TO_STRING(ID2D1Brush);
            TO_STRING(ID2D1Device1);
            TO_STRING(ID2D1DeviceContext1);
            TO_STRING(ID2D1Factory);
            TO_STRING(IDWriteTextFormat);
            TO_STRING(IDXGIDevice);
            TO_STRING(IDirect3DDevice);
            TO_STRING(IInspectable);
            TO_STRING(IUnknown);
            TO_STRING(ID2D1StrokeStyle1);

#undef TO_STRING

            template<>
            static inline std::wstring ToString<CanvasDebugLevel>(const CanvasDebugLevel& value)
            {
                switch (value)
                {
                case CanvasDebugLevel::None: return L"CanvasDebugLevel::None";
                case CanvasDebugLevel::Error: return L"CanvasDebugLevel::Error";
                case CanvasDebugLevel::Warning: return L"CanvasDebugLevel::Warning";
                case CanvasDebugLevel::Information: return L"CanvasDebugLevel::Information";
                default: return L"<<invalid CanvasDebugLevel>>";
                }
            }

            template<>
            static inline std::wstring ToString<CanvasHardwareAcceleration>(const CanvasHardwareAcceleration& value)
            {
                switch (value)
                {
                case CanvasHardwareAcceleration::On: return L"CanvasHardwareAcceleration::On";
                case CanvasHardwareAcceleration::Off: return L"CanvasHardwareAcceleration::Off";
                default: return L"<<invalid CanvasHardwareAcceleration>>";
                }
            }

            template<>
            static inline std::wstring ToString<ABI::Windows::UI::Color>(const ABI::Windows::UI::Color& value)
            {
                wchar_t buf[256];
                ThrowIfFailed(StringCchPrintf(
                    buf,
                    _countof(buf),
                    L"Windows.UI.Color{A=%i,R=%i,G=%i,B=%i}",
                    value.A,
                    value.R,
                    value.G,
                    value.B));

                return buf;
            }

            template<>
            static inline std::wstring ToString<D2D1_COLOR_F>(const D2D1_COLOR_F& value)
            {
                wchar_t buf[256];
                ThrowIfFailed(StringCchPrintf(
                    buf,
                    _countof(buf),
                    L"D2D1_COLOR_F{A=%f,R=%f,G=%f,B=%f}",
                    value.a,
                    value.r,
                    value.g,
                    value.b));

                return buf;
            }

            template<>
            static inline std::wstring ToString<Numerics::Matrix3x2>(const Numerics::Matrix3x2& value)
            {
                wchar_t buf[256];
                ThrowIfFailed(StringCchPrintf(
                    buf,
                    _countof(buf),
                    L"Numerics.Matrix{M11=%f,M12=%f,M21=%f,M22=%f,M31=%f,M32=%f}",
                    value.M11, value.M12,
                    value.M21, value.M22,
                    value.M31, value.M32));

                return buf;
            }

            template<>
            static inline std::wstring ToString<D2D1_MATRIX_3X2_F>(const D2D1_MATRIX_3X2_F& value)
            {
                wchar_t buf[256];
                ThrowIfFailed(StringCchPrintf(
                    buf,
                    _countof(buf),
                    L"D2D1_MATRIX_3X2_F{_11=%f,_12=%f,_21=%f,_22=%f,_31=%f,_32=%f}",
                    value._11, value._12,
                    value._21, value._22,
                    value._31, value._32));

                return buf;
            }

            template<>
            static inline std::wstring ToString<CanvasCapStyle>(const CanvasCapStyle& value)
            {
                switch (value)
                {
                case CanvasCapStyle::Flat: return L"CanvasCapStyle::Flat";
                case CanvasCapStyle::Round: return L"CanvasCapStyle::Round";
                case CanvasCapStyle::Square: return L"CanvasCapStyle::Square";
                case CanvasCapStyle::Triangle: return L"CanvasCapStyle::Triangle";
                default: return L"<<invalid CanvasCapStyle>>";
                }
            }

            template<>
            static inline std::wstring ToString<CanvasTextDirection>(const CanvasTextDirection& value)
            {
                switch (value)
                {
                case CanvasTextDirection::TopToBottom: return L"CanvasTextDirection::TopToBottom";
                case CanvasTextDirection::BottomToTop: return L"CanvasTextDirection::BottomToTop";
                case CanvasTextDirection::LeftToRight: return L"CanvasTextDirection::LeftToRight";
                case CanvasTextDirection::RightToLeft: return L"CanvasTextDirection::RightToLeft";
                default: return L"<<invalid CanvasTextDirection>>";
                }
            }

            template<>
            static inline std::wstring ToString<CanvasLineJoin>(const CanvasLineJoin& value)
            {
                switch (value)
                {
                case CanvasLineJoin::Miter: return L"CanvasLineJoin::Miter";
                case CanvasLineJoin::Bevel: return L"CanvasLineJoin::Bevel";
                case CanvasLineJoin::Round: return L"CanvasLineJoin::Round";
                case CanvasLineJoin::MiterOrBevel: return L"CanvasLineJoin::MiterOrBevel";
                default: return L"<<invalid CanvasLineJoin>>";
                }
            }

            template<>
            static inline std::wstring ToString<CanvasDashStyle>(const CanvasDashStyle& value)
            {
                switch (value)
                {
                case CanvasDashStyle::Solid: return L"CanvasDashStyle::Solid";
                case CanvasDashStyle::Dash: return L"CanvasDashStyle::Dash";
                case CanvasDashStyle::Dot: return L"CanvasDashStyle::Dot";
                case CanvasDashStyle::DashDot: return L"CanvasDashStyle::DashDot";
                case CanvasDashStyle::DashDotDot: return L"CanvasDashStyle::DashDotDot";
                default: return L"<<invalid CanvasDashStyle>>";
                }
            }

            template<>
            static inline std::wstring ToString<CanvasStrokeTransformBehavior>(const CanvasStrokeTransformBehavior& value)
            {
                switch (value)
                {
                case CanvasStrokeTransformBehavior::Normal: return L"CanvasStrokeTransformBehavior::Normal";
                case CanvasStrokeTransformBehavior::Fixed: return L"CanvasStrokeTransformBehavior::Fixed";
                case CanvasStrokeTransformBehavior::Hairline: return L"CanvasStrokeTransformBehavior::Hairline";
                default: return L"<<invalid CanvasStrokeTransformBehavior>>";
                }
            }

            template<>
            static inline std::wstring ToString<D2D1_CAP_STYLE>(const D2D1_CAP_STYLE& value)
            {
                switch (value)
                {
                case D2D1_CAP_STYLE_FLAT: return L"D2D1_CAP_STYLE_FLAT";
                case D2D1_CAP_STYLE_SQUARE: return L"D2D1_CAP_STYLE_SQUARE";
                case D2D1_CAP_STYLE_ROUND: return L"D2D1_CAP_STYLE_ROUND";
                case D2D1_CAP_STYLE_TRIANGLE: return L"D2D1_CAP_STYLE_TRIANGLE";
                default: return L"<<invalid D2D1_CAP_STYLE>>";
                }
            }

            template<>
            static inline std::wstring ToString<D2D1_LINE_JOIN>(const D2D1_LINE_JOIN& value)
            {
                switch (value)
                {
                case D2D1_LINE_JOIN_MITER: return L"D2D1_LINE_JOIN_MITER";
                case D2D1_LINE_JOIN_BEVEL: return L"D2D1_LINE_JOIN_BEVEL";
                case D2D1_LINE_JOIN_ROUND: return L"D2D1_LINE_JOIN_ROUND";
                case D2D1_LINE_JOIN_MITER_OR_BEVEL: return L"D2D1_LINE_JOIN_MITER_OR_BEVEL";
                default: return L"<<invalid D2D1_LINE_JOIN>>";
                }
            }

            template<>
            static inline std::wstring ToString<D2D1_DASH_STYLE>(const D2D1_DASH_STYLE& value)
            {
                switch (value)
                {
                case D2D1_DASH_STYLE_SOLID: return L"D2D1_DASH_STYLE_SOLID";
                case D2D1_DASH_STYLE_DASH: return L"D2D1_DASH_STYLE_DASH";
                case D2D1_DASH_STYLE_DOT: return L"D2D1_DASH_STYLE_DOT";
                case D2D1_DASH_STYLE_DASH_DOT: return L"D2D1_DASH_STYLE_DASH_DOT";
                case D2D1_DASH_STYLE_DASH_DOT_DOT: return L"D2D1_DASH_STYLE_DASH_DOT_DOT";
                case D2D1_DASH_STYLE_CUSTOM: return L"D2D1_DASH_STYLE_CUSTOM";
                default: return L"<<invalid D2D1_DASH_STYLE>>";
                }
            }

            template<>
            static inline std::wstring ToString<D2D1_STROKE_TRANSFORM_TYPE>(const D2D1_STROKE_TRANSFORM_TYPE& value)
            {
                switch (value)
                {
                case D2D1_STROKE_TRANSFORM_TYPE_NORMAL: return L"D2D1_STROKE_TRANSFORM_TYPE_NORMAL";
                case D2D1_STROKE_TRANSFORM_TYPE_FIXED: return L"D2D1_STROKE_TRANSFORM_TYPE_FIXED";
                case D2D1_STROKE_TRANSFORM_TYPE_HAIRLINE: return L"D2D1_STROKE_TRANSFORM_TYPE_HAIRLINE";
                default: return L"<<invalid D2D1_STROKE_TRANSFORM_TYPE>>";
                }
            }

            template<>
            static inline std::wstring ToString<DWRITE_FLOW_DIRECTION>(const DWRITE_FLOW_DIRECTION& value)
            {
                switch (value)
                {
                case DWRITE_FLOW_DIRECTION_TOP_TO_BOTTOM: return L"DWRITE_FLOW_DIRECTION_TOP_TO_BOTTOM";
                case DWRITE_FLOW_DIRECTION_BOTTOM_TO_TOP: return L"DWRITE_FLOW_DIRECTION_BOTTOM_TO_TOP";
                case DWRITE_FLOW_DIRECTION_LEFT_TO_RIGHT: return L"DWRITE_FLOW_DIRECTION_LEFT_TO_RIGHT";
                case DWRITE_FLOW_DIRECTION_RIGHT_TO_LEFT: return L"DWRITE_FLOW_DIRECTION_RIGHT_TO_LEFT";
                default: return L"<<invalid DWRITE_FLOW_DIRECTION>>";
                }
            }

            template<>
            static inline std::wstring ToString<CanvasLineSpacingMethod>(const CanvasLineSpacingMethod& value)
            {
                switch (value)
                {
                case CanvasLineSpacingMethod::Default: return L"CanvasLineSpacingMethod::Default";
                case CanvasLineSpacingMethod::Uniform: return L"CanvasLineSpacingMethod::Uniform";
                default: return L"<<invalid CanvasLineSpacingMethod>>";
                }
            }

            template<>
            static inline std::wstring ToString<DWRITE_LINE_SPACING_METHOD>(const DWRITE_LINE_SPACING_METHOD& value)
            {
                switch (value)
                {
                case DWRITE_LINE_SPACING_METHOD_DEFAULT: return L"DWRITE_LINE_SPACING_METHOD_DEFAULT";
                case DWRITE_LINE_SPACING_METHOD_UNIFORM: return L"DWRITE_LINE_SPACING_METHOD_UNIFORM";
                default: return L"<<invalid DWRITE_LINE_SPACING_METHOD>>";
                }
            }

            template<>
            static inline std::wstring ToString<ABI::Windows::UI::Text::FontStretch>(const ABI::Windows::UI::Text::FontStretch& value)
            {
                switch (value)
                {
                case ABI::Windows::UI::Text::FontStretch_Undefined: return L"FontStretch::Undefined";
                case ABI::Windows::UI::Text::FontStretch_UltraCondensed: return L"FontStretch::UltraCondensed";
                case ABI::Windows::UI::Text::FontStretch_ExtraCondensed: return L"FontStretch::ExtraCondensed";
                case ABI::Windows::UI::Text::FontStretch_Condensed: return L"FontStretch::Condensed";
                case ABI::Windows::UI::Text::FontStretch_SemiCondensed: return L"FontStretch::SemiCondensed";
                case ABI::Windows::UI::Text::FontStretch_Normal: return L"FontStretch::Normal";
                case ABI::Windows::UI::Text::FontStretch_SemiExpanded: return L"FontStretch::SemiExpanded";
                case ABI::Windows::UI::Text::FontStretch_Expanded: return L"FontStretch::Expanded";
                case ABI::Windows::UI::Text::FontStretch_ExtraExpanded: return L"FontStretch::ExtraExpanded";
                case ABI::Windows::UI::Text::FontStretch_UltraExpanded: return L"FontStretch::UltraExpanded";
                default: return L"<<invalid FontStretch>>";
                }
            }

            template<>
            static inline std::wstring ToString<DWRITE_FONT_STRETCH>(const DWRITE_FONT_STRETCH& value)
            {
                switch (value)
                {
                case DWRITE_FONT_STRETCH_UNDEFINED: return L"DWRITE_FONT_STRETCH_UNDEFINED";
                case DWRITE_FONT_STRETCH_ULTRA_CONDENSED: return L"DWRITE_FONT_STRETCH_ULTRA_CONDENSED";
                case DWRITE_FONT_STRETCH_EXTRA_CONDENSED: return L"DWRITE_FONT_STRETCH_EXTRA_CONDENSED";
                case DWRITE_FONT_STRETCH_CONDENSED: return L"DWRITE_FONT_STRETCH_CONDENSED";
                case DWRITE_FONT_STRETCH_SEMI_CONDENSED: return L"DWRITE_FONT_STRETCH_SEMI_CONDENSED";
                case DWRITE_FONT_STRETCH_NORMAL: return L"DWRITE_FONT_STRETCH_NORMAL/MEDIUM";
                case DWRITE_FONT_STRETCH_SEMI_EXPANDED: return L"DWRITE_FONT_STRETCH_SEMI_EXPANDED";
                case DWRITE_FONT_STRETCH_EXPANDED: return L"DWRITE_FONT_STRETCH_EXPANDED";
                case DWRITE_FONT_STRETCH_EXTRA_EXPANDED: return L"DWRITE_FONT_STRETCH_EXTRA_EXPANDED";
                case DWRITE_FONT_STRETCH_ULTRA_EXPANDED: return L"DWRITE_FONT_STRETCH_ULTRA_EXPANDED";
                default: return L"<<invalid DWRITE_FONT_STRETCH>>";
                }
            }

            template<>
            static inline std::wstring ToString<ABI::Windows::UI::Text::FontStyle>(const ABI::Windows::UI::Text::FontStyle& value)
            {
                switch (value)
                {
                case ABI::Windows::UI::Text::FontStyle_Normal: return L"FontStyle::Normal";
                case ABI::Windows::UI::Text::FontStyle_Oblique: return L"FontStyle::Oblique";
                case ABI::Windows::UI::Text::FontStyle_Italic: return L"FontStyle::Italic";
                default: return L"<<invalid FontStyle>>";
                }
            }

            template<>
            static inline std::wstring ToString<DWRITE_FONT_STYLE>(const DWRITE_FONT_STYLE& value)
            {
                switch (value)
                {
                case DWRITE_FONT_STYLE_NORMAL: return L"DWRITE_FONT_STYLE_NORMAL";
                case DWRITE_FONT_STYLE_OBLIQUE: return L"DWRITE_FONT_STYLE_OBLIQUE";
                case DWRITE_FONT_STYLE_ITALIC: return L"DWRITE_FONT_STYLE_ITALIC";
                default: return L"<<invalid DWRITE_FONT_STYLE>>";
                }
            }

#define ENUM_TO_STRING(TYPE)                                            \
    template<> static inline std::wstring ToString<TYPE>(const TYPE& value) \
    {                                                                   \
        switch (value)                                                  

#define ENUM_VALUE(NAME) case NAME: return WIDEN(#NAME)
#define END_ENUM(TYPE) default: return L"<<invalid " WIDEN(#TYPE) L">>"; }

            ENUM_TO_STRING(CanvasVerticalAlignment)
            {
                ENUM_VALUE(CanvasVerticalAlignment::Top);
                ENUM_VALUE(CanvasVerticalAlignment::Bottom);
                ENUM_VALUE(CanvasVerticalAlignment::Center);
                END_ENUM(CanvasVerticalAlignment);
            }
            
            ENUM_TO_STRING(DWRITE_PARAGRAPH_ALIGNMENT)
            {
                ENUM_VALUE(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
                ENUM_VALUE(DWRITE_PARAGRAPH_ALIGNMENT_FAR);
                ENUM_VALUE(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
                END_ENUM(DWRITE_PARAGRAPH_ALIGNMENT);
            }

            ENUM_TO_STRING(DWRITE_READING_DIRECTION)
            {
                ENUM_VALUE(DWRITE_READING_DIRECTION_LEFT_TO_RIGHT);
                ENUM_VALUE(DWRITE_READING_DIRECTION_RIGHT_TO_LEFT);
                ENUM_VALUE(DWRITE_READING_DIRECTION_TOP_TO_BOTTOM);
                ENUM_VALUE(DWRITE_READING_DIRECTION_BOTTOM_TO_TOP);
                END_ENUM(DWRITE_READING_DIRECTION);
            }

            ENUM_TO_STRING(ABI::Windows::UI::Text::ParagraphAlignment)
            {
                ENUM_VALUE(ABI::Windows::UI::Text::ParagraphAlignment_Undefined);
                ENUM_VALUE(ABI::Windows::UI::Text::ParagraphAlignment_Left);
                ENUM_VALUE(ABI::Windows::UI::Text::ParagraphAlignment_Center);
                ENUM_VALUE(ABI::Windows::UI::Text::ParagraphAlignment_Right);
                ENUM_VALUE(ABI::Windows::UI::Text::ParagraphAlignment_Justify);
                END_ENUM(ABI::Windows::UI::Text::ParagraphAlignment);
            }

            ENUM_TO_STRING(DWRITE_TEXT_ALIGNMENT)
            {
                ENUM_VALUE(DWRITE_TEXT_ALIGNMENT_LEADING);
                ENUM_VALUE(DWRITE_TEXT_ALIGNMENT_TRAILING);
                ENUM_VALUE(DWRITE_TEXT_ALIGNMENT_CENTER);
                ENUM_VALUE(DWRITE_TEXT_ALIGNMENT_JUSTIFIED);
                END_ENUM(DWRITE_TEXT_ALIGNMENT);
            }

            ENUM_TO_STRING(CanvasTextTrimmingGranularity)
            {
                ENUM_VALUE(CanvasTextTrimmingGranularity::None);
                ENUM_VALUE(CanvasTextTrimmingGranularity::Character);
                ENUM_VALUE(CanvasTextTrimmingGranularity::Word);
                END_ENUM(CanvasTextTrimmingGranularity);
            }
            
            ENUM_TO_STRING(DWRITE_TRIMMING_GRANULARITY)
            {
                ENUM_VALUE(DWRITE_TRIMMING_GRANULARITY_NONE);
                ENUM_VALUE(DWRITE_TRIMMING_GRANULARITY_CHARACTER);
                ENUM_VALUE(DWRITE_TRIMMING_GRANULARITY_WORD);
                END_ENUM(DWRITE_TRIMMING_GRANULARITY);
            }

            ENUM_TO_STRING(CanvasWordWrapping)
            {
                ENUM_VALUE(CanvasWordWrapping::Wrap);
                ENUM_VALUE(CanvasWordWrapping::NoWrap);
                ENUM_VALUE(CanvasWordWrapping::EmergencyBreak);
                ENUM_VALUE(CanvasWordWrapping::WholeWord);
                ENUM_VALUE(CanvasWordWrapping::Character);
                END_ENUM(CanvasWordWrapping);
            }

            ENUM_TO_STRING(DWRITE_WORD_WRAPPING)
            {
                ENUM_VALUE(DWRITE_WORD_WRAPPING_WRAP);
                ENUM_VALUE(DWRITE_WORD_WRAPPING_NO_WRAP);
                ENUM_VALUE(DWRITE_WORD_WRAPPING_EMERGENCY_BREAK);
                ENUM_VALUE(DWRITE_WORD_WRAPPING_WHOLE_WORD);
                ENUM_VALUE(DWRITE_WORD_WRAPPING_CHARACTER);
                END_ENUM(DWRITE_WORD_WRAPPING);
            }

            template<typename T>
            static inline std::wstring ToStringAsInt(T value)
            {
                wchar_t buf[256];
                ThrowIfFailed(StringCchPrintf(
                    buf,
                    _countof(buf),
                    L"%d",
                    static_cast<int>(value)));
                return buf;
            }

            template<>
            static inline std::wstring ToString<ABI::Windows::UI::Text::FontWeight>(const ABI::Windows::UI::Text::FontWeight& value)
            {
                return ToStringAsInt(value.Weight);
            }

#define TO_STRING_AS_INT(TYPE)                                          \
            template<>                                                  \
            static inline std::wstring ToString<TYPE>(const TYPE& value) \
            {                                                           \
                return ToStringAsInt(value);                            \
            }

            TO_STRING_AS_INT(DWRITE_FONT_WEIGHT);
            TO_STRING_AS_INT(CanvasDrawTextOptions);
            TO_STRING_AS_INT(D2D1_DRAW_TEXT_OPTIONS);
        }

        inline bool operator==(const D2D1_POINT_2F& a, const D2D1_POINT_2F& b)
        {
            return a.x == b.x &&
                a.y == b.y;
        }

        inline bool operator==(const D2D1_RECT_F& a, const D2D1_RECT_F& b)
        {
            return a.left == b.left &&
                a.top == b.top &&
                a.right == b.right &&
                a.bottom == b.bottom;
        }

        inline bool operator==(const D2D1_ROUNDED_RECT& a, const D2D1_ROUNDED_RECT& b)
        {
            return a.rect == b.rect &&
                a.radiusX == b.radiusX &&
                a.radiusY == b.radiusY;
        }

        inline bool operator==(const D2D1_ELLIPSE& a, const D2D1_ELLIPSE& b)
        {
            return a.point == b.point &&
                a.radiusX == b.radiusX &&
                a.radiusY == b.radiusY;
        }

        inline bool operator==(const ABI::Windows::UI::Color& a, const ABI::Windows::UI::Color& b)
        {
            return a.A == b.A &&
                a.R == b.R &&
                a.G == b.G &&
                a.B == b.B;
        }

        inline bool operator==(const D2D1_COLOR_F& a, const D2D1_COLOR_F& b)
        {
            return a.a == b.a &&
                a.r == b.r &&
                a.g == b.g &&
                a.b == b.b;
        }

        inline bool operator==(const Numerics::Matrix3x2& a, const Numerics::Matrix3x2& b)
        {
            return
                a.M11 == b.M11 && a.M12 == b.M12 &&
                a.M21 == b.M21 && a.M22 == b.M22 &&
                a.M31 == b.M31 && a.M32 == b.M32;
        }

        inline bool operator==(const D2D1_MATRIX_3X2_F& a, const D2D1_MATRIX_3X2_F& b)
        {
            return
                a._11 == b._11 && a._12 == b._12 &&
                a._21 == b._21 && a._22 == b._22 &&
                a._31 == b._31 && a._32 == b._32;
        }

        inline bool operator==(const ABI::Windows::UI::Text::FontWeight& a, const ABI::Windows::UI::Text::FontWeight& b)
        {
            return a.Weight == b.Weight;
        }
    }
}

#define ASSERT_IMPLEMENTS_INTERFACE(obj, INTERFACE)                     \
    do                                                                  \
    {                                                                   \
        ComPtr<INTERFACE> iface;                                        \
        Assert::AreEqual(S_OK, obj.As(&iface), L"Implements " L#INTERFACE); \
    } while(false)


