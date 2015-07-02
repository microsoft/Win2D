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
using Microsoft.Graphics.Canvas.Effects;
using Microsoft.Graphics.Canvas.Text;
using Microsoft.Graphics.Canvas.UI;
using Microsoft.Graphics.Canvas.UI.Xaml;
using System;
using System.Numerics;
using Windows.UI;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

#if !WINDOWS_UAP
using ExampleGallery.Direct3DInterop;
#endif

namespace ExampleGallery
{
    public sealed partial class Direct3DInteropExample : Page
    {
        public bool SpinEnabled { get; set; }
        public bool BloomEnabled { get; set; }
        public float BloomIntensity { get; set; }
        public float BloomThreshold { get; set; }
        public float BloomBlur { get; set; }


        // This example is temporarily disabled for UAP.
        // We'll turn it on as soon as a UAP NuGet package for the DirectX Tool Kit is available.
#if !WINDOWS_UAP


        // The TeapotRenderer class is provided by the ExampleGallery.Direct3DInterop project,
        // which is written in C++/CX. It uses interop to combine Direct3D rendering with Win2D.
        TeapotRenderer teapot;

        float spinTheTeapot;


        // Scrolling text is drawn into a rendertarget (by Win2D).
        const string scrollingText = "This demo shows interop between Win2D and Direct3D.\n\n" +
                                     "This scrolling text is drawn into a render target by Win2D.\n\n" +
                                     "The teapot is drawn by Direct3D (via the DirectX Tool Kit).\n\n" +
                                     "Bloom post processing uses Win2D image effects.";

        const float textRenderTargetSize = 256;
        const float textMargin = 8;
        const float textScrollSpeed = 96;
        const float fontSize = 42;

        CanvasTextLayout textLayout;
        CanvasRenderTarget textRenderTarget;


        // Bloom postprocess uses Win2D image effects to create a glow around bright parts of the 3D model.
        CanvasRenderTarget bloomRenderTarget;
        LinearTransferEffect extractBrightAreas;
        GaussianBlurEffect blurBrightAreas;
        LinearTransferEffect adjustBloomIntensity;
        BlendEffect bloomResult;


        public Direct3DInteropExample()
        {
            SpinEnabled = true;

            InitializeBloomFilter();

            this.InitializeComponent();
        }


        void canvas_CreateResources(CanvasAnimatedControl sender, CanvasCreateResourcesEventArgs args)
        {
            // Create the Direct3D teapot model.
            teapot = new TeapotRenderer(sender);

            // Create Win2D resources for drawing the scrolling text.
            var textFormat = new CanvasTextFormat
            {
                FontSize = fontSize,
                HorizontalAlignment = CanvasHorizontalAlignment.Center
            };

            textLayout = new CanvasTextLayout(sender, scrollingText, textFormat, textRenderTargetSize - textMargin * 2, float.MaxValue);

            textRenderTarget = new CanvasRenderTarget(sender, textRenderTargetSize, textRenderTargetSize);

            // Set the scrolling text rendertarget (a Win2D object) as
            // source texture for our 3D teapot model (which uses Direct3D).
            teapot.SetTexture(textRenderTarget);
        }


        void canvas_Update(ICanvasAnimatedControl sender, CanvasAnimatedUpdateEventArgs args)
        {
            if (ThumbnailGenerator.IsDrawingThumbnail)
            {
                spinTheTeapot = 5.7f;
            }
            else if (SpinEnabled)
            {
                spinTheTeapot += (float)args.Timing.ElapsedTime.TotalSeconds;
            }
        }


        void canvas_Draw(ICanvasAnimatedControl sender, CanvasAnimatedDrawEventArgs args)
        {
            // Update the scrolling text rendertarget.
            DrawScrollingText(args.Timing);

            if (BloomEnabled && !ThumbnailGenerator.IsDrawingThumbnail)
            {
                // If the bloom filter is turned on, draw the teapot into a rendertarget.
                DemandCreateBloomRenderTarget(sender);
                
                using (var drawingSession = bloomRenderTarget.CreateDrawingSession())
                {
                    drawingSession.Clear(Colors.Black);

                    DrawTeapot(sender, drawingSession);
                }

                // Apply the bloom filter, which uses the rendertarget containing the teapot as input,
                // adds a glow effect, and draws the combined result to our CanvasAnimatedControl.
                ApplyBloomFilter(args.DrawingSession);
            }
            else
            {
                // If not blooming, draw the teapot directly to the CanvasAnimatedControl swapchain.
                DrawTeapot(sender, args.DrawingSession);
            }
        }


        void DrawTeapot(ICanvasAnimatedControl sender, CanvasDrawingSession drawingSession)
        {
            Vector2 size = sender.Size.ToVector2();

            // Draw some text (using Win2D) to make sure this appears behind the teapot.
            if (!ThumbnailGenerator.IsDrawingThumbnail)
            {
                drawingSession.DrawText("Text drawn before the teapot", size * new Vector2(0.5f, 0.1f), Colors.Gray);
            }

            // Draw the teapot (using Direct3D).
            teapot.SetWorld(Matrix4x4.CreateFromYawPitchRoll(-spinTheTeapot, spinTheTeapot / 23, spinTheTeapot / 42));
            teapot.SetView(Matrix4x4.CreateLookAt(new Vector3(1.5f, 1, 0), Vector3.Zero, Vector3.UnitY));
            teapot.SetProjection(Matrix4x4.CreatePerspectiveFieldOfView(1, size.X / size.Y, 0.1f, 10f));

            teapot.Draw(drawingSession);

            // Draw more text (using Win2D) to make sure this appears above the teapot.
            if (!ThumbnailGenerator.IsDrawingThumbnail)
            {
                drawingSession.DrawText("\nText drawn after the teapot", size * new Vector2(0.5f, 0.1f), Colors.Gray);
            }
        }


        void DrawScrollingText(CanvasTimingInformation timing)
        {
            // We draw the text into a rendertarget, and update this every frame to make it scroll.
            // The resulting rendertarget is then mapped as a texture onto the Direct3D teapot model.
            using (var drawingSession = textRenderTarget.CreateDrawingSession())
            {
                drawingSession.Clear(Colors.Firebrick);
                drawingSession.DrawRectangle(0, 0, textRenderTargetSize - 1, textRenderTargetSize - 1, Colors.DarkRed);

                float wrapPosition = (float)textLayout.LayoutBounds.Height + textRenderTargetSize;
                float scrollOffset = textRenderTargetSize - ((float)timing.TotalTime.TotalSeconds * textScrollSpeed) % wrapPosition;

                drawingSession.DrawTextLayout(textLayout, new Vector2(textMargin, scrollOffset), Colors.Black);
            }
        }


        void InitializeBloomFilter()
        {
            BloomEnabled = true;

            // A bloom filter makes graphics appear to be bright and glowing by adding blur around only
            // the brightest parts of the image. This approximates the look of HDR (high dynamic range)
            // rendering, in which the color of the brightest light sources spills over onto surrounding
            // pixels.
            //
            // Many different visual styles can be achieved by adjusting these settings:
            //
            //  Intensity = how much bloom is added
            //      0 = none
            //
            //  Threshold = how bright does a pixel have to be in order for it to bloom
            //      0 = the entire image blooms equally
            //
            //  Blur = how much the glow spreads sideways around bright areas

            BloomIntensity = 200;
            BloomThreshold = 80;
            BloomBlur = 48;

            // Before drawing, properties of these effects will be adjusted to match the
            // current settings (see ApplyBloomFilter), and an input image connected to
            // extractBrightAreas.Source and bloomResult.Background (see DemandCreateBloomRenderTarget).

            // Step 1: use a transfer effect to extract only pixels brighter than the threshold.
            extractBrightAreas = new LinearTransferEffect
            {
                ClampOutput = true,
            };

            // Step 2: blur these bright pixels.
            blurBrightAreas = new GaussianBlurEffect
            {
                Source = extractBrightAreas,
            };

            // Step 3: adjust how much bloom is wanted.
            adjustBloomIntensity = new LinearTransferEffect
            {
                Source = blurBrightAreas,
            };

            // Step 4: blend the bloom over the top of the original image.
            bloomResult = new BlendEffect
            {
                Foreground = adjustBloomIntensity,
                Mode = BlendEffectMode.Screen,
            };
        }


        void ApplyBloomFilter(CanvasDrawingSession drawingSession)
        {
            // Configure effects to use the latest threshold, blur, and intensity settings.
            extractBrightAreas.RedSlope = 
            extractBrightAreas.GreenSlope = 
            extractBrightAreas.BlueSlope = 1 / (1 - BloomThreshold / 100);

            extractBrightAreas.RedOffset = 
            extractBrightAreas.GreenOffset = 
            extractBrightAreas.BlueOffset = -BloomThreshold / 100 / (1 - BloomThreshold / 100);

            blurBrightAreas.BlurAmount = BloomBlur;
            
            adjustBloomIntensity.RedSlope = 
            adjustBloomIntensity.GreenSlope = 
            adjustBloomIntensity.BlueSlope = BloomIntensity / 100;

            // Apply the bloom effect.
            drawingSession.DrawImage(bloomResult);
        }

        int DipsToPixelSize(ICanvasAnimatedControl sender, float dips)
        {
            System.Diagnostics.Debug.Assert(dips > 0);
            return Math.Max(sender.ConvertDipsToPixels(dips, CanvasDpiRounding.Round), 1);
        }

        void DemandCreateBloomRenderTarget(ICanvasAnimatedControl sender)
        {
            // Early-out if we already have a rendertarget of the correct size.
            // Compare sizes as pixels rather than DIPs to avoid rounding artifacts.
            if (bloomRenderTarget != null &&
                bloomRenderTarget.SizeInPixels.Width == DipsToPixelSize(sender, (float)sender.Size.Width) &&
                bloomRenderTarget.SizeInPixels.Height == DipsToPixelSize(sender, (float)sender.Size.Height))
            {
                return;
            }

            // Destroy the old rendertarget.
            if (bloomRenderTarget != null)
            {
                bloomRenderTarget.Dispose();
            }

            // Create the new rendertarget.
            bloomRenderTarget = new CanvasRenderTarget(sender, sender.Size);

            // Configure the bloom effect to use this new rendertarget.
            extractBrightAreas.Source = bloomRenderTarget;
            bloomResult.Background = bloomRenderTarget;
        }


#else   // WINDOWS_UAP

        public Direct3DInteropExample()
        {
            this.InitializeComponent();
        }

        void canvas_CreateResources(CanvasAnimatedControl sender, CanvasCreateResourcesEventArgs args)
        {
        }

        void canvas_Update(ICanvasAnimatedControl sender, CanvasAnimatedUpdateEventArgs args)
        {
        }

        void canvas_Draw(ICanvasAnimatedControl sender, CanvasAnimatedDrawEventArgs args)
        {
            args.DrawingSession.DrawText(
                "This example is temporarily disabled for UAP.\n\n" +
                "We'll turn it on as soon as a UAP NuGet package\n" +
                "for the DirectX Tool Kit is available.",
                sender.Size.ToVector2() / 2,
                Colors.Red,
                new CanvasTextFormat
                {
                    HorizontalAlignment = CanvasHorizontalAlignment.Center,
                    VerticalAlignment = CanvasVerticalAlignment.Center,
                });
        }

#endif  // WINDOWS_UAP


        private void control_Unloaded(object sender, RoutedEventArgs e)
        {
            // Explicitly remove references to allow the Win2D controls to get garbage collected
            canvas.RemoveFromVisualTree();
            canvas = null;
        }
    }
}
