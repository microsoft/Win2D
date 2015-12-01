// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

using Microsoft.Graphics.Canvas.Text;
using Microsoft.Graphics.Canvas.UI.Composition;
using System.Numerics;
using Windows.Foundation;
using Windows.Graphics.DirectX;
using Windows.UI;
using Windows.UI.Composition;

namespace CompositionExample
{
    class DrawingSurfaceRenderer
    {
        SpriteVisual drawingSurfaceVisual;
        CompositionDrawingSurface drawingSurface;

        int drawCount;

        public Visual Visual { get { return drawingSurfaceVisual; } }

        public Size Size { get { return drawingSurface.Size; } }

        public DrawingSurfaceRenderer(Compositor compositor, CompositionGraphicsDevice compositionGraphicsDevice)
        {
            drawingSurfaceVisual = compositor.CreateSpriteVisual();
            drawingSurface = compositionGraphicsDevice.CreateDrawingSurface(new Size(256, 256), DirectXPixelFormat.B8G8R8A8UIntNormalized, DirectXAlphaMode.Premultiplied);
            drawingSurfaceVisual.Brush = compositor.CreateSurfaceBrush(drawingSurface);
            DrawDrawingSurface();

            compositionGraphicsDevice.RenderingDeviceReplaced += CompositionGraphicsDevice_RenderingDeviceReplaced;
        }

        void CompositionGraphicsDevice_RenderingDeviceReplaced(CompositionGraphicsDevice sender, RenderingDeviceReplacedEventArgs args)
        {
            DrawDrawingSurface();
        }

        void DrawDrawingSurface()
        {
            ++drawCount;

            using (var ds = CanvasComposition.CreateDrawingSession(drawingSurface))
            {
                ds.Clear(Colors.Transparent);

                var rect = new Rect(new Point(2, 2), (drawingSurface.Size.ToVector2() - new Vector2(4, 4)).ToSize());

                ds.FillRoundedRectangle(rect, 15, 15, Colors.LightBlue);
                ds.DrawRoundedRectangle(rect, 15, 15, Colors.Gray, 2);

                ds.DrawText("This is a composition drawing surface", rect, Colors.Black, new CanvasTextFormat()
                {
                    FontFamily = "Comic Sans MS",
                    FontSize = 32,
                    WordWrapping = CanvasWordWrapping.WholeWord,
                    VerticalAlignment = CanvasVerticalAlignment.Center,
                    HorizontalAlignment = CanvasHorizontalAlignment.Center
                });

                ds.DrawText("Draws: " + drawCount, rect, Colors.Black, new CanvasTextFormat()
                {
                    FontSize = 10,
                    VerticalAlignment = CanvasVerticalAlignment.Bottom,
                    HorizontalAlignment = CanvasHorizontalAlignment.Center
                });
            }
        }
    }
}
