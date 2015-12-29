// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"
#include "ScopedBitmapMappedPixelAccess.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas
{
    ScopedBitmapMappedPixelAccess::ScopedBitmapMappedPixelAccess(ICanvasDevice* device, ID2D1Bitmap1* d2dBitmap, D2D1_RECT_U const* optionalSubRectangle)
    {
        auto bitmapSize = d2dBitmap->GetPixelSize();
        
        auto bitmapOptions = D2D1_BITMAP_OPTIONS_CPU_READ | 
                             D2D1_BITMAP_OPTIONS_CANNOT_DRAW;
        
        D2D1_BITMAP_PROPERTIES1 bitmapProperties = D2D1::BitmapProperties1(bitmapOptions, d2dBitmap->GetPixelFormat());

        if (optionalSubRectangle)
        {
            assert(optionalSubRectangle->right > optionalSubRectangle->left);
            assert(optionalSubRectangle->bottom > optionalSubRectangle->top);

            bitmapSize.width = optionalSubRectangle->right - optionalSubRectangle->left;
            bitmapSize.height = optionalSubRectangle->bottom - optionalSubRectangle->top;
        }

        auto deviceContext = As<ICanvasDeviceInternal>(device)->GetResourceCreationDeviceContext();

        ThrowIfFailed(deviceContext->CreateBitmap(
            bitmapSize, 
            nullptr,
            0,
            &bitmapProperties,
            &m_stagingResource));

        // 
        // This class copies only the requested subrectangle, not the
        // whole texture, in the interest of a small perf gain.
        // The copied area is located at (0,0).
        //
        ThrowIfFailed(m_stagingResource->CopyFromBitmap(
            nullptr, 
            d2dBitmap,
            optionalSubRectangle));

        ThrowIfFailed(m_stagingResource->Map(
            D2D1_MAP_OPTIONS_READ,
            &m_mappedSubresource));

        m_lockedBufferSize = m_mappedSubresource.pitch * bitmapSize.height;
    }


    ScopedBitmapMappedPixelAccess::~ScopedBitmapMappedPixelAccess()
    {
        ThrowIfFailed(m_stagingResource->Unmap());
    }

}}}}
