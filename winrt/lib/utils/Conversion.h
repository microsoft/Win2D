// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas
{
    using namespace ::Microsoft::WRL;
    using namespace WinRTDirectX;
    using namespace Brushes;
#if WINVER > _WIN32_WINNT_WINBLUE
    using namespace Svg;
#endif
    using namespace Text;
    using namespace UI;
    using namespace UI::Xaml;
    using namespace Geometry;

    template<typename TOutput, typename TInput> TOutput ReinterpretAs(TInput value)
    {
        static_assert(std::is_pointer<TInput>::value, "Types must be pointers");
        static_assert(std::is_pointer<TOutput>::value, "Types must be pointers");

        static_assert(sizeof(TInput) == sizeof(TOutput), "Types must be the same size");

        // Replace eg. "Foo const*" with "Foo*".
        typedef std::add_pointer<std::remove_const<std::remove_pointer<TOutput>::type>::type>::type TOutputWithoutConst;
        typedef std::add_pointer<std::remove_const<std::remove_pointer<TInput> ::type>::type>::type TInputWithoutConst;

        static_assert(ValidateReinterpretAs<TOutputWithoutConst, TInputWithoutConst> ::value ||
                      ValidateReinterpretAs<TInputWithoutConst,  TOutputWithoutConst>::value, "Invalid ReinterpretAs type parameters");

        return reinterpret_cast<TOutput>(value);
    }

    template<typename TOutput, typename TInput> TOutput StaticCastAs(TInput value)
    {
        static_assert(ValidateStaticCastAs<TOutput, TInput> ::value ||
                      ValidateStaticCastAs<TInput,  TOutput>::value, "Invalid StaticCastAs type parameters");

        return static_cast<TOutput>(value);
    }

    template<typename TOutput, typename TInput>
    struct ValidateReinterpretAs : std::false_type
    {
    };

    template<typename TOutput, typename TInput>
    struct ValidateStaticCastAs : std::false_type
    {
    };

    template<> struct ValidateReinterpretAs<D2D1_MATRIX_3X2_F*, Numerics::Matrix3x2*> : std::true_type
    {
        static_assert(offsetof(D2D1_MATRIX_3X2_F, _11) == offsetof(Numerics::Matrix3x2, M11), "Matrix3x2 layout must match D2D1_MATRIX_3X2_F");
        static_assert(offsetof(D2D1_MATRIX_3X2_F, _12) == offsetof(Numerics::Matrix3x2, M12), "Matrix3x2 layout must match D2D1_MATRIX_3X2_F");
        static_assert(offsetof(D2D1_MATRIX_3X2_F, _21) == offsetof(Numerics::Matrix3x2, M21), "Matrix3x2 layout must match D2D1_MATRIX_3X2_F");
        static_assert(offsetof(D2D1_MATRIX_3X2_F, _22) == offsetof(Numerics::Matrix3x2, M22), "Matrix3x2 layout must match D2D1_MATRIX_3X2_F");
        static_assert(offsetof(D2D1_MATRIX_3X2_F, _31) == offsetof(Numerics::Matrix3x2, M31), "Matrix3x2 layout must match D2D1_MATRIX_3X2_F");
        static_assert(offsetof(D2D1_MATRIX_3X2_F, _32) == offsetof(Numerics::Matrix3x2, M32), "Matrix3x2 layout must match D2D1_MATRIX_3X2_F");
    };

    template<> struct ValidateReinterpretAs<DXGI_MATRIX_3X2_F*, Numerics::Matrix3x2*> : std::true_type
    {
        static_assert(offsetof(DXGI_MATRIX_3X2_F, _11) == offsetof(Numerics::Matrix3x2, M11), "Matrix3x2 layout must match DXGI_MATRIX_3X2_F");
        static_assert(offsetof(DXGI_MATRIX_3X2_F, _12) == offsetof(Numerics::Matrix3x2, M12), "Matrix3x2 layout must match DXGI_MATRIX_3X2_F");
        static_assert(offsetof(DXGI_MATRIX_3X2_F, _21) == offsetof(Numerics::Matrix3x2, M21), "Matrix3x2 layout must match DXGI_MATRIX_3X2_F");
        static_assert(offsetof(DXGI_MATRIX_3X2_F, _22) == offsetof(Numerics::Matrix3x2, M22), "Matrix3x2 layout must match DXGI_MATRIX_3X2_F");
        static_assert(offsetof(DXGI_MATRIX_3X2_F, _31) == offsetof(Numerics::Matrix3x2, M31), "Matrix3x2 layout must match DXGI_MATRIX_3X2_F");
        static_assert(offsetof(DXGI_MATRIX_3X2_F, _32) == offsetof(Numerics::Matrix3x2, M32), "Matrix3x2 layout must match DXGI_MATRIX_3X2_F");
    };

    template<> struct ValidateReinterpretAs<D2D1_MATRIX_4X4_F*, Numerics::Matrix4x4*> : std::true_type
    {
        static_assert(offsetof(D2D1_MATRIX_4X4_F, _11) == offsetof(Numerics::Matrix4x4, M11), "Matrix4x4 layout must match D2D1_MATRIX_4X4_F");
        static_assert(offsetof(D2D1_MATRIX_4X4_F, _12) == offsetof(Numerics::Matrix4x4, M12), "Matrix4x4 layout must match D2D1_MATRIX_4X4_F");
        static_assert(offsetof(D2D1_MATRIX_4X4_F, _13) == offsetof(Numerics::Matrix4x4, M13), "Matrix4x4 layout must match D2D1_MATRIX_4X4_F");
        static_assert(offsetof(D2D1_MATRIX_4X4_F, _14) == offsetof(Numerics::Matrix4x4, M14), "Matrix4x4 layout must match D2D1_MATRIX_4X4_F");
        static_assert(offsetof(D2D1_MATRIX_4X4_F, _21) == offsetof(Numerics::Matrix4x4, M21), "Matrix4x4 layout must match D2D1_MATRIX_4X4_F");
        static_assert(offsetof(D2D1_MATRIX_4X4_F, _22) == offsetof(Numerics::Matrix4x4, M22), "Matrix4x4 layout must match D2D1_MATRIX_4X4_F");
        static_assert(offsetof(D2D1_MATRIX_4X4_F, _23) == offsetof(Numerics::Matrix4x4, M23), "Matrix4x4 layout must match D2D1_MATRIX_4X4_F");
        static_assert(offsetof(D2D1_MATRIX_4X4_F, _24) == offsetof(Numerics::Matrix4x4, M24), "Matrix4x4 layout must match D2D1_MATRIX_4X4_F");
        static_assert(offsetof(D2D1_MATRIX_4X4_F, _31) == offsetof(Numerics::Matrix4x4, M31), "Matrix4x4 layout must match D2D1_MATRIX_4X4_F");
        static_assert(offsetof(D2D1_MATRIX_4X4_F, _32) == offsetof(Numerics::Matrix4x4, M32), "Matrix4x4 layout must match D2D1_MATRIX_4X4_F");
        static_assert(offsetof(D2D1_MATRIX_4X4_F, _33) == offsetof(Numerics::Matrix4x4, M33), "Matrix4x4 layout must match D2D1_MATRIX_4X4_F");
        static_assert(offsetof(D2D1_MATRIX_4X4_F, _34) == offsetof(Numerics::Matrix4x4, M34), "Matrix4x4 layout must match D2D1_MATRIX_4X4_F");
        static_assert(offsetof(D2D1_MATRIX_4X4_F, _41) == offsetof(Numerics::Matrix4x4, M41), "Matrix4x4 layout must match D2D1_MATRIX_4X4_F");
        static_assert(offsetof(D2D1_MATRIX_4X4_F, _42) == offsetof(Numerics::Matrix4x4, M42), "Matrix4x4 layout must match D2D1_MATRIX_4X4_F");
        static_assert(offsetof(D2D1_MATRIX_4X4_F, _43) == offsetof(Numerics::Matrix4x4, M43), "Matrix4x4 layout must match D2D1_MATRIX_4X4_F");
        static_assert(offsetof(D2D1_MATRIX_4X4_F, _44) == offsetof(Numerics::Matrix4x4, M44), "Matrix4x4 layout must match D2D1_MATRIX_4X4_F");
    };

    template<> struct ValidateReinterpretAs<DWRITE_MATRIX*, Numerics::Matrix3x2*> : std::true_type
    {
        static_assert(offsetof(DWRITE_MATRIX, m11) == offsetof(Numerics::Matrix3x2, M11), "Matrix3x2 layout must match DWRITE_MATRIX");
        static_assert(offsetof(DWRITE_MATRIX, m12) == offsetof(Numerics::Matrix3x2, M12), "Matrix3x2 layout must match DWRITE_MATRIX");
        static_assert(offsetof(DWRITE_MATRIX, m21) == offsetof(Numerics::Matrix3x2, M21), "Matrix3x2 layout must match DWRITE_MATRIX");
        static_assert(offsetof(DWRITE_MATRIX, m22) == offsetof(Numerics::Matrix3x2, M22), "Matrix3x2 layout must match DWRITE_MATRIX");
        static_assert(offsetof(DWRITE_MATRIX, dx) == offsetof(Numerics::Matrix3x2, M31), "Matrix3x2 layout must match DWRITE_MATRIX");
        static_assert(offsetof(DWRITE_MATRIX, dy) == offsetof(Numerics::Matrix3x2, M32), "Matrix3x2 layout must match DWRITE_MATRIX");
    };

    template<> struct ValidateReinterpretAs<DWRITE_MATRIX*, D2D1_MATRIX_3X2_F*> : std::true_type
    {
        static_assert(offsetof(DWRITE_MATRIX, m11) == offsetof(D2D1_MATRIX_3X2_F, _11), "D2D1_MATRIX_3X2_F layout must match DWRITE_MATRIX");
        static_assert(offsetof(DWRITE_MATRIX, m12) == offsetof(D2D1_MATRIX_3X2_F, _12), "D2D1_MATRIX_3X2_F layout must match DWRITE_MATRIX");
        static_assert(offsetof(DWRITE_MATRIX, m21) == offsetof(D2D1_MATRIX_3X2_F, _21), "D2D1_MATRIX_3X2_F layout must match DWRITE_MATRIX");
        static_assert(offsetof(DWRITE_MATRIX, m22) == offsetof(D2D1_MATRIX_3X2_F, _22), "D2D1_MATRIX_3X2_F layout must match DWRITE_MATRIX");
        static_assert(offsetof(DWRITE_MATRIX, dx) == offsetof(D2D1_MATRIX_3X2_F, _31), "D2D1_MATRIX_3X2_F layout must match DWRITE_MATRIX");
        static_assert(offsetof(DWRITE_MATRIX, dy) == offsetof(D2D1_MATRIX_3X2_F, _32), "D2D1_MATRIX_3X2_F layout must match DWRITE_MATRIX");
    };

    template<> struct ValidateReinterpretAs<Numerics::Vector4*, D2D1_COLOR_F*> : std::true_type
    {
        static_assert(offsetof(D2D1_COLOR_F, r) == offsetof(Numerics::Vector4, X), "Vector4 layout must match D2D1_COLOR_F");
        static_assert(offsetof(D2D1_COLOR_F, g) == offsetof(Numerics::Vector4, Y), "Vector4 layout must match D2D1_COLOR_F");
        static_assert(offsetof(D2D1_COLOR_F, b) == offsetof(Numerics::Vector4, Z), "Vector4 layout must match D2D1_COLOR_F");
        static_assert(offsetof(D2D1_COLOR_F, a) == offsetof(Numerics::Vector4, W), "Vector4 layout must match D2D1_COLOR_F");
    };

    template<> struct ValidateReinterpretAs<Numerics::Vector4*, D2D1_RECT_F*> : std::true_type
    {
        static_assert(offsetof(D2D1_RECT_F, left)   == offsetof(Numerics::Vector4, X), "Vector4 layout must match D2D1_RECT_F");
        static_assert(offsetof(D2D1_RECT_F, top)    == offsetof(Numerics::Vector4, Y), "Vector4 layout must match D2D1_RECT_F");
        static_assert(offsetof(D2D1_RECT_F, right)  == offsetof(Numerics::Vector4, Z), "Vector4 layout must match D2D1_RECT_F");
        static_assert(offsetof(D2D1_RECT_F, bottom) == offsetof(Numerics::Vector4, W), "Vector4 layout must match D2D1_RECT_F");
    };

    template<> struct ValidateReinterpretAs<DXGI_SURFACE_DESC*, Direct3DSurfaceDescription*> : std::true_type
    {
        static_assert(offsetof(DXGI_SURFACE_DESC, Width)      == offsetof(Direct3DSurfaceDescription,     Width),                  "Direct3DSurfaceDescription layout must match DXGI_SURFACE_DESC layout");
        static_assert(offsetof(DXGI_SURFACE_DESC, Height)     == offsetof(Direct3DSurfaceDescription,     Height),                 "Direct3DSurfaceDescription layout must match DXGI_SURFACE_DESC layout");
        static_assert(offsetof(DXGI_SURFACE_DESC, Format)     == offsetof(Direct3DSurfaceDescription,     Format),                 "Direct3DSurfaceDescription layout must match DXGI_SURFACE_DESC layout");
        static_assert(offsetof(DXGI_SURFACE_DESC, SampleDesc) == offsetof(Direct3DSurfaceDescription,     MultisampleDescription), "Direct3DSurfaceDescription layout must match DXGI_SURFACE_DESC layout");
        static_assert(offsetof(DXGI_SAMPLE_DESC,  Count)      == offsetof(Direct3DMultisampleDescription, Count),                  "GraphicsMultisampleDescription layout must match DXGI_SAMPLE_DESC layout");
        static_assert(offsetof(DXGI_SAMPLE_DESC,  Quality)    == offsetof(Direct3DMultisampleDescription, Quality),                "GraphicsMultisampleDescription layout must match DXGI_SAMPLE_DESC layout");
    };

    template<> struct ValidateReinterpretAs<D2D1_TRIANGLE*, CanvasTriangleVertices*> : std::true_type
    {
        static_assert(offsetof(D2D1_TRIANGLE, point1) == offsetof(CanvasTriangleVertices, Vertex1), "CanvasTriangleVertices layout must match D2D1_TRIANGLE");
        static_assert(offsetof(D2D1_TRIANGLE, point2) == offsetof(CanvasTriangleVertices, Vertex2), "CanvasTriangleVertices layout must match D2D1_TRIANGLE");
        static_assert(offsetof(D2D1_TRIANGLE, point3) == offsetof(CanvasTriangleVertices, Vertex3), "CanvasTriangleVertices layout must match D2D1_TRIANGLE");
    };

    template<> struct ValidateStaticCastAs<CanvasEdgeBehavior, D2D1_EXTEND_MODE> : std::true_type
    {
        static_assert(static_cast<uint32_t>(D2D1_EXTEND_MODE_CLAMP)  == static_cast<uint32_t>(CanvasEdgeBehavior::Clamp),  "CanvasEdgeBehavior must match D2D1_EXTEND_MODE");
        static_assert(static_cast<uint32_t>(D2D1_EXTEND_MODE_WRAP)   == static_cast<uint32_t>(CanvasEdgeBehavior::Wrap),   "CanvasEdgeBehavior must match D2D1_EXTEND_MODE");
        static_assert(static_cast<uint32_t>(D2D1_EXTEND_MODE_MIRROR) == static_cast<uint32_t>(CanvasEdgeBehavior::Mirror), "CanvasEdgeBehavior must match D2D1_EXTEND_MODE");
    };

    template<> struct ValidateStaticCastAs<CanvasColorSpace, D2D1_COLOR_SPACE> : std::true_type
    {
        static_assert(static_cast<uint32_t>(D2D1_COLOR_SPACE_CUSTOM) == static_cast<uint32_t>(CanvasColorSpace::Custom), "CanvasColorSpace must match D2D1_COLOR_SPACE");
        static_assert(static_cast<uint32_t>(D2D1_COLOR_SPACE_SRGB)   == static_cast<uint32_t>(CanvasColorSpace::Srgb),   "CanvasColorSpace must match D2D1_COLOR_SPACE");
        static_assert(static_cast<uint32_t>(D2D1_COLOR_SPACE_SCRGB)  == static_cast<uint32_t>(CanvasColorSpace::ScRgb),  "CanvasColorSpace must match D2D1_COLOR_SPACE");
    };

    template<> struct ValidateStaticCastAs<CanvasDrawTextOptions, D2D1_DRAW_TEXT_OPTIONS> : std::true_type
    {
        static_assert(static_cast<uint32_t>(D2D1_DRAW_TEXT_OPTIONS_NONE) == static_cast<uint32_t>(CanvasDrawTextOptions::Default), "CanvasDrawTextOptions must match D2D1_DRAW_TEXT_OPTIONS");
        static_assert(static_cast<uint32_t>(D2D1_DRAW_TEXT_OPTIONS_NO_SNAP) == static_cast<uint32_t>(CanvasDrawTextOptions::NoPixelSnap), "CanvasDrawTextOptions must match D2D1_DRAW_TEXT_OPTIONS");
        static_assert(static_cast<uint32_t>(D2D1_DRAW_TEXT_OPTIONS_CLIP) == static_cast<uint32_t>(CanvasDrawTextOptions::Clip), "CanvasDrawTextOptions must match D2D1_DRAW_TEXT_OPTIONS");

        // Using hardcoded constant instead of D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT, because that enum entry is missing from the VS2013 Windows Phone version of d2d1.h
        static_assert(static_cast<uint32_t>(4U) == static_cast<uint32_t>(CanvasDrawTextOptions::EnableColorFont), "CanvasDrawTextOptions must match D2D1_DRAW_TEXT_OPTIONS");
    };

    template<> struct ValidateStaticCastAs<CanvasFilledRegionDetermination, D2D1_FILL_MODE> : std::true_type
    {
        static_assert(static_cast<uint32_t>(D2D1_FILL_MODE_ALTERNATE) == static_cast<uint32_t>(CanvasFilledRegionDetermination::Alternate), "CanvasFilledRegionDetermination must match D2D1_FILL_MODE");
        static_assert(static_cast<uint32_t>(D2D1_FILL_MODE_WINDING) == static_cast<uint32_t>(CanvasFilledRegionDetermination::Winding), "CanvasFilledRegionDetermination must match D2D1_FILL_MODE");
    };


#if WINVER > _WIN32_WINNT_WINBLUE
    template<> struct ValidateStaticCastAs<CanvasSvgDisplay, D2D1_SVG_DISPLAY> : std::true_type
    {
        static_assert(static_cast<uint32_t>(D2D1_SVG_DISPLAY_INLINE) == static_cast<uint32_t>(CanvasSvgDisplay::Inline), "CanvasSvgDisplay must match D2D1_SVG_DISPLAY");
        static_assert(static_cast<uint32_t>(D2D1_SVG_DISPLAY_NONE) == static_cast<uint32_t>(CanvasSvgDisplay::None), "CanvasSvgDisplay must match D2D1_SVG_DISPLAY");
    };

    template<> struct ValidateStaticCastAs<CanvasSvgOverflow, D2D1_SVG_OVERFLOW> : std::true_type
    {
        static_assert(static_cast<uint32_t>(D2D1_SVG_OVERFLOW_VISIBLE) == static_cast<uint32_t>(CanvasSvgOverflow::DoNotClipToViewport), "CanvasSvgOverflow must match D2D1_SVG_OVERFLOW");
        static_assert(static_cast<uint32_t>(D2D1_SVG_OVERFLOW_HIDDEN) == static_cast<uint32_t>(CanvasSvgOverflow::ClipToViewport), "CanvasSvgOverflow must match D2D1_SVG_OVERFLOW");
    };

    template<> struct ValidateStaticCastAs<CanvasSvgVisibility, D2D1_SVG_VISIBILITY> : std::true_type
    {
        static_assert(static_cast<uint32_t>(D2D1_SVG_VISIBILITY_VISIBLE) == static_cast<uint32_t>(CanvasSvgVisibility::Visible), "CanvasSvgVisibility must match D2D1_SVG_VISIBILITY");
        static_assert(static_cast<uint32_t>(D2D1_SVG_VISIBILITY_HIDDEN) == static_cast<uint32_t>(CanvasSvgVisibility::Hidden), "CanvasSvgVisibility must match D2D1_SVG_VISIBILITY");
    };

    template<> struct ValidateStaticCastAs<CanvasSvgUnits, D2D1_SVG_UNIT_TYPE> : std::true_type
    {
        static_assert(static_cast<uint32_t>(D2D1_SVG_UNIT_TYPE_USER_SPACE_ON_USE) == static_cast<uint32_t>(CanvasSvgUnits::UserSpaceOnUse), "CanvasSvgVisibility must match D2D1_SVG_UNIT_TYPE");
        static_assert(static_cast<uint32_t>(D2D1_SVG_UNIT_TYPE_OBJECT_BOUNDING_BOX) == static_cast<uint32_t>(CanvasSvgUnits::ObjectBoundingBox), "CanvasSvgVisibility must match D2D1_SVG_UNIT_TYPE");
    };

    template<> struct ValidateStaticCastAs<CanvasSvgLengthUnits, D2D1_SVG_LENGTH_UNITS> : std::true_type
    {
        static_assert(static_cast<uint32_t>(D2D1_SVG_LENGTH_UNITS_NUMBER) == static_cast<uint32_t>(CanvasSvgLengthUnits::Number), "CanvasSvgLengthUnits must match D2D1_SVG_LENGTH");
        static_assert(static_cast<uint32_t>(D2D1_SVG_LENGTH_UNITS_PERCENTAGE) == static_cast<uint32_t>(CanvasSvgLengthUnits::Percentage), "CanvasSvgLengthUnits must match D2D1_SVG_LENGTH");
    };

    template<> struct ValidateStaticCastAs<CanvasSvgAspectAlignment, D2D1_SVG_ASPECT_ALIGN> : std::true_type
    {
        static_assert(static_cast<uint32_t>(D2D1_SVG_ASPECT_ALIGN_NONE) == static_cast<uint32_t>(CanvasSvgAspectAlignment::None), "CanvasSvgAspectAlignment must match D2D1_SVG_ASPECT_ALIGN");
        static_assert(static_cast<uint32_t>(D2D1_SVG_ASPECT_ALIGN_X_MIN_Y_MIN) == static_cast<uint32_t>(CanvasSvgAspectAlignment::XMinYMin), "CanvasSvgAspectAlignment must match D2D1_SVG_ASPECT_ALIGN");
        static_assert(static_cast<uint32_t>(D2D1_SVG_ASPECT_ALIGN_X_MID_Y_MIN) == static_cast<uint32_t>(CanvasSvgAspectAlignment::XMidYMin), "CanvasSvgAspectAlignment must match D2D1_SVG_ASPECT_ALIGN");
        static_assert(static_cast<uint32_t>(D2D1_SVG_ASPECT_ALIGN_X_MAX_Y_MIN) == static_cast<uint32_t>(CanvasSvgAspectAlignment::XMaxYMin), "CanvasSvgAspectAlignment must match D2D1_SVG_ASPECT_ALIGN");
        static_assert(static_cast<uint32_t>(D2D1_SVG_ASPECT_ALIGN_X_MIN_Y_MID) == static_cast<uint32_t>(CanvasSvgAspectAlignment::XMinYMid), "CanvasSvgAspectAlignment must match D2D1_SVG_ASPECT_ALIGN");
        static_assert(static_cast<uint32_t>(D2D1_SVG_ASPECT_ALIGN_X_MID_Y_MID) == static_cast<uint32_t>(CanvasSvgAspectAlignment::XMidYMid), "CanvasSvgAspectAlignment must match D2D1_SVG_ASPECT_ALIGN");
        static_assert(static_cast<uint32_t>(D2D1_SVG_ASPECT_ALIGN_X_MAX_Y_MID) == static_cast<uint32_t>(CanvasSvgAspectAlignment::XMaxYMid), "CanvasSvgAspectAlignment must match D2D1_SVG_ASPECT_ALIGN");
        static_assert(static_cast<uint32_t>(D2D1_SVG_ASPECT_ALIGN_X_MIN_Y_MAX) == static_cast<uint32_t>(CanvasSvgAspectAlignment::XMinYMax), "CanvasSvgAspectAlignment must match D2D1_SVG_ASPECT_ALIGN");
        static_assert(static_cast<uint32_t>(D2D1_SVG_ASPECT_ALIGN_X_MID_Y_MAX) == static_cast<uint32_t>(CanvasSvgAspectAlignment::XMidYMax), "CanvasSvgAspectAlignment must match D2D1_SVG_ASPECT_ALIGN");
        static_assert(static_cast<uint32_t>(D2D1_SVG_ASPECT_ALIGN_X_MAX_Y_MAX) == static_cast<uint32_t>(CanvasSvgAspectAlignment::XMaxYMax), "CanvasSvgAspectAlignment must match D2D1_SVG_ASPECT_ALIGN");
    };

    template<> struct ValidateStaticCastAs<CanvasSvgAspectScaling, D2D1_SVG_ASPECT_SCALING> : std::true_type
    {
        static_assert(static_cast<uint32_t>(D2D1_SVG_ASPECT_SCALING_MEET) == static_cast<uint32_t>(CanvasSvgAspectScaling::Meet), "CanvasSvgAspectScaling must match D2D1_SVG_ASPECT_SCALING");
        static_assert(static_cast<uint32_t>(D2D1_SVG_ASPECT_SCALING_SLICE) == static_cast<uint32_t>(CanvasSvgAspectScaling::Slice), "CanvasSvgAspectScaling must match D2D1_SVG_ASPECT_SCALING");
    };

    template<> struct ValidateStaticCastAs<CanvasSvgPaintType, D2D1_SVG_PAINT_TYPE> : std::true_type
    {
        static_assert(static_cast<uint32_t>(D2D1_SVG_PAINT_TYPE_NONE) == static_cast<uint32_t>(CanvasSvgPaintType::None), "CanvasSvgPaintType must match D2D1_SVG_PAINT_TYPE");
        static_assert(static_cast<uint32_t>(D2D1_SVG_PAINT_TYPE_COLOR) == static_cast<uint32_t>(CanvasSvgPaintType::Color), "CanvasSvgPaintType must match D2D1_SVG_PAINT_TYPE");
        static_assert(static_cast<uint32_t>(D2D1_SVG_PAINT_TYPE_CURRENT_COLOR) == static_cast<uint32_t>(CanvasSvgPaintType::CurrentColor), "CanvasSvgPaintType must match D2D1_SVG_PAINT_TYPE");
        static_assert(static_cast<uint32_t>(D2D1_SVG_PAINT_TYPE_URI) == static_cast<uint32_t>(CanvasSvgPaintType::Uri), "CanvasSvgPaintType must match D2D1_SVG_PAINT_TYPE");
        static_assert(static_cast<uint32_t>(D2D1_SVG_PAINT_TYPE_URI_NONE) == static_cast<uint32_t>(CanvasSvgPaintType::UriThenNone), "CanvasSvgPaintType must match D2D1_SVG_PAINT_TYPE");
        static_assert(static_cast<uint32_t>(D2D1_SVG_PAINT_TYPE_URI_COLOR) == static_cast<uint32_t>(CanvasSvgPaintType::UriThenColor), "CanvasSvgPaintType must match D2D1_SVG_PAINT_TYPE");
        static_assert(static_cast<uint32_t>(D2D1_SVG_PAINT_TYPE_URI_CURRENT_COLOR) == static_cast<uint32_t>(CanvasSvgPaintType::UriThenCurrentColor), "CanvasSvgPaintType must match D2D1_SVG_PAINT_TYPE");
    };

    template<> struct ValidateStaticCastAs<CanvasSvgPathCommand, D2D1_SVG_PATH_COMMAND> : std::true_type
    {
        static_assert(static_cast<uint32_t>(D2D1_SVG_PATH_COMMAND_CLOSE_PATH) == static_cast<uint32_t>(CanvasSvgPathCommand::ClosePath), "CanvasSvgPathCommand must match D2D1_SVG_PATH_COMMAND");
        static_assert(static_cast<uint32_t>(D2D1_SVG_PATH_COMMAND_MOVE_ABSOLUTE) == static_cast<uint32_t>(CanvasSvgPathCommand::MoveAbsolute), "CanvasSvgPathCommand must match D2D1_SVG_PATH_COMMAND");
        static_assert(static_cast<uint32_t>(D2D1_SVG_PATH_COMMAND_MOVE_RELATIVE) == static_cast<uint32_t>(CanvasSvgPathCommand::MoveRelative), "CanvasSvgPathCommand must match D2D1_SVG_PATH_COMMAND");
        static_assert(static_cast<uint32_t>(D2D1_SVG_PATH_COMMAND_LINE_ABSOLUTE) == static_cast<uint32_t>(CanvasSvgPathCommand::LineAbsolute), "CanvasSvgPathCommand must match D2D1_SVG_PATH_COMMAND");
        static_assert(static_cast<uint32_t>(D2D1_SVG_PATH_COMMAND_LINE_RELATIVE) == static_cast<uint32_t>(CanvasSvgPathCommand::LineRelative), "CanvasSvgPathCommand must match D2D1_SVG_PATH_COMMAND");
        static_assert(static_cast<uint32_t>(D2D1_SVG_PATH_COMMAND_CUBIC_ABSOLUTE) == static_cast<uint32_t>(CanvasSvgPathCommand::CubicAbsolute), "CanvasSvgPathCommand must match D2D1_SVG_PATH_COMMAND");
        static_assert(static_cast<uint32_t>(D2D1_SVG_PATH_COMMAND_CUBIC_RELATIVE) == static_cast<uint32_t>(CanvasSvgPathCommand::CubicRelative), "CanvasSvgPathCommand must match D2D1_SVG_PATH_COMMAND");
        static_assert(static_cast<uint32_t>(D2D1_SVG_PATH_COMMAND_QUADRADIC_ABSOLUTE) == static_cast<uint32_t>(CanvasSvgPathCommand::QuadraticAbsolute), "CanvasSvgPathCommand must match D2D1_SVG_PATH_COMMAND");
        static_assert(static_cast<uint32_t>(D2D1_SVG_PATH_COMMAND_QUADRADIC_RELATIVE) == static_cast<uint32_t>(CanvasSvgPathCommand::QuadraticRelative), "CanvasSvgPathCommand must match D2D1_SVG_PATH_COMMAND");
        static_assert(static_cast<uint32_t>(D2D1_SVG_PATH_COMMAND_ARC_ABSOLUTE) == static_cast<uint32_t>(CanvasSvgPathCommand::ArcAbsolute), "CanvasSvgPathCommand must match D2D1_SVG_PATH_COMMAND");
        static_assert(static_cast<uint32_t>(D2D1_SVG_PATH_COMMAND_ARC_RELATIVE) == static_cast<uint32_t>(CanvasSvgPathCommand::ArcRelative), "CanvasSvgPathCommand must match D2D1_SVG_PATH_COMMAND");
        static_assert(static_cast<uint32_t>(D2D1_SVG_PATH_COMMAND_HORIZONTAL_ABSOLUTE) == static_cast<uint32_t>(CanvasSvgPathCommand::HorizontalAbsolute), "CanvasSvgPathCommand must match D2D1_SVG_PATH_COMMAND");
        static_assert(static_cast<uint32_t>(D2D1_SVG_PATH_COMMAND_HORIZONTAL_RELATIVE) == static_cast<uint32_t>(CanvasSvgPathCommand::HorizontalRelative), "CanvasSvgPathCommand must match D2D1_SVG_PATH_COMMAND");
        static_assert(static_cast<uint32_t>(D2D1_SVG_PATH_COMMAND_VERTICAL_ABSOLUTE) == static_cast<uint32_t>(CanvasSvgPathCommand::VerticalAbsolute), "CanvasSvgPathCommand must match D2D1_SVG_PATH_COMMAND");
        static_assert(static_cast<uint32_t>(D2D1_SVG_PATH_COMMAND_VERTICAL_RELATIVE) == static_cast<uint32_t>(CanvasSvgPathCommand::VerticalRelative), "CanvasSvgPathCommand must match D2D1_SVG_PATH_COMMAND");
        static_assert(static_cast<uint32_t>(D2D1_SVG_PATH_COMMAND_CUBIC_SMOOTH_ABSOLUTE) == static_cast<uint32_t>(CanvasSvgPathCommand::CubicSmoothAbsolute), "CanvasSvgPathCommand must match D2D1_SVG_PATH_COMMAND");
        static_assert(static_cast<uint32_t>(D2D1_SVG_PATH_COMMAND_CUBIC_SMOOTH_RELATIVE) == static_cast<uint32_t>(CanvasSvgPathCommand::CubicSmoothRelative), "CanvasSvgPathCommand must match D2D1_SVG_PATH_COMMAND");
        static_assert(static_cast<uint32_t>(D2D1_SVG_PATH_COMMAND_QUADRADIC_SMOOTH_ABSOLUTE) == static_cast<uint32_t>(CanvasSvgPathCommand::QuadraticSmoothAbsolute), "CanvasSvgPathCommand must match D2D1_SVG_PATH_COMMAND");
        static_assert(static_cast<uint32_t>(D2D1_SVG_PATH_COMMAND_QUADRADIC_SMOOTH_RELATIVE) == static_cast<uint32_t>(CanvasSvgPathCommand::QuadraticSmoothRelative), "CanvasSvgPathCommand must match D2D1_SVG_PATH_COMMAND");
    };

    template<> struct ValidateReinterpretAs<D2D1_SVG_PATH_COMMAND*, CanvasSvgPathCommand*> : std::true_type
    {
        ValidateStaticCastAs<CanvasSvgPathCommand, D2D1_SVG_PATH_COMMAND> Validation;
        static_assert(sizeof(CanvasSvgPathCommand) == sizeof(D2D1_SVG_PATH_COMMAND), "size of CanvasSvgPathCommand must match D2D1_SVG_PATH_COMMAND");
    };

#endif

    template<> struct ValidateReinterpretAs<D2D1_POINT_2F*, Numerics::Vector2*> : std::true_type
    {
        static_assert(offsetof(D2D1_POINT_2F, x) == offsetof(Numerics::Vector2, X), "Vector2 layout must match D2D1_POINT_2F layout");
        static_assert(offsetof(D2D1_POINT_2F, y) == offsetof(Numerics::Vector2, Y), "Vector2 layout must match D2D1_POINT_2F layout");
        static_assert(sizeof(D2D1_POINT_2F) == sizeof(Numerics::Vector2), "size of D2D1_POINT_2F must match Vector2");
    };

    template<> struct ValidateReinterpretAs<DWRITE_UNICODE_RANGE*, CanvasUnicodeRange*> : std::true_type
    {
        static_assert(offsetof(DWRITE_UNICODE_RANGE, first) == offsetof(CanvasUnicodeRange, First), "CanvasUnicodeRange layout must match DWRITE_UNICODE_RANGE");
        static_assert(offsetof(DWRITE_UNICODE_RANGE, last) == offsetof(CanvasUnicodeRange, Last), "CanvasUnicodeRange layout must match DWRITE_UNICODE_RANGE");
    };

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

    inline D2D1_COLOR_F ToD2DColor(Numerics::Vector4 const& colorHdr)
    {
        return *ReinterpretAs<D2D1_COLOR_F const*>(&colorHdr);
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

    inline D2D1_RECT_U ToD2DRectU(int32_t left, int32_t top, int32_t width, int32_t height)
    {
        if (left < 0) ThrowHR(E_INVALIDARG);
        if (top < 0) ThrowHR(E_INVALIDARG);
        if (width < 0) ThrowHR(E_INVALIDARG);
        if (height < 0) ThrowHR(E_INVALIDARG);

        D2D1_RECT_U d2dRect;
        d2dRect.left = static_cast<UINT32>(left);
        d2dRect.top = static_cast<UINT32>(top);
        d2dRect.right = d2dRect.left + static_cast<UINT32>(width);
        d2dRect.bottom = d2dRect.top + static_cast<UINT32>(height);

        return d2dRect;
    }

    inline D2D1_POINT_2U ToD2DPointU(int32_t x, int32_t y)
    {
        if (x < 0) ThrowHR(E_INVALIDARG);
        if (y < 0) ThrowHR(E_INVALIDARG);

        return D2D1::Point2U(x, y);
    }

    inline D2D1_ROUNDED_RECT ToD2DRoundedRect(ABI::Windows::Foundation::Rect const& rect, float rx, float ry)
    {
        return D2D1_ROUNDED_RECT{ ToD2DRect(rect), rx, ry };
    }

    inline D2D1_ELLIPSE ToD2DEllipse(Numerics::Vector2 const& point, float rx, float ry)
    {
        return D2D1::Ellipse(ToD2DPoint(point), rx, ry);
    }

    inline D2D1_COLOR_INTERPOLATION_MODE ToD2DColorInterpolation(CanvasAlphaMode alphaMode)
    {
        switch (alphaMode)
        {
            case CanvasAlphaMode::Premultiplied: return D2D1_COLOR_INTERPOLATION_MODE_PREMULTIPLIED;
            case CanvasAlphaMode::Straight: return D2D1_COLOR_INTERPOLATION_MODE_STRAIGHT;
            case CanvasAlphaMode::Ignore:
            default: return D2D1_COLOR_INTERPOLATION_MODE_FORCE_DWORD;
        }
    }

    inline D2D1_ALPHA_MODE ConvertDxgiAlphaModeToD2DAlphaMode(DXGI_ALPHA_MODE alphaMode)
    {
        switch (alphaMode)
        {
            case DXGI_ALPHA_MODE_PREMULTIPLIED: return D2D1_ALPHA_MODE_PREMULTIPLIED; 
            case DXGI_ALPHA_MODE_STRAIGHT: return D2D1_ALPHA_MODE_STRAIGHT; 
            case DXGI_ALPHA_MODE_IGNORE: return D2D1_ALPHA_MODE_IGNORE; 
            default:
                assert(false); // Unexpected
                ThrowHR(E_UNEXPECTED);
        }
    }

    inline CanvasAlphaMode FromD2DAlphaMode(D2D1_ALPHA_MODE alphaMode)
    {
        switch (alphaMode)
        {
            case D2D1_ALPHA_MODE_PREMULTIPLIED: return CanvasAlphaMode::Premultiplied;
            case D2D1_ALPHA_MODE_STRAIGHT: return CanvasAlphaMode::Straight;
            case D2D1_ALPHA_MODE_IGNORE: return CanvasAlphaMode::Ignore;
            default: assert(false); return CanvasAlphaMode::Premultiplied;
        }
    }

    inline D2D1_ALPHA_MODE ToD2DAlphaMode(CanvasAlphaMode alphaMode)
    {
        switch (alphaMode)
        {
            case CanvasAlphaMode::Premultiplied: return D2D1_ALPHA_MODE_PREMULTIPLIED;
            case CanvasAlphaMode::Straight: return D2D1_ALPHA_MODE_STRAIGHT;
            case CanvasAlphaMode::Ignore: return D2D1_ALPHA_MODE_IGNORE;
            default: return D2D1_ALPHA_MODE_FORCE_DWORD;
        }
    }

#if WINVER > _WIN32_WINNT_WINBLUE

    inline CanvasAlphaMode ToCanvasAlphaMode(ABI::Windows::Graphics::Imaging::BitmapAlphaMode alphaMode)
    {
        using namespace ABI::Windows::Graphics::Imaging;
        
        switch (alphaMode)
        {
            case BitmapAlphaMode_Premultiplied: return CanvasAlphaMode::Premultiplied;
            case BitmapAlphaMode_Straight: return CanvasAlphaMode::Straight;
            case BitmapAlphaMode_Ignore: return CanvasAlphaMode::Ignore;
            default: assert(false); return CanvasAlphaMode::Premultiplied;
        }
    }

#endif
    
    inline CanvasAlphaMode FromD2DColorInterpolation(D2D1_COLOR_INTERPOLATION_MODE colorInterpolation)
    {
        switch (colorInterpolation)
        {
            case D2D1_COLOR_INTERPOLATION_MODE_PREMULTIPLIED: return CanvasAlphaMode::Premultiplied;
            case D2D1_COLOR_INTERPOLATION_MODE_STRAIGHT: return CanvasAlphaMode::Straight;
            default: assert(false); break;
        }
        return CanvasAlphaMode::Premultiplied;
    }

    inline CanvasAlphaMode FromDxgiAlphaMode(DXGI_ALPHA_MODE alphaMode)
    {
        switch (alphaMode)
        {
            case DXGI_ALPHA_MODE_PREMULTIPLIED: return CanvasAlphaMode::Premultiplied;
            case DXGI_ALPHA_MODE_STRAIGHT: return CanvasAlphaMode::Straight;
            case DXGI_ALPHA_MODE_IGNORE: return CanvasAlphaMode::Ignore;
            default: assert(false); return CanvasAlphaMode::Premultiplied;
        }
    }

    inline DXGI_ALPHA_MODE ToDxgiAlphaMode(CanvasAlphaMode alphaMode)
    {
        switch (alphaMode)
        {
            case CanvasAlphaMode::Premultiplied: return DXGI_ALPHA_MODE_PREMULTIPLIED;
            case CanvasAlphaMode::Straight: return DXGI_ALPHA_MODE_STRAIGHT;
            case CanvasAlphaMode::Ignore: return DXGI_ALPHA_MODE_IGNORE;
            default: assert(false); return DXGI_ALPHA_MODE_PREMULTIPLIED;
        }
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

    inline CanvasSwapChainRotation FromDxgiRotation(DXGI_MODE_ROTATION rotation)
    {
        switch (rotation)
        {
            case DXGI_MODE_ROTATION_UNSPECIFIED:
            case DXGI_MODE_ROTATION_IDENTITY: return CanvasSwapChainRotation::None;
            case DXGI_MODE_ROTATION_ROTATE90: return CanvasSwapChainRotation::Rotate90;
            case DXGI_MODE_ROTATION_ROTATE180: return CanvasSwapChainRotation::Rotate180;
            case DXGI_MODE_ROTATION_ROTATE270: return CanvasSwapChainRotation::Rotate270;
            default: assert(false); return CanvasSwapChainRotation::None;
        }
    }

    inline DXGI_MODE_ROTATION ToDxgiRotation(CanvasSwapChainRotation rotation)
    {
        switch (rotation)
        {
            case CanvasSwapChainRotation::None: return DXGI_MODE_ROTATION_IDENTITY;
            case CanvasSwapChainRotation::Rotate90: return DXGI_MODE_ROTATION_ROTATE90;
            case CanvasSwapChainRotation::Rotate180: return DXGI_MODE_ROTATION_ROTATE180;
            case CanvasSwapChainRotation::Rotate270: return DXGI_MODE_ROTATION_ROTATE270;
            default: assert(false); return DXGI_MODE_ROTATION_IDENTITY;
        }
    }

    inline D2D1_FILTER ToD2DFilter(CanvasImageInterpolation interpolation)
    {
        switch (interpolation)
        {
            case CanvasImageInterpolation::NearestNeighbor: return D2D1_FILTER_MIN_MAG_MIP_POINT;
            case CanvasImageInterpolation::Linear: return D2D1_FILTER_MIN_MAG_MIP_LINEAR;
            case CanvasImageInterpolation::Anisotropic: return D2D1_FILTER_ANISOTROPIC;
            default: return D2D1_FILTER_FORCE_DWORD;
        }
    }

    inline CanvasImageInterpolation FromD2DFilter(D2D1_FILTER filter)
    {
        switch (filter)
        {
            case D2D1_FILTER_MIN_MAG_MIP_POINT: return CanvasImageInterpolation::NearestNeighbor;
            case D2D1_FILTER_MIN_MAG_MIP_LINEAR: return CanvasImageInterpolation::Linear;
            case D2D1_FILTER_ANISOTROPIC: return CanvasImageInterpolation::Anisotropic;
            default: assert(false); return CanvasImageInterpolation::NearestNeighbor;
        }
    }

    inline DWRITE_TEXT_RANGE ToDWriteTextRange(int32_t position, int32_t characterCount)
    {
        ThrowIfNegative(position);
        ThrowIfNegative(characterCount);

        return DWRITE_TEXT_RANGE{ static_cast<uint32_t>(position), static_cast<uint32_t>(characterCount) };
    }

    inline float PixelsToDips(int pixels, float dpi)
    {
        return pixels * DEFAULT_DPI / dpi;
    }

    inline int DipsToPixels(
        float dips, 
        float dpi, 
        CanvasDpiRounding dpiRounding)
    {
        float scaled = dips * dpi / DEFAULT_DPI;
        switch (dpiRounding)
        {
            case CanvasDpiRounding::Floor: scaled = floorf(scaled); break;
            case CanvasDpiRounding::Round: scaled = roundf(scaled); break;
            case CanvasDpiRounding::Ceiling: scaled = ceilf(scaled); break;
            default: ThrowHR(E_INVALIDARG);
        }

        return (int)(scaled);
    }

    inline int SizeDipsToPixels(
        float dips,
        float dpi)
    {
        int result = DipsToPixels(dips, dpi, CanvasDpiRounding::Round);

        // Zero versus non-zero is pretty important for things like control sizes, so we want
        // to avoid ever rounding non-zero input sizes down to zero during conversion to pixels.
        // If the input value was small but positive, it's safer to round up to one instead.
        if (result == 0 && dips > 0)
        {
            return 1;
        }

        return result;
    }

    inline RECT ToRECT(ABI::Windows::Foundation::Rect const& rect, float dpi)
    {
        auto left = DipsToPixels(rect.X, dpi, CanvasDpiRounding::Round);
        auto top = DipsToPixels(rect.Y, dpi, CanvasDpiRounding::Round);
        auto right = DipsToPixels(rect.X + rect.Width, dpi, CanvasDpiRounding::Round);
        auto bottom = DipsToPixels(rect.Y + rect.Height, dpi, CanvasDpiRounding::Round);

        if (right == left && rect.Width > 0)
            right++;

        if (bottom == top && rect.Height > 0)
            bottom++;

        return RECT{ left, top, right, bottom };
    }

    inline ABI::Windows::Foundation::Rect ToRect(RECT const& rect, float dpi)
    {
        auto x = PixelsToDips(rect.left, dpi);
        auto y = PixelsToDips(rect.top, dpi);
        auto width = PixelsToDips(rect.right - rect.left, dpi);
        auto height = PixelsToDips(rect.bottom - rect.top, dpi);

        return ABI::Windows::Foundation::Rect{ x, y, width, height };
    }

#if WINVER > _WIN32_WINNT_WINBLUE
    inline D2D1_PATCH_EDGE_MODE ToD2DPatchEdgeMode(CanvasGradientMeshPatchEdge edge)
    {
        switch (edge)
        {
            case CanvasGradientMeshPatchEdge::Aliased: return D2D1_PATCH_EDGE_MODE_ALIASED;
            case CanvasGradientMeshPatchEdge::Antialiased: return D2D1_PATCH_EDGE_MODE_ANTIALIASED;
            case CanvasGradientMeshPatchEdge::AliasedAndInflated: return D2D1_PATCH_EDGE_MODE_ALIASED_INFLATED;
            default: assert(false); return D2D1_PATCH_EDGE_MODE_ALIASED;
        }
    }

    inline CanvasGradientMeshPatchEdge FromD2DPatchEdgeMode(D2D1_PATCH_EDGE_MODE edge)
    {
        switch (edge)
        {
            case D2D1_PATCH_EDGE_MODE_ALIASED: return CanvasGradientMeshPatchEdge::Aliased;
            case D2D1_PATCH_EDGE_MODE_ANTIALIASED: return CanvasGradientMeshPatchEdge::Antialiased;
            case D2D1_PATCH_EDGE_MODE_ALIASED_INFLATED: return CanvasGradientMeshPatchEdge::AliasedAndInflated;
            default: assert(false); return CanvasGradientMeshPatchEdge::Aliased;
        }
    }

#endif
    
    inline unsigned short CheckCastAsUShort(int i)
    {
        if (static_cast<unsigned>(i) > USHORT_MAX)
            ThrowHR(E_INVALIDARG);

        return static_cast<unsigned short>(i);
    }
}}}}
