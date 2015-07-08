// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

using Microsoft.Graphics.Canvas;
using Microsoft.Graphics.Canvas.Text;
using Microsoft.Graphics.Canvas.UI.Xaml;
using System;
using Windows.Foundation;
using Windows.UI;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;

namespace ExampleGallery
{
    public sealed partial class ImageSourceUpdateRegion : UserControl
    {
        int width = 1024;
        int height = 1024;
        int step = 32;
        CanvasImageSource imageSource = null;
        DispatcherTimer dispatcherTimer = new DispatcherTimer();
        Random random = new Random();

        public ImageSourceUpdateRegion()
        {
            this.InitializeComponent();

            dispatcherTimer.Interval = TimeSpan.FromSeconds(0.25);
            dispatcherTimer.Tick += OnTick;
            dispatcherTimer.Start();
        }

        private void UserControl_Loaded(object sender, RoutedEventArgs args)
        {
            CompositionTarget.SurfaceContentsLost += CompositionTarget_SurfaceContentsLost;
            UpdateImageSource();
        }

        private void UserControl_Unloaded(object sender, RoutedEventArgs args)
        {
            dispatcherTimer.Stop();
            CompositionTarget.SurfaceContentsLost -= CompositionTarget_SurfaceContentsLost;
        }

        private void OnTick(object sender, object args)
        {
            UpdateImageSource();
        }

        private void CompositionTarget_SurfaceContentsLost(object sender, object e)
        {
            imageSource = null;
            UpdateImageSource();
        }

        private void UpdateImageSource()
        {
            try
            {
                if (imageSource == null)
                {
                    CreateImageSource();
                }
                else
                {
                    int updateWidth = random.Next(step, Math.Min(width - step, step * 5));
                    int updateHeight = random.Next(step, Math.Min(height - step, step * 5));
                    int x = random.Next(0, width - updateWidth);
                    int y = random.Next(0, height - updateHeight);

                    byte[] color = new byte[3];
                    random.NextBytes(color);

                    Draw(x, y, updateWidth, updateHeight, Color.FromArgb(255, color[0], color[1], color[2]));
                }
            }
            catch (Exception e)
            {
                if (imageSource != null && imageSource.Device.IsDeviceLost(e.HResult))
                {
                    // XAML will also raise a SurfaceContentsLost event, and we use this to trigger redrawing
                    // the surface.
                }
                else
                {
                    throw;
                }
            }
        }

        private void CreateImageSource()
        {
            var device = CanvasDevice.GetSharedDevice(false);
            imageSource = new CanvasImageSource(device, width, height, 96);
            image.Source = imageSource;

            Draw(0, 0, width, height, Colors.Blue);
        }

        private void Draw(int updateLeft, int updateTop, int updateWidth, int updateHeight, Color color)
        {
            int drawTextCalls = 0;

            using (var ds = imageSource.CreateDrawingSession(color, new Rect(updateLeft, updateTop, updateWidth, updateHeight)))
            {
                CanvasTextFormat format = new CanvasTextFormat
                {
                    FontSize = 16,
                    HorizontalAlignment = CanvasHorizontalAlignment.Center,
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

            status.Text = string.Format("There were {0} DrawText() calls on last update", drawTextCalls);
        }
    }
}
