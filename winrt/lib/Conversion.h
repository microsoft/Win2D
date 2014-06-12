// Copyright (c) Microsoft Corporation.  All rights reserved

#pragma once

#include "CanvasBrush.h"
#include "ErrorHandling.h"

namespace canvas
{
    using namespace Microsoft::WRL;
    using namespace ABI::Microsoft::Graphics::Canvas;

    inline float ToNormalizedFloat(uint8_t v)
    {
        return static_cast<float>(v) / 255.0f;
    }

    inline uint8_t NormalizedToUint8(float f)
    {
        return static_cast<uint8_t>(f * 255.0f);
    }

    inline float Saturate(float f) // Named after the clamping behavior of HLSL saturate().
    {
        f = std::min<float>(1, f);
        f = std::max<float>(0, f);
        return f;
    }

    inline uint8_t SaturateAndNormalizeToUint8(float f)
    {
        return NormalizedToUint8(Saturate(f));
    }

    inline D2D1_COLOR_F ToD2DColor(const ABI::Windows::UI::Color& color)
    {
        return D2D1::ColorF(
            ToNormalizedFloat(color.R),
            ToNormalizedFloat(color.G),
            ToNormalizedFloat(color.B),
            ToNormalizedFloat(color.A));
    }

    inline ABI::Windows::UI::Color ToWindowsColor(const D2D1_COLOR_F color)
    {
        ABI::Windows::UI::Color windowsColor;
        windowsColor.A = SaturateAndNormalizeToUint8(color.a);
        windowsColor.R = SaturateAndNormalizeToUint8(color.r);
        windowsColor.G = SaturateAndNormalizeToUint8(color.g);
        windowsColor.B = SaturateAndNormalizeToUint8(color.b);
        return windowsColor;
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

    inline D2D1_POINT_2F ToD2DPoint(const ABI::Windows::Foundation::Point& point)
    {
        return D2D1_POINT_2F{point.X, point.Y};
    }

    inline D2D1_RECT_F ToD2DRect(const ABI::Windows::Foundation::Rect& rect)
    {
        auto left = rect.X;
        auto right = rect.X + rect.Width;
        auto top = rect.Y;
        auto bottom = rect.Y + rect.Height;

        return D2D1_RECT_F{left, top, right, bottom};
    }

    inline D2D1_ROUNDED_RECT ToD2DRoundedRect(const CanvasRoundedRectangle& roundedRect)
    {
        auto rect = ToD2DRect(roundedRect.Rect);
        auto radiusX = roundedRect.RadiusX;
        auto radiusY = roundedRect.RadiusY;

        return D2D1_ROUNDED_RECT{rect, radiusX, radiusY};
    }

    inline const D2D1_ELLIPSE* ToD2DEllipse(const CanvasEllipse* ellipse)
    {
        static_assert(offsetof(D2D1_POINT_2F, x) == offsetof(ABI::Windows::Foundation::Point, X), "Point layout must match D2D1_POINT_2F layout");
        static_assert(offsetof(D2D1_POINT_2F, y) == offsetof(ABI::Windows::Foundation::Point, Y), "Point layout must match D2D1_POINT_2F layout");
        static_assert(offsetof(D2D1_ELLIPSE, point) == offsetof(CanvasEllipse, Point), "CanvasEllipse layout must match D2D1_ELLIPSE layout");
        static_assert(offsetof(D2D1_ELLIPSE, radiusX) == offsetof(CanvasEllipse, RadiusX), "CanvasEllipse layout must match D2D1_ELLIPSE layout");
        static_assert(offsetof(D2D1_ELLIPSE, radiusY) == offsetof(CanvasEllipse, RadiusY), "CanvasEllipse layout must match D2D1_ELLIPSE layout");

        return reinterpret_cast<const D2D1_ELLIPSE*>(ellipse);
    }

    inline ComPtr<ID2D1Brush> ToD2DBrush(ICanvasBrush* brush)
    {
        ComPtr<ICanvasBrushInternal> internal;
        ThrowIfFailed(brush->QueryInterface(IID_PPV_ARGS(&internal)));
        return internal->GetD2DBrush();
    }

    inline D2D1_MATRIX_3X2_F ToD2DMatrix3x2(const Math::Matrix3x2& matrix)
    {
        return D2D1::Matrix3x2F(
            matrix.M11, matrix.M12,
            matrix.M21, matrix.M22,
            matrix.M31, matrix.M32
            );
    }

    inline Math::Matrix3x2 ToMathMatrix3x2(const D2D1_MATRIX_3X2_F& matrix)
    {
        Math::Matrix3x2 mathMatrix = {
            matrix._11, matrix._12,
            matrix._21, matrix._22,
            matrix._31, matrix._32
        };
        return mathMatrix;
    }
}
