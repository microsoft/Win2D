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
