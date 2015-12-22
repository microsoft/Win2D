// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"
#include "TextureUtilities.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas
{
    template<typename TSrc, typename TDest, typename TConverter>
    static void CopyBetweenPixelFormats(D3D11_MAPPED_SUBRESOURCE const& src, D3D11_MAPPED_SUBRESOURCE const& dest, unsigned width, unsigned height, TConverter&& converter)
    {
        auto srcData = reinterpret_cast<uint8_t*>(src.pData);
        auto destData = reinterpret_cast<uint8_t*>(dest.pData);

        for (unsigned y = height; y > 0; y--)
        {
            auto s = reinterpret_cast<TSrc*>(srcData);
            auto d = reinterpret_cast<TDest*>(destData);

            for (unsigned x = width; x > 0; x--)
            {
                *(d++) = converter(*(s++));
            }

            srcData += src.RowPitch;
            destData += dest.RowPitch;
        }
    }


    ScopedBitmapMappedPixelAccess::ScopedBitmapMappedPixelAccess(ID2D1Bitmap1* d2dBitmap, D3D11_MAP mapType, D2D1_RECT_U const* optionalSubRectangle)
        : m_mapType(mapType)
        , m_useSubrectangle(false)
        , m_emulateA8WithBgra32(false)
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

        // On feature level 9.x hardware, D2D sometimes emulates A8 format using an
        // internal BGRA32 surface. When directly mapping the underlying DXGI surface,
        // we need to account for this and convert between A8 and BGRA32.
        auto d2dFormat = d2dBitmap->GetPixelFormat().format;

        if (textureDescription.Format != d2dFormat)
        {
            // The only time the D2D and DXGI formats are expected to differ is this A8 -> BGRA32 emulation path.
            if (d2dFormat != DXGI_FORMAT_A8_UNORM || textureDescription.Format != DXGI_FORMAT_B8G8R8A8_UNORM)
                ThrowHR(E_UNEXPECTED);

            m_emulateA8WithBgra32 = true;
            m_mappedBgra = m_mappedSubresource;

            // Update m_mappedSubresource to point to a temporary m_alphaBuffer,
            // rather than directly to the real DXGI surface. Callers will then
            // party on m_alphaBuffer, which is in A8 format like they expect.
            m_lockedBufferSize = stagingDescription.Width * stagingDescription.Height;
            m_alphaBuffer.resize(m_lockedBufferSize);
            m_mappedSubresource = D3D11_MAPPED_SUBRESOURCE{ m_alphaBuffer.data(), stagingDescription.Width };

            if (m_mapType == D3D11_MAP_READ)
            {
                // Convert BGRA32 -> A8.
                CopyBetweenPixelFormats<uint32_t, uint8_t>(
                    m_mappedBgra, 
                    m_mappedSubresource, 
                    stagingDescription.Width, 
                    stagingDescription.Height, 
                    [](uint32_t value) { return static_cast<uint8_t>(value >> 24); });
            }
        }
        else
        {
            m_lockedBufferSize = m_mappedSubresource.RowPitch * stagingDescription.Height;
        }
    }

    ScopedBitmapMappedPixelAccess::~ScopedBitmapMappedPixelAccess()
    {
        m_immediateContext->Unmap(m_stagingResource.Get(), m_subresourceIndex);

        if (m_mapType == D3D11_MAP_WRITE)
        {
            if (m_emulateA8WithBgra32)
            {
                // This size calculation is valid because in alpha emulation mode, m_alphaBuffer is always tightly packed.
                auto width = m_mappedSubresource.RowPitch;
                auto height = m_lockedBufferSize / width;

                // Convert A8 -> BGRA32.
                CopyBetweenPixelFormats<uint8_t, uint32_t>(
                    m_mappedSubresource, 
                    m_mappedBgra, 
                    width, 
                    height,
                    [](uint8_t value) { return static_cast<uint32_t>(value) << 24; });
            }

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
