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
using Windows.Foundation;
using Windows.UI;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace ExampleGallery
{
    public delegate void ShapeDrawer(CanvasControl canvas, CanvasDrawingSession drawingSession);

    public class Shape
    {
        public string Name { get; set; }
        public ShapeDrawer Drawer { get; set;}

        public override string ToString()
        {
            return Name;
        }
    }

    public sealed partial class ShapesExample : UserControl
    {
        public ShapesExample()
        {
            this.InitializeComponent();
        }

        private void UserControl_Loaded(object sender, RoutedEventArgs e)
        {
            this.Shapes.ItemsSource = new List<Shape>
            {
                new Shape() { Name = "Line", Drawer = this.DrawLine },
                new Shape() { Name = "Rectangle", Drawer = this.DrawRectangle },
                new Shape() { Name = "Rounded Rectangle", Drawer = this.DrawRoundedRectangle },
                new Shape() { Name = "Circle", Drawer = this.DrawCircles }
            };
            this.Shapes.SelectedIndex = 0;
        }

        void Shapes_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            this.canvas.Invalidate();
        }

        CanvasSolidColorBrush brush;

        void Canvas_CreateResources(CanvasControl sender, object args)
        {
            this.brush = new CanvasSolidColorBrush(sender, Colors.AntiqueWhite);
        }

        void Canvas_Draw(CanvasControl sender, CanvasDrawEventArgs args)
        {
            var ds = args.DrawingSession;
            ds.Clear(Color.FromArgb(0,0,0,0));

            var shape = this.Shapes.SelectedItem as Shape;
            if (shape == null)
                return;

            var width = sender.ActualWidth;
            var height = sender.ActualHeight;

            if (shape.Drawer != null)
                shape.Drawer(sender, ds);
        }

        private void DrawLine(CanvasControl sender, CanvasDrawingSession ds)
        {
            var width = sender.ActualWidth;
            var height = sender.ActualHeight;

            var middle = height / 2;

            int steps = Math.Min((int)(width / 10), 30);

            for (int i = 0; i < steps; ++i)
            {
                var mu = (double)i / steps;
                var a = mu * Math.PI * 2;

                byte c = (byte)((Math.Sin(a) + 1) * 127.5);
                this.brush.Color = Color.FromArgb(255, 128, 128, c);

                var x = width * mu;
                var y = middle + Math.Sin(a) * (middle * 0.3);

                var strokeWidth = (float)(Math.Cos(a) + 1) * 5;

                ds.DrawLine(new Point(x, 0), new Point(x, y), this.brush, strokeWidth);
                ds.DrawLine(new Point(x, height), new Point(x, y), this.brush, 10 - strokeWidth);
            }
        }

        private void DrawRectangle(CanvasControl sender, CanvasDrawingSession ds)
        {
            var width = sender.ActualWidth;
            var height = sender.ActualHeight;

            int steps = Math.Min((int)(width / 10), 20);

            for (int i = 0; i < steps; ++i)
            {
                var mu = (double)i / steps;
                var a = mu * Math.PI * 2;

                mu *= 0.5;
                var x = mu * width;
                var y = mu * height;

                var xx = (1 - mu) * width;
                var yy = (1 - mu) * height;

                byte c = (byte)((Math.Sin(a) + 1) * 127.5);
                this.brush.Color = Color.FromArgb(255, 90, 128, c);

                ds.DrawRectangle(new Rect(new Point(x, y), new Point(xx, yy)), this.brush, 2.0f);
            }
        }

        private void DrawRoundedRectangle(CanvasControl sender, CanvasDrawingSession ds)
        {
            var width = sender.ActualWidth;
            var height = sender.ActualHeight;

            int steps = Math.Min((int)(width / 30), 10);

            for (int i = 0; i < steps; ++i)
            {
                var mu = (double)i / steps;
                var a = mu * Math.PI * 2;

                mu *= 0.5;
                var x = mu * width;
                var y = mu * height;

                var xx = (1 - mu) * width;
                var yy = (1 - mu) * height;

                byte c = (byte)((Math.Sin(a) + 1) * 127.5);
                this.brush.Color = Color.FromArgb(255, 90, 128, c);

                ds.DrawRoundedRectangle(
                    new CanvasRoundedRectangle()
                    {
                        RadiusX=(float)(mu * 50),
                        RadiusY = (float)(mu * 50),
                        Rect=new Rect(new Point(x,y), new Point(xx,yy))              
                    },
                    this.brush,
                    2.0f);
            }
        }

        private void DrawCircles(CanvasControl sender, CanvasDrawingSession ds)
        {
            var width = sender.ActualWidth;
            var height = sender.ActualHeight;

            float endpointMargin = 100;
            float controlMarginX = endpointMargin * 4;
            float controlMarginY = endpointMargin * 2;

            for (int i = 0; i < 25; i++)
            {
                Point[] bez = new Point[4];
                int n = (i * 24) + 9 - (i / 2);

                for (int k = 0; k < 3; k++)
                {
                    int j = 4 - (2 * k);
                    bez[k].X = (0 + (((n >> (j + 1)) & 1) * (width - controlMarginX)));
                    bez[k].Y = (0 + (((n >> j) & 1) * (height - controlMarginY)));
                }
                bez[3].X = width - endpointMargin; // Collect the ends in the lower right
                bez[3].Y = height - endpointMargin;

                const int nSteps = 80;
                const float tStep = 1.0f / nSteps;
                float t = 0;
                for (int step = 0; step < nSteps; step++)
                {
                    double s = 1 - t;
                    double ss = s * s;
                    double sss = ss * s;
                    double tt = t * t;
                    double ttt = tt * t;
                    double x = (sss * bez[0].X) + (3 * ss * t * bez[1].X) + (3 * s * tt * bez[2].X) + (ttt * bez[3].X);
                    double y = (sss * bez[0].Y) + (3 * ss * t * bez[1].Y) + (3 * s * tt * bez[2].Y) + (ttt * bez[3].Y);
                    float radius = (float)(ttt * 100);
                    float strokeWidth = (float)((0.5 - Math.Abs(ss - 0.5)) * 10);

                    var a = t * Math.PI * 2;
                    byte c = (byte)((Math.Sin(a) + 1) * 127.5);
                    this.brush.Color = Color.FromArgb(255, 90, 128, c);

                    ds.DrawCircle(new Point(x, y), radius, this.brush, strokeWidth);
                    t += tStep;
                }
            }

        }
    }
}
