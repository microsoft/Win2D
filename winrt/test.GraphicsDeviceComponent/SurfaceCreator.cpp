// Copyright (c) Microsoft Corporation.  All rights reserved

#include "pch.h"

namespace GraphicsDeviceComponent
{
    using namespace Microsoft::Graphics::Canvas;
    using namespace Microsoft::Graphics::Canvas::DirectX;
    using namespace Microsoft::Graphics::Canvas::DirectX::Direct3D11;
    using namespace Microsoft::WRL;

    public ref class SurfaceCreator sealed
    {
    public:
        static Direct3DSurface^ CreateSurface(
            Direct3DDevice^ device,
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
