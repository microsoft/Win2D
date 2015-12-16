// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"
#include "TextureUtilities.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas
{
    ScopedBitmapMappedPixelAccess::ScopedBitmapMappedPixelAccess(ID2D1Bitmap1* d2dBitmap, D3D11_MAP mapType, D2D1_RECT_U const* optionalSubRectangle)
        : m_mapType(mapType)
        , m_useSubrectangle(false)
        , m_d2dResourceLock(d2dBitmap)
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

        assert(m_mapType == D3D11_MAP_READ || m_mapType == D3D11_MAP_WRITE);
        UINT cpuAccessFlags = 
            m_mapType == D3D11_MAP_READ ? D3D11_CPU_ACCESS_READ : D3D11_CPU_ACCESS_WRITE;

        D3D11_TEXTURE2D_DESC stagingDescription = textureDescription;
        stagingDescription.CPUAccessFlags = cpuAccessFlags;
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

            m_useSubrectangle = true;
            m_subRectangle = *optionalSubRectangle;
        }

        ComPtr<ID3D11Texture2D> stagingTexture;
        ThrowIfFailed(d3dDevice->CreateTexture2D(&stagingDescription, nullptr, &stagingTexture));

        d3dDevice->GetImmediateContext(&m_immediateContext);

        m_stagingResource = As<ID3D11Resource>(stagingTexture);
        m_sourceResource = As<ID3D11Resource>(bitmapTexture);

        // 
        // This class copies only the requested subrectangle, not the
        // whole texture, in the interest of a small perf gain.
        // The copied area is located at (0,0).
        //
        if (m_mapType == D3D11_MAP_READ)
        {
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

            m_immediateContext->CopySubresourceRegion(
                m_stagingResource.Get(),
                0, // Dest subresource
                0, // Dest X
                0, // Dest Y
                0, // Dest Z
                m_sourceResource.Get(),
                m_subresourceIndex,
                optionalSubRectangle ? &sourceBox : nullptr);
        }

        ThrowIfFailed(m_immediateContext->Map(
            m_stagingResource.Get(),
            0, // staging texture doesn't have any subresources
            m_mapType,
            0, // Flags
            &m_mappedSubresource));

        m_lockedBufferSize = m_mappedSubresource.RowPitch * stagingDescription.Height;
    }

    ScopedBitmapMappedPixelAccess::~ScopedBitmapMappedPixelAccess()
    {
        m_immediateContext->Unmap(m_stagingResource.Get(), m_subresourceIndex);

        if (m_mapType == D3D11_MAP_WRITE)
        {
            UINT destX = 0;
            UINT destY = 0;
            if (m_useSubrectangle)
            {
                destX = m_subRectangle.left;
                destY = m_subRectangle.top;
            }

            m_immediateContext->CopySubresourceRegion(
                m_sourceResource.Get(),
                m_subresourceIndex, // Dest subresource
                destX, // Dest X
                destY, // Dest Y
                0, // Dest Z
                m_stagingResource.Get(),
                0, // Source subresource
                nullptr // Source box
                );
        }
    }

    uint8_t* ScopedBitmapMappedPixelAccess::GetLockedData()
    {
        return static_cast<uint8_t*>(m_mappedSubresource.pData);
    }

    unsigned int ScopedBitmapMappedPixelAccess::GetLockedBufferSize()
    {
        return m_lockedBufferSize;
    }

    unsigned int ScopedBitmapMappedPixelAccess::GetStride()
    {
        return m_mappedSubresource.RowPitch;
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

}}}}
