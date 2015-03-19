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
using Microsoft.Graphics.Canvas.Numerics;
using System;
using System.Collections.Generic;
using System.Linq;
using Windows.UI;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace ExampleGallery
{
    public sealed partial class ShapesExample : UserControl
    {
        class Shape
        {
            public string Name { get; set; }
            public Action<CanvasControl, CanvasDrawingSession> Drawer { get; set; }
        }

        List<Shape> availableShapes;

        public ShapesExample()
        {
            this.InitializeComponent();

            availableShapes = new List<Shape>
            {
                new Shape() { Name = "Line",              Drawer = this.DrawLine             },
                new Shape() { Name = "Rectangle",         Drawer = this.DrawRectangle        },
                new Shape() { Name = "Rounded Rectangle", Drawer = this.DrawRoundedRectangle },
                new Shape() { Name = "Circle",            Drawer = this.DrawCircles          }
            };
        }

        private void UserControl_Loaded(object sender, RoutedEventArgs e)
        {
            this.Shapes.ItemsSource = from shape in availableShapes select shape.Name;
            this.Shapes.SelectedIndex = 0;
        }

        void Shapes_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            this.canvas.Invalidate();
        }

        void Canvas_Draw(CanvasControl sender, CanvasDrawEventArgs args)
        {
            var currentShape = availableShapes[this.Shapes.SelectedIndex];

            currentShape.Drawer(sender, args.DrawingSession);
        }

        private void DrawLine(CanvasControl sender, CanvasDrawingSession ds)
        {
            var width = (float)sender.ActualWidth;
            var height = (float)sender.ActualHeight;

            var middle = height / 2;

            int steps = Math.Min((int)(width / 10), 30);

            for (int i = 0; i < steps; ++i)
            {
                var mu = (float)i / steps;
                var a = (float)(mu * Math.PI * 2);

                var color = GradientColor(mu);

                var x = width * mu;
                var y = (float)(middle + Math.Sin(a) * (middle * 0.3));

                var strokeWidth = (float)(Math.Cos(a) + 1) * 5;

                ds.DrawLine(x, 0, x, y, color, strokeWidth);
                ds.DrawLine(x, height, x, y, color, 10 - strokeWidth);
            }
        }

        private void DrawRectangle(CanvasControl sender, CanvasDrawingSession ds)
        {
            var width = (float)sender.ActualWidth;
            var height = (float)sender.ActualHeight;

            int steps = Math.Min((int)(width / 10), 20);

            for (int i = 0; i < steps; ++i)
            {
                var mu = (float)i / steps;

                var color = GradientColor(mu);

                mu *= 0.5f;
                var x = mu * width;
                var y = mu * height;

                var xx = (1 - mu) * width;
                var yy = (1 - mu) * height;

                ds.DrawRectangle(x, y, xx - x, yy - y, color, 2.0f);
            }
        }

        private void DrawRoundedRectangle(CanvasControl sender, CanvasDrawingSession ds)
        {
            var width = (float)sender.ActualWidth;
            var height = (float)sender.ActualHeight;

            int steps = Math.Min((int)(width / 30), 10);

            for (int i = 0; i < steps; ++i)
            {
                var mu = (float)i / steps;

                var color = GradientColor(mu);

                mu *= 0.5f;
                var x = mu * width;
                var y = mu * height;

                var xx = (1 - mu) * width;
                var yy = (1 - mu) * height;

                var radius = mu * 50.0f;

                ds.DrawRoundedRectangle(
                    x, y,
                    xx-x, yy-y,
                    radius, radius,
                    color,
                    2.0f);
            }
        }

        private void DrawCircles(CanvasControl sender, CanvasDrawingSession ds)
        {
            float width = (float)sender.ActualWidth;
            float height = (float)sender.ActualHeight;

            float endpointMargin = Math.Min(width, height) / 8;
            float controlMarginX = endpointMargin * 4;
            float controlMarginY = endpointMargin * 2;

            for (int i = 0; i < 25; i++)
            {
                Vector2[] bez = new Vector2[4];
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
                    float s = 1 - t;
                    float ss = s * s;
                    float sss = ss * s;
                    float tt = t * t;
                    float ttt = tt * t;
                    float x = (sss * bez[0].X) + (3 * ss * t * bez[1].X) + (3 * s * tt * bez[2].X) + (ttt * bez[3].X);
                    float y = (sss * bez[0].Y) + (3 * ss * t * bez[1].Y) + (3 * s * tt * bez[2].Y) + (ttt * bez[3].Y);
                    float radius = ttt * endpointMargin;
                    float strokeWidth = (0.5f - Math.Abs(ss - 0.5f)) * 10;

                    ds.DrawCircle(x, y, radius, GradientColor(t), strokeWidth);
                    t += tStep;
                }
            }
        }

        private static Color GradientColor(float mu)
        {
            byte c = (byte)((Math.Sin(mu * Math.PI * 2) + 1) * 127.5);
            
            return Color.FromArgb(255, 90, 128, c);
        }

        private void UserControl_Unloaded(object sender, RoutedEventArgs e)
        {
            // Explicitly remove references to allow the Win2D controls to get garbage collected
            canvas.RemoveFromVisualTree();
            canvas = null;
        }
    }
}
