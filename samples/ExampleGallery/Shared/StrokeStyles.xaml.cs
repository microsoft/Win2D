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

using Microsoft.Graphics.Canvas.Geometry;
using Microsoft.Graphics.Canvas.UI.Xaml;
using System;
using System.Collections.Generic;
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

            StartCap = strokeStyle.StartCap;
            EndCap = strokeStyle.EndCap;
            DashStyle = strokeStyle.DashStyle;
            DashCap = strokeStyle.DashCap;
            LineJoin = strokeStyle.LineJoin;
        }

        public List<CanvasDashStyle> DashStyles { get { return Utils.GetEnumAsList<CanvasDashStyle>(); } }
        public List<CanvasCapStyle> CapStyles { get { return Utils.GetEnumAsList<CanvasCapStyle>(); } }
        public List<CanvasLineJoin> LineJoins { get { return Utils.GetEnumAsList<CanvasLineJoin>(); } }

        public CanvasCapStyle StartCap { get; set; }
        public CanvasCapStyle EndCap { get; set; }
        public CanvasDashStyle DashStyle { get; set; }
        public CanvasCapStyle DashCap { get; set; }
        public CanvasLineJoin LineJoin { get; set; }

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

            var width = (float)sender.ActualWidth;
            var height = (float)sender.ActualHeight;

            var col1Left = width * 0.1f;
            var col1Right = width * 0.4f;
            var col2Left = width * 0.6f;
            var col2Right = width * 0.9f;

            var row1Top = height * 0.1f;
            var row1Bottom = height * 0.4f;
            var row2Top = height * 0.6f;
            var row2Bottom = height * 0.9f;

            //
            // Draw hairlines showing the start/end points of the line.  
            // This helps demonstrate the behavior of start/end cap.
            //
            ds.DrawLine(
                col1Left,
                row1Top,
                col1Left,
                row1Bottom,
                Colors.Aqua,
                1.0f,
                this.hairlineStrokeStyle);

            ds.DrawLine(
                col1Right,
                row1Top,
                col1Right,
                row1Bottom,
                Colors.Aqua,
                1.0f,
                this.hairlineStrokeStyle);

            //
            // Draw the demo shapes with the chosen stroke style
            //
            var strokeWidth = (float)Math.Max(5, Math.Min(30, width / 50));

            ds.DrawLine(
                col1Left, 
                (row1Top + row1Bottom) / 2,
                col1Right,
                (row1Top + row1Bottom) / 2,
                Colors.Green,
                strokeWidth,
                this.strokeStyle);

            ds.DrawRectangle(
                new Rect(new Point(col2Left, row1Top), new Point(col2Right, row1Bottom)),
                Colors.Green,
                strokeWidth,
                this.strokeStyle);

            ds.DrawRoundedRectangle(
                new Rect(new Point(col1Left, row2Top), new Point(col1Right, row2Bottom)),
                width * 0.1f,
                height * 0.1f,
                Colors.Green,
                strokeWidth,
                this.strokeStyle);

            ds.DrawEllipse(
                (col2Left + col2Right) / 2,
                (row2Top + row2Bottom) / 2,
                (col2Right - col2Left) / 2,
                (row2Bottom - row2Top) / 2,
                Colors.Green,
                strokeWidth,
                this.strokeStyle);

            //
            // Explain what is going on if this combination of properties leaves dots invisible
            //
            bool hasDots = DashStyle == CanvasDashStyle.Dot ||
                           DashStyle == CanvasDashStyle.DashDot ||
                           DashStyle == CanvasDashStyle.DashDotDot;

            if (hasDots && DashCap == CanvasCapStyle.Flat)
            {
                ds.DrawText("Dots have zero size when DashCap = CanvasCapStyle.Flat", col1Left, 0, Colors.White);
            }
        }

        private void SettingsCombo_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            this.canvas.Invalidate();
        }

        private void control_Unloaded(object sender, RoutedEventArgs e)
        {
            // Explicitly remove references to allow the Win2D controls to get garbage collected
            canvas.RemoveFromVisualTree();
            canvas = null;
        }
    }
}
