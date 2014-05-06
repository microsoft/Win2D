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

            template<>
            static inline std::wstring ToString<IDXGIDevice>(IDXGIDevice* value)
            {
                return PointerToString(L"IDXGIDevice", value);
            }

            template<>
            static inline std::wstring ToString<ID3D11Device>(ID3D11Device* value)
            {
                return PointerToString(L"ID3D11Device", value);
            }

            template<>
            static inline std::wstring ToString<ID3D11Texture2D>(ID3D11Texture2D* value)
            {
                return PointerToString(L"ID3D11Texture2D", value);
            }

            template<>
            static inline std::wstring ToString<IDXGISurface>(IDXGISurface* value)
            {
                return PointerToString(L"IDXGISurface", value);
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
