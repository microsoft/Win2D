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
        public enum EffectType
        {
            Ripples,
            Turbulence,
            LinearGradient,
            RadialGradient,
        }

        public EffectType CurrentEffect { get; set; }
        public List<EffectType> EffectsList { get { return Utils.GetEnumAsList<EffectType>(); } }

        CanvasBitmap bitmapTiger;
        Vector2 bitmapSize;

        // Custom pixel shader effects.
        PixelShaderEffect dissolveEffect;
        PixelShaderEffect rippleEffect;

        // Other images, used as mask inputs to the dissolveEffect.
        ICanvasImage turbulence;
        Transform2DEffect linearGradient;
        CanvasCommandList radialGradient;


        public CustomEffects()
        {
            this.InitializeComponent();
        }


        void Canvas_CreateResources(CanvasAnimatedControl sender, CanvasCreateResourcesEventArgs args)
        {
            args.TrackAsyncAction(Canvas_CreateResourcesAsync(sender).AsAsyncAction());
        }


        async Task Canvas_CreateResourcesAsync(CanvasAnimatedControl sender)
        {
            bitmapTiger = await CanvasBitmap.LoadAsync(sender, "imageTiger.jpg");
            bitmapSize = bitmapTiger.Size.ToVector2();

            // The Dissolve shader has two input textures:
            //
            //  - The first is an image that will be dissolved away to nothing.
            //
            //  - The second is a dissolve mask whose red channel controls the order in which pixels
            //    of the first image disappear as the dissolveAmount property is animated.
            //
            // This example selects different dissolve masks depending on the CurrentEffect.

            dissolveEffect = new PixelShaderEffect(await Utils.ReadAllBytes("Shaders/Dissolve.bin"))
            {
                Source1 = bitmapTiger
            };

            // The Ripples shader has no input textures.
            // It generates an animatable series of concentric circles.
            // This is used as a mask input to the dissolveEffect.
            rippleEffect = new PixelShaderEffect(await Utils.ReadAllBytes("Shaders/Ripples.bin"));

            rippleEffect.Properties["frequency"] = 0.15f;
            rippleEffect.Properties["dpi"] = sender.Dpi;
#if WINDOWS_UWP
            rippleEffect.Properties["center"] = bitmapSize / 3;
#else
            rippleEffect.Properties["center"] = (Microsoft.Graphics.Canvas.Numerics.Vector2)(bitmapSize / 3);

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


        void Canvas_Draw(ICanvasAnimatedControl sender, CanvasAnimatedDrawEventArgs args)
        {
            var elapsedTime = (float)args.Timing.TotalTime.TotalSeconds;

            ICanvasImage dissolveMask;

            switch (CurrentEffect)
            {
                case EffectType.Ripples:
                    // Use the custom rippleEffect as our dissolve mask, and animate its offset.
                    dissolveMask = rippleEffect;
                    rippleEffect.Properties["offset"] = -elapsedTime * 6;
                    break;

                case EffectType.Turbulence:
                    // Use a turbulence image as the dissolve mask.
                    dissolveMask = turbulence;
                    break;

                case EffectType.LinearGradient:
                    // Use a linear gradient as the dissolve mask, and slowly rotate it.
                    dissolveMask = linearGradient;
                    linearGradient.TransformMatrix = Matrix3x2.CreateRotation(elapsedTime / 3, bitmapSize / 2);
                    break;

                case EffectType.RadialGradient:
                    // Use a radial gradient as the dissolve mask.
                    dissolveMask = radialGradient;
                    break;

                default:
                    throw new NotSupportedException();
            }

            dissolveEffect.Source2 = dissolveMask;

            // Animate the dissolve amount.
            dissolveEffect.Properties["dissolveAmount"] = (float)Math.Sin(elapsedTime * 2) / 2 + 0.5f;

            // Center in the control.
            var position = (sender.Size.ToVector2() - bitmapSize) / 2;
            position.Y -= bitmapSize.Y * 0.75f;

            // Draw the custom effect.
            args.DrawingSession.DrawImage(dissolveEffect, position);

            if (!ThumbnailGenerator.IsDrawingThumbnail)
            {
                // Display the current dissolve mask.
                args.DrawingSession.DrawText("Dissolve mask:", position.X, position.Y + bitmapSize.Y * 1.5f - 32, Colors.Gray);
                args.DrawingSession.DrawImage(dissolveMask, position.X, position.Y + bitmapSize.Y * 1.5f, bitmapTiger.Bounds);

                // Display the current dissolve amount.
                string dissolvePercentage = string.Format("{0:0}%", (float)dissolveEffect.Properties["dissolveAmount"] * 100);
                args.DrawingSession.DrawText(dissolvePercentage, position + bitmapSize * new Vector2(1.2f, 0.4f), Colors.Gray);
            }
        }


        void CreateTurbulence()
        {
            turbulence = new LinearTransferEffect
            {
                Source = new TurbulenceEffect
                {
                    Size = bitmapSize,
                    Frequency = new Vector2(0.02f),
                    Octaves = 3,
                },

                RedSlope = 2,
                RedOffset = -0.2f,
            };
        }


        void CreateLinearGradient(ICanvasResourceCreator resourceCreator)
        {
            var commandList = new CanvasCommandList(resourceCreator);

            using (var drawingSession = commandList.CreateDrawingSession())
            {
                var brush = new CanvasLinearGradientBrush(resourceCreator, Colors.White, Colors.Black)
                {
                    StartPoint = new Vector2(-bitmapSize.X / 4, 0),
                    EndPoint = new Vector2(bitmapSize.X * 5 / 4, 0),
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
                    Center = bitmapSize / 2,

                    RadiusX = bitmapSize.X / sqrt2,
                    RadiusY = bitmapSize.Y / sqrt2,
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
