// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#include "utils/D2DResourceLock.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas
{
    class ScopedBitmapMappedPixelAccess : LifespanTracker<ScopedBitmapMappedPixelAccess>
    {
        D3D11_MAPPED_SUBRESOURCE m_mappedSubresource;
        unsigned int m_subresourceIndex;
        unsigned int m_lockedBufferSize;
        ComPtr<ID3D11Resource> m_sourceResource;
        ComPtr<ID3D11Resource> m_stagingResource;
        ComPtr<ID3D11DeviceContext> m_immediateContext;
        D3D11_MAP m_mapType;
        D2D1_RECT_U m_subRectangle;
        bool m_useSubrectangle;

        D2DResourceLock m_d2dResourceLock;

    public:
        ScopedBitmapMappedPixelAccess(ID2D1Bitmap1* d2dBitmap, D3D11_MAP mapType, D2D1_RECT_U const* optionalSubRectangle = nullptr);

        ~ScopedBitmapMappedPixelAccess();

        void* GetLockedData();

        unsigned int GetLockedBufferSize();

        unsigned int GetStride();
    };


    unsigned int GetBytesPerPixel(DXGI_FORMAT format);


    ComPtr<ID3D11Texture2D> GetTexture2DForDXGISurface(
        ComPtr<IDXGISurface2> const& dxgiSurface,
        uint32_t* subresourceIndexOut = nullptr);


    inline bool IsRenderTargetBitmap(ID2D1Bitmap1* d2dBitmap)
    {
        auto options = d2dBitmap->GetOptions();
        return (options & D2D1_BITMAP_OPTIONS_TARGET) != 0;
    }

}}}}
