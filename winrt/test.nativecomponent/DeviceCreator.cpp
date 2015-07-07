// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

namespace NativeComponent
{
    using namespace Microsoft::Graphics::Canvas;
    using namespace WinRTDirectX;

    public ref class DeviceCreator sealed
    {
    public:
        static IDirect3DDevice^ CreateDevice()
        {
            using namespace Microsoft::WRL;

            ComPtr<ID3D11Device> d3dDevice;

            if (FAILED(D3D11CreateDevice(
                nullptr,            // adapter
                D3D_DRIVER_TYPE_WARP,
                nullptr,            // software
                D3D11_CREATE_DEVICE_DEBUG,
                nullptr,            // feature levels
                0,                  // feature levels count
                D3D11_SDK_VERSION,
                &d3dDevice,
                nullptr,        // feature level
                nullptr)))
            {
                __abi_ThrowIfFailed(D3D11CreateDevice(
                    nullptr,            // adapter
                    D3D_DRIVER_TYPE_WARP,
                    nullptr,            // software
                    0,                    // flags
                    nullptr,            // feature levels
                    0,                  // feature levels count
                    D3D11_SDK_VERSION,
                    &d3dDevice,
                    nullptr,        // feature level
                    nullptr));      // immediate context
            }

            ComPtr<IDXGIDevice> dxgiDevice;
            __abi_ThrowIfFailed(d3dDevice.As(&dxgiDevice));
            return CreateDirect3DDevice(dxgiDevice.Get());
        }
    };
}
