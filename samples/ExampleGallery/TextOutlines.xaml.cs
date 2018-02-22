// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

using Microsoft.Graphics.Canvas;
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

namespace ExampleGallery
{
    public sealed partial class TextOutlines : UserControl
    {
        CanvasTextLayout textLayout;
        CanvasGeometry textGeometry;
        CanvasStrokeStyle dashedStroke = new CanvasStrokeStyle()
        {
            DashStyle = CanvasDashStyle.Dash
        };

        List<Utils.WordBoundary> everyOtherWordBoundary;

        public bool ShowNonOutlineText { get; set; }

        bool needsResourceRecreation;
        
        public List<CanvasTextDirection> TextDirectionOptions { get { return Utils.GetEnumAsList<CanvasTextDirection>(); } }
        public CanvasTextDirection CurrentTextDirection { get; set; }

        public List<CanvasVerticalGlyphOrientation> VerticalGlyphOrientationOptions { get { return Utils.GetEnumAsList<CanvasVerticalGlyphOrientation>(); } }
        public CanvasVerticalGlyphOrientation CurrentVerticalGlyphOrientation { get; set; }

        public enum TextLengthOption { Short, Paragraph };
        public List<TextLengthOption> TextLengthOptions { get { return Utils.GetEnumAsList<TextLengthOption>(); } }
        public TextLengthOption CurrentTextLengthOption { get; set; }

        public enum TextEffectOption { None, UnderlineEveryOtherWord, StrikeEveryOtherWord};
        public List<TextEffectOption> TextEffectOptions { get { return Utils.GetEnumAsList<TextEffectOption>(); } }
        public TextEffectOption CurrentTextEffectOption { get; set;  }

        //
        // Apps using text-to-geometry will typically use the 'Layout' option. 
        //
        // The 'GlyphRun' option exercises a lower-level API, and demonstrates 
        // how a custom text renderer could use text-to-geometry. The visual 
        // output between these two options should be identical.
        //
        public enum TextOutlineGranularity { Layout, GlyphRun }

        public List<TextOutlineGranularity> TextOutlineGranularityOptions { get { return Utils.GetEnumAsList<TextOutlineGranularity>(); } }
        public TextOutlineGranularity CurrentTextOutlineGranularityOption { get; set; }

        public TextOutlines()
        {
            this.InitializeComponent();

            CurrentTextEffectOption = TextEffectOption.None;
            CurrentTextDirection = CanvasTextDirection.TopToBottomThenLeftToRight;
            CurrentVerticalGlyphOrientation = CanvasVerticalGlyphOrientation.Default;
            CurrentTextLengthOption = TextLengthOption.Paragraph;
            CurrentTextOutlineGranularityOption = TextOutlineGranularity.GlyphRun;

            if (ThumbnailGenerator.IsDrawingThumbnail)
            {
                CurrentTextDirection = CanvasTextDirection.LeftToRightThenTopToBottom;
                CurrentTextLengthOption = TextLengthOption.Short;
            }

            ShowNonOutlineText = true;
            CurrentTextOutlineGranularityOption = TextOutlineGranularity.Layout;
        }

        class GlyphRunsToGeometryConverter : ICanvasTextRenderer
        {
            private List<CanvasGeometry> geometries = new List<CanvasGeometry>();
            private ICanvasResourceCreator resourceCreator;

            public GlyphRunsToGeometryConverter(ICanvasResourceCreator rc)
            {
                resourceCreator = rc;
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
                CanvasGeometry geometry = CanvasGeometry.CreateGlyphRun(
                    resourceCreator,
                    position,
                    fontFace,
                    fontSize,
                    glyphs,
                    isSideways,
                    bidiLevel,
                    measuringMode,
                    glyphOrientation);
                geometries.Add(geometry);
            }

            public CanvasGeometry GetGeometry()
            {
                return CanvasGeometry.CreateGroup(resourceCreator, geometries.ToArray());
            }

            private float GetGlyphOrientationInRadians(CanvasGlyphOrientation glyphOrientation)
            {
                switch (glyphOrientation)
                {
                    case CanvasGlyphOrientation.Upright: return 0;
                    case CanvasGlyphOrientation.Clockwise90Degrees: return (float)Math.PI / 2;
                    case CanvasGlyphOrientation.Clockwise180Degrees: return -(float)Math.PI;
                    case CanvasGlyphOrientation.Clockwise270Degrees:
                    default: return -(float)Math.PI / 2;
                }
            }

            private CanvasGeometry GetTransformedRectangle(
                float width,
                float thickness,
                float offset,
                Vector2 position,
                CanvasGlyphOrientation glyphOrientation)
            {
                var geometry = CanvasGeometry.CreateRectangle(
                    resourceCreator,
                    new Rect(0, offset, width, thickness));

                var rotate = System.Numerics.Matrix3x2.CreateRotation(GetGlyphOrientationInRadians(glyphOrientation));
                var translate = System.Numerics.Matrix3x2.CreateTranslation(position);

                return geometry.Transform(rotate * translate);
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
                var geometry = GetTransformedRectangle(strikethroughWidth, strikethroughThickness, strikethroughOffset, position, glyphOrientation);

                geometries.Add(geometry);
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
                var geometry = GetTransformedRectangle(underlineWidth, underlineThickness, underlineOffset, position, glyphOrientation);

                geometries.Add(geometry);
            }

            public void DrawInlineObject(
                Vector2 baselineOrigin,
                ICanvasTextInlineObject inlineObject,
                bool isSideways,
                bool isRightToLeft,
                object brush,
                CanvasGlyphOrientation glyphOrientation)
            {
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
                textGeometry.Dispose();
            }

            textLayout = CreateTextLayout(resourceCreator, (float)targetSize.Width, (float)targetSize.Height);

            if (CurrentTextOutlineGranularityOption == TextOutlineGranularity.Layout)
            {
                textGeometry = CanvasGeometry.CreateText(textLayout);
            }
            else
            {
                GlyphRunsToGeometryConverter converter = new GlyphRunsToGeometryConverter(resourceCreator);

                textLayout.DrawToTextRenderer(converter, 0, 0);

                textGeometry = converter.GetGeometry();
            }

            needsResourceRecreation = false;
        }

        private CanvasTextLayout CreateTextLayout(ICanvasResourceCreator resourceCreator, float canvasWidth, float canvasHeight)
        {
            string testString;

            if (CurrentTextLengthOption == TextLengthOption.Short)
            {
                testString = "one two";
            }
            else
            {
                testString = "This is some text which demonstrates Win2D's text-to-geometry option; there are sub-options of this test which apply lining options such as underline or strike-through. Additionally, this example applies different text directions to ensure glyphs are transformed correctly.";
            }

            if (ThumbnailGenerator.IsDrawingThumbnail)
            {
                testString = "a";
            }

            everyOtherWordBoundary = Utils.GetEveryOtherWord(testString);

            float sizeDim = Math.Min(canvasWidth, canvasHeight);         

            CanvasTextFormat textFormat = new CanvasTextFormat()
            {
                FontSize = CurrentTextLengthOption == TextLengthOption.Paragraph? sizeDim * 0.09f : sizeDim * 0.3f,
                Direction = CurrentTextDirection,
            };

            if (ThumbnailGenerator.IsDrawingThumbnail)
            {
                textFormat.FontSize = sizeDim * 0.75f;
            }

            CanvasTextLayout textLayout = new CanvasTextLayout(resourceCreator, testString, textFormat, canvasWidth, canvasHeight);

            if (CurrentTextEffectOption == TextEffectOption.UnderlineEveryOtherWord)
            {
                foreach (var wb in everyOtherWordBoundary)
                {
                    textLayout.SetUnderline(wb.Start, wb.Length, true);
                }
            }
            else if (CurrentTextEffectOption == TextEffectOption.StrikeEveryOtherWord)
            {
                foreach (var wb in everyOtherWordBoundary)
                {
                    textLayout.SetStrikethrough(wb.Start, wb.Length, true);
                }
            }

            textLayout.TrimmingGranularity = CanvasTextTrimmingGranularity.Character;
            textLayout.TrimmingSign = CanvasTrimmingSign.Ellipsis;

            textLayout.VerticalGlyphOrientation = CurrentVerticalGlyphOrientation;

            return textLayout;
        }

        private void Canvas_Draw(CanvasControl sender, CanvasDrawEventArgs args)
        {
            EnsureResources(sender, sender.Size);

            float strokeWidth = CurrentTextLengthOption == TextLengthOption.Paragraph ? 2.0f : 15.0f;

            args.DrawingSession.DrawGeometry(textGeometry, Colors.White, strokeWidth, dashedStroke);

            if (ShowNonOutlineText)
            {
                Color semitrans = Colors.CornflowerBlue;
                semitrans.A = 127;
                args.DrawingSession.DrawTextLayout(textLayout, 0, 0, semitrans);
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
