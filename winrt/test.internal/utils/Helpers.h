// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace Microsoft
{
    namespace VisualStudio
    {
        namespace CppUnitTestFramework
        {

#define ENUM_TO_STRING(TYPE)                                                        \
            template<> inline std::wstring ToString<TYPE>(TYPE const& value) \
            {                                                                       \
                switch (value)

#define ENUM_VALUE(NAME)                                                            \
                case NAME:                                                          \
                    return WIDEN(#NAME)

#define END_ENUM(TYPE)                                                              \
                default:                                                            \
                    return L"<<invalid " WIDEN(#TYPE) L">>";                        \
                }

            template<>
            inline std::wstring ToString<RECT>(RECT const& value)
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
            inline std::wstring ToString<D2D1_POINT_2F>(D2D1_POINT_2F const& value)
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
            inline std::wstring ToString<D2D1_RECT_F>(D2D1_RECT_F const& value)
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
            inline std::wstring ToString<D2D_SIZE_U>(D2D_SIZE_U const& value)
            {
                wchar_t buf[256];
                ThrowIfFailed(StringCchPrintf(
                    buf,
                    _countof(buf),
                    L"D2D_SIZE_U{%u,%u}",
                    value.width,
                    value.height));
                return buf;
            }

            template<>
            inline std::wstring ToString<D2D1_ROUNDED_RECT>(D2D1_ROUNDED_RECT const& roundedRect)
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
            inline std::wstring ToString<D2D1_ELLIPSE>(D2D1_ELLIPSE const& ellipse)
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
            inline std::wstring ToString<D2D1_SIZE_F>(D2D1_SIZE_F const& value)
            {
                wchar_t buf[256];
                ThrowIfFailed(StringCchPrintf(
                    buf,
                    _countof(buf),
                    L"D2D1_SIZE_F{%f,%f}",
                    value.width,
                    value.height));
                return buf;
            }

            template<>
            inline std::wstring ToString<ABI::Windows::Foundation::Size>(ABI::Windows::Foundation::Size const& s)
            {
                wchar_t buf[256];
                ThrowIfFailed(StringCchPrintf(
                    buf,
                    _countof(buf),
                    L"Size{%f,%f}",
                    s.Width,
                    s.Height));
                return buf;
            }

            template<>
            inline std::wstring ToString<IID>(IID const& iid)
            {
                wchar_t* iidString = nullptr;
                ThrowIfFailed(StringFromIID(iid, &iidString));
                std::wstring value(iidString);
                CoTaskMemFree(iidString);
                return value;
            }            

            template<>
            inline std::wstring ToString<CanvasTimingInformation>(CanvasTimingInformation const& value)
            {
                wchar_t buf[256];
                ThrowIfFailed(StringCchPrintf(
                    buf,
                    _countof(buf),
                    L"CanvasTimingInformation{UpdateCount=%d,TotalTime=%I64d,ElapsedTime=%I64d,IsRunningSlowly=%d}",
                    value.UpdateCount, value.TotalTime.Duration, value.ElapsedTime.Duration, value.IsRunningSlowly));

                return buf;
            }

            inline std::wstring PointerToString(const wchar_t* name, void* value)
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
            inline std::wstring ToString<T>(T* value)    \
            {                                                   \
                return PointerToString(L#T, value);             \
            }

            TO_STRING(ICanvasControl);
            TO_STRING(ICanvasDevice);
            TO_STRING(ICanvasDrawingSession);
            TO_STRING(ID2D1Brush);
            TO_STRING(ID2D1Image);
            TO_STRING(ID2D1Device);
            TO_STRING(ID2D1Device1);
            TO_STRING(ID2D1DeviceContext1);
            TO_STRING(ID2D1Factory);
            TO_STRING(IDWriteTextFormat);
            TO_STRING(IDWriteTextFormat1);
            TO_STRING(IDXGIDevice3);
            TO_STRING(IDirect3DDevice);
            TO_STRING(IInspectable);
            TO_STRING(IUnknown);
            TO_STRING(ID2D1StrokeStyle1);
            TO_STRING(ID2D1Bitmap);
            TO_STRING(ID2D1Bitmap1);
            TO_STRING(IDXGISurface);
            TO_STRING(IDXGISwapChain);
            TO_STRING(ICanvasSwapChain);
            TO_STRING(ABI::Windows::UI::Core::ICoreCursor);
            TO_STRING(ID2D1Geometry);
            TO_STRING(ID2D1PathGeometry);
            TO_STRING(ID2D1SimplifiedGeometrySink);
            TO_STRING(ID2D1GeometrySink);
            TO_STRING(ID2D1GeometryRealization);
            TO_STRING(IDWriteTextLayout);
            TO_STRING(IDispatchedHandler);
            TO_STRING(IAsyncAction);
            TO_STRING(IDWriteInlineObject);
            TO_STRING(ID2D1TransformNode);

#if WINVER > _WIN32_WINNT_WINBLUE
            TO_STRING(ID2D1GradientMesh);
#endif

#undef TO_STRING

            ENUM_TO_STRING(CanvasDebugLevel)
            {
                ENUM_VALUE(CanvasDebugLevel::None);
                ENUM_VALUE(CanvasDebugLevel::Error);
                ENUM_VALUE(CanvasDebugLevel::Warning);
                ENUM_VALUE(CanvasDebugLevel::Information);
                END_ENUM(CanvasDebugLevel);
            }

            ENUM_TO_STRING(RunWithDeviceFlags)
            {
                ENUM_VALUE(RunWithDeviceFlags::None);
                ENUM_VALUE(RunWithDeviceFlags::NewlyCreatedDevice);
                ENUM_VALUE(RunWithDeviceFlags::ResourcesNotCreated);
                END_ENUM(RunWithDeviceFlags);
            }

            ENUM_TO_STRING(CanvasCreateResourcesReason)
            {
                ENUM_VALUE(CanvasCreateResourcesReason::FirstTime);
                ENUM_VALUE(CanvasCreateResourcesReason::NewDevice);
                ENUM_VALUE(CanvasCreateResourcesReason::DpiChanged);
                END_ENUM(CanvasCreateResourcesReason);
            }

            ENUM_TO_STRING(CanvasSwapChainRotation)
            {
                ENUM_VALUE(CanvasSwapChainRotation::None);
                ENUM_VALUE(CanvasSwapChainRotation::Rotate90);
                ENUM_VALUE(CanvasSwapChainRotation::Rotate180);
                ENUM_VALUE(CanvasSwapChainRotation::Rotate270);
                END_ENUM(CanvasSwapChainRotation);
            }

            ENUM_TO_STRING(ChangeReason)
            {
                ENUM_VALUE(ChangeReason::Other);
                ENUM_VALUE(ChangeReason::ClearColor);
                ENUM_VALUE(ChangeReason::DeviceLost);
                END_ENUM(ChangeReason);
            }

            ENUM_TO_STRING(CoreProcessEventsOption)
            {
                ENUM_VALUE(CoreProcessEventsOption_ProcessOneAndAllPending);
                ENUM_VALUE(CoreProcessEventsOption_ProcessOneIfPresent);
                ENUM_VALUE(CoreProcessEventsOption_ProcessUntilQuit);
                ENUM_VALUE(CoreProcessEventsOption_ProcessAllIfPresent);
                END_ENUM(CoreProcessEventsOption);
            }

            template<>
            inline std::wstring ToString<ABI::Windows::UI::Color>(ABI::Windows::UI::Color const& value)
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
            inline std::wstring ToString<D2D1_COLOR_F>(D2D1_COLOR_F const& value)
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
            inline std::wstring ToString<Numerics::Matrix3x2>(Numerics::Matrix3x2 const& value)
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
            inline std::wstring ToString<D2D1_MATRIX_3X2_F>(D2D1_MATRIX_3X2_F const& value)
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
            inline std::wstring ToString<D2D1_MATRIX_4X4_F>(D2D1_MATRIX_4X4_F const& value)
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
            inline std::wstring ToString<D2D1_MATRIX_5X4_F>(D2D1_MATRIX_5X4_F const& value)
            {
                wchar_t buf[512];
                ThrowIfFailed(StringCchPrintf(
                    buf,
                    _countof(buf),
                    L"D2D1_MATRIX_5X4_F{_11=%f,_12=%f,_13=%f,_14=%f,_21=%f,_22=%f,_23=%f,_24=%f,_31=%f,_32=%f,_33=%f,_34=%f,_41=%f,_42=%f,_43=%f,_44=%f,_51=%f,_52=%f,_53=%f,_54=%f}",
                    value._11, value._12, value._13, value._14,
                    value._21, value._22, value._23, value._24,
                    value._31, value._32, value._33, value._34,
                    value._41, value._42, value._43, value._44,
                    value._51, value._52, value._53, value._54));

                return buf;
            }

            template<>
            inline std::wstring ToString<DWRITE_MATRIX>(DWRITE_MATRIX const& value)
            {
                wchar_t buf[256];
                ThrowIfFailed(StringCchPrintf(
                    buf,
                    _countof(buf),
                    L"DWRITE_MATRIX{m11=%f,m12=%f,m21=%f,m22=%f,dx=%f,dy=%f}",
                    value.m11, value.m12,
                    value.m21, value.m22,
                    value.dx, value.dy));

                return buf;
            }

            template<>
            inline std::wstring ToString<ABI::Windows::Foundation::Rect>(ABI::Windows::Foundation::Rect const& value)
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
            inline std::wstring ToString<D2D1_POINT_2U>(D2D1_POINT_2U const& value)
            {
                wchar_t buf[256];
                ThrowIfFailed(StringCchPrintf(
                    buf,
                    _countof(buf),
                    L"D2D1_POINT_2U{X=%u,Y=%u}",
                    value.x, value.y));

                return buf;
            }

            template<>
            inline std::wstring ToString<D2D1_RECT_U>(D2D1_RECT_U const& value)
            {
                wchar_t buf[256];
                ThrowIfFailed(StringCchPrintf(
                    buf,
                    _countof(buf),
                    L"D2D1_RECT_U{l=%u,t=%u,r=%u,b=%u}",
                    value.left, value.top, value.right, value.bottom));

                return buf;
            }

            template<>
            inline std::wstring ToString<ABI::Windows::Foundation::Point>(ABI::Windows::Foundation::Point const& value)
            {
                wchar_t buf[256];
                ThrowIfFailed(StringCchPrintf(
                    buf,
                    _countof(buf),
                    L"Point{X=%f,Y=%f}",
                    value.X, value.Y));

                return buf;
            }

            template<>
            inline std::wstring ToString<Numerics::Vector2>(Numerics::Vector2 const& value)
            {
                wchar_t buf[256];
                ThrowIfFailed(StringCchPrintf(
                    buf,
                    _countof(buf),
                    L"Vector2{X=%f,Y=%f}",
                    value.X, value.Y));

                return buf;
            }

            template<>
            inline std::wstring ToString<Numerics::Vector4>(Numerics::Vector4 const& value)
            {
                wchar_t buf[256];
                ThrowIfFailed(StringCchPrintf(
                    buf,
                    _countof(buf),
                    L"Vector4{X=%f,Y=%f,Z=%f,W=%f}",
                    value.X, value.Y, value.Z, value.W));

                return buf;
            }

            template<>
            inline std::wstring ToString<D2D1_TRIANGLE>(D2D1_TRIANGLE const& value)
            {
                wchar_t buf[256];
                ThrowIfFailed(StringCchPrintf(
                    buf,
                    _countof(buf),
                    L"D2D1_TRIANGLE{{%s},{%s},{%s}}",
                    ToString(value.point1).c_str(),
                    ToString(value.point2).c_str(),
                    ToString(value.point3).c_str()));

                return buf;
            }

            template<>
            inline std::wstring ToString<std::vector<BYTE>>(std::vector<BYTE> const& value)
            {
                return L"std::vector<BYTE>";
            }

            template<>
            inline std::wstring ToString<unsigned short>(unsigned short const& value)
            {
                wchar_t buf[256];
                ThrowIfFailed(StringCchPrintf(
                    buf,
                    _countof(buf),
                    L"{%hu}", 
                    value));
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
                ENUM_VALUE(CanvasTextDirection::LeftToRightThenTopToBottom);
                ENUM_VALUE(CanvasTextDirection::RightToLeftThenTopToBottom);
                ENUM_VALUE(CanvasTextDirection::LeftToRightThenBottomToTop);
                ENUM_VALUE(CanvasTextDirection::RightToLeftThenBottomToTop);
                ENUM_VALUE(CanvasTextDirection::TopToBottomThenLeftToRight);
                ENUM_VALUE(CanvasTextDirection::BottomToTopThenLeftToRight);
                ENUM_VALUE(CanvasTextDirection::TopToBottomThenRightToLeft);
                ENUM_VALUE(CanvasTextDirection::BottomToTopThenRightToLeft);
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
                END_ENUM(D2D1_CAP_STYLE);
            }

            ENUM_TO_STRING(D2D1_LINE_JOIN)
            {
                ENUM_VALUE(D2D1_LINE_JOIN_MITER);
                ENUM_VALUE(D2D1_LINE_JOIN_BEVEL);
                ENUM_VALUE(D2D1_LINE_JOIN_ROUND);
                ENUM_VALUE(D2D1_LINE_JOIN_MITER_OR_BEVEL);
                END_ENUM(D2D1_LINE_JOIN);
            }

            ENUM_TO_STRING(D2D1_DASH_STYLE)
            {
                ENUM_VALUE(D2D1_DASH_STYLE_SOLID);
                ENUM_VALUE(D2D1_DASH_STYLE_DASH);
                ENUM_VALUE(D2D1_DASH_STYLE_DOT);
                ENUM_VALUE(D2D1_DASH_STYLE_DASH_DOT);
                ENUM_VALUE(D2D1_DASH_STYLE_DASH_DOT_DOT);
                ENUM_VALUE(D2D1_DASH_STYLE_CUSTOM);
                END_ENUM(D2D1_DASH_STYLE);
            }

            ENUM_TO_STRING(D2D1_STROKE_TRANSFORM_TYPE)
            {
                ENUM_VALUE(D2D1_STROKE_TRANSFORM_TYPE_NORMAL);
                ENUM_VALUE(D2D1_STROKE_TRANSFORM_TYPE_FIXED);
                ENUM_VALUE(D2D1_STROKE_TRANSFORM_TYPE_HAIRLINE);
                END_ENUM(D2D1_STROKE_TRANSFORM_TYPE);
            }

            ENUM_TO_STRING(D2D1_FIGURE_BEGIN)
            {
                ENUM_VALUE(D2D1_FIGURE_BEGIN_FILLED);
                ENUM_VALUE(D2D1_FIGURE_BEGIN_HOLLOW);
                END_ENUM(D2D1_FIGURE_BEGIN);
            }

            ENUM_TO_STRING(D2D1_FIGURE_END)
            {
                ENUM_VALUE(D2D1_FIGURE_END_CLOSED);
                ENUM_VALUE(D2D1_FIGURE_END_OPEN);
                END_ENUM(D2D1_FIGURE_END);
            }

            ENUM_TO_STRING(D2D1_SWEEP_DIRECTION)
            {
                ENUM_VALUE(D2D1_SWEEP_DIRECTION_COUNTER_CLOCKWISE);
                ENUM_VALUE(D2D1_SWEEP_DIRECTION_CLOCKWISE);
                END_ENUM(D2D1_SWEEP_DIRECTION);
            }

            ENUM_TO_STRING(D2D1_ARC_SIZE)
            {
                ENUM_VALUE(D2D1_ARC_SIZE_SMALL);
                ENUM_VALUE(D2D1_ARC_SIZE_LARGE);
                END_ENUM(D2D1_ARC_SIZE);
            }

            ENUM_TO_STRING(D2D1_PATH_SEGMENT)
            {
                ENUM_VALUE(D2D1_PATH_SEGMENT_NONE);
                ENUM_VALUE(D2D1_PATH_SEGMENT_FORCE_ROUND_LINE_JOIN);
                ENUM_VALUE(D2D1_PATH_SEGMENT_FORCE_UNSTROKED);
                END_ENUM(D2D1_PATH_SEGMENT);
            }

            ENUM_TO_STRING(D2D1_FILL_MODE)
            {
                ENUM_VALUE(D2D1_FILL_MODE_ALTERNATE);
                ENUM_VALUE(D2D1_FILL_MODE_WINDING);
                END_ENUM(D2D1_FILL_MODE);
            }

            ENUM_TO_STRING(D2D1_BUFFER_PRECISION)
            {
                ENUM_VALUE(D2D1_BUFFER_PRECISION_UNKNOWN);
                ENUM_VALUE(D2D1_BUFFER_PRECISION_8BPC_UNORM);
                ENUM_VALUE(D2D1_BUFFER_PRECISION_8BPC_UNORM_SRGB);
                ENUM_VALUE(D2D1_BUFFER_PRECISION_16BPC_UNORM);
                ENUM_VALUE(D2D1_BUFFER_PRECISION_16BPC_FLOAT);
                ENUM_VALUE(D2D1_BUFFER_PRECISION_32BPC_FLOAT);
                END_ENUM(D2D1_BUFFER_PRECISION);
            }

            ENUM_TO_STRING(SamplerCoordinateMapping)
            {
                ENUM_VALUE(SamplerCoordinateMapping_Unknown);
                ENUM_VALUE(SamplerCoordinateMapping_OneToOne);
                ENUM_VALUE(SamplerCoordinateMapping_Offset);
                END_ENUM(SamplerCoordinateMapping);
            }

            template<>
            inline std::wstring ToString<__int64>(__int64 const& value)
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

            ENUM_TO_STRING(DWRITE_LINE_SPACING_METHOD)
            {
                ENUM_VALUE(DWRITE_LINE_SPACING_METHOD_DEFAULT);
                ENUM_VALUE(DWRITE_LINE_SPACING_METHOD_UNIFORM);
#if WINVER > _WIN32_WINNT_WINBLUE
                ENUM_VALUE(DWRITE_LINE_SPACING_METHOD_PROPORTIONAL);
#endif
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

            ENUM_TO_STRING(CanvasHorizontalAlignment)
            {
                ENUM_VALUE(CanvasHorizontalAlignment::Left);
                ENUM_VALUE(CanvasHorizontalAlignment::Right);
                ENUM_VALUE(CanvasHorizontalAlignment::Center);
                ENUM_VALUE(CanvasHorizontalAlignment::Justified);
                END_ENUM(CanvasHorizontalAlignment);
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
                ENUM_VALUE(CanvasTextAntialiasing::Auto);
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

            ENUM_TO_STRING(D2D1_BITMAP_INTERPOLATION_MODE)
            {
                ENUM_VALUE(D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR);
                ENUM_VALUE(D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
                END_ENUM(D2D1_BITMAP_INTERPOLATION_MODE);
            }
            
#if WINVER > _WIN32_WINNT_WINBLUE

            ENUM_TO_STRING(D2D1_SPRITE_OPTIONS)
            {
                ENUM_VALUE(D2D1_SPRITE_OPTIONS_NONE);
                ENUM_VALUE(D2D1_SPRITE_OPTIONS_CLAMP_TO_SOURCE_RECTANGLE);
                END_ENUM(D2D1_SPRITE_OPTIONS);
            }

#endif

            ENUM_TO_STRING(CanvasAlphaMode)
            {
                ENUM_VALUE(CanvasAlphaMode::Ignore);
                ENUM_VALUE(CanvasAlphaMode::Premultiplied);
                ENUM_VALUE(CanvasAlphaMode::Straight);
                END_ENUM(CanvasAlphaMode);
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

            ENUM_TO_STRING(ABI::Windows::UI::Core::CoreDispatcherPriority)
            {
                ENUM_VALUE(ABI::Windows::UI::Core::CoreDispatcherPriority_Idle);
                ENUM_VALUE(ABI::Windows::UI::Core::CoreDispatcherPriority_Low);
                ENUM_VALUE(ABI::Windows::UI::Core::CoreDispatcherPriority_Normal);
                ENUM_VALUE(ABI::Windows::UI::Core::CoreDispatcherPriority_High);
                END_ENUM(ABI::Windows::UI::Core::CoreDispatcherPriority);
            }

            ENUM_TO_STRING(D2D1_COMBINE_MODE)
            {
                ENUM_VALUE(D2D1_COMBINE_MODE_UNION);
                ENUM_VALUE(D2D1_COMBINE_MODE_INTERSECT);
                ENUM_VALUE(D2D1_COMBINE_MODE_XOR);
                ENUM_VALUE(D2D1_COMBINE_MODE_EXCLUDE);
                END_ENUM(D2D1_COMBINE_MODE_EXCLUDE);
            }

            ENUM_TO_STRING(D2D1_GEOMETRY_SIMPLIFICATION_OPTION)
            {
                ENUM_VALUE(D2D1_GEOMETRY_SIMPLIFICATION_OPTION_CUBICS_AND_LINES);
                ENUM_VALUE(D2D1_GEOMETRY_SIMPLIFICATION_OPTION_LINES);
                END_ENUM(D2D1_GEOMETRY_SIMPLIFICATION_OPTION);
            }

            ENUM_TO_STRING(CanvasGeometryRelation)
            {
                ENUM_VALUE(CanvasGeometryRelation::Disjoint);
                ENUM_VALUE(CanvasGeometryRelation::Contained);
                ENUM_VALUE(CanvasGeometryRelation::Contains);
                ENUM_VALUE(CanvasGeometryRelation::Overlap);
                END_ENUM(CanvasGeometryRelation);
            }

            ENUM_TO_STRING(CanvasLayerOptions)
            {
                ENUM_VALUE(CanvasLayerOptions::None);
                ENUM_VALUE(CanvasLayerOptions::InitializeFromBackground);
                ENUM_VALUE(CanvasLayerOptions::IgnoreAlpha);
                END_ENUM(CanvasLayerOptions);
            }

            ENUM_TO_STRING(CanvasVerticalGlyphOrientation)
            {
                ENUM_VALUE(CanvasVerticalGlyphOrientation::Default);
                ENUM_VALUE(CanvasVerticalGlyphOrientation::Stacked);
                END_ENUM(CanvasVerticalGlyphOrientation);
            }

            ENUM_TO_STRING(DWRITE_VERTICAL_GLYPH_ORIENTATION)
            {
                ENUM_VALUE(DWRITE_VERTICAL_GLYPH_ORIENTATION_DEFAULT);
                ENUM_VALUE(DWRITE_VERTICAL_GLYPH_ORIENTATION_STACKED);
                END_ENUM(DWRITE_VERTICAL_GLYPH_ORIENTATION);
            }

            ENUM_TO_STRING(CanvasOpticalAlignment)
            {
                ENUM_VALUE(CanvasOpticalAlignment::Default);
                ENUM_VALUE(CanvasOpticalAlignment::NoSideBearings);
                END_ENUM(CanvasOpticalAlignment);
            }

            ENUM_TO_STRING(DWRITE_OPTICAL_ALIGNMENT)
            {
                ENUM_VALUE(DWRITE_OPTICAL_ALIGNMENT_NONE);
                ENUM_VALUE(DWRITE_OPTICAL_ALIGNMENT_NO_SIDE_BEARINGS);
                END_ENUM(DWRITE_OPTICAL_ALIGNMENT);
            }

            ENUM_TO_STRING(DWRITE_MEASURING_MODE)
            {
                ENUM_VALUE(DWRITE_MEASURING_MODE_NATURAL);
                ENUM_VALUE(DWRITE_MEASURING_MODE_GDI_CLASSIC);
                ENUM_VALUE(DWRITE_MEASURING_MODE_GDI_NATURAL);
                END_ENUM(DWRITE_MEASURING_MODE);
            }

#if WINVER > _WIN32_WINNT_WINBLUE
            ENUM_TO_STRING(DWRITE_FONT_PROPERTY_ID)
            {
                ENUM_VALUE(DWRITE_FONT_PROPERTY_ID_NONE);
                ENUM_VALUE(DWRITE_FONT_PROPERTY_ID_FAMILY_NAME);
                ENUM_VALUE(DWRITE_FONT_PROPERTY_ID_PREFERRED_FAMILY_NAME);
                ENUM_VALUE(DWRITE_FONT_PROPERTY_ID_FACE_NAME);
                ENUM_VALUE(DWRITE_FONT_PROPERTY_ID_FULL_NAME);
                ENUM_VALUE(DWRITE_FONT_PROPERTY_ID_WIN32_FAMILY_NAME);
                ENUM_VALUE(DWRITE_FONT_PROPERTY_ID_POSTSCRIPT_NAME);
                ENUM_VALUE(DWRITE_FONT_PROPERTY_ID_DESIGN_SCRIPT_LANGUAGE_TAG);
                ENUM_VALUE(DWRITE_FONT_PROPERTY_ID_SUPPORTED_SCRIPT_LANGUAGE_TAG);
                ENUM_VALUE(DWRITE_FONT_PROPERTY_ID_SEMANTIC_TAG);
                ENUM_VALUE(DWRITE_FONT_PROPERTY_ID_WEIGHT);
                ENUM_VALUE(DWRITE_FONT_PROPERTY_ID_STRETCH);
                ENUM_VALUE(DWRITE_FONT_PROPERTY_ID_STYLE);
                ENUM_VALUE(DWRITE_FONT_PROPERTY_ID_TOTAL);
                END_ENUM(DWRITE_FONT_PROPERTY_ID);
            }
#endif

            ENUM_TO_STRING(CanvasFigureFill)
            {
                ENUM_VALUE(CanvasFigureFill::Default);
                ENUM_VALUE(CanvasFigureFill::DoesNotAffectFills);
                END_ENUM(CanvasFigureFill);
            }

            ENUM_TO_STRING(CanvasSweepDirection)
            {
                ENUM_VALUE(CanvasSweepDirection::CounterClockwise);
                ENUM_VALUE(CanvasSweepDirection::Clockwise);
                END_ENUM(CanvasSweepDirection);
            }

            ENUM_TO_STRING(CanvasArcSize)
            {
                ENUM_VALUE(CanvasArcSize::Small);
                ENUM_VALUE(CanvasArcSize::Large);
                END_ENUM(CanvasArcSize);
            }

            ENUM_TO_STRING(CanvasFilledRegionDetermination)
            {
                ENUM_VALUE(CanvasFilledRegionDetermination::Alternate);
                ENUM_VALUE(CanvasFilledRegionDetermination::Winding);
                END_ENUM(CanvasFilledRegionDetermination);
            }

            ENUM_TO_STRING(CanvasFigureSegmentOptions)
            {
                ENUM_VALUE(CanvasFigureSegmentOptions::None);
                ENUM_VALUE(CanvasFigureSegmentOptions::ForceUnstroked);
                ENUM_VALUE(CanvasFigureSegmentOptions::ForceRoundLineJoin);
                END_ENUM(CanvasFigureSegmentOptions);
            }

            ENUM_TO_STRING(CanvasFigureLoop)
            {
                ENUM_VALUE(CanvasFigureLoop::Open);
                ENUM_VALUE(CanvasFigureLoop::Closed);
                END_ENUM(CanvasFigureLoop);
            }

            ENUM_TO_STRING(CanvasTextRenderingMode)
            {
                ENUM_VALUE(CanvasTextRenderingMode::Default);
                ENUM_VALUE(CanvasTextRenderingMode::Aliased);
                ENUM_VALUE(CanvasTextRenderingMode::GdiClassic);
                ENUM_VALUE(CanvasTextRenderingMode::GdiNatural);
                ENUM_VALUE(CanvasTextRenderingMode::Natural);
                ENUM_VALUE(CanvasTextRenderingMode::NaturalSymmetric);
                ENUM_VALUE(CanvasTextRenderingMode::Outline);
#if WINVER > _WIN32_WINNT_WINBLUE
                ENUM_VALUE(CanvasTextRenderingMode::NaturalSymmetricDownsampled);
#endif
                END_ENUM(CanvasTextRenderingMode);
            }

            ENUM_TO_STRING(CanvasTextGridFit)
            {
                ENUM_VALUE(CanvasTextGridFit::Default);
                ENUM_VALUE(CanvasTextGridFit::Disable);
                ENUM_VALUE(CanvasTextGridFit::Enable);
                END_ENUM(CanvasTextGridFit);
            }

#if WINVER > _WIN32_WINNT_WINBLUE
            ENUM_TO_STRING(CanvasGradientMeshPatchEdge)
            {
                ENUM_VALUE(CanvasGradientMeshPatchEdge::Aliased);
                ENUM_VALUE(CanvasGradientMeshPatchEdge::Antialiased);
                ENUM_VALUE(CanvasGradientMeshPatchEdge::AliasedAndInflated);
                END_ENUM(CanvasGradientMeshPatchEdge);
            }

            template<>
            inline std::wstring ToString<CanvasGradientMeshPatch>(CanvasGradientMeshPatch const& value)
            {
                return L"CanvasGradientMeshPatch";
            }

            template<>
            inline std::wstring ToString<D2D1_GRADIENT_MESH_PATCH>(D2D1_GRADIENT_MESH_PATCH const& value)
            {
                return L"D2D1_GRADIENT_MESH_PATCH";
            }

            template<>
            inline std::wstring ToString<DWRITE_FONT_PROPERTY>(DWRITE_FONT_PROPERTY const& value)
            {
                wchar_t buf[256];
                ThrowIfFailed(StringCchPrintf(
                    buf,
                    _countof(buf),
                    L"DWRITE_FONT_PROPERTY{%i,%s,%s}",
                    value.propertyId,
                    value.propertyValue,
                    value.localeName));
                return buf;
            }

            ENUM_TO_STRING(CanvasLineSpacingMode)
            {
                ENUM_VALUE(CanvasLineSpacingMode::Default);
                ENUM_VALUE(CanvasLineSpacingMode::Uniform);
                ENUM_VALUE(CanvasLineSpacingMode::Proportional);
                END_ENUM(CanvasLineSpacingMode);
            }
#endif

            ENUM_TO_STRING(CanvasTrimmingSign)
            {
                ENUM_VALUE(CanvasTrimmingSign::None);
                ENUM_VALUE(CanvasTrimmingSign::Ellipsis);
                END_ENUM(CanvasTrimmingSign);
            }

            ENUM_TO_STRING(DWRITE_OUTLINE_THRESHOLD)
            {
                ENUM_VALUE(DWRITE_OUTLINE_THRESHOLD_ANTIALIASED);
                ENUM_VALUE(DWRITE_OUTLINE_THRESHOLD_ALIASED);
                END_ENUM(DWRITE_OUTLINE_THRESHOLD);
            }

            ENUM_TO_STRING(DWRITE_NUMBER_SUBSTITUTION_METHOD)
            {
                ENUM_VALUE(DWRITE_NUMBER_SUBSTITUTION_METHOD_FROM_CULTURE);
                ENUM_VALUE(DWRITE_NUMBER_SUBSTITUTION_METHOD_CONTEXTUAL);
                ENUM_VALUE(DWRITE_NUMBER_SUBSTITUTION_METHOD_NONE);
                ENUM_VALUE(DWRITE_NUMBER_SUBSTITUTION_METHOD_NATIONAL);
                ENUM_VALUE(DWRITE_NUMBER_SUBSTITUTION_METHOD_TRADITIONAL);
                END_ENUM(DWRITE_NUMBER_SUBSTITUTION_METHOD);
            }

            ENUM_TO_STRING(CanvasFontFileFormatType)
            {
                ENUM_VALUE(CanvasFontFileFormatType::Cff);
                ENUM_VALUE(CanvasFontFileFormatType::TrueType);
                ENUM_VALUE(CanvasFontFileFormatType::TrueTypeCollection);
                ENUM_VALUE(CanvasFontFileFormatType::Type1);
                ENUM_VALUE(CanvasFontFileFormatType::Vector);
                ENUM_VALUE(CanvasFontFileFormatType::Bitmap);
                ENUM_VALUE(CanvasFontFileFormatType::Unknown);
                ENUM_VALUE(CanvasFontFileFormatType::RawCff);
                END_ENUM(CanvasFontFileFormatType);
            }

            ENUM_TO_STRING(CanvasFontSimulations)
            {
                ENUM_VALUE(CanvasFontSimulations::None);
                ENUM_VALUE(CanvasFontSimulations::Bold);
                ENUM_VALUE(CanvasFontSimulations::Oblique);
                END_ENUM(CanvasFontSimulations);
            }

            ENUM_TO_STRING(CanvasFontPropertyIdentifier)
            {
                ENUM_VALUE(CanvasFontPropertyIdentifier::None);
                ENUM_VALUE(CanvasFontPropertyIdentifier::FamilyName);
                ENUM_VALUE(CanvasFontPropertyIdentifier::PreferredFamilyName);
                ENUM_VALUE(CanvasFontPropertyIdentifier::FaceName);
                ENUM_VALUE(CanvasFontPropertyIdentifier::FullName);
                ENUM_VALUE(CanvasFontPropertyIdentifier::Win32FamilyName);
                ENUM_VALUE(CanvasFontPropertyIdentifier::PostscriptName);
                ENUM_VALUE(CanvasFontPropertyIdentifier::DesignScriptLanguageTag);
                ENUM_VALUE(CanvasFontPropertyIdentifier::SupportedScriptLanguageTag);
                ENUM_VALUE(CanvasFontPropertyIdentifier::SemanticTag);
                ENUM_VALUE(CanvasFontPropertyIdentifier::Weight);
                ENUM_VALUE(CanvasFontPropertyIdentifier::Stretch);
                ENUM_VALUE(CanvasFontPropertyIdentifier::Style);
                ENUM_VALUE(CanvasFontPropertyIdentifier::Total);
                END_ENUM(CanvasFontPropertyIdentifier);
            }

            ENUM_TO_STRING(DWRITE_INFORMATIONAL_STRING_ID)
            {
                ENUM_VALUE(DWRITE_INFORMATIONAL_STRING_NONE);
                ENUM_VALUE(DWRITE_INFORMATIONAL_STRING_COPYRIGHT_NOTICE);
                ENUM_VALUE(DWRITE_INFORMATIONAL_STRING_VERSION_STRINGS);
                ENUM_VALUE(DWRITE_INFORMATIONAL_STRING_TRADEMARK);
                ENUM_VALUE(DWRITE_INFORMATIONAL_STRING_MANUFACTURER);
                ENUM_VALUE(DWRITE_INFORMATIONAL_STRING_DESIGNER);
                ENUM_VALUE(DWRITE_INFORMATIONAL_STRING_DESIGNER_URL);
                ENUM_VALUE(DWRITE_INFORMATIONAL_STRING_DESCRIPTION);
                ENUM_VALUE(DWRITE_INFORMATIONAL_STRING_FONT_VENDOR_URL);
                ENUM_VALUE(DWRITE_INFORMATIONAL_STRING_LICENSE_DESCRIPTION);
                ENUM_VALUE(DWRITE_INFORMATIONAL_STRING_LICENSE_INFO_URL);
                ENUM_VALUE(DWRITE_INFORMATIONAL_STRING_WIN32_FAMILY_NAMES);
                ENUM_VALUE(DWRITE_INFORMATIONAL_STRING_WIN32_SUBFAMILY_NAMES);
                ENUM_VALUE(DWRITE_INFORMATIONAL_STRING_PREFERRED_FAMILY_NAMES);
                ENUM_VALUE(DWRITE_INFORMATIONAL_STRING_PREFERRED_SUBFAMILY_NAMES);
                ENUM_VALUE(DWRITE_INFORMATIONAL_STRING_SAMPLE_TEXT);
                ENUM_VALUE(DWRITE_INFORMATIONAL_STRING_FULL_NAME);
                ENUM_VALUE(DWRITE_INFORMATIONAL_STRING_POSTSCRIPT_NAME);
                ENUM_VALUE(DWRITE_INFORMATIONAL_STRING_POSTSCRIPT_CID_NAME);
                END_ENUM(DWRITE_INFORMATIONAL_STRING_ID);
            }

            ENUM_TO_STRING(CanvasTextMeasuringMode)
            {
                ENUM_VALUE(CanvasTextMeasuringMode::Natural);
                ENUM_VALUE(CanvasTextMeasuringMode::GdiNatural);
                ENUM_VALUE(CanvasTextMeasuringMode::GdiClassic);
                END_ENUM(CanvasTextMeasuringMode);
            }

            ENUM_TO_STRING(CanvasGlyphOrientation)
            {
                ENUM_VALUE(CanvasGlyphOrientation::Upright);
                ENUM_VALUE(CanvasGlyphOrientation::Clockwise90Degrees);
                ENUM_VALUE(CanvasGlyphOrientation::Clockwise180Degrees);
                ENUM_VALUE(CanvasGlyphOrientation::Clockwise270Degrees);
                END_ENUM(CanvasGlyphOrientation);
            }

            ENUM_TO_STRING(EffectBorderMode)
            {
                ENUM_VALUE(EffectBorderMode::Soft);
                ENUM_VALUE(EffectBorderMode::Hard);
                END_ENUM(EffectBorderMode);
            }

            ENUM_TO_STRING(DWRITE_FONT_FEATURE_TAG)
            {
                ENUM_VALUE(DWRITE_FONT_FEATURE_TAG_ALTERNATIVE_FRACTIONS);
                ENUM_VALUE(DWRITE_FONT_FEATURE_TAG_PETITE_CAPITALS_FROM_CAPITALS);
                ENUM_VALUE(DWRITE_FONT_FEATURE_TAG_SMALL_CAPITALS_FROM_CAPITALS);
                ENUM_VALUE(DWRITE_FONT_FEATURE_TAG_CONTEXTUAL_ALTERNATES);
                ENUM_VALUE(DWRITE_FONT_FEATURE_TAG_CASE_SENSITIVE_FORMS);
                ENUM_VALUE(DWRITE_FONT_FEATURE_TAG_GLYPH_COMPOSITION_DECOMPOSITION);
                ENUM_VALUE(DWRITE_FONT_FEATURE_TAG_CONTEXTUAL_LIGATURES);
                ENUM_VALUE(DWRITE_FONT_FEATURE_TAG_CAPITAL_SPACING);
                ENUM_VALUE(DWRITE_FONT_FEATURE_TAG_CONTEXTUAL_SWASH);
                ENUM_VALUE(DWRITE_FONT_FEATURE_TAG_CURSIVE_POSITIONING);
                ENUM_VALUE(DWRITE_FONT_FEATURE_TAG_DEFAULT);
                ENUM_VALUE(DWRITE_FONT_FEATURE_TAG_DISCRETIONARY_LIGATURES);
                ENUM_VALUE(DWRITE_FONT_FEATURE_TAG_EXPERT_FORMS);
                ENUM_VALUE(DWRITE_FONT_FEATURE_TAG_FRACTIONS);
                ENUM_VALUE(DWRITE_FONT_FEATURE_TAG_FULL_WIDTH);
                ENUM_VALUE(DWRITE_FONT_FEATURE_TAG_HALF_FORMS);
                ENUM_VALUE(DWRITE_FONT_FEATURE_TAG_HALANT_FORMS);
                ENUM_VALUE(DWRITE_FONT_FEATURE_TAG_ALTERNATE_HALF_WIDTH);
                ENUM_VALUE(DWRITE_FONT_FEATURE_TAG_HISTORICAL_FORMS);
                ENUM_VALUE(DWRITE_FONT_FEATURE_TAG_HORIZONTAL_KANA_ALTERNATES);
                ENUM_VALUE(DWRITE_FONT_FEATURE_TAG_HISTORICAL_LIGATURES);
                ENUM_VALUE(DWRITE_FONT_FEATURE_TAG_HALF_WIDTH);
                ENUM_VALUE(DWRITE_FONT_FEATURE_TAG_HOJO_KANJI_FORMS);
                ENUM_VALUE(DWRITE_FONT_FEATURE_TAG_JIS04_FORMS);
                ENUM_VALUE(DWRITE_FONT_FEATURE_TAG_JIS78_FORMS);
                ENUM_VALUE(DWRITE_FONT_FEATURE_TAG_JIS83_FORMS);
                ENUM_VALUE(DWRITE_FONT_FEATURE_TAG_JIS90_FORMS);
                ENUM_VALUE(DWRITE_FONT_FEATURE_TAG_KERNING);
                ENUM_VALUE(DWRITE_FONT_FEATURE_TAG_STANDARD_LIGATURES);
                ENUM_VALUE(DWRITE_FONT_FEATURE_TAG_LINING_FIGURES);
                ENUM_VALUE(DWRITE_FONT_FEATURE_TAG_LOCALIZED_FORMS);
                ENUM_VALUE(DWRITE_FONT_FEATURE_TAG_MARK_POSITIONING);
                ENUM_VALUE(DWRITE_FONT_FEATURE_TAG_MATHEMATICAL_GREEK);
                ENUM_VALUE(DWRITE_FONT_FEATURE_TAG_MARK_TO_MARK_POSITIONING);
                ENUM_VALUE(DWRITE_FONT_FEATURE_TAG_ALTERNATE_ANNOTATION_FORMS);
                ENUM_VALUE(DWRITE_FONT_FEATURE_TAG_NLC_KANJI_FORMS);
                ENUM_VALUE(DWRITE_FONT_FEATURE_TAG_OLD_STYLE_FIGURES);
                ENUM_VALUE(DWRITE_FONT_FEATURE_TAG_ORDINALS);
                ENUM_VALUE(DWRITE_FONT_FEATURE_TAG_PROPORTIONAL_ALTERNATE_WIDTH);
                ENUM_VALUE(DWRITE_FONT_FEATURE_TAG_PETITE_CAPITALS);
                ENUM_VALUE(DWRITE_FONT_FEATURE_TAG_PROPORTIONAL_FIGURES);
                ENUM_VALUE(DWRITE_FONT_FEATURE_TAG_PROPORTIONAL_WIDTHS);
                ENUM_VALUE(DWRITE_FONT_FEATURE_TAG_QUARTER_WIDTHS);
                ENUM_VALUE(DWRITE_FONT_FEATURE_TAG_REQUIRED_LIGATURES);
                ENUM_VALUE(DWRITE_FONT_FEATURE_TAG_RUBY_NOTATION_FORMS);
                ENUM_VALUE(DWRITE_FONT_FEATURE_TAG_STYLISTIC_ALTERNATES);
                ENUM_VALUE(DWRITE_FONT_FEATURE_TAG_SCIENTIFIC_INFERIORS);
                ENUM_VALUE(DWRITE_FONT_FEATURE_TAG_SMALL_CAPITALS);
                ENUM_VALUE(DWRITE_FONT_FEATURE_TAG_SIMPLIFIED_FORMS);
                ENUM_VALUE(DWRITE_FONT_FEATURE_TAG_STYLISTIC_SET_1);
                ENUM_VALUE(DWRITE_FONT_FEATURE_TAG_STYLISTIC_SET_2);
                ENUM_VALUE(DWRITE_FONT_FEATURE_TAG_STYLISTIC_SET_3);
                ENUM_VALUE(DWRITE_FONT_FEATURE_TAG_STYLISTIC_SET_4);
                ENUM_VALUE(DWRITE_FONT_FEATURE_TAG_STYLISTIC_SET_5);
                ENUM_VALUE(DWRITE_FONT_FEATURE_TAG_STYLISTIC_SET_6);
                ENUM_VALUE(DWRITE_FONT_FEATURE_TAG_STYLISTIC_SET_7);
                ENUM_VALUE(DWRITE_FONT_FEATURE_TAG_STYLISTIC_SET_8);
                ENUM_VALUE(DWRITE_FONT_FEATURE_TAG_STYLISTIC_SET_9);
                ENUM_VALUE(DWRITE_FONT_FEATURE_TAG_STYLISTIC_SET_10);
                ENUM_VALUE(DWRITE_FONT_FEATURE_TAG_STYLISTIC_SET_11);
                ENUM_VALUE(DWRITE_FONT_FEATURE_TAG_STYLISTIC_SET_12);
                ENUM_VALUE(DWRITE_FONT_FEATURE_TAG_STYLISTIC_SET_13);
                ENUM_VALUE(DWRITE_FONT_FEATURE_TAG_STYLISTIC_SET_14);
                ENUM_VALUE(DWRITE_FONT_FEATURE_TAG_STYLISTIC_SET_15);
                ENUM_VALUE(DWRITE_FONT_FEATURE_TAG_STYLISTIC_SET_16);
                ENUM_VALUE(DWRITE_FONT_FEATURE_TAG_STYLISTIC_SET_17);
                ENUM_VALUE(DWRITE_FONT_FEATURE_TAG_STYLISTIC_SET_18);
                ENUM_VALUE(DWRITE_FONT_FEATURE_TAG_STYLISTIC_SET_19);
                ENUM_VALUE(DWRITE_FONT_FEATURE_TAG_STYLISTIC_SET_20);
                ENUM_VALUE(DWRITE_FONT_FEATURE_TAG_SUBSCRIPT);
                ENUM_VALUE(DWRITE_FONT_FEATURE_TAG_SUPERSCRIPT);
                ENUM_VALUE(DWRITE_FONT_FEATURE_TAG_SWASH);
                ENUM_VALUE(DWRITE_FONT_FEATURE_TAG_TITLING);
                ENUM_VALUE(DWRITE_FONT_FEATURE_TAG_TRADITIONAL_NAME_FORMS);
                ENUM_VALUE(DWRITE_FONT_FEATURE_TAG_TABULAR_FIGURES);
                ENUM_VALUE(DWRITE_FONT_FEATURE_TAG_TRADITIONAL_FORMS);
                ENUM_VALUE(DWRITE_FONT_FEATURE_TAG_THIRD_WIDTHS);
                ENUM_VALUE(DWRITE_FONT_FEATURE_TAG_UNICASE);
                ENUM_VALUE(DWRITE_FONT_FEATURE_TAG_VERTICAL_WRITING);
                ENUM_VALUE(DWRITE_FONT_FEATURE_TAG_VERTICAL_ALTERNATES_AND_ROTATION);
                ENUM_VALUE(DWRITE_FONT_FEATURE_TAG_SLASHED_ZERO);
                END_ENUM(DWRITE_FONT_FEATURE_TAG);
            }

            ENUM_TO_STRING(CanvasTypographyFeatureName)
            {
                ENUM_VALUE(CanvasTypographyFeatureName::None);
                ENUM_VALUE(CanvasTypographyFeatureName::Default);
                ENUM_VALUE(CanvasTypographyFeatureName::VerticalWriting);
                ENUM_VALUE(CanvasTypographyFeatureName::VerticalAlternatesAndRotation);
                ENUM_VALUE(CanvasTypographyFeatureName::AlternativeFractions);
                ENUM_VALUE(CanvasTypographyFeatureName::PetiteCapitalsFromCapitals);
                ENUM_VALUE(CanvasTypographyFeatureName::SmallCapitalsFromCapitals);
                ENUM_VALUE(CanvasTypographyFeatureName::ContextualAlternates);
                ENUM_VALUE(CanvasTypographyFeatureName::CaseSensitiveForms);
                ENUM_VALUE(CanvasTypographyFeatureName::GlyphCompositionDecomposition);
                ENUM_VALUE(CanvasTypographyFeatureName::ContextualLigatures);
                ENUM_VALUE(CanvasTypographyFeatureName::CapitalSpacing);
                ENUM_VALUE(CanvasTypographyFeatureName::ContextualSwash);
                ENUM_VALUE(CanvasTypographyFeatureName::CursivePositioning);
                ENUM_VALUE(CanvasTypographyFeatureName::DiscretionaryLigatures);
                ENUM_VALUE(CanvasTypographyFeatureName::ExpertForms);
                ENUM_VALUE(CanvasTypographyFeatureName::Fractions);
                ENUM_VALUE(CanvasTypographyFeatureName::FullWidth);
                ENUM_VALUE(CanvasTypographyFeatureName::HalfForms);
                ENUM_VALUE(CanvasTypographyFeatureName::HalantForms);
                ENUM_VALUE(CanvasTypographyFeatureName::AlternateHalfWidth);
                ENUM_VALUE(CanvasTypographyFeatureName::HistoricalForms);
                ENUM_VALUE(CanvasTypographyFeatureName::HorizontalKanaAlternates);
                ENUM_VALUE(CanvasTypographyFeatureName::HistoricalLigatures);
                ENUM_VALUE(CanvasTypographyFeatureName::HalfWidth);
                ENUM_VALUE(CanvasTypographyFeatureName::HojoKanjiForms);
                ENUM_VALUE(CanvasTypographyFeatureName::Jis04Forms);
                ENUM_VALUE(CanvasTypographyFeatureName::Jis78Forms);
                ENUM_VALUE(CanvasTypographyFeatureName::Jis83Forms);
                ENUM_VALUE(CanvasTypographyFeatureName::Jis90Forms);
                ENUM_VALUE(CanvasTypographyFeatureName::Kerning);
                ENUM_VALUE(CanvasTypographyFeatureName::StandardLigatures);
                ENUM_VALUE(CanvasTypographyFeatureName::LiningFigures);
                ENUM_VALUE(CanvasTypographyFeatureName::LocalizedForms);
                ENUM_VALUE(CanvasTypographyFeatureName::MarkPositioning);
                ENUM_VALUE(CanvasTypographyFeatureName::MathematicalGreek);
                ENUM_VALUE(CanvasTypographyFeatureName::MarkToMarkPositioning);
                ENUM_VALUE(CanvasTypographyFeatureName::AlternateAnnotationForms);
                ENUM_VALUE(CanvasTypographyFeatureName::NlcKanjiForms);
                ENUM_VALUE(CanvasTypographyFeatureName::OldStyleFigures);
                ENUM_VALUE(CanvasTypographyFeatureName::Ordinals);
                ENUM_VALUE(CanvasTypographyFeatureName::ProportionalAlternateWidth);
                ENUM_VALUE(CanvasTypographyFeatureName::PetiteCapitals);
                ENUM_VALUE(CanvasTypographyFeatureName::ProportionalFigures);
                ENUM_VALUE(CanvasTypographyFeatureName::ProportionalWidths);
                ENUM_VALUE(CanvasTypographyFeatureName::QuarterWidths);
                ENUM_VALUE(CanvasTypographyFeatureName::RequiredLigatures);
                ENUM_VALUE(CanvasTypographyFeatureName::RubyNotationForms);
                ENUM_VALUE(CanvasTypographyFeatureName::StylisticAlternates);
                ENUM_VALUE(CanvasTypographyFeatureName::ScientificInferiors);
                ENUM_VALUE(CanvasTypographyFeatureName::SmallCapitals);
                ENUM_VALUE(CanvasTypographyFeatureName::SimplifiedForms);
                ENUM_VALUE(CanvasTypographyFeatureName::StylisticSet1);
                ENUM_VALUE(CanvasTypographyFeatureName::StylisticSet2);
                ENUM_VALUE(CanvasTypographyFeatureName::StylisticSet3);
                ENUM_VALUE(CanvasTypographyFeatureName::StylisticSet4);
                ENUM_VALUE(CanvasTypographyFeatureName::StylisticSet5);
                ENUM_VALUE(CanvasTypographyFeatureName::StylisticSet6);
                ENUM_VALUE(CanvasTypographyFeatureName::StylisticSet7);
                ENUM_VALUE(CanvasTypographyFeatureName::StylisticSet8);
                ENUM_VALUE(CanvasTypographyFeatureName::StylisticSet9);
                ENUM_VALUE(CanvasTypographyFeatureName::StylisticSet10);
                ENUM_VALUE(CanvasTypographyFeatureName::StylisticSet11);
                ENUM_VALUE(CanvasTypographyFeatureName::StylisticSet12);
                ENUM_VALUE(CanvasTypographyFeatureName::StylisticSet13);
                ENUM_VALUE(CanvasTypographyFeatureName::StylisticSet14);
                ENUM_VALUE(CanvasTypographyFeatureName::StylisticSet15);
                ENUM_VALUE(CanvasTypographyFeatureName::StylisticSet16);
                ENUM_VALUE(CanvasTypographyFeatureName::StylisticSet17);
                ENUM_VALUE(CanvasTypographyFeatureName::StylisticSet18);
                ENUM_VALUE(CanvasTypographyFeatureName::StylisticSet19);
                ENUM_VALUE(CanvasTypographyFeatureName::StylisticSet20);
                ENUM_VALUE(CanvasTypographyFeatureName::Subscript);
                ENUM_VALUE(CanvasTypographyFeatureName::Superscript);
                ENUM_VALUE(CanvasTypographyFeatureName::Swash);
                ENUM_VALUE(CanvasTypographyFeatureName::Titling);
                ENUM_VALUE(CanvasTypographyFeatureName::TraditionalNameForms);
                ENUM_VALUE(CanvasTypographyFeatureName::TabularFigures);
                ENUM_VALUE(CanvasTypographyFeatureName::TraditionalForms);
                ENUM_VALUE(CanvasTypographyFeatureName::ThirdWidths);
                ENUM_VALUE(CanvasTypographyFeatureName::Unicase);
                ENUM_VALUE(CanvasTypographyFeatureName::SlashedZero);
                END_ENUM(CanvasTypographyFeatureName);
            }

            ENUM_TO_STRING(D2D1_RENDERING_PRIORITY)
            {
                ENUM_VALUE(D2D1_RENDERING_PRIORITY_NORMAL);
                ENUM_VALUE(D2D1_RENDERING_PRIORITY_LOW);
                END_ENUM(D2D1_RENDERING_PRIORITY);
            }

            ENUM_TO_STRING(CanvasScriptShape)
            {
                ENUM_VALUE(CanvasScriptShape::Default);
                ENUM_VALUE(CanvasScriptShape::NoVisual);
                END_ENUM(CanvasScriptShape);
            }

            ENUM_TO_STRING(DWRITE_SCRIPT_SHAPES)
            {
                ENUM_VALUE(DWRITE_SCRIPT_SHAPES_DEFAULT);
                ENUM_VALUE(DWRITE_SCRIPT_SHAPES_NO_VISUAL);
                END_ENUM(DWRITE_SCRIPT_SHAPES);
            }

            ENUM_TO_STRING(DWRITE_GLYPH_ORIENTATION_ANGLE)
            {
                ENUM_VALUE(DWRITE_GLYPH_ORIENTATION_ANGLE_0_DEGREES);
                ENUM_VALUE(DWRITE_GLYPH_ORIENTATION_ANGLE_90_DEGREES);
                ENUM_VALUE(DWRITE_GLYPH_ORIENTATION_ANGLE_180_DEGREES);
                ENUM_VALUE(DWRITE_GLYPH_ORIENTATION_ANGLE_270_DEGREES);
                END_ENUM(DWRITE_GLYPH_ORIENTATION_ANGLE);
            }

            ENUM_TO_STRING(CanvasLineBreakCondition)
            {
                ENUM_VALUE(CanvasLineBreakCondition::Neutral);
                ENUM_VALUE(CanvasLineBreakCondition::CanBreak);
                ENUM_VALUE(CanvasLineBreakCondition::CannotBreak);
                ENUM_VALUE(CanvasLineBreakCondition::MustBreak);
                END_ENUM(CanvasLineBreakCondition);
            }

            ENUM_TO_STRING(CanvasGlyphJustification)
            {
                ENUM_VALUE(CanvasGlyphJustification::None);
                ENUM_VALUE(CanvasGlyphJustification::ArabicBlank);
                ENUM_VALUE(CanvasGlyphJustification::Character);
                ENUM_VALUE(CanvasGlyphJustification::Blank);
                ENUM_VALUE(CanvasGlyphJustification::ArabicNormal);
                ENUM_VALUE(CanvasGlyphJustification::ArabicKashida);
                ENUM_VALUE(CanvasGlyphJustification::ArabicAlef);
                ENUM_VALUE(CanvasGlyphJustification::ArabicHa);
                ENUM_VALUE(CanvasGlyphJustification::ArabicRa);
                ENUM_VALUE(CanvasGlyphJustification::ArabicBa);
                ENUM_VALUE(CanvasGlyphJustification::ArabicBara);
                ENUM_VALUE(CanvasGlyphJustification::ArabicSeen);
                ENUM_VALUE(CanvasGlyphJustification::ArabicSeenM);
                END_ENUM(CanvasGlyphJustification);
            }

            template<typename T>
            inline std::wstring ToStringAsInt(T value)
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
            inline std::wstring ToString<ABI::Windows::UI::Text::FontWeight>(ABI::Windows::UI::Text::FontWeight const& value)
            {
                return ToStringAsInt(value.Weight);
            }

            template<>
            inline std::wstring ToString<WinString>(WinString const& value)
            {
                return static_cast<wchar_t const*>(value);
            }

#define TO_STRING_AS_INT(TYPE)                                          \
            template<>                                                  \
            inline std::wstring ToString<TYPE>(TYPE const& value) \
            {                                                           \
                return ToStringAsInt(value);                            \
            }

            TO_STRING_AS_INT(DWRITE_FONT_WEIGHT);
            TO_STRING_AS_INT(CanvasDrawTextOptions);
            TO_STRING_AS_INT(D2D1_DRAW_TEXT_OPTIONS);
            TO_STRING_AS_INT(DirectXPixelFormat);
            TO_STRING_AS_INT(DXGI_FORMAT);
            TO_STRING_AS_INT(D2D1_BITMAP_OPTIONS);
            TO_STRING_AS_INT(D2D1_ALPHA_MODE);
            TO_STRING_AS_INT(D2D1_DEVICE_CONTEXT_OPTIONS);
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

        inline bool operator==(ABI::Windows::Foundation::Size const& a, ABI::Windows::Foundation::Size const& b)
        {
            return a.Width == b.Width && a.Height == b.Height;
        }

        inline bool operator==(CanvasTimingInformation const& a, CanvasTimingInformation const& b)
        {
            return a.UpdateCount == b.UpdateCount &&
                   a.TotalTime.Duration == b.TotalTime.Duration &&
                   a.ElapsedTime.Duration == b.ElapsedTime.Duration &&
                   a.IsRunningSlowly == b.IsRunningSlowly;
        }

        inline bool operator==(ABI::Windows::Foundation::Point const& a, ABI::Windows::Foundation::Point const& b)
        {
            return a.X == b.X && a.Y == b.Y;
        }

        inline bool operator==(D2D1_SIZE_F const& a, D2D1_SIZE_F const& b)
        {
            return a.width == b.width && a.height == b.height;
        }

        inline bool operator==(Numerics::Vector2 const& a, Numerics::Vector2 const& b)
        {
            return a.X == b.X &&
                   a.Y == b.Y;
        }

        inline bool operator==(Numerics::Vector4 const& a, Numerics::Vector4 const& b)
        {
            return a.X == b.X &&
                   a.Y == b.Y &&
                   a.Z == b.Z &&
                   a.W == b.W;
        }

        inline bool operator==(D2D1_TRIANGLE const& a, D2D1_TRIANGLE const& b)
        {
            return a.point1 == b.point1 &&
                   a.point2 == b.point2 &&
                   a.point3 == b.point3;
        }

#if WINVER > _WIN32_WINNT_WINBLUE
        inline bool operator==(CanvasGradientMeshPatch const& a, CanvasGradientMeshPatch const& b)
        {
            return
                a.Point00 == b.Point00 &&
                a.Point01 == b.Point01 &&
                a.Point02 == b.Point02 &&
                a.Point03 == b.Point03 &&

                a.Point10 == b.Point10 &&
                a.Point11 == b.Point11 &&
                a.Point12 == b.Point12 &&
                a.Point13 == b.Point13 &&

                a.Point20 == b.Point20 &&
                a.Point21 == b.Point21 &&
                a.Point22 == b.Point22 &&
                a.Point23 == b.Point23 &&

                a.Point30 == b.Point30 &&
                a.Point31 == b.Point31 &&
                a.Point32 == b.Point32 &&
                a.Point33 == b.Point33 &&

                a.Color00 == b.Color00 &&
                a.Color03 == b.Color03 &&
                a.Color30 == b.Color30 &&
                a.Color33 == b.Color33 &&

                a.Edge00To03 == b.Edge00To03 &&
                a.Edge03To33 == b.Edge03To33 &&
                a.Edge33To30 == b.Edge33To30 &&
                a.Edge30To00 == b.Edge30To00;
        }

        inline bool operator==(D2D1_GRADIENT_MESH_PATCH const& a, D2D1_GRADIENT_MESH_PATCH const& b)
        {
            return
                a.point00 == b.point00 &&
                a.point01 == b.point01 &&
                a.point02 == b.point02 &&
                a.point03 == b.point03 &&

                a.point10 == b.point10 &&
                a.point11 == b.point11 &&
                a.point12 == b.point12 &&
                a.point13 == b.point13 &&

                a.point20 == b.point20 &&
                a.point21 == b.point21 &&
                a.point22 == b.point22 &&
                a.point23 == b.point23 &&

                a.point30 == b.point30 &&
                a.point31 == b.point31 &&
                a.point32 == b.point32 &&
                a.point33 == b.point33 &&

                a.color00 == b.color00 &&
                a.color03 == b.color03 &&
                a.color30 == b.color30 &&
                a.color33 == b.color33 &&

                a.topEdgeMode == b.topEdgeMode &&
                a.leftEdgeMode == b.leftEdgeMode &&
                a.bottomEdgeMode == b.bottomEdgeMode &&
                a.rightEdgeMode == b.rightEdgeMode;
        }

        inline bool operator==(DWRITE_FONT_PROPERTY const& a, DWRITE_FONT_PROPERTY const& b)
        {
            return a.propertyId == b.propertyId &&
                wcscmp(a.propertyValue, b.propertyValue) == 0 &&
                wcscmp(a.localeName, b.localeName) == 0;
        }
#endif      

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

inline bool operator==(D2D1_MATRIX_5X4_F const& a, D2D1_MATRIX_5X4_F const& b)
{
    return
        a._11 == b._11 && a._12 == b._12 && a._13 == b._13 && a._14 == b._14 &&
        a._21 == b._21 && a._22 == b._22 && a._23 == b._23 && a._24 == b._24 &&
        a._31 == b._31 && a._32 == b._32 && a._33 == b._33 && a._34 == b._34 &&
        a._41 == b._41 && a._42 == b._42 && a._43 == b._43 && a._44 == b._44 &&
        a._51 == b._51 && a._52 == b._52 && a._53 == b._53 && a._54 == b._54;
}

inline bool operator==(DWRITE_MATRIX const& a, DWRITE_MATRIX const& b)
{
    return
        a.m11 == b.m11 && a.m12 == b.m12 &&
        a.m21 == b.m21 && a.m22 == b.m22 &&
        a.dx == b.dx && a.dy == b.dy;
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

    if (expectedHR == S_OK)
    {
        Assert::IsNull(errorInfo.Get());
        return;
    }

    Assert::IsNotNull(errorInfo.Get());

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


inline bool IsWeakRefValid(WeakRef weakRef)
{
    ComPtr<IInspectable> obj;
    ThrowIfFailed(weakRef.As(&obj));
    return static_cast<bool>(obj);
}

template<typename CANVAS_TYPE>
void VerifyConvertDipsToPixels(float dpi, CANVAS_TYPE dpiOwner)
{
    const float testValue = 101;

    const float expectedScaledValue = testValue * dpi / DEFAULT_DPI;
    Assert::AreNotEqual(expectedScaledValue, floorf(expectedScaledValue)); // Should not be integral

    struct TestCase
    {
        CanvasDpiRounding DpiRounding;
        int Expected;
    } testCases[] = {
        { CanvasDpiRounding::Floor, (int)(floorf(expectedScaledValue)) },
        { CanvasDpiRounding::Round, (int)(roundf(expectedScaledValue)) },
        { CanvasDpiRounding::Ceiling, (int)(ceilf(expectedScaledValue)) }
    };

    for (auto testCase : testCases)
    {
        int pixels = 0;
        ThrowIfFailed(dpiOwner->ConvertDipsToPixels(testValue, testCase.DpiRounding, &pixels));
        Assert::AreEqual(testCase.Expected, pixels);
    }

    // Check error case
    int pixels = 0;
    Assert::AreEqual(E_INVALIDARG, dpiOwner->ConvertDipsToPixels(testValue, static_cast<CanvasDpiRounding>(-1), &pixels));

    // Check zero case
    ThrowIfFailed(dpiOwner->ConvertDipsToPixels(0, CanvasDpiRounding::Round, &pixels));
    Assert::AreEqual(0, pixels);
}


inline void VerifyStringMapContainsKeyValue(
    ComPtr<IMapView<HSTRING, HSTRING>> const& stringMap,
    wchar_t const* key,
    wchar_t const* expectedValue)
{
    WinString keyString(key);

    boolean hasKey;
    ThrowIfFailed(stringMap->HasKey(keyString, &hasKey));
    Assert::IsTrue(!!hasKey);

    WinString value;
    ThrowIfFailed(stringMap->Lookup(keyString, value.GetAddressOf()));
    Assert::AreEqual(expectedValue, static_cast<const wchar_t*>(value));
}
