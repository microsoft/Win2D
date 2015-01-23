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
using System.IO;
using System.Linq;
using System.Numerics;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;

// The User Control item template is documented at http://go.microsoft.com/fwlink/?LinkId=234236

namespace ExampleGallery
{
    public sealed partial class AnimatedControlExample : UserControl
    {
        public AnimatedControlExample()
        {
            this.InitializeComponent();
        }

        Queue<int> updatesPerDraw = new Queue<int>();

        int drawCount;
        int updatesThisDraw;

        private void OnDraw(ICanvasAnimatedControl sender, CanvasAnimatedDrawEventArgs args)
        {
            int updateCount = (int)(args.Timing.UpdateCount);

            drawCount++;

            var ds = args.DrawingSession;

            var size = sender.Size;
            var middle = new Vector2((float)size.Width / 2, (float)size.Height / 2);
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

            updatesThisDraw = 0;
        }

        private void DrawSweep(CanvasDrawingSession ds, int c, Vector2 middle, float radius, Color color)
        {
            var t = (-c / 60.0) * Math.PI;
            ds.DrawLine(middle, middle + new Vector2((float)Math.Sin(t), (float)Math.Cos(t)) * radius, color, 10);
        }

        private void OnUpdate(ICanvasAnimatedControl sender, CanvasAnimatedUpdateEventArgs args)
        {
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
    }
}
