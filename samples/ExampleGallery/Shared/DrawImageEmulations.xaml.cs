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

#if WINDOWS_UAP
using Windows.Graphics.DirectX;
#else
using Microsoft.Graphics.Canvas.DirectX;
#endif

using Microsoft.Graphics.Canvas;
using Microsoft.Graphics.Canvas.Effects;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Numerics;
using System.Reflection;
using System.Threading.Tasks;
using Windows.ApplicationModel;
using Windows.Foundation;
using Windows.UI;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Microsoft.Graphics.Canvas.UI.Xaml;
using Microsoft.Graphics.Canvas.Brushes;
using Microsoft.Graphics.Canvas.UI;
using Microsoft.Graphics.Canvas.Geometry;
using Microsoft.Graphics.Canvas.Text;


namespace ExampleGallery
{
    public sealed partial class DrawImageEmulations : UserControl, INotifyPropertyChanged
    {
        public event PropertyChangedEventHandler PropertyChanged;

        public DrawImageEmulations()
        {
            this.InitializeComponent();

            Modes = new List<ModeDescription>();
            Modes.Add(new ModeDescription("Offset emulation", typeof(OffsetDemo)));
            Modes.Add(new ModeDescription("Destination Rectangle emulation", typeof(DestRectDemo)));
            Modes.Add(new ModeDescription("Opacity emulation", typeof(OpacityDemo)));
            CurrentMode = Modes[0];

            this.DataContext = this;
        }

        public string DescriptionText
        {
            get { return (string)CurrentMode.Type.GetRuntimeProperty("Description").GetValue(null); }
        }

        public interface IDemo
        {
            void Draw(CanvasDrawingSession ds, int frameCounter, float width, float height);
        }

        public struct ModeDescription
        {
            public string Name;
            public Type Type;

            public ModeDescription(string name, Type type)
            {
                Name = name;
                Type = type;
            }

            public override string ToString()
            {
                return Name;
            }

            public IDemo Create(DrawImageEmulations example, CanvasControl sender)
            {
                return (IDemo)Activator.CreateInstance(Type, example, sender);
            }
        }

        public List<ModeDescription> Modes { get; set; }

        ModeDescription currentMode;
        public ModeDescription CurrentMode
        {
            get 
            { 
                return currentMode; 
            }
            set
            {
                currentMode = value;
                modeInstance = null;
                if (PropertyChanged != null)
                    PropertyChanged(this, new PropertyChangedEventArgs("DescriptionText"));
            }
        }

        IDemo modeInstance;

        CanvasBitmap tiger;
        CanvasImageBrush checkedFillPattern;
        int frameCounter;

        private void control_Unloaded(object sender, RoutedEventArgs e)
        {
            // Explicitly remove references to allow the Win2D controls to get garbage collected
            canvasControl.RemoveFromVisualTree();
            canvasControl = null;
            modeInstance = null;
        }

        private void canvasControl_CreateResources(CanvasControl sender, CanvasCreateResourcesEventArgs args)
        {
            modeInstance = null;

            args.TrackAsyncAction(canvasControl_CreateResourcesAsync(sender).AsAsyncAction());

            var checks = CanvasBitmap.CreateFromColors(sender, new Color[] { Colors.Gray, Colors.Black, Colors.Black, Colors.Gray }, 2, 2);
            checkedFillPattern = new CanvasImageBrush(sender, checks)
            {
                ExtendX = CanvasEdgeBehavior.Wrap,
                ExtendY = CanvasEdgeBehavior.Wrap,
                Transform = Matrix3x2.CreateScale(16),
                Interpolation = CanvasImageInterpolation.NearestNeighbor
            };
        }

        async Task canvasControl_CreateResourcesAsync(CanvasControl sender)
        {
            tiger = await CanvasBitmap.LoadAsync(sender, "imageTiger.jpg");
        }

        private void canvasControl_Draw(CanvasControl sender, CanvasDrawEventArgs args)
        {
            if (DesignMode.DesignModeEnabled)
                return;

            if (modeInstance == null)
            {
                frameCounter = 0;
                modeInstance = CurrentMode.Create(this, sender);
            }

            modeInstance.Draw(args.DrawingSession, frameCounter, (float)sender.ActualWidth, (float)sender.ActualHeight);

            frameCounter++;
            sender.Invalidate();
        }

        class OffsetDemo : IDemo
        {
            public static string Description
            {
                get
                {
                    return "When DrawImage() is called with an offset, but it would be more optimal to call ID2D1DeviceContext::DrawBitmap() then it needs to " +
                        "calculate an appropriate destination rectangle based on the size of the source rectangle.  The demo shows this happening when drawing " +
                        "a bitmap with a different DPI to the destination.";
                }
            }

            CanvasImageBrush fillPattern;
            CanvasBitmap sourceBitmap;
            HueRotationEffect sourceEffect;
            CanvasRenderTarget showSourceRectRT;

            CanvasStrokeStyle hairline = new CanvasStrokeStyle()
            {
                TransformBehavior = CanvasStrokeTransformBehavior.Hairline
            };

            public OffsetDemo(DrawImageEmulations example, CanvasControl sender)
            {
                fillPattern = example.checkedFillPattern;

                var rt = new CanvasRenderTarget(sender, (float)example.tiger.Size.Width, (float)example.tiger.Size.Height, sender.Dpi / 3);
                using (var ds = rt.CreateDrawingSession())
                {
                    ds.DrawImage(example.tiger, rt.Bounds);
                }
                sourceBitmap = rt;

                sourceEffect = new HueRotationEffect()
                {
                    Source = sourceBitmap,
                    Angle = 1
                };

                showSourceRectRT = new CanvasRenderTarget(sender, (float)rt.Size.Width, (float)rt.Size.Height, rt.Dpi);
            }

            public void Draw(CanvasDrawingSession ds, int frameCounter, float width, float height)
            {
                Size sz = sourceBitmap.Size;

                double sourceSizeScale = (Math.Sin(frameCounter * 0.03) * 0.3) + 0.7;

                Point center = new Point(Math.Sin(frameCounter * 0.02), (Math.Cos(frameCounter * 0.01)));

                center.X *= (1 - sourceSizeScale) * sz.Width * 0.5;
                center.Y *= (1 - sourceSizeScale) * sz.Height * 0.5;

                center.X += sz.Width * 0.5;
                center.Y += sz.Height * 0.5;

                Rect sourceRect = new Rect(
                    center.X - sz.Width * sourceSizeScale * 0.5,
                    center.Y - sz.Height * sourceSizeScale * 0.5,
                    sz.Width * sourceSizeScale,
                    sz.Height * sourceSizeScale);
                
                UpdateSourceRectRT(sourceRect);

                var format = new CanvasTextFormat()
                {
                    FontSize = 14,
                    HorizontalAlignment = CanvasHorizontalAlignment.Right,
                    VerticalAlignment = CanvasVerticalAlignment.Center,
                    WordWrapping = CanvasWordWrapping.Wrap
                };

                float y = 0;
                float labelX = width / 2 - 5;
                float imageX = width / 2 + 5;
                float entryHeight = (float)sourceBitmap.Bounds.Height;
                float margin = 14;

                Rect labelRect = new Rect(0, y, labelX, entryHeight);

                ds.DrawText(string.Format("Source image {0} DPI", sourceBitmap.Dpi), labelRect, Colors.White, format);
                ds.FillRectangle(imageX, y, (float)sz.Width, (float)sz.Height, fillPattern);
                ds.DrawImage(showSourceRectRT, imageX, y, showSourceRectRT.Bounds, 1, CanvasImageInterpolation.NearestNeighbor);

                y += entryHeight + margin;
                labelRect.Y = y;

                ds.DrawText("D2D DrawBitmap (emulated dest rect)", labelRect, Colors.White, format);
                ds.FillRectangle(imageX, y, (float)sz.Width, (float)sz.Height, fillPattern);
                ds.DrawImage(sourceBitmap, imageX, y, sourceRect);
                
                y += (float)sourceBitmap.Bounds.Height + 14;
                labelRect.Y = y;

                ds.DrawText("D2D DrawImage", labelRect, Colors.White, format);
                ds.FillRectangle(imageX, y, (float)sz.Width, (float)sz.Height, fillPattern);
                ds.DrawImage(sourceEffect, imageX, y, sourceRect);

            }

            void UpdateSourceRectRT(Rect sourceRect)
            {
                using (var ds = showSourceRectRT.CreateDrawingSession())
                {
                    ds.DrawImage(sourceBitmap);

                    ds.Antialiasing = CanvasAntialiasing.Aliased;
                    sourceRect.X += 0.5;
                    sourceRect.Y += 0.5;
                    sourceRect.Width -= 0.5;
                    sourceRect.Height -= 0.5;
                    ds.DrawRectangle(sourceRect, Colors.Yellow, 1, hairline);
                }
            }
        }

        class DestRectDemo : IDemo
        {
            public static string Description
            {
                get
                {
                    return "When DrawImage() is called with a DestinationRectangle parameter, but needs to call ID2D1DeviceContext::DrawImage() then " +
                        "it needs to emulate the behavior of ID2D1DeviceContext::DrawBitmap() by scaling the source image to fit inside the destination rectangle. " +
                        "However, if the source image is not a bitmap then it does not clip the source rectangle to the bounds of the source image since this " +
                        "would be prohibitively expensive.";
                }
            }

            CanvasImageBrush fillPattern;
            CanvasBitmap sourceBitmap;
            HueRotationEffect sourceEffect;

            CanvasStrokeStyle hairline = new CanvasStrokeStyle()
            {
                TransformBehavior = CanvasStrokeTransformBehavior.Hairline
            };


            public DestRectDemo(DrawImageEmulations example, CanvasControl sender)
            {
                fillPattern = example.checkedFillPattern;
                sourceBitmap = example.tiger;
                sourceEffect = new HueRotationEffect()
                {
                    Source = sourceBitmap,
                    Angle = 1
                };
            }

            public void Draw(CanvasDrawingSession ds, int frameCounter, float width, float height)
            {
                var sz = sourceBitmap.Size;
                Rect sourceRect = new Rect(
                    sz.Width * 0.25 + Math.Sin(frameCounter * 0.02) * (sz.Width * 0.5),
                    sz.Height * 0.25 + Math.Cos(frameCounter * 0.01) * (sz.Height * 0.5),
                    sz.Width * 0.5,
                    sz.Height * 0.5);

                double y = DrawSourceImage(ds, sourceRect, width);

                double displayWidth = width / 2;
                double x = displayWidth;
                double destHeight = (height - y) / 3;

                Rect bitmapDestRect = new Rect(x, y + 5, displayWidth, destHeight - 10);
                y += destHeight;

                Rect bitmapDestRect2 = new Rect(x, y + 5, displayWidth, destHeight - 10);
                y += destHeight;

                Rect effectDestRect = new Rect(x, y + 5, displayWidth, destHeight - 10);

                var format = new CanvasTextFormat()
                {
                    FontSize = 14,
                    HorizontalAlignment = CanvasHorizontalAlignment.Right,
                    VerticalAlignment = CanvasVerticalAlignment.Center
                };

                ds.DrawText("D2D DrawBitmap", 0, (float)bitmapDestRect.Y, (float)displayWidth - 10, (float)destHeight, Colors.White, format);
                ds.DrawText("D2D DrawImage (bitmap)", 0, (float)bitmapDestRect2.Y, (float)displayWidth - 10, (float)destHeight, Colors.White, format);
                ds.DrawText("D2D DrawImage (effect)", 0, (float)effectDestRect.Y, (float)displayWidth - 10, (float)destHeight, Colors.White, format);

                ds.FillRectangle(bitmapDestRect, fillPattern);
                ds.FillRectangle(bitmapDestRect2, fillPattern);
                ds.FillRectangle(effectDestRect, fillPattern);

                ds.DrawImage(sourceBitmap, bitmapDestRect, sourceRect);
                ds.DrawImage(sourceBitmap, bitmapDestRect2, sourceRect, 1, CanvasImageInterpolation.Cubic);
                ds.DrawImage(sourceEffect, effectDestRect, sourceRect);

                ds.DrawRectangle(bitmapDestRect, Colors.Yellow, 1, hairline);
                ds.DrawRectangle(bitmapDestRect2, Colors.Yellow, 1, hairline);
                ds.DrawRectangle(effectDestRect, Colors.Yellow, 1, hairline);
            }

            double DrawSourceImage(CanvasDrawingSession ds, Rect sourceRect, float width)
            {
                var sourceImageOffsetX = (float)((width / 2) - (sourceBitmap.Size.Width / 2));
                var sourceImageOffsetY = (float)(10);
                ds.DrawImage(sourceBitmap, sourceImageOffsetX, sourceImageOffsetY);

                sourceRect.X += sourceImageOffsetX;
                sourceRect.Y += sourceImageOffsetY;
                ds.DrawRectangle(sourceRect, Colors.Yellow, 1, hairline);

                return sourceImageOffsetY + sourceBitmap.Size.Height * 1.2;
            }
        }

        class OpacityDemo : IDemo
        {
            public static string Description
            {
                get
                {
                    return "In some cases DrawImage() needs to emulate the behavior of the opacity parameter to ID2D1DeviceContext::DrawBitmap. " +
                        "It does this by using a ColorMatrixEffect to modify alpha value.  This example compares calling DrawImage() with different " +
                        "alpha modes for the source image and render target so verify that the ID2D1DeviceContext::DrawImage() + effect code path produces the " +
                        "same output as the ID2D1DeviceContext::DrawBitmap() code path.";
                }
            }

            CanvasRenderTarget[] premultipliedTarget;
            CanvasRenderTarget[] ignoreTarget;

            CanvasRenderTarget premultipliedSource;
            CanvasRenderTarget ignoreSource;

            CanvasImageBrush fillPattern;

            public OpacityDemo(DrawImageEmulations example, CanvasControl sender)
            {
                fillPattern = example.checkedFillPattern;
                premultipliedSource = CreateSourceImage(sender, CanvasAlphaMode.Premultiplied);
                ignoreSource = CreateSourceImage(sender, CanvasAlphaMode.Ignore);

                premultipliedTarget = new CanvasRenderTarget[2]
                {
                    CreateTarget(sender, CanvasAlphaMode.Premultiplied),
                    CreateTarget(sender, CanvasAlphaMode.Premultiplied)
                };

                ignoreTarget = new CanvasRenderTarget[2]
                {
                    CreateTarget(sender, CanvasAlphaMode.Ignore),
                    CreateTarget(sender, CanvasAlphaMode.Ignore)
                };
            }

            private CanvasRenderTarget CreateTarget(CanvasControl sender, CanvasAlphaMode alphaMode)
            {
                return new CanvasRenderTarget(sender, 64, 64, sender.Dpi, DirectXPixelFormat.B8G8R8A8UIntNormalized, alphaMode);
            }

            private CanvasRenderTarget CreateSourceImage(CanvasControl sender, CanvasAlphaMode alphaMode)
            {
                var image = new CanvasRenderTarget(sender, 64, 64, sender.Dpi, DirectXPixelFormat.B8G8R8A8UIntNormalized, alphaMode);
                
                using (var ds = image.CreateDrawingSession())
                {
                    ds.Clear(Colors.Transparent);

                    ds.FillRectangle(0, 0, 64, 16, Colors.Blue);
                    ds.FillRectangle(0, 32 - 8, 64, 16, Color.FromArgb(128, 0, 0, 255));
                    ds.FillRectangle(0, 64 - 16, 64, 16, Colors.Blue);

                    ds.FillCircle(32, 32, 16, Colors.Yellow);
                }
                return image;
            }

            public void Draw(CanvasDrawingSession ds, int frameCounter, float width, float height)
            {
                ++frameCounter;
                float opacity = (float)(Math.Sin(frameCounter * 0.05) + 1) / 2;

                DoTest(opacity, ignoreSource, premultipliedTarget[0]);
                DoTest(opacity, ignoreSource, ignoreTarget[0]);
                DoTest(opacity, premultipliedSource, premultipliedTarget[1]);
                DoTest(opacity, premultipliedSource, ignoreTarget[1]);

                float halfWidth = width / 2;
                var premultipliedToIgnoreLabel = MakeLabel(ds, "Draw Premultiplied to Ignore", halfWidth);
                var premultipliedToPremultipliedLabel = MakeLabel(ds, "Draw Premultiplied to Premultiplied", halfWidth);
                var ignoreToIgnoreLabel = MakeLabel(ds, "Draw Ignore to Ignore", halfWidth);
                var ignoreToPremultipliedLabel = MakeLabel(ds, "Draw Ignore to Premultiplied", halfWidth);

                var totalHeight = 64 * 8.0f;

                float middle = width / 2;

                float imageX = middle + 10;
                float textRight = middle - 10;

                float y = (height / 2) - (totalHeight / 2);
                float ydiff = 64.0f * 2.0f;

                ds.DrawImage(premultipliedTarget[0], imageX, y);
                ds.DrawTextLayout(premultipliedToPremultipliedLabel, 0, y, Colors.White);
                y += ydiff;

                ds.DrawImage(ignoreTarget[0], imageX, y);
                ds.DrawTextLayout(premultipliedToIgnoreLabel, 0, y, Colors.White);
                y += ydiff;

                ds.DrawImage(premultipliedTarget[1], imageX, y);
                ds.DrawTextLayout(ignoreToPremultipliedLabel, 0, y, Colors.White);
                y += ydiff;

                ds.DrawImage(ignoreTarget[1], imageX, y);
                ds.DrawTextLayout(ignoreToIgnoreLabel, 0, y, Colors.White);
                y += ydiff;
            }

            CanvasStrokeStyle hairline = new CanvasStrokeStyle()
            {
                TransformBehavior = CanvasStrokeTransformBehavior.Hairline
            };

            void DoTest(float opacity, CanvasBitmap source, CanvasRenderTarget target)
            {
                using (var ds = target.CreateDrawingSession())
                {
                    ds.FillRectangle(target.Bounds, fillPattern);

                    var leftHalf = source.Bounds;
                    leftHalf.Width /= 2;

                    var rightHalf = source.Bounds;
                    rightHalf.Width /= 2;
                    rightHalf.X += rightHalf.Width;

                    // This version calls D2D DrawBitmap
                    ds.DrawImage(source, 0, 0, leftHalf, opacity, CanvasImageInterpolation.Linear);

                    // This version calls D2D DrawImage with emulated opacity
                    ds.DrawImage(source, (float)rightHalf.X, 0, rightHalf, opacity, CanvasImageInterpolation.Cubic);

                    ds.Antialiasing = CanvasAntialiasing.Aliased;
                    ds.DrawLine((float)rightHalf.X, 0, (float)rightHalf.X, (float)rightHalf.Height, Colors.Black, 1, hairline);
                }
            }

            CanvasTextLayout MakeLabel(CanvasDrawingSession ds, string text, float halfWidth)
            {
                var format = new CanvasTextFormat()
                {
                    FontSize = 14,
                    HorizontalAlignment = CanvasHorizontalAlignment.Right,
                    VerticalAlignment = CanvasVerticalAlignment.Center
                };

                return new CanvasTextLayout(ds, text, format, halfWidth, 64);
            }

            void Label(CanvasDrawingSession ds, float x, float y, string text)
            {
                var textFormat = new CanvasTextFormat()
                {
                    FontSize = 16,
                    HorizontalAlignment = CanvasHorizontalAlignment.Center,
                    VerticalAlignment = CanvasVerticalAlignment.Bottom
                };

                ds.DrawText(text, x + 128, y + 256 - 16, Colors.White, textFormat);
            }
        }        
    }
}
