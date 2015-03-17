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
using System.Linq;
using Windows.Foundation;
using Windows.UI;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using System.Numerics;
using System.Threading;

namespace ExampleGallery
{
    public sealed partial class TextLayouts : UserControl
    {
        CanvasTextLayout textLayout;
        string testString;
        bool showLayoutBounds;
        bool showDrawBounds;
        bool needsResourceRecreation;
        Size resourceRealizationSize;

        bool hasSelection;
        int selectionStartIndex = 0;
        int selectionEndIndex = 0;
        CanvasCommandList textCommandList;

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

            canvas.Input.PointerPressed += OnPointerPressed;
            canvas.Input.PointerMoved += OnPointerMoved;
            canvas.Input.PointerReleased += OnPointerReleased;

            CurrentTextSampleOption = TextSampleOption.QuickBrownFox;

            showLayoutBounds = true;
        }

        Rect InflateRect(Rect r)
        {
            return new Rect(
                new Point(Math.Floor(r.Left), Math.Floor(r.Top)),
                new Point(Math.Ceiling(r.Right), Math.Ceiling(r.Bottom)));
        }

        void EnsureResources(ICanvasResourceCreatorWithDpi resourceCreator, Size targetSize)
        {
            if (!needsResourceRecreation) 
                return;

            float canvasWidth = (float)targetSize.Width;
            float canvasHeight = (float)targetSize.Height;

            if(textLayout != null) 
                textLayout.Dispose();
            textLayout = CreateTextLayout(canvasWidth, canvasHeight);

            Rect layoutBounds = textLayout.LayoutBounds;

            var textBrush = new CanvasLinearGradientBrush(resourceCreator, Colors.Blue, Colors.Yellow);
            textBrush.StartPoint = new System.Numerics.Vector2((float)(layoutBounds.Left + layoutBounds.Right) / 2, (float)layoutBounds.Top);
            textBrush.EndPoint = new System.Numerics.Vector2((float)(layoutBounds.Left + layoutBounds.Right) / 2, (float)layoutBounds.Bottom);

            if(textCommandList != null) 
                textCommandList.Dispose();
            textCommandList = new CanvasCommandList(resourceCreator);
            using (CanvasDrawingSession textTargetDrawingSession = textCommandList.CreateDrawingSession())
            {
                textTargetDrawingSession.Clear(Colors.Transparent);
                textTargetDrawingSession.DrawTextLayout(textLayout, 0, 0, textBrush);
            }

            needsResourceRecreation = false;
            resourceRealizationSize = targetSize;
        }

        private CanvasTextLayout CreateTextLayout(float canvasWidth, float canvasHeight)
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
                        ParagraphAlignment = Windows.UI.Text.ParagraphAlignment.Center,
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
                        FontSize = sizeDim * 0.03f,
                        ParagraphAlignment = Windows.UI.Text.ParagraphAlignment.Left,
                        VerticalAlignment = CanvasVerticalAlignment.Top
                    };
                    break;

                case TextSampleOption.乱数假文:
                default:
                    testString = "不結邊的聲不陸還下絕去以得一票來夠年維書：原有馬下，不時不去遊制日經養投外光、調預知拉越位；去進就管一率客於行小眾一關金的相。現的一展己明物程上益驚，步使超友究比質管日。散奇說。有急理裝臺也足報國發，院來自加樂為？應報心境好調怕銀利的器理濟生他，地人知滿提球術不先表……值想不為品華信子會他過知天清與止為的。實時知嚴怕傳著配法類加，點展源又上後吸上戲上上的不當。一種人利取過微家品領不作出得那、確他他如樂德的，活布還理收，高子許臺輪電性跑整想水功用通。大雙小分過排現令簡。公方要排題公排見教去。終後少！口許一此表自無人……種活構自動一些林、切、師是放不注！氣以那中以新統雖這水？名標天。每利出、帶未心時我斷有看系是士，道此我地技一平水大非我意然，現了。它好作下物失不感一，適事資清有足收不推行……館臺卻告下、動論然中率麼說裡了本大解事成先文如。可再冷自們。一為何。史地著，證政科、展言用往。";

                    textFormat = new CanvasTextFormat()
                    {
                        FontSize = sizeDim * 0.06f,
                        ParagraphAlignment = Windows.UI.Text.ParagraphAlignment.Center,
                        VerticalAlignment = CanvasVerticalAlignment.Center,
                        ReadingDirection = CanvasTextDirection.TopToBottom,
                        FlowDirection = CanvasTextDirection.RightToLeft
                    };
                    break;
            }

            return new CanvasTextLayout(testString, textFormat, canvasWidth, canvasHeight);
        }

        private void Canvas_Draw(ICanvasAnimatedControl sender, CanvasAnimatedDrawEventArgs args)
        {
            EnsureResources(sender, sender.Size);

            args.DrawingSession.DrawImage(textCommandList);

            if (hasSelection)
            {
                var colorMatrixEffect = new ColorMatrixEffect()
                {
                    Source = textCommandList
                };

                var negativeColors = new Matrix5x4();
                negativeColors.M11 = -1; // Invert red
                negativeColors.M51 = 1;

                negativeColors.M22 = -1; // Invert blue
                negativeColors.M52 = 1;

                negativeColors.M33 = -1; // Invert green
                negativeColors.M53 = 1;

                negativeColors.M54 = 1; // Hardcode alpha at 1

                colorMatrixEffect.ColorMatrix = negativeColors;

                var imageBrush = new CanvasImageBrush(sender, colorMatrixEffect);
                imageBrush.SourceRectangle = new Rect(0, 0, (float)sender.Size.Width, (float)sender.Size.Height);

                int firstIndex = Math.Min(selectionStartIndex, selectionEndIndex);
                int length = Math.Abs(selectionEndIndex - selectionStartIndex) + 1;
                CanvasTextLayoutRegion[] descriptions = textLayout.GetCharacterRegions(firstIndex, length);
                foreach (CanvasTextLayoutRegion description in descriptions)
                {
                    args.DrawingSession.FillRectangle(InflateRect(description.LayoutBounds), imageBrush);
                }
            }

            if (showLayoutBounds)
            {
                for (int i = 0; i < testString.Length; i++)
                {
                    CanvasTextLayoutRegion textLayoutRegion;
                    textLayout.GetCaretPosition(i, false, out textLayoutRegion);

                    float cellWidth = (float)textLayoutRegion.LayoutBounds.Width;

                    Windows.Foundation.Rect r = textLayoutRegion.LayoutBounds;
                    args.DrawingSession.DrawRectangle(r, Colors.Blue, 2);
                }
            }

            if (showDrawBounds)
            {
                args.DrawingSession.DrawRectangle(textLayout.DrawBounds, Colors.Green, 2);
            }

        }

        private void Canvas_Update(ICanvasAnimatedControl sender, CanvasAnimatedUpdateEventArgs args)
        {
            if (resourceRealizationSize != sender.Size)
            {
                needsResourceRecreation = true;
            }
        }

        private void Canvas_CreateResources(CanvasAnimatedControl sender, object args)
        {
            needsResourceRecreation = true;
        }

        void ShowLayoutBounds_Checked(object sender, RoutedEventArgs e)
        {
            showLayoutBounds = true;
        }

        void ShowLayoutBounds_Unchecked(object sender, RoutedEventArgs e)
        {
            showLayoutBounds = false;
        }

        void ShowDrawBounds_Checked(object sender, RoutedEventArgs e)
        {
            showDrawBounds = true;
        }

        void ShowDrawBounds_Unchecked(object sender, RoutedEventArgs e)
        {
            showDrawBounds = false;
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

        private void OnPointerPressed(object sender, PointerEventArgs args)
        {
            selectionStartIndex = GetHitIndex(args.CurrentPoint.Position);
            selectionEndIndex = selectionStartIndex;
        }

        private void OnPointerMoved(object sender, PointerEventArgs args)
        {
            foreach (var point in args.GetIntermediatePoints())
            {
                if (point.IsInContact)
                {
                    selectionEndIndex = GetHitIndex(point.Position);
                }
            }
        }

        private void OnPointerReleased(object sender, PointerEventArgs args)
        {
            if(!hasSelection)
            {
                if (!textLayout.HitTest(args.CurrentPoint.Position.ToVector2()))
                {
                    hasSelection = false;
                }
            }
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
        }
    }
}
