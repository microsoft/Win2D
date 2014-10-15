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
#include "TextureUtilities.h"
#include "PolymorphicBitmapManager.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas
{
    ScopedBitmapLock::ScopedBitmapLock(ID2D1Bitmap1* d2dBitmap, D2D1_RECT_U const* optionalSubRectangle)
    {
        ComPtr<IDXGISurface> dxgiSurface;
        ThrowIfFailed(d2dBitmap->GetSurface(&dxgiSurface));

        auto dxgiSurface2 = As<IDXGISurface2>(dxgiSurface);

        DXGI_SURFACE_DESC surfaceDescription;
        ThrowIfFailed(dxgiSurface->GetDesc(&surfaceDescription));

        auto bitmapTexture = GetTexture2DForDXGISurface(dxgiSurface2, &m_subresourceIndex);

        D3D11_TEXTURE2D_DESC textureDescription;
        bitmapTexture->GetDesc(&textureDescription);

        ComPtr<ID3D11Device> d3dDevice;
        bitmapTexture->GetDevice(&d3dDevice);

        D3D11_TEXTURE2D_DESC stagingDescription = textureDescription;
        stagingDescription.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
        stagingDescription.BindFlags = 0;
        stagingDescription.Usage = D3D11_USAGE_STAGING;
        stagingDescription.ArraySize = 1;
        stagingDescription.MipLevels = 0;
        stagingDescription.Width = surfaceDescription.Width;
        stagingDescription.Height = surfaceDescription.Height;
        if (optionalSubRectangle)
        {
            assert(optionalSubRectangle->right > optionalSubRectangle->left);
            assert(optionalSubRectangle->bottom > optionalSubRectangle->top);
            stagingDescription.Width = optionalSubRectangle->right - optionalSubRectangle->left;
            stagingDescription.Height = optionalSubRectangle->bottom - optionalSubRectangle->top;
        }

        ComPtr<ID3D11Texture2D> stagingTexture;
        ThrowIfFailed(d3dDevice->CreateTexture2D(&stagingDescription, nullptr, &stagingTexture));

        d3dDevice->GetImmediateContext(&m_immediateContext);

        m_stagingResource = As<ID3D11Resource>(stagingTexture);
        auto sourceResource = As<ID3D11Resource>(bitmapTexture);

        D3D11_BOX sourceBox;
        if (optionalSubRectangle)
        {
            sourceBox.left = optionalSubRectangle->left;
            sourceBox.top = optionalSubRectangle->top;
            sourceBox.right = optionalSubRectangle->right;
            sourceBox.bottom = optionalSubRectangle->bottom;
            sourceBox.front = 0;
            sourceBox.back = 1;
        }

        // 
        // This copies only the requested subrectangle, not the
        // whole texture, for interest of a small perf gain.
        // The copied area is located at (0,0).
        //
        m_immediateContext->CopySubresourceRegion(
            m_stagingResource.Get(),
            0, // Dest subresource
            0, // Dest X
            0, // Dest Y
            0, // Dest Z
            sourceResource.Get(),
            m_subresourceIndex,
            optionalSubRectangle? &sourceBox : nullptr);

        ThrowIfFailed(m_immediateContext->Map(
            m_stagingResource.Get(),
            0, // staging texture doesn't have any subresources
            D3D11_MAP_READ,
            0, // Flags
            &m_mappedSubresource));

        m_lockedBufferSize = m_mappedSubresource.RowPitch * stagingDescription.Height;
    }

    ScopedBitmapLock::~ScopedBitmapLock()
    {
        m_immediateContext->Unmap(m_stagingResource.Get(), m_subresourceIndex);
    }

    void* ScopedBitmapLock::GetLockedData()
    {
        return m_mappedSubresource.pData;
    }

    unsigned int ScopedBitmapLock::GetLockedBufferSize()
    {
        return m_lockedBufferSize;
    }

    unsigned int ScopedBitmapLock::GetStride()
    {
        return m_mappedSubresource.RowPitch;
    }

    unsigned int GetBytesPerPixel(DXGI_FORMAT format)
    {
        switch (format)
        {
        case DXGI_FORMAT_R32G32B32A32_TYPELESS: return 16;
        case DXGI_FORMAT_R32G32B32A32_FLOAT: return 16;
        case DXGI_FORMAT_R32G32B32A32_UINT: return 16;
        case DXGI_FORMAT_R32G32B32A32_SINT: return 16;
        case DXGI_FORMAT_R32G32B32_TYPELESS: return 12;
        case DXGI_FORMAT_R32G32B32_FLOAT: return 12;
        case DXGI_FORMAT_R32G32B32_UINT: return 12;
        case DXGI_FORMAT_R32G32B32_SINT: return 12;
        case DXGI_FORMAT_R16G16B16A16_TYPELESS: return 8;
        case DXGI_FORMAT_R16G16B16A16_FLOAT: return 8;
        case DXGI_FORMAT_R16G16B16A16_UNORM: return 8;
        case DXGI_FORMAT_R16G16B16A16_UINT: return 8;
        case DXGI_FORMAT_R16G16B16A16_SNORM: return 8;
        case DXGI_FORMAT_R16G16B16A16_SINT: return 8;
        case DXGI_FORMAT_R32G32_TYPELESS: return 8;
        case DXGI_FORMAT_R32G32_FLOAT: return 8;
        case DXGI_FORMAT_R32G32_UINT: return 8;
        case DXGI_FORMAT_R32G32_SINT: return 8;
        case DXGI_FORMAT_R32G8X24_TYPELESS: return 8;
        case DXGI_FORMAT_D32_FLOAT_S8X24_UINT: return 8;
        case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS: return 8;
        case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT: return 8;
        case DXGI_FORMAT_R10G10B10A2_TYPELESS: return 4;
        case DXGI_FORMAT_R10G10B10A2_UNORM: return 4;
        case DXGI_FORMAT_R10G10B10A2_UINT: return 4;
        case DXGI_FORMAT_R11G11B10_FLOAT: return 4;
        case DXGI_FORMAT_R8G8B8A8_TYPELESS: return 4;
        case DXGI_FORMAT_R8G8B8A8_UNORM: return 4;
        case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB: return 4;
        case DXGI_FORMAT_R8G8B8A8_UINT: return 4;
        case DXGI_FORMAT_R8G8B8A8_SNORM: return 4;
        case DXGI_FORMAT_R8G8B8A8_SINT: return 4;
        case DXGI_FORMAT_R16G16_TYPELESS: return 4;
        case DXGI_FORMAT_R16G16_FLOAT: return 4;
        case DXGI_FORMAT_R16G16_UNORM: return 4;
        case DXGI_FORMAT_R16G16_UINT: return 4;
        case DXGI_FORMAT_R16G16_SNORM: return 4;
        case DXGI_FORMAT_R16G16_SINT: return 4;
        case DXGI_FORMAT_R32_TYPELESS: return 4;
        case DXGI_FORMAT_D32_FLOAT: return 4;
        case DXGI_FORMAT_R32_FLOAT: return 4;
        case DXGI_FORMAT_R32_UINT: return 4;
        case DXGI_FORMAT_R32_SINT: return 4;
        case DXGI_FORMAT_R24G8_TYPELESS: return 4;
        case DXGI_FORMAT_D24_UNORM_S8_UINT: return 4;
        case DXGI_FORMAT_R24_UNORM_X8_TYPELESS: return 4;
        case DXGI_FORMAT_X24_TYPELESS_G8_UINT: return 4;
        case DXGI_FORMAT_R8G8_TYPELESS: return 2;
        case DXGI_FORMAT_R8G8_UNORM: return 2;
        case DXGI_FORMAT_R8G8_UINT: return 2;
        case DXGI_FORMAT_R8G8_SNORM: return 2;
        case DXGI_FORMAT_R8G8_SINT: return 2;
        case DXGI_FORMAT_R16_TYPELESS: return 2;
        case DXGI_FORMAT_R16_FLOAT: return 2;
        case DXGI_FORMAT_D16_UNORM: return 2;
        case DXGI_FORMAT_R16_UNORM: return 2;
        case DXGI_FORMAT_R16_UINT: return 2;
        case DXGI_FORMAT_R16_SNORM: return 2;
        case DXGI_FORMAT_R16_SINT: return 2;
        case DXGI_FORMAT_R8_TYPELESS: return 1;
        case DXGI_FORMAT_R8_UNORM: return 1;
        case DXGI_FORMAT_R8_UINT: return 1;
        case DXGI_FORMAT_R8_SNORM: return 1;
        case DXGI_FORMAT_R8_SINT: return 1;
        case DXGI_FORMAT_A8_UNORM: return 1;
        case DXGI_FORMAT_R9G9B9E5_SHAREDEXP: return 4;
        case DXGI_FORMAT_B5G6R5_UNORM: return 2;
        case DXGI_FORMAT_B5G5R5A1_UNORM: return 2;
        case DXGI_FORMAT_B8G8R8A8_UNORM: return 4;
        case DXGI_FORMAT_B8G8R8X8_UNORM: return 4;
        case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM: return 4;
        case DXGI_FORMAT_B8G8R8A8_TYPELESS: return 4;
        case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB: return 4;
        case DXGI_FORMAT_B8G8R8X8_TYPELESS: return 4;
        case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB: return 4;        
        case DXGI_FORMAT_P8: return 1;
        case DXGI_FORMAT_A8P8: return 2;
        case DXGI_FORMAT_B4G4R4A4_UNORM: return 2;
        default:
            // Some formats such as DXGI_FORMAT_UNKNOWN, and some block-compressed formats
            // do not have valid sizes here.
            ThrowHR(E_INVALIDARG);
        }
    }

    ComPtr<ID3D11Texture2D> GetTexture2DForDXGISurface(
        ComPtr<IDXGISurface2> const& dxgiSurface,
        uint32_t* subresourceIndexOut)
    {
        // GetResource is called in case the interopped texture is actually a
        // subresource. If the texture is not a subresource, this is equivalent
        // to a QI to ID3D11Texture2D and will always yield subresource index 0.
        //
        ComPtr<ID3D11Texture2D> parentResource;
        UINT subresourceIndex;
        ThrowIfFailed(dxgiSurface->GetResource(IID_PPV_ARGS(&parentResource), &subresourceIndex));

        if (subresourceIndexOut) *subresourceIndexOut = subresourceIndex;

        return parentResource;
    }

} } } }