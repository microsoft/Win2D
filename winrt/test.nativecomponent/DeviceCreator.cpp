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
            return CreateDevice(false);
        }

        static IDirect3DDevice^ CreateDevice(bool useFeatureLevel93)
        {
            using namespace Microsoft::WRL;

            ComPtr<ID3D11Device> d3dDevice;

            D3D_FEATURE_LEVEL const* featureLevels;
            unsigned featureLevelsCount;

            if (useFeatureLevel93)
            {
                static const D3D_FEATURE_LEVEL featureLevel93 = D3D_FEATURE_LEVEL_9_3;

                featureLevels = &featureLevel93;
                featureLevelsCount = 1;
            }
            else
            {
                featureLevels = nullptr;
                featureLevelsCount = 0;
            }

            if (FAILED(D3D11CreateDevice(
                nullptr,            // adapter
                D3D_DRIVER_TYPE_WARP,
                nullptr,            // software
                D3D11_CREATE_DEVICE_DEBUG | D3D11_CREATE_DEVICE_BGRA_SUPPORT,
                featureLevels,
                featureLevelsCount,
                D3D11_SDK_VERSION,
                &d3dDevice,
                nullptr,        // feature level
                nullptr)))
            {
                __abi_ThrowIfFailed(D3D11CreateDevice(
                    nullptr,            // adapter
                    D3D_DRIVER_TYPE_WARP,
                    nullptr,            // software
                    D3D11_CREATE_DEVICE_BGRA_SUPPORT,
                    featureLevels,
                    featureLevelsCount,
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
