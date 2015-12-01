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
        CancellationTokenSource drawLoopCancellationTokenSource;

        int drawCount;
        int deviceCount;

        volatile bool paused;

        public bool Paused
        {
            get { return paused; }
            set { paused = value; }
        }

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
            drawLoopCancellationTokenSource?.Cancel();
            swapChain?.Dispose();
        }

        public void SetDevice(CanvasDevice device, Size windowSize)
        {
            ++deviceCount;

            drawLoopCancellationTokenSource?.Cancel();

            swapChain = new CanvasSwapChain(device, 256, 256, 96);
            swapChainVisual.Brush = compositor.CreateSurfaceBrush(CanvasComposition.CreateCompositionSurfaceForSwapChain(compositor, swapChain));

            drawLoopCancellationTokenSource = new CancellationTokenSource();
            Task.Factory.StartNew(
                DrawLoop,
                drawLoopCancellationTokenSource.Token,
                TaskCreationOptions.LongRunning,
                TaskScheduler.Default);
        }

        void DrawLoop()
        {
            var canceled = drawLoopCancellationTokenSource.Token;

            try
            {
                // Tracking the previous pause state lets us draw once even after becoming paused,
                // so the label text can change to indicate the paused state.
                bool wasPaused = false;

                while (!canceled.IsCancellationRequested)
                {
                    bool isPaused = paused;

                    if (!isPaused || isPaused != wasPaused)
                    {
                        DrawSwapChain(swapChain, isPaused);
                    }
                    else
                    {
                        // A more sophisticated implementation might want to exit the draw loop when paused,
                        // but to keep things simple we just wait on vblank to yield CPU.
                        swapChain.WaitForVerticalBlank();
                    }

                    wasPaused = isPaused;
                }

                swapChain.Dispose();
            }
            catch (Exception e) when (swapChain.Device.IsDeviceLost(e.HResult))
            {
                swapChain.Device.RaiseDeviceLost();
            }
        }

        void DrawSwapChain(CanvasSwapChain swapChain, bool isPaused)
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

                var label = string.Format("Draws: {0}\nDevices: {1}\nTap to {2}", drawCount, deviceCount, isPaused ? "unpause" : "pause");

                ds.DrawText(label, rect, Colors.Black, new CanvasTextFormat()
                {
                    FontSize = 10,
                    VerticalAlignment = CanvasVerticalAlignment.Bottom,
                    HorizontalAlignment = CanvasHorizontalAlignment.Center
                });
            }

            swapChain.Present();
        }
    }
}
