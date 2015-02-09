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
using System.Numerics;
using System.Reflection;
using Windows.UI;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;

namespace ExampleGallery
{
    public sealed partial class AnimatedControlExample : UserControl
    {
        public AnimatedControlExample()
        {
            this.InitializeComponent();

            animatedControl.Input.PointerPressed += OnAnimatedControlPointerPressed;
            animatedControl.Input.PointerMoved += OnAnimatedControlPointerMoved;

            var colors = typeof(Colors).GetTypeInfo().DeclaredProperties;
            PropertyInfo transparentPropertyInfo = null;
            foreach (var item in colors)
            {
                clearColor.Items.Add(item);
                if (item.Name == "Transparent")
                    transparentPropertyInfo = item;
            }

            animatedControl.ClearColor = Colors.Transparent;
            clearColor.SelectedItem = transparentPropertyInfo;
        }

        Queue<int> updatesPerDraw = new Queue<int>();

        int drawCount;
        int updatesThisDraw;

        Queue<Vector2> pointerPoints = new Queue<Vector2>();
        const int pointerPointLimit = 100;

        private void OnDraw(ICanvasAnimatedControl sender, CanvasAnimatedDrawEventArgs args)
        {
            int updateCount = (int)(args.Timing.UpdateCount);

            drawCount++;

            var ds = args.DrawingSession;

            var size = sender.Size;
            var middle = size.ToVector2() / 2;
            var radius = (float)size.Width / 10;

            ds.FillCircle(middle, radius, Colors.Blue);
            ds.DrawCircle(middle, radius, Colors.White, 5);

            var dr = radius * 2;

            DrawSweep(ds, updateCount, middle, dr, Colors.Red);
            DrawSweep(ds, drawCount, middle, dr, Colors.Green);

            ds.FillCircle(middle, radius / 5, Colors.Blue);
            ds.DrawCircle(middle, radius / 5, Colors.White, 5);

            ds.DrawText(string.Format("{0} updates", updateCount), 0, 0, Colors.Red);
            ds.DrawText(string.Format("{0} draws", drawCount), 0, 20, Colors.Green);

            const int maxEntries = 120;
            updatesPerDraw.Enqueue(updatesThisDraw);
            while (updatesPerDraw.Count > maxEntries)
                updatesPerDraw.Dequeue();

            ds.Antialiasing = CanvasAntialiasing.Aliased;

            var ew = (float)size.Width / (float)maxEntries;

            System.Diagnostics.Debug.Assert(updatesPerDraw.Count <= maxEntries);
            int index = 0;
            foreach (int update in updatesPerDraw)
            {
                float height = update * 10.0f;
                ds.FillRectangle(ew * index, (float)size.Height - height, ew, height, Colors.Gray);
                index++;
            }

            int pointerPointIndex = 0;
            Vector2 prev = new Vector2(0, 0);
            const float penRadius = 10;
            foreach (Vector2 p in pointerPoints)
            {
                if (pointerPointIndex != 0)
                {
                    ds.DrawLine(prev, p, Colors.DarkRed, penRadius * 2);
                }
                ds.FillEllipse(p, penRadius, penRadius, Colors.DarkRed);
                prev = p;
                pointerPointIndex++;
            }

            updatesThisDraw = 0;
        }

        private void DrawSweep(CanvasDrawingSession ds, int c, Vector2 middle, float radius, Color color)
        {
            var t = (-c / 60.0) * Math.PI;
            ds.DrawLine(middle, middle + new Vector2((float)Math.Sin(t), (float)Math.Cos(t)) * radius, color, 10);
        }

        private void OnUpdate(ICanvasAnimatedControl sender, CanvasAnimatedUpdateEventArgs args)
        {
            if (pointerPoints.Count > 0) pointerPoints.Dequeue();

            updatesThisDraw++;
        }

        private void Pause_Checked(object sender, RoutedEventArgs e)
        {
            var button = (ToggleButton)sender;
            this.animatedControl.Paused = button.IsChecked.Value;
        }

        private void Pause_Unchecked(object sender, RoutedEventArgs e)
        {
            var button = (ToggleButton)sender;
            this.animatedControl.Paused = button.IsChecked.Value;
        }

        void OnAnimatedControlPointerPressed(object sender, PointerEventArgs args)
        {
            pointerPoints.Clear();
        }

        void OnAnimatedControlPointerMoved(object sender, PointerEventArgs args)
        {
            foreach (var point in args.GetIntermediatePoints())
            {
                if (point.IsInContact)
                {
                    if (pointerPoints.Count > pointerPointLimit)
                    {
                        pointerPoints.Dequeue();
                    }

                    pointerPoints.Enqueue(new Vector2((float)point.Position.X, (float)point.Position.Y));
                }
            }
        }

        private void clearColor_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            var color = (Color)((PropertyInfo)clearColor.SelectedItem).GetValue(null);
            animatedControl.ClearColor = color;
        }
    }
}
