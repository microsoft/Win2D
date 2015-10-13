// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

using Microsoft.Graphics.Canvas;
using Microsoft.Graphics.Canvas.Text;
using Microsoft.Graphics.Canvas.UI.Composition;
using System;
using System.Numerics;
using System.Threading;
using System.Threading.Tasks;
using Windows.Foundation;
using Windows.UI;
using Windows.UI.Composition;

namespace CompositionExample
{
    class SwapChainRenderer : IDisposable
    {
        Compositor compositor;
        CanvasSwapChain swapChain;
        SpriteVisual swapChainVisual;
        CancellationTokenSource swapChainDrawLoopCancellationTokenSource;
        int drawCount;

        public Visual Visual { get { return swapChainVisual; } }

        public Size Size
        {
            get
            {
                if (swapChain == null)
                    return new Size(0, 0);

                return swapChain.Size;
            }
        }

        public SwapChainRenderer(Compositor compositor)
        {
            this.compositor = compositor;
            swapChainVisual = compositor.CreateSpriteVisual();
        }

        public void Dispose()
        {
            swapChainDrawLoopCancellationTokenSource?.Cancel();
            swapChain?.Dispose();
        }

        public void SetDevice(CanvasDevice device, Size windowSize)
        {
            swapChainDrawLoopCancellationTokenSource?.Cancel();

            swapChain = new CanvasSwapChain(device, 256, 256, 96);
            swapChainVisual.Brush = compositor.CreateSurfaceBrush(CanvasComposition.CreateCompositionSurfaceForSwapChain(compositor, swapChain));

            swapChainDrawLoopCancellationTokenSource = new CancellationTokenSource();
            Task.Factory.StartNew(
                DrawLoop,
                swapChainDrawLoopCancellationTokenSource.Token,
                TaskCreationOptions.LongRunning,
                TaskScheduler.Default);
        }

        void DrawLoop()
        {
            var canceled = swapChainDrawLoopCancellationTokenSource.Token;

            try
            {
                while (!canceled.IsCancellationRequested)
                {
                    DrawSwapChain(swapChain);
                }

                swapChain.Dispose();
            }
            catch (Exception e) when (swapChain.Device.IsDeviceLost(e.HResult))
            {
                swapChain.Device.RaiseDeviceLost();
            }
        }

        void DrawSwapChain(CanvasSwapChain swapChain)
        {
            ++drawCount;

            using (var ds = swapChain.CreateDrawingSession(Colors.Transparent))
            {
                var size = swapChain.Size.ToVector2();
                var radius = (Math.Min(size.X, size.Y) / 2.0f) - 4.0f;

                var center = size / 2;

                ds.FillCircle(center, radius, Colors.LightGoldenrodYellow);
                ds.DrawCircle(center, radius, Colors.LightGray);

                double mu = (-drawCount / 50.0f);

                for (int i =0; i < 16; ++i)
                {
                    double a = mu + (i / 16.0) * Math.PI * 2;
                    var x = (float)Math.Sin(a);
                    var y = (float)Math.Cos(a);
                    ds.DrawLine(center, center + new Vector2(x, y) * radius, Colors.Black, 5);
                }

                var rectLength = Math.Sqrt(radius * radius * 2);

                ds.FillCircle(center, (float)rectLength / 2, Colors.LightGoldenrodYellow);

                var rect = new Rect(center.X - rectLength / 2, center.Y - rectLength / 2, rectLength, rectLength);

                ds.DrawText("This is a swap chain",
                    rect,
                    Colors.Black,
                    new CanvasTextFormat()
                    {
                        FontFamily = "Comic Sans MS",
                        FontSize = 24,
                        VerticalAlignment = CanvasVerticalAlignment.Center,
                        HorizontalAlignment = CanvasHorizontalAlignment.Center,
                        WordWrapping = CanvasWordWrapping.WholeWord,
                    });
            }

            swapChain.Present();
        }
    }
}
