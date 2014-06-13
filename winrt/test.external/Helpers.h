// Copyright (c) Microsoft Corporation.  All rights reserved

#pragma once

namespace Microsoft
{
    namespace VisualStudio
    {
        namespace CppUnitTestFramework
        {
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

            TO_STRING(IDXGIDevice);
            TO_STRING(ID3D11Device);
            TO_STRING(ID3D11Texture2D);
            TO_STRING(IDXGISurface);
            TO_STRING(ID2D1Device1);

#undef TO_STRING

            template<>
            static inline std::wstring ToString<Windows::UI::Color>(Windows::UI::Color* value)
            {
                return value->ToString()->Data();
            }

            template<>
            static inline std::wstring ToString<Windows::UI::Color>(const Windows::UI::Color& value)
            {
                return L"Color";
            }

            template<>
            static inline std::wstring ToString<Microsoft::Graphics::Canvas::Math::Matrix3x2>(const Microsoft::Graphics::Canvas::Math::Matrix3x2& value)
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

            // TODO: Consider sharing these definitions somehow between internal and external tests.
#define CX_VALUE_TO_STRING(T)                                       \
            template<>                                              \
            static inline std::wstring ToString<T>(const T& value)  \
            {                                                       \
                return value.ToString()->Data();                    \
            }
            
            CX_VALUE_TO_STRING(Microsoft::Graphics::Canvas::CanvasHardwareAcceleration);

#undef CX_VALUE_TO_STRING

            inline bool operator==(const Windows::UI::Color& a, const Windows::UI::Color& b)
            {
                return a.A == b.A &&
                    a.R == b.R &&
                    a.G == b.G &&
                    a.B == b.B;
            }

            inline bool operator==(const Microsoft::Graphics::Canvas::Math::Matrix3x2& a, const Microsoft::Graphics::Canvas::Math::Matrix3x2& b)
            {
                return
                    a.M11 == b.M11 && a.M12 == b.M12 &&
                    a.M21 == b.M21 && a.M22 == b.M22 &&
                    a.M31 == b.M31 && a.M32 == b.M32;
            }
        }
    }
}


inline ComPtr<ID3D11Device> CreateD3DDevice()
{
    ComPtr<ID3D11Device> d3dDevice;
    D3D_FEATURE_LEVEL featureLevel;
    ComPtr<ID3D11DeviceContext> immediateContext;

    ThrowIfFailed(D3D11CreateDevice(
        nullptr,            // adapter
        D3D_DRIVER_TYPE_WARP,
        nullptr,            // software
        D3D11_CREATE_DEVICE_DEBUG,
        nullptr,            // feature levels
        0,                  // feature levels count
        D3D11_SDK_VERSION,
        &d3dDevice,
        &featureLevel,
        &immediateContext));

    return d3dDevice;
}

template<typename ACTION_ON_CLOSED_OBJECT>
void ExpectObjectClosed(ACTION_ON_CLOSED_OBJECT&& fn)
{
    Assert::ExpectException<Platform::ObjectDisposedException^>(
        [&]()
        {
            fn();
        });
}
