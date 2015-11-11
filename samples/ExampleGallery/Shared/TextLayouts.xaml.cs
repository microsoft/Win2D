// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

using Microsoft.Graphics.Canvas;
using Microsoft.Graphics.Canvas.Brushes;
using Microsoft.Graphics.Canvas.Geometry;
using Microsoft.Graphics.Canvas.Text;
using Microsoft.Graphics.Canvas.UI;
using Microsoft.Graphics.Canvas.UI.Xaml;
using System;
using System.Collections.Generic;
using System.Numerics;
using System.Linq;
using System.Threading.Tasks;
using Windows.Foundation;
using Windows.Globalization;
using Windows.UI;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;

#if !WINDOWS_UWP
using Vector2 = Microsoft.Graphics.Canvas.Numerics.Vector2;
#endif

namespace ExampleGallery
{
    public sealed partial class TextLayouts : UserControl
    {
        CanvasTextLayout textLayout;
        CanvasLinearGradientBrush textBrush;
        CanvasLinearGradientBrush selectionTextBrush;
        CanvasStrokeStyle dashedStroke = new CanvasStrokeStyle()
        {
            DashStyle = CanvasDashStyle.Dash
        };
        bool inlineObjectsEnabledChanged;

        //
        // This is stored as a local static so that SpecialGlyph has access to it.
        // This app only has one inline object type, but this pattern generalizes
        // well to apps with multiple inline objects which all need to know about
        // a drawing session.
        //
        static CanvasDrawingSession currentDrawingSession;

        class SpecialGlyph : ICanvasTextInlineObject
        {
            CanvasBitmap resourceBitmap;
            float size;

            public void Draw(ICanvasTextRenderer renderer, Vector2 position, bool isSideways, bool isRightToLeft, object brush)
            {
                currentDrawingSession.DrawImage(resourceBitmap, new Rect(position.X, position.Y, size, size));
            }

            public void SetBitmap(CanvasBitmap bitmap)
            {
                resourceBitmap = bitmap;
            }

            public void SetLayout(CanvasTextLayout layout)
            {
                size = layout.DefaultFontSize;
            }

            public float Baseline { get { return size; } }
            public Rect DrawBounds { get { return new Rect(0, 0, size, size); } }
            public Size Size { get { return new Size(size, size); } }
            public bool SupportsSideways { get { return false; } }
            public CanvasLineBreakCondition BreakBefore { get { return CanvasLineBreakCondition.CanBreak; } }
            public CanvasLineBreakCondition BreakAfter { get { return CanvasLineBreakCondition.CanBreak; } }
        }
        SpecialGlyph inlineObject = new SpecialGlyph();

        string testString;

        public bool UseEllipsisTrimming { get; set; }

        public bool ApplyInlineObjects { get; set; }

        public bool ShowPerCharacterLayoutBounds { get; set; }
        public bool ShowLayoutBounds { get; set; }
        public bool ShowDrawBounds { get; set; }

        bool needsResourceRecreation;
        Size resourceRealizationSize;

        bool hasSelection;
        int selectionStartIndex = 0;
        int selectionEndIndex = 0;

        public enum TextSampleOption
        {
            QuickBrownFox,
            LatinLoremIpsum,
            乱数假文 // Chinese lorem ipsum
        }
        public List<TextSampleOption> TextSampleOptions { get { return Utils.GetEnumAsList<TextSampleOption>(); } }
        public TextSampleOption CurrentTextSampleOption { get; set; }

        public TextLayouts()
        {
            this.InitializeComponent();

            CurrentTextSampleOption = TextSampleOption.QuickBrownFox;
            ShowPerCharacterLayoutBounds = true;
            UseEllipsisTrimming = true;
        }

        Rect InflateRect(Rect r)
        {
            return new Rect(
                new Point(Math.Floor(r.Left), Math.Floor(r.Top)),
                new Point(Math.Ceiling(r.Right), Math.Ceiling(r.Bottom)));
        }

        void EnsureResources(ICanvasResourceCreatorWithDpi resourceCreator, Size targetSize)
        {
            if (resourceRealizationSize == targetSize && !needsResourceRecreation)
                return;

            float canvasWidth = (float)targetSize.Width;
            float canvasHeight = (float)targetSize.Height;

            if (textLayout != null) 
                textLayout.Dispose();
            textLayout = CreateTextLayout(resourceCreator, canvasWidth, canvasHeight);

            inlineObject.SetLayout(textLayout);

            Rect layoutBounds = textLayout.LayoutBounds;

            textBrush = new CanvasLinearGradientBrush(resourceCreator, Colors.Red, Colors.Green);
            textBrush.StartPoint = new System.Numerics.Vector2((float)(layoutBounds.Left + layoutBounds.Right) / 2, (float)layoutBounds.Top);
            textBrush.EndPoint = new System.Numerics.Vector2((float)(layoutBounds.Left + layoutBounds.Right) / 2, (float)layoutBounds.Bottom);

            selectionTextBrush = new CanvasLinearGradientBrush(resourceCreator, Colors.Green, Colors.Red);
            selectionTextBrush.StartPoint = textBrush.StartPoint;
            selectionTextBrush.EndPoint = textBrush.EndPoint;

            inlineObjectsEnabledChanged = true;

            needsResourceRecreation = false;
            resourceRealizationSize = targetSize;
        }

        private CanvasTextLayout CreateTextLayout(ICanvasResourceCreator resourceCreator, float canvasWidth, float canvasHeight)
        {
            float sizeDim = Math.Min(canvasWidth, canvasHeight);

            CanvasTextFormat textFormat;
            switch (CurrentTextSampleOption)
            {
                case TextSampleOption.QuickBrownFox:
                    testString = "The quick brown fox jumps over the lazy dog.";
                    textFormat = new CanvasTextFormat()
                    {
                        FontSize = sizeDim * 0.1f,
                        HorizontalAlignment = CanvasHorizontalAlignment.Center,
                        VerticalAlignment = CanvasVerticalAlignment.Center
                    };
                    break;

                case TextSampleOption.LatinLoremIpsum:
                    testString = @"                    Lorem ipsum dolor sit amet, consectetur adipiscing elit. Integer lacinia odio lectus, eget luctus felis tincidunt sit amet. Maecenas vel ex porttitor, ultrices nunc feugiat, porttitor quam. Cras non interdum urna. In sagittis tempor leo quis laoreet. Sed pretium tellus ut commodo viverra. Ut volutpat in risus at aliquam. Sed faucibus vitae dolor ut commodo.
                    Mauris mollis rhoncus libero ut porttitor. Suspendisse at egestas nunc. Proin non neque nibh. Mauris eu ornare arcu. Etiam non sem eleifend, imperdiet erat at, hendrerit ante. Pellentesque habitant morbi tristique senectus et netus et malesuada fames ac turpis egestas. Integer porttitor mauris eu pulvinar commodo. Interdum et malesuada fames ac ante ipsum primis in faucibus.
                    Mauris ultricies fermentum sem sed consequat. Vestibulum iaculis dui nulla, nec pharetra dolor gravida in. Pellentesque vel nisi urna. Donec gravida nunc sed pellentesque feugiat. Aliquam iaculis enim non enim ultrices aliquam. In at leo sed lorem interdum bibendum at non enim. Vivamus fermentum nisl eros, sit amet laoreet justo tincidunt et. Maecenas nunc erat, placerat non efficitur quis, convallis et nunc. Integer eget nunc id nunc convallis hendrerit. Mauris ac ornare nibh, vel condimentum dolor. Nullam dictum nibh eget tempus suscipit. Sed in ligula vitae ligula dignissim semper vel pretium nisl. Suspendisse potenti. Cum sociis natoque penatibus et magnis dis parturient montes, nascetur ridiculus mus.
                    Cras sit amet pretium libero. Maecenas non mauris vitae ante auctor hendrerit. Donec ut felis metus. Pellentesque habitant morbi tristique senectus et netus et malesuada fames ac turpis egestas. Phasellus sodales commodo imperdiet. Vestibulum maximus diam ac dui rhoncus blandit. Curabitur id tincidunt urna. Vivamus vitae blandit sem. Duis non sem nibh. Proin a lacus ac est egestas aliquam in vel dolor. Duis semper mattis elit, vitae bibendum lacus tempus vel. Quisque eget ultrices orci, id sagittis est. Duis fringilla pretium diam, at finibus ex elementum vel. Aenean nibh lectus, consequat nec libero interdum, gravida vestibulum mi.
                    Etiam arcu libero, semper et maximus eget, mollis in dui. Interdum et malesuada fames ac ante ipsum primis in faucibus. Integer hendrerit neque non libero rutrum tincidunt. Fusce in odio vel nisl aliquam tempus. Fusce sollicitudin nisi vel eros consequat, ac pulvinar lorem imperdiet. Aliquam pulvinar metus sed varius porta. Curabitur ac leo laoreet, bibendum erat sit amet, pulvinar purus. Sed laoreet ipsum eu tortor congue vulputate. Integer convallis vulputate urna. Nulla quis vestibulum justo. Vestibulum non purus nunc.";

                    textFormat = new CanvasTextFormat()
                    {
                        FontSize = sizeDim * 0.025f,
                        HorizontalAlignment = CanvasHorizontalAlignment.Left,
                        VerticalAlignment = CanvasVerticalAlignment.Top
                    };
                    break;

                case TextSampleOption.乱数假文:
                default:
                    testString = "不結邊的聲不陸還下絕去以得一票來夠年維書：原有馬下，不時不去遊制日經養投外光、調預知拉越位；去進就管一率客於行小眾一關金的相。現的一展己明物程上益驚，步使超友究比質管日。散奇說。有急理裝臺也足報國發，院來自加樂為？應報心境好調怕銀利的器理濟生他，地人知滿提球術不先表……值想不為品華信子會他過知天清與止為的。實時知嚴怕傳著配法類加，點展源又上後吸上戲上上的不當。一種人利取過微家品領不作出得那、確他他如樂德的，活布還理收，高子許臺輪電性跑整想水功用通。大雙小分過排現令簡。公方要排題公排見教去。終後少！口許一此表自無人……種活構自動一些林、切、師是放不注！氣以那中以新統雖這水？名標天。每利出、帶未心時我斷有看系是士，道此我地技一平水大非我意然，現了。它好作下物失不感一，適事資清有足收不推行……館臺卻告下、動論然中率麼說裡了本大解事成先文如。可再冷自們。一為何。史地著，證政科、展言用往。";

                    textFormat = new CanvasTextFormat()
                    {
                        FontSize = sizeDim * 0.04f,
                        HorizontalAlignment = CanvasHorizontalAlignment.Center,
                        VerticalAlignment = CanvasVerticalAlignment.Center,
                        Direction = CanvasTextDirection.TopToBottomThenRightToLeft
                    };
                    break;
            }

            textFormat.FontFamily = fontPicker.CurrentFontFamily;

            textFormat.TrimmingGranularity = CanvasTextTrimmingGranularity.Word;
            textFormat.TrimmingSign = UseEllipsisTrimming ? CanvasTrimmingSign.Ellipsis : CanvasTrimmingSign.None;

            var textLayout = new CanvasTextLayout(resourceCreator, testString, textFormat, canvasWidth, canvasHeight);

            return textLayout;
        }

        void EnsureInlineObjects()
        {
            if (!inlineObjectsEnabledChanged) return;

            //
            // Changing this option doesn't require re-recreation of the text layout.
            //
            if (ApplyInlineObjects)
            {
                for (int i = 0; i < testString.Length; ++i)
                {
                    if (i % 5 == 0)
                        textLayout.SetInlineObject(i, 1, inlineObject);
                }
            }
            else
            {
                textLayout.SetInlineObject(0, testString.Length, null);
            }

            inlineObjectsEnabledChanged = false;
        }

        private void Canvas_Draw(CanvasControl sender, CanvasDrawEventArgs args)
        {
            EnsureResources(sender, sender.Size);

            currentDrawingSession = args.DrawingSession;

            textLayout.SetBrush(0, testString.Length, null);
            if (hasSelection)
            {
                int firstIndex = Math.Min(selectionStartIndex, selectionEndIndex);
                int length = Math.Abs(selectionEndIndex - selectionStartIndex) + 1;
                CanvasTextLayoutRegion[] descriptions = textLayout.GetCharacterRegions(firstIndex, length);
                foreach (CanvasTextLayoutRegion description in descriptions)
                {
                    args.DrawingSession.FillRectangle(InflateRect(description.LayoutBounds), Colors.White);
                }
                textLayout.SetBrush(firstIndex, length, selectionTextBrush);
            }

            EnsureInlineObjects();

            args.DrawingSession.DrawTextLayout(textLayout, 0, 0, textBrush);

            if (ShowPerCharacterLayoutBounds)
            {
                for (int i = 0; i < testString.Length; i++)
                {
                    CanvasTextLayoutRegion textLayoutRegion;
                    textLayout.GetCaretPosition(i, false, out textLayoutRegion);

                    args.DrawingSession.DrawRectangle(textLayoutRegion.LayoutBounds, Colors.Blue, 2);
                }
            }

            if (ShowDrawBounds)
            {
                args.DrawingSession.DrawRectangle(textLayout.DrawBounds, Colors.Green, 2);
            }

            if (ShowLayoutBounds)
            {
                args.DrawingSession.DrawRectangle(textLayout.LayoutBounds, Colors.Red, 2, dashedStroke);
            }

        }


        async Task Canvas_CreateResourcesAsync(CanvasControl sender)
        {
            inlineObject.SetBitmap(await CanvasBitmap.LoadAsync(sender, "imageTiger.jpg"));
        }

        private void Canvas_CreateResources(CanvasControl sender, CanvasCreateResourcesEventArgs args)
        {
            needsResourceRecreation = true;

            args.TrackAsyncAction(Canvas_CreateResourcesAsync(sender).AsAsyncAction());
        }

        int GetHitIndex(Point mouseOverPt)
        {
            CanvasTextLayoutRegion textLayoutRegion;
            hasSelection = textLayout.HitTest(
                (float)mouseOverPt.X,
                (float)mouseOverPt.Y,
                out textLayoutRegion);
            return textLayoutRegion.CharacterIndex;
        }

        private void Canvas_PointerPressed(object sender, Windows.UI.Xaml.Input.PointerRoutedEventArgs e)
        {
            selectionStartIndex = GetHitIndex(e.GetCurrentPoint(canvas).Position);
            selectionEndIndex = selectionStartIndex;
            canvas.Invalidate();
            e.Handled = true;
        }

        private void Canvas_PointerMoved(object sender, Windows.UI.Xaml.Input.PointerRoutedEventArgs e)
        {
            foreach (var point in e.GetIntermediatePoints(canvas))
            {
                if (point.IsInContact)
                {
                    selectionEndIndex = GetHitIndex(point.Position);
                }
            }
            canvas.Invalidate();
            e.Handled = true;
        }

        void ClearSelection()
        {
            hasSelection = false;
            selectionStartIndex = 0;
            selectionEndIndex = 0;
        }

        private void TextSampleOptions_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            ClearSelection();
            needsResourceRecreation = true;
            canvas.Invalidate();
        }

        private void UseInlineObjectsClicked(object sender, RoutedEventArgs e)
        {
            inlineObjectsEnabledChanged = true;

            canvas.Invalidate();
        }

        private void InvalidateCanvas(object sender, RoutedEventArgs e)
        {
            canvas.Invalidate();
        }

        private void EllipsisTrimming_Click(object sender, RoutedEventArgs e)
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
