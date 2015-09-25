// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

using Microsoft.Graphics.Canvas;
using Microsoft.Graphics.Canvas.Geometry;
using Microsoft.Graphics.Canvas.Text;
using Microsoft.Graphics.Canvas.UI.Xaml;
using System;
using System.Collections.Generic;
using Windows.Foundation;
using Windows.Globalization;
using Windows.UI;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;

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
        
        struct WordBoundary { public int Start; public int Length; }
        List<WordBoundary> everyOtherWordBoundary = new List<WordBoundary>();

        public bool ShowNonOutlineText { get; set; }

        bool needsResourceRecreation;
        Size resourceRealizationSize;
        
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

        public TextOutlines()
        {
            this.InitializeComponent();

            CurrentTextEffectOption = TextEffectOption.None;
            CurrentTextDirection = CanvasTextDirection.TopToBottomThenLeftToRight;
            CurrentVerticalGlyphOrientation = CanvasVerticalGlyphOrientation.Default;
            CurrentTextLengthOption = TextLengthOption.Paragraph;

            if(ThumbnailGenerator.IsDrawingThumbnail)
            {
                CurrentTextDirection = CanvasTextDirection.LeftToRightThenTopToBottom;
                CurrentTextLengthOption = TextLengthOption.Short;
            }

            ShowNonOutlineText = true;
        }

        void EnsureResources(ICanvasResourceCreatorWithDpi resourceCreator, Size targetSize)
        {
            if (resourceRealizationSize != targetSize && !needsResourceRecreation)
                return;

            float canvasWidth = (float)targetSize.Width;
            float canvasHeight = (float)targetSize.Height;

            if (textLayout != null)
            {
                textLayout.Dispose();
                textGeometry.Dispose();
            }

            textLayout = CreateTextLayout(resourceCreator, canvasWidth, canvasHeight);
            textGeometry = CanvasGeometry.CreateText(textLayout);

            needsResourceRecreation = false;
            resourceRealizationSize = targetSize;
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
                testString = "This is some text which demonstrates Win2D's text-to-geometry option; there are sub-options of this test which apply lining options such as underline or strike-through. Additionally,  this example applies different text directions to ensure glyphs are transformed correctly.";
            }

            if(ThumbnailGenerator.IsDrawingThumbnail)
            {
                testString = "a";
            }

            for (int i = 0; i < testString.Length; ++i)
            {
                if (testString[i] == ' ')
                {
                    int nextSpace = testString.IndexOf(' ', i + 1);
                    int limit = nextSpace == -1 ? testString.Length : nextSpace;

                    WordBoundary wb = new WordBoundary();
                    wb.Start = i + 1;
                    wb.Length = limit - i - 1;
                    everyOtherWordBoundary.Add(wb);
                    i = limit;
                }
            }

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

            if(CurrentTextEffectOption == TextEffectOption.UnderlineEveryOtherWord)
            {
                foreach(WordBoundary wb in everyOtherWordBoundary)
                {
                    textLayout.SetUnderline(wb.Start, wb.Length, true);
                }
            }
            else if (CurrentTextEffectOption == TextEffectOption.StrikeEveryOtherWord)
            {
                foreach (WordBoundary wb in everyOtherWordBoundary)
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

        private void TextSampleOptions_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            needsResourceRecreation = true;
            canvas.Invalidate();
        }

        private void InvalidateCanvas(object sender, RoutedEventArgs e)
        {
            canvas.Invalidate();
        }

        private void TextDirectionOptions_SelectionChanged(object sender, SelectionChangedEventArgs e)
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
