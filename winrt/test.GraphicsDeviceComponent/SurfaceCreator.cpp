// Copyright (c) Microsoft Corporation.  All rights reserved

#include "pch.h"

namespace GraphicsDeviceComponent
{
    using namespace Microsoft::Graphics::Canvas;
    using namespace Microsoft::WRL;

    public ref class SurfaceCreator sealed
    {
    public:
        static DirectX11Surface^ CreateSurface(
            DirectX11Device^ device,
            uint32_t width,
            uint32_t height,
            GraphicsResourceFormat format)
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

            return CreateDirectX11Surface(dxgiSurface.Get());
        }
    };
}
