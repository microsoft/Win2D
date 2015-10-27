// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

using namespace Microsoft::Graphics::Canvas;
using namespace Windows::Foundation::Numerics;

namespace ExampleGallery
{
    namespace Direct3DInterop
    {
        namespace SpriteBatchPerformance
        {
            //
            // The sprite batch performance tests need to be able to run
            // multiple scenarios against the same set of sprites.
            //
            // However, we don't want to pay the price for passing this data
            // into the scenario runner as part of the scenario, so we call
            // AddSprite before we start timing to populate the runner.
            //
            // The runners do as little work as possible in AddSprite since the
            // time taken to process AddSprite is not counted towards the
            // scenario time.
            //
            public interface class IScenarioRunner
            {
                virtual void AddSprite(CanvasBitmap^ bitmap, float4 tint, float2 position, float rotation) = 0;
                virtual void RunScenario(CanvasDrawingSession^ drawingSession, CanvasSpriteSortMode sortMode) = 0;
            };

            struct Sprite
            {
                CanvasBitmap^ Bitmap;
                float4 Tint;
                float2 Position;
                float Rotation;
            };

            //
            // This runner uses Win2D via C++/CX to draw the sprites.
            //
            public ref class CppWin2DScenarioRunner sealed : public IScenarioRunner
            {
                std::vector<Sprite> m_sprites;

            public:
                virtual void AddSprite(CanvasBitmap^ bitmap, float4 tint, float2 position, float rotation)
                {
                    m_sprites.push_back(Sprite{ bitmap, tint, position, rotation });
                }

                virtual void RunScenario(CanvasDrawingSession^ drawingSession, CanvasSpriteSortMode sortMode)
                {
                    auto spriteBatch = drawingSession->CreateSpriteBatch(sortMode);
                    for (auto const& sprite : m_sprites)
                    {
                        spriteBatch->Draw(sprite.Bitmap, sprite.Position, sprite.Tint, float2::zero(), sprite.Rotation, float2::one(), CanvasSpriteFlip::None);
                    }
                    delete spriteBatch;
                }
            };

            
            //
            // This runner calls Direct2D's SpriteBatch APIs directly.
            //
            public ref class Direct2DScenarioRunner sealed : public IScenarioRunner
            {
                std::vector<Sprite> m_sprites;
                
            public:
                virtual void AddSprite(CanvasBitmap^ bitmap, float4 tint, float2 position, float rotation)
                {
                    m_sprites.push_back(Sprite{ bitmap, tint, position, rotation });
                }

                virtual void RunScenario(CanvasDrawingSession^ drawingSession, CanvasSpriteSortMode sortMode)
                {
                    using namespace Microsoft::WRL;

                    UNREFERENCED_PARAMETER(sortMode); // sort mode is ignored

                    std::vector<ID2D1Bitmap*> bitmaps;
                    std::vector<D2D1_RECT_F> destRects;
                    std::vector<D2D1_COLOR_F> colors;
                    std::vector<D2D1_MATRIX_3X2_F> transforms;

                    for (auto const& sprite : m_sprites)
                    {
                        bitmaps.push_back(GetWrappedResource<ID2D1Bitmap>(sprite.Bitmap).Get());

                        auto size = sprite.Bitmap->Size;
                        destRects.push_back(D2D1_RECT_F{ 0, 0, size.Width, size.Height });
                        colors.push_back(D2D1_COLOR_F{ sprite.Tint.x, sprite.Tint.y, sprite.Tint.z, sprite.Tint.w });
                        transforms.push_back(D2D1::Matrix3x2F::Rotation(DirectX::XMConvertToDegrees(sprite.Rotation)) * D2D1::Matrix3x2F::Translation(sprite.Position.x, sprite.Position.y));
                    }

                    if (destRects.empty())
                        return;

                    auto deviceContext = GetWrappedResource<ID2D1DeviceContext3>(drawingSession);

                    auto oldAntialiasMode = deviceContext->GetAntialiasMode();
                    deviceContext->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);

                    ComPtr<ID2D1SpriteBatch> spriteBatch;
                    __abi_ThrowIfFailed(deviceContext->CreateSpriteBatch(&spriteBatch));

                    __abi_ThrowIfFailed(spriteBatch->AddSprites(
                        static_cast<uint32_t>(destRects.size()),
                        destRects.data(),
                        nullptr, // source rectangles
                        colors.data(),
                        transforms.data()));

                    uint32_t startIndex = 0;
                    uint32_t endIndex = 0;

                    ID2D1Bitmap* lastBitmap = bitmaps.front();

                    for (auto bitmap : bitmaps)
                    {
                        if (bitmap != lastBitmap)
                        {
                            deviceContext->DrawSpriteBatch(
                                spriteBatch.Get(),
                                startIndex,
                                endIndex - startIndex,
                                lastBitmap);

                            lastBitmap = bitmap;
                            startIndex = endIndex;
                        }

                        ++endIndex;
                    }

                    if (endIndex > startIndex)
                    {
                        deviceContext->DrawSpriteBatch(
                            spriteBatch.Get(),
                            startIndex,
                            endIndex - startIndex,
                            lastBitmap);
                    }

                    deviceContext->SetAntialiasMode(oldAntialiasMode);
                }
            };
        }
    }
}
