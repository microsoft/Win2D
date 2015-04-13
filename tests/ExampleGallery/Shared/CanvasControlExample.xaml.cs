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
using Microsoft.Graphics.Canvas.Text;
using Microsoft.Graphics.Canvas.UI.Xaml;
using System.Numerics;
using Windows.UI;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Input;

namespace ExampleGallery
{
    public sealed partial class CanvasControlExample : UserControl
    {
        int drawCount01;
        int drawCount11;
        int drawCount02;
        int drawCount12;

        public CanvasControlExample()
        {
            this.InitializeComponent();
        }

        private void DrawCanvasState(CanvasControl canvas, CanvasDrawingSession ds, int drawCount)
        {
            Vector2 size = canvas.Size.ToVector2();

            ds.DrawLine(Vector2.Zero, size, Colors.Aqua);
            ds.DrawLine(0, size.Y, size.X, 0, Colors.Aqua);

            if (!ThumbnailGenerator.IsDrawingThumbnail)
            {
                var text = string.Format("{0}x{1}\n{2} redraws", size.X, size.Y, drawCount);

                ds.DrawText(
                    text,
                    0, 0,
                    Colors.FloralWhite,
                    new CanvasTextFormat()
                    {
                        VerticalAlignment = CanvasVerticalAlignment.Top,
                        HorizontalAlignment = CanvasHorizontalAlignment.Left,
                        FontSize = 10
                    });
            }
        }

        private void Canvas01_Draw(CanvasControl sender, CanvasDrawEventArgs args)
        {
            DrawCanvasState(sender, args.DrawingSession, ++drawCount01);
        }

        private void Canvas11_Draw(CanvasControl sender, CanvasDrawEventArgs args)
        {
            DrawCanvasState(sender, args.DrawingSession, ++drawCount11);
        }

        private void Canvas02_Draw(CanvasControl sender, CanvasDrawEventArgs args)
        {
            DrawCanvasState(sender, args.DrawingSession, ++drawCount02);
        }

        private void Canvas12_Draw(CanvasControl sender, CanvasDrawEventArgs args)
        {
            DrawCanvasState(sender, args.DrawingSession, ++drawCount12);
        }

        private void Canvas01_Tapped(object sender, TappedRoutedEventArgs e)
        {
            this.storyboardA.Begin();
        }

        private void Canvas11_Tapped(object sender, TappedRoutedEventArgs e)
        {
            this.storyboardB.Begin();
        }

        private void Canvas02_Tapped(object sender, TappedRoutedEventArgs e)
        {
            this.storyboardC.Begin();
        }

        private void Canvas12_Tapped(object sender, TappedRoutedEventArgs e)
        {
            this.storyboardD.Begin();
        }

        private void UserControl_Unloaded(object sender, RoutedEventArgs e)
        {
            // Explicitly remove references to allow the Win2D controls to get garbage collected
            canvas01.RemoveFromVisualTree();
            canvas01 = null;

            canvas02.RemoveFromVisualTree();
            canvas02 = null;

            canvas11.RemoveFromVisualTree();
            canvas11 = null;

            canvas12.RemoveFromVisualTree();
            canvas12 = null;
        }
    }
}
