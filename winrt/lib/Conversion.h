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

    template<typename TOutput, typename TInput> TOutput ReinterpretAs(TInput value)
    {
        static_assert(std::is_pointer<TInput>::value, "Types must be pointers");
        static_assert(std::is_pointer<TOutput>::value, "Types must be pointers");
        static_assert(sizeof(TInput) == sizeof(TOutput), "Types must be the same size");

        // Replace eg. "Foo const*" with "Foo*".
        typedef std::add_pointer<std::remove_const<std::remove_pointer<TOutput>::type>::type>::type TOutputWithoutConst;
        typedef std::add_pointer<std::remove_const<std::remove_pointer<TInput>::type>::type>::type TInputWithoutConst;

        ValidateReinterpretAs<TOutputWithoutConst, TInputWithoutConst>();

        return reinterpret_cast<TOutput>(value);
    }

    template<typename TOutput, typename TInput> TOutput StaticCastAs(TInput value)
    {
        ValidateStaticCastAs<TOutput, TInput>();

        return static_cast<TOutput>(value);
    }

    template<typename TOutput, typename TInput> void ValidateReinterpretAs()
    {
        static_assert(false, "Invalid ReinterpretAs type parameters");
    }

    template<typename TOutput, typename TInput> void ValidateStaticCastAs()
    {
        static_assert(false, "Invalid StaticCastAs type parameters");
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

    template<> inline void ValidateReinterpretAs<Numerics::Vector4*, D2D1_COLOR_F*>()
    {
        static_assert(offsetof(D2D1_COLOR_F, r) == offsetof(Numerics::Vector4, X), "Vector4 layout must match D2D1_COLOR_F");
        static_assert(offsetof(D2D1_COLOR_F, g) == offsetof(Numerics::Vector4, Y), "Vector4 layout must match D2D1_COLOR_F");
        static_assert(offsetof(D2D1_COLOR_F, b) == offsetof(Numerics::Vector4, Z), "Vector4 layout must match D2D1_COLOR_F");
        static_assert(offsetof(D2D1_COLOR_F, a) == offsetof(Numerics::Vector4, W), "Vector4 layout must match D2D1_COLOR_F");
    }

    template<> inline void ValidateReinterpretAs<D2D1_COLOR_F*, Numerics::Vector4*>()
    {
        ValidateReinterpretAs<Numerics::Vector4*, D2D1_COLOR_F*>();
    }

    template<> inline void ValidateReinterpretAs<Numerics::Vector4*, D2D1_RECT_F*>()
    {
        static_assert(offsetof(D2D1_RECT_F, left)   == offsetof(Numerics::Vector4, X), "Vector4 layout must match D2D1_RECT_F");
        static_assert(offsetof(D2D1_RECT_F, top)    == offsetof(Numerics::Vector4, Y), "Vector4 layout must match D2D1_RECT_F");
        static_assert(offsetof(D2D1_RECT_F, right)  == offsetof(Numerics::Vector4, Z), "Vector4 layout must match D2D1_RECT_F");
        static_assert(offsetof(D2D1_RECT_F, bottom) == offsetof(Numerics::Vector4, W), "Vector4 layout must match D2D1_RECT_F");
    }

    template<> inline void ValidateReinterpretAs<D2D1_RECT_F*, Numerics::Vector4*>()
    {
        ValidateReinterpretAs<Numerics::Vector4*, D2D1_RECT_F*>();
    }

    template<> inline void ValidateReinterpretAs<DXGI_SURFACE_DESC*, DirectX::Direct3D11::Direct3DSurfaceDescription*>()
    {
        using namespace DirectX::Direct3D11;
        static_assert(sizeof(DXGI_SURFACE_DESC) == sizeof(Direct3DSurfaceDescription), "Direct3DSurfaceDescription layout must match DXGI_SURFACE_DESC layout");
        static_assert(offsetof(DXGI_SURFACE_DESC, Width) == offsetof(Direct3DSurfaceDescription, Width), "Direct3DSurfaceDescription layout must match DXGI_SURFACE_DESC layout");
        static_assert(offsetof(DXGI_SURFACE_DESC, Height) == offsetof(Direct3DSurfaceDescription, Height), "Direct3DSurfaceDescription layout must match DXGI_SURFACE_DESC layout");
        static_assert(offsetof(DXGI_SURFACE_DESC, Format) == offsetof(Direct3DSurfaceDescription, Format), "Direct3DSurfaceDescription layout must match DXGI_SURFACE_DESC layout");
        static_assert(offsetof(DXGI_SURFACE_DESC, SampleDesc) == offsetof(Direct3DSurfaceDescription, MultisampleDescription), "Direct3DSurfaceDescription layout must match DXGI_SURFACE_DESC layout");
        static_assert(offsetof(DXGI_SAMPLE_DESC, Count) == offsetof(Direct3DMultisampleDescription, Count), "GraphicsMultisampleDescription layout must match DXGI_SAMPLE_DESC layout");
        static_assert(offsetof(DXGI_SAMPLE_DESC, Quality) == offsetof(Direct3DMultisampleDescription, Quality), "GraphicsMultisampleDescription layout must match DXGI_SAMPLE_DESC layout");
    }

    template<> inline void ValidateStaticCastAs<CanvasEdgeBehavior, D2D1_EXTEND_MODE>()
    {
        static_assert(static_cast<uint32_t>(D2D1_EXTEND_MODE_CLAMP) == static_cast<uint32_t>(CanvasEdgeBehavior::Clamp), "CanvasEdgeBehavior must match D2D1_EXTEND_MODE");
        static_assert(static_cast<uint32_t>(D2D1_EXTEND_MODE_WRAP) == static_cast<uint32_t>(CanvasEdgeBehavior::Wrap), "CanvasEdgeBehavior must match D2D1_EXTEND_MODE");
        static_assert(static_cast<uint32_t>(D2D1_EXTEND_MODE_MIRROR) == static_cast<uint32_t>(CanvasEdgeBehavior::Mirror), "CanvasEdgeBehavior must match D2D1_EXTEND_MODE");
    }

    template<> inline void ValidateStaticCastAs<CanvasColorSpace, D2D1_COLOR_SPACE>()
    {
        static_assert(static_cast<uint32_t>(D2D1_COLOR_SPACE_CUSTOM) == static_cast<uint32_t>(CanvasColorSpace::Custom), "CanvasColorSpace must match D2D1_COLOR_SPACE");
        static_assert(static_cast<uint32_t>(D2D1_COLOR_SPACE_SRGB) == static_cast<uint32_t>(CanvasColorSpace::Srgb), "CanvasColorSpace must match D2D1_COLOR_SPACE");
        static_assert(static_cast<uint32_t>(D2D1_COLOR_SPACE_SCRGB) == static_cast<uint32_t>(CanvasColorSpace::ScRgb), "CanvasColorSpace must match D2D1_COLOR_SPACE");
    }

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

    inline Numerics::Vector4 ToVector4(ABI::Windows::UI::Color const& color)
    {
        auto d2dColor = ToD2DColor(color);
        return *ReinterpretAs<Numerics::Vector4 const*>(&d2dColor);
    }

    inline Numerics::Vector3 ToVector3(ABI::Windows::UI::Color const& color)
    {
        auto d2dColor = ToD2DColor(color);
        return Numerics::Vector3{ d2dColor.r, d2dColor.g, d2dColor.b };
    }

    inline ABI::Windows::UI::Color ToWindowsColor(D2D1_COLOR_F const& color)
    {
        ABI::Windows::UI::Color windowsColor;
        windowsColor.A = SaturateAndNormalizeToUint8(color.a);
        windowsColor.R = SaturateAndNormalizeToUint8(color.r);
        windowsColor.G = SaturateAndNormalizeToUint8(color.g);
        windowsColor.B = SaturateAndNormalizeToUint8(color.b);
        return windowsColor;
    }

    inline ABI::Windows::UI::Color ToWindowsColor(Numerics::Vector4 const& vector)
    {
        return ToWindowsColor(*ReinterpretAs<D2D1_COLOR_F const*>(&vector));
    }

    inline ABI::Windows::UI::Color ToWindowsColor(Numerics::Vector3 const& vector)
    {
        return ToWindowsColor(D2D1_COLOR_F{ vector.X, vector.Y, vector.Z, 1 });
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

    inline Numerics::Vector2 FromD2DPoint(D2D1_POINT_2F const& point)
    {
        return Numerics::Vector2{ point.x, point.y };
    }

    inline D2D1_RECT_F ToD2DRect(ABI::Windows::Foundation::Rect const& rect)
    {
        auto left = rect.X;
        auto right = isinf(rect.Width) ? rect.Width : rect.X + rect.Width;
        auto top = rect.Y;
        auto bottom = isinf(rect.Height) ? rect.Height : rect.Y + rect.Height;

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

    inline D2D1_SIZE_U ToD2DSizeU(float width, float height)
    {
        if (width < 0) ThrowHR(E_INVALIDARG);
        if (height < 0) ThrowHR(E_INVALIDARG);

        if (width > static_cast<float>(UINT_MAX)) ThrowHR(E_INVALIDARG);
        if (height > static_cast<float>(UINT_MAX)) ThrowHR(E_INVALIDARG);

        if (floorf(width) != width) ThrowHR(E_INVALIDARG);
        if (floorf(height) != height) ThrowHR(E_INVALIDARG);

        return D2D1_SIZE_U{ static_cast<UINT>(width), static_cast<UINT>(height) };
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

    inline D2D1_COLOR_INTERPOLATION_MODE ToD2DColorInterpolation(CanvasAlphaBehavior alphaBehavior)
    {
        switch (alphaBehavior)
        {
            case CanvasAlphaBehavior::Premultiplied: return D2D1_COLOR_INTERPOLATION_MODE_PREMULTIPLIED;
            case CanvasAlphaBehavior::Straight: return D2D1_COLOR_INTERPOLATION_MODE_STRAIGHT;
            case CanvasAlphaBehavior::Ignore:
            default: return D2D1_COLOR_INTERPOLATION_MODE_FORCE_DWORD;
        }
    }

    inline D2D1_ALPHA_MODE ToD2DAlphaMode(CanvasAlphaBehavior alphaBehavior)
    {
        switch (alphaBehavior)
        {
        case CanvasAlphaBehavior::Premultiplied: return D2D1_ALPHA_MODE_PREMULTIPLIED;
        case CanvasAlphaBehavior::Straight: return D2D1_ALPHA_MODE_STRAIGHT;
        case CanvasAlphaBehavior::Ignore: return D2D1_ALPHA_MODE_IGNORE;
        default: return D2D1_ALPHA_MODE_FORCE_DWORD;
        }
    }

    inline CanvasAlphaBehavior FromD2DColorInterpolation(D2D1_COLOR_INTERPOLATION_MODE colorInterpolation)
    {
        switch (colorInterpolation)
        {
            case D2D1_COLOR_INTERPOLATION_MODE_PREMULTIPLIED: return CanvasAlphaBehavior::Premultiplied;
            case D2D1_COLOR_INTERPOLATION_MODE_STRAIGHT: return CanvasAlphaBehavior::Straight;
            default: assert(false); break;
        }
        return CanvasAlphaBehavior::Premultiplied;
    }

    inline D2D1_BUFFER_PRECISION ToD2DBufferPrecision(CanvasBufferPrecision bufferPrecision)
    {
        switch (bufferPrecision)
        {
            case CanvasBufferPrecision::Precision8UIntNormalized: return D2D1_BUFFER_PRECISION_8BPC_UNORM;
            case CanvasBufferPrecision::Precision8UIntNormalizedSrgb: return D2D1_BUFFER_PRECISION_8BPC_UNORM_SRGB;
            case CanvasBufferPrecision::Precision16UIntNormalized: return D2D1_BUFFER_PRECISION_16BPC_UNORM;
            case CanvasBufferPrecision::Precision16Float: return D2D1_BUFFER_PRECISION_16BPC_FLOAT;
            case CanvasBufferPrecision::Precision32Float: return D2D1_BUFFER_PRECISION_32BPC_FLOAT;
            default: return D2D1_BUFFER_PRECISION_FORCE_DWORD;
        }
    }

    inline CanvasBufferPrecision FromD2DBufferPrecision(D2D1_BUFFER_PRECISION bufferPrecision)
    {
        switch (bufferPrecision)
        {
            case D2D1_BUFFER_PRECISION_8BPC_UNORM: return CanvasBufferPrecision::Precision8UIntNormalized;
            case D2D1_BUFFER_PRECISION_8BPC_UNORM_SRGB: return CanvasBufferPrecision::Precision8UIntNormalizedSrgb;
            case D2D1_BUFFER_PRECISION_16BPC_UNORM: return CanvasBufferPrecision::Precision16UIntNormalized;
            case D2D1_BUFFER_PRECISION_16BPC_FLOAT: return CanvasBufferPrecision::Precision16Float;
            case D2D1_BUFFER_PRECISION_32BPC_FLOAT: return CanvasBufferPrecision::Precision32Float;
            default: assert(false); return CanvasBufferPrecision::Precision8UIntNormalized;
        }
    }
}}}}
