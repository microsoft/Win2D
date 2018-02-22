// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

using Microsoft.Graphics.Canvas;
using System;
using System.Numerics;
using System.Threading.Tasks;
using Windows.Foundation;

namespace ExampleGallery
{
    class SpriteSheet
    {
        CanvasBitmap bitmap;
        int spritesPerRow;
        Vector2 origin;

        public SpriteSheet(CanvasBitmap bitmap, Vector2 spriteSize, Vector2 origin)
        {
            this.bitmap = bitmap;
            SpriteSize = spriteSize;
            this.origin = origin;

            spritesPerRow = (int)(bitmap.Size.Width / spriteSize.X);
        }

        public Vector2 SpriteSize { get; private set; }

        public static async Task<SpriteSheet> LoadAsync(CanvasDevice device, string filename, Vector2 spriteSize, Vector2 origin)
        {
            return new SpriteSheet(await CanvasBitmap.LoadAsync(device, filename), spriteSize, origin);
        }

        public void Draw(CanvasSpriteBatch spriteBatch, int sprite, Vector2 position, Vector4 tint)
        {
            if (sprite < 0)
                return;

            spriteBatch.DrawFromSpriteSheet(bitmap, position, GetSourceRect(sprite), tint, origin, 0, Vector2.One, CanvasSpriteFlip.None);
        }

        Rect GetSourceRect(int sprite)
        {
            int row = sprite / spritesPerRow;
            int column = sprite % spritesPerRow;

            return new Rect(
                (float)(SpriteSize.X * column),
                (float)(SpriteSize.Y * row),
                (float)SpriteSize.X,
                (float)SpriteSize.Y);
        }
    }
}
