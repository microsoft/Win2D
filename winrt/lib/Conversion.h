// Copyright (c) Microsoft Corporation.  All rights reserved

#pragma once

#include "ErrorHandling.h"

namespace canvas
{
    inline float ToNormalizedFloat(uint8_t v)
    {
        return static_cast<float>(v) / 255.0f;
    }

    inline uint8_t NormalizedToUint8(float f)
    {
        return static_cast<uint8_t>(f * 255.0f);
    }

    inline D2D1_COLOR_F WindowsColorToD2DColor(const ABI::Windows::UI::Color& color)
    {
        return D2D1::ColorF(
            ToNormalizedFloat(color.R),
            ToNormalizedFloat(color.G),
            ToNormalizedFloat(color.B),
            ToNormalizedFloat(color.A));
    }

    inline RECT ToRECT(const ABI::Windows::Foundation::Rect& rect)
    {
        float floatRight = rect.X + rect.Width;
        float floatBottom = rect.Y + rect.Height;

        //
        // Outside this range the conversion becomes inprecise, so we don't
        // allow it.  This is because ToRECT is generally used to convert to a
        // RECT that is going to specify a region in pixels, and float precision
        // errors could have very surprising effects here.
        //
        // This causes us a problem because VirtualSurfaceImageSources may want
        // to specify regions with values up to 17000000 and we cannot reliably
        // convert Rect to RECT for all regions in the VSIS.
        //
        // Realistically this won't happen since nobody would create a VSIS that
        // was 1x17000000.  The next valid size would be 2x8500000, and this
        // falls within this range, and even that is excessively rectangular.
        //
        const float maxMagnitude = 16777216.0f;
        const float minMagnitude = -maxMagnitude;

        if (rect.X < minMagnitude || rect.X > maxMagnitude)
            throw InvalidArgException();

        if (rect.Y < minMagnitude || rect.Y > maxMagnitude)
            throw InvalidArgException();

        if (floatRight < minMagnitude || floatRight > maxMagnitude)
            throw InvalidArgException();

        if (floatBottom < minMagnitude || floatBottom > maxMagnitude)
            throw InvalidArgException();

        auto left   = static_cast<long>(rect.X);
        auto top    = static_cast<long>(rect.Y);
        auto right  = static_cast<long>(floatRight);
        auto bottom = static_cast<long>(floatBottom);

        return RECT{left,top,right,bottom}; 
    }
}
