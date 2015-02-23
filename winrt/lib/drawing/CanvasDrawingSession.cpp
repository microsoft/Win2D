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

#include "pch.h"

#include "CanvasTextFormat.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas
{
    using namespace ABI::Windows::Foundation;
    using namespace ABI::Windows::UI;

    ComPtr<ID2D1StrokeStyle1> ToD2DStrokeStyle(ICanvasStrokeStyle* strokeStyle, ID2D1DeviceContext* deviceContext)
    {
        if (!strokeStyle) return nullptr;

        ComPtr<ID2D1Factory> d2dFactory;
        deviceContext->GetFactory(&d2dFactory);

        ComPtr<ICanvasStrokeStyleInternal> internal;
        ThrowIfFailed(strokeStyle->QueryInterface(internal.GetAddressOf()));

        return internal->GetRealizedD2DStrokeStyle(d2dFactory.Get());
    }
    

    //
    // This drawing session adapter is used when wrapping an existing
    // ID2D1DeviceContext.  In this wrapper, interop, case we don't want
    // CanvasDrawingSession to call any additional methods in the device context.
    //
    class NoopCanvasDrawingSessionAdapter : public ICanvasDrawingSessionAdapter,
                                            private LifespanTracker<NoopCanvasDrawingSessionAdapter>
    {
    public:

        virtual D2D1_POINT_2F GetRenderingSurfaceOffset() override
        {
            return D2D1::Point2F(0, 0);
        }

        virtual void EndDraw() override
        {
            // nothing
        }
    };


    CanvasDrawingSessionManager::CanvasDrawingSessionManager()
        : m_adapter(std::make_shared<NoopCanvasDrawingSessionAdapter>())
    {
    }

    ComPtr<CanvasDrawingSession> CanvasDrawingSessionManager::CreateNew(
        ID2D1DeviceContext1* deviceContext,
        std::shared_ptr<ICanvasDrawingSessionAdapter> drawingSessionAdapter)
    {
        return CreateNew(nullptr, deviceContext, drawingSessionAdapter);
    }

    ComPtr<CanvasDrawingSession> CanvasDrawingSessionManager::CreateNew(
        ICanvasDevice* owner,
        ID2D1DeviceContext1* deviceContext,
        std::shared_ptr<ICanvasDrawingSessionAdapter> drawingSessionAdapter)
    {
        return Make<CanvasDrawingSession>(
            shared_from_this(),
            owner,
            deviceContext,
            drawingSessionAdapter);
    }


    ComPtr<CanvasDrawingSession> CanvasDrawingSessionManager::CreateWrapper(
        ID2D1DeviceContext1* resource)
    {
        auto drawingSession = Make<CanvasDrawingSession>(
            shared_from_this(), 
            nullptr,
            resource, 
            m_adapter);
        CheckMakeResult(drawingSession);
        return drawingSession;
    }


    CanvasDrawingSession::CanvasDrawingSession(
        std::shared_ptr<CanvasDrawingSessionManager> manager,
        ICanvasDevice* owner,
        ID2D1DeviceContext1* deviceContext,
        std::shared_ptr<ICanvasDrawingSessionAdapter> adapter)
        : ResourceWrapper(manager, deviceContext)
        , m_owner(owner)
        , m_adapter(adapter)
    {
        CheckInPointer(adapter.get());
    }


    CanvasDrawingSession::~CanvasDrawingSession()
    {
        // Ignore any errors when closing during destruction
        (void)Close();
    }


    IFACEMETHODIMP CanvasDrawingSession::Close()
    {
        // Base class Close() called outside of ExceptionBoundary since this
        // already has its own boundary.
        HRESULT hr = ResourceWrapper::Close();
        if (FAILED(hr))
            return hr;

        return ExceptionBoundary(
            [&]
            {
                if (m_adapter)
                {
                    // Arrange it so that m_adapter will always get
                    // reset, even if EndDraw throws.
                    auto adapter = m_adapter;
                    m_adapter.reset();

                    adapter->EndDraw();
                }        
            });
    }


    IFACEMETHODIMP CanvasDrawingSession::Clear(
        Color color)
    {
        return ExceptionBoundary(
            [&]
            {
                auto& deviceContext = GetResource();

                auto d2dColor = ToD2DColor(color);
                deviceContext->Clear(&d2dColor);
            });
    }


    // 
    // DrawImage
    //  

    IFACEMETHODIMP CanvasDrawingSession::DrawImage(
        ICanvasImage* image,
        Vector2 offset)
    {
        return DrawImageImpl(image, offset, nullptr, CanvasImageInterpolation::Linear, nullptr);
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawImageAtCoords(
        ICanvasImage* image,
        float x,
        float y)
    {
        return DrawImageImpl(image, Vector2{ x, y }, nullptr, CanvasImageInterpolation::Linear, nullptr);
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawImageAtOrigin(
        ICanvasImage* image)
    {
        return DrawImageImpl(image, Vector2{ 0, 0 }, nullptr, CanvasImageInterpolation::Linear, nullptr);
    }

    IFACEMETHODIMP CanvasDrawingSession::DrawImageWithSourceRect(
        ICanvasImage* image,
        Vector2 offset,
        Rect sourceRect)
    {
        return DrawImageImpl(image, offset, &sourceRect, CanvasImageInterpolation::Linear, nullptr);
    }

    IFACEMETHODIMP CanvasDrawingSession::DrawImageWithSourceRectAndInterpolation(
        ICanvasImage* image,
        Vector2 offset,
        Rect sourceRect,
        CanvasImageInterpolation interpolation)
    {
        return DrawImageImpl(image, offset, &sourceRect, interpolation, nullptr);
    }

    IFACEMETHODIMP CanvasDrawingSession::DrawImageWithSourceRectAndInterpolationAndComposite(
        ICanvasImage* image,
        Vector2 offset,
        Rect sourceRect,
        CanvasImageInterpolation interpolation,
        CanvasComposite composite)
    {
        return DrawImageImpl(image, offset, &sourceRect, interpolation, &composite);
    }

    IFACEMETHODIMP CanvasDrawingSession::DrawImageAtCoordsWithSourceRect(
        ICanvasImage* image,
        float x,
        float y,
        Rect sourceRect)
    {
        return DrawImageImpl(image, Vector2{ x, y }, &sourceRect, CanvasImageInterpolation::Linear, nullptr);
    }

    IFACEMETHODIMP CanvasDrawingSession::DrawImageAtCoordsWithSourceRectAndInterpolation(
        ICanvasImage* image,
        float x,
        float y,
        Rect sourceRect,
        CanvasImageInterpolation interpolation)
    {
        return DrawImageImpl(image, Vector2{ x, y }, &sourceRect, interpolation, nullptr);
    }

    IFACEMETHODIMP CanvasDrawingSession::DrawImageAtCoordsWithSourceRectAndInterpolationAndComposite(
        ICanvasImage* image,
        float x,
        float y,
        Rect sourceRect,
        CanvasImageInterpolation interpolation,
        CanvasComposite composite)
    {
        return DrawImageImpl(image, Vector2{ x, y }, &sourceRect, interpolation, &composite);
    }

    IFACEMETHODIMP CanvasDrawingSession::DrawBitmapWithDestRect(
        ICanvasBitmap* bitmap,
        Rect destinationRect)
    {   
        return DrawBitmapWithDestRectAndSourceRectImpl(bitmap, destinationRect, nullptr, 1.0f, CanvasImageInterpolation::Linear, nullptr);
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawBitmapWithDestRectAndSourceRect(
        ICanvasBitmap* bitmap,
        Rect destinationRect,
        Rect sourceRect)
    {
        return DrawBitmapWithDestRectAndSourceRectImpl(bitmap, destinationRect, &sourceRect, 1.0f, CanvasImageInterpolation::Linear, nullptr);
    }

    IFACEMETHODIMP CanvasDrawingSession::DrawBitmapWithDestRectAndSourceRectAndOpacity(
        ICanvasBitmap* bitmap,
        Rect destinationRect,
        Rect sourceRect,
        float opacity)
    {
        return DrawBitmapWithDestRectAndSourceRectImpl(bitmap, destinationRect, &sourceRect, opacity, CanvasImageInterpolation::Linear, nullptr);
    }

    IFACEMETHODIMP CanvasDrawingSession::DrawBitmapWithDestRectAndSourceRectAndOpacityAndInterpolation(
        ICanvasBitmap* bitmap,
        Rect destinationRect,
        Rect sourceRect,
        float opacity,
        CanvasImageInterpolation interpolation)
    {
        return DrawBitmapWithDestRectAndSourceRectImpl(bitmap, destinationRect, &sourceRect, opacity, interpolation, nullptr);
    }

    IFACEMETHODIMP CanvasDrawingSession::DrawBitmapWithDestRectAndSourceRectAndOpacityAndInterpolationAndPerspective(
        ICanvasBitmap* bitmap,
        Rect destinationRect,
        Rect sourceRect,
        float opacity,
        CanvasImageInterpolation interpolation,
        ABI::Microsoft::Graphics::Canvas::Numerics::Matrix4x4 perspective)
    {
        return DrawBitmapWithDestRectAndSourceRectImpl(bitmap, destinationRect, &sourceRect, opacity, interpolation, &perspective);
    }

    // DrawBitmap uses the current primitive blend setting, but DrawImage takes an explicit
    // composite mode parameter. We can only substitute the former for the latter if these match.
    //
    // In some cases where they do not match, we could change the primitive blend, use DrawBitmap,
    // then change it back again. But that would be more intrusive, so this implementation plays
    // it safe and only optimizes the simple case where the modes match exactly.
    //
    // If the composite parameter is null, the caller did not explicitly specify a composite mode.
    // In that case we will use GetCompositeModeFromPrimitiveBlend, so any primitive blend that
    // has a matching composite mode is valid.
    static bool IsValidDrawBitmapCompositeMode(CanvasComposite const* composite, ID2D1DeviceContext1* deviceContext)
    {
        switch (deviceContext->GetPrimitiveBlend())
        {
        case D2D1_PRIMITIVE_BLEND_SOURCE_OVER:
            return !composite || *composite == CanvasComposite::SourceOver;

        case D2D1_PRIMITIVE_BLEND_COPY:
            return !composite || *composite == CanvasComposite::Copy;

        case D2D1_PRIMITIVE_BLEND_ADD:
            return !composite || *composite == CanvasComposite::Add;

        default:
            return false;
        }
    }

    // When using a DrawImage overload that does not take an explicit composite mode parameter,
    // we try to match the current device context primitive blend setting.
    static D2D1_COMPOSITE_MODE GetCompositeModeFromPrimitiveBlend(ID2D1DeviceContext1* deviceContext)
    {
        switch (deviceContext->GetPrimitiveBlend())
        {
        case D2D1_PRIMITIVE_BLEND_SOURCE_OVER:
            return D2D1_COMPOSITE_MODE_SOURCE_OVER;

        case D2D1_PRIMITIVE_BLEND_COPY:
            return D2D1_COMPOSITE_MODE_SOURCE_COPY;

        case D2D1_PRIMITIVE_BLEND_ADD:
            return D2D1_COMPOSITE_MODE_PLUS;

        case D2D1_PRIMITIVE_BLEND_MIN:
            ThrowHR(E_FAIL, HStringReference(Strings::DrawImageMinBlendNotSupported).Get());

        default:
            ThrowHR(E_UNEXPECTED);
        }
    }

    static bool IsValidDrawBitmapInterpolationMode(CanvasImageInterpolation interpolation)
    {
        return interpolation == CanvasImageInterpolation::Linear ||
               interpolation == CanvasImageInterpolation::NearestNeighbor;
    }

    HRESULT CanvasDrawingSession::DrawImageImpl(
        ICanvasImage* image,
        Vector2 offset,
        Rect* sourceRect,
        CanvasImageInterpolation interpolation,
        CanvasComposite const* composite)
    {
        return ExceptionBoundary(
            [&]
        {
            auto& deviceContext = GetResource();
            CheckInPointer(image);

            D2D1_RECT_F d2dSourceRect;
            if (sourceRect) d2dSourceRect = ToD2DRect(*sourceRect);

            // If this is a bitmap being drawn with sufficiently simple options, we can take the DrawBitmap fast path.
            auto internalBitmap = MaybeAs<ICanvasBitmapInternal>(image);
            
            if (internalBitmap &&
                IsValidDrawBitmapCompositeMode(composite, deviceContext.Get()) &&
                IsValidDrawBitmapInterpolationMode(interpolation))
            {
                auto& d2dBitmap = internalBitmap->GetD2DBitmap();

                // DrawImage infers output size from the source image, but DrawBitmap takes an explicit dest rect.
                // So to use DrawBitmap, we must duplicate the same size logic that DrawImage would normally apply for us.
                D2D1_SIZE_F destSize;
                
                if (sourceRect)
                {
                    // If there is an explicit source rectangle, that determines the destination size too.
                    destSize = D2D1_SIZE_F{ sourceRect->Width, sourceRect->Height };
                }
                else if (deviceContext->GetUnitMode() == D2D1_UNIT_MODE_DIPS)
                {
                    // Use the size of the bitmap, in dips.
                    destSize = d2dBitmap->GetSize();
                }
                else
                {
                    // Use the size of the bitmap, in pixels.
                    auto pixelSize = d2dBitmap->GetPixelSize();
                    destSize = D2D1_SIZE_F{ static_cast<float>(pixelSize.width), static_cast<float>(pixelSize.height) };
                }

                D2D1_RECT_F d2dDestRect
                {
                    offset.X,
                    offset.Y,
                    offset.X + destSize.width,
                    offset.Y + destSize.height
                };
                
                deviceContext->DrawBitmap(
                    d2dBitmap.Get(),
                    &d2dDestRect,
                    1,
                    static_cast<D2D1_INTERPOLATION_MODE>(interpolation),
                    sourceRect ? &d2dSourceRect : nullptr,
                    nullptr);
            }
            else
            {
                // If DrawBitmap cannot handle this request, we must use the DrawImage slow path.
                D2D1_POINT_2F d2dOffset = ToD2DPoint(offset);

                D2D1_COMPOSITE_MODE compositeMode = composite ? static_cast<D2D1_COMPOSITE_MODE>(*composite)
                                                              : GetCompositeModeFromPrimitiveBlend(deviceContext.Get());

                deviceContext->DrawImage(
                    As<ICanvasImageInternal>(image)->GetD2DImage(deviceContext.Get()).Get(),
                    &d2dOffset,
                    sourceRect ? &d2dSourceRect : nullptr,
                    static_cast<D2D1_INTERPOLATION_MODE>(interpolation),
                    compositeMode);
            }
        });

    }

    HRESULT CanvasDrawingSession::DrawBitmapWithDestRectAndSourceRectImpl(
        ICanvasBitmap* bitmap,
        Rect destinationRect,
        Rect* sourceRect,
        float opacity,
        CanvasImageInterpolation interpolation,
        Numerics::Matrix4x4* perspective)
    {
        return ExceptionBoundary(
            [&]
        {
            auto& deviceContext = GetResource();
            CheckInPointer(bitmap);

            ComPtr<ICanvasBitmapInternal> internal;
            ThrowIfFailed(bitmap->QueryInterface(IID_PPV_ARGS(&internal)));

            D2D1_RECT_F d2dDestRect = ToD2DRect(destinationRect);
            D2D1_RECT_F d2dSourceRect;
            if (sourceRect) d2dSourceRect = ToD2DRect(*sourceRect);

            deviceContext->DrawBitmap(
                internal->GetD2DBitmap().Get(),
                &d2dDestRect,
                opacity,
                static_cast<D2D1_INTERPOLATION_MODE>(interpolation),
                sourceRect ? &d2dSourceRect : nullptr,
                ReinterpretAs<D2D1_MATRIX_4X4_F*>(perspective));
        });
    }

    //
    // DrawLine
    //
    
    IFACEMETHODIMP CanvasDrawingSession::DrawLineWithBrush(
        Vector2 point0,
        Vector2 point1,
        ICanvasBrush* brush)
    {
        return DrawLineWithBrushAndStrokeWidthAndStrokeStyle(
            point0, 
            point1, 
            brush, 
            1.0f, 
            nullptr);
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawLineAtCoordsWithBrush(
        float x0,
        float y0,
        float x1,
        float y1,
        ICanvasBrush* brush)
    {
        return DrawLineWithBrushAndStrokeWidthAndStrokeStyle(
            Vector2{ x0, y0 }, 
            Vector2{ x1, y1 }, 
            brush, 
            1.0f, 
            nullptr);
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawLineWithColor(
        Vector2 point0,
        Vector2 point1,
        Color color)
    {
        return DrawLineWithColorAndStrokeWidthAndStrokeStyle(
            point0, 
            point1, 
            color, 
            1.0f, 
            nullptr);
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawLineAtCoordsWithColor(
        float x0,
        float y0,
        float x1,
        float y1,
        Color color)
    {
        return DrawLineWithColorAndStrokeWidthAndStrokeStyle(
            Vector2{ x0, y0 }, 
            Vector2{ x1, y1 }, 
            color, 
            1.0f, 
            nullptr);
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawLineWithBrushAndStrokeWidth(
        Vector2 point0,
        Vector2 point1,
        ICanvasBrush* brush,
        float strokeWidth)
    {
        return DrawLineWithBrushAndStrokeWidthAndStrokeStyle(
            point0, 
            point1, 
            brush, 
            strokeWidth, 
            nullptr);
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawLineAtCoordsWithBrushAndStrokeWidth(
        float x0,
        float y0,
        float x1,
        float y1,
        ICanvasBrush* brush,
        float strokeWidth)
    {
        return DrawLineWithBrushAndStrokeWidthAndStrokeStyle(
            Vector2{ x0, y0 }, 
            Vector2{ x1, y1 }, 
            brush, 
            strokeWidth, 
            nullptr);
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawLineWithColorAndStrokeWidth(
        Vector2 point0,
        Vector2 point1,
        Color color,
        float strokeWidth)
    {
        return DrawLineWithColorAndStrokeWidthAndStrokeStyle(
            point0, 
            point1, 
            color, 
            strokeWidth, 
            nullptr);
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawLineAtCoordsWithColorAndStrokeWidth(
        float x0,
        float y0,
        float x1,
        float y1,
        Color color,
        float strokeWidth)
    {
        return DrawLineWithColorAndStrokeWidthAndStrokeStyle(
            Vector2{ x0, y0 }, 
            Vector2{ x1, y1 }, 
            color, 
            strokeWidth, 
            nullptr);
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawLineWithBrushAndStrokeWidthAndStrokeStyle(
        Vector2 point0,
        Vector2 point1,
        ICanvasBrush* brush,
        float strokeWidth,
        ICanvasStrokeStyle* strokeStyle)
    {
        return ExceptionBoundary(
            [&]
            {
                DrawLineImpl(
                    point0,
                    point1,
                    ToD2DBrush(brush).Get(),
                    strokeWidth,
                    strokeStyle);
            });
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawLineAtCoordsWithBrushAndStrokeWidthAndStrokeStyle(
        float x0,
        float y0,
        float x1,
        float y1,
        ICanvasBrush* brush,
        float strokeWidth,
        ICanvasStrokeStyle* strokeStyle)
    {
        return DrawLineWithBrushAndStrokeWidthAndStrokeStyle(
            Vector2{ x0, y0 }, 
            Vector2{ x1, y1 }, 
            brush, 
            strokeWidth, 
            strokeStyle);
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawLineWithColorAndStrokeWidthAndStrokeStyle(
        Vector2 point0,
        Vector2 point1,
        Color color,
        float strokeWidth,
        ICanvasStrokeStyle* strokeStyle)
    {
        return ExceptionBoundary(
            [&]
            {
                DrawLineImpl(
                    point0,
                    point1,
                    GetColorBrush(color),
                    strokeWidth,
                    strokeStyle);
            });
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawLineAtCoordsWithColorAndStrokeWidthAndStrokeStyle(
        float x0,
        float y0,
        float x1,
        float y1,
        Color color,
        float strokeWidth,
        ICanvasStrokeStyle* strokeStyle)
    {
        return DrawLineWithColorAndStrokeWidthAndStrokeStyle(
            Vector2{ x0, y0 }, 
            Vector2{ x1, y1 }, 
            color, 
            strokeWidth, 
            strokeStyle);
    }


    void CanvasDrawingSession::DrawLineImpl(
        Vector2 const& point0,
        Vector2 const& point1,
        ID2D1Brush* brush,
        float strokeWidth,
        ICanvasStrokeStyle* strokeStyle)
    {
        auto& deviceContext = GetResource();
        CheckInPointer(brush);

        deviceContext->DrawLine(
            ToD2DPoint(point0),
            ToD2DPoint(point1),
            brush,
            strokeWidth,
            ToD2DStrokeStyle(strokeStyle, deviceContext.Get()).Get());
    }


    //
    // DrawRectangle
    //

    IFACEMETHODIMP CanvasDrawingSession::DrawRectangleWithBrush(
        Rect rect,
        ICanvasBrush* brush)
    {
        return DrawRectangleWithBrushAndStrokeWidthAndStrokeStyle(
            rect, 
            brush, 
            1.0f, 
            nullptr);
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawRectangleAtCoordsWithBrush(
        float x,
        float y,
        float w,
        float h,
        ICanvasBrush* brush)
    {
        return DrawRectangleWithBrushAndStrokeWidthAndStrokeStyle(
            Rect{ x, y, w, h }, 
            brush, 
            1.0f, 
            nullptr);
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawRectangleWithColor(
        Rect rect,
        Color color)
    {
        return DrawRectangleWithColorAndStrokeWidthAndStrokeStyle(
            rect, 
            color, 
            1.0f, 
            nullptr);
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawRectangleAtCoordsWithColor(
        float x,
        float y,
        float w,
        float h,
        Color color)
    {
        return DrawRectangleWithColorAndStrokeWidthAndStrokeStyle(
            Rect{ x, y, w, h }, 
            color, 
            1.0f, 
            nullptr);
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawRectangleWithBrushAndStrokeWidth(
        Rect rect,
        ICanvasBrush* brush,
        float strokeWidth)
    {
        return DrawRectangleWithBrushAndStrokeWidthAndStrokeStyle(
            rect, 
            brush, 
            strokeWidth, 
            nullptr);
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawRectangleAtCoordsWithBrushAndStrokeWidth(
        float x,
        float y,
        float w,
        float h,
        ICanvasBrush* brush,
        float strokeWidth)
    {
        return DrawRectangleWithBrushAndStrokeWidthAndStrokeStyle(
            Rect{ x, y, w, h }, 
            brush, 
            strokeWidth, 
            nullptr);
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawRectangleWithColorAndStrokeWidth(
        Rect rect,
        Color color,
        float strokeWidth)
    {
        return DrawRectangleWithColorAndStrokeWidthAndStrokeStyle(
            rect, 
            color, 
            strokeWidth, 
            nullptr);
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawRectangleAtCoordsWithColorAndStrokeWidth(
        float x,
        float y,
        float w,
        float h,
        Color color,
        float strokeWidth)
    {
        return DrawRectangleWithColorAndStrokeWidthAndStrokeStyle(
            Rect{ x, y, w, h }, 
            color, 
            strokeWidth, 
            nullptr);
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawRectangleWithBrushAndStrokeWidthAndStrokeStyle(
        Rect rect,
        ICanvasBrush* brush,
        float strokeWidth,
        ICanvasStrokeStyle* strokeStyle)
    {
        return ExceptionBoundary(
            [&]
            {
                DrawRectangleImpl(
                    rect,
                    ToD2DBrush(brush).Get(),
                    strokeWidth,
                    strokeStyle);
            });
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawRectangleAtCoordsWithBrushAndStrokeWidthAndStrokeStyle(
        float x,
        float y,
        float w,
        float h,
        ICanvasBrush* brush,
        float strokeWidth,
        ICanvasStrokeStyle* strokeStyle)
    {
        return DrawRectangleWithBrushAndStrokeWidthAndStrokeStyle(
            Rect{ x, y, w, h }, 
            brush, 
            strokeWidth, 
            strokeStyle);
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawRectangleWithColorAndStrokeWidthAndStrokeStyle(
        Rect rect,
        Color color,
        float strokeWidth,
        ICanvasStrokeStyle* strokeStyle)
    {
        return ExceptionBoundary(
            [&]
            {
                DrawRectangleImpl(
                    rect,
                    GetColorBrush(color),
                    strokeWidth,
                    strokeStyle);
            });
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawRectangleAtCoordsWithColorAndStrokeWidthAndStrokeStyle(
        float x,
        float y,
        float w,
        float h,
        Color color,
        float strokeWidth,
        ICanvasStrokeStyle* strokeStyle)
    {
        return DrawRectangleWithColorAndStrokeWidthAndStrokeStyle(
            Rect{ x, y, w, h }, 
            color, 
            strokeWidth, 
            strokeStyle);
    }


    void CanvasDrawingSession::DrawRectangleImpl(
        Rect const& rect,
        ID2D1Brush* brush,
        float strokeWidth,
        ICanvasStrokeStyle* strokeStyle)
    {
        auto& deviceContext = GetResource();
        CheckInPointer(brush);

        auto d2dRect = ToD2DRect(rect);

        deviceContext->DrawRectangle(
            &d2dRect,
            brush,
            strokeWidth,
            ToD2DStrokeStyle(strokeStyle, deviceContext.Get()).Get());
    }


    //
    // FillRectangle
    //

    IFACEMETHODIMP CanvasDrawingSession::FillRectangleWithBrush(
        Rect rect,
        ICanvasBrush* brush)
    {
        return ExceptionBoundary(
            [&]
            {
                FillRectangleImpl(
                    rect,
                    ToD2DBrush(brush).Get());
            });
    }


    IFACEMETHODIMP CanvasDrawingSession::FillRectangleAtCoordsWithBrush(
        float x,
        float y,
        float w,
        float h,
        ICanvasBrush* brush)
    {
        return FillRectangleWithBrush(
            Rect{ x, y, w, h }, 
            brush);
    }


    IFACEMETHODIMP CanvasDrawingSession::FillRectangleWithColor(
        Rect rect,
        Color color)
    {
        return ExceptionBoundary(
            [&]
            {
                FillRectangleImpl(
                    rect,
                    GetColorBrush(color));
            });
    }


    IFACEMETHODIMP CanvasDrawingSession::FillRectangleAtCoordsWithColor(
        float x,
        float y,
        float w,
        float h,
        Color color)
    {
        return FillRectangleWithColor(
            Rect{ x, y, w, h }, 
            color);
    }


    void CanvasDrawingSession::FillRectangleImpl(
        Rect const& rect,
        ID2D1Brush* brush)
    {
        auto& deviceContext = GetResource();
        CheckInPointer(brush);

        auto d2dRect = ToD2DRect(rect);

        deviceContext->FillRectangle(
            &d2dRect,
            brush);
    }


    //
    // DrawRoundedRectangle
    //

    IFACEMETHODIMP CanvasDrawingSession::DrawRoundedRectangleWithBrush(
        Rect rect,
        float radiusX,
        float radiusY,
        ICanvasBrush* brush)
    {
        return DrawRoundedRectangleWithBrushAndStrokeWidthAndStrokeStyle(
            rect, 
            radiusX, 
            radiusY, 
            brush, 
            1.0f, 
            nullptr);
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawRoundedRectangleAtCoordsWithBrush(
        float x,
        float y,
        float w,
        float h,
        float radiusX,
        float radiusY,
        ICanvasBrush* brush)
    {
        return DrawRoundedRectangleWithBrushAndStrokeWidthAndStrokeStyle(
            Rect{ x, y, w, h }, 
            radiusX, 
            radiusY, 
            brush, 
            1.0f, 
            nullptr);
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawRoundedRectangleWithColor(
        Rect rect,
        float radiusX,
        float radiusY,
        Color color)
    {
        return DrawRoundedRectangleWithColorAndStrokeWidthAndStrokeStyle(
            rect, 
            radiusX, 
            radiusY, 
            color, 
            1.0f, 
            nullptr);
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawRoundedRectangleAtCoordsWithColor(
        float x,
        float y,
        float w,
        float h,
        float radiusX,
        float radiusY,
        Color color)
    {
        return DrawRoundedRectangleWithColorAndStrokeWidthAndStrokeStyle(
            Rect{ x, y, w, h }, 
            radiusX, 
            radiusY, 
            color, 
            1.0f, 
            nullptr);
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawRoundedRectangleWithBrushAndStrokeWidth(
        Rect rect,
        float radiusX,
        float radiusY,
        ICanvasBrush* brush,
        float strokeWidth)
    {
        return DrawRoundedRectangleWithBrushAndStrokeWidthAndStrokeStyle(
            rect, 
            radiusX, 
            radiusY, 
            brush, 
            strokeWidth, 
            nullptr);
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawRoundedRectangleAtCoordsWithBrushAndStrokeWidth(
        float x,
        float y,
        float w,
        float h,
        float radiusX,
        float radiusY,
        ICanvasBrush* brush,
        float strokeWidth)
    {
        return DrawRoundedRectangleWithBrushAndStrokeWidthAndStrokeStyle(
            Rect{ x, y, w, h }, 
            radiusX, 
            radiusY, 
            brush, 
            strokeWidth, 
            nullptr);
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawRoundedRectangleWithColorAndStrokeWidth(
        Rect rect,
        float radiusX,
        float radiusY,
        Color color,
        float strokeWidth)
    {
        return DrawRoundedRectangleWithColorAndStrokeWidthAndStrokeStyle(
            rect, 
            radiusX, 
            radiusY, 
            color, 
            strokeWidth, 
            nullptr);
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawRoundedRectangleAtCoordsWithColorAndStrokeWidth(
        float x,
        float y,
        float w,
        float h,
        float radiusX,
        float radiusY,
        Color color,
        float strokeWidth)
    {
        return DrawRoundedRectangleWithColorAndStrokeWidthAndStrokeStyle(
            Rect{ x, y, w, h }, 
            radiusX, 
            radiusY, 
            color, 
            strokeWidth, 
            nullptr);
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawRoundedRectangleWithBrushAndStrokeWidthAndStrokeStyle(
        Rect rect,
        float radiusX,
        float radiusY,
        ICanvasBrush* brush,
        float strokeWidth,
        ICanvasStrokeStyle* strokeStyle)
    {
        return ExceptionBoundary(
            [&]
            {
                DrawRoundedRectangleImpl(
                    rect,
                    radiusX,
                    radiusY,
                    ToD2DBrush(brush).Get(),
                    strokeWidth,
                    strokeStyle);
            });
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawRoundedRectangleAtCoordsWithBrushAndStrokeWidthAndStrokeStyle(
        float x,
        float y,
        float w,
        float h,
        float radiusX,
        float radiusY,
        ICanvasBrush* brush,
        float strokeWidth,
        ICanvasStrokeStyle* strokeStyle)
    {
        return DrawRoundedRectangleWithBrushAndStrokeWidthAndStrokeStyle(
            Rect{ x, y, w, h }, 
            radiusX, 
            radiusY, 
            brush, 
            strokeWidth, 
            strokeStyle);
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawRoundedRectangleWithColorAndStrokeWidthAndStrokeStyle(
        Rect rect,
        float radiusX,
        float radiusY,
        Color color,
        float strokeWidth,
        ICanvasStrokeStyle* strokeStyle)
    {
        return ExceptionBoundary(
            [&]
            {
                DrawRoundedRectangleImpl(
                    rect, 
                    radiusX, 
                    radiusY,
                    GetColorBrush(color),
                    strokeWidth,
                    strokeStyle);
            });
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawRoundedRectangleAtCoordsWithColorAndStrokeWidthAndStrokeStyle(
        float x,
        float y,
        float w,
        float h,
        float radiusX,
        float radiusY,
        Color color,
        float strokeWidth,
        ICanvasStrokeStyle* strokeStyle)
    {
        return DrawRoundedRectangleWithColorAndStrokeWidthAndStrokeStyle(
            Rect{ x, y, w, h }, 
            radiusX, 
            radiusY, 
            color, 
            strokeWidth, 
            strokeStyle);
    }


    void CanvasDrawingSession::DrawRoundedRectangleImpl(
        Rect const& rect,
        float radiusX,
        float radiusY,
        ID2D1Brush* brush,
        float strokeWidth,
        ICanvasStrokeStyle* strokeStyle)
    {
        auto& deviceContext = GetResource();
        CheckInPointer(brush);

        auto d2dRoundedRect = ToD2DRoundedRect(rect, radiusX, radiusY);

        deviceContext->DrawRoundedRectangle(
            &d2dRoundedRect,
            brush,
            strokeWidth,
            ToD2DStrokeStyle(strokeStyle, deviceContext.Get()).Get());
    }


    //
    // FillRoundedRectangle
    //
    
    IFACEMETHODIMP CanvasDrawingSession::FillRoundedRectangleWithBrush(
        Rect rect,
        float radiusX,
        float radiusY,
        ICanvasBrush* brush)
    {
        return ExceptionBoundary(
            [&]
            {
                FillRoundedRectangleImpl(
                    rect,
                    radiusX,
                    radiusY,
                    ToD2DBrush(brush).Get());
            });
    }


    IFACEMETHODIMP CanvasDrawingSession::FillRoundedRectangleAtCoordsWithBrush(
        float x,
        float y,
        float w,
        float h,
        float radiusX,
        float radiusY,
        ICanvasBrush* brush)
    {
        return FillRoundedRectangleWithBrush(
            Rect{ x, y, w, h }, 
            radiusX, 
            radiusY, 
            brush);
    }


    IFACEMETHODIMP CanvasDrawingSession::FillRoundedRectangleWithColor(
        Rect rect,
        float radiusX,
        float radiusY,
        Color color)
    {
        return ExceptionBoundary(
            [&]
            {
                FillRoundedRectangleImpl(
                    rect, 
                    radiusX, 
                    radiusY,
                    GetColorBrush(color));
            });
    }


    IFACEMETHODIMP CanvasDrawingSession::FillRoundedRectangleAtCoordsWithColor(
        float x,
        float y,
        float w,
        float h,
        float radiusX,
        float radiusY,
        Color color)
    {
        return FillRoundedRectangleWithColor(
            Rect{ x, y, w, h }, 
            radiusX, 
            radiusY, 
            color);
    }


    void CanvasDrawingSession::FillRoundedRectangleImpl(
        Rect const& rect,
        float radiusX,
        float radiusY,
        ID2D1Brush* brush)
    {
        auto& deviceContext = GetResource();
        CheckInPointer(brush);

        auto d2dRoundedRect = ToD2DRoundedRect(rect, radiusX, radiusY);

        deviceContext->FillRoundedRectangle(
            &d2dRoundedRect,
            brush);
    }


    //
    // DrawEllipse
    //

    IFACEMETHODIMP CanvasDrawingSession::DrawEllipseWithBrush(
        Vector2 centerPoint,
        float radiusX,
        float radiusY,
        ICanvasBrush* brush)
    {
        return DrawEllipseWithBrushAndStrokeWidthAndStrokeStyle(
            centerPoint, 
            radiusX, 
            radiusY, 
            brush, 
            1.0f, 
            nullptr);
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawEllipseAtCoordsWithBrush(
        float x,
        float y,
        float radiusX,
        float radiusY,
        ICanvasBrush* brush)
    {
        return DrawEllipseWithBrushAndStrokeWidthAndStrokeStyle(
            Vector2{ x, y }, 
            radiusX, 
            radiusY, 
            brush, 
            1.0f, 
            nullptr);
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawEllipseWithColor(
        Vector2 centerPoint,
        float radiusX,
        float radiusY,
        Color color)
    {
        return DrawEllipseWithColorAndStrokeWidthAndStrokeStyle(
            centerPoint, 
            radiusX, 
            radiusY, 
            color, 
            1.0f, 
            nullptr);
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawEllipseAtCoordsWithColor(
        float x,
        float y,
        float radiusX,
        float radiusY,
        Color color)
    {
        return DrawEllipseWithColorAndStrokeWidthAndStrokeStyle(
            Vector2{ x, y }, 
            radiusX, 
            radiusY, 
            color, 
            1.0f, 
            nullptr);
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawEllipseWithBrushAndStrokeWidth(
        Vector2 centerPoint,
        float radiusX,
        float radiusY,
        ICanvasBrush* brush,
        float strokeWidth)
    {
        return DrawEllipseWithBrushAndStrokeWidthAndStrokeStyle(
            centerPoint, 
            radiusX, 
            radiusY, 
            brush, 
            strokeWidth, 
            nullptr);
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawEllipseAtCoordsWithBrushAndStrokeWidth(
        float x,
        float y,
        float radiusX,
        float radiusY,
        ICanvasBrush* brush,
        float strokeWidth)
    {
        return DrawEllipseWithBrushAndStrokeWidthAndStrokeStyle(
            Vector2{ x, y }, 
            radiusX, 
            radiusY, 
            brush, 
            strokeWidth, 
            nullptr);
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawEllipseWithColorAndStrokeWidth(
        Vector2 centerPoint,
        float radiusX,
        float radiusY,
        Color color,
        float strokeWidth)
    {
        return DrawEllipseWithColorAndStrokeWidthAndStrokeStyle(
            centerPoint, 
            radiusX, 
            radiusY, 
            color, 
            strokeWidth, 
            nullptr);
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawEllipseAtCoordsWithColorAndStrokeWidth(
        float x,
        float y,
        float radiusX,
        float radiusY,
        Color color,
        float strokeWidth)
    {
        return DrawEllipseWithColorAndStrokeWidthAndStrokeStyle(
            Vector2{ x, y }, 
            radiusX, 
            radiusY, 
            color, 
            strokeWidth, 
            nullptr);
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawEllipseWithBrushAndStrokeWidthAndStrokeStyle(
        Vector2 centerPoint,
        float radiusX,
        float radiusY,
        ICanvasBrush* brush,
        float strokeWidth,
        ICanvasStrokeStyle* strokeStyle)
    {
        return ExceptionBoundary(
            [&]
            {
                DrawEllipseImpl(
                    centerPoint, 
                    radiusX, 
                    radiusY,
                    ToD2DBrush(brush).Get(),
                    strokeWidth,
                    strokeStyle);
            });
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawEllipseAtCoordsWithBrushAndStrokeWidthAndStrokeStyle(
        float x,
        float y,
        float radiusX,
        float radiusY,
        ICanvasBrush* brush,
        float strokeWidth,
        ICanvasStrokeStyle* strokeStyle)
    {
        return DrawEllipseWithBrushAndStrokeWidthAndStrokeStyle(
            Vector2{ x, y }, 
            radiusX, 
            radiusY, 
            brush, 
            strokeWidth, 
            strokeStyle);
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawEllipseWithColorAndStrokeWidthAndStrokeStyle(
        Vector2 centerPoint,
        float radiusX,
        float radiusY,
        Color color,
        float strokeWidth,
        ICanvasStrokeStyle* strokeStyle)
    {
        return ExceptionBoundary(
            [&]
            {
                DrawEllipseImpl(
                    centerPoint, 
                    radiusX, 
                    radiusY,
                    GetColorBrush(color),
                    strokeWidth,
                    strokeStyle);
            });
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawEllipseAtCoordsWithColorAndStrokeWidthAndStrokeStyle(
        float x,
        float y,
        float radiusX,
        float radiusY,
        Color color,
        float strokeWidth,
        ICanvasStrokeStyle* strokeStyle)
    {
        return DrawEllipseWithColorAndStrokeWidthAndStrokeStyle(
            Vector2{ x, y }, 
            radiusX, 
            radiusY, 
            color, 
            strokeWidth, 
            strokeStyle);
    }


    void CanvasDrawingSession::DrawEllipseImpl(
        Vector2 const& centerPoint,
        float radiusX,
        float radiusY,
        ID2D1Brush* brush,
        float strokeWidth,
        ICanvasStrokeStyle* strokeStyle)
    {
        auto& deviceContext = GetResource();
        CheckInPointer(brush);

        auto d2dEllipse = ToD2DEllipse(centerPoint, radiusX, radiusY);

        deviceContext->DrawEllipse(
            &d2dEllipse,
            brush,
            strokeWidth,
            ToD2DStrokeStyle(strokeStyle, deviceContext.Get()).Get());
    }


    //
    // FillEllipse
    //

    IFACEMETHODIMP CanvasDrawingSession::FillEllipseWithBrush(
        Vector2 centerPoint,
        float radiusX,
        float radiusY,
        ICanvasBrush* brush)
    {
        return ExceptionBoundary(
            [&]
            {
                FillEllipseImpl(
                    centerPoint, 
                    radiusX, 
                    radiusY,
                    ToD2DBrush(brush).Get());
            });
    }


    IFACEMETHODIMP CanvasDrawingSession::FillEllipseAtCoordsWithBrush(
        float x,
        float y,
        float radiusX,
        float radiusY,
        ICanvasBrush* brush)
    {
        return FillEllipseWithBrush(
            Vector2{ x, y }, 
            radiusX, 
            radiusY, 
            brush);
    }


    IFACEMETHODIMP CanvasDrawingSession::FillEllipseWithColor(
        Vector2 centerPoint,
        float radiusX,
        float radiusY,
        Color color)
    {
        return ExceptionBoundary(
            [&]
            {
                FillEllipseImpl(
                    centerPoint, 
                    radiusX, 
                    radiusY,
                    GetColorBrush(color));
            });
    }


    IFACEMETHODIMP CanvasDrawingSession::FillEllipseAtCoordsWithColor(
        float x,
        float y,
        float radiusX,
        float radiusY,
        Color color)
    {
        return FillEllipseWithColor(
            Vector2{ x, y, }, 
            radiusX, 
            radiusY, 
            color);
    }


    void CanvasDrawingSession::FillEllipseImpl(
        Vector2 const& centerPoint,
        float radiusX,
        float radiusY,
        ID2D1Brush* brush)
    {
        auto& deviceContext = GetResource();
        CheckInPointer(brush);

        auto d2dEllipse = ToD2DEllipse(centerPoint, radiusX, radiusY);

        deviceContext->FillEllipse(
            &d2dEllipse,
            brush);
    }


    //
    // DrawCircle
    //

    IFACEMETHODIMP CanvasDrawingSession::DrawCircleWithBrush(
        Vector2 centerPoint,
        float radius,
        ICanvasBrush* brush)
    {
        return DrawEllipseWithBrushAndStrokeWidthAndStrokeStyle(
            centerPoint, 
            radius, 
            radius, 
            brush, 
            1.0f, 
            nullptr);
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawCircleAtCoordsWithBrush(
        float x,
        float y,
        float radius,
        ICanvasBrush* brush)
    {
        return DrawEllipseWithBrushAndStrokeWidthAndStrokeStyle(
            Vector2{ x, y }, 
            radius, 
            radius, 
            brush, 
            1.0f, 
            nullptr);
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawCircleWithColor(
        Vector2 centerPoint,
        float radius,
        Color color)
    {
        return DrawEllipseWithColorAndStrokeWidthAndStrokeStyle(
            centerPoint, 
            radius, 
            radius, 
            color, 
            1.0f, 
            nullptr);
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawCircleAtCoordsWithColor(
        float x,
        float y,
        float radius,
        Color color)
    {
        return DrawEllipseWithColorAndStrokeWidthAndStrokeStyle(
            Vector2{ x, y }, 
            radius, 
            radius, 
            color, 
            1.0f, 
            nullptr);
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawCircleWithBrushAndStrokeWidth(
        Vector2 centerPoint,
        float radius,
        ICanvasBrush* brush,
        float strokeWidth)
    {
        return DrawEllipseWithBrushAndStrokeWidthAndStrokeStyle(
            centerPoint, 
            radius, 
            radius, 
            brush, 
            strokeWidth, 
            nullptr);
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawCircleAtCoordsWithBrushAndStrokeWidth(
        float x,
        float y,
        float radius,
        ICanvasBrush* brush,
        float strokeWidth)
    {
        return DrawEllipseWithBrushAndStrokeWidthAndStrokeStyle(
            Vector2{ x, y }, 
            radius, 
            radius, 
            brush, 
            strokeWidth, 
            nullptr);
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawCircleWithColorAndStrokeWidth(
        Vector2 centerPoint,
        float radius,
        Color color,
        float strokeWidth)
    {
        return DrawEllipseWithColorAndStrokeWidthAndStrokeStyle(
            centerPoint, 
            radius, 
            radius, 
            color, 
            strokeWidth, 
            nullptr);
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawCircleAtCoordsWithColorAndStrokeWidth(
        float x,
        float y,
        float radius,
        Color color,
        float strokeWidth)
    {
        return DrawEllipseWithColorAndStrokeWidthAndStrokeStyle(
            Vector2{ x, y }, 
            radius, 
            radius, 
            color, 
            strokeWidth, 
            nullptr);
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawCircleWithBrushAndStrokeWidthAndStrokeStyle(
        Vector2 centerPoint,
        float radius,
        ICanvasBrush* brush,
        float strokeWidth,
        ICanvasStrokeStyle* strokeStyle)
    {
        return DrawEllipseWithBrushAndStrokeWidthAndStrokeStyle(
            centerPoint, 
            radius, 
            radius, 
            brush, 
            strokeWidth, 
            strokeStyle);
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawCircleAtCoordsWithBrushAndStrokeWidthAndStrokeStyle(
        float x,
        float y,
        float radius,
        ICanvasBrush* brush,
        float strokeWidth,
        ICanvasStrokeStyle* strokeStyle)
    {
        return DrawEllipseWithBrushAndStrokeWidthAndStrokeStyle(
            Vector2{ x, y }, 
            radius, 
            radius, 
            brush, 
            strokeWidth, 
            strokeStyle);
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawCircleWithColorAndStrokeWidthAndStrokeStyle(
        Vector2 centerPoint,
        float radius,
        Color color,
        float strokeWidth,
        ICanvasStrokeStyle* strokeStyle)
    {
        return DrawEllipseWithColorAndStrokeWidthAndStrokeStyle(
            centerPoint, 
            radius, 
            radius, 
            color, 
            strokeWidth, 
            strokeStyle);
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawCircleAtCoordsWithColorAndStrokeWidthAndStrokeStyle(
        float x,
        float y,
        float radius,
        Color color,
        float strokeWidth,
        ICanvasStrokeStyle* strokeStyle)
    {
        return DrawEllipseWithColorAndStrokeWidthAndStrokeStyle(
            Vector2{ x, y }, 
            radius, 
            radius, 
            color, 
            strokeWidth, 
            strokeStyle);
    }


    //
    // FillCircle
    //

    IFACEMETHODIMP CanvasDrawingSession::FillCircleWithBrush(
        Vector2 centerPoint,
        float radius,
        ICanvasBrush* brush)
    {
        return FillEllipseWithBrush(
            centerPoint, 
            radius, 
            radius, 
            brush);
    }


    IFACEMETHODIMP CanvasDrawingSession::FillCircleAtCoordsWithBrush(
        float x,
        float y,
        float radius,
        ICanvasBrush* brush)
    {
        return FillEllipseWithBrush(
            Vector2{ x, y }, 
            radius, 
            radius, 
            brush);
    }


    IFACEMETHODIMP CanvasDrawingSession::FillCircleWithColor(
        Vector2 centerPoint,
        float radius,
        Color color)
    {
        return FillEllipseWithColor(
            centerPoint, 
            radius, 
            radius, 
            color);
    }


    IFACEMETHODIMP CanvasDrawingSession::FillCircleAtCoordsWithColor(
        float x,
        float y,
        float radius,
        Color color)
    {
        return FillEllipseWithColor(
            Vector2{ x, y }, 
            radius, 
            radius, 
            color);
    }


    //
    // DrawText
    //

    IFACEMETHODIMP CanvasDrawingSession::DrawTextAtPointWithColor(
        HSTRING text,
        Vector2 point,
        Color color)
    {
        return DrawTextAtPointWithColorAndFormat(
            text, 
            point, 
            color, 
            nullptr);
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawTextAtPointCoordsWithColor(
        HSTRING text,
        float x,
        float y,
        Color color)
    {
        return DrawTextAtPointWithColorAndFormat(
            text, 
            Vector2{ x, y }, 
            color, 
            nullptr);
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawTextAtPointWithBrushAndFormat(
        HSTRING text,
        Vector2 point,
        ICanvasBrush* brush,
        ICanvasTextFormat* format)
    {
        return ExceptionBoundary(
            [&]
            {
                DrawTextAtPointImpl(
                    text, 
                    point, 
                    ToD2DBrush(brush).Get(), 
                    format);
            });
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawTextAtRectWithBrushAndFormat(
        HSTRING text,
        Rect rectangle,
        ICanvasBrush* brush,
        ICanvasTextFormat* format)
    {
        return ExceptionBoundary(
            [&]
            {
                DrawTextAtRectImpl(
                    text,
                    rectangle,
                    ToD2DBrush(brush).Get(),
                    format);
            });
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawTextAtPointCoordsWithBrushAndFormat(
        HSTRING text,
        float x,
        float y,
        ICanvasBrush* brush,
        ICanvasTextFormat* format)
    {
        return DrawTextAtPointWithBrushAndFormat(
            text,
            Vector2{ x, y },
            brush,
            format);
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawTextAtRectCoordsWithBrushAndFormat(
        HSTRING text,
        float x,
        float y,
        float w,
        float h,
        ICanvasBrush* brush,
        ICanvasTextFormat* format)
    {
        return DrawTextAtRectWithBrushAndFormat(
            text,
            Rect{ x, y, w, h },
            brush,
            format);
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawTextAtPointWithColorAndFormat(
        HSTRING text,
        Vector2 point,
        Color color,
        ICanvasTextFormat* format)
    {
        return ExceptionBoundary(
            [&]
            {
                DrawTextAtPointImpl(
                    text, 
                    point, 
                    GetColorBrush(color), 
                    format);
            });
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawTextAtRectWithColorAndFormat(
        HSTRING text,
        Rect rectangle,
        Color color,
        ICanvasTextFormat* format)
    {
        return ExceptionBoundary(
            [&]
            {
                DrawTextAtRectImpl(
                    text, 
                    rectangle, 
                    GetColorBrush(color), 
                    format);
            });
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawTextAtPointCoordsWithColorAndFormat(
        HSTRING text,
        float x,
        float y,
        Color color,
        ICanvasTextFormat* format)
    {
        return DrawTextAtPointWithColorAndFormat(
            text,
            Vector2{ x, y },
            color,
            format);
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawTextAtRectCoordsWithColorAndFormat(
        HSTRING text,
        float x,
        float y,
        float w,
        float h,
        Color color,
        ICanvasTextFormat* format)
    {
        return DrawTextAtRectWithColorAndFormat(
            text,
            Rect{ x, y, w, h },
            color,
            format);
    }


    void CanvasDrawingSession::DrawTextAtRectImpl(
        HSTRING text,
        Rect const& rect,
        ID2D1Brush* brush,
        ICanvasTextFormat* format)
    {
        auto& deviceContext = GetResource();
        CheckInPointer(brush);

        if (!format)
        {
            format = GetDefaultTextFormat();
        }

        ComPtr<ICanvasTextFormatInternal> formatInternal;
        ThrowIfFailed(format->QueryInterface(formatInternal.GetAddressOf()));

        uint32_t textLength;
        auto textBuffer = WindowsGetStringRawBuffer(text, &textLength);
        ThrowIfNullPointer(textBuffer, E_INVALIDARG);

        auto d2dRect = ToD2DRect(rect);

        deviceContext->DrawText(
            textBuffer,
            textLength,
            formatInternal->GetRealizedTextFormat().Get(),
            &d2dRect,
            brush,
            static_cast<D2D1_DRAW_TEXT_OPTIONS>(formatInternal->GetDrawTextOptions()));
    }


    void CanvasDrawingSession::DrawTextAtPointImpl(
        HSTRING text,
        Vector2 const& point,
        ID2D1Brush* brush,
        ICanvasTextFormat* format)
    {
        if (!format)
        {
            format = GetDefaultTextFormat();
        }

        // When drawing using just a point we specify a zero sized rectangle and
        // disable word wrapping.
        Rect rect{ point.X, point.Y, 0, 0 };

        //
        // TODO #802: there's a thread-safety implication since we're modifying state
        // on something that _may_ be being used on another thread.
        //
        CanvasWordWrapping oldWordWrapping{};
        ThrowIfFailed(format->get_WordWrapping(&oldWordWrapping));

        ThrowIfFailed(format->put_WordWrapping(CanvasWordWrapping::NoWrap));

        auto restoreWordWrapping = MakeScopeWarden(
            [&]
            {
                format->put_WordWrapping(oldWordWrapping);
            });

        DrawTextAtRectImpl(text, rect, brush, format);
    }


    ICanvasTextFormat* CanvasDrawingSession::GetDefaultTextFormat()
    {
        if (!m_defaultTextFormat)
        {
            m_defaultTextFormat = Make<CanvasTextFormat>();
            CheckMakeResult(m_defaultTextFormat);
        }

        return m_defaultTextFormat.Get();
    }

    
    //
    // DrawGeometry
    //

    IFACEMETHODIMP CanvasDrawingSession::DrawGeometryWithBrush(
        ICanvasGeometry* geometry,
        ICanvasBrush* brush)
    {
        return DrawGeometryWithBrushAndStrokeWidthAndStrokeStyle(
            geometry,
            brush, 
            1.0f, 
            nullptr);
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawGeometryWithColor(
        ICanvasGeometry* geometry,
        Color color)
    {
        return DrawGeometryWithColorAndStrokeWidthAndStrokeStyle(
            geometry,
            color, 
            1.0f, 
            nullptr);
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawGeometryWithBrushAndStrokeWidth(
        ICanvasGeometry* geometry,
        ICanvasBrush* brush,
        float strokeWidth)
    {
        return DrawGeometryWithBrushAndStrokeWidthAndStrokeStyle(
            geometry,
            brush, 
            strokeWidth, 
            nullptr);
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawGeometryWithColorAndStrokeWidth(
        ICanvasGeometry* geometry,
        Color color,
        float strokeWidth)
    {
        return DrawGeometryWithColorAndStrokeWidthAndStrokeStyle(
            geometry,
            color, 
            strokeWidth, 
            nullptr);
    }

    IFACEMETHODIMP CanvasDrawingSession::DrawGeometryWithBrushAndStrokeWidthAndStrokeStyle(
        ICanvasGeometry* geometry,
        ICanvasBrush* brush,
        float strokeWidth,
        ICanvasStrokeStyle* strokeStyle)
    {
        return ExceptionBoundary(
            [&]
            {
                auto& deviceContext = GetResource();
                CheckInPointer(geometry);
                CheckInPointer(brush);

                deviceContext->DrawGeometry(
                    GetWrappedResource<ID2D1Geometry>(geometry).Get(),
                    ToD2DBrush(brush).Get(),
                    strokeWidth,
                    ToD2DStrokeStyle(strokeStyle, deviceContext.Get()).Get());
            });
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawGeometryWithColorAndStrokeWidthAndStrokeStyle(
        ICanvasGeometry* geometry,
        Color color,
        float strokeWidth,
        ICanvasStrokeStyle* strokeStyle)
    {
        return ExceptionBoundary(
            [&]
            {
                auto& deviceContext = GetResource();
                CheckInPointer(geometry);

                deviceContext->DrawGeometry(
                    GetWrappedResource<ID2D1Geometry>(geometry).Get(),
                    GetColorBrush(color),
                    strokeWidth,
                    ToD2DStrokeStyle(strokeStyle, deviceContext.Get()).Get());  
            });
    }


    //
    // FillGeometry
    //

    IFACEMETHODIMP CanvasDrawingSession::FillGeometryWithBrush(
        ICanvasGeometry* geometry,
        ICanvasBrush* brush)
    {
        return ExceptionBoundary(
            [&]
            {
                auto& deviceContext = GetResource();
                CheckInPointer(geometry);
                CheckInPointer(brush);

                deviceContext->FillGeometry(
                    GetWrappedResource<ID2D1Geometry>(geometry).Get(),
                    ToD2DBrush(brush).Get(),
                    nullptr);
            });
    }


    IFACEMETHODIMP CanvasDrawingSession::FillGeometryWithColor(
        ICanvasGeometry* geometry,
        Color color)
    {
        return ExceptionBoundary(
            [&]
            {
                auto& deviceContext = GetResource();
                CheckInPointer(geometry);

                deviceContext->FillGeometry(
                    GetWrappedResource<ID2D1Geometry>(geometry).Get(),
                    GetColorBrush(color),
                    nullptr);
            });
    }

    //
    // DrawCachedGeometry
    //

    IFACEMETHODIMP CanvasDrawingSession::DrawCachedGeometryWithBrush(
        ICanvasCachedGeometry* cachedGeometry,
        ICanvasBrush* brush)
    {
        return ExceptionBoundary(
            [&]
            {
                auto& deviceContext = GetResource();
                CheckInPointer(cachedGeometry);
                CheckInPointer(brush);

                deviceContext->DrawGeometryRealization(
                    GetWrappedResource<ID2D1GeometryRealization>(cachedGeometry).Get(),
                    ToD2DBrush(brush).Get());
            });
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawCachedGeometryWithColor(
        ICanvasCachedGeometry* cachedGeometry,
        Color color)
    {
        return ExceptionBoundary(
            [&]
            {
                auto& deviceContext = GetResource();
                CheckInPointer(cachedGeometry);

                deviceContext->DrawGeometryRealization(
                    GetWrappedResource<ID2D1GeometryRealization>(cachedGeometry).Get(),
                    GetColorBrush(color));
            });
    }

    ID2D1SolidColorBrush* CanvasDrawingSession::GetColorBrush(Color const& color)
    {
        if (m_solidColorBrush)
        {
            m_solidColorBrush->SetColor(ToD2DColor(color));
        }
        else
        {
            // TODO #802: pool and reuse this brush along with the device context?
            auto& deviceContext = GetResource();
            ThrowIfFailed(deviceContext->CreateSolidColorBrush(ToD2DColor(color), &m_solidColorBrush));
        }

        return m_solidColorBrush.Get();
    }


    ComPtr<ID2D1Brush> CanvasDrawingSession::ToD2DBrush(ICanvasBrush* brush)
    {
        if (!brush)
            return nullptr;

        auto& deviceContext = GetResource();

        return As<ICanvasBrushInternal>(brush)->GetD2DBrush(deviceContext.Get());
    }


    IFACEMETHODIMP CanvasDrawingSession::get_Antialiasing(CanvasAntialiasing* value)
    {
        return ExceptionBoundary(
            [&]
            {
                auto& deviceContext = GetResource();
                CheckInPointer(value);

                *value = static_cast<CanvasAntialiasing>(deviceContext->GetAntialiasMode());
            });
	}

    IFACEMETHODIMP CanvasDrawingSession::put_Antialiasing(CanvasAntialiasing value)
	{
        return ExceptionBoundary(
            [&]
            {
                auto& deviceContext = GetResource();

                deviceContext->SetAntialiasMode(static_cast<D2D1_ANTIALIAS_MODE>(value));
            });
	}

    IFACEMETHODIMP CanvasDrawingSession::get_Blend(CanvasBlend* value)
	{
        return ExceptionBoundary(
            [&]
            {
                auto& deviceContext = GetResource();
                CheckInPointer(value);

                *value = static_cast<CanvasBlend>(deviceContext->GetPrimitiveBlend());
            });
	}

    IFACEMETHODIMP CanvasDrawingSession::put_Blend(CanvasBlend value)
	{
        return ExceptionBoundary(
            [&]
            {
                auto& deviceContext = GetResource();

                deviceContext->SetPrimitiveBlend(static_cast<D2D1_PRIMITIVE_BLEND>(value));
            });
	}

    IFACEMETHODIMP CanvasDrawingSession::get_TextAntialiasing(CanvasTextAntialiasing* value)
    {
        return ExceptionBoundary(
            [&]
            {
                auto& deviceContext = GetResource();
                CheckInPointer(value);

                *value = static_cast<CanvasTextAntialiasing>(deviceContext->GetTextAntialiasMode());
            });
	}

    IFACEMETHODIMP CanvasDrawingSession::put_TextAntialiasing(CanvasTextAntialiasing value)
	{
        return ExceptionBoundary(
            [&]
            {
                auto& deviceContext = GetResource();

                deviceContext->SetTextAntialiasMode(static_cast<D2D1_TEXT_ANTIALIAS_MODE>(value));
            });
	}

    IFACEMETHODIMP CanvasDrawingSession::get_Transform(ABI::Microsoft::Graphics::Canvas::Numerics::Matrix3x2* value)
    {
        return ExceptionBoundary(
            [&]
            {
                auto& deviceContext = GetResource();
                CheckInPointer(value);

                D2D1_MATRIX_3X2_F transform;
                deviceContext->GetTransform(&transform);
                
                //
                // Un-apply the offset transform. This could be done with a matrix invert, 
                // but this is cheaper.
                //
                // This polls from the wrapped object, rather than stores a local transform 
                // member. This ensures that any transforms performed in native interop
                // will be retrievable here.
                //
                const D2D1_POINT_2F renderingSurfaceOffset = m_adapter->GetRenderingSurfaceOffset();
                transform._31 -= renderingSurfaceOffset.x;
                transform._32 -= renderingSurfaceOffset.y;

                *value = *reinterpret_cast<ABI::Microsoft::Graphics::Canvas::Numerics::Matrix3x2*>(&transform);
            });
	}

    IFACEMETHODIMP CanvasDrawingSession::put_Transform(ABI::Microsoft::Graphics::Canvas::Numerics::Matrix3x2 value)
	{
        return ExceptionBoundary(
            [&]
            {
                auto& deviceContext = GetResource();
                
                D2D1_POINT_2F offset = m_adapter->GetRenderingSurfaceOffset();

                D2D1_MATRIX_3X2_F transform = *(ReinterpretAs<D2D1_MATRIX_3X2_F*>(&value));
                transform._31 += offset.x;
                transform._32 += offset.y;

                deviceContext->SetTransform(transform);
            });
	}

    IFACEMETHODIMP CanvasDrawingSession::get_Units(CanvasUnits* value)
    {
        return ExceptionBoundary(
            [&]
            {
                auto& deviceContext = GetResource();
                CheckInPointer(value);

                *value = static_cast<CanvasUnits>(deviceContext->GetUnitMode());
            });
	}

    IFACEMETHODIMP CanvasDrawingSession::put_Units(CanvasUnits value)
	{
        return ExceptionBoundary(
            [&]
            {
                auto& deviceContext = GetResource();

                deviceContext->SetUnitMode(static_cast<D2D1_UNIT_MODE>(value));
            });
    }

    IFACEMETHODIMP CanvasDrawingSession::get_Device(ICanvasDevice** value)
    {
        using namespace ::Microsoft::WRL::Wrappers;

        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(value);

                if (!m_owner)
                {
                    auto& deviceContext = GetResource();

                    ComPtr<ID2D1Device> d2dDeviceBase;
                    deviceContext->GetDevice(&d2dDeviceBase);

                    ComPtr<ID2D1Device1> d2dDevice;
                    ThrowIfFailed(d2dDeviceBase.As(&d2dDevice));

                    auto canvasDeviceManager = CanvasDeviceFactory::GetOrCreateManager();

                    m_owner = canvasDeviceManager->GetOrCreate(d2dDevice.Get());
                }

                ThrowIfFailed(m_owner.CopyTo(value));
            });
    }

    IFACEMETHODIMP CanvasDrawingSession::get_Dpi(float* dpi)
    {
        return ExceptionBoundary(
            [&]
            {
                auto& deviceContext = GetResource();
                CheckInPointer(dpi);

                *dpi = GetDpi(deviceContext);
            });
    }

    IFACEMETHODIMP CanvasDrawingSession::ConvertPixelsToDips(int pixels, float* dips)
    {
        return ExceptionBoundary(
            [&]
            {
                auto& deviceContext = GetResource();
                CheckInPointer(dips);

                *dips = PixelsToDips(pixels, GetDpi(deviceContext));
            });
    }

    IFACEMETHODIMP CanvasDrawingSession::ConvertDipsToPixels(float dips, int* pixels)
    {
        return ExceptionBoundary(
            [&]
            {
                auto& deviceContext = GetResource();
                CheckInPointer(pixels);

                *pixels = DipsToPixels(dips, GetDpi(deviceContext));
            });
    }

    ActivatableStaticOnlyFactory(CanvasDrawingSessionFactory);
}}}}
