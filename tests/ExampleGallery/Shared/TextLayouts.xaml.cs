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
        bool showLayoutBounds;
        bool showDrawBounds;

        bool hasSelection;
        int selectionStartIndex = 0;
        int selectionEndIndex = 0;

        public TextLayouts()
        {
            this.InitializeComponent();

            canvas.Input.PointerPressed += OnPointerPressed;
            canvas.Input.PointerMoved += OnPointerMoved;
            canvas.Input.PointerReleased += OnPointerReleased;
        }

        CanvasTextFormat textFormat = new CanvasTextFormat()
        {
            FontSize = 100,
            ParagraphAlignment = Windows.UI.Text.ParagraphAlignment.Center,
            VerticalAlignment = CanvasVerticalAlignment.Center
        };

        private void Canvas_Draw(ICanvasAnimatedControl sender, CanvasAnimatedDrawEventArgs args)
        {
            string testString = "The quick brown fox jumped over the lazy dogs.";

            textLayout = new CanvasTextLayout(
                testString,
                textFormat,
                (float)sender.Size.Width,
                (float)sender.Size.Height);

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

            args.DrawingSession.DrawTextLayout(textLayout, 0, 0, Colors.White);

            if(hasSelection)
            {
                int firstIndex = Math.Min(selectionStartIndex, selectionEndIndex);
                int length = Math.Abs(selectionEndIndex - selectionStartIndex) + 1;
                CanvasTextLayoutRegion[] descriptions = textLayout.GetCharacterRegions(firstIndex, length);
                foreach (CanvasTextLayoutRegion description in descriptions)
                {
                    args.DrawingSession.DrawRectangle(description.LayoutBounds, Colors.Red);
                }
            }
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
                mouseOverPt.ToVector2(),
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
                if(!textLayout.HitTest(args.CurrentPoint.Position.ToVector2()))
                {
                    hasSelection = false;
                }
            }
        }
    }
}
