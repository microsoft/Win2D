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
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

// The User Control item template is documented at http://go.microsoft.com/fwlink/?LinkId=234236

namespace ExampleGallery
{
    public sealed partial class ImageSourceUpdateRegion : UserControl
    {
        int width = 1024;
        int height = 1024;
        int step = 32;
        CanvasImageSource imageSource;
        DispatcherTimer dispatcherTimer = new DispatcherTimer();
        Random random = new Random();

        public ImageSourceUpdateRegion()
        {
            this.InitializeComponent();

            dispatcherTimer.Interval = new TimeSpan(0, 0, 0, 0, 250);
            dispatcherTimer.Tick += OnTick;
            dispatcherTimer.Start();
        }

        private void UserControl_Unloaded(object sender, RoutedEventArgs e)
        {
            dispatcherTimer.Stop();
        }

        private void OnTick(object sender, object e)
        {
            int updateWidth = random.Next(step, Math.Min(width - step, step * 5));
            int updateHeight = random.Next(step, Math.Min(height - step, step * 5));
            int x = random.Next(0, width - updateWidth);
            int y = random.Next(0, height - updateHeight);

            byte[] color = new byte[3];
            random.NextBytes(color);

            Draw(x, y, updateWidth, updateHeight, Color.FromArgb(255, color[0], color[1], color[2]));
        }

        private void OnCreateResources(CanvasControl sender, object args)
        {
            imageSource = new CanvasImageSource(sender, width, height);
            image.Source = imageSource;

            Draw(0, 0, width, height, Colors.Blue);
        }

        private void Draw(int updateLeft, int updateTop, int updateWidth, int updateHeight, Color color)
        {
            if (imageSource == null)
                return;

            int drawTextCalls = 0;

            try
            {
                using (var ds = imageSource.CreateDrawingSession(color, new Rect(updateLeft, updateTop, updateWidth, updateHeight)))
                {
                    CanvasTextFormat format = new CanvasTextFormat
                    {
                        FontSize = 16,
                        ParagraphAlignment = Windows.UI.Text.ParagraphAlignment.Center,
                        VerticalAlignment = CanvasVerticalAlignment.Center
                    };

                    // Round update region to step
                    updateLeft = (updateLeft / step) * step;
                    updateTop = (updateTop / step) * step;
                    int updateRight = updateLeft + ((updateWidth / step) + 1) * step;
                    int updateBottom = updateTop + ((updateHeight / step) + 1) * step;

                    for (int x = updateLeft; x <= updateRight; x += step)
                    {
                        for (int y = updateTop; y <= updateBottom; y += step)
                        {
                            int n = (x / step) + (y / step) * width;
                            var str = (n % 99).ToString();

                            ds.DrawText(str, x, y, Colors.White, format);
                            drawTextCalls++;
                        }
                    }
                }
            }
            catch (System.Runtime.InteropServices.COMException e)
            {
                // Ignore device lost errors; ultimately, the control will be notified that surfaces have been lost and will call OnCreateResources
                switch ((uint)e.HResult)
                {
                    case 0x887A0006: // DXGI_ERROR_DEVICE_HUNG
                    case 0x887A0005: // DXGI_ERROR_DEVICE_REMOVED
                    case 0x887A0007: // DXGI_ERROR_DEVICE_RESET
                    case 0x887A0020: // DXGI_ERROR_DRIVER_INTERNAL_ERROR
                        break;
                    default:
                        throw;
                }
            }

            status.Text = string.Format("There were {0} DrawText() calls on last update", drawTextCalls);
        }
    }
}
