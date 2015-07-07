// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

using Microsoft.Graphics.Canvas.Text;
using Microsoft.Graphics.Canvas.UI.Xaml;
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

            // Explicitly remove references to allow the Win2D controls to get garbage collected
            canvas.RemoveFromVisualTree();
            canvas = null;
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
