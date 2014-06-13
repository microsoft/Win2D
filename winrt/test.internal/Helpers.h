// Copyright (c) Microsoft Corporation.  All rights reserved

#pragma once

namespace Microsoft
{
    namespace VisualStudio
    {
        namespace CppUnitTestFramework
        {
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
            TO_STRING(IDXGIDevice);
            TO_STRING(IDirectX11Device);
            TO_STRING(IInspectable);
            TO_STRING(IUnknown);

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
            static inline std::wstring ToString<Math::Matrix3x2>(const Math::Matrix3x2& value)
            {
                wchar_t buf[256];
                ThrowIfFailed(StringCchPrintf(
                    buf,
                    _countof(buf),
                    L"Math.Matrix{M11=%f,M12=%f,M21=%f,M22=%f,M31=%f,M32=%f}",
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

        inline bool operator==(const Math::Matrix3x2& a, const Math::Matrix3x2& b)
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
    }
}

