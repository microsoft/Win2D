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
using Windows.Foundation;
using Windows.UI;
using Windows.UI.Text;
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

        private void Button_ClickA(object sender, RoutedEventArgs e)
        {
            this.storyboardA.Begin();
        }

        private void Button_ClickB(object sender, RoutedEventArgs e)
        {
            this.storyboardB.Begin();
        }

        private void Button_ClickC(object sender, RoutedEventArgs e)
        {
            this.storyboardC.Begin();
        }

        private void Button_ClickD(object sender, RoutedEventArgs e)
        {
            this.storyboardD.Begin();
        }

        private void DrawCanvasState(CanvasControl canvas, CanvasDrawingSession ds, CanvasSolidColorBrush brush, int drawCount)
        {
            ds.Clear(Color.FromArgb(0, 0, 0, 0));

            brush.Color = Colors.Aqua;
            ds.DrawLine(new Point(0, 0), new Point(canvas.ActualWidth, canvas.ActualHeight), brush);
            ds.DrawLine(new Point(0, canvas.ActualHeight), new Point(canvas.ActualWidth, 0), brush);

            brush.Color = Colors.FloralWhite;

            var text = String.Format("{0}x{1}\n{2} redraws", (int)canvas.ActualWidth, (int)canvas.ActualHeight, drawCount);

            ds.DrawText(
                text,
                new Point(0, 0),
                brush,
                new CanvasTextFormat()
                {
                    VerticalAlignment = CanvasVerticalAlignment.Top,
                    ParagraphAlignment = ParagraphAlignment.Left,
                    FontSize = 10
                });
        }

        //
        // TODO #1836: Once multiple CanvasControls share a single device we 
        // won't need to have a 'new CanvasSolidColorBrush' per handler.
        //
        // While it is possible to create resources from a CanvasControl rather 
        // than a Device directly, different CanvasControls still have 
        // different devices, and different resource domains.
        //

        private void Canvas01_Drawing(CanvasControl sender, CanvasDrawingEventArgs args)
        {
            using (CanvasSolidColorBrush brush = new CanvasSolidColorBrush(sender, Colors.FloralWhite))
            {
                DrawCanvasState(sender, args.DrawingSession, brush, ++drawCount01);
            }
        }

        private void Canvas11_Drawing(CanvasControl sender, CanvasDrawingEventArgs args)
        {
            using (CanvasSolidColorBrush brush = new CanvasSolidColorBrush(sender, Colors.FloralWhite))
            {
                DrawCanvasState(sender, args.DrawingSession, brush, ++drawCount11);
            }
        }

        private void Canvas02_Drawing(CanvasControl sender, CanvasDrawingEventArgs args)
        {
            using (CanvasSolidColorBrush brush = new CanvasSolidColorBrush(sender, Colors.FloralWhite))
            {
                DrawCanvasState(sender, args.DrawingSession, brush, ++drawCount02);
            }
        }

        private void Canvas12_Drawing(CanvasControl sender, CanvasDrawingEventArgs args)
        {
            using (CanvasSolidColorBrush brush = new CanvasSolidColorBrush(sender, Colors.FloralWhite))
            {
                DrawCanvasState(sender, args.DrawingSession, brush, ++drawCount12);
            }
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
    }
}
