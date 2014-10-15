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

#pragma once

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas
{
    class ScopedBitmapLock
    {
        D3D11_MAPPED_SUBRESOURCE m_mappedSubresource;
        unsigned int m_subresourceIndex;
        unsigned int m_lockedBufferSize;
        ComPtr<ID3D11Resource> m_stagingResource;
        ComPtr<ID3D11DeviceContext> m_immediateContext;

    public:
        ScopedBitmapLock(ID2D1Bitmap1* d2dBitmap, D2D1_RECT_U const* optionalSubRectangle = nullptr);

        ~ScopedBitmapLock();

        void* GetLockedData();

        unsigned int GetLockedBufferSize();

        unsigned int GetStride();
    };

    unsigned int GetBytesPerPixel(DXGI_FORMAT format);

    ComPtr<ID3D11Texture2D> GetTexture2DForDXGISurface(
        ComPtr<IDXGISurface2> const& dxgiSurface,
        uint32_t* subresourceIndexOut = nullptr);

} } } }