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
            template<> static inline std::wstring ToString<TYPE>(TYPE const& value) \
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
            static inline std::wstring ToString<RECT>(RECT const& value)
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
            static inline std::wstring ToString<D2D1_POINT_2F>(D2D1_POINT_2F const& value)
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
            static inline std::wstring ToString<D2D1_RECT_F>(D2D1_RECT_F const& value)
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
            static inline std::wstring ToString<D2D1_ROUNDED_RECT>(D2D1_ROUNDED_RECT const& roundedRect)
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
            static inline std::wstring ToString<D2D1_ELLIPSE>(D2D1_ELLIPSE const& ellipse)
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
            static inline std::wstring ToString<IID>(IID const& iid)
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
            TO_STRING(ID2D1Image);
            TO_STRING(ID2D1Device);
            TO_STRING(ID2D1Device1);
            TO_STRING(ID2D1DeviceContext1);
            TO_STRING(ID2D1Factory);
            TO_STRING(IDWriteTextFormat);
            TO_STRING(IDXGIDevice3);
            TO_STRING(IDirect3DDevice);
            TO_STRING(IInspectable);
            TO_STRING(IUnknown);
            TO_STRING(ID2D1StrokeStyle1);
            TO_STRING(ID2D1Bitmap);
            TO_STRING(ID2D1Bitmap1);

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
            static inline std::wstring ToString<ABI::Windows::UI::Color>(ABI::Windows::UI::Color const& value)
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
            static inline std::wstring ToString<D2D1_COLOR_F>(D2D1_COLOR_F const& value)
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
            static inline std::wstring ToString<Numerics::Matrix3x2>(Numerics::Matrix3x2 const& value)
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
            static inline std::wstring ToString<D2D1_MATRIX_3X2_F>(D2D1_MATRIX_3X2_F const& value)
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
            static inline std::wstring ToString<D2D1_MATRIX_4X4_F>(D2D1_MATRIX_4X4_F const& value)
            {
                wchar_t buf[256];
                ThrowIfFailed(StringCchPrintf(
                    buf,
                    _countof(buf),
                    L"D2D1_MATRIX_4X4_F{_11=%f,_12=%f,_13=%f,_14=%f,_21=%f,_22=%f,_23=%f,_24=%f,_31=%f,_32=%f,_33=%f,_34=%f,_41=%f,_42=%f,_43=%f,_44=%f}",
                    value._11, value._12, value._13, value._14,
                    value._21, value._22, value._23, value._24,
                    value._31, value._32, value._33, value._34,
                    value._41, value._42, value._43, value._44));

                return buf;
            }

            template<>
            static inline std::wstring ToString<ABI::Windows::Foundation::Rect>(ABI::Windows::Foundation::Rect const& value)
            {
                wchar_t buf[256];
                ThrowIfFailed(StringCchPrintf(
                    buf,
                    _countof(buf),
                    L"Rect{X=%f,Y=%f,W=%f,H=%f}",
                    value.X, value.Y,
                    value.Width, value.Height));

                return buf;
            }

            template<>
            static inline std::wstring ToString<D2D1_POINT_2U>(D2D1_POINT_2U const& value)
            {
                wchar_t buf[256];
                ThrowIfFailed(StringCchPrintf(
                    buf,
                    _countof(buf),
                    L"D2D1_POINT_2U{X=%f,Y=%f}",
                    value.x, value.y));

                return buf;
            }

            template<>
            static inline std::wstring ToString<D2D1_RECT_U>(D2D1_RECT_U const& value)
            {
                wchar_t buf[256];
                ThrowIfFailed(StringCchPrintf(
                    buf,
                    _countof(buf),
                    L"D2D1_RECT_U{l=%f,t=%f,r=%f,b=%f}",
                    value.left, value.top, value.right, value.bottom));

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
            static inline std::wstring ToString<__int64>(__int64 const& value)
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

            ENUM_TO_STRING(D2D1_ANTIALIAS_MODE)
            {
                ENUM_VALUE(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
                ENUM_VALUE(D2D1_ANTIALIAS_MODE_ALIASED);
                END_ENUM(D2D1_ANTIALIAS_MODE);
            }

            ENUM_TO_STRING(CanvasAntialiasing)
            {
                ENUM_VALUE(CanvasAntialiasing::Antialiased);
                ENUM_VALUE(CanvasAntialiasing::Aliased);
                END_ENUM(CanvasAntialiasing);
            }

            ENUM_TO_STRING(D2D1_PRIMITIVE_BLEND)
            {
                ENUM_VALUE(D2D1_PRIMITIVE_BLEND_SOURCE_OVER);
                ENUM_VALUE(D2D1_PRIMITIVE_BLEND_COPY);
                ENUM_VALUE(D2D1_PRIMITIVE_BLEND_MIN);
                ENUM_VALUE(D2D1_PRIMITIVE_BLEND_ADD);
                END_ENUM(D2D1_PRIMITIVE_BLEND);
            }

            ENUM_TO_STRING(CanvasBlend)
            {
                ENUM_VALUE(CanvasBlend::SourceOver);
                ENUM_VALUE(CanvasBlend::Copy);
                ENUM_VALUE(CanvasBlend::Min);
                ENUM_VALUE(CanvasBlend::Add);
                END_ENUM(CanvasBlend);
            }

            ENUM_TO_STRING(D2D1_TEXT_ANTIALIAS_MODE)
            {
                ENUM_VALUE(D2D1_TEXT_ANTIALIAS_MODE_DEFAULT);
                ENUM_VALUE(D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE);
                ENUM_VALUE(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);
                ENUM_VALUE(D2D1_TEXT_ANTIALIAS_MODE_ALIASED);
                END_ENUM(D2D1_TEXT_ANTIALIAS_MODE);
            }

            ENUM_TO_STRING(CanvasTextAntialiasing)
            {
                ENUM_VALUE(CanvasTextAntialiasing::Default);
                ENUM_VALUE(CanvasTextAntialiasing::ClearType);
                ENUM_VALUE(CanvasTextAntialiasing::Grayscale);
                ENUM_VALUE(CanvasTextAntialiasing::Aliased);
                END_ENUM(CanvasTextAntialiasing);
            }

            ENUM_TO_STRING(D2D1_UNIT_MODE)
            {
                ENUM_VALUE(D2D1_UNIT_MODE_DIPS);
                ENUM_VALUE(D2D1_UNIT_MODE_PIXELS);
                END_ENUM(D2D1_UNIT_MODE);
            }

            ENUM_TO_STRING(CanvasUnits)
            {
                ENUM_VALUE(CanvasUnits::Dips);
                ENUM_VALUE(CanvasUnits::Pixels);
                END_ENUM(CanvasUnits);
            }

            ENUM_TO_STRING(AsyncStatus)
            {
                ENUM_VALUE(AsyncStatus::Started);
                ENUM_VALUE(AsyncStatus::Completed);
                ENUM_VALUE(AsyncStatus::Canceled);
                ENUM_VALUE(AsyncStatus::Error);
                END_ENUM(AsyncStatus);
            }
            
            ENUM_TO_STRING(CanvasEdgeBehavior)
            {
                ENUM_VALUE(CanvasEdgeBehavior::Clamp);
                ENUM_VALUE(CanvasEdgeBehavior::Mirror);
                ENUM_VALUE(CanvasEdgeBehavior::Wrap);
                END_ENUM(CanvasEdgeBehavior);
            }            
            
            ENUM_TO_STRING(D2D1_EXTEND_MODE)
            {
                ENUM_VALUE(D2D1_EXTEND_MODE_CLAMP);
                ENUM_VALUE(D2D1_EXTEND_MODE_MIRROR);
                ENUM_VALUE(D2D1_EXTEND_MODE_WRAP);
                END_ENUM(D2D1_EXTEND_MODE);
            }

            ENUM_TO_STRING(CanvasImageInterpolation)
            {
                ENUM_VALUE(CanvasImageInterpolation::NearestNeighbor);
                ENUM_VALUE(CanvasImageInterpolation::Linear);
                ENUM_VALUE(CanvasImageInterpolation::Cubic);
                ENUM_VALUE(CanvasImageInterpolation::MultiSampleLinear);
                ENUM_VALUE(CanvasImageInterpolation::Anisotropic);
                ENUM_VALUE(CanvasImageInterpolation::HighQualityCubic);
                END_ENUM(CanvasImageInterpolation);
            }

            ENUM_TO_STRING(D2D1_INTERPOLATION_MODE)
            {
                ENUM_VALUE(D2D1_INTERPOLATION_MODE_NEAREST_NEIGHBOR);
                ENUM_VALUE(D2D1_INTERPOLATION_MODE_LINEAR);
                ENUM_VALUE(D2D1_INTERPOLATION_MODE_CUBIC);
                ENUM_VALUE(D2D1_INTERPOLATION_MODE_MULTI_SAMPLE_LINEAR);
                ENUM_VALUE(D2D1_INTERPOLATION_MODE_ANISOTROPIC);
                ENUM_VALUE(D2D1_INTERPOLATION_MODE_HIGH_QUALITY_CUBIC);
                END_ENUM(D2D1_INTERPOLATION_MODE);
            }

            ENUM_TO_STRING(CanvasAlphaBehavior)
            {
                ENUM_VALUE(CanvasAlphaBehavior::Ignore);
                ENUM_VALUE(CanvasAlphaBehavior::Premultiplied);
                ENUM_VALUE(CanvasAlphaBehavior::Straight);
                END_ENUM(CanvasAlphaBehavior);
            }

            ENUM_TO_STRING(CanvasColorSpace)
            {
                ENUM_VALUE(CanvasColorSpace::Custom);
                ENUM_VALUE(CanvasColorSpace::Srgb);
                ENUM_VALUE(CanvasColorSpace::ScRgb);
                END_ENUM(CanvasColorSpace);
            }

            ENUM_TO_STRING(CanvasBufferPrecision)
            {
                ENUM_VALUE(CanvasBufferPrecision::Precision8UIntNormalized);
                ENUM_VALUE(CanvasBufferPrecision::Precision8UIntNormalizedSrgb);
                ENUM_VALUE(CanvasBufferPrecision::Precision16UIntNormalized);
                ENUM_VALUE(CanvasBufferPrecision::Precision16Float);
                ENUM_VALUE(CanvasBufferPrecision::Precision32Float);
                END_ENUM(CanvasBufferPrecision);
            }

            ENUM_TO_STRING(D2D1_COMPOSITE_MODE)
            {
                ENUM_VALUE(D2D1_COMPOSITE_MODE_SOURCE_OVER);
                ENUM_VALUE(D2D1_COMPOSITE_MODE_DESTINATION_OVER);
                ENUM_VALUE(D2D1_COMPOSITE_MODE_SOURCE_IN);
                ENUM_VALUE(D2D1_COMPOSITE_MODE_DESTINATION_IN);
                ENUM_VALUE(D2D1_COMPOSITE_MODE_SOURCE_OUT);
                ENUM_VALUE(D2D1_COMPOSITE_MODE_DESTINATION_OUT);
                ENUM_VALUE(D2D1_COMPOSITE_MODE_SOURCE_ATOP);
                ENUM_VALUE(D2D1_COMPOSITE_MODE_DESTINATION_ATOP);
                ENUM_VALUE(D2D1_COMPOSITE_MODE_XOR);
                ENUM_VALUE(D2D1_COMPOSITE_MODE_PLUS);
                ENUM_VALUE(D2D1_COMPOSITE_MODE_SOURCE_COPY);
                ENUM_VALUE(D2D1_COMPOSITE_MODE_BOUNDED_SOURCE_COPY);
                ENUM_VALUE(D2D1_COMPOSITE_MODE_MASK_INVERT);
                END_ENUM(D2D1_COMPOSITE_MODE);
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
            static inline std::wstring ToString<ABI::Windows::UI::Text::FontWeight>(ABI::Windows::UI::Text::FontWeight const& value)
            {
                return ToStringAsInt(value.Weight);
            }

#define TO_STRING_AS_INT(TYPE)                                          \
            template<>                                                  \
            static inline std::wstring ToString<TYPE>(TYPE const& value) \
            {                                                           \
                return ToStringAsInt(value);                            \
            }

            TO_STRING_AS_INT(DWRITE_FONT_WEIGHT);
            TO_STRING_AS_INT(CanvasDrawTextOptions);
            TO_STRING_AS_INT(D2D1_DRAW_TEXT_OPTIONS);
        }

        inline bool operator==(D2D1_POINT_2F const& a, D2D1_POINT_2F const& b)
        {
            return a.x == b.x &&
                a.y == b.y;
        }

        inline bool operator==(D2D1_RECT_F const& a, D2D1_RECT_F const& b)
        {
            return a.left == b.left &&
                a.top == b.top &&
                a.right == b.right &&
                a.bottom == b.bottom;
        }

        inline bool operator==(D2D1_ROUNDED_RECT const& a, D2D1_ROUNDED_RECT const& b)
        {
            return a.rect == b.rect &&
                a.radiusX == b.radiusX &&
                a.radiusY == b.radiusY;
        }

        inline bool operator==(D2D1_ELLIPSE const& a, D2D1_ELLIPSE const& b)
        {
            return a.point == b.point &&
                a.radiusX == b.radiusX &&
                a.radiusY == b.radiusY;
        }

        inline bool operator==(ABI::Windows::UI::Color const& a, ABI::Windows::UI::Color const& b)
        {
            return a.A == b.A &&
                a.R == b.R &&
                a.G == b.G &&
                a.B == b.B;
        }

        inline bool operator==(D2D1_COLOR_F const& a, D2D1_COLOR_F const& b)
        {
            return a.a == b.a &&
                a.r == b.r &&
                a.g == b.g &&
                a.b == b.b;
        }

        inline bool operator==(Numerics::Matrix3x2 const& a, Numerics::Matrix3x2 const& b)
        {
            return
                a.M11 == b.M11 && a.M12 == b.M12 &&
                a.M21 == b.M21 && a.M22 == b.M22 &&
                a.M31 == b.M31 && a.M32 == b.M32;
        }

        inline bool operator==(ABI::Windows::UI::Text::FontWeight const& a, ABI::Windows::UI::Text::FontWeight const& b)
        {
            return a.Weight == b.Weight;
        }

        inline bool operator==(ABI::Windows::Foundation::Rect const& a, ABI::Windows::Foundation::Rect const& b)
        {
            return a.X == b.X &&
            a.Y == b.Y &&
            a.Width == b.Width &&
            a.Height == b.Height;
        }

    }
}

inline bool operator==(D2D1_MATRIX_3X2_F const& a, D2D1_MATRIX_3X2_F const& b)
{
    return
        a._11 == b._11 && a._12 == b._12 &&
        a._21 == b._21 && a._22 == b._22 &&
        a._31 == b._31 && a._32 == b._32;
}

inline bool operator==(D2D1_MATRIX_4X4_F const& a, D2D1_MATRIX_4X4_F const& b)
{
    return
        a._11 == b._11 && a._12 == b._12 && a._13 == b._13 && a._14 == b._14 &&
        a._21 == b._21 && a._22 == b._22 && a._23 == b._23 && a._24 == b._24 &&
        a._31 == b._31 && a._32 == b._32 && a._33 == b._33 && a._34 == b._34 &&
        a._41 == b._41 && a._42 == b._42 && a._43 == b._43 && a._44 == b._44;
}

inline bool operator==(D2D1_POINT_2U const& a, D2D1_POINT_2U const& b)
{
    return a.x == b.x && a.y == b.y;
}

#define ASSERT_IMPLEMENTS_INTERFACE(obj, INTERFACE)                     \
    {                                                                   \
        ComPtr<INTERFACE> iface;                                        \
        Assert::AreEqual(S_OK, obj.As(&iface), L"Implements " L#INTERFACE); \
    }


template<typename T>
inline void ExpectHResultException(HRESULT expectedHR, T&& lambda)
{
    try
    {
        lambda();
        Assert::Fail(L"Expected this to throw.");
    }
    catch (HResultException const& e)
    {
        Assert::AreEqual(expectedHR, e.GetHr());
    }
}


inline void ValidateStoredErrorState(HRESULT expectedHR, wchar_t const* expectedMessage)
{
    ComPtr<IRestrictedErrorInfo> errorInfo;
    ThrowIfFailed(GetRestrictedErrorInfo(&errorInfo));

    BSTR description = nullptr;
    BSTR restrictedDescription = nullptr;
    BSTR capabilitySid = nullptr;

    auto cleanup = MakeScopeWarden([&]
    {
        if (description)           SysFreeString(description);
        if (restrictedDescription) SysFreeString(restrictedDescription);
        if (capabilitySid)         SysFreeString(capabilitySid);
    });

    HRESULT actualHR;
    
    ThrowIfFailed(errorInfo->GetErrorDetails(&description, &actualHR, &restrictedDescription, &capabilitySid));

    Assert::AreEqual(expectedHR, actualHR);
    Assert::AreEqual(expectedMessage, restrictedDescription);
}


template<typename T>
void AssertClassName(ComPtr<T> obj, wchar_t const* expectedClassName)
{
    ComPtr<IInspectable> inspectable;
    ThrowIfFailed(obj.As(&inspectable));
    
    WinString className;
    ThrowIfFailed(inspectable->GetRuntimeClassName(className.GetAddressOf()));
    
    Assert::AreEqual(expectedClassName, static_cast<wchar_t const*>(className));
}
