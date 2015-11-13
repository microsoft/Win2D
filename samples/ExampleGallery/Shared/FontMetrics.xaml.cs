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
using Windows.UI.Text;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;

#if !WINDOWS_UWP
using Vector2 = Microsoft.Graphics.Canvas.Numerics.Vector2;
using Matrix3x2 = Microsoft.Graphics.Canvas.Numerics.Matrix3x2;
#endif

namespace ExampleGallery
{
    public sealed partial class FontMetrics : UserControl
    {
        CanvasTextLayout textLayout;

        public float CurrentFontSize { get; set; }
        public float CurrentUppercaseFontSize { get; set; }
        public float CurrentLowercaseFontSize { get; set; }
        public bool UniformStyle { get; set; }
        public bool UseBoldFace { get; set; }
        public bool UseItalicFace { get; set; }

        bool needsResourceRecreation;
        Size resourceRealizationSize;
        float sizeDim;
        bool defaultFontSizeSet;

        public FontMetrics()
        {
            this.InitializeComponent();

            fontPicker.SelectDefaultFont();
            uppercaseFontPicker.SelectDefaultFont();
            lowercaseFontPicker.SelectDefaultFont();

            UniformStyle = true;
        }

        void EnsureResources(ICanvasResourceCreatorWithDpi resourceCreator, Size targetSize)
        {
            if (resourceRealizationSize == targetSize && !needsResourceRecreation)
                return;

            float canvasWidth = (float)targetSize.Width;
            float canvasHeight = (float)targetSize.Height;
            sizeDim = Math.Min(canvasWidth, canvasHeight);

            if (!defaultFontSizeSet)
            {
                CurrentFontSize = sizeDim / 1.3f;
                CurrentFontSize = Math.Max((float)fontSizeSlider.Minimum, CurrentFontSize);
                CurrentFontSize = Math.Min((float)fontSizeSlider.Maximum, CurrentFontSize);
                fontSizeSlider.Value = CurrentFontSize;
                defaultFontSizeSet = true;
            }

            if (textLayout != null) 
                textLayout.Dispose();
            textLayout = CreateTextLayout(resourceCreator, canvasWidth, canvasHeight);

            if (!UniformStyle)
            {
                textLayout.SetFontSize(0, 1, CurrentUppercaseFontSize);
                textLayout.SetFontFamily(0, 1, uppercaseFontPicker.CurrentFontFamily);

                textLayout.SetFontSize(1, 2, CurrentLowercaseFontSize);
                textLayout.SetFontFamily(1, 2, lowercaseFontPicker.CurrentFontFamily);
            }

            needsResourceRecreation = false;
            resourceRealizationSize = targetSize;
        }

        class FontMetricsHolder : ICanvasTextRenderer
        {
            public struct Metrics
            {
                public float Ascent;
                public float LineGap;
                public float Descent;
                public float CapHeight;
                public float LowercaseLetterHeight;
            }

            public List<Metrics> GlyphRunMetrics = new List<Metrics>();

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
                Metrics m = new Metrics();
                m.Ascent = fontFace.Ascent;
                m.LineGap = fontFace.LineGap;
                m.Descent = fontFace.Descent;
                m.CapHeight = fontFace.CapHeight;
                m.LowercaseLetterHeight = fontFace.LowercaseLetterHeight;

                GlyphRunMetrics.Add(m);
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

            public bool PixelSnappingDisabled { get { return false; } }

            public Matrix3x2 Transform { get { return System.Numerics.Matrix3x2.Identity; } }
        }

        private CanvasTextLayout CreateTextLayout(ICanvasResourceCreator resourceCreator, float canvasWidth, float canvasHeight)
        {
            CanvasTextFormat textFormat = new CanvasTextFormat()
            {
                FontFamily = fontPicker.CurrentFontFamily,
                FontSize = CurrentFontSize,
                WordWrapping = CanvasWordWrapping.NoWrap,
                HorizontalAlignment = CanvasHorizontalAlignment.Center,
                VerticalAlignment = CanvasVerticalAlignment.Center,
                FontWeight = UseBoldFace ? FontWeights.Bold : FontWeights.Normal,
                FontStyle = UseItalicFace ? FontStyle.Italic : FontStyle.Normal
            };

            string testString = "Ajc";

            return new CanvasTextLayout(resourceCreator, testString, textFormat, canvasWidth, canvasHeight);
        }

        class Labeler
        {
            float fontSize;
            float sizeDim;
            float horizontalMidpoint;
            CanvasTextFormat leftJustifiedTextFormat, rightJustifiedTextFormat;
            Rect textLayoutBounds;
            int colorIndex;
            Color color;
            Vector2 layoutSize;
            float baselineInWorldSpace;
            FontMetricsHolder.Metrics glyphRunMetrics;
            CanvasDrawingSession drawingSession;

            public enum Side { Left, Right };

            static Color[] colors = { Colors.LightBlue, Colors.LightGoldenrodYellow, Colors.Lavender, Colors.Ivory, Colors.LightGray, };

            static float strokeWidth = 3.0f;

            static CanvasStrokeStyle dashedStroke = new CanvasStrokeStyle() { DashStyle = CanvasDashStyle.Dash };

            public Labeler(float f, float sd, Vector2 l, Rect tlb, float b, FontMetricsHolder.Metrics m, CanvasDrawingSession ds)
            {
                fontSize = f;
                sizeDim = sd;
                layoutSize = l;
                textLayoutBounds = tlb;
                baselineInWorldSpace = b;
                glyphRunMetrics = m;
                drawingSession = ds;

                horizontalMidpoint = (layoutSize.X / 2);

                leftJustifiedTextFormat = new CanvasTextFormat()
                {
                    VerticalAlignment = CanvasVerticalAlignment.Top,
                    HorizontalAlignment = CanvasHorizontalAlignment.Left
                };
                rightJustifiedTextFormat = new CanvasTextFormat()
                {
                    VerticalAlignment = CanvasVerticalAlignment.Top,
                    HorizontalAlignment = CanvasHorizontalAlignment.Right
                };

                NextLabel();
            }

            public static void DrawBaseline(CanvasDrawingSession ds, Vector2 layoutSize, float baselineInWorldSpace)
            {
                ds.DrawLine(0, baselineInWorldSpace, layoutSize.X, baselineInWorldSpace, Colors.LightGreen, strokeWidth * 2, dashedStroke);
                ds.DrawText("baseline", 100, baselineInWorldSpace, Colors.LightGreen);
            }

            public void DrawLineGap()
            {
                DrawLabel(drawingSession, "lineGap", -glyphRunMetrics.LineGap - glyphRunMetrics.Ascent, -glyphRunMetrics.Ascent, Labeler.Side.Right, 2);
            }

            public void DrawAscent()
            {
                DrawLabel(drawingSession, "ascent", -glyphRunMetrics.Ascent, 0, Labeler.Side.Left, 1);
            }

            public void DrawDescent()
            {
                DrawLabel(drawingSession, "descent", 0, glyphRunMetrics.Descent, Labeler.Side.Right, 3);
            }

            public void DrawCapitalHeight()
            {
                DrawLabel(drawingSession, "capital height", -glyphRunMetrics.CapHeight, 0, Labeler.Side.Left, 2);
            }

            public void DrawLowercaseHeight()
            {
                DrawLabel(drawingSession, "lowercase height", 0, -glyphRunMetrics.LowercaseLetterHeight, Labeler.Side.Right, 2);
            }

            void DrawLabel(CanvasDrawingSession ds, string text, float h1InEmSpace, float h2InEmSpace, Side whichSide, int tab)
            {
                //
                // The heights are offset from the baseline.
                //
                float h1 = baselineInWorldSpace + EmSpaceToWorldSpace(h1InEmSpace);
                float h2 = baselineInWorldSpace + EmSpaceToWorldSpace(h2InEmSpace);
                float midHeight = (h1 + h2) / 2;

                float amountPerTab = sizeDim / 7.0f;

                if (whichSide == Side.Left)
                {
                    float margin = tab * amountPerTab;

                    ds.DrawLine(margin, h1, margin, h2, color, strokeWidth);
                    ds.DrawLine(margin, h1, horizontalMidpoint, h1, color, strokeWidth);
                    ds.DrawLine(margin, h2, horizontalMidpoint, h2, color, strokeWidth);

                    ds.DrawLine(margin - amountPerTab, midHeight, margin, midHeight, color, strokeWidth);
                    ds.DrawText(text, margin, midHeight, color, rightJustifiedTextFormat);
                }
                else
                {
                    float rMargin = layoutSize.X - (tab * amountPerTab);
                    ds.DrawLine(rMargin, h1, rMargin, h2, color, strokeWidth);
                    ds.DrawLine(rMargin, h1, horizontalMidpoint, h1, color, strokeWidth);
                    ds.DrawLine(rMargin, h2, horizontalMidpoint, h2, color, strokeWidth);

                    ds.DrawLine(rMargin + amountPerTab, midHeight, rMargin, midHeight, color, strokeWidth);
                    ds.DrawText(text, rMargin, midHeight, color, leftJustifiedTextFormat);
                }

                NextLabel();
            }

            float EmSpaceToWorldSpace(float f)
            {
                return f * fontSize;
            }

            void NextLabel()
            {
                colorIndex = ((colorIndex + 1) % colors.Length);
                color = colors[colorIndex];
                color.A = 150;
            }

        }

        float GetGlyphSize(float fontSize, FontMetricsHolder.Metrics metrics)
        {
            //
            // This isn't a universally reliable way of determining baseline height
            // within a layout, but it's fine for this demo- since we've got no inline objects,
            // horizontal text and no custom line spacing.
            //
            return fontSize * (metrics.Ascent + metrics.LineGap);
        }

        float GetBaselineInWorldSpace(FontMetricsHolder fmh)
        {
            FontMetricsHolder.Metrics metrics;
            float fontScale;

            if (UniformStyle)
            {
                fontScale = CurrentFontSize;
                metrics = fmh.GlyphRunMetrics[0];
            }
            else
            {
                // Baseline is decided from whichever is the larger font selection.
                if (GetGlyphSize(CurrentUppercaseFontSize, fmh.GlyphRunMetrics[0]) > GetGlyphSize(CurrentLowercaseFontSize, fmh.GlyphRunMetrics[fmh.GlyphRunMetrics.Count - 1]))
                {
                    fontScale = CurrentUppercaseFontSize;
                    metrics = fmh.GlyphRunMetrics[0];
                }
                else
                {
                    fontScale = CurrentLowercaseFontSize;
                    metrics = fmh.GlyphRunMetrics[fmh.GlyphRunMetrics.Count - 1];
                }
            }

            return (float)textLayout.LayoutBounds.Top + (fontScale * (metrics.Ascent + metrics.LineGap));
        }

        private void Canvas_Draw(CanvasControl sender, CanvasDrawEventArgs args)
        {
            args.DrawingSession.Transform = System.Numerics.Matrix3x2.CreateTranslation(0, 5);
            EnsureResources(sender, sender.Size);

            FontMetricsHolder fmh = new FontMetricsHolder();
            textLayout.DrawToTextRenderer(fmh, new System.Numerics.Vector2(0, 0));

            args.DrawingSession.DrawTextLayout(textLayout, 0, 0, Colors.White);

            float baselineInWorldSpace = GetBaselineInWorldSpace(fmh);
            Labeler.DrawBaseline(args.DrawingSession, sender.Size.ToVector2(), baselineInWorldSpace);

            if(UniformStyle)
            {
                Labeler l = new Labeler(CurrentFontSize, sizeDim, sender.Size.ToVector2(), textLayout.LayoutBounds, baselineInWorldSpace, fmh.GlyphRunMetrics[0], args.DrawingSession);
                l.DrawAscent();
                l.DrawLineGap();
                l.DrawDescent();
                l.DrawCapitalHeight();
                l.DrawLowercaseHeight();
            }
            else
            {
                Labeler l1 = new Labeler(CurrentUppercaseFontSize, sizeDim, sender.Size.ToVector2(), textLayout.LayoutBounds, baselineInWorldSpace, fmh.GlyphRunMetrics[0], args.DrawingSession);
                l1.DrawLineGap();
                l1.DrawCapitalHeight();
                l1.DrawAscent();

                Labeler l2 = new Labeler(CurrentLowercaseFontSize, sizeDim, sender.Size.ToVector2(), textLayout.LayoutBounds, baselineInWorldSpace, fmh.GlyphRunMetrics[fmh.GlyphRunMetrics.Count - 1], args.DrawingSession);
                l2.DrawLineGap();
                l2.DrawLowercaseHeight();
                l2.DrawDescent();
            }
        }

        private void Canvas_CreateResources(CanvasControl sender, object args)
        {
            needsResourceRecreation = true;
        }

        private void UniformStyle_ValueChanged(object sender, RoutedEventArgs e)
        {
            if(UniformStyle)
            {
                fontSizeLabel.Visibility = Visibility.Visible;
                fontSizeSlider.Visibility = Visibility.Visible;
                fontLabel.Visibility = Visibility.Visible;
                fontPicker.Visibility = Visibility.Visible;

                uppercaseFontSizeLabel.Visibility = Visibility.Collapsed;
                uppercaseFontSizeSlider.Visibility = Visibility.Collapsed;
                lowercaseFontSizeLabel.Visibility = Visibility.Collapsed;
                lowercaseFontSizeSlider.Visibility = Visibility.Collapsed;
                uppercaseFontLabel.Visibility = Visibility.Collapsed;
                uppercaseFontPicker.Visibility = Visibility.Collapsed;
                lowercaseFontLabel.Visibility = Visibility.Collapsed;
                lowercaseFontPicker.Visibility = Visibility.Collapsed;

                fontSizeSlider.Value = (uppercaseFontSizeSlider.Value + lowercaseFontSizeSlider.Value) / 2;
                fontPicker.SelectFont(uppercaseFontPicker.CurrentFontFamily);
            }
            else
            {
                fontSizeLabel.Visibility = Visibility.Collapsed;
                fontSizeSlider.Visibility = Visibility.Collapsed;
                fontLabel.Visibility = Visibility.Collapsed;
                fontPicker.Visibility = Visibility.Collapsed;

                uppercaseFontSizeLabel.Visibility = Visibility.Visible;
                uppercaseFontSizeSlider.Visibility = Visibility.Visible;
                lowercaseFontSizeLabel.Visibility = Visibility.Visible;
                lowercaseFontSizeSlider.Visibility = Visibility.Visible;
                uppercaseFontLabel.Visibility = Visibility.Visible;
                uppercaseFontPicker.Visibility = Visibility.Visible;
                lowercaseFontLabel.Visibility = Visibility.Visible;
                lowercaseFontPicker.Visibility = Visibility.Visible;

                uppercaseFontSizeSlider.Value = fontSizeSlider.Value;
                lowercaseFontSizeSlider.Value = fontSizeSlider.Value;
                uppercaseFontPicker.SelectFont(fontPicker.CurrentFontFamily);
                lowercaseFontPicker.SelectFont(fontPicker.CurrentFontFamily);
            }
        }

        private void CheckBox_ValueChanged(object sender, RoutedEventArgs e)
        {
            needsResourceRecreation = true;

            canvas.Invalidate();
        }

        void Slider_ValueChanged(object sender, RangeBaseValueChangedEventArgs e)
        {
            needsResourceRecreation = true;

            if (canvas != null)
            {
                canvas.Invalidate();
            }
        }

        private void Canvas_SizeChanged(object sender, SizeChangedEventArgs e)
        {
            needsResourceRecreation = true;

            canvas.Invalidate();
        }

        private void fontPicker_SelectionChanged(object sender, SelectionChangedEventArgs e)
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
