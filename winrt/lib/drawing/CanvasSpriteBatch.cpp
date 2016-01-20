// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

#if WINVER > _WIN32_WINNT_WINBLUE

#include <WindowsNumerics.h>

#include "CanvasSpriteBatch.h"

using namespace ::Windows::Foundation::Numerics;

//
// CanvasSpriteBatchStatics implementation
//


ActivatableStaticOnlyFactory(CanvasSpriteBatchStatics);

IFACEMETHODIMP CanvasSpriteBatchStatics::IsSupported(ICanvasDevice* device, boolean* isSupported)
{
    return ExceptionBoundary([&]
    {
        CheckInPointer(device);
        CheckInPointer(isSupported);
        
        auto lease = As<ICanvasDeviceInternal>(device)->GetResourceCreationDeviceContext();
        auto deviceContext3 = MaybeAs<ID2D1DeviceContext3>(lease.Get());
        
        *isSupported = static_cast<bool>(deviceContext3);
    });
}


//
// CanvasSpriteBatch implementation
//


Vector4 const CanvasSpriteBatch::DEFAULT_TINT{ 1.0f, 1.0f, 1.0f, 1.0f };


static D2D1_RECT_F MakeDestRect(ComPtr<ID2D1Bitmap> const& d2dBitmap, Vector2 offset = Vector2{ 0, 0 })
{
    auto sizeInDips = d2dBitmap->GetSize();
    return D2D1_RECT_F{ offset.X, offset.Y, offset.X + sizeInDips.width, offset.Y + sizeInDips.height };
}


static D2D1_RECT_F MakeDestRect(Rect sourceRect, Vector2 offset = Vector2{ 0, 0 })
{
    return D2D1_RECT_F{ offset.X, offset.Y, offset.X + sourceRect.Width, offset.Y + sourceRect.Height };
}


static D2D1_RECT_U MakeSourceRect(CanvasSpriteFlip flip, uint32_t left, uint32_t top, uint32_t right, uint32_t bottom)
{
    switch (flip)
    {
    case CanvasSpriteFlip::None:       return D2D1_RECT_U{ left,  top,    right, bottom };
    case CanvasSpriteFlip::Horizontal: return D2D1_RECT_U{ right, top,    left,  bottom };
    case CanvasSpriteFlip::Vertical:   return D2D1_RECT_U{ left,  bottom, right, top    };
    case CanvasSpriteFlip::Both:       return D2D1_RECT_U{ right, bottom, left,  top    };
    default:
        assert(false);
        ThrowHR(E_UNEXPECTED);
    }
}


static D2D1_RECT_U MakeSourceRect(ComPtr<ID2D1Bitmap> const& d2dBitmap, CanvasSpriteFlip flip)
{
    auto sizeInPixels = d2dBitmap->GetPixelSize();
    return MakeSourceRect(flip, 0, 0, sizeInPixels.width, sizeInPixels.height);
}


static D2D1_RECT_U MakeSourceRect(CanvasSpriteFlip flip, D2D1_UNIT_MODE unitMode, ICanvasBitmap* bitmap, Rect sourceRect)
{
    float dpi = 96.0f;

    if (unitMode == D2D1_UNIT_MODE_DIPS)
        ThrowIfFailed(As<ICanvasResourceCreatorWithDpi>(bitmap)->get_Dpi(&dpi));
    
    auto sourceLeft   = DipsToPixels(sourceRect.X,      dpi, CanvasDpiRounding::Round);
    auto sourceTop    = DipsToPixels(sourceRect.Y,      dpi, CanvasDpiRounding::Round);
    auto sourceWidth  = DipsToPixels(sourceRect.Width,  dpi, CanvasDpiRounding::Round);
    auto sourceHeight = DipsToPixels(sourceRect.Height, dpi, CanvasDpiRounding::Round);

    auto sourceRight  = sourceLeft + sourceWidth;
    auto sourceBottom = sourceTop + sourceHeight;

    return MakeSourceRect(
        flip,
        static_cast<uint32_t>(std::max(0, sourceLeft)),
        static_cast<uint32_t>(std::max(0, sourceTop)),
        static_cast<uint32_t>(std::max(0, sourceRight)),
        static_cast<uint32_t>(std::max(0, sourceBottom)));
}


static float3x2 MakeTransform(Vector2 const& origin, float rotation, Vector2 const& scale, Vector2 const& offset)
{
    return
        make_float3x2_translation(-origin.X, -origin.Y) *
        make_float3x2_rotation(rotation) *
        make_float3x2_scale(scale) *
        make_float3x2_translation(offset);
}


CanvasSpriteBatch::CanvasSpriteBatch(
    ComPtr<ID2D1DeviceContext3> const& deviceContext,
    CanvasSpriteSortMode sortMode,
    D2D1_BITMAP_INTERPOLATION_MODE interpolation,
    D2D1_SPRITE_OPTIONS options)
    : m_deviceContext(deviceContext.Get())
    , m_sortMode(sortMode)
    , m_interpolationMode(interpolation)
    , m_spriteOptions(options)
    , m_unitMode(deviceContext->GetUnitMode())
{
    assert(m_sortMode == CanvasSpriteSortMode::None
        || m_sortMode == CanvasSpriteSortMode::Bitmap);
    
    assert(m_interpolationMode == D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR
        || m_interpolationMode == D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);

    assert(m_spriteOptions == D2D1_SPRITE_OPTIONS_NONE
        || m_spriteOptions == D2D1_SPRITE_OPTIONS_CLAMP_TO_SOURCE_RECTANGLE);
}


CanvasSpriteBatch::~CanvasSpriteBatch()
{
    (void)Close();
}


IFACEMETHODIMP CanvasSpriteBatch::DrawToRect( 
    ICanvasBitmap* bitmap,
    Rect destRect)
{
    return DrawToRectWithTint(bitmap, destRect, DEFAULT_TINT);
}


IFACEMETHODIMP CanvasSpriteBatch::DrawAtOffset( 
    ICanvasBitmap* bitmap,
    Vector2 offset)
{
    return DrawAtOffsetWithTint(bitmap, offset, DEFAULT_TINT);
}


IFACEMETHODIMP CanvasSpriteBatch::DrawWithTransform( 
    ICanvasBitmap* bitmap,
    Matrix3x2 transform)
{
    return DrawWithTransformAndTint(bitmap, transform, DEFAULT_TINT);
}


IFACEMETHODIMP CanvasSpriteBatch::DrawToRectWithTint( 
    ICanvasBitmap* bitmap,
    Rect destRect,
    Vector4 tint)
{
    return DrawToRectWithTintAndFlip(bitmap, destRect, tint, CanvasSpriteFlip::None);
}


IFACEMETHODIMP CanvasSpriteBatch::DrawAtOffsetWithTint( 
    ICanvasBitmap* bitmap,
    Vector2 offset,
    Vector4 tint)
{
    return ExceptionBoundary([&]
    {
        CheckInPointer(bitmap);
        EnsureNotClosed();
        
        auto d2dBitmap = GetWrappedResource<ID2D1Bitmap>(bitmap);
        auto d2dDestRect = MakeDestRect(d2dBitmap, offset);
        auto d2dSourceRect = MakeSourceRect(d2dBitmap, CanvasSpriteFlip::None);
        
        m_sprites.emplace_back(
            std::move(d2dBitmap),
            d2dDestRect,
            d2dSourceRect,
            tint);
    });
}


IFACEMETHODIMP CanvasSpriteBatch::DrawWithTransformAndTint( 
    ICanvasBitmap* bitmap,
    Matrix3x2 transform,
    Vector4 tint)
{
    return DrawWithTransformAndTintAndFlip(bitmap, transform, tint, CanvasSpriteFlip::None);
}


IFACEMETHODIMP CanvasSpriteBatch::DrawToRectWithTintAndFlip( 
    ICanvasBitmap* bitmap,
    Rect destRect,
    Vector4 tint,
    CanvasSpriteFlip flip)
{
    return ExceptionBoundary([&]
    {
        CheckInPointer(bitmap);
        EnsureNotClosed();
        
        auto d2dBitmap = GetWrappedResource<ID2D1Bitmap>(bitmap);
        auto d2dSourceRect = MakeSourceRect(d2dBitmap, flip);
        
        m_sprites.emplace_back(
            std::move(d2dBitmap),
            ToD2DRect(destRect),
            d2dSourceRect,
            tint);
    });
}


IFACEMETHODIMP CanvasSpriteBatch::DrawWithTransformAndTintAndFlip(
    ICanvasBitmap* bitmap,
    Matrix3x2 transform,
    Vector4 tint,
    CanvasSpriteFlip flip)
{
    return ExceptionBoundary([&]
    {
        CheckInPointer(bitmap);
        EnsureNotClosed();
        
        auto d2dBitmap = GetWrappedResource<ID2D1Bitmap>(bitmap);
        auto d2dDestRect = MakeDestRect(d2dBitmap);
        auto d2dSourceRect = MakeSourceRect(d2dBitmap, flip);

        m_sprites.emplace_back(
            std::move(d2dBitmap),
            d2dDestRect,
            d2dSourceRect,
            tint,
            transform);
    });
}


IFACEMETHODIMP CanvasSpriteBatch::DrawAtOffsetWithTintAndTransform( 
    ICanvasBitmap* bitmap,
    Vector2 offset,
    Vector4 tint,
    Vector2 origin,
    float rotation,
    Vector2 scale,
    CanvasSpriteFlip flip)
{
    return ExceptionBoundary([&]
    {
        CheckInPointer(bitmap);
        EnsureNotClosed();
        
        auto d2dBitmap = GetWrappedResource<ID2D1Bitmap>(bitmap);
        auto d2dDestRect = MakeDestRect(d2dBitmap);
        auto d2dSourceRect = MakeSourceRect(d2dBitmap, flip);
        auto transform = MakeTransform(origin, rotation, scale, offset);

        m_sprites.emplace_back(
            std::move(d2dBitmap),
            d2dDestRect,
            d2dSourceRect,
            tint,
            transform);
    });
}


IFACEMETHODIMP CanvasSpriteBatch::DrawFromSpriteSheetToRect( 
    ICanvasBitmap* bitmap,
    Rect destRect,
    Rect sourceRect)
{
    return DrawFromSpriteSheetToRectWithTint(bitmap, destRect, sourceRect, DEFAULT_TINT);
}


IFACEMETHODIMP CanvasSpriteBatch::DrawFromSpriteSheetAtOffset( 
    ICanvasBitmap* bitmap,
    Vector2 offset,
    Rect sourceRect)
{
    return DrawFromSpriteSheetAtOffsetWithTint(bitmap, offset, sourceRect, DEFAULT_TINT);
}


IFACEMETHODIMP CanvasSpriteBatch::DrawFromSpriteSheetWithTransform( 
    ICanvasBitmap* bitmap,
    Matrix3x2 transform,
    Rect sourceRect)
{
    return DrawFromSpriteSheetWithTransformAndTint(bitmap, transform, sourceRect, DEFAULT_TINT);
}


IFACEMETHODIMP CanvasSpriteBatch::DrawFromSpriteSheetToRectWithTint( 
    ICanvasBitmap* bitmap,
    Rect destRect,
    Rect sourceRect,
    Vector4 tint)
{
    return DrawFromSpriteSheetToRectWithTintAndFlip(bitmap, destRect, sourceRect, tint, CanvasSpriteFlip::None);
}


IFACEMETHODIMP CanvasSpriteBatch::DrawFromSpriteSheetAtOffsetWithTint( 
    ICanvasBitmap* bitmap,
    Vector2 offset,
    Rect sourceRect,
    Vector4 tint)
{
    return ExceptionBoundary([&]
    {
        CheckInPointer(bitmap);
        EnsureNotClosed();
        
        auto d2dBitmap = GetWrappedResource<ID2D1Bitmap>(bitmap);
        auto d2dDestRect = MakeDestRect(sourceRect, offset);
        auto d2dSourceRect = MakeSourceRect(CanvasSpriteFlip::None, m_unitMode, bitmap, sourceRect);
        
        m_sprites.emplace_back(
            std::move(d2dBitmap),
            d2dDestRect,
            d2dSourceRect,
            tint);
    });
}


IFACEMETHODIMP CanvasSpriteBatch::DrawFromSpriteSheetWithTransformAndTint( 
    ICanvasBitmap* bitmap,
    Matrix3x2 transform,
    Rect sourceRect,
    Vector4 tint)
{
    return DrawFromSpriteSheetWithTransformAndTintAndFlip(bitmap, transform, sourceRect, tint, CanvasSpriteFlip::None);
}


IFACEMETHODIMP CanvasSpriteBatch::DrawFromSpriteSheetToRectWithTintAndFlip( 
    ICanvasBitmap* bitmap,
    Rect destRect,
    Rect sourceRect,
    Vector4 tint,
    CanvasSpriteFlip flip)
{
    return ExceptionBoundary([&]
    {
        CheckInPointer(bitmap);
        EnsureNotClosed();
        
        auto d2dBitmap = GetWrappedResource<ID2D1Bitmap>(bitmap);
        auto d2dSourceRect = MakeSourceRect(flip, m_unitMode, bitmap, sourceRect);
        
        m_sprites.emplace_back(
            std::move(d2dBitmap),
            ToD2DRect(destRect),
            d2dSourceRect,
            tint);
    });
}


IFACEMETHODIMP CanvasSpriteBatch::DrawFromSpriteSheetWithTransformAndTintAndFlip(
    ICanvasBitmap* bitmap,
    Matrix3x2 transform,
    Rect sourceRect,
    Vector4 tint,
    CanvasSpriteFlip flip)
{
    return ExceptionBoundary([&]
    {
        CheckInPointer(bitmap);
        EnsureNotClosed();
        
        auto d2dBitmap = GetWrappedResource<ID2D1Bitmap>(bitmap);
        auto d2dDestRect = MakeDestRect(sourceRect);
        auto d2dSourceRect = MakeSourceRect(flip, m_unitMode, bitmap, sourceRect);
        
        m_sprites.emplace_back(
            std::move(d2dBitmap),
            d2dDestRect,
            d2dSourceRect,
            tint,
            transform);
    });
}


IFACEMETHODIMP CanvasSpriteBatch::DrawFromSpriteSheetAtOffsetWithTintAndTransform( 
    ICanvasBitmap* bitmap,
    Vector2 offset,
    Rect sourceRect,
    Vector4 tint,
    Vector2 origin,
    float rotation,
    Vector2 scale,
    CanvasSpriteFlip flip)
{
    return ExceptionBoundary([&]
    {
        CheckInPointer(bitmap);
        EnsureNotClosed();
        
        auto d2dBitmap = GetWrappedResource<ID2D1Bitmap>(bitmap);
        auto d2dDestRect = MakeDestRect(sourceRect);
        auto d2dSourceRect = MakeSourceRect(flip, m_unitMode, bitmap, sourceRect);
        auto transform = MakeTransform(origin, rotation, scale, offset);

        m_sprites.emplace_back(
            std::move(d2dBitmap),
            d2dDestRect,
            d2dSourceRect,
            tint,
            transform);
    });
}


template<typename T>
class BatchFinder
{
    std::vector<T> const& m_sprites;
    uint32_t const m_maxSpritesPerBatch;

    uint32_t m_startIndex;
    uint32_t m_endIndex;
    ID2D1Bitmap* m_bitmap;

public:
    BatchFinder(std::vector<T> const& sprites, uint32_t maxSpritesPerBatch) noexcept
        : m_sprites(sprites)
        , m_maxSpritesPerBatch(maxSpritesPerBatch)
        , m_startIndex(0)
        , m_endIndex(0)
        , m_bitmap(nullptr)
    {
        FindNext();
    }

    void FindNext() noexcept
    {
        m_startIndex = m_endIndex;
        if (m_endIndex >= m_sprites.size())
        {
            m_bitmap = nullptr;
            return;
        }

        m_bitmap = m_sprites[m_endIndex].Bitmap.Get();

        for (; InCurrentBatch(); ++m_endIndex)
        {
            // nothing
        }
    }

    bool Done() const noexcept
    {
        return (m_bitmap == nullptr);
    }

    uint32_t CurrentStartIndex() const noexcept
    {
        return m_startIndex;
    }

    uint32_t CurrentSpriteCount() const noexcept
    {
        return m_endIndex - m_startIndex;
    }

    ID2D1Bitmap* CurrentBitmap() const noexcept
    {
        return m_bitmap;
    }


    BatchFinder(BatchFinder const&) = delete;
    BatchFinder& operator=(BatchFinder const&) = delete;

private:
    bool InCurrentBatch()
    {
        if (m_endIndex - m_startIndex >= m_maxSpritesPerBatch)
            return false;
        
        return m_endIndex != m_sprites.size() && m_sprites[m_endIndex].Bitmap.Get() == m_bitmap;
    }
};

IFACEMETHODIMP CanvasSpriteBatch::Close()
{
    return ExceptionBoundary([&]
    {
        auto deviceContext = m_deviceContext.Close();
        if (!deviceContext)
            return;

        if (m_sprites.empty()) // early out if there's nothing to draw
            return;

        //
        // Sort the sprites
        //
        
        if (m_sortMode == CanvasSpriteSortMode::Bitmap)
        {
            std::stable_sort(m_sprites.begin(), m_sprites.end(),
                [] (auto const& a, auto const& b)
                {
                    return a.Bitmap.Get() < b.Bitmap.Get();
                });
        }

        //
        // Build up a D2D sprite batch from our sprites
        //
        
        ComPtr<ID2D1SpriteBatch> spriteBatch;
        ThrowIfFailed(deviceContext->CreateSpriteBatch(&spriteBatch));

        assert(m_sprites.size() < std::numeric_limits<uint32_t>::max());

        auto firstSprite = &m_sprites.front();
        auto stride = static_cast<uint32_t>(sizeof(Sprite));

        ThrowIfFailed(spriteBatch->AddSprites(
            static_cast<uint32_t>(m_sprites.size()),
            &firstSprite->DestinationRect,
            &firstSprite->SourceRect,
            &firstSprite->Color,
            &firstSprite->Transform,
            stride,
            stride,
            stride,
            stride));

        //
        // Get the device context into the right state
        //
        
        auto originalAntialiasMode = deviceContext->GetAntialiasMode();

        if (originalAntialiasMode == D2D1_ANTIALIAS_MODE_PER_PRIMITIVE)
            deviceContext->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);

        auto originalUnitMode = deviceContext->GetUnitMode();
        if (originalUnitMode != m_unitMode)
            deviceContext->SetUnitMode(m_unitMode);

        //
        // Draw the sprites - one DrawSpriteBatch call for each bitmap
        //

        // Figure out if we need to quirk the batch size to workaround an issue
        // with older Qualcomm drivers.
        ComPtr<ID2D1Device> d2dDevice;
        deviceContext->GetDevice(&d2dDevice);
        auto device = ResourceManager::GetOrCreate<ICanvasDeviceInternal>(d2dDevice.Get());
        bool quirked = device->IsSpriteBatchQuirkRequired();
        uint32_t maxSpritesPerBatch = quirked ? 256 : std::numeric_limits<uint32_t>::max();
        
        for (BatchFinder<Sprite> batchFinder(m_sprites, maxSpritesPerBatch); !batchFinder.Done(); batchFinder.FindNext())
        {
            deviceContext->DrawSpriteBatch(
                spriteBatch.Get(),
                batchFinder.CurrentStartIndex(),
                batchFinder.CurrentSpriteCount(),
                batchFinder.CurrentBitmap(),
                m_interpolationMode,
                m_spriteOptions);

            if (quirked)
            {
                // Direct2D will helpfully batch up our DrawSpriteBatch calls - when
                // we're manually unbatching them to avoid limits of the maximum sprites per batch!
                // An explicit Flush here prevents that from happening.
                deviceContext->Flush();
            }
        }

        //
        // Restore the state we may have changed
        //

        if (originalUnitMode != m_unitMode)
            deviceContext->SetUnitMode(originalUnitMode);

        if (originalAntialiasMode == D2D1_ANTIALIAS_MODE_PER_PRIMITIVE)
            deviceContext->SetAntialiasMode(originalAntialiasMode);
    });
}


IFACEMETHODIMP CanvasSpriteBatch::get_Device(
    ICanvasDevice** value)
{
    return ExceptionBoundary([&]
    {
        CheckAndClearOutPointer(value);

        ComPtr<ID2D1Device> d2dDevice;
        m_deviceContext.EnsureNotClosed()->GetDevice(&d2dDevice);

        auto device = ResourceManager::GetOrCreate<ICanvasDevice>(d2dDevice.Get());

        ThrowIfFailed(device.CopyTo(value));
    });
}


IFACEMETHODIMP CanvasSpriteBatch::get_Dpi( 
    float* dpi)
{
    return ExceptionBoundary([&]
    {
        CheckInPointer(dpi);

        *dpi = GetDpi(m_deviceContext.EnsureNotClosed());
    });
}


IFACEMETHODIMP CanvasSpriteBatch::ConvertPixelsToDips( 
    int32_t pixels,
    float* dips)
{
    return ExceptionBoundary([&]
    {
        CheckInPointer(dips);
        
        *dips = PixelsToDips(pixels, GetDpi(m_deviceContext.EnsureNotClosed()));
    });      
}


IFACEMETHODIMP CanvasSpriteBatch::ConvertDipsToPixels( 
    float dips,
    CanvasDpiRounding dpiRounding,
    int32_t* pixels)
{
    return ExceptionBoundary([&]
    {
        CheckInPointer(pixels);

        *pixels = DipsToPixels(dips, GetDpi(m_deviceContext.EnsureNotClosed()), dpiRounding);
    });
}


void CanvasSpriteBatch::EnsureNotClosed()
{
    m_deviceContext.EnsureNotClosed();
}


#endif
