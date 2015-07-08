// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

namespace NativeComponent
{
    using namespace Microsoft::Graphics::Canvas;
    using namespace Microsoft::WRL;
    using namespace WinRTDirectX;

    public ref class SurfaceCreator sealed
    {
    public:
        static IDirect3DSurface^ CreateSurface(
            IDirect3DDevice^ device,
            int32_t width,
            int32_t height,
            DirectXPixelFormat format)
        {
            ComPtr<ID3D11Device> d3dDevice;
            ThrowIfFailed(GetDXGIInterface(device, d3dDevice.GetAddressOf()));

            CD3D11_TEXTURE2D_DESC textureDesc(
                static_cast<DXGI_FORMAT>(format),
                width,
                height,
                1,              // array levels and mipmaps need to be 1
                1);             // otherwise we can't get the IDXGISurface from
                                // this texture

            ComPtr<ID3D11Texture2D> texture;
            ThrowIfFailed(d3dDevice->CreateTexture2D(
                &textureDesc,
                nullptr,
                &texture));

            ComPtr<IDXGISurface> dxgiSurface;
            ThrowIfFailed(texture.As(&dxgiSurface));

            return CreateDirect3DSurface(dxgiSurface.Get());
        }
    };
}
