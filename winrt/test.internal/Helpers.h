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

#define ENUM_TO_STRING(TYPE)                                                        \
            template<> static inline std::wstring ToString<TYPE>(const TYPE& value) \
            {                                                                       \
                switch (value)

#define ENUM_VALUE(NAME)                                                            \
                case NAME:                                                          \
                    return WIDEN(#NAME)

#define END_ENUM(TYPE)                                                              \
                default:                                                            \
                    return L"<<invalid " WIDEN(#TYPE) L">>";                        \
                }

            ENUM_TO_STRING(CanvasBackground)
            {
                ENUM_VALUE(CanvasBackground::Transparent);
                ENUM_VALUE(CanvasBackground::Opaque);
                END_ENUM(CanvasBackground);
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
            TO_STRING(ICanvasDrawingSession);
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

            ENUM_TO_STRING(CanvasDebugLevel)
            {
                ENUM_VALUE(CanvasDebugLevel::None);
                ENUM_VALUE(CanvasDebugLevel::Error);
                ENUM_VALUE(CanvasDebugLevel::Warning);
                ENUM_VALUE(CanvasDebugLevel::Information);
                END_ENUM(CanvasDebugLevel);
            }

            ENUM_TO_STRING(CanvasHardwareAcceleration)
            {
                ENUM_VALUE(CanvasHardwareAcceleration::On);
                ENUM_VALUE(CanvasHardwareAcceleration::Off);
                END_ENUM(CanvasHardwareAcceleration);
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

            ENUM_TO_STRING(CanvasCapStyle)
            {
                ENUM_VALUE(CanvasCapStyle::Flat);
                ENUM_VALUE(CanvasCapStyle::Round);
                ENUM_VALUE(CanvasCapStyle::Square);
                ENUM_VALUE(CanvasCapStyle::Triangle);
                END_ENUM(CanvasCapStyle);
            }

            ENUM_TO_STRING(CanvasTextDirection)
            {
                ENUM_VALUE(CanvasTextDirection::TopToBottom);
                ENUM_VALUE(CanvasTextDirection::BottomToTop);
                ENUM_VALUE(CanvasTextDirection::LeftToRight);
                ENUM_VALUE(CanvasTextDirection::RightToLeft);
                END_ENUM(CanvasTextDirection);
            }

            ENUM_TO_STRING(CanvasLineJoin)
            {
                ENUM_VALUE(CanvasLineJoin::Miter);
                ENUM_VALUE(CanvasLineJoin::Bevel);
                ENUM_VALUE(CanvasLineJoin::Round);
                ENUM_VALUE(CanvasLineJoin::MiterOrBevel);
                END_ENUM(CanvasLineJoin);
            }

            ENUM_TO_STRING(CanvasDashStyle)
            {
                ENUM_VALUE(CanvasDashStyle::Solid);
                ENUM_VALUE(CanvasDashStyle::Dash);
                ENUM_VALUE(CanvasDashStyle::Dot);
                ENUM_VALUE(CanvasDashStyle::DashDot);
                ENUM_VALUE(CanvasDashStyle::DashDotDot);
                END_ENUM(CanvasDashStyle);
            }

            ENUM_TO_STRING(CanvasStrokeTransformBehavior)
            {
                ENUM_VALUE(CanvasStrokeTransformBehavior::Normal);
                ENUM_VALUE(CanvasStrokeTransformBehavior::Fixed);
                ENUM_VALUE(CanvasStrokeTransformBehavior::Hairline);
                END_ENUM(CanvasStrokeTransformBehavior);
            }

            ENUM_TO_STRING(D2D1_CAP_STYLE)
            {
                ENUM_VALUE(D2D1_CAP_STYLE_FLAT);
                ENUM_VALUE(D2D1_CAP_STYLE_SQUARE);
                ENUM_VALUE(D2D1_CAP_STYLE_ROUND);
                ENUM_VALUE(D2D1_CAP_STYLE_TRIANGLE);
                END_ENUM(D2D1_CAP_STYLE_TRIANGLE);
            }

            ENUM_TO_STRING(D2D1_LINE_JOIN)
            {
                ENUM_VALUE(D2D1_LINE_JOIN_MITER);
                ENUM_VALUE(D2D1_LINE_JOIN_BEVEL);
                ENUM_VALUE(D2D1_LINE_JOIN_ROUND);
                ENUM_VALUE(D2D1_LINE_JOIN_MITER_OR_BEVEL);
                END_ENUM(D2D1_LINE_JOIN_MITER_OR_BEVEL);
            }

            ENUM_TO_STRING(D2D1_DASH_STYLE)
            {
                ENUM_VALUE(D2D1_DASH_STYLE_SOLID);
                ENUM_VALUE(D2D1_DASH_STYLE_DASH);
                ENUM_VALUE(D2D1_DASH_STYLE_DOT);
                ENUM_VALUE(D2D1_DASH_STYLE_DASH_DOT);
                ENUM_VALUE(D2D1_DASH_STYLE_DASH_DOT_DOT);
                ENUM_VALUE(D2D1_DASH_STYLE_CUSTOM);
                END_ENUM(D2D1_DASH_STYLE_CUSTOM);
            }

            ENUM_TO_STRING(D2D1_STROKE_TRANSFORM_TYPE)
            {
                ENUM_VALUE(D2D1_STROKE_TRANSFORM_TYPE_NORMAL);
                ENUM_VALUE(D2D1_STROKE_TRANSFORM_TYPE_FIXED);
                ENUM_VALUE(D2D1_STROKE_TRANSFORM_TYPE_HAIRLINE);
                END_ENUM(D2D1_STROKE_TRANSFORM_TYPE_HAIRLINE);
            }

            template<>
            static inline std::wstring ToString<__int64>(const __int64& value)
            {
                wchar_t buf[256];
                ThrowIfFailed(StringCchPrintf(
                    buf,
                    _countof(buf),
                    L"%I64d",
                    value));

                return buf;
            }

            ENUM_TO_STRING(DWRITE_FLOW_DIRECTION)
            {
                ENUM_VALUE(DWRITE_FLOW_DIRECTION_TOP_TO_BOTTOM);
                ENUM_VALUE(DWRITE_FLOW_DIRECTION_BOTTOM_TO_TOP);
                ENUM_VALUE(DWRITE_FLOW_DIRECTION_LEFT_TO_RIGHT);
                ENUM_VALUE(DWRITE_FLOW_DIRECTION_RIGHT_TO_LEFT);
                END_ENUM(DWRITE_FLOW_DIRECTION_RIGHT_TO_LEFT);
            }

            ENUM_TO_STRING(CanvasLineSpacingMethod)
            {
                ENUM_VALUE(CanvasLineSpacingMethod::Default);
                ENUM_VALUE(CanvasLineSpacingMethod::Uniform);
                END_ENUM(CanvasLineSpacingMethod);
            }

            ENUM_TO_STRING(DWRITE_LINE_SPACING_METHOD)
            {
                ENUM_VALUE(DWRITE_LINE_SPACING_METHOD_DEFAULT);
                ENUM_VALUE(DWRITE_LINE_SPACING_METHOD_UNIFORM);
                END_ENUM(DWRITE_LINE_SPACING_METHOD);
            }

            ENUM_TO_STRING(ABI::Windows::UI::Text::FontStretch)
            {
                ENUM_VALUE(ABI::Windows::UI::Text::FontStretch_Undefined);
                ENUM_VALUE(ABI::Windows::UI::Text::FontStretch_UltraCondensed);
                ENUM_VALUE(ABI::Windows::UI::Text::FontStretch_ExtraCondensed);
                ENUM_VALUE(ABI::Windows::UI::Text::FontStretch_Condensed);
                ENUM_VALUE(ABI::Windows::UI::Text::FontStretch_SemiCondensed);
                ENUM_VALUE(ABI::Windows::UI::Text::FontStretch_Normal);
                ENUM_VALUE(ABI::Windows::UI::Text::FontStretch_SemiExpanded);
                ENUM_VALUE(ABI::Windows::UI::Text::FontStretch_Expanded);
                ENUM_VALUE(ABI::Windows::UI::Text::FontStretch_ExtraExpanded);
                ENUM_VALUE(ABI::Windows::UI::Text::FontStretch_UltraExpanded);
                END_ENUM(ABI);
            }

            ENUM_TO_STRING(DWRITE_FONT_STRETCH)
            {
                ENUM_VALUE(DWRITE_FONT_STRETCH_UNDEFINED);
                ENUM_VALUE(DWRITE_FONT_STRETCH_ULTRA_CONDENSED);
                ENUM_VALUE(DWRITE_FONT_STRETCH_EXTRA_CONDENSED);
                ENUM_VALUE(DWRITE_FONT_STRETCH_CONDENSED);
                ENUM_VALUE(DWRITE_FONT_STRETCH_SEMI_CONDENSED);
                ENUM_VALUE(DWRITE_FONT_STRETCH_NORMAL);
                ENUM_VALUE(DWRITE_FONT_STRETCH_SEMI_EXPANDED);
                ENUM_VALUE(DWRITE_FONT_STRETCH_EXPANDED);
                ENUM_VALUE(DWRITE_FONT_STRETCH_EXTRA_EXPANDED);
                ENUM_VALUE(DWRITE_FONT_STRETCH_ULTRA_EXPANDED);
                END_ENUM(DWRITE_FONT_STRETCH_ULTRA_EXPANDED);
            }

            ENUM_TO_STRING(ABI::Windows::UI::Text::FontStyle)
            {
                ENUM_VALUE(ABI::Windows::UI::Text::FontStyle_Normal);
                ENUM_VALUE(ABI::Windows::UI::Text::FontStyle_Oblique);
                ENUM_VALUE(ABI::Windows::UI::Text::FontStyle_Italic);
                END_ENUM(ABI);
            }

            ENUM_TO_STRING(DWRITE_FONT_STYLE)
            {
                ENUM_VALUE(DWRITE_FONT_STYLE_NORMAL);
                ENUM_VALUE(DWRITE_FONT_STYLE_OBLIQUE);
                ENUM_VALUE(DWRITE_FONT_STYLE_ITALIC);
                END_ENUM(DWRITE_FONT_STYLE_ITALIC);
            }

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


