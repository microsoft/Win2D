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

namespace NativeComponent
{
    using namespace Microsoft::Graphics::Canvas;
    using namespace Microsoft::Graphics::Canvas::DirectX::Direct3D11;

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
