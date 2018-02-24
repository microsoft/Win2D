// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

using Microsoft.Graphics.Canvas;
using Microsoft.Graphics.Canvas.Brushes;
using Microsoft.Graphics.Canvas.Geometry;
using Microsoft.Graphics.Canvas.Text;
using Microsoft.Graphics.Canvas.UI.Xaml;
using System;
using System.Collections.Generic;
using System.Numerics;
using Windows.Foundation;
using Windows.UI;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

//
// This example has some similar features to the TextOutlines demo. 
// However this example doesn't use a custom renderer to draw geometry.
// Rather, it has a custom renderer which uses DrawGlyphRun to draw text.
//
// This shows how to implement a custom text renderer which handles glyph
// rotation. Glyph rotation is based off of the text orientation,
// and whether vertical glyphs are stacked or rotated.
//
// When constructing geometry from text, Win2D bakes that transform
// into the geometry. Since apps may not always want to use text-as-geometry,
// CanvasTextLayout.GetGlyphOrientationTransform is appropriate for the general 
// case where we're drawing glyphs using DrawGlyphRun to a CanvasDrawingSession.
//

namespace ExampleGallery
{
    public sealed partial class GlyphRotation : UserControl
    {
        CanvasTextLayout textLayout;
        CanvasGeometry textReference;
        List<Utils.WordBoundary> everyOtherWordBoundary;
        CanvasSolidColorBrush textBrush;
        string testString;

        public bool ShowNonCustomText { get; set; }

        bool needsResourceRecreation;
        
        public List<CanvasTextDirection> TextDirectionOptions { get { return Utils.GetEnumAsList<CanvasTextDirection>(); } }
        public CanvasTextDirection CurrentTextDirection { get; set; }

        public List<CanvasVerticalGlyphOrientation> VerticalGlyphOrientationOptions { get { return Utils.GetEnumAsList<CanvasVerticalGlyphOrientation>(); } }
        public CanvasVerticalGlyphOrientation CurrentVerticalGlyphOrientation { get; set; }

        public enum TextEffectOption { None, UnderlineEveryOtherWord, StrikeEveryOtherWord};
        public List<TextEffectOption> TextEffectOptions { get { return Utils.GetEnumAsList<TextEffectOption>(); } }
        public TextEffectOption CurrentTextEffectOption { get; set;  }

        public GlyphRotation()
        {
            this.InitializeComponent();

            DataContext = this;

            CurrentTextEffectOption = TextEffectOption.UnderlineEveryOtherWord;
            CurrentTextDirection = CanvasTextDirection.RightToLeftThenBottomToTop;
            CurrentVerticalGlyphOrientation = CanvasVerticalGlyphOrientation.Default;

            ShowNonCustomText = true;

            testString = "This demonstrates the correct way to do glyph rotation from a custom text renderer in Win2D. Of course, if your script is always read left-to-right then top-to-bottom, there is no need to handle rotation. However, in general, callers of DrawGlyphRun should ensure that the correct transform is used- particularly for vertical text.";

            if (ThumbnailGenerator.IsDrawingThumbnail)
            {
                testString = "Glyph rotation";
                CurrentTextDirection = CanvasTextDirection.TopToBottomThenLeftToRight;
            }

            everyOtherWordBoundary = Utils.GetEveryOtherWord(testString);
        }

        static CanvasDrawingSession drawingSession;

        class CustomTextRenderer : ICanvasTextRenderer
        {
            CanvasSolidColorBrush textBrush;

            public CustomTextRenderer(CanvasSolidColorBrush brush)
            {
                textBrush = brush;
            }

            public void DrawGlyphRun(
                Vector2 position,
                CanvasFontFace fontFace,
                float fontSize,
                CanvasGlyph[] glyphs,
                bool isSideways,
                uint bidiLevel,
                object brush,
                CanvasTextMeasuringMode measuringMode,
                string locale,
                string textString,
                int[] custerMapIndices,
                uint textPosition,
                CanvasGlyphOrientation glyphOrientation)
            {
                if (glyphs == null)
                    return;

                var previousTransform = drawingSession.Transform;

                drawingSession.Transform = CanvasTextLayout.GetGlyphOrientationTransform(glyphOrientation, isSideways, position);

                drawingSession.DrawGlyphRun(
                    position,
                    fontFace,
                    fontSize,
                    glyphs,
                    isSideways,
                    bidiLevel,
                    textBrush);

                drawingSession.Transform = previousTransform;
            }

            private Rect RotateRectangle(Rect r, CanvasGlyphOrientation orientation)
            {
                switch (orientation)
                {
                    case CanvasGlyphOrientation.Clockwise90Degrees:
                        return new Rect(new Point(-r.Top, r.Left), new Point(-r.Bottom, r.Right));
                    case CanvasGlyphOrientation.Clockwise180Degrees:
                        return new Rect(new Point(-r.Left, -r.Top), new Point(-r.Right, -r.Bottom));
                    case CanvasGlyphOrientation.Clockwise270Degrees:
                        return new Rect(new Point(r.Top, -r.Left), new Point(r.Bottom, -r.Right));
                    case CanvasGlyphOrientation.Upright:
                    default:
                        return r;
                }
            }

            private Rect GetLineRectangle(
                Vector2 position,
                float lineWidth,
                float lineThickness,
                float lineOffset,
                CanvasGlyphOrientation glyphOrientation)
            {
                Rect rect = new Rect(0, lineOffset, lineWidth, lineThickness);
                rect = RotateRectangle(rect, glyphOrientation);
                rect.X += position.X;
                rect.Y += position.Y;
                return rect;
            }

            public void DrawStrikethrough(
                Vector2 position,
                float strikethroughWidth,
                float strikethroughThickness,
                float strikethroughOffset,
                CanvasTextDirection textDirection,
                object brush,
                CanvasTextMeasuringMode measuringMode,
                string locale,
                CanvasGlyphOrientation glyphOrientation)
            {
                drawingSession.FillRectangle(
                    GetLineRectangle(position, strikethroughWidth, strikethroughThickness, strikethroughOffset, glyphOrientation),
                    textBrush);
            }

            public void DrawUnderline(
                Vector2 position,
                float underlineWidth,
                float underlineThickness,
                float underlineOffset,
                float runHeight,
                CanvasTextDirection textDirection,
                object brush,
                CanvasTextMeasuringMode measuringMode,
                string locale,
                CanvasGlyphOrientation glyphOrientation)
            {
                drawingSession.FillRectangle(
                    GetLineRectangle(position, underlineWidth, underlineThickness, underlineOffset, glyphOrientation),
                    textBrush);
            }

            public void DrawInlineObject(
                Vector2 baselineOrigin,
                ICanvasTextInlineObject inlineObject,
                bool isSideways,
                bool isRightToLeft,
                object brush,
                CanvasGlyphOrientation glyphOrientation)
            {
                // There aren't any inline objects in this sample.
            }

            public float Dpi { get { return 96; } }

            public bool PixelSnappingDisabled { get { return true; } }

            public Matrix3x2 Transform { get { return System.Numerics.Matrix3x2.Identity; } }

        }

        void EnsureResources(ICanvasResourceCreatorWithDpi resourceCreator, Size targetSize)
        {
            if (!needsResourceRecreation)
                return;

            if (textLayout != null)
            {
                textLayout.Dispose();
                textReference.Dispose();
                textBrush.Dispose();
            }

            textLayout = CreateTextLayout(resourceCreator, (float)targetSize.Width, (float)targetSize.Height);

            textReference = CanvasGeometry.CreateText(textLayout);

            textBrush = new CanvasSolidColorBrush(resourceCreator, Colors.LightBlue);

            needsResourceRecreation = false;
        }

        private CanvasTextLayout CreateTextLayout(ICanvasResourceCreator resourceCreator, float canvasWidth, float canvasHeight)
        {
            float sizeDim = Math.Min(canvasWidth, canvasHeight);         

            CanvasTextFormat textFormat = new CanvasTextFormat()
            {
                FontSize = ThumbnailGenerator.IsDrawingThumbnail? sizeDim * 0.25f : sizeDim * 0.085f,
                Direction = CurrentTextDirection,
            };

            CanvasTextLayout textLayout = new CanvasTextLayout(resourceCreator, testString, textFormat, canvasWidth, canvasHeight);

            if (CurrentTextEffectOption == TextEffectOption.UnderlineEveryOtherWord)
            {
                foreach (Utils.WordBoundary wb in everyOtherWordBoundary)
                {
                    textLayout.SetUnderline(wb.Start, wb.Length, true);
                }
            }
            else if (CurrentTextEffectOption == TextEffectOption.StrikeEveryOtherWord)
            {
                foreach (Utils.WordBoundary wb in everyOtherWordBoundary)
                {
                    textLayout.SetStrikethrough(wb.Start, wb.Length, true);
                }
            }

            textLayout.VerticalGlyphOrientation = CurrentVerticalGlyphOrientation;

            return textLayout;
        }

        private void Canvas_Draw(CanvasControl sender, CanvasDrawEventArgs args)
        {
            EnsureResources(sender, sender.Size);

            drawingSession = args.DrawingSession;

            CustomTextRenderer textRenderer = new CustomTextRenderer(textBrush);
            textLayout.DrawToTextRenderer(textRenderer, 0, 0);

            if (ShowNonCustomText)
            {
                Color semitrans = Colors.White;
                semitrans.A = 127;

                float strokeWidth = ThumbnailGenerator.IsDrawingThumbnail ? 22.0f : 5.0f;
                args.DrawingSession.DrawGeometry(textReference, semitrans, strokeWidth);
            }
        }

        private void Canvas_CreateResources(CanvasControl sender, object args)
        {
            needsResourceRecreation = true;
        }

        private void Canvas_SizeChanged(object sender, SizeChangedEventArgs e)
        {
            needsResourceRecreation = true;
        }

        private void InvalidateCanvas(object sender, RoutedEventArgs e)
        {
            needsResourceRecreation = true;
            canvas.Invalidate();
        }

        private void control_Unloaded(object sender, RoutedEventArgs e)
        {
            // Explicitly remove references to allow the Win2D controls to get garbage collected
            canvas.RemoveFromVisualTree();
            canvas = null;
        }
    }
}
