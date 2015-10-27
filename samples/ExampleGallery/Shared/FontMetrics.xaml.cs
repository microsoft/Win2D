// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

using Microsoft.Graphics.Canvas;
using Microsoft.Graphics.Canvas.Geometry;
using Microsoft.Graphics.Canvas.Text;
using Microsoft.Graphics.Canvas.UI.Xaml;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Numerics;
using Windows.Foundation;
using Windows.Globalization;
using Windows.UI;
using Windows.UI.Text;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Media;

namespace ExampleGallery
{
    public sealed partial class FontMetrics : UserControl
    {
        CanvasTextLayout textLayout;
        CanvasFontFace fontFace;

        public float CurrentFontSize { get; set; }
        public bool UseBoldFace { get; set; }
        public bool UseItalicFace { get; set; }

        public string CurrentFontFamilyName { get; set; }

        //
        // These are a few fonts that are built-in on all Win2D's platforms, have standard italic/bold faces
        // with standard names.
        //
        public string[] FontFamilies { get { return new string[]{ "Arial", "Segoe UI", "Comic Sans MS"}; } }

        bool needsResourceRecreation;
        Size resourceRealizationSize;
        float sizeDim;
        bool defaultFontSizeSet;

        public FontMetrics()
        {
            this.InitializeComponent();

            CurrentFontFamilyName = "Arial";

            this.fontFace = FindFontFace(CurrentFontFamilyName, UseBoldFace, UseItalicFace);
        }

        CanvasFontFace FindFontFace(string fontFamily, bool useBold, bool useItalic)
        {
            var systemFonts = CanvasFontSet.SystemFontSet;

            //
            // TODO: #5770 Expose all the system fonts through the UI, 
            // and use Win2D to perform the mapping of the family name to the font face object.
            //
            string faceName = "Regular";
            if (useBold && useItalic)
                faceName = "Bold Italic";
            else if (useBold)
                faceName = "Bold";
            else if (useItalic)
                faceName = "Italic";

            foreach (var font in systemFonts.Fonts)
            {
                var familyNames = font.GetInformationalStrings(CanvasFontInformation.Win32FamilyNames);

                //
                // This is fixed at querying the 'en-us' locale name. Reason 
                // being, we don't want to bother looking up the face names 
                // for each locale, and because many locales do not have 
                // a specific family or face name defined at all.
                //
                if (familyNames["en-us"] == fontFamily)
                {
                    if (font.FaceNames["en-us"] == faceName)
                    {
                        return font;
                    }
                }
            }
            return null;
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

            needsResourceRecreation = false;
            resourceRealizationSize = targetSize;
        }

        private CanvasTextLayout CreateTextLayout(ICanvasResourceCreator resourceCreator, float canvasWidth, float canvasHeight)
        {
            CanvasTextFormat textFormat = new CanvasTextFormat()
            {
                FontFamily = CurrentFontFamilyName,
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
            CanvasStrokeStyle dashedStroke;
            float strokeWidth;
            Vector2 layoutSize;

            public enum Side { Left, Right };

            static Color[] colors = { Colors.LightBlue, Colors.LightGoldenrodYellow, Colors.Lavender, Colors.Ivory, Colors.LightGray, };

            public Labeler(float f, float sd, Vector2 l, Rect tlb)
            {
                fontSize = f;
                sizeDim = sd;
                layoutSize = l;
                textLayoutBounds = tlb;
                strokeWidth = 3.0f;

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
                dashedStroke = new CanvasStrokeStyle()
                {
                    DashStyle = CanvasDashStyle.Dash
                };

                NextLabel();
            }

            public void DrawBaseline(CanvasDrawingSession ds, string text, float heightInEmSpace)
            {
                float height = (float)textLayoutBounds.Top + EmSpaceToWorldSpace(heightInEmSpace);

                ds.DrawLine(0, height, layoutSize.X, height, Colors.LightGreen, strokeWidth * 2, dashedStroke);
                ds.DrawText(text, 100, height, Colors.LightGreen);
            }

            public void DrawLabel(CanvasDrawingSession ds, string text, float h1InEmSpace, float h2InEmSpace, Side whichSide, int tab)
            {
                float h1 = (float)textLayoutBounds.Top + EmSpaceToWorldSpace(h1InEmSpace);
                float h2 = (float)textLayoutBounds.Top + EmSpaceToWorldSpace(h2InEmSpace);
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

        private void Canvas_Draw(CanvasControl sender, CanvasDrawEventArgs args)
        {
            args.DrawingSession.Transform = Matrix3x2.CreateTranslation(0, 5);
            EnsureResources(sender, sender.Size);

            Labeler l = new Labeler(CurrentFontSize, sizeDim, sender.Size.ToVector2(), textLayout.LayoutBounds);

            float baselineY = fontFace.Ascent + fontFace.LineGap;
            l.DrawBaseline(args.DrawingSession, "baseline", baselineY);

            args.DrawingSession.DrawTextLayout(textLayout, 0, 0, Colors.White);

            l.DrawLabel(args.DrawingSession, "ascent", fontFace.LineGap, baselineY, Labeler.Side.Left, 1);
            l.DrawLabel(args.DrawingSession, "lineGap", 0, fontFace.LineGap, Labeler.Side.Right, 2);
            l.DrawLabel(args.DrawingSession, "descent", baselineY, baselineY + fontFace.Descent, Labeler.Side.Right, 3);
            l.DrawLabel(args.DrawingSession, "capital height", baselineY - fontFace.CapHeight, baselineY, Labeler.Side.Left, 2);
            l.DrawLabel(args.DrawingSession, "lowercase height", baselineY - fontFace.LowercaseLetterHeight, baselineY, Labeler.Side.Right, 2);

        }

        private void Canvas_CreateResources(CanvasControl sender, object args)
        {
            needsResourceRecreation = true;
        }

        private void CheckBox_ValueChanged(object sender, RoutedEventArgs e)
        {
            this.fontFace = FindFontFace(CurrentFontFamilyName, UseBoldFace, UseItalicFace);

            needsResourceRecreation = true;

            canvas.Invalidate();
        }

        private void ComboBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            this.fontFace = FindFontFace(CurrentFontFamilyName, UseBoldFace, UseItalicFace);

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


        private void control_Unloaded(object sender, RoutedEventArgs e)
        {
            // Explicitly remove references to allow the Win2D controls to get garbage collected
            canvas.RemoveFromVisualTree();
            canvas = null;
        }

    }
}
