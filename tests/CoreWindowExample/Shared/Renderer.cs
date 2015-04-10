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

using Microsoft.Graphics.Canvas;
using Microsoft.Graphics.Canvas.Brushes;
using Microsoft.Graphics.Canvas.Effects;
using Microsoft.Graphics.Canvas.Geometry;
using Microsoft.Graphics.Canvas.Text;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Numerics;
using Windows.Foundation;
using Windows.Graphics.Display;
using Windows.UI;
using Windows.UI.Core;

namespace CoreWindowExample
{
    class Renderer
    {
        CoreWindow window;
        CanvasDevice device;
        SwapChainManager swapChainManager;

        Dictionary<uint, Point> currentPointsInContact = new Dictionary<uint, Point>();

        CanvasRenderTarget[] accumulationBuffers = new CanvasRenderTarget[2];
        int currentBuffer = 0;

        HueRotationEffect inputEffect = new HueRotationEffect()
        {
            Angle = 0.5f
        };

        Transform2DEffect outputEffect;
        CanvasImageBrush imageBrush;

        int ticksSinceLastTouch = 0;


        public Renderer(CoreWindow window)
        {
            this.window = window;
            window.PointerPressed += window_PointerPressed;
            window.PointerMoved += window_PointerMoved;
            window.PointerReleased += window_PointerReleased;

            device = new CanvasDevice();
            swapChainManager = new SwapChainManager(window, device);
            
            var effect = new GaussianBlurEffect()
            {
                BlurAmount = 5,
                Source = inputEffect
            };

            outputEffect = new Transform2DEffect()
            {
                Source = effect
            };

            imageBrush = new CanvasImageBrush(device);
            imageBrush.Opacity = 0.99f;
        }

        void window_PointerPressed(CoreWindow sender, PointerEventArgs args)
        {
            UpdateIntermediatePoints(args);
        }

        void window_PointerMoved(CoreWindow sender, PointerEventArgs args)
        {
            UpdateIntermediatePoints(args);
        }

        void window_PointerReleased(CoreWindow sender, PointerEventArgs args)
        {
            currentPointsInContact.Remove(args.CurrentPoint.PointerId);
            UpdateIntermediatePoints(args);
        }

        void UpdateIntermediatePoints(PointerEventArgs args)
        {
            foreach (var point in args.GetIntermediatePoints())
            {
                if (point.IsInContact)
                {
                    currentPointsInContact[point.PointerId] = point.Position;
                }
                else
                {
                    currentPointsInContact.Remove(point.PointerId);
                }
            }
        }

        public void Render()
        {
            swapChainManager.EnsureMatchesWindow(window);

            // Update the accumulation buffer
            SwapAccumulationBuffers();
            EnsureCurrentBufferMatchesWindow();

            using (var geom = MakeGeometryForContactPoints())
            {
                using (var ds = FrontBuffer.CreateDrawingSession())
                {
                    ds.Clear(Colors.Transparent);

                    AccumulateBackBufferOntoFrontBuffer(ds);
                    DrawContactGeometry(ds, geom);
                }

                var swapChain = swapChainManager.SwapChain;

                using (var ds = swapChain.CreateDrawingSession(Colors.CornflowerBlue))
                {
                    ds.DrawImage(FrontBuffer);

                    DrawFaceInCenterOfGeometry(ds, geom);
                    DrawContactPoints(ds);
                    DrawInfo(ds);
                    DrawTitle(ds, swapChain.Size);
                }

                swapChain.Present();
            }
        }

        void SwapAccumulationBuffers()
        {
            currentBuffer = (currentBuffer + 1) % 2;
        }

        void EnsureCurrentBufferMatchesWindow()
        {
            var bounds = window.Bounds;
            Size windowSize = new Size(bounds.Width, bounds.Height);
            float dpi = DisplayInformation.GetForCurrentView().LogicalDpi;

            var buffer = accumulationBuffers[currentBuffer];

            if (buffer == null || SwapChainManager.SizeEqualsWithTolerance(buffer.Size, windowSize) || buffer.Dpi != dpi)
            {
                if (buffer != null)
                    buffer.Dispose();

                buffer = new CanvasRenderTarget(device, (float)windowSize.Width, (float)windowSize.Height, dpi);
                accumulationBuffers[currentBuffer] = buffer;
            }
        }

        CanvasRenderTarget FrontBuffer { get { return accumulationBuffers[currentBuffer]; } }

        CanvasRenderTarget BackBuffer { get { return accumulationBuffers[(currentBuffer + 1) % 2]; } }

        CanvasGeometry MakeGeometryForContactPoints()
        {
            if (currentPointsInContact.Count == 0)
            {
                return null;
            }
            else if (currentPointsInContact.Count <= 2)
            {
                return MakeCircleGeometry();
            }
            else
            {
                return MakeConvexGeometry();
            }
        }

        CanvasGeometry MakeCircleGeometry()
        {
            bool first = true;
            Rect bounds;

            foreach (var point in currentPointsInContact.Values)
            {
                if (first)
                {
                    bounds = new Rect(point, point);
                    first = false;
                }
                else
                {
                    bounds.Union(point);
                }
            }

            var radius = Math.Max(bounds.Width, bounds.Height) * 0.5;

            if (radius == 0)
                radius = 50;

            var center = new Vector2((float)(bounds.Left + bounds.Width * 0.5), (float)(bounds.Top + bounds.Height * 0.5));

            return CanvasGeometry.CreateCircle(device, center, (float)radius);
            
        }

        CanvasGeometry MakeConvexGeometry()
        {
            if (currentPointsInContact.Count == 0)
                return null;

            var hull = ConvexHull.Create(currentPointsInContact.Values.Select(x => x.ToVector2()));

            // Figure out where the center is
            var hullCenter = Vector2.Zero;
            foreach (var p in hull)
            {
                hullCenter += p;
            }
            hullCenter /= hull.Count;

            // Build the geometry, made up of arcs around the hull
            using (var builder = new CanvasPathBuilder(device))
            {
                builder.BeginFigure(hull[0]);

                for (int i = 1; i <= hull.Count; ++i)
                {
                    //
                    // i-2 --- i-1 ---  i  --- i+1
                    //  a  ---  b  ---  c  ---  d
                    //  
                    // The bezier is between i-1 and i.  We need to figure out the control points
                    // that make this smooth.

                    var a = hull[(i + hull.Count - 2) % hull.Count];
                    var b = hull[(i + hull.Count - 1) % hull.Count];
                    var c = hull[i % hull.Count];
                    var d = hull[(i + 1) % hull.Count];

                    var ab = Vector2.Normalize(b - a);
                    var bc = Vector2.Normalize(c - b);
                    var cp1 = b + Vector2.Normalize(ab + bc) * (c - b).Length() * 0.5f;

                    var cd = Vector2.Normalize(d - c);
                    var cp2 = c - Vector2.Normalize(bc + cd) * (c - b).Length() * 0.5f;

                    builder.AddCubicBezier(cp1, cp2, c);
                }

                builder.EndFigure(CanvasFigureLoop.Closed);

                return CanvasGeometry.CreatePath(builder);
            }
        }

        void AccumulateBackBufferOntoFrontBuffer(CanvasDrawingSession ds)
        {
            // If this is the first frame then there's no back buffer
            if (BackBuffer == null)
                return;

            inputEffect.Source = BackBuffer;

            // Adjust the scale, so that if the front and back buffer are different sizes (eg the window was resized) 
            // then the contents is scaled up as appropriate.
            var scaleX = FrontBuffer.Size.Width / BackBuffer.Size.Width;
            var scaleY = FrontBuffer.Size.Height / BackBuffer.Size.Height;

            var transform = Matrix3x2.CreateScale((float)scaleX, (float)scaleY);

            // we do a bit of extra scale for effect
            transform *= Matrix3x2.CreateScale(1.01f, 1.01f, FrontBuffer.Size.ToVector2() / 2);

            outputEffect.TransformMatrix = transform;

            imageBrush.Image = outputEffect;
            imageBrush.SourceRectangle = new Rect(0, 0, FrontBuffer.Size.Width, FrontBuffer.Size.Height);
            ds.FillRectangle(imageBrush.SourceRectangle.Value, imageBrush);
        }

        static void DrawContactGeometry(CanvasDrawingSession ds, CanvasGeometry geom)
        {
            if (geom == null)
                return;

            ds.FillGeometry(geom, Colors.Green);
            ds.DrawGeometry(geom, Colors.Blue, 5);
        }

        static void DrawFaceInCenterOfGeometry(CanvasDrawingSession ds, CanvasGeometry geom)
        {
            if (geom == null)
                return;

            var bounds = geom.ComputeBounds();
            var centerX = bounds.X + bounds.Width / 2;
            var centerY = bounds.Y + bounds.Height / 2;

            var center = new Vector2((float)centerX, (float)centerY);

            ds.DrawText("ಠ⌣ಠ", center, Colors.White,
                new CanvasTextFormat()
                {
                    FontSize = 36,
                    HorizontalAlignment = CanvasHorizontalAlignment.Center,
                    VerticalAlignment = CanvasVerticalAlignment.Center
                });
        }

        void DrawContactPoints(CanvasDrawingSession ds)
        {
            foreach (var entry in currentPointsInContact)
            {
                ds.DrawCircle(entry.Value.ToVector2(), 20, Colors.Red);
            }
        }

        void DrawInfo(CanvasDrawingSession ds)
        {
            var swapChain = swapChainManager.SwapChain;
            var size = swapChain.Size;

            var message = string.Format("{0:00}x{1:00} @{2:00}dpi", size.Width, size.Height, swapChain.Dpi);

            if (swapChain.Rotation != CanvasSwapChainRotation.None)
                message += " " + swapChain.Rotation.ToString();

            ds.DrawText(message, size.ToVector2(), Colors.White,
                new CanvasTextFormat()
                {
                    FontSize = 12,
                    HorizontalAlignment = CanvasHorizontalAlignment.Right,
                    VerticalAlignment = CanvasVerticalAlignment.Bottom
                });

            if (currentPointsInContact.Count == 0)
            {
                ticksSinceLastTouch++;
                if (ticksSinceLastTouch > 100)
                {
                    ds.DrawText("Touch the screen!", size.ToVector2() * 0.5f, Colors.White,
                        new CanvasTextFormat()
                        {
                            FontSize = 24,
                            HorizontalAlignment = CanvasHorizontalAlignment.Center,
                            VerticalAlignment = CanvasVerticalAlignment.Center
                        });
                }
            }
            else
            {
                ticksSinceLastTouch = 0;
            }
        }

        static void DrawTitle(CanvasDrawingSession ds, Size size)
        {
            ds.DrawText("Win2D CoreWindow Example", new Rect(new Point(0, 0), size), Colors.White,
                new CanvasTextFormat()
                {
                    FontSize = 28
                });
        }

        public void Trim()
        {
            device.Trim();
        }
    }
}
