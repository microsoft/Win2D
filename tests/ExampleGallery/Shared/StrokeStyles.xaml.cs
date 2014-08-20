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
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace ExampleGallery
{
    public sealed partial class StrokeStyles : UserControl
    {
        public StrokeStyles()
        {
            this.InitializeComponent();
        }

        private void OnLoaded(object sender, RoutedEventArgs e)
        {
            this.canvas.Invalidate();
        }

        public List<CanvasDashStyle> DashStyles { get { return GetEnumAsList<CanvasDashStyle>(); } }
        public List<CanvasCapStyle> CapStyles { get { return GetEnumAsList<CanvasCapStyle>(); } }
        public List<CanvasLineJoin> LineJoins { get { return GetEnumAsList<CanvasLineJoin>(); } }

        private List<T> GetEnumAsList<T>()
        {
            return new List<T>(Enum.GetValues(typeof(T)).Cast<T>());
        }

        public CanvasCapStyle StartCap { get; set; }
        public CanvasCapStyle EndCap { get; set; }
        public CanvasDashStyle DashStyle { get; set; }
        public CanvasCapStyle DashCap { get; set; }
        public CanvasLineJoin LineJoin { get; set; }

        CanvasSolidColorBrush brush;
        CanvasStrokeStyle strokeStyle = new CanvasStrokeStyle();

        CanvasStrokeStyle hairlineStrokeStyle = new CanvasStrokeStyle()
        {
            TransformBehavior = CanvasStrokeTransformBehavior.Hairline
        };

        private void Canvas_Draw(CanvasControl sender, CanvasDrawEventArgs args)
        {
            strokeStyle.StartCap = StartCap;
            strokeStyle.EndCap = EndCap;
            strokeStyle.DashStyle = DashStyle;
            strokeStyle.DashCap = DashCap;
            strokeStyle.LineJoin = LineJoin;

            var ds = args.DrawingSession;

            ds.Clear(Color.FromArgb(0, 0, 0, 0));

            var width = sender.ActualWidth;
            var height = sender.ActualHeight;

            var topLeftRect = new Rect(new Point(width * 0.1, height * 0.1), new Point(width * 0.4, height * 0.4));
            var topRightRect = new Rect(new Point(width * 0.6, height * 0.1), new Point(width * 0.9, height * 0.4));
            var bottomLeftRect = new Rect(new Point(width * 0.1, height * 0.6), new Point(width * 0.4, height * 0.9));
            var bottomRightRect = new Rect(new Point(width * 0.6, height * 0.6), new Point(width * 0.9, height * 0.9));

            var topLeftMiddle = topLeftRect.Top + topLeftRect.Height * 0.5;

            //
            // Draw hairlines showing the start/end points of the line.  
            // This helps demonstrate the behavior of start/end cap.
            //
            this.brush.Color = Colors.Aqua;

            ds.DrawLine(
                new Point(topLeftRect.Left, topLeftRect.Top),
                new Point(topLeftRect.Left, topLeftRect.Bottom),
                this.brush,
                1.0f,
                this.hairlineStrokeStyle);

            ds.DrawLine(
                new Point(topLeftRect.Right, topLeftRect.Top),
                new Point(topLeftRect.Right, topLeftRect.Bottom),
                this.brush,
                1.0f,
                this.hairlineStrokeStyle);

            //
            // Draw the demo shapes with the chosen stroke style
            //
            this.brush.Color = Colors.AntiqueWhite;

            var strokeWidth = (float)Math.Max(5, Math.Min(30, width / 50));

            ds.DrawLine(
                new Point(topLeftRect.Left, topLeftMiddle),
                new Point(topLeftRect.Right, topLeftMiddle),
                this.brush,
                strokeWidth,
                this.strokeStyle);

            ds.DrawRectangle(
                topRightRect,
                this.brush,
                strokeWidth,
                this.strokeStyle);

            ds.DrawRoundedRectangle(
                new CanvasRoundedRectangle()
                {
                    Rect = bottomLeftRect,
                    RadiusX = (float)(width * 0.1),
                    RadiusY = (float)(height * 0.1)
                },
                this.brush,
                strokeWidth,
                this.strokeStyle);

            ds.DrawEllipse(
                new CanvasEllipse()
                {
                    Point = new Point(
                        bottomRightRect.Left + bottomRightRect.Width * 0.5,
                        bottomRightRect.Top + bottomRightRect.Height * 0.5),
                    RadiusX = (float)(bottomRightRect.Width * 0.5),
                    RadiusY = (float)(bottomRightRect.Height * 0.5)
                },
                this.brush,
                strokeWidth,
                this.strokeStyle);
        }

        private void Canvas_CreateResources(CanvasControl sender, object args)
        {
            this.brush = new CanvasSolidColorBrush(sender, Colors.AntiqueWhite);
        }

        private void SettingsCombo_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            this.canvas.Invalidate();
        }
    }
}
