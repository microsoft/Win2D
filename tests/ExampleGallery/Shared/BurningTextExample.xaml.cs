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
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Numerics;
using Windows.Foundation;
using Windows.UI;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Input;

namespace ExampleGallery
{
    public sealed partial class BurningTextExample : UserControl
    {
        public BurningTextExample()
        {
            this.InitializeComponent();
        }

        // References to specific effects so we can dynamically update their properties.
        CanvasRenderTarget textRenderTarget;
        MorphologyEffect morphology;
        CompositeEffect composite;
        Transform2DEffect flameAnimation;
        Transform2DEffect flamePosition;

        Stopwatch timer;

        Size canvasSize;

        void Canvas_CreateResources(CanvasControl sender, object args)
        {
            canvasSize = new Size(0, 0);
            timer = Stopwatch.StartNew();
            CreateFlameEffect();
        }

        private void Canvas_Draw(CanvasControl sender, CanvasDrawEventArgs args)
        {
            var ds = args.DrawingSession;

            // If the window size has changed, (re)create the text bitmap.
            var newCanvasSize = new Size(sender.ActualWidth, sender.ActualHeight);
            if (canvasSize != newCanvasSize)
            {
                canvasSize = newCanvasSize;

                SetupText();
            };

            UpdateAnimation((float)timer.Elapsed.TotalSeconds);
            ds.DrawImage(composite);

            sender.Invalidate();
        }

        private void UpdateAnimation(float elapsedMilliseconds)
        {
            // Animate the flame by shifting the Perlin noise upwards (-Y) over time.
            flameAnimation.TransformMatrix = Matrix3x2.CreateTranslation(0, -elapsedMilliseconds * 60.0f);

            // Scale the flame effect 2x vertically, aligned so it starts above the text.
            var w = (float)canvas.ActualWidth;
            var h = (float)canvas.ActualHeight;

            float verticalOffset = GetFontSize(w) * 1.2f;

            var centerPoint = new Vector2(w / 2, h / 2 + verticalOffset);

            flamePosition.TransformMatrix = Matrix3x2.CreateScale(1, 2, centerPoint);
        }

        /// <summary>
        /// Renders text into an intermediate bitmap and sets this bitmap as the input to the flame
        /// effect graph. The effect graph must already be created before calling this method.
        /// </summary>
        private void SetupText()
        {
            float w = (float)canvasSize.Width;
            float h = (float)canvasSize.Height;

            textRenderTarget = new CanvasRenderTarget(this.canvas, w, h);

            using (var ds = textRenderTarget.CreateDrawingSession())
            {
                ds.Clear(Color.FromArgb(0, 0, 0, 0));

                ds.DrawText(
                    textInput.Text,
                    w / 2,
                    h / 2,
                    Colors.White,
                    new CanvasTextFormat
                    {
                        FontFamily = "Segoe UI",
                        FontSize = GetFontSize(w),
                        ParagraphAlignment = Windows.UI.Text.ParagraphAlignment.Center,
                        VerticalAlignment = CanvasVerticalAlignment.Top
                    });
            }

            // Hook up the bitmap to the inputs of the flame effect graph.
            morphology.Source = textRenderTarget;
            composite.Inputs[1] = textRenderTarget;
        }

        /// <summary>
        /// Calculates a good font size so the text will fit even on smaller phone screens.
        /// </summary>
        private static float GetFontSize(float displayWidth)
        {
            const float maxFontSize = 72;
            const float scaleFactor = 12;

            return Math.Min(displayWidth / scaleFactor, maxFontSize);
        }

        /// <summary>
        /// Generate the flame effect graph. This method is called before the text bitmap (input)
        /// is created.
        /// </summary>
        private void CreateFlameEffect()
        {
            // Thicken the text bitmap.
            morphology = new MorphologyEffect
            {
                // Set the text bitmap as the source when it is created.
                Mode = MorphologyEffectMode.Dilate,
                Width = 7,
                Height = 1
            };

            // Blur, then colorize the text from black to red to orange as the alpha increases.
            var colorize = new ColorMatrixEffect
            {
                Source = new GaussianBlurEffect
                {
                    Source = morphology,
                    BlurAmount = 3f
                },
                ColorMatrix = new Matrix5x4
                {
                    M11 = 0f, M12 = 0f, M13 = 0f, M14 = 0f,
                    M21 = 0f, M22 = 0f, M23 = 0f, M24 = 0f,
                    M31 = 0f, M32 = 0f, M33 = 0f, M34 = 0f,
                    M41 = 0f, M42 = 1f, M43 = 0f, M44 = 1f,
                    M51 = 1f, M52 = -0.5f, M53 = 0f, M54 = 0f
                }
            };

            // Generate a Perlin noise field (see flamePosition).
            // Animate the noise by modifying flameAnimation's transform matrix at render time.
            flameAnimation = new Transform2DEffect
            {
                Source = new BorderEffect
                {
                    Source = new TurbulenceEffect
                    {
                        Frequency = new Vector2(0.109f, 0.109f),
                        Size = new Vector2(500.0f, 80.0f)
                    },
                    // Use Mirror extend mode to allow us to spatially translate the noise
                    // without any visible seams.
                    ExtendX = CanvasEdgeBehavior.Mirror,
                    ExtendY = CanvasEdgeBehavior.Mirror
                }
            };

            // Give the flame its wavy appearance by generating a displacement map from the noise
            // (see flameAnimation) and applying this to the text.
            // Stretch and position this flame behind the original text.
            flamePosition = new Transform2DEffect
            {
                Source = new DisplacementMapEffect
                {
                    Source = colorize,
                    Displacement = flameAnimation,
                    Amount = 40.0f
                }
                // Set the transform matrix at render time as it depends on window size.
            };

            // Composite the text over the flames.
            composite = new CompositeEffect()
            {
                Inputs = { flamePosition, null }
                // Replace null with the text bitmap when it is created.
            };
        }

        private void textInput_TextChanged(object sender, TextChangedEventArgs e)
        {
            SetupText();
        }
    }
}
