// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

using Microsoft.Graphics.Canvas;
using Microsoft.Graphics.Canvas.Brushes;
using Microsoft.Graphics.Canvas.Text;
using Microsoft.Graphics.Canvas.UI.Xaml;
using System;
using System.Collections.Generic;
using System.Diagnostics;
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
    //
    // This sample demonstrates a way to use subscript and superscript with
    // arbitrary fonts. Unlike the subscript/superscript typography option,
    // which only works on select fonts, and can be limited to specific
    // character ranges, this approach will work on any piece of text
    // with any font. 
    // 
    // When given the option, it's usually preferable to use the
    // superscript and subscript typographic feature rather than
    // do what this example does, since it will tend to have better visual 
    // consistency with the rest of the text. 
    //
    // This approach does two things:
    //   - Scales superscript/subscript text down, by changing the font size on
    //     the text layout.
    //   - Adjusts the baseline of characters to be higher or lower using a 
    //     custom text renderer.
    //
    // The functions SetSubscript() and SetSuperscript(), which take a text layout
    // and character range, are what kicks things off.
    //
    public sealed partial class SubscriptSuperscript : UserControl
    {
        CanvasTextLayout textLayout;
        CanvasSolidColorBrush textBrush;

        public float CurrentFontSize { get; set; }
        public bool UseBoldFace { get; set; }
        public bool UseItalicFace { get; set; }
        public bool ShowUnformatted { get; set; }

        public enum TextSampleOption
        {
            ChemicalFormula,
            RightTriangle,
            ShortExpression
        }
        public List<TextSampleOption> TextSampleOptions { get { return Utils.GetEnumAsList<TextSampleOption>(); } }
        public TextSampleOption CurrentTextSampleOption { get; set; }
        
        //
        // When implementing an actual subscript/superscript typography option,
        // a font author will fine-tune the placement of baselines based on how the font
        // itself looks and is measured. Adjusting these values may look better
        // on some fonts.
        // We picked some reasonable default choices for these.
        //
        const float fontSizeShrinkAmount = 0.65f;
        const float subscriptBaselineScale = 0.2f;
        const float superscriptBaselineScale = 0.7f;
    
        bool needsResourceRecreation;
        Size resourceRealizationSize;
        float sizeDim;
        bool defaultFontSizeSet;

        public SubscriptSuperscript()
        {
            this.InitializeComponent();

            if (ThumbnailGenerator.IsDrawingThumbnail)
                CurrentTextSampleOption = TextSampleOption.ShortExpression;
        }

        void EnsureResources(ICanvasResourceCreatorWithDpi resourceCreator, Size targetSize)
        {
            if (resourceRealizationSize == targetSize && !needsResourceRecreation)
                return;

            float canvasWidth = (float)targetSize.Width;
            float canvasHeight = (float)targetSize.Height;
            sizeDim = Math.Min(canvasWidth, canvasHeight);
            
            textBrush = new CanvasSolidColorBrush(resourceCreator, Colors.Thistle);

            if (!defaultFontSizeSet)
            {
                if (ThumbnailGenerator.IsDrawingThumbnail)
                {
                    CurrentFontSize = 180;
                }
                else
                {
                    CurrentFontSize = sizeDim / 20;
                }
                CurrentFontSize = Math.Max((float)fontSizeSlider.Minimum, CurrentFontSize);
                CurrentFontSize = Math.Min((float)fontSizeSlider.Maximum, CurrentFontSize);
                fontSizeSlider.Value = CurrentFontSize;
                defaultFontSizeSet = true;
            }
            
            string sampleText = null;
            switch (CurrentTextSampleOption)
            {
                case TextSampleOption.ChemicalFormula:
                    sampleText =
                        "H2O is the chemical formula for water.\r\n\r\n" +
                        "And, the isotope Carbon-12 may be written as 12C.\r\n\r\n" +
                        "Often, chemical formulas make use of both superscript and subscript text.";
                    break;
                case TextSampleOption.RightTriangle:
                    sampleText =
                        "The side lengths of a right-angle triangle can be written as a2 + b2 = c2.\r\n\r\n" +
                        "If the triangle's shorter sides are lengths 3 and 4, the remaining side must be 5, since 32 + 42 = 52.";
                    break;
                case TextSampleOption.ShortExpression:
                    sampleText = "ax2by3";
                    break;
                default:
                    Debug.Assert(false, "Unexpected text sample option");
                    break;
            }

            if (textLayout != null)
                textLayout.Dispose();
            textLayout = CreateTextLayout(sampleText, resourceCreator, canvasWidth, canvasHeight);

            switch (CurrentTextSampleOption)
            {
                case TextSampleOption.ChemicalFormula:
                    SetSubscript(textLayout, sampleText.IndexOf("H2O") + 1, 1);
                    SetSuperscript(textLayout, sampleText.IndexOf("12C"), 2);
                    SetSubscript(textLayout, sampleText.IndexOf("subscript"), "subscript".Length);
                    SetSuperscript(textLayout, sampleText.IndexOf("superscript"), "superscript".Length);
                    break;
                case TextSampleOption.RightTriangle:
                    for (int i = 0; i < sampleText.Length; ++i)
                    {
                        if (sampleText[i] == '2')
                            SetSuperscript(textLayout, i, 1);
                    }
                    break;
                case TextSampleOption.ShortExpression:
                    SetSubscript(textLayout, 1, 1);
                    SetSuperscript(textLayout, 2, 1);
                    SetSubscript(textLayout, 4, 1);
                    SetSuperscript(textLayout, 5, 1);
                    break;
                default:
                    Debug.Assert(false, "Unexpected text sample option");
                    break;
            }
            
            subscriptSuperscriptRenderer = new SubscriptSuperscriptRenderer();

            needsResourceRecreation = false;
            resourceRealizationSize = targetSize;
        }

        //
        // We need some means of telling the text layout which characters
        // are subscript and which are superscript; keying off the font size
        // isn't enough, if we want to be able to mix subscript and superscript
        // together.
        //
        // CustomBrushData is a piece of metadata we can attach to characters
        // which describes whether they're superscript or subscript. Since it's all 
        // handled by SetCustomBrush, we don't need to buffer any data, do any thinking
        // about character ranges, or optimize things for where formatting changes are.
        //
        // SetCustomBrush is a really flexible way of attaching some supplementary
        // drawing data to individual characters, and here it's a way of informing
        // SubscriptSuperscriptRenderer how to adjust baselines.
        //
        // If you'd prefer not to use CustomBrushData, or for some reason you need
        // to set the custom brush to something else, it'd also work to store 
        // superscript/subscript info in the text renderer itself.
        // 
        class CustomBrushData
        {
            public enum BaselineAdjustmentType { None, Raise, Lower }
            public BaselineAdjustmentType BaselineAdjustment;
        }

        private void ShrinkFontAndAttachCustomBrushData(
            CanvasTextLayout textLayout, 
            int textPosition, 
            int characterCount, 
            CustomBrushData.BaselineAdjustmentType baselineAdjustmentType)
        {
            textLayout.SetFontSize(textPosition, characterCount, (float)CurrentFontSize * fontSizeShrinkAmount);
            textLayout.SetCustomBrush(textPosition, characterCount, new CustomBrushData() { BaselineAdjustment = baselineAdjustmentType });
        }
       
        private void SetSubscript(CanvasTextLayout textLayout, int textPosition, int characterCount)
        {
            ShrinkFontAndAttachCustomBrushData(textLayout, textPosition, characterCount, CustomBrushData.BaselineAdjustmentType.Lower);
        }

        private void SetSuperscript(CanvasTextLayout textLayout, int textPosition, int characterCount)
        {
            ShrinkFontAndAttachCustomBrushData(textLayout, textPosition, characterCount, CustomBrushData.BaselineAdjustmentType.Raise);
        }
  
        SubscriptSuperscriptRenderer subscriptSuperscriptRenderer = null;

        // 
        // There's a limitation to this approach of shrinking text/adjusting baselines, 
        // worth calling out. If there's an entire line of just subscript or an entire line
        // of just superscript, the overall line height will be short compared to full-size
        // text. Admittedly, this is a rather contrived and rare situation; it can be fixed by
        // inserting invisible full-size whitespace characters into your text. This approach
        // works for a vast majority of cases.
        //
        class SubscriptSuperscriptRenderer : ICanvasTextRenderer
        {
            public CanvasDrawingSession DrawingSession;
            public CanvasSolidColorBrush TextBrush;
            
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
                int[] clusterMapIndices,
                uint startingTextPosition,
                CanvasGlyphOrientation glyphOrientation)
            {
                if (glyphs == null || glyphs.Length == 0)
                    return;

                float scaledFontAscent = fontFace.Ascent * fontSize;
                float subscriptBaselineDropAmount = scaledFontAscent * subscriptBaselineScale;
                float superscriptBaselineRaiseAmount = scaledFontAscent * superscriptBaselineScale;

                // Draw glyph-by-glyph.
                for (int i = 0; i < glyphs.Length; ++i)
                {
                    CanvasGlyph[] singleGlyph = new CanvasGlyph[1];
                    singleGlyph[0] = glyphs[i];

                    Vector2 positionForThisGlyph = position;

                    CustomBrushData brushData = (CustomBrushData)brush;
                    if (brushData != null)
                    {
                        if (brushData.BaselineAdjustment == CustomBrushData.BaselineAdjustmentType.Lower)
                        {
                            positionForThisGlyph.Y += subscriptBaselineDropAmount;
                        }
                        else if (brushData.BaselineAdjustment == CustomBrushData.BaselineAdjustmentType.Raise)
                        {
                            positionForThisGlyph.Y -= superscriptBaselineRaiseAmount;
                        }
                    }

                    DrawingSession.DrawGlyphRun(
                        positionForThisGlyph,
                        fontFace,
                        fontSize,
                        singleGlyph,
                        isSideways,
                        bidiLevel,
                        TextBrush);

                    position.X += glyphs[i].Advance;
                }
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
                // Normally, we'd add strikethrough support here. Strikethrough isn't used by this demo.
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
                // Normally, we'd add underline support here. Underline isn't used by this demo.
            }

            public void DrawInlineObject(
                Vector2 baselineOrigin,
                ICanvasTextInlineObject inlineObject,
                bool isSideways,
                bool isRightToLeft,
                object brush,
                CanvasGlyphOrientation glyphOrientation)
            {
                // This sample doesn't use inline objects.
            }

            public float Dpi { get { return 96; } }

            public bool PixelSnappingDisabled { get { return false; } }

            public Matrix3x2 Transform { get { return System.Numerics.Matrix3x2.Identity; } }
        }


        private CanvasTextLayout CreateTextLayout(string sampleText, ICanvasResourceCreator resourceCreator, float canvasWidth, float canvasHeight)
        {
            CanvasTextFormat textFormat = new CanvasTextFormat()
            {
                FontFamily = fontPicker.CurrentFontFamily,
                FontSize = CurrentFontSize,
                WordWrapping = CanvasWordWrapping.Wrap,
                HorizontalAlignment = CanvasHorizontalAlignment.Left,
                VerticalAlignment = CanvasVerticalAlignment.Center,
                FontWeight = UseBoldFace ? FontWeights.Bold : FontWeights.Normal,
                FontStyle = UseItalicFace ? FontStyle.Italic : FontStyle.Normal
            };
            
            return new CanvasTextLayout(resourceCreator, sampleText, textFormat, canvasWidth, canvasHeight);
        }

        private void Canvas_Draw(CanvasControl sender, CanvasDrawEventArgs args)
        {
            EnsureResources(sender, sender.Size);
            
            if (ShowUnformatted)
            {
                args.DrawingSession.DrawTextLayout(textLayout, 0, 0, Colors.DarkGray);
            }

            subscriptSuperscriptRenderer.DrawingSession = args.DrawingSession;
            subscriptSuperscriptRenderer.TextBrush = textBrush;

            textLayout.DrawToTextRenderer(subscriptSuperscriptRenderer, new System.Numerics.Vector2(0, 0));
        }

        private void Canvas_CreateResources(CanvasControl sender, object args)
        {
            needsResourceRecreation = true;
        }

        private void TextSampleOptions_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            needsResourceRecreation = true;
            canvas.Invalidate();
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
