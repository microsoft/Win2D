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

            TO_STRING(IUnknown);
            TO_STRING(IInspectable);
            TO_STRING(ICanvasDevice);
            TO_STRING(ID2D1Brush);
            TO_STRING(ID2D1DeviceContext1);

#undef TO_STRING
        }

        inline bool operator==(const RECT& a, const RECT& b)
        {
            return a.left == b.left &&
                a.top == b.top &&
                a.right == b.right &&
                a.bottom == b.bottom;
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
    }
}

