// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

using Microsoft.Graphics.Canvas;
using Microsoft.Graphics.Canvas.Effects;
using Microsoft.Graphics.Canvas.Text;
using Microsoft.Graphics.Canvas.UI;
using Microsoft.Graphics.Canvas.UI.Xaml;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Numerics;
using Windows.Foundation;
using Windows.UI;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace ExampleGallery
{
    public sealed partial class EffectRegionMapping : UserControl
    {
        const float width = 420;
        const float height = 200;
        const float gap = 16;

        CanvasRenderTarget sourceRenderTarget;
        CanvasRenderTarget destRenderTarget;

        GaussianBlurEffect blurEffect;
        ShadowEffect shadowEffect;
        CompositeEffect compositeEffect;

        // Use a separate CanvasTextLayout per character, so we can draw them individually.
        List<CanvasTextLayout> characterLayouts;
        List<Vector2> characterPositions;

        int highlightedCharacter;
        int previousHighlight;

        DispatcherTimer timer;


        public EffectRegionMapping()
        {
            this.InitializeComponent();

            timer = new DispatcherTimer();
            timer.Interval = TimeSpan.FromSeconds(0.5);
            timer.Tick += timer_Tick;
            timer.Start();
        }


        void timer_Tick(object sender, object e)
        {
            highlightedCharacter = (highlightedCharacter + 1) % characterLayouts.Count;

            canvas.Invalidate();
        }


        void canvas_CreateResources(CanvasControl sender, CanvasCreateResourcesEventArgs args)
        {
            // Fix the source rendertarget at 96 DPI, but let the destination rendertarget inherit
            // its DPI from the CanvasControl. This means their DPI will differ when run on a high
            // DPI display, so we can make sure the region mapping functions handle that correctly.
            sourceRenderTarget = new CanvasRenderTarget(sender, width, height, 96);
            destRenderTarget = new CanvasRenderTarget(sender, width, height);

            // Effect graph applies a subtle blur to the image, and also creates a shadow
            // with a larger blur amount and offset translation.
            blurEffect = new GaussianBlurEffect
            {
                Source = sourceRenderTarget,
                BlurAmount = 2
            };

            shadowEffect = new ShadowEffect
            {
                Source = sourceRenderTarget,
                BlurAmount = 6
            };

            compositeEffect = new CompositeEffect
            {
                Sources =
                {
                    new Transform2DEffect
                    {
                        Source = shadowEffect,
                        TransformMatrix = Matrix3x2.CreateTranslation(16, 16),
                    },

                    blurEffect
                }
            };

            // Arrange a bunch of text characters.
            var textFormat = new CanvasTextFormat
            {
                FontFamily = "Comic Sans MS",
                FontSize = 60,
            };

            characterLayouts = new List<CanvasTextLayout>();
            characterPositions = new List<Vector2>();

            var position = Vector2.Zero;

            for (char character = 'a'; character <= 'z'; character++)
            {
                var textLayout = new CanvasTextLayout(sender, character.ToString(), textFormat, width, height);

                characterLayouts.Add(textLayout);
                characterPositions.Add(position);

                if (character == 'm')
                {
                    position.X = 0;
                    position.Y += (float)textLayout.LayoutBounds.Height;
                }
                else
                {
                    position.X += (float)textLayout.LayoutBounds.Width;
                }
            }
        }


        void canvas_Draw(CanvasControl sender, CanvasDrawEventArgs args)
        {
            // We animate the source image by changing which character is highlighted in yellow.
            // Therefore there can be two changed regions: the highlighted character has changed from
            // white to yellow, while the previous highlight has changed from yellow back to white.

            // Look up the bounds of the two changed characters.
            var highlightBounds = GetCharacterBounds(highlightedCharacter);
            var previousBounds = GetCharacterBounds(previousHighlight);

            // Tell our effects that the highlighted character region has changed.
            blurEffect.InvalidateSourceRectangle(args.DrawingSession, 0, highlightBounds);
            shadowEffect.InvalidateSourceRectangle(args.DrawingSession, 0, highlightBounds);

            // Query what part of the output image will change as a result.
            var highlightInvalidRects = compositeEffect.GetInvalidRectangles(args.DrawingSession);
            var highlightInvalidUnion = GetRectangleUnion(highlightInvalidRects);

            // Also tell the effects about the previously highlighted character.
            blurEffect.InvalidateSourceRectangle(args.DrawingSession, 0, previousBounds);
            shadowEffect.InvalidateSourceRectangle(args.DrawingSession, 0, previousBounds);

            // Query the output region again. This will return a superset of highlightInvalidRects,
            // as it now accounts for the change to previousBounds as well as highlightBounds.
            var totalInvalidRects = compositeEffect.GetInvalidRectangles(args.DrawingSession);
            var totalInvalidUnion = GetRectangleUnion(totalInvalidRects);

            // We can also look up in the opposite direction: given that we are going to redraw only
            // the totalInvalidUnion area, what portion of each source image is needed to do that?
            // When using filter kernels like blur, this will be larger than just highlightBounds+previousBounds.
            var requiredSourceRects = compositeEffect.GetRequiredSourceRectangles(args.DrawingSession,
                                                                                  totalInvalidUnion,
                                                                                  new ICanvasEffect[] { blurEffect, shadowEffect },
                                                                                  new uint[] { 0, 0 },
                                                                                  new Rect[2] { sourceRenderTarget.Bounds, sourceRenderTarget.Bounds });

            // How about if we were going to redraw only highlightBounds, skipping previousBounds?
            // (we don't actually do this, but do display what source regions it would require).
            var blurSourceRect = compositeEffect.GetRequiredSourceRectangle(args.DrawingSession, highlightInvalidUnion, blurEffect, 0, sourceRenderTarget.Bounds);
            var shadowSourceRect = compositeEffect.GetRequiredSourceRectangle(args.DrawingSession, highlightInvalidUnion, shadowEffect, 0, sourceRenderTarget.Bounds);

            // Draw text into the source rendertarget.
            using (var drawingSession = sourceRenderTarget.CreateDrawingSession())
            {
                // To make sure the correct requiredSourceRects were reported, we clear the background outside
                // that region to magenta. If everything is working correctly, this should never be picked up by
                // effect drawing, as we only leave magenta in the areas we don't expect the effects to read from.
                drawingSession.Clear(Colors.Magenta);

                // Clear the requiredSourceRects to transparent.
                drawingSession.Blend = CanvasBlend.Copy;

                foreach (var r in requiredSourceRects)
                {
                    drawingSession.FillRectangle(r, Colors.Transparent);
                }

                // Draw the text characters.
                drawingSession.Blend = CanvasBlend.SourceOver;

                for (int i = 0; i < characterLayouts.Count; i++)
                {
                    var color = (i == highlightedCharacter) ? Colors.Yellow : Colors.White;

                    drawingSession.DrawTextLayout(characterLayouts[i], characterPositions[i], color);
                }
            }

            // Draw the effect graph (which reads from sourceRenderTarget) into destRenderTarget.
            using (var drawingSession = destRenderTarget.CreateDrawingSession())
            {
                // Slightly darken down the existing contents of the output rendertarget. This causes everything
                // except totalInvalidUnion to gradually fade out, so we can see which areas are getting redrawn.
                // If this FillRectangle was removed, the result of redrawing only the changed region would be
                // identical to if we redrew the whole thing every time (by removing the CreateLayer call).
                drawingSession.FillRectangle(destRenderTarget.Bounds, Color.FromArgb(16, 0, 0, 0));

                // Clip our drawing to the totalInvalidUnion rectangle,
                // which should be the only part of the output that has changed.
                using (var layer = drawingSession.CreateLayer(1, totalInvalidUnion))
                {
                    drawingSession.Clear(Colors.CornflowerBlue);
                    drawingSession.DrawImage(compositeEffect, totalInvalidUnion, totalInvalidUnion);
                }
            }

            if (!ThumbnailGenerator.IsDrawingThumbnail)
            {
                args.DrawingSession.Transform = Matrix3x2.CreateTranslation(gap, gap);

                // Display sourceRenderTarget.
                args.DrawingSession.DrawImage(sourceRenderTarget);

                // Display highlightBounds, blurSourceRect, and shadowSourceRect.
                args.DrawingSession.DrawRectangle(highlightBounds, Colors.Gray);
                args.DrawingSession.DrawRectangle(blurSourceRect, Colors.Blue);
                args.DrawingSession.DrawRectangle(shadowSourceRect, Colors.Blue);
            }

            args.DrawingSession.Transform = Matrix3x2.CreateTranslation(gap, gap * 2 + height);

            // Display destRenderTarget.
            args.DrawingSession.DrawImage(destRenderTarget);

            // Display highlightInvalidRects.
            foreach (var i in highlightInvalidRects)
            {
                args.DrawingSession.DrawRectangle(i, Colors.DarkBlue);
            }

            previousHighlight = highlightedCharacter;

            // When generating thumbnails, repeat the first draw a bunch of times to reach a more interesting image.
            if (ThumbnailGenerator.IsDrawingThumbnail && highlightedCharacter < characterLayouts.Count * 5 / 6)
            {
                highlightedCharacter++;
                canvas_Draw(sender, args);
            }
        }


        Rect GetCharacterBounds(int characterIndex)
        {
            var bounds = characterLayouts[characterIndex].DrawBounds;

            bounds.X += characterPositions[characterIndex].X;
            bounds.Y += characterPositions[characterIndex].Y;

            return bounds;
        }


        Rect GetRectangleUnion(Rect[] rectangles)
        {
            var union = rectangles.Aggregate(RectHelper.Union);

            if (double.IsInfinity(union.Width) || double.IsInfinity(union.Height))
            {
                union = destRenderTarget.Bounds;
            }

            return union;
        }


        void control_Unloaded(object sender, RoutedEventArgs e)
        {
            // Explicitly remove references to allow the Win2D controls to get garbage collected
            canvas.RemoveFromVisualTree();
            canvas = null;

            timer.Stop();
        }
    }
}
