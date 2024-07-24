// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas
{
    class CanvasSpriteBatchStatics
        : public AgileActivationFactory<ICanvasSpriteBatchStatics>
    {
        InspectableClassStatic(RuntimeClass_Microsoft_Graphics_Canvas_CanvasSpriteBatch, BaseTrust);

    public:
        IFACEMETHODIMP IsSupported(ICanvasDevice* device, boolean* value) override;
    };

    
    class CanvasSpriteBatch
        : public RuntimeClass<ICanvasSpriteBatch, IClosable, ICanvasResourceCreator, ICanvasResourceCreatorWithDpi>
        , private LifespanTracker<CanvasSpriteBatch>
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_CanvasSpriteBatch, BaseTrust);

        ClosablePtr<ID2D1DeviceContext3> m_deviceContext;
        CanvasSpriteSortMode m_sortMode;
        D2D1_BITMAP_INTERPOLATION_MODE m_interpolationMode;
        D2D1_SPRITE_OPTIONS m_spriteOptions;
        D2D1_UNIT_MODE m_unitMode;
        
        struct Sprite
        {
            ComPtr<ID2D1Bitmap> Bitmap;
            D2D1_RECT_F DestinationRect;
            D2D1_RECT_U SourceRect;
            D2D1_COLOR_F Color;
            D2D1_MATRIX_3X2_F Transform;

            Sprite(
                ComPtr<ID2D1Bitmap>&& bitmap,
                D2D1_RECT_F const& destinationRect,
                D2D1_RECT_U const& sourceRect,
                Vector4 const& tint,
                Matrix3x2 const& transform)
                : Bitmap(std::move(bitmap))
                , DestinationRect(destinationRect)
                , SourceRect(sourceRect)
                , Color(*ReinterpretAs<D2D1_COLOR_F const*>(&tint))
                , Transform(*ReinterpretAs<D2D1_MATRIX_3X2_F const*>(&transform))
            {
            }

            Sprite(
                ComPtr<ID2D1Bitmap>&& bitmap,
                D2D1_RECT_F const& destinationRect,
                D2D1_RECT_U const& sourceRect,
                Vector4 const& tint)
                : Sprite(std::move(bitmap), destinationRect, sourceRect, tint, Identity3x2())
            {
            }
        };

        std::vector<Sprite> m_sprites;

    public:
        static Vector4 const DEFAULT_TINT;
        
        CanvasSpriteBatch(
            ComPtr<ID2D1DeviceContext3> const& deviceContext,
            CanvasSpriteSortMode sortMode,
            D2D1_BITMAP_INTERPOLATION_MODE interpolation,
            D2D1_SPRITE_OPTIONS options);

        ~CanvasSpriteBatch();

        //
        // ICanvasSpriteBatch
        //

        IFACEMETHODIMP DrawToRect( 
            ICanvasBitmap *bitmap,
            Rect destRect) override;
                        
        IFACEMETHODIMP DrawAtOffset( 
            ICanvasBitmap *bitmap,
            Vector2 offset) override;
                        
        IFACEMETHODIMP DrawWithTransform( 
            ICanvasBitmap *bitmap,
            Matrix3x2 transform) override;
                        
        IFACEMETHODIMP DrawToRectWithTint( 
            ICanvasBitmap *bitmap,
            Rect destRect,
            Vector4 tint) override;
                        
        IFACEMETHODIMP DrawAtOffsetWithTint( 
            ICanvasBitmap *bitmap,
            Vector2 offset,
            Vector4 tint) override;
                        
        IFACEMETHODIMP DrawWithTransformAndTint( 
            ICanvasBitmap *bitmap,
            Matrix3x2 transform,
            Vector4 tint) override;
                        
        IFACEMETHODIMP DrawToRectWithTintAndFlip( 
            ICanvasBitmap *bitmap,
            Rect destRect,
            Vector4 tint,
            CanvasSpriteFlip flip) override;

        IFACEMETHODIMP DrawWithTransformAndTintAndFlip(
            ICanvasBitmap *bitmap,
            Matrix3x2 transform,
            Vector4 tint,
            CanvasSpriteFlip flip) override;
                       
        IFACEMETHODIMP DrawAtOffsetWithTintAndTransform( 
            ICanvasBitmap *bitmap,
            Vector2 offset,
            Vector4 tint,
            Vector2 origin,
            float rotation,
            Vector2 scale,
            CanvasSpriteFlip flip) override;
                        
        IFACEMETHODIMP DrawFromSpriteSheetToRect( 
            ICanvasBitmap *bitmap,
            Rect destRect,
            Rect sourceRect) override;
                        
        IFACEMETHODIMP DrawFromSpriteSheetAtOffset( 
            ICanvasBitmap *bitmap,
            Vector2 offset,
            Rect sourceRect) override;
                        
        IFACEMETHODIMP DrawFromSpriteSheetWithTransform( 
            ICanvasBitmap *bitmap,
            Matrix3x2 transform,
            Rect sourceRect) override;
                        
        IFACEMETHODIMP DrawFromSpriteSheetToRectWithTint( 
            ICanvasBitmap *bitmap,
            Rect destRect,
            Rect sourceRect,
            Vector4 tint) override;
                        
        IFACEMETHODIMP DrawFromSpriteSheetAtOffsetWithTint( 
            ICanvasBitmap *bitmap,
            Vector2 offset,
            Rect sourceRect,
            Vector4 tint) override;
                        
        IFACEMETHODIMP DrawFromSpriteSheetWithTransformAndTint( 
            ICanvasBitmap *bitmap,
            Matrix3x2 transform,
            Rect sourceRect,
            Vector4 tint) override;
                        
        IFACEMETHODIMP DrawFromSpriteSheetToRectWithTintAndFlip( 
            ICanvasBitmap *bitmap,
            Rect destRect,
            Rect sourceRect,
            Vector4 tint,
            CanvasSpriteFlip flip) override;

        IFACEMETHODIMP DrawFromSpriteSheetWithTransformAndTintAndFlip(
            ICanvasBitmap *bitmap,
            Matrix3x2 transform,
            Rect sourceRect,
            Vector4 tint,
            CanvasSpriteFlip flip) override;
                        
        IFACEMETHODIMP DrawFromSpriteSheetAtOffsetWithTintAndTransform( 
            ICanvasBitmap *bitmap,
            Vector2 offset,
            Rect sourceRect,
            Vector4 tint,
            Vector2 origin,
            float rotation,
            Vector2 scale,
            CanvasSpriteFlip flip) override;

        //
        // IClosable
        //

        IFACEMETHODIMP Close() override;

        //
        // ICanvasResourceCreator
        //

        IFACEMETHODIMP get_Device( 
            ICanvasDevice** value) override;
        

        //
        // ICanvasResourceCreatorWithDpi
        //

        IFACEMETHODIMP get_Dpi( 
            float* dpi) override;
        
        IFACEMETHODIMP ConvertPixelsToDips( 
            int32_t pixels,
            float* dips) override;
        
        IFACEMETHODIMP ConvertDipsToPixels( 
            float dips,
            CanvasDpiRounding dpiRounding,
            int32_t* pixels) override;

    private:
        void EnsureNotClosed();
    };

} } } }

