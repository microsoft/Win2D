// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#include "utils/D2DResourceLock.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas
{
    class ScopedBitmapMappedPixelAccess : LifespanTracker<ScopedBitmapMappedPixelAccess>
    {
        D2D1_MAPPED_RECT m_mappedSubresource;
        unsigned int m_lockedBufferSize;
        ComPtr<ID2D1Bitmap1> m_stagingResource;

    public:
        ScopedBitmapMappedPixelAccess(ICanvasDevice* device, ID2D1Bitmap1* d2dBitmap, D2D1_RECT_U const* optionalSubRectangle = nullptr);
        ~ScopedBitmapMappedPixelAccess();

        uint8_t* GetLockedData()           const { return m_mappedSubresource.bits; }
        unsigned int GetLockedBufferSize() const { return m_lockedBufferSize; }
        unsigned int GetStride()           const { return m_mappedSubresource.pitch; }
    };


    inline stdext::checked_array_iterator<uint8_t*> begin(ScopedBitmapMappedPixelAccess& pixels)
    {
        return stdext::make_checked_array_iterator(pixels.GetLockedData(), pixels.GetLockedBufferSize());
    }

}}}}
