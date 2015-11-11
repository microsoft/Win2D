// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

#include "CanvasActiveLayer.h"
#include "text/CanvasTextFormat.h"
#include "text/CanvasTextRenderingParameters.h"
#include "text/CanvasFontFace.h"
#include "utils/TemporaryTransform.h"
#include "text/TextUtilities.h"
#include "text/InternalDWriteTextRenderer.h"
#include "text/DrawGlyphRunHelper.h"

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


    static D2D1_SIZE_F GetBitmapSize(D2D1_UNIT_MODE unitMode, ID2D1Bitmap* bitmap)
    {
        switch (unitMode)
        {
        case D2D1_UNIT_MODE_DIPS:
            return bitmap->GetSize();
            
        case D2D1_UNIT_MODE_PIXELS:
        {
            auto pixelSize = bitmap->GetPixelSize();
            return D2D1_SIZE_F{ static_cast<float>(pixelSize.width), static_cast<float>(pixelSize.height) };
        }

        default:
            assert(false);
            return D2D1_SIZE_F{};
        }
    }
    

    //
    // This drawing session adapter is used when wrapping an existing
    // ID2D1DeviceContext.  In this wrapper, interop, case we don't want
    // CanvasDrawingSession to call any additional methods in the device context.
    //
    class NoopCanvasDrawingSessionAdapter : public DrawingSessionBaseAdapter,
                                            private LifespanTracker<NoopCanvasDrawingSessionAdapter>
    {
    public:
        virtual void EndDraw(ID2D1DeviceContext1*) override
        {
            // nothing
        }
    };


    ComPtr<CanvasDrawingSession> CanvasDrawingSession::CreateNew(
        ID2D1DeviceContext1* deviceContext,
        std::shared_ptr<ICanvasDrawingSessionAdapter> drawingSessionAdapter,
        ICanvasDevice* owner,
        D2D1_POINT_2F offset)
    {
        InitializeDefaultState(deviceContext);

        auto drawingSession = Make<CanvasDrawingSession>(
            deviceContext,
            drawingSessionAdapter,
            owner,
            offset);
        CheckMakeResult(drawingSession);

        return drawingSession;
    }


    void CanvasDrawingSession::InitializeDefaultState(ID2D1DeviceContext1* deviceContext)
    {
        // Win2D wants a different text antialiasing default vs. native D2D.
        deviceContext->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);
    }


#if WINVER > _WIN32_WINNT_WINBLUE
    ComPtr<IInkD2DRenderer> DrawingSessionBaseAdapter::CreateInkRenderer()
    {
        ComPtr<IInkD2DRenderer> inkRenderer;

        ThrowIfFailed(CoCreateInstance(__uuidof(InkD2DRenderer),
            nullptr,
            CLSCTX_INPROC_SERVER,
            IID_PPV_ARGS(&inkRenderer)));

        return inkRenderer;
    }


    bool DrawingSessionBaseAdapter::IsHighContrastEnabled()
    {
        if (!m_accessibilitySettings)
        {
            ComPtr<IActivationFactory> activationFactory;
            ThrowIfFailed(GetActivationFactory(HStringReference(RuntimeClass_Windows_UI_ViewManagement_AccessibilitySettings).Get(), &activationFactory));
            ThrowIfFailed(activationFactory->ActivateInstance(&m_accessibilitySettings));
        }

        boolean isHighContrastEnabled;
        ThrowIfFailed(m_accessibilitySettings->get_HighContrast(&isHighContrastEnabled));
        return !!isHighContrastEnabled;
    }
#endif


    CanvasDrawingSession::CanvasDrawingSession(
        ID2D1DeviceContext1* deviceContext,
        std::shared_ptr<ICanvasDrawingSessionAdapter> adapter,
        ICanvasDevice* owner,
        D2D1_POINT_2F offset)
        : ResourceWrapper(deviceContext)
        , m_adapter(adapter ? adapter : std::make_shared<NoopCanvasDrawingSessionAdapter>())
        , m_offset(offset)
        , m_nextLayerId(0)
        , m_owner(owner)
    {
    }


    CanvasDrawingSession::~CanvasDrawingSession()
    {
        // Ignore any errors when closing during destruction
        (void)Close();
    }


    IFACEMETHODIMP CanvasDrawingSession::Close()
    {
        return ExceptionBoundary(
            [&]
            {
                auto deviceContext = MaybeGetResource();
        
                ReleaseResource();

                if (!m_activeLayerIds.empty())
                    ThrowHR(E_FAIL, Strings::DidNotPopLayer);

                if (m_adapter)
                {
                    assert(deviceContext);
                    
                    // Arrange it so that m_adapter will always get
                    // reset, even if EndDraw throws.
                    auto adapter = m_adapter;
                    m_adapter.reset();

                    adapter->EndDraw(deviceContext.Get());
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


    IFACEMETHODIMP CanvasDrawingSession::Flush()
    {
        return ExceptionBoundary(
            [&]
            {
                ThrowIfFailed(GetResource()->Flush(nullptr, nullptr));
            });
    }


    // 
    // DrawImage
    //  

    IFACEMETHODIMP CanvasDrawingSession::DrawImageAtOrigin(
        ICanvasImage* image)
    {
        auto offset = Vector2{ 0, 0 };
        return DrawImageImpl(image, &offset, nullptr, nullptr, DefaultDrawImageOpacity(), DefaultDrawImageInterpolation(), nullptr);
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawImageAtOffset(
        ICanvasImage* image,
        Vector2 offset)
    {
        return DrawImageImpl(image, &offset, nullptr, nullptr, DefaultDrawImageOpacity(), DefaultDrawImageInterpolation(), nullptr);
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawImageAtCoords(
        ICanvasImage* image,
        float x,
        float y)
    {
        auto offset = Vector2{ x, y };
        return DrawImageImpl(image, &offset, nullptr, nullptr, DefaultDrawImageOpacity(), DefaultDrawImageInterpolation(), nullptr);
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawImageToRect(
        ICanvasBitmap* bitmap,
        Rect destinationRect)
    {   
        return DrawBitmapImpl(bitmap, nullptr, &destinationRect, nullptr, DefaultDrawImageOpacity(), DefaultDrawImageInterpolation(), nullptr);
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawImageAtOffsetWithSourceRect(
        ICanvasImage* image,
        Vector2 offset,
        Rect sourceRect)
    {
        return DrawImageImpl(image, &offset, nullptr, &sourceRect, DefaultDrawImageOpacity(), DefaultDrawImageInterpolation(), nullptr);
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawImageAtCoordsWithSourceRect(
        ICanvasImage* image,
        float x,
        float y,
        Rect sourceRect)
    {
        auto offset = Vector2{ x, y };
        return DrawImageImpl(image, &offset, nullptr, &sourceRect, DefaultDrawImageOpacity(), DefaultDrawImageInterpolation(), nullptr);
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawImageToRectWithSourceRect(
        ICanvasImage* image,
        Rect destinationRect,
        Rect sourceRect)
    {
        return DrawImageImpl(image, nullptr, &destinationRect, &sourceRect, DefaultDrawImageOpacity(), DefaultDrawImageInterpolation(), nullptr);
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawImageAtOffsetWithSourceRectAndOpacity(
        ICanvasImage* image, 
        Vector2 offset,
        Rect sourceRectangle,
        float opacity)
    {
        return DrawImageImpl(image, &offset, nullptr, &sourceRectangle, opacity, DefaultDrawImageInterpolation(), nullptr);
    }

    IFACEMETHODIMP CanvasDrawingSession::DrawImageAtCoordsWithSourceRectAndOpacity(
        ICanvasImage* image, 
        float x,
        float y,
        Rect sourceRectangle,
        float opacity)
    {
        auto offset = Vector2{ x, y };
        return DrawImageImpl(image, &offset, nullptr, &sourceRectangle, opacity, DefaultDrawImageInterpolation(), nullptr);
    }

    IFACEMETHODIMP CanvasDrawingSession::DrawImageToRectWithSourceRectAndOpacity(
        ICanvasImage* image,
        Rect destinationRectangle,
        Rect sourceRectangle,
        float opacity)
    {
        return DrawImageImpl(image, nullptr, &destinationRectangle, &sourceRectangle, opacity, DefaultDrawImageInterpolation(), nullptr);
    }

    IFACEMETHODIMP CanvasDrawingSession::DrawImageAtOffsetWithSourceRectAndOpacityAndInterpolation(
        ICanvasImage* image, 
        Vector2 offset,
        Rect sourceRectangle,
        float opacity,
        CanvasImageInterpolation interpolation)
    {
        return DrawImageImpl(image, &offset, nullptr, &sourceRectangle, opacity, interpolation, nullptr);
    }

    IFACEMETHODIMP CanvasDrawingSession::DrawImageAtCoordsWithSourceRectAndOpacityAndInterpolation(
        ICanvasImage* image, 
        float x,
        float y,
        Rect sourceRectangle,
        float opacity,
        CanvasImageInterpolation interpolation)
    {
        auto offset = Vector2{ x, y };
        return DrawImageImpl(image, &offset, nullptr, &sourceRectangle, opacity, interpolation, nullptr);
    }
    
    IFACEMETHODIMP CanvasDrawingSession::DrawImageToRectWithSourceRectAndOpacityAndInterpolation(
        ICanvasImage* image,
        Rect destinationRectangle,
        Rect sourceRectangle,
        float opacity,
        CanvasImageInterpolation interpolation)
    {
        return DrawImageImpl(image, nullptr, &destinationRectangle, &sourceRectangle, opacity, interpolation, nullptr);
    }

    IFACEMETHODIMP CanvasDrawingSession::DrawImageAtOffsetWithSourceRectAndOpacityAndInterpolationAndComposite(
        ICanvasImage* image, 
        Vector2 offset,
        Rect sourceRectangle,
        float opacity,
        CanvasImageInterpolation interpolation,
        CanvasComposite composite)
    {
        return DrawImageImpl(image, &offset, nullptr, &sourceRectangle, opacity, interpolation, &composite);
    }

    IFACEMETHODIMP CanvasDrawingSession::DrawImageAtCoordsWithSourceRectAndOpacityAndInterpolationAndComposite(
        ICanvasImage* image, 
        float x,
        float y,
        Rect sourceRectangle,
        float opacity,
        CanvasImageInterpolation interpolation,
        CanvasComposite composite)
    {
        auto offset = Vector2{ x, y };
        return DrawImageImpl(image, &offset, nullptr, &sourceRectangle, opacity, interpolation, &composite);
    }

    IFACEMETHODIMP CanvasDrawingSession::DrawImageToRectWithSourceRectAndOpacityAndInterpolationAndComposite(
        ICanvasImage* image,
        Rect destinationRectangle,
        Rect sourceRectangle,
        float opacity,
        CanvasImageInterpolation interpolation,
        CanvasComposite composite)
    {
        return DrawImageImpl(image, nullptr, &destinationRectangle, &sourceRectangle, opacity, interpolation, &composite);
    }

    IFACEMETHODIMP CanvasDrawingSession::DrawImageAtOffsetWithSourceRectAndOpacityAndInterpolationAndPerspective(
        ICanvasBitmap* bitmap, 
        Vector2 offset,
        Rect sourceRectangle,
        float opacity,
        CanvasImageInterpolation interpolation,
        Matrix4x4 perspective)
    {
        return DrawBitmapImpl(bitmap, &offset, nullptr, &sourceRectangle, opacity, interpolation, &perspective);
    }

    IFACEMETHODIMP CanvasDrawingSession::DrawImageAtCoordsWithSourceRectAndOpacityAndInterpolationAndPerspective(
        ICanvasBitmap* bitmap, 
        float x,
        float y,
        Rect sourceRectangle,
        float opacity,
        CanvasImageInterpolation interpolation,
        Matrix4x4 perspective)
    {
        auto offset = Vector2{ x, y };
        return DrawBitmapImpl(bitmap, &offset, nullptr, &sourceRectangle, opacity, interpolation, &perspective);
    }

    IFACEMETHODIMP CanvasDrawingSession::DrawImageToRectWithSourceRectAndOpacityAndInterpolationAndPerspective(
        ICanvasBitmap* bitmap,
        Rect destinationRectangle,
        Rect sourceRectangle,
        float opacity,
        CanvasImageInterpolation interpolation,
        Matrix4x4 perspective)
    {
        return DrawBitmapImpl(bitmap, nullptr, &destinationRectangle, &sourceRectangle, opacity, interpolation, &perspective);
    }


    class DrawImageWorker
    {
        ICanvasDevice* m_canvasDevice;
        ID2D1DeviceContext1* m_deviceContext;
        Vector2* m_offset;
        Rect* m_destinationRect;
        Rect* m_sourceRect;
        float m_opacity;
        CanvasImageInterpolation m_interpolation;

        D2D1_RECT_F m_d2dSourceRect;
        ComPtr<ID2D1Image> m_opacityEffectOutput;
        ComPtr<ID2D1Image> m_borderEffectOutput;

    public:
        DrawImageWorker(ICanvasDevice* canvasDevice, ID2D1DeviceContext1* deviceContext, Vector2* offset, Rect* destinationRect, Rect* sourceRect, float opacity, CanvasImageInterpolation interpolation)
            : m_canvasDevice(canvasDevice)
            , m_deviceContext(deviceContext)
            , m_offset(offset)
            , m_destinationRect(destinationRect)
            , m_sourceRect(sourceRect)
            , m_opacity(opacity)
            , m_interpolation(interpolation)
        {
            assert(m_offset || m_destinationRect);

            if (m_sourceRect)
                m_d2dSourceRect = ToD2DRect(*sourceRect);
        }

        void DrawBitmap(ICanvasBitmap* bitmap, Numerics::Matrix4x4* perspective)
        {
            DrawBitmap(As<ICanvasBitmapInternal>(bitmap).Get(), perspective);
        }

        void DrawImage(ICanvasImage* image, CanvasComposite const* composite)
        {
            // If this is a bitmap being drawn with sufficiently simple options, we can take the DrawBitmap fast path.
            auto internalBitmap = MaybeAs<ICanvasBitmapInternal>(image);
            
            if (internalBitmap &&
                IsValidDrawBitmapCompositeMode(composite) &&
                IsValidDrawBitmapInterpolationMode())
            {
                DrawBitmap(internalBitmap.Get(), nullptr);
            }
            else
            {
                // If DrawBitmap cannot handle this request, we must use the DrawImage slow path.

                auto internalImage = As<ICanvasImageInternal>(image);
                auto d2dImage = internalImage->GetD2DImage(m_canvasDevice, m_deviceContext);

                auto d2dInterpolationMode = static_cast<D2D1_INTERPOLATION_MODE>(m_interpolation);
                auto d2dCompositeMode = composite ? static_cast<D2D1_COMPOSITE_MODE>(*composite)
                                                  : GetCompositeModeFromPrimitiveBlend();

                if (m_offset)
                    DrawImageAtOffset(d2dImage.Get(), *m_offset, d2dInterpolationMode, d2dCompositeMode);
                else
                    DrawImageToRect(d2dImage.Get(), *m_destinationRect, d2dInterpolationMode, d2dCompositeMode);
            }
        }

    private:
        void DrawBitmap(ICanvasBitmapInternal* internalBitmap, Numerics::Matrix4x4* perspective)
        {
            auto& d2dBitmap = internalBitmap->GetD2DBitmap();

            auto d2dDestRect = CalculateDestRect(d2dBitmap.Get());

            m_deviceContext->DrawBitmap(
                d2dBitmap.Get(),
                &d2dDestRect,
                m_opacity,
                static_cast<D2D1_INTERPOLATION_MODE>(m_interpolation),
                GetD2DSourceRect(),
                ReinterpretAs<D2D1_MATRIX_4X4_F*>(perspective));
        }

        void DrawImageAtOffset(
            ID2D1Image* d2dImage,
            Vector2 offset,
            D2D1_INTERPOLATION_MODE d2dInterpolationMode,
            D2D1_COMPOSITE_MODE d2dCompositeMode)
        {
            auto d2dOffset = ToD2DPoint(offset);

            d2dImage = MaybeApplyOpacityEffect(d2dImage);

            m_deviceContext->DrawImage(d2dImage, &d2dOffset, GetD2DSourceRect(), d2dInterpolationMode, d2dCompositeMode);
        }

        void DrawImageToRect(
            ID2D1Image* d2dImage,
            Rect const& destinationRect,
            D2D1_INTERPOLATION_MODE d2dInterpolationMode,
            D2D1_COMPOSITE_MODE d2dCompositeMode)
        {
            assert(m_sourceRect);

            d2dImage = MaybeAdjustD2DSourceRect(d2dImage);
            d2dImage = MaybeApplyOpacityEffect(d2dImage);

            float sourceWidth  = m_d2dSourceRect.right - m_d2dSourceRect.left;
            float sourceHeight = m_d2dSourceRect.bottom - m_d2dSourceRect.top;

            if (sourceWidth == 0.0f || sourceHeight == 0.0f)
            {
                // There's no useful scale factor for scaling from something
                // that is zero sized. Consistent with observed DrawBitmap
                // behavior, we don't attempt to draw anything in this case.
                return;
            }

            auto offset = Vector2{ destinationRect.X, destinationRect.Y };
            auto scale = Vector2{ destinationRect.Width / sourceWidth, destinationRect.Height / sourceHeight };

            TemporaryTransform<ID2D1DeviceContext1> transform(m_deviceContext, offset, scale);

            auto d2dOffset = D2D1_POINT_2F{ 0, 0 };
            m_deviceContext->DrawImage(d2dImage, &d2dOffset, &m_d2dSourceRect, d2dInterpolationMode, d2dCompositeMode);
        }

        ID2D1Image* MaybeApplyOpacityEffect(ID2D1Image* d2dImage)
        {
            if (m_opacity >= 1.0f)
                return d2dImage;

            ComPtr<ID2D1Effect> opacityEffect;

            ThrowIfFailed(m_deviceContext->CreateEffect(CLSID_D2D1ColorMatrix, &opacityEffect));

            if (auto bitmap = MaybeAs<ID2D1Bitmap>(d2dImage))
            {
                //
                // When drawing a bitmap we need to explicitly compensate for
                // the bitmap's DPI before passing it to the color matrix effect
                // (since effects by default ignore a bitmap's DPI).
                //
                ThrowIfFailed(D2D1::SetDpiCompensatedEffectInput(m_deviceContext, opacityEffect.Get(), 0, bitmap.Get()));
            }
            else
            {
                opacityEffect->SetInput(0, d2dImage);
            }

            D2D1_MATRIX_5X4_F opacityMatrix = D2D1::Matrix5x4F(
                1, 0, 0, 0,
                0, 1, 0, 0,
                0, 0, 1, 0,
                0, 0, 0, m_opacity,
                0, 0, 0, 0);

            opacityEffect->SetValue(D2D1_COLORMATRIX_PROP_COLOR_MATRIX, opacityMatrix);
            
            opacityEffect->GetOutput(&m_opacityEffectOutput);
            return m_opacityEffectOutput.Get();
        }

        // DrawBitmap uses the current primitive blend setting, but DrawImage
        // takes an explicit composite mode parameter. We can only substitute
        // the former for the latter if these match.
        //
        // In some cases where they do not match, we could change the primitive
        // blend, use DrawBitmap, then change it back again. But that would be
        // more intrusive, so this implementation plays it safe and only
        // optimizes the simple case where the modes match exactly.
        //
        // If the composite parameter is null, the caller did not explicitly
        // specify a composite mode.  In that case we will use
        // GetCompositeModeFromPrimitiveBlend, so any primitive blend that has a
        // matching composite mode is valid.
        bool IsValidDrawBitmapCompositeMode(CanvasComposite const* composite)
        {
            switch (m_deviceContext->GetPrimitiveBlend())
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

        // When using a DrawImage overload that does not take an explicit
        // composite mode parameter, we try to match the current device context
        // primitive blend setting.
        D2D1_COMPOSITE_MODE GetCompositeModeFromPrimitiveBlend()
        {
            switch (m_deviceContext->GetPrimitiveBlend())
            {
            case D2D1_PRIMITIVE_BLEND_SOURCE_OVER:
                return D2D1_COMPOSITE_MODE_SOURCE_OVER;
                
            case D2D1_PRIMITIVE_BLEND_COPY:
                return D2D1_COMPOSITE_MODE_SOURCE_COPY;
                
            case D2D1_PRIMITIVE_BLEND_ADD:
                return D2D1_COMPOSITE_MODE_PLUS;
                
            case D2D1_PRIMITIVE_BLEND_MIN:
                ThrowHR(E_FAIL, Strings::DrawImageMinBlendNotSupported);
                
            default:
                ThrowHR(E_UNEXPECTED);
            }
        }
    
        // Although there are some ID2D1DeviceContext::DrawBitmap methods that
        // appear to take a full set of interpolation modes, it turns out that
        // the implementation of these do not all fully match the behavior of
        // their DrawImage equivalents.  Therefore, we will only use DrawBitmap
        // for this limited set of interpolation modes.
        bool IsValidDrawBitmapInterpolationMode()
        {
            return m_interpolation == CanvasImageInterpolation::Linear ||
                   m_interpolation == CanvasImageInterpolation::NearestNeighbor;
        }

        // DrawImage infers output size from the source image, but DrawBitmap
        // takes an explicit dest rect.  So to use DrawBitmap, we must duplicate
        // the same size logic that DrawImage would normally apply for us.
        D2D1_RECT_F CalculateDestRect(ID2D1Bitmap* d2dBitmap)
        {
            if (m_destinationRect)
                return ToD2DRect(*m_destinationRect);
            
            D2D1_SIZE_F destSize;
            
            if (m_sourceRect)
            {
                // If there is an explicit source rectangle, that determines the destination size too.
                destSize = D2D1_SIZE_F{ m_sourceRect->Width, m_sourceRect->Height };
            }
            else
            {
                destSize = GetBitmapSize(m_deviceContext->GetUnitMode(), d2dBitmap);
            }
            
            return D2D1_RECT_F
            {
                m_offset->X,
                m_offset->Y,
                m_offset->X + destSize.width,
                m_offset->Y + destSize.height
            };
        }

        ID2D1Image* MaybeAdjustD2DSourceRect(ID2D1Image* d2dImage)
        {
            auto d2dBitmap = MaybeAs<ID2D1Bitmap>(d2dImage);
            if (!d2dBitmap)
                return d2dImage;

            // If this is actually a bitmap, then sourceRect needs to be
            // adjusted so that it doesn't go beyond the bounds of the image.
            // This is in keeping with DrawBitmap's behavior.  We don't attempt
            // to do this with more generic image types since this is
            // prohibitively expensive.

            m_d2dSourceRect.left = std::max(m_d2dSourceRect.left, 0.0f); 
            m_d2dSourceRect.top  = std::max(m_d2dSourceRect.top,  0.0f);

            D2D1_SIZE_F size = GetBitmapSize(m_deviceContext->GetUnitMode(), d2dBitmap.Get());

            m_d2dSourceRect.right  = std::min(m_d2dSourceRect.right,  size.width);
            m_d2dSourceRect.bottom = std::min(m_d2dSourceRect.bottom, size.height);

            // D2D bitmap and image rendering paths have different border sampling behavior, so
            // when we emulate DrawBitmap using DrawImage, we must insert an explicit BorderEffect
            // to avoid unwanted translucency along the edges. We could get fancy and only do this
            // if the source rect is such that we will actually sample outside the bounds of the
            // image, but it is non trivial to detect that for different filter modes, and this
            // is a slow path in any case so we keep it simple and always add the border.

            ComPtr<ID2D1Effect> borderEffect;
            ThrowIfFailed(m_deviceContext->CreateEffect(CLSID_D2D1Border, &borderEffect));
            ThrowIfFailed(D2D1::SetDpiCompensatedEffectInput(m_deviceContext, borderEffect.Get(), 0, d2dBitmap.Get()));

            borderEffect->GetOutput(&m_borderEffectOutput);
            return m_borderEffectOutput.Get();
        }

        D2D1_RECT_F* GetD2DSourceRect()
        {
            if (m_sourceRect)
                return &m_d2dSourceRect;
            else
                return nullptr;
        }
    };

    HRESULT CanvasDrawingSession::DrawImageImpl(
        ICanvasImage* image,
        Vector2* offset,
        Rect* destinationRect,
        Rect* sourceRect,
        float opacity,
        CanvasImageInterpolation interpolation,
        CanvasComposite const* composite)
    {
        return ExceptionBoundary([&]
        {
            auto& deviceContext = GetResource();
            CheckInPointer(image);

            DrawImageWorker(GetDevice().Get(), deviceContext.Get(), offset, destinationRect, sourceRect, opacity, interpolation).DrawImage(image, composite);
        });

    }

    HRESULT CanvasDrawingSession::DrawBitmapImpl(
        ICanvasBitmap* bitmap,
        Vector2* offset,
        Rect* destinationRect,
        Rect* sourceRect,
        float opacity,
        CanvasImageInterpolation interpolation,
        Numerics::Matrix4x4* perspective)
    {        
        return ExceptionBoundary([&]
        {
            auto& deviceContext = GetResource();
            CheckInPointer(bitmap);

            DrawImageWorker(GetDevice().Get(), deviceContext.Get(), offset, destinationRect, sourceRect, opacity, interpolation).DrawBitmap(bitmap, perspective);
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


    static bool ArePointsInsideBitmap(ID2D1Bitmap* bitmap, D2D1_POINT_2F const& point1, D2D1_POINT_2F const& point2, D2D1_UNIT_MODE unitMode)
    {
        D2D1_SIZE_F bitmapSize = GetBitmapSize(unitMode, bitmap);

        const float epsilon = 0.001f;

        return point1.x >= -epsilon &&
               point1.y >= -epsilon &&
               point2.x >= -epsilon &&
               point2.y >= -epsilon &&
               point1.x <= bitmapSize.width  + epsilon &&
               point1.y <= bitmapSize.height + epsilon &&
               point2.x <= bitmapSize.width  + epsilon &&
               point2.y <= bitmapSize.height + epsilon;
    }


    static bool TryGetFillOpacityMaskParameters(ID2D1Brush* opacityBrush, ID2D1DeviceContext1* deviceContext, D2D1_RECT_F const& destRect, ID2D1Bitmap** opacityBitmap, D2D1_RECT_F* opacitySourceRect)
    {
        // Is this a bitmap brush?
        auto bitmapBrush = MaybeAs<ID2D1BitmapBrush1>(opacityBrush);

        if (!bitmapBrush)
            return false;

        bitmapBrush->GetBitmap(opacityBitmap);
        
        if (!*opacityBitmap)
            return false;

        // Make sure the brush transform contains only positive scaling and translation, as other
        // transforms cannot be represented in FillOpacityMask sourceRect/destRect format.
        D2D1::Matrix3x2F brushTransform;
        bitmapBrush->GetTransform(&brushTransform);

        if (brushTransform._11 <= 0 ||
            brushTransform._22 <= 0 ||
            brushTransform._12 != 0 ||
            brushTransform._21 != 0)
        {
            return false;
        }

        // Transform the dest rect by the inverse of the brush transform, yielding a FillOpacityMask source rect.
        if (!D2D1InvertMatrix(&brushTransform))
            return false;

        auto tl = D2D1_POINT_2F{ destRect.left,  destRect.top    } * brushTransform;
        auto br = D2D1_POINT_2F{ destRect.right, destRect.bottom } * brushTransform;

        // Can't use FillOpacityMask if the source rect goes outside the bounds of the bitmap.
        if (!ArePointsInsideBitmap(*opacityBitmap, tl, br, deviceContext->GetUnitMode()))
            return false;

        // FillOpacityMask always uses default alpha and interpolation mode.
        if (bitmapBrush->GetOpacity() != 1.0f)
            return false;

        if (bitmapBrush->GetInterpolationMode1() != D2D1_BITMAP_INTERPOLATION_MODE_LINEAR)
            return false;

        // FillOpacityMask requires that antialiasing be disabled.
        if (deviceContext->GetAntialiasMode() != D2D1_ANTIALIAS_MODE_ALIASED)
            return false;

        // Ok then! FillOpacityMask is a go.
        *opacitySourceRect = D2D1_RECT_F{ tl.x, tl.y, br.x, br.y };

        return true;
    }


    IFACEMETHODIMP CanvasDrawingSession::FillRectangleWithBrushAndOpacityBrush(
        Rect rect,
        ICanvasBrush* brush,
        ICanvasBrush* opacityBrush)
    {
        return ExceptionBoundary(
            [&]
            {
                auto& deviceContext = GetResource();
                CheckInPointer(brush);

                auto d2dRect = ToD2DRect(rect);
                auto d2dBrush = ToD2DBrush(brush);
                auto d2dOpacityBrush = ToD2DBrush(opacityBrush);

                ComPtr<ID2D1Bitmap> opacityBitmap;
                D2D1_RECT_F opacitySourceRect;

                if (TryGetFillOpacityMaskParameters(d2dOpacityBrush.Get(), deviceContext.Get(), d2dRect, &opacityBitmap, &opacitySourceRect))
                {
                    // Fast path: we can use FillOpacityMask.
                    deviceContext->FillOpacityMask(opacityBitmap.Get(), d2dBrush.Get(), &d2dRect, &opacitySourceRect);
                }
                else
                {
                    // Slow path: if FillOpacityMask does not support the requested operation, we use
                    // a layer to apply the opacity brush (if any) and then draw a regular rectangle.
                    if (d2dOpacityBrush)
                    {
                        auto layerParameters = D2D1::LayerParameters1();
                        layerParameters.opacityBrush = d2dOpacityBrush.Get();

                        deviceContext->PushLayer(&layerParameters, nullptr);
                    }

                    deviceContext->FillRectangle(&d2dRect, d2dBrush.Get());

                    if (d2dOpacityBrush)
                    {
                        deviceContext->PopLayer();
                    }
                }
        });
    }


    IFACEMETHODIMP CanvasDrawingSession::FillRectangleAtCoordsWithBrushAndOpacityBrush(
        float x,
        float y,
        float w,
        float h,
        ICanvasBrush* brush,
        ICanvasBrush* opacityBrush)
    {
        return FillRectangleWithBrushAndOpacityBrush(
            Rect{ x, y, w, h },
            brush,
            opacityBrush);
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
        if (!format)
            format = GetDefaultTextFormat();

        auto formatInternal = As<ICanvasTextFormatInternal>(format);
        auto realizedFormat = formatInternal->GetRealizedTextFormat();
        auto drawTextOptions = formatInternal->GetDrawTextOptions();
        
        DrawTextImpl(text, rect, brush, realizedFormat.Get(), drawTextOptions);
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

        auto formatInternal = As<ICanvasTextFormatInternal>(format);
        auto drawTextOptions = formatInternal->GetDrawTextOptions();

        ComPtr<IDWriteTextFormat> realizedTextFormat;
        
        //
        // Drawing at a point only works if word wrapping is turned off.  We
        // don't want to modify the original format passed in (since DrawText is
        // conceptually a read-only operation and we want the same format to be
        // usable across multiple threads).  Instead we create a temporary clone
        // of the original with a different word wrapping setting.
        //
        
        CanvasWordWrapping wordWrapping;
        ThrowIfFailed(format->get_WordWrapping(&wordWrapping));

        if (wordWrapping == CanvasWordWrapping::NoWrap)
        {
            realizedTextFormat = formatInternal->GetRealizedTextFormat();
        }
        else
        {
            realizedTextFormat = formatInternal->GetRealizedTextFormatClone(CanvasWordWrapping::NoWrap);
        }

        DrawTextImpl(text, rect, brush, realizedTextFormat.Get(), drawTextOptions);
    }


    void CanvasDrawingSession::DrawTextImpl(
        HSTRING text,
        Rect const& rect,
        ID2D1Brush* brush,
        IDWriteTextFormat* realizedFormat,
        D2D1_DRAW_TEXT_OPTIONS drawTextOptions)
    {
        auto& deviceContext = GetResource();
        CheckInPointer(brush);

        uint32_t textLength;
        auto textBuffer = WindowsGetStringRawBuffer(text, &textLength);
        ThrowIfNullPointer(textBuffer, E_INVALIDARG);

        auto d2dRect = ToD2DRect(rect);

        deviceContext->DrawText(textBuffer, textLength, realizedFormat, &d2dRect, brush, drawTextOptions);
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
    // Text layout
    //

    IFACEMETHODIMP CanvasDrawingSession::DrawTextLayoutWithBrush(
        ICanvasTextLayout* textLayout,
        Numerics::Vector2 point,
        ICanvasBrush* brush)
    {
        return DrawTextLayoutAtCoordsWithBrush(
            textLayout,
            point.X,
            point.Y,
            brush);
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawTextLayoutAtCoordsWithBrush(
        ICanvasTextLayout* textLayout,
        float x,
        float y,
        ICanvasBrush* brush)
    {
        return ExceptionBoundary(
            [&]
            {
                auto& deviceContext = GetResource();
                CheckInPointer(textLayout);
                CheckInPointer(brush);

                CanvasDrawTextOptions drawTextOptions;
                ThrowIfFailed(textLayout->get_Options(&drawTextOptions));

                deviceContext->DrawTextLayout(
                    D2D1_POINT_2F{ x, y },
                    GetWrappedResource<IDWriteTextLayout>(textLayout).Get(),
                    ToD2DBrush(brush).Get(),
                    StaticCastAs<D2D1_DRAW_TEXT_OPTIONS>(drawTextOptions));
            });
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawTextLayoutWithColor(
        ICanvasTextLayout* textLayout,
        Numerics::Vector2 point,
        ABI::Windows::UI::Color color)
    {
        return DrawTextLayoutAtCoordsWithColor(
            textLayout,
            point.X,
            point.Y,
            color);
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawTextLayoutAtCoordsWithColor(
        ICanvasTextLayout* textLayout,
        float x,
        float y,
        ABI::Windows::UI::Color color)
    {
        return ExceptionBoundary(
            [&]
            {
                auto& deviceContext = GetResource();
                CheckInPointer(textLayout);

                CanvasDrawTextOptions drawTextOptions;
                ThrowIfFailed(textLayout->get_Options(&drawTextOptions));

                deviceContext->DrawTextLayout(
                    D2D1_POINT_2F{ x, y },
                    GetWrappedResource<IDWriteTextLayout>(textLayout).Get(),
                    GetColorBrush(color),
                    StaticCastAs<D2D1_DRAW_TEXT_OPTIONS>(drawTextOptions));
            });
    }

    
    //
    // DrawGeometry
    //

    IFACEMETHODIMP CanvasDrawingSession::DrawGeometryWithBrush(
        ICanvasGeometry* geometry,
        Vector2 offset,
        ICanvasBrush* brush)
    {
        return DrawGeometryWithBrushAndStrokeWidthAndStrokeStyle(
            geometry, 
            offset, 
            brush, 
            1.0f, 
            nullptr);
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawGeometryWithColor(
        ICanvasGeometry* geometry,
        Vector2 offset,
        ABI::Windows::UI::Color color)
    {
        return DrawGeometryWithColorAndStrokeWidthAndStrokeStyle(
            geometry,
            offset,
            color,
            1.0f,
            nullptr);
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawGeometryAtCoordsWithBrush(
        ICanvasGeometry* geometry,
        float x,
        float y,
        ICanvasBrush* brush)
    {
        return DrawGeometryWithBrushAndStrokeWidthAndStrokeStyle(
            geometry,
            Vector2{ x, y },
            brush,
            1.0f,
            nullptr);
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawGeometryAtCoordsWithColor(
        ICanvasGeometry* geometry,
        float x,
        float y,
        ABI::Windows::UI::Color color)
    {
        return DrawGeometryWithColorAndStrokeWidthAndStrokeStyle(
            geometry,
            Vector2{ x, y },
            color,
            1.0f,
            nullptr);
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawGeometryAtOriginWithBrush(
        ICanvasGeometry* geometry,
        ICanvasBrush* brush)
    {
        return DrawGeometryAtOriginWithBrushAndStrokeWidthAndStrokeStyle(
            geometry,
            brush, 
            1.0f, 
            nullptr);
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawGeometryAtOriginWithColor(
        ICanvasGeometry* geometry,
        Color color)
    {
        return DrawGeometryAtOriginWithColorAndStrokeWidthAndStrokeStyle(
            geometry,
            color, 
            1.0f, 
            nullptr);
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawGeometryWithBrushAndStrokeWidth(
        ICanvasGeometry* geometry,
        Vector2 offset,
        ICanvasBrush* brush,
        float strokeWidth)
    {
        return DrawGeometryWithBrushAndStrokeWidthAndStrokeStyle(
            geometry,
            offset,
            brush,
            strokeWidth,
            nullptr);
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawGeometryWithColorAndStrokeWidth(
        ICanvasGeometry* geometry,
        Vector2 offset,
        ABI::Windows::UI::Color color,
        float strokeWidth)
    {
        return DrawGeometryWithColorAndStrokeWidthAndStrokeStyle(
            geometry,
            offset,
            color,
            strokeWidth,
            nullptr);
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawGeometryAtCoordsWithBrushAndStrokeWidth(
        ICanvasGeometry* geometry,
        float x,
        float y,
        ICanvasBrush* brush,
        float strokeWidth)
    {
        return DrawGeometryWithBrushAndStrokeWidthAndStrokeStyle(
            geometry,
            Vector2{ x, y },
            brush,
            strokeWidth,
            nullptr);
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawGeometryAtCoordsWithColorAndStrokeWidth(
        ICanvasGeometry* geometry,
        float x,
        float y,
        ABI::Windows::UI::Color color,
        float strokeWidth)
    {
        return DrawGeometryWithColorAndStrokeWidthAndStrokeStyle(
            geometry,
            Vector2{ x, y },
            color,
            strokeWidth,
            nullptr);
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawGeometryAtOriginWithBrushAndStrokeWidth(
        ICanvasGeometry* geometry,
        ICanvasBrush* brush,
        float strokeWidth)
    {
        return DrawGeometryAtOriginWithBrushAndStrokeWidthAndStrokeStyle(
            geometry,
            brush, 
            strokeWidth, 
            nullptr);
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawGeometryAtOriginWithColorAndStrokeWidth(
        ICanvasGeometry* geometry,
        Color color,
        float strokeWidth)
    {
        return DrawGeometryAtOriginWithColorAndStrokeWidthAndStrokeStyle(
            geometry,
            color, 
            strokeWidth, 
            nullptr);
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawGeometryWithBrushAndStrokeWidthAndStrokeStyle(
        ICanvasGeometry* geometry,
        Vector2 offset,
        ICanvasBrush* brush,
        float strokeWidth,
        ICanvasStrokeStyle* strokeStyle)
    {
        return ExceptionBoundary(
            [&]
            {
                auto d2dBrush = ToD2DBrush(brush);

                TemporaryTransform<ID2D1DeviceContext1> transform(GetResource().Get(), offset);
                TemporaryTransform<ID2D1Brush> brushTransform(d2dBrush.Get(), Vector2{ -offset.X, -offset.Y }, true);

                DrawGeometryImpl(
                    geometry,
                    d2dBrush.Get(),
                    strokeWidth,
                    strokeStyle);
            });
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawGeometryWithColorAndStrokeWidthAndStrokeStyle(
        ICanvasGeometry* geometry,
        Vector2 offset,
        ABI::Windows::UI::Color color,
        float strokeWidth,
        ICanvasStrokeStyle* strokeStyle)
    {
        return ExceptionBoundary(
            [&]
            {
                TemporaryTransform<ID2D1DeviceContext1> transform(GetResource().Get(), offset);

                DrawGeometryImpl(
                    geometry,
                    GetColorBrush(color),
                    strokeWidth,
                    strokeStyle);
            });
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawGeometryAtCoordsWithBrushAndStrokeWidthAndStrokeStyle(
        ICanvasGeometry* geometry,
        float x,
        float y,
        ICanvasBrush* brush,
        float strokeWidth,
        ICanvasStrokeStyle* strokeStyle)
    {
        return DrawGeometryWithBrushAndStrokeWidthAndStrokeStyle(
            geometry,
            Vector2{ x, y },
            brush,
            strokeWidth,
            strokeStyle);
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawGeometryAtCoordsWithColorAndStrokeWidthAndStrokeStyle(
        ICanvasGeometry* geometry,
        float x,
        float y,
        ABI::Windows::UI::Color color,
        float strokeWidth,
        ICanvasStrokeStyle* strokeStyle)
    {
        return DrawGeometryWithColorAndStrokeWidthAndStrokeStyle(
            geometry,
            Vector2{ x, y },
            color,
            strokeWidth,
            strokeStyle);
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawGeometryAtOriginWithBrushAndStrokeWidthAndStrokeStyle(
        ICanvasGeometry* geometry,
        ICanvasBrush* brush,
        float strokeWidth,
        ICanvasStrokeStyle* strokeStyle)
    {
        return ExceptionBoundary(
            [&]
            {
                DrawGeometryImpl(
                    geometry,
                    ToD2DBrush(brush).Get(),
                    strokeWidth,
                    strokeStyle);
            });
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawGeometryAtOriginWithColorAndStrokeWidthAndStrokeStyle(
        ICanvasGeometry* geometry,
        Color color,
        float strokeWidth,
        ICanvasStrokeStyle* strokeStyle)
    {
        return ExceptionBoundary(
            [&]
            {
                DrawGeometryImpl(
                    geometry,
                    GetColorBrush(color),
                    strokeWidth,
                    strokeStyle);
            });
    }


    void CanvasDrawingSession::DrawGeometryImpl(
        ICanvasGeometry* geometry,
        ID2D1Brush* brush,
        float strokeWidth,
        ICanvasStrokeStyle* strokeStyle)
    {
        auto& deviceContext = GetResource();
        CheckInPointer(geometry);
        CheckInPointer(brush);

        deviceContext->DrawGeometry(
            GetWrappedResource<ID2D1Geometry>(geometry).Get(),
            brush,
            strokeWidth,
            ToD2DStrokeStyle(strokeStyle, deviceContext.Get()).Get());
    }


    //
    // FillGeometry
    //

    IFACEMETHODIMP CanvasDrawingSession::FillGeometryWithBrush(
        ICanvasGeometry* geometry,
        Vector2 offset,
        ICanvasBrush* brush)
    {
        return FillGeometryWithBrushAndOpacityBrush(
            geometry,
            offset,
            brush,
            nullptr);
    }


    IFACEMETHODIMP CanvasDrawingSession::FillGeometryWithBrushAndOpacityBrush(
        ICanvasGeometry* geometry,
        Vector2 offset,
        ICanvasBrush* brush,
        ICanvasBrush* opacityBrush)
    {
        return ExceptionBoundary(
            [&]
            {
                auto d2dBrush = ToD2DBrush(brush);
                auto d2dOpacityBrush = ToD2DBrush(opacityBrush);

                Vector2 inverseOffset{ -offset.X, -offset.Y };

                TemporaryTransform<ID2D1DeviceContext1> transform(GetResource().Get(), offset);
                TemporaryTransform<ID2D1Brush> brushTransform(d2dBrush.Get(), inverseOffset, true);
                TemporaryTransform<ID2D1Brush> opacityBrushTransform(d2dOpacityBrush.Get(), inverseOffset, true);

                FillGeometryImpl(
                    geometry,
                    d2dBrush.Get(),
                    d2dOpacityBrush.Get());
            });
    }


    IFACEMETHODIMP CanvasDrawingSession::FillGeometryWithColor(
        ICanvasGeometry* geometry,
        Vector2 offset,
        ABI::Windows::UI::Color color)
    {
        return ExceptionBoundary(
            [&]
            {
                TemporaryTransform<ID2D1DeviceContext1> transform(GetResource().Get(), offset);

                FillGeometryImpl(
                    geometry,
                    GetColorBrush(color),
                    nullptr);
            });
    }


    IFACEMETHODIMP CanvasDrawingSession::FillGeometryAtCoordsWithBrush(
        ICanvasGeometry* geometry,
        float x,
        float y,
        ICanvasBrush* brush)
    {
        return FillGeometryWithBrushAndOpacityBrush(
            geometry,
            Vector2{ x, y },
            brush,
            nullptr);
    }


    IFACEMETHODIMP CanvasDrawingSession::FillGeometryAtCoordsWithBrushAndOpacityBrush(
        ICanvasGeometry* geometry,
        float x,
        float y,
        ICanvasBrush* brush,
        ICanvasBrush* opacityBrush)
    {
        return FillGeometryWithBrushAndOpacityBrush(
            geometry,
            Vector2{ x, y },
            brush,
            opacityBrush);
    }


    IFACEMETHODIMP CanvasDrawingSession::FillGeometryAtCoordsWithColor(
        ICanvasGeometry* geometry,
        float x,
        float y,
        ABI::Windows::UI::Color color)
    {
        return FillGeometryWithColor(
            geometry,
            Vector2{ x, y },
            color);
    }


    IFACEMETHODIMP CanvasDrawingSession::FillGeometryAtOriginWithBrush(
        ICanvasGeometry* geometry,
        ICanvasBrush* brush)
    {
        return FillGeometryAtOriginWithBrushAndOpacityBrush(
            geometry,
            brush,
            nullptr);
    }


    IFACEMETHODIMP CanvasDrawingSession::FillGeometryAtOriginWithBrushAndOpacityBrush(
        ICanvasGeometry* geometry,
        ICanvasBrush* brush,
        ICanvasBrush* opacityBrush)
    {
        return ExceptionBoundary(
            [&]
            {
                FillGeometryImpl(
                    geometry,
                    ToD2DBrush(brush).Get(),
                    ToD2DBrush(opacityBrush).Get());
            });
    }


    IFACEMETHODIMP CanvasDrawingSession::FillGeometryAtOriginWithColor(
        ICanvasGeometry* geometry,
        Color color)
    {
        return ExceptionBoundary(
            [&]
            {
                FillGeometryImpl(
                    geometry,
                    GetColorBrush(color),
                    nullptr);
            });
    }


    static bool IsBitmapBrushWithClampExtendMode(ID2D1Brush* brush)
    {
        auto bitmapBrush = MaybeAs<ID2D1BitmapBrush1>(brush);

        return bitmapBrush &&
               bitmapBrush->GetExtendModeX() == D2D1_EXTEND_MODE_CLAMP &&
               bitmapBrush->GetExtendModeY() == D2D1_EXTEND_MODE_CLAMP;
    }


    void CanvasDrawingSession::FillGeometryImpl(
        ICanvasGeometry* geometry,
        ID2D1Brush* brush,
        ID2D1Brush* opacityBrush)
    {
        auto& deviceContext = GetResource();
        CheckInPointer(geometry);
        CheckInPointer(brush);

        auto d2dGeometry = GetWrappedResource<ID2D1Geometry>(geometry);

        if (!opacityBrush || IsBitmapBrushWithClampExtendMode(brush))
        {
            // Fast path: if there is no opacity brush, or if our color brush is
            // a clamped bitmap, D2D can fill the geometry directly in a single call.
            deviceContext->FillGeometry(
                d2dGeometry.Get(),
                brush,
                opacityBrush);
        }
        else
        {
            // Slow path: if FillGeometry does not directly support the requested
            // operation, use a layer to apply the opacity brush instead.
            auto layerParameters = D2D1::LayerParameters1();
            layerParameters.opacityBrush = opacityBrush;

            deviceContext->PushLayer(&layerParameters, nullptr);

            deviceContext->FillGeometry(
                d2dGeometry.Get(),
                brush,
                nullptr);

            deviceContext->PopLayer();
        }
    }


    //
    // DrawCachedGeometry
    //

    IFACEMETHODIMP CanvasDrawingSession::DrawCachedGeometryWithBrush(
        ICanvasCachedGeometry* cachedGeometry,
        Vector2 offset,
        ICanvasBrush* brush)
    {
        return ExceptionBoundary(
            [&]
            {
                auto d2dBrush = ToD2DBrush(brush);

                TemporaryTransform<ID2D1DeviceContext1> transform(GetResource().Get(), offset);
                TemporaryTransform<ID2D1Brush> brushTransform(d2dBrush.Get(), Vector2{ -offset.X, -offset.Y }, true);

                DrawCachedGeometryImpl(
                    cachedGeometry,
                    d2dBrush.Get());
            });
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawCachedGeometryWithColor(
        ICanvasCachedGeometry* cachedGeometry,
        Vector2 offset,
        ABI::Windows::UI::Color color)
    {
        return ExceptionBoundary(
            [&]
            {
                TemporaryTransform<ID2D1DeviceContext1> transform(GetResource().Get(), offset);

                DrawCachedGeometryImpl(
                    cachedGeometry,
                    GetColorBrush(color));
            });
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawCachedGeometryAtCoordsWithBrush(
        ICanvasCachedGeometry* cachedGeometry,
        float x,
        float y,
        ICanvasBrush* brush)
    {
        return DrawCachedGeometryWithBrush(
            cachedGeometry,
            Vector2{ x, y },
            brush);
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawCachedGeometryAtCoordsWithColor(
        ICanvasCachedGeometry* cachedGeometry,
        float x,
        float y,
        ABI::Windows::UI::Color color)
    {
        return DrawCachedGeometryWithColor(
            cachedGeometry,
            Vector2{ x, y },
            color);
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawCachedGeometryAtOriginWithBrush(
        ICanvasCachedGeometry* cachedGeometry,
        ICanvasBrush* brush)
    {
        return ExceptionBoundary(
            [&]
            {
                DrawCachedGeometryImpl(
                    cachedGeometry,
                    ToD2DBrush(brush).Get());
            });
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawCachedGeometryAtOriginWithColor(
        ICanvasCachedGeometry* cachedGeometry,
        Color color)
    {
        return ExceptionBoundary(
            [&]
            {
                DrawCachedGeometryImpl(
                    cachedGeometry,
                    GetColorBrush(color));
            });
    }


    void CanvasDrawingSession::DrawCachedGeometryImpl(
        ICanvasCachedGeometry* cachedGeometry,
        ID2D1Brush* brush)
    {
        auto& deviceContext = GetResource();
        CheckInPointer(cachedGeometry);
        CheckInPointer(brush);

        deviceContext->DrawGeometryRealization(
            GetWrappedResource<ID2D1GeometryRealization>(cachedGeometry).Get(),
            brush);
    }

#if WINVER > _WIN32_WINNT_WINBLUE
    IFACEMETHODIMP CanvasDrawingSession::DrawInk(IIterable<InkStroke*>* inkStrokeCollection)
    {
        return ExceptionBoundary(
            [&]
            {
                if (!m_adapter)
                    ThrowHR(RO_E_CLOSED);

                DrawInkImpl(inkStrokeCollection, m_adapter->IsHighContrastEnabled());
            });
    }

    IFACEMETHODIMP CanvasDrawingSession::DrawInkWithHighContrast(
        IIterable<InkStroke*>* inkStrokeCollection,
        boolean highContrast)
    {
        return ExceptionBoundary(
            [&]
            {
                DrawInkImpl(inkStrokeCollection, !!highContrast);
            });
    }

    void CanvasDrawingSession::DrawInkImpl(
        IIterable<InkStroke*>* inkStrokeCollection,
        bool highContrast)
    {
        auto& deviceContext = GetResource();

        CheckInPointer(inkStrokeCollection);

        ComPtr<IUnknown> inkStrokeCollectionAsIUnknown = As<IUnknown>(inkStrokeCollection);

        if (!m_inkD2DRenderer)
        {
            m_inkD2DRenderer = m_adapter->CreateInkRenderer();
        }

        m_inkD2DRenderer->Draw(
            deviceContext.Get(), 
            inkStrokeCollectionAsIUnknown.Get(), 
            highContrast);
    }

    IFACEMETHODIMP CanvasDrawingSession::DrawGradientMeshAtOrigin(ICanvasGradientMesh* gradientMesh)
    {
        return ExceptionBoundary(
            [&]
            {
                auto& deviceContext = GetResource();
                auto deviceContext2 = As<ID2D1DeviceContext2>(deviceContext);

                CheckInPointer(gradientMesh);

                deviceContext2->DrawGradientMesh(
                    GetWrappedResource<ID2D1GradientMesh>(gradientMesh).Get());
            });
    }

    IFACEMETHODIMP CanvasDrawingSession::DrawGradientMesh(ICanvasGradientMesh* gradientMesh, Vector2 point)
    {
        return ExceptionBoundary(
            [&]
            {
                auto& deviceContext = GetResource();
                auto deviceContext2 = As<ID2D1DeviceContext2>(deviceContext);

                CheckInPointer(gradientMesh);
                
                TemporaryTransform<ID2D1DeviceContext1> transform(GetResource().Get(), point);

                deviceContext2->DrawGradientMesh(
                    GetWrappedResource<ID2D1GradientMesh>(gradientMesh).Get());
            });
    }

    IFACEMETHODIMP CanvasDrawingSession::DrawGradientMeshAtCoords(ICanvasGradientMesh* gradientMesh, float x, float y)
    {
        return ExceptionBoundary(
            [&]
            {
                auto& deviceContext = GetResource();
                auto deviceContext2 = As<ID2D1DeviceContext2>(deviceContext);

                CheckInPointer(gradientMesh);
                
                TemporaryTransform<ID2D1DeviceContext1> transform(GetResource().Get(), Vector2{ x, y });

                deviceContext2->DrawGradientMesh(
                    GetWrappedResource<ID2D1GradientMesh>(gradientMesh).Get());
            });
    }

#endif

    ID2D1SolidColorBrush* CanvasDrawingSession::GetColorBrush(Color const& color)
    {
        if (m_solidColorBrush)
        {
            m_solidColorBrush->SetColor(ToD2DColor(color));
        }
        else
        {
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

        return As<ICanvasBrushInternal>(brush)->GetD2DBrush(deviceContext.Get(), GetBrushFlags::None);
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

    IFACEMETHODIMP CanvasDrawingSession::get_TextRenderingParameters(ICanvasTextRenderingParameters** value)
    {
        return ExceptionBoundary(
            [&]
            {
                auto& deviceContext = GetResource();
                CheckAndClearOutPointer(value);

                ComPtr<IDWriteRenderingParams> dwriteRenderingParams;
                deviceContext->GetTextRenderingParams(&dwriteRenderingParams);

                if (dwriteRenderingParams)
                {
                    auto canvasTextRenderingParams = ResourceManager::GetOrCreate<ICanvasTextRenderingParameters>(dwriteRenderingParams.Get());

                    ThrowIfFailed(canvasTextRenderingParams.CopyTo(value));
                }
            });
    }

    IFACEMETHODIMP CanvasDrawingSession::put_TextRenderingParameters(ICanvasTextRenderingParameters* value)
    {
        return ExceptionBoundary(
            [&]
            {
                auto& deviceContext = GetResource();

                ComPtr<IDWriteRenderingParams> dwriteRenderingParams;
                if (value)
                {
                    dwriteRenderingParams = GetWrappedResource<IDWriteRenderingParams>(value);
                }

                deviceContext->SetTextRenderingParams(dwriteRenderingParams.Get());

            });
    }


    //
    // Converts the given offset from DIPs to the appropriate unit
    //
    static D2D1_POINT_2F GetOffsetInCorrectUnits(ID2D1DeviceContext1* deviceContext, D2D1_POINT_2F const& offset)
    {
        auto unitMode = deviceContext->GetUnitMode();

        if (unitMode == D2D1_UNIT_MODE_DIPS)
        {
            return offset;
        }
        else if (unitMode == D2D1_UNIT_MODE_PIXELS)
        {
            auto dpi = GetDpi(deviceContext);
            D2D1_POINT_2F adjustedOffset = {
                (float)DipsToPixels(offset.x, dpi, CanvasDpiRounding::Floor),
                (float)DipsToPixels(offset.y, dpi, CanvasDpiRounding::Floor)
            };
            return adjustedOffset;
        }
        else
        {
            assert(false);
            ThrowHR(E_UNEXPECTED);
        }
    }

    //
    // Gets the current transform from the given device context, stripping out
    // the current offset
    //
    static Matrix3x2 GetTransform(ID2D1DeviceContext1* deviceContext, D2D1_POINT_2F const& offset)
    {
        D2D1_MATRIX_3X2_F transform;
        deviceContext->GetTransform(&transform);

        // We assume that the currently set transform has the offset applied to
        // it, correctly set for the current unit mode.  We need to subtract
        // that from the transform before returning it.
        auto adjustedOffset = GetOffsetInCorrectUnits(deviceContext, offset);
        transform._31 -= adjustedOffset.x;
        transform._32 -= adjustedOffset.y;

        return *reinterpret_cast<ABI::Microsoft::Graphics::Canvas::Numerics::Matrix3x2*>(&transform);
    }

    //
    // Sets the transform on the given device context, applied the offset.
    //
    static void SetTransform(ID2D1DeviceContext1* deviceContext, D2D1_POINT_2F const& offset, Matrix3x2 const& matrix)
    {
        auto adjustedOffset = GetOffsetInCorrectUnits(deviceContext, offset);

        D2D1_MATRIX_3X2_F transform = *(ReinterpretAs<D2D1_MATRIX_3X2_F const*>(&matrix));
        transform._31 += adjustedOffset.x;
        transform._32 += adjustedOffset.y;

        deviceContext->SetTransform(transform);
    }

    IFACEMETHODIMP CanvasDrawingSession::get_Transform(ABI::Microsoft::Graphics::Canvas::Numerics::Matrix3x2* value)
    {
        return ExceptionBoundary(
            [&]
            {                               
                auto& deviceContext = GetResource();
                CheckInPointer(value);

                *value = GetTransform(deviceContext.Get(), m_offset);
            });
    }

    IFACEMETHODIMP CanvasDrawingSession::put_Transform(ABI::Microsoft::Graphics::Canvas::Numerics::Matrix3x2 value)
    {
        return ExceptionBoundary(
            [&]
            {
                auto& deviceContext = GetResource();

                SetTransform(deviceContext.Get(), m_offset, value);
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

                if (m_offset.x != 0 || m_offset.y != 0)
                {
                    auto transform = GetTransform(deviceContext.Get(), m_offset);
                    deviceContext->SetUnitMode(static_cast<D2D1_UNIT_MODE>(value));
                    SetTransform(deviceContext.Get(), m_offset, transform);
                }
                else
                {
                    deviceContext->SetUnitMode(static_cast<D2D1_UNIT_MODE>(value));
                }
            });
    }

    IFACEMETHODIMP CanvasDrawingSession::get_Device(ICanvasDevice** value)
    {
        using namespace ::Microsoft::WRL::Wrappers;

        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(value);

                ThrowIfFailed(GetDevice().CopyTo(value));
            });
    }

    ComPtr<ICanvasDevice> const& CanvasDrawingSession::GetDevice()
    {
        if (!m_owner)
        {
            auto& deviceContext = GetResource();

            ComPtr<ID2D1Device> d2dDevice;
            deviceContext->GetDevice(&d2dDevice);

            m_owner = ResourceManager::GetOrCreate<ICanvasDevice>(d2dDevice.Get());
        }

        return m_owner;
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

    IFACEMETHODIMP CanvasDrawingSession::ConvertDipsToPixels(float dips, CanvasDpiRounding dpiRounding, int* pixels)
    {
        return ExceptionBoundary(
            [&]
            {
                auto& deviceContext = GetResource();
                CheckInPointer(pixels);

                *pixels = DipsToPixels(dips, GetDpi(deviceContext), dpiRounding);
            });
    }


    //
    // CreateLayer
    //

    IFACEMETHODIMP CanvasDrawingSession::CreateLayerWithOpacity(
        float opacity,
        ICanvasActiveLayer** layer)
    {
        return CreateLayerImpl(opacity, nullptr, nullptr, nullptr, nullptr, CanvasLayerOptions::None, layer);
    }

    IFACEMETHODIMP CanvasDrawingSession::CreateLayerWithOpacityBrush(
        ICanvasBrush* opacityBrush,
        ICanvasActiveLayer** layer)
    {
        return CreateLayerImpl(1.0f, opacityBrush, nullptr, nullptr, nullptr, CanvasLayerOptions::None, layer);
    }

    IFACEMETHODIMP CanvasDrawingSession::CreateLayerWithOpacityAndClipRectangle(
        float opacity,
        Rect clipRectangle,
        ICanvasActiveLayer** layer)
    {
        return CreateLayerImpl(opacity, nullptr, &clipRectangle, nullptr, nullptr, CanvasLayerOptions::None, layer);
    }

    IFACEMETHODIMP CanvasDrawingSession::CreateLayerWithOpacityBrushAndClipRectangle(
        ICanvasBrush* opacityBrush,
        Rect clipRectangle,
        ICanvasActiveLayer** layer)
    {
        return CreateLayerImpl(1.0f, opacityBrush, &clipRectangle, nullptr, nullptr, CanvasLayerOptions::None, layer);
    }

    IFACEMETHODIMP CanvasDrawingSession::CreateLayerWithOpacityAndClipGeometry(
        float opacity,
        ICanvasGeometry* clipGeometry,
        ICanvasActiveLayer** layer)
    {
        return CreateLayerImpl(opacity, nullptr, nullptr, clipGeometry, nullptr, CanvasLayerOptions::None, layer);
    }

    IFACEMETHODIMP CanvasDrawingSession::CreateLayerWithOpacityBrushAndClipGeometry(
        ICanvasBrush* opacityBrush,
        ICanvasGeometry* clipGeometry,
        ICanvasActiveLayer** layer)
    {
        return CreateLayerImpl(1.0f, opacityBrush, nullptr, clipGeometry, nullptr, CanvasLayerOptions::None, layer);
    }

    IFACEMETHODIMP CanvasDrawingSession::CreateLayerWithOpacityAndClipGeometryAndTransform(
        float opacity,
        ICanvasGeometry* clipGeometry,
        Matrix3x2 geometryTransform,
        ICanvasActiveLayer** layer)
    {
        return CreateLayerImpl(opacity, nullptr, nullptr, clipGeometry, &geometryTransform, CanvasLayerOptions::None, layer);
    }

    IFACEMETHODIMP CanvasDrawingSession::CreateLayerWithOpacityBrushAndClipGeometryAndTransform(
        ICanvasBrush* opacityBrush,
        ICanvasGeometry* clipGeometry,
        Matrix3x2 geometryTransform,
        ICanvasActiveLayer** layer)
    {
        return CreateLayerImpl(1.0f, opacityBrush, nullptr, clipGeometry, &geometryTransform, CanvasLayerOptions::None, layer);
    }

    IFACEMETHODIMP CanvasDrawingSession::CreateLayerWithAllOptions(
        float opacity,
        ICanvasBrush* opacityBrush,
        Rect clipRectangle,
        ICanvasGeometry* clipGeometry,
        Matrix3x2 geometryTransform,
        CanvasLayerOptions options,
        ICanvasActiveLayer** layer)
    {
        return CreateLayerImpl(opacity, opacityBrush, &clipRectangle, clipGeometry, &geometryTransform, options, layer);
    }

    IFACEMETHODIMP CanvasDrawingSession::DrawGlyphRun(
        Vector2 point,
        ICanvasFontFace* fontFace,
        float fontSize,
        uint32_t glyphCount,
        CanvasGlyph* glyphs,
        boolean isSideways,
        uint32_t bidiLevel,
        ICanvasBrush* brush)
    {
        return DrawGlyphRunWithMeasuringModeAndDescription(
            point,
            fontFace,
            fontSize,
            glyphCount,
            glyphs,
            isSideways,
            bidiLevel,
            brush,
            CanvasTextMeasuringMode::Natural,
            nullptr,
            nullptr,
            0,
            0,
            0);
    }

    IFACEMETHODIMP CanvasDrawingSession::DrawGlyphRunWithMeasuringMode(
        Vector2 point,
        ICanvasFontFace* fontFace,
        float fontSize,
        uint32_t glyphCount,
        CanvasGlyph* glyphs,
        boolean isSideways,
        uint32_t bidiLevel,
        ICanvasBrush* brush,
        CanvasTextMeasuringMode textMeasuringMode)
    {
        return DrawGlyphRunWithMeasuringModeAndDescription(
            point,
            fontFace,
            fontSize,
            glyphCount,
            glyphs,
            isSideways,
            bidiLevel,
            brush,
            textMeasuringMode,
            nullptr,
            nullptr,
            0,
            0,
            0);
    }

    IFACEMETHODIMP CanvasDrawingSession::DrawGlyphRunWithMeasuringModeAndDescription(
        Vector2 point,
        ICanvasFontFace* fontFace,
        float fontSize,
        uint32_t glyphCount,
        CanvasGlyph* glyphs,
        boolean isSideways,
        uint32_t bidiLevel,
        ICanvasBrush* brush,
        CanvasTextMeasuringMode textMeasuringMode,
        HSTRING localeName,
        HSTRING text,
        uint32_t clusterMapIndicesCount,
        int* clusterMapIndices,
        uint32_t textPosition)
    {
        return ExceptionBoundary(
            [&]
            {
                auto& deviceContext = GetResource();

                CheckInPointer(fontFace);
                CheckInPointer(glyphs);

                DrawGlyphRunHelper helper(
                    fontFace, 
                    fontSize, 
                    glyphCount, 
                    glyphs, 
                    isSideways, 
                    bidiLevel, 
                    brush, 
                    textMeasuringMode, 
                    localeName, 
                    text, 
                    clusterMapIndicesCount, 
                    clusterMapIndices, 
                    textPosition,
                    deviceContext);

                auto d2dBrush = MaybeAs<ID2D1Brush>(helper.ClientDrawingEffect);

                deviceContext->DrawGlyphRun(
                    ToD2DPoint(point),
                    &helper.DWriteGlyphRun,
                    &helper.DWriteGlyphRunDescription,
                    d2dBrush.Get(),
                    helper.MeasuringMode);
            });
    }

    // Returns true if the current transform matrix contains only scaling and translation, but no rotation or skew.
    static bool TransformIsAxisPreserving(ID2D1DeviceContext* deviceContext)
    {
        D2D1_MATRIX_3X2_F transform;

        deviceContext->GetTransform(&transform);

        return transform._12 == 0.0f &&
               transform._21 == 0.0f;
    }

    HRESULT CanvasDrawingSession::CreateLayerImpl(
        float opacity,
        ICanvasBrush* opacityBrush,
        Rect const* clipRectangle,
        ICanvasGeometry* clipGeometry,
        Matrix3x2 const* geometryTransform,
        CanvasLayerOptions options,
        ICanvasActiveLayer** layer)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckAndClearOutPointer(layer);

                auto& deviceContext = GetResource();

                // Convert the layer parameters to D2D format.
                auto d2dBrush = ToD2DBrush(opacityBrush);
                auto d2dRect = clipRectangle ? ToD2DRect(*clipRectangle) : D2D1::InfiniteRect();
                auto d2dGeometry = clipGeometry ? GetWrappedResource<ID2D1Geometry>(clipGeometry) : nullptr;
                auto d2dMatrix = geometryTransform ? *ReinterpretAs<D2D1_MATRIX_3X2_F const*>(geometryTransform) : D2D1::Matrix3x2F::Identity();
                auto d2dAntialiasMode = deviceContext->GetAntialiasMode();

                // Simple cases can be optimized to use PushAxisAlignedClip instead of PushLayer.
                bool isAxisAlignedClip = clipRectangle &&
                                         !d2dBrush &&
                                         !d2dGeometry &&
                                         opacity == 1.0f &&
                                         options == CanvasLayerOptions::None &&
                                         TransformIsAxisPreserving(deviceContext.Get());

                // Store a unique ID, used for validation in PopLayer. This extra state 
                // is needed because the D2D PopLayer method always just pops the topmost 
                // layer, but we want to make sure our CanvasActiveLayer objects are 
                // closed in the right order if there is nesting.
                //
                // Unlike most places where we stash extra state in a resource wrapper 
                // type, this does not break interop, because our IClosable based layer 
                // API already prevents cross-API push and pop of layers. You can do 
                // interop in code using layers, but cannot push from one side of the 
                // interop boundary and then pop from the other, which is what would 
                // break this tracking were it possible.

                int layerId = ++m_nextLayerId;

                m_activeLayerIds.push_back(layerId);

                // Construct a scope object that will pop the layer when its Close method is called.
                WeakRef weakSelf = AsWeak(this);

                auto activeLayer = Make<CanvasActiveLayer>(
                    [weakSelf, layerId, isAxisAlignedClip]() mutable
                    {
                        auto strongSelf = LockWeakRef<ICanvasDrawingSession>(weakSelf);
                        auto self = static_cast<CanvasDrawingSession*>(strongSelf.Get());

                        if (self)
                            self->PopLayer(layerId, isAxisAlignedClip);
                    });

                CheckMakeResult(activeLayer);

                if (isAxisAlignedClip)
                {
                    // Tell D2D to push an axis aligned clip region.
                    deviceContext->PushAxisAlignedClip(&d2dRect, d2dAntialiasMode);
                }
                else
                {
                    // Tell D2D to push the layer.
                    D2D1_LAYER_PARAMETERS1 parameters =
                    {
                        d2dRect,
                        d2dGeometry.Get(),
                        d2dAntialiasMode,
                        d2dMatrix,
                        opacity,
                        d2dBrush.Get(),
                        static_cast<D2D1_LAYER_OPTIONS1>(options)
                    };

                    deviceContext->PushLayer(&parameters, nullptr);
                }

                ThrowIfFailed(activeLayer.CopyTo(layer));
            });
    }

    void CanvasDrawingSession::PopLayer(int layerId, bool isAxisAlignedClip)
    {
        auto& deviceContext = GetResource();

        assert(!m_activeLayerIds.empty());

        if (m_activeLayerIds.back() != layerId)
            ThrowHR(E_FAIL, Strings::PoppedWrongLayer);

        m_activeLayerIds.pop_back();

        if (isAxisAlignedClip)
        {
            deviceContext->PopAxisAlignedClip();
        }
        else
        {
            deviceContext->PopLayer();
        }
    }

}}}}
