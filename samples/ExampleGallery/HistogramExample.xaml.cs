// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

using Microsoft.Graphics.Canvas;
using Microsoft.Graphics.Canvas.Brushes;
using Microsoft.Graphics.Canvas.Effects;
using Microsoft.Graphics.Canvas.Text;
using Microsoft.Graphics.Canvas.UI;
using Microsoft.Graphics.Canvas.UI.Xaml;
using System;
using System.Numerics;
using System.Threading.Tasks;
using Windows.Foundation;
using Windows.UI;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;

namespace ExampleGallery
{
    public sealed partial class HistogramExample : UserControl
    {
        CanvasBitmap bitmap;

        CanvasLinearGradientBrush redBrush;
        CanvasLinearGradientBrush greenBrush;
        CanvasLinearGradientBrush blueBrush;

        BrightnessEffect brightnessEffect;
        SaturationEffect saturationEffect;
        HueRotationEffect hueEffect;

        public float Brightness { get; set; }
        public float Saturation { get; set; }
        public float Hue { get; set; }


        public HistogramExample()
        {
            Brightness = 1;
            Saturation = 2;
            Hue = 0;

            this.InitializeComponent();
        }


        void Canvas_CreateResources(CanvasControl sender, CanvasCreateResourcesEventArgs args)
        {
            args.TrackAsyncAction(Canvas_CreateResourcesAsync(sender).AsAsyncAction());
        }


        async Task Canvas_CreateResourcesAsync(CanvasControl sender)
        {
            bitmap = await CanvasBitmap.LoadAsync(sender, "imageTiger.jpg");

            redBrush   = CreateGradientBrush(sender, 255, 0, 0);
            greenBrush = CreateGradientBrush(sender, 0, 255, 0);
            blueBrush  = CreateGradientBrush(sender, 0, 0, 255);

            brightnessEffect = new BrightnessEffect  { Source = bitmap };
            saturationEffect = new SaturationEffect  { Source = brightnessEffect };
            hueEffect        = new HueRotationEffect { Source = saturationEffect };
        }


        void Canvas_Draw(CanvasControl sender, CanvasDrawEventArgs args)
        {
            if (CanvasImage.IsHistogramSupported(sender.Device))
            {
                SetEffectProperties();

                // Draw the image.
                if (!ThumbnailGenerator.IsDrawingThumbnail)
                {
                    args.DrawingSession.DrawImage(hueEffect, (sender.Size.ToVector2() - bitmap.Size.ToVector2()) * new Vector2(0.5f, 0.25f));
                }

                // Overlay a histogram for each color channel.
                args.DrawingSession.Blend = CanvasBlend.Add;

                DrawHistogram(args.DrawingSession, sender.Size, EffectChannelSelect.Red,   redBrush);
                DrawHistogram(args.DrawingSession, sender.Size, EffectChannelSelect.Green, greenBrush);
                DrawHistogram(args.DrawingSession, sender.Size, EffectChannelSelect.Blue,  blueBrush);
            }
            else
            {
                DrawNotSupported(args.DrawingSession, sender.Size);
            }
        }


        void DrawHistogram(CanvasDrawingSession drawingSession, Size size, EffectChannelSelect channelSelect, CanvasLinearGradientBrush brush)
        {
            const int binCount = 64;
            const float graphPower = 0.25f; // Nonlinear scale makes the graph show small changes more clearly.

            float[] histogram = CanvasImage.ComputeHistogram(hueEffect, bitmap.Bounds, drawingSession, channelSelect, binCount);

            var w = (float)size.Width / binCount;
            var h = (float)size.Height;

            for (int i = 0; i < binCount; i++)
            {
                var x = i * w;
                var y = (1 - (float)Math.Pow(histogram[i], graphPower)) * h;

                brush.StartPoint = new Vector2(x, y);
                brush.EndPoint = new Vector2(x, h);

                drawingSession.FillRectangle(x, y, w, h - y, brush);
            }
        }


        static void DrawNotSupported(CanvasDrawingSession drawingSession, Size size)
        {
            var bounds = new Rect(new Point(), size);

            var format = new CanvasTextFormat
            {
                FontSize = 23,
                FontFamily = "Comic Sans MS",
                HorizontalAlignment = CanvasHorizontalAlignment.Center,
                VerticalAlignment = CanvasVerticalAlignment.Center,
            };

            drawingSession.DrawText(":-(\n\nComputeHistogram is not supported on this GPU", bounds, Colors.Gray, format);
        }


        void SetEffectProperties()
        {
            brightnessEffect.BlackPoint = new Vector2(Math.Max(1 - Brightness, 0), 0);
            brightnessEffect.WhitePoint = new Vector2(Math.Min(2 - Brightness, 1), 1);

            saturationEffect.Saturation = Saturation;

            hueEffect.Angle = Hue;
        }


        static CanvasLinearGradientBrush CreateGradientBrush(ICanvasResourceCreator resourceCreator, byte r, byte g, byte b)
        {
            const int stopCount = 8;
            const float falloffPower = 4;

            var stops = new CanvasGradientStop[stopCount];

            for (int i = 0; i < stopCount; i++)
            {
                var t = (float)i / (stopCount - 1);
                var a = (byte)(SmoothStep(t) * 255);

                stops[i].Position = (float)Math.Pow(1 - t, falloffPower);
                stops[i].Color = Color.FromArgb(a, r, g, b);
            }

            return new CanvasLinearGradientBrush(resourceCreator, stops);
        }


        static float SmoothStep(float x)
        {
            return (3 * x * x) - (2 * x * x * x);
        }


        void Slider_ValueChanged(object sender, RangeBaseValueChangedEventArgs e)
        {
            if (canvas != null)
                canvas.Invalidate();
        }


        void control_Unloaded(object sender, RoutedEventArgs e)
        {
            // Explicitly remove references to allow the Win2D controls to get garbage collected
            canvas.RemoveFromVisualTree();
            canvas = null;
        }
    }
}
