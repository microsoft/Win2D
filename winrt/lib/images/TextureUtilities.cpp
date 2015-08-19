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

    void* ScopedBitmapMappedPixelAccess::GetLockedData()
    {
        return m_mappedSubresource.pData;
    }

    unsigned int ScopedBitmapMappedPixelAccess::GetLockedBufferSize()
    {
        return m_lockedBufferSize;
    }

    unsigned int ScopedBitmapMappedPixelAccess::GetStride()
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


    ComPtr<ID2D1Bitmap1> CreateD2DBitmap(
        ICanvasDevice* canvasDevice, 
        IDirect3DSurface* surface,
        float dpi,
        CanvasAlphaMode alpha)
    {
        auto dxgiSurface = GetDXGIInterface<IDXGISurface2>(surface);
        auto deviceContext = As<ICanvasDeviceInternal>(canvasDevice)->CreateDeviceContext();

        D2D1_BITMAP_PROPERTIES1 bitmapProperties = D2D1::BitmapProperties1();
        bitmapProperties.pixelFormat.alphaMode = ToD2DAlphaMode(alpha);
        bitmapProperties.dpiX = dpi;
        bitmapProperties.dpiY = dpi;

        // D2D requires bitmap flags that match the surface format, if a
        // D2D1_BITMAP_PROPERTIES1 is specified.
        //
        ComPtr<ID3D11Texture2D> parentResource = GetTexture2DForDXGISurface(dxgiSurface);

        ComPtr<IDXGIResource1> dxgiResource;
        ThrowIfFailed(parentResource.As(&dxgiResource));

        DXGI_USAGE dxgiUsage;
        ThrowIfFailed(dxgiResource->GetUsage(&dxgiUsage));

        D3D11_TEXTURE2D_DESC texture2DDesc;
        parentResource->GetDesc(&texture2DDesc);

        if (texture2DDesc.BindFlags & D3D11_BIND_RENDER_TARGET && !(dxgiUsage & DXGI_USAGE_READ_ONLY))
        {
            bitmapProperties.bitmapOptions |= D2D1_BITMAP_OPTIONS_TARGET;
        }

        if (!(texture2DDesc.BindFlags & D3D11_BIND_SHADER_RESOURCE))
        {
            bitmapProperties.bitmapOptions |= D2D1_BITMAP_OPTIONS_CANNOT_DRAW;
        }

        if (texture2DDesc.Usage & D3D11_USAGE_STAGING && texture2DDesc.CPUAccessFlags & D3D11_CPU_ACCESS_READ)
        {
            bitmapProperties.bitmapOptions |= D2D1_BITMAP_OPTIONS_CPU_READ;
        }
        
        ComPtr<ID2D1Bitmap1> d2dBitmap;
        ThrowIfFailed(deviceContext->CreateBitmapFromDxgiSurface(
            dxgiSurface.Get(),
            &bitmapProperties,
            &d2dBitmap));

        return d2dBitmap;
    }

}}}}
