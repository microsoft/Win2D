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

#include "CanvasBrush.h"
#include "ErrorHandling.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas
{
    using namespace ::Microsoft::WRL;

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

    inline D2D1_COLOR_F ToD2DColor(ABI::Windows::UI::Color const& color)
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

    inline RECT ToRECT(ABI::Windows::Foundation::Rect const& rect)
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
            ThrowHR(E_INVALIDARG);

        if (rect.Y < minMagnitude || rect.Y > maxMagnitude)
            ThrowHR(E_INVALIDARG);

        if (floatRight < minMagnitude || floatRight > maxMagnitude)
            ThrowHR(E_INVALIDARG);

        if (floatBottom < minMagnitude || floatBottom > maxMagnitude)
            ThrowHR(E_INVALIDARG);

        auto left   = static_cast<long>(rect.X);
        auto top    = static_cast<long>(rect.Y);
        auto right  = static_cast<long>(floatRight);
        auto bottom = static_cast<long>(floatBottom);

        return RECT{ left, top, right, bottom };
    }

    inline D2D1_POINT_2F ToD2DPoint(Numerics::Vector2 const& point)
    {
        return D2D1_POINT_2F{ point.X, point.Y };
    }

    inline D2D1_RECT_F ToD2DRect(ABI::Windows::Foundation::Rect const& rect)
    {
        auto left = rect.X;
        auto right = rect.X + rect.Width;
        auto top = rect.Y;
        auto bottom = rect.Y + rect.Height;

        return D2D1_RECT_F{ left, top, right, bottom };
    }

    inline ABI::Windows::Foundation::Rect FromD2DRect(D2D1_RECT_F const& d2dRect)
    {
        auto x = d2dRect.left;
        auto width = d2dRect.right - x;
        auto y = d2dRect.top;
        auto height = d2dRect.bottom - y;

        ABI::Windows::Foundation::Rect rect{ x, y, width, height };
        return rect;
    }

    inline D2D1_SIZE_U ToD2DSizeU(const ABI::Windows::Foundation::Size& size)
    {
        if (size.Width < 0) ThrowHR(E_INVALIDARG);
        if (size.Height < 0) ThrowHR(E_INVALIDARG);

        if (size.Width > static_cast<float>(UINT_MAX)) ThrowHR(E_INVALIDARG);
        if (size.Height > static_cast<float>(UINT_MAX)) ThrowHR(E_INVALIDARG);

        if (floorf(size.Width) != size.Width) ThrowHR(E_INVALIDARG);
        if (floorf(size.Height) != size.Height) ThrowHR(E_INVALIDARG);

        return D2D1_SIZE_U{ static_cast<UINT>(size.Width), static_cast<UINT>(size.Height) };
    }

    inline D2D1_ROUNDED_RECT ToD2DRoundedRect(ABI::Windows::Foundation::Rect const& rect, float rx, float ry)
    {
        return D2D1_ROUNDED_RECT{ ToD2DRect(rect), rx, ry };
    }

    inline D2D1_ELLIPSE ToD2DEllipse(Numerics::Vector2 const& point, float rx, float ry)
    {
        return D2D1::Ellipse(ToD2DPoint(point), rx, ry);
    }

    inline ComPtr<ID2D1Brush> ToD2DBrush(ICanvasBrush* brush)
    {
        if (!brush)
            return nullptr;

        ComPtr<ICanvasBrushInternal> internal;
        ThrowIfFailed(brush->QueryInterface(IID_PPV_ARGS(&internal)));
        return internal->GetD2DBrush();
    }

    template<typename TOutput, typename TInput> TOutput ReinterpretAs(TInput value)
    {
        ValidateReinterpretAs<TOutput, TInput>();

        return reinterpret_cast<TOutput>(value);
    }

    template<typename TOutput, typename TInput> void ValidateReinterpretAs()
    {
        static_assert(false, "Invalid ReinterpretAs type parameters");
    }

    template<> inline void ValidateReinterpretAs<D2D1_MATRIX_3X2_F*, Numerics::Matrix3x2*>()
    {
        static_assert(offsetof(D2D1_MATRIX_3X2_F, _11) == offsetof(Numerics::Matrix3x2, M11), "Matrix3x2 layout must match D2D1_MATRIX_3X2_F");
        static_assert(offsetof(D2D1_MATRIX_3X2_F, _12) == offsetof(Numerics::Matrix3x2, M12), "Matrix3x2 layout must match D2D1_MATRIX_3X2_F");
        static_assert(offsetof(D2D1_MATRIX_3X2_F, _21) == offsetof(Numerics::Matrix3x2, M21), "Matrix3x2 layout must match D2D1_MATRIX_3X2_F");
        static_assert(offsetof(D2D1_MATRIX_3X2_F, _22) == offsetof(Numerics::Matrix3x2, M22), "Matrix3x2 layout must match D2D1_MATRIX_3X2_F");
        static_assert(offsetof(D2D1_MATRIX_3X2_F, _31) == offsetof(Numerics::Matrix3x2, M31), "Matrix3x2 layout must match D2D1_MATRIX_3X2_F");
        static_assert(offsetof(D2D1_MATRIX_3X2_F, _32) == offsetof(Numerics::Matrix3x2, M32), "Matrix3x2 layout must match D2D1_MATRIX_3X2_F");
    }
}}}}
