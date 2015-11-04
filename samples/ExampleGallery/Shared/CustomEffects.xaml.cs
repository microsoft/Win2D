// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

using Microsoft.Graphics.Canvas;
using Microsoft.Graphics.Canvas.Brushes;
using Microsoft.Graphics.Canvas.Effects;
using Microsoft.Graphics.Canvas.UI;
using Microsoft.Graphics.Canvas.UI.Xaml;
using System;
using System.Collections.Generic;
using System.Numerics;
using System.Threading.Tasks;
using Windows.UI;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace ExampleGallery
{
    public sealed partial class CustomEffects : UserControl
    {
        public enum DissolveType
        {
            Ripples,
            Turbulence,
            LinearGradient,
            RadialGradient,
            NoDissolve,
        }

        public DissolveType CurrentDissolve { get; set; }
        public List<DissolveType> DissolveList { get { return Utils.GetEnumAsList<DissolveType>(); } }

        public bool SketchEnabled { get; set; }

        Random random = new Random();

        CanvasBitmap bitmapTiger;
        Vector2 tigerSize;

        // Custom pixel shader effects.
        PixelShaderEffect sketchEffect;
        PixelShaderEffect dissolveEffect;
        PixelShaderEffect rippleEffect;

        // Other images, used as mask inputs to the dissolveEffect.
        ICanvasImage turbulence;
        Transform2DEffect linearGradient;
        CanvasCommandList radialGradient;


        public CustomEffects()
        {
            SketchEnabled = true;

            this.InitializeComponent();
        }


        void Canvas_CreateResources(CanvasAnimatedControl sender, CanvasCreateResourcesEventArgs args)
        {
            args.TrackAsyncAction(Canvas_CreateResourcesAsync(sender).AsAsyncAction());
        }


        async Task Canvas_CreateResourcesAsync(CanvasAnimatedControl sender)
        {
            bitmapTiger = await CanvasBitmap.LoadAsync(sender, "imageTiger.jpg");
            tigerSize = bitmapTiger.Size.ToVector2();

            // The Sketch shader has two input textures:
            //
            //  - First is the image that will be processed by the sketch effect.
            //    The sketch shader applies a 3x3 edge detection filter kernel to this input,
            //    so we specify Offset coordinate mapping mode with a max offset of 1 dip.
            //
            //  - Second is an overlay containing a pencil sketch texture. The JitterX and JitterY
            //    properties offset this by randomly varying amounts, so we specify Unknown mapping
            //    mode to indicate that the entire image must be made available to the shader.

            sketchEffect = new PixelShaderEffect(await Utils.ReadAllBytes("Shaders/Sketch.bin"))
            {
                Source1 = bitmapTiger,
                Source1BorderMode = EffectBorderMode.Hard,
                Source1Mapping = SamplerCoordinateMapping.Offset,
                MaxSamplerOffset = (int)Math.Ceiling(sender.Dpi / 96),

                Source2 = await CanvasBitmap.LoadAsync(sender, "Shaders/SketchTexture.jpg"),
                Source2Mapping = SamplerCoordinateMapping.Unknown
            };

            sketchEffect.Properties["EdgeOffset"] = sender.Dpi / 96;

            // The Dissolve shader has two input textures:
            //
            //  - The first is an image that will be dissolved away to nothing.
            //
            //  - The second is a dissolve mask whose red channel controls the order in which pixels
            //    of the first image disappear as the dissolveAmount property is animated.
            //
            // This example selects different dissolve masks depending on the CurrentEffect.

            dissolveEffect = new PixelShaderEffect(await Utils.ReadAllBytes("Shaders/Dissolve.bin"));

            // The Ripples shader has no input textures.
            // It generates an animatable series of concentric circles.
            // This is used as a mask input to the dissolveEffect.
            rippleEffect = new PixelShaderEffect(await Utils.ReadAllBytes("Shaders/Ripples.bin"));

            rippleEffect.Properties["frequency"] = 0.15f;
            rippleEffect.Properties["dpi"] = sender.Dpi;
#if WINDOWS_UWP
            rippleEffect.Properties["center"] = tigerSize / 3;
#else
            rippleEffect.Properties["center"] = (Microsoft.Graphics.Canvas.Numerics.Vector2)(tigerSize / 3);

            // When compiling for Windows 8.1, we must explicitly convert vector and matrix values
            // from System.Numerics to their Microsoft.Graphics.Canvas.Numerics equivalents before
            // passing them to PixelShaderEffect.Properties. This is not neccessary when targetting
            // UWP, which handles the conversion automatically. For more info, see the article:
            // http://blogs.msdn.com/b/win2d/archive/2015/06/02/winrt-vector-and-matrix-types-in-windows-10.aspx
#endif

            // Create other dissolve mask images.
            CreateTurbulence();
            CreateLinearGradient(sender);
            CreateRadialGradient(sender);
        }


        void Canvas_Update(ICanvasAnimatedControl sender, CanvasAnimatedUpdateEventArgs args)
        {
            // Randomly offset the sketch overlay pattern to create a hand-drawn animation effect.
            const int jitterRate = 5;
            const float jitterOffset = 256;

            if (args.Timing.UpdateCount % jitterRate == 0)
            {
                sketchEffect.Properties["JitterX"] = (float)random.NextDouble() * jitterOffset;
                sketchEffect.Properties["JitterY"] = (float)random.NextDouble() * jitterOffset;
            }
        }

        void Canvas_Draw(ICanvasAnimatedControl sender, CanvasAnimatedDrawEventArgs args)
        {
            var elapsedTime = (float)args.Timing.TotalTime.TotalSeconds;

            // Center in the control.
            var position = (sender.Size.ToVector2() - tigerSize) / 2;
            position.Y -= tigerSize.Y * 0.75f;

            // Is the sketch effect enabled?
            ICanvasImage sourceImage;

            if (SketchEnabled)
                sourceImage = sketchEffect;
            else
                sourceImage = bitmapTiger;

            // Which dissolve mode are we currently displaying?
            ICanvasImage dissolveMask;

            switch (CurrentDissolve)
            {
                case DissolveType.Ripples:
                    // Use the custom rippleEffect as our dissolve mask, and animate its offset.
                    dissolveMask = rippleEffect;
                    rippleEffect.Properties["offset"] = -elapsedTime * 6;
                    break;

                case DissolveType.Turbulence:
                    // Use a turbulence image as the dissolve mask.
                    dissolveMask = turbulence;
                    break;

                case DissolveType.LinearGradient:
                    // Use a linear gradient as the dissolve mask, and slowly rotate it.
                    dissolveMask = linearGradient;
                    linearGradient.TransformMatrix = Matrix3x2.CreateRotation(elapsedTime / 3, tigerSize / 2);
                    break;

                case DissolveType.RadialGradient:
                    // Use a radial gradient as the dissolve mask.
                    dissolveMask = radialGradient;
                    break;

                case DissolveType.NoDissolve:
                    // Dissolve is turned off, so just draw the source image directly.
                    args.DrawingSession.DrawImage(sourceImage, position);
                    return;

                default:
                    throw new NotSupportedException();
            }

            // Animate the dissolve amount.
            dissolveEffect.Properties["dissolveAmount"] = (float)Math.Sin(elapsedTime * 2.4) / 2 + 0.5f;

            // Draw the custom effect.
            dissolveEffect.Source1 = sourceImage;
            dissolveEffect.Source2 = dissolveMask;

            args.DrawingSession.DrawImage(dissolveEffect, position);

            if (!ThumbnailGenerator.IsDrawingThumbnail)
            {
                // Display the current dissolve mask.
                args.DrawingSession.DrawText("Dissolve mask:", position.X, position.Y + tigerSize.Y * 1.5f - 32, Colors.Gray);
                args.DrawingSession.DrawImage(dissolveMask, position.X, position.Y + tigerSize.Y * 1.5f, bitmapTiger.Bounds);

                // Display the current dissolve amount.
                string dissolvePercentage = string.Format("{0:0}%", (float)dissolveEffect.Properties["dissolveAmount"] * 100);
                args.DrawingSession.DrawText(dissolvePercentage, position + tigerSize * new Vector2(1.2f, 0.4f), Colors.Gray);
            }
        }


        void CreateTurbulence()
        {
            turbulence = new ColorMatrixEffect
            {
                Source = new TurbulenceEffect
                {
                    Size = tigerSize,
                    Frequency = new Vector2(0.02f),
                    Octaves = 3,
                },

                // Adjust intensity of the generated turbulence, and copy the red channel over green and blue.
                ColorMatrix = new Matrix5x4
                {
                    M11 = 1.8f,
                    M12 = 1.8f,
                    M13 = 1.8f,

                    M51 = -0.4f,
                    M52 = -0.4f,
                    M53 = -0.4f,

                    M54 = 1
                }
            };
        }


        void CreateLinearGradient(ICanvasResourceCreator resourceCreator)
        {
            var commandList = new CanvasCommandList(resourceCreator);

            using (var drawingSession = commandList.CreateDrawingSession())
            {
                var brush = new CanvasLinearGradientBrush(resourceCreator, Colors.White, Colors.Black)
                {
                    StartPoint = new Vector2(-tigerSize.X / 4, 0),
                    EndPoint = new Vector2(tigerSize.X * 5 / 4, 0),
                };

                drawingSession.FillRectangle(bitmapTiger.Bounds, brush);
            }

            // Wrap the gradient with a border effect to avoid edge artifacts as we rotate it.
            linearGradient = new Transform2DEffect
            {
                Source = new BorderEffect
                {
                    Source = commandList
                }
            };
        }


        void CreateRadialGradient(ICanvasResourceCreator resourceCreator)
        {
            radialGradient = new CanvasCommandList(resourceCreator);

            using (var drawingSession = radialGradient.CreateDrawingSession())
            {
                var sqrt2 = (float)Math.Sqrt(2);

                var brush = new CanvasRadialGradientBrush(resourceCreator, Colors.White, Colors.Black)
                {
                    Center = tigerSize / 2,

                    RadiusX = tigerSize.X / sqrt2,
                    RadiusY = tigerSize.Y / sqrt2,
                };

                drawingSession.FillRectangle(bitmapTiger.Bounds, brush);
            }
        }


        void control_Unloaded(object sender, RoutedEventArgs e)
        {
            // Explicitly remove references to allow the Win2D controls to get garbage collected
            canvas.RemoveFromVisualTree();
            canvas = null;
        }
    }
}
