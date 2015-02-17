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

#include "ClosablePtr.h"
#include "ErrorHandling.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas
{
    using namespace ABI::Microsoft::Graphics::Canvas::Numerics;
    using namespace ABI::Windows::Foundation;
    using namespace ::Microsoft::WRL;

    class ICanvasDrawingSessionAdapter
    {
    public:
        virtual ~ICanvasDrawingSessionAdapter() = default;
        virtual D2D1_POINT_2F GetRenderingSurfaceOffset() = 0;
        virtual void EndDraw() = 0;
    };

    class CanvasDrawingSessionManager;
    class CanvasDrawingSession;

    struct CanvasDrawingSessionTraits
    {
        typedef ID2D1DeviceContext1 resource_t;
        typedef CanvasDrawingSession wrapper_t;
        typedef ICanvasDrawingSession wrapper_interface_t;
        typedef CanvasDrawingSessionManager manager_t;
    };

    class CanvasDrawingSession : RESOURCE_WRAPPER_RUNTIME_CLASS(
        CanvasDrawingSessionTraits,
        ICanvasResourceCreatorWithDpi,
        ICanvasResourceCreator)
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_CanvasDrawingSession, BaseTrust);

        std::shared_ptr<ICanvasDrawingSessionAdapter> m_adapter;
        ComPtr<ID2D1SolidColorBrush> m_solidColorBrush;
        ComPtr<ICanvasTextFormat> m_defaultTextFormat;

        //
        // Contract:
        //     Drawing sessions created conventionally initialize this member.
        //     Drawing sessions created through interop set this member to null.
        //
        //     The thing this affects is DrawingSession's use as an ICanvasResourceCreator.
        //     If the backpointer is initialized, that is the resource creator's device.
        //     If the backpointer is null, a CanvasDevice wrapper is produced based on 
        //     this drawing session's device context. That wrapper is created on demand 
        //     by get_Device.
        //
        ComPtr<ICanvasDevice> m_owner;

    public:
        CanvasDrawingSession(
            std::shared_ptr<CanvasDrawingSessionManager> manager,
            ICanvasDevice* owner,
            ID2D1DeviceContext1* deviceContext,
            std::shared_ptr<ICanvasDrawingSessionAdapter> drawingSessionAdapter);

        virtual ~CanvasDrawingSession();

        // IClosable

        IFACEMETHOD(Close)() override;

        // ICanvasDrawingSession

        IFACEMETHOD(Clear)(
            ABI::Windows::UI::Color color) override;

        // 
        // DrawImage
        // 

        IFACEMETHOD(DrawImage)(
            ICanvasImage* image,
            Vector2 offset) override;

        IFACEMETHOD(DrawImageAtCoords)(
            ICanvasImage* image,
            float x,
            float y) override;

        IFACEMETHOD(DrawImageAtOrigin)(
            ICanvasImage* image) override;

        IFACEMETHOD(DrawImageWithSourceRect)(
            ICanvasImage* image,
            Vector2 offset,
            Rect sourceRect) override;

        IFACEMETHOD(DrawImageWithSourceRectAndInterpolation)(
            ICanvasImage* image,
            Vector2 offset,
            Rect sourceRect,
            CanvasImageInterpolation interpolation) override;

        IFACEMETHOD(DrawImageWithSourceRectAndInterpolationAndComposite)(
            ICanvasImage* image,
            Vector2 offset,
            Rect sourceRect,
            CanvasImageInterpolation interpolation,
            CanvasComposite composite) override;

        IFACEMETHOD(DrawImageAtCoordsWithSourceRect)(
            ICanvasImage* image,
            float x,
            float y,
            Rect sourceRect) override;

        IFACEMETHOD(DrawImageAtCoordsWithSourceRectAndInterpolation)(
            ICanvasImage* image,
            float x,
            float y,
            Rect sourceRect,
            CanvasImageInterpolation interpolation) override;

        IFACEMETHOD(DrawImageAtCoordsWithSourceRectAndInterpolationAndComposite)(
            ICanvasImage* image,
            float x,
            float y,
            Rect sourceRect,
            CanvasImageInterpolation interpolation,
            CanvasComposite composite) override;

        IFACEMETHOD(DrawBitmapWithDestRect)(
            ICanvasBitmap* bitmap,
            Rect destinationRect) override;

        IFACEMETHOD(DrawBitmapWithDestRectAndSourceRect)(
            ICanvasBitmap* bitmap,
            Rect destinationRect,
            Rect sourceRect) override;

        IFACEMETHOD(DrawBitmapWithDestRectAndSourceRectAndOpacity)(
            ICanvasBitmap* bitmap,
            Rect destinationRect,
            Rect sourceRect,
            float opacity) override;

        IFACEMETHOD(DrawBitmapWithDestRectAndSourceRectAndOpacityAndInterpolation)(
            ICanvasBitmap* bitmap,
            Rect destinationRect,
            Rect sourceRect,
            float opacity,
            CanvasImageInterpolation interpolation) override;

        IFACEMETHOD(DrawBitmapWithDestRectAndSourceRectAndOpacityAndInterpolationAndPerspective)(
            ICanvasBitmap* bitmap,
            Rect destinationRect,
            Rect sourceRect,
            float opacity,
            CanvasImageInterpolation interpolation,
            ABI::Microsoft::Graphics::Canvas::Numerics::Matrix4x4 perspective) override;

        //
        // DrawLine
        //
        
        // 0 additional parameters

        IFACEMETHOD(DrawLineWithBrush)(
            Vector2 point0,
            Vector2 point1,
            ICanvasBrush* brush) override;

        IFACEMETHOD(DrawLineAtCoordsWithBrush)(
            float x0,
            float y0,
            float x1,
            float y1,
            ICanvasBrush* brush) override;

        IFACEMETHOD(DrawLineWithColor)(
            Vector2 point0,
            Vector2 point1,
            ABI::Windows::UI::Color color) override;

        IFACEMETHOD(DrawLineAtCoordsWithColor)(
            float x0,
            float y0,
            float x1,
            float y1,
            ABI::Windows::UI::Color color) override;

        // 1 additional parameter (StrokeWidth)

        IFACEMETHOD(DrawLineWithBrushAndStrokeWidth)(
            Vector2 point0,
            Vector2 point1,
            ICanvasBrush* brush,
            float strokeWidth) override;

        IFACEMETHOD(DrawLineAtCoordsWithBrushAndStrokeWidth)(
            float x0,
            float y0,
            float x1,
            float y1,
            ICanvasBrush* brush,
            float strokeWidth) override;

        IFACEMETHOD(DrawLineWithColorAndStrokeWidth)(
            Vector2 point0,
            Vector2 point1,
            ABI::Windows::UI::Color color,
            float strokeWidth) override;

        IFACEMETHOD(DrawLineAtCoordsWithColorAndStrokeWidth)(
            float x0,
            float y0,
            float x1,
            float y1,
            ABI::Windows::UI::Color color,
            float strokeWidth) override;

        // 2 additional parameters (StrokeWidth, StrokeStyle)

        IFACEMETHOD(DrawLineWithBrushAndStrokeWidthAndStrokeStyle)(
            Vector2 point0,
            Vector2 point1,
            ICanvasBrush* brush,
            float strokeWidth,
            ICanvasStrokeStyle* strokeStyle) override;

        IFACEMETHOD(DrawLineAtCoordsWithBrushAndStrokeWidthAndStrokeStyle)(
            float x0,
            float y0,
            float x1,
            float y1,
            ICanvasBrush* brush,
            float strokeWidth,
            ICanvasStrokeStyle* strokeStyle) override;

        IFACEMETHOD(DrawLineWithColorAndStrokeWidthAndStrokeStyle)(
            Vector2 point0,
            Vector2 point1,
            ABI::Windows::UI::Color color,
            float strokeWidth,
            ICanvasStrokeStyle* strokeStyle) override;

        IFACEMETHOD(DrawLineAtCoordsWithColorAndStrokeWidthAndStrokeStyle)(
            float x0,
            float y0,
            float x1,
            float y1,
            ABI::Windows::UI::Color color,
            float strokeWidth,
            ICanvasStrokeStyle* strokeStyle) override;

        //
        // DrawRectangle
        //

        // 0 additional parameters

        IFACEMETHOD(DrawRectangleWithBrush)(
            ABI::Windows::Foundation::Rect rect,
            ICanvasBrush* brush) override;

        IFACEMETHOD(DrawRectangleAtCoordsWithBrush)(
            float x,
            float y,
            float w,
            float h,
            ICanvasBrush* brush) override;

        IFACEMETHOD(DrawRectangleWithColor)(
            ABI::Windows::Foundation::Rect rect,
            ABI::Windows::UI::Color color) override;

        IFACEMETHOD(DrawRectangleAtCoordsWithColor)(
            float x,
            float y,
            float w,
            float h,
            ABI::Windows::UI::Color color) override;

        // 1 additional parameter (StrokeWidth)

        IFACEMETHOD(DrawRectangleWithBrushAndStrokeWidth)(
            ABI::Windows::Foundation::Rect rect,
            ICanvasBrush* brush,
            float strokeWidth) override;

        IFACEMETHOD(DrawRectangleAtCoordsWithBrushAndStrokeWidth)(
            float x,
            float y,
            float w,
            float h,
            ICanvasBrush* brush,
            float strokeWidth) override;

        IFACEMETHOD(DrawRectangleWithColorAndStrokeWidth)(
            ABI::Windows::Foundation::Rect rect,
            ABI::Windows::UI::Color color,
            float strokeWidth) override;

        IFACEMETHOD(DrawRectangleAtCoordsWithColorAndStrokeWidth)(
            float x,
            float y,
            float w,
            float h,
            ABI::Windows::UI::Color color,
            float strokeWidth) override;

        // 2 additional parameters (StrokeWidth, StrokeStyle)

        IFACEMETHOD(DrawRectangleWithBrushAndStrokeWidthAndStrokeStyle)(
            ABI::Windows::Foundation::Rect rect,
            ICanvasBrush* brush,
            float strokeWidth,
            ICanvasStrokeStyle* strokeStyle) override;

        IFACEMETHOD(DrawRectangleAtCoordsWithBrushAndStrokeWidthAndStrokeStyle)(
            float x,
            float y,
            float w,
            float h,
            ICanvasBrush* brush,
            float strokeWidth,
            ICanvasStrokeStyle* strokeStyle) override;

        IFACEMETHOD(DrawRectangleWithColorAndStrokeWidthAndStrokeStyle)(
            ABI::Windows::Foundation::Rect rect,
            ABI::Windows::UI::Color color,
            float strokeWidth,
            ICanvasStrokeStyle* strokeStyle) override;

        IFACEMETHOD(DrawRectangleAtCoordsWithColorAndStrokeWidthAndStrokeStyle)(
            float x,
            float y,
            float w,
            float h,
            ABI::Windows::UI::Color color,
            float strokeWidth,
            ICanvasStrokeStyle* strokeStyle) override;

        //
        // FillRectangle
        //

        IFACEMETHOD(FillRectangleWithBrush)(
            ABI::Windows::Foundation::Rect rect,
            ICanvasBrush* brush) override;

        IFACEMETHOD(FillRectangleAtCoordsWithBrush)(
            float x,
            float y,
            float w,
            float h,
            ICanvasBrush* brush) override;

        IFACEMETHOD(FillRectangleWithColor)(
            ABI::Windows::Foundation::Rect rect,
            ABI::Windows::UI::Color color) override;

        IFACEMETHOD(FillRectangleAtCoordsWithColor)(
            float x,
            float y,
            float w,
            float h,
            ABI::Windows::UI::Color color) override;

        //
        // DrawRoundedRectangle
        //

        // 0 additional parameters

        IFACEMETHOD(DrawRoundedRectangleWithBrush)(
            ABI::Windows::Foundation::Rect rect,
            float radiusX,
            float radiusY,
            ICanvasBrush* brush) override;

        IFACEMETHOD(DrawRoundedRectangleAtCoordsWithBrush)(
            float x,
            float y,
            float w,
            float h,
            float radiusX,
            float radiusY,
            ICanvasBrush* brush) override;

        IFACEMETHOD(DrawRoundedRectangleWithColor)(
            ABI::Windows::Foundation::Rect rect,
            float radiusX,
            float radiusY,
            ABI::Windows::UI::Color color) override;

        IFACEMETHOD(DrawRoundedRectangleAtCoordsWithColor)(
            float x,
            float y,
            float w,
            float h,
            float radiusX,
            float radiusY,
            ABI::Windows::UI::Color color) override;

        // 1 additional parameter (StrokeWidth)

        IFACEMETHOD(DrawRoundedRectangleWithBrushAndStrokeWidth)(
            ABI::Windows::Foundation::Rect rect,
            float radiusX,
            float radiusY,
            ICanvasBrush* brush,
            float strokeWidth) override;

        IFACEMETHOD(DrawRoundedRectangleAtCoordsWithBrushAndStrokeWidth)(
            float x,
            float y,
            float w,
            float h,
            float radiusX,
            float radiusY,
            ICanvasBrush* brush,
            float strokeWidth) override;

        IFACEMETHOD(DrawRoundedRectangleWithColorAndStrokeWidth)(
            ABI::Windows::Foundation::Rect rect,
            float radiusX,
            float radiusY,
            ABI::Windows::UI::Color color,
            float strokeWidth) override;

        IFACEMETHOD(DrawRoundedRectangleAtCoordsWithColorAndStrokeWidth)(
            float x,
            float y,
            float w,
            float h,
            float radiusX,
            float radiusY,
            ABI::Windows::UI::Color color,
            float strokeWidth) override;

        // 2 additional parameters (StrokeWidth, StrokeStyle)

        IFACEMETHOD(DrawRoundedRectangleWithBrushAndStrokeWidthAndStrokeStyle)(
            ABI::Windows::Foundation::Rect rect,
            float radiusX,
            float radiusY,
            ICanvasBrush* brush,
            float strokeWidth,
            ICanvasStrokeStyle* strokeStyle) override;

        IFACEMETHOD(DrawRoundedRectangleAtCoordsWithBrushAndStrokeWidthAndStrokeStyle)(
            float x,
            float y,
            float w,
            float h,
            float radiusX,
            float radiusY,
            ICanvasBrush* brush,
            float strokeWidth,
            ICanvasStrokeStyle* strokeStyle) override;

        IFACEMETHOD(DrawRoundedRectangleWithColorAndStrokeWidthAndStrokeStyle)(
            ABI::Windows::Foundation::Rect rect,
            float radiusX,
            float radiusY,
            ABI::Windows::UI::Color color,
            float strokeWidth,
            ICanvasStrokeStyle* strokeStyle) override;

        IFACEMETHOD(DrawRoundedRectangleAtCoordsWithColorAndStrokeWidthAndStrokeStyle)(
            float x,
            float y,
            float w,
            float h,
            float radiusX,
            float radiusY,
            ABI::Windows::UI::Color color,
            float strokeWidth,
            ICanvasStrokeStyle* strokeStyle) override;

        //
        // FillRoundedRectangle
        //
        
        IFACEMETHOD(FillRoundedRectangleWithBrush)(
            ABI::Windows::Foundation::Rect rect,
            float radiusX,
            float radiusY,
            ICanvasBrush* brush) override;

        IFACEMETHOD(FillRoundedRectangleAtCoordsWithBrush)(
            float x,
            float y,
            float w,
            float h,
            float radiusX,
            float radiusY,
            ICanvasBrush* brush) override;

        IFACEMETHOD(FillRoundedRectangleWithColor)(
            ABI::Windows::Foundation::Rect rect,
            float radiusX,
            float radiusY,
            ABI::Windows::UI::Color color) override;

        IFACEMETHOD(FillRoundedRectangleAtCoordsWithColor)(
            float x,
            float y,
            float w,
            float h,
            float radiusX,
            float radiusY,
            ABI::Windows::UI::Color color) override;

        //
        // DrawEllipse
        //

        // 0 additional parameters
        
        IFACEMETHOD(DrawEllipseWithBrush)(
            Vector2 centerPoint,
            float radiusX,
            float radiusY,
            ICanvasBrush* brush) override;

        IFACEMETHOD(DrawEllipseAtCoordsWithBrush)(
            float x,
            float y,
            float radiusX,
            float radiusY,
            ICanvasBrush* brush) override;

        IFACEMETHOD(DrawEllipseWithColor)(
            Vector2 centerPoint,
            float radiusX,
            float radiusY,
            ABI::Windows::UI::Color color) override;

        IFACEMETHOD(DrawEllipseAtCoordsWithColor)(
            float x,
            float y,
            float radiusX,
            float radiusY,
            ABI::Windows::UI::Color color) override;

        // 1 additional parameter (StrokeWidth)

        IFACEMETHOD(DrawEllipseWithBrushAndStrokeWidth)(
            Vector2 centerPoint,
            float radiusX,
            float radiusY,
            ICanvasBrush* brush,
            float strokeWidth) override;

        IFACEMETHOD(DrawEllipseAtCoordsWithBrushAndStrokeWidth)(
            float x,
            float y,
            float radiusX,
            float radiusY,
            ICanvasBrush* brush,
            float strokeWidth) override;

        IFACEMETHOD(DrawEllipseWithColorAndStrokeWidth)(
            Vector2 centerPoint,
            float radiusX,
            float radiusY,
            ABI::Windows::UI::Color color,
            float strokeWidth) override;

        IFACEMETHOD(DrawEllipseAtCoordsWithColorAndStrokeWidth)(
            float x,
            float y,
            float radiusX,
            float radiusY,
            ABI::Windows::UI::Color color,
            float strokeWidth) override;

        // 2 additional parameters (StrokeWidth, StrokeStyle)

        IFACEMETHOD(DrawEllipseWithBrushAndStrokeWidthAndStrokeStyle)(
            Vector2 centerPoint,
            float radiusX,
            float radiusY,
            ICanvasBrush* brush,
            float strokeWidth,
            ICanvasStrokeStyle* strokeStyle) override;

        IFACEMETHOD(DrawEllipseAtCoordsWithBrushAndStrokeWidthAndStrokeStyle)(
            float x,
            float y,
            float radiusX,
            float radiusY,
            ICanvasBrush* brush,
            float strokeWidth,
            ICanvasStrokeStyle* strokeStyle) override;

        IFACEMETHOD(DrawEllipseWithColorAndStrokeWidthAndStrokeStyle)(
            Vector2 centerPoint,
            float radiusX,
            float radiusY,
            ABI::Windows::UI::Color color,
            float strokeWidth,
            ICanvasStrokeStyle* strokeStyle) override;

        IFACEMETHOD(DrawEllipseAtCoordsWithColorAndStrokeWidthAndStrokeStyle)(
            float x,
            float y,
            float radiusX,
            float radiusY,
            ABI::Windows::UI::Color color,
            float strokeWidth,
            ICanvasStrokeStyle* strokeStyle) override;

        //
        // FillEllipse
        //

        IFACEMETHOD(FillEllipseWithBrush)(
            Vector2 centerPoint,
            float radiusX,
            float radiusY,
            ICanvasBrush* brush) override;

        IFACEMETHOD(FillEllipseAtCoordsWithBrush)(
            float x,
            float y,
            float radiusX,
            float radiusY,
            ICanvasBrush* brush) override;

        IFACEMETHOD(FillEllipseWithColor)(
            Vector2 centerPoint,
            float radiusX,
            float radiusY,
            ABI::Windows::UI::Color color) override;

        IFACEMETHOD(FillEllipseAtCoordsWithColor)(
            float x,
            float y,
            float radiusX,
            float radiusY,
            ABI::Windows::UI::Color color) override;

        //
        // DrawCircle
        //

        // 0 additional parameters
        
        IFACEMETHOD(DrawCircleWithBrush)(
            Vector2 centerPoint,
            float radius,
            ICanvasBrush* brush) override;

        IFACEMETHOD(DrawCircleAtCoordsWithBrush)(
            float x,
            float y,
            float radius,
            ICanvasBrush* brush) override;

        IFACEMETHOD(DrawCircleWithColor)(
            Vector2 centerPoint,
            float radius,
            ABI::Windows::UI::Color color) override;

        IFACEMETHOD(DrawCircleAtCoordsWithColor)(
            float x,
            float y,
            float radius,
            ABI::Windows::UI::Color color) override;

        // 1 additional parameter (StrokeWidth)

        IFACEMETHOD(DrawCircleWithBrushAndStrokeWidth)(
            Vector2 centerPoint,
            float radius,
            ICanvasBrush* brush,
            float strokeWidth) override;

        IFACEMETHOD(DrawCircleAtCoordsWithBrushAndStrokeWidth)(
            float x,
            float y,
            float radius,
            ICanvasBrush* brush,
            float strokeWidth) override;

        IFACEMETHOD(DrawCircleWithColorAndStrokeWidth)(
            Vector2 centerPoint,
            float radius,
            ABI::Windows::UI::Color color,
            float strokeWidth) override;

        IFACEMETHOD(DrawCircleAtCoordsWithColorAndStrokeWidth)(
            float x,
            float y,
            float radius,
            ABI::Windows::UI::Color color,
            float strokeWidth) override;

        // 2 additional parameters (StrokeWidth, StrokeStyle)

        IFACEMETHOD(DrawCircleWithBrushAndStrokeWidthAndStrokeStyle)(
            Vector2 centerPoint,
            float radius,
            ICanvasBrush* brush,
            float strokeWidth,
            ICanvasStrokeStyle* strokeStyle) override;

        IFACEMETHOD(DrawCircleAtCoordsWithBrushAndStrokeWidthAndStrokeStyle)(
            float x,
            float y,
            float radius,
            ICanvasBrush* brush,
            float strokeWidth,
            ICanvasStrokeStyle* strokeStyle) override;

        IFACEMETHOD(DrawCircleWithColorAndStrokeWidthAndStrokeStyle)(
            Vector2 centerPoint,
            float radius,
            ABI::Windows::UI::Color color,
            float strokeWidth,
            ICanvasStrokeStyle* strokeStyle) override;

        IFACEMETHOD(DrawCircleAtCoordsWithColorAndStrokeWidthAndStrokeStyle)(
            float x,
            float y,
            float radius,
            ABI::Windows::UI::Color color,
            float strokeWidth,
            ICanvasStrokeStyle* strokeStyle) override;

        //
        // FillCircle
        //

        IFACEMETHOD(FillCircleWithBrush)(
            Vector2 centerPoint,
            float radius,
            ICanvasBrush* brush) override;

        IFACEMETHOD(FillCircleAtCoordsWithBrush)(
            float x,
            float y,
            float radius,
            ICanvasBrush* brush) override;

        IFACEMETHOD(FillCircleWithColor)(
            Vector2 centerPoint,
            float radius,
            ABI::Windows::UI::Color color) override;

        IFACEMETHOD(FillCircleAtCoordsWithColor)(
            float x,
            float y,
            float radius,
            ABI::Windows::UI::Color color) override;

        //
        // DrawText
        //

        // 0 additional parameters

        IFACEMETHOD(DrawTextAtPointWithColor)(
            HSTRING text,
            Vector2 point,
            ABI::Windows::UI::Color color) override;

        IFACEMETHOD(DrawTextAtPointCoordsWithColor)(
            HSTRING text,
            float x,
            float y,
            ABI::Windows::UI::Color color) override;

        // 1 additional parameter (TextFormat)

        IFACEMETHOD(DrawTextAtPointWithBrushAndFormat)(
            HSTRING text,
            Vector2 point,
            ICanvasBrush* brush,
            ICanvasTextFormat* format) override;

        IFACEMETHOD(DrawTextAtRectWithBrushAndFormat)(
            HSTRING text,
            ABI::Windows::Foundation::Rect rectangle,
            ICanvasBrush* brush,
            ICanvasTextFormat* format) override;

        IFACEMETHOD(DrawTextAtPointCoordsWithBrushAndFormat)(
            HSTRING text,
            float x,
            float y,
            ICanvasBrush* brush,
            ICanvasTextFormat* format) override;

        IFACEMETHOD(DrawTextAtRectCoordsWithBrushAndFormat)(
            HSTRING text,
            float x,
            float y,
            float w,
            float h,
            ICanvasBrush* brush,
            ICanvasTextFormat* format) override;

        IFACEMETHOD(DrawTextAtPointWithColorAndFormat)(
            HSTRING text,
            Vector2 point,
            ABI::Windows::UI::Color color,
            ICanvasTextFormat* format) override;

        IFACEMETHOD(DrawTextAtRectWithColorAndFormat)(
            HSTRING text,
            ABI::Windows::Foundation::Rect rectangle,
            ABI::Windows::UI::Color color,
            ICanvasTextFormat* format) override;

        IFACEMETHOD(DrawTextAtPointCoordsWithColorAndFormat)(
            HSTRING text,
            float x,
            float y,
            ABI::Windows::UI::Color color,
            ICanvasTextFormat* format) override;

        IFACEMETHOD(DrawTextAtRectCoordsWithColorAndFormat)(
            HSTRING text,
            float x,
            float y,
            float w,
            float h,
            ABI::Windows::UI::Color color,
            ICanvasTextFormat* format) override;
        
        //
        // DrawGeometry
        //

        // 0 additional parameters
        
        IFACEMETHOD(DrawGeometryWithBrush)(
            ICanvasGeometry* geometry,
            ICanvasBrush* brush) override;

        IFACEMETHOD(DrawGeometryWithColor)(
            ICanvasGeometry* geometry,
            ABI::Windows::UI::Color color) override;

        // 1 additional parameter (StrokeWidth)

        IFACEMETHOD(DrawGeometryWithBrushAndStrokeWidth)(
            ICanvasGeometry* geometry,
            ICanvasBrush* brush,
            float strokeWidth) override;

        IFACEMETHOD(DrawGeometryWithColorAndStrokeWidth)(
            ICanvasGeometry* geometry,
            ABI::Windows::UI::Color color,
            float strokeWidth) override;

        // 2 additional parameters (StrokeWidth, StrokeStyle)

        IFACEMETHOD(DrawGeometryWithBrushAndStrokeWidthAndStrokeStyle)(
            ICanvasGeometry* geometry,
            ICanvasBrush* brush,
            float strokeWidth,
            ICanvasStrokeStyle* strokeStyle) override;


        IFACEMETHOD(DrawGeometryWithColorAndStrokeWidthAndStrokeStyle)(
            ICanvasGeometry* geometry,
            ABI::Windows::UI::Color color,
            float strokeWidth,
            ICanvasStrokeStyle* strokeStyle) override;

        //
        // FillGeometry
        //

        IFACEMETHOD(FillGeometryWithBrush)(
            ICanvasGeometry* geometry,
            ICanvasBrush* brush) override;

        IFACEMETHOD(FillGeometryWithColor)(
            ICanvasGeometry* geometry,
            ABI::Windows::UI::Color color) override;

        //
        // State properties
        //

        IFACEMETHOD(get_Antialiasing)(CanvasAntialiasing* value);
        IFACEMETHOD(put_Antialiasing)(CanvasAntialiasing value);

        IFACEMETHOD(get_Blend)(CanvasBlend* value);
        IFACEMETHOD(put_Blend)(CanvasBlend value);

        IFACEMETHOD(get_TextAntialiasing)(CanvasTextAntialiasing* value);
        IFACEMETHOD(put_TextAntialiasing)(CanvasTextAntialiasing value);

        IFACEMETHOD(get_Transform)(ABI::Microsoft::Graphics::Canvas::Numerics::Matrix3x2* value);
        IFACEMETHOD(put_Transform)(ABI::Microsoft::Graphics::Canvas::Numerics::Matrix3x2 value);

        IFACEMETHOD(get_Units)(CanvasUnits* value);
        IFACEMETHOD(put_Units)(CanvasUnits value);

        //
        // ICanvasResourceCreator
        //

        IFACEMETHODIMP get_Device(ICanvasDevice** value);

        //
        // ICanvasResourceCreatorWithDpi
        //

        IFACEMETHODIMP get_Dpi(float* dpi);

        IFACEMETHODIMP ConvertPixelsToDips(int pixels, float* dips);
        IFACEMETHODIMP ConvertDipsToPixels(float dips, int* pixels);

    private:
        void DrawLineImpl(
            Vector2 const& p0,
            Vector2 const& p1,
            ID2D1Brush* brush,
            float strokeWidth,
            ICanvasStrokeStyle* strokeStyle);

        void DrawRectangleImpl(
            Rect const& rect,
            ID2D1Brush* brush,
            float strokeWidth,
            ICanvasStrokeStyle* strokeStyle);

        void FillRectangleImpl(
            Rect const& rect,
            ID2D1Brush* brush);

        void DrawRoundedRectangleImpl(
            Rect const& rect,
            float radiusX,
            float radiusY,
            ID2D1Brush* brush,
            float strokeWidth,
            ICanvasStrokeStyle* strokeStyle);

        void FillRoundedRectangleImpl(
            Rect const& rect,
            float radiusX,
            float radiusY,
            ID2D1Brush* brush);

        void DrawEllipseImpl(
            Vector2 const& centerPoint,
            float radiusX,
            float radiusY,
            ID2D1Brush* brush,
            float strokeWidth,
            ICanvasStrokeStyle* strokeStyle);

        void FillEllipseImpl(
            Vector2 const& centerPoint,
            float radiusX,
            float radiusY,
            ID2D1Brush* brush);

        void DrawTextAtRectImpl(
            HSTRING text,
            Rect const& rect,
            ID2D1Brush* brush,
            ICanvasTextFormat* format);

        void DrawTextAtPointImpl(
            HSTRING text,
            Vector2 const& point,
            ID2D1Brush* brush,
            ICanvasTextFormat* format);

        ICanvasTextFormat* GetDefaultTextFormat();

        ID2D1SolidColorBrush* GetColorBrush(ABI::Windows::UI::Color const& color);
        ComPtr<ID2D1Brush> ToD2DBrush(ICanvasBrush* brush);

        HRESULT DrawImageImpl(
            ICanvasImage* image,
            Vector2 offset,
            Rect* sourceRect,
            CanvasImageInterpolation interpolation,
            CanvasComposite const* composite);

        HRESULT DrawBitmapWithDestRectAndSourceRectImpl(
            ICanvasBitmap* bitmap,
            Rect destinationRect,
            Rect* sourceRect,
            float opacity,
            CanvasImageInterpolation interpolation,
            ABI::Microsoft::Graphics::Canvas::Numerics::Matrix4x4* perspective);
    };


    class CanvasDrawingSessionManager : public ResourceManager<CanvasDrawingSessionTraits>
    {
        std::shared_ptr<ICanvasDrawingSessionAdapter> m_adapter;

    public:
        CanvasDrawingSessionManager();

        ComPtr<CanvasDrawingSession> CreateNew(
            ICanvasDevice* owner,
            ID2D1DeviceContext1* deviceContext,
            std::shared_ptr<ICanvasDrawingSessionAdapter> drawingSessionAdapter);

        ComPtr<CanvasDrawingSession> CreateNew(
            ID2D1DeviceContext1* deviceContext,
            std::shared_ptr<ICanvasDrawingSessionAdapter> drawingSessionAdapter);

        ComPtr<CanvasDrawingSession> CreateWrapper(
            ID2D1DeviceContext1* resource);
    };


    class CanvasDrawingSessionFactory
        : public ActivationFactory<ICanvasDrawingSessionStatics, CloakedIid<ICanvasFactoryNative>>,
          public PerApplicationManager<CanvasDrawingSessionFactory, CanvasDrawingSessionManager>
    {
        InspectableClassStatic(RuntimeClass_Microsoft_Graphics_Canvas_CanvasDrawingSession, BaseTrust);

    public:
        IMPLEMENT_DEFAULT_ICANVASFACTORYNATIVE();
    };


    //
    // A CanvasDrawingSessionAdapter that calls BeginDraw and EndDraw on the
    // device context.
    //
    class SimpleCanvasDrawingSessionAdapter : public ICanvasDrawingSessionAdapter,
                                              private LifespanTracker<SimpleCanvasDrawingSessionAdapter>
    {
        ComPtr<ID2D1DeviceContext1> m_d2dDeviceContext;

    public:
        SimpleCanvasDrawingSessionAdapter(ID2D1DeviceContext1* d2dDeviceContext)
            : m_d2dDeviceContext(d2dDeviceContext) 
        {
            d2dDeviceContext->BeginDraw();
        }

        virtual D2D1_POINT_2F GetRenderingSurfaceOffset() override
        {
            return D2D1::Point2F(0, 0);
        }

        virtual void EndDraw() override
        {
            ThrowIfFailed(m_d2dDeviceContext->EndDraw());
        }
    };
}}}}
