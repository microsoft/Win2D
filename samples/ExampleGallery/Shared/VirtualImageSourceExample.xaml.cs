// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

using Microsoft.Graphics.Canvas;
using Microsoft.Graphics.Canvas.Text;
using Microsoft.Graphics.Canvas.UI.Xaml;
using System;
using System.Linq;
using System.Numerics;
using Windows.Foundation;
using Windows.UI;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;

namespace ExampleGallery
{
    public sealed partial class VirtualImageSourceExample : UserControl
    {
        const int width = 10000;
        const int height = 10000;
        const float dpi = 96;
        const int step = 32;

        CanvasVirtualImageSource imageSource = null;

        Random random = new Random();

        public VirtualImageSourceExample()
        {
            this.InitializeComponent();
        }

        private void UserControl_Loaded(object sender, RoutedEventArgs args)
        {
            CompositionTarget.SurfaceContentsLost += CompositionTarget_SurfaceContentsLost;

            var device = CanvasDevice.GetSharedDevice(false);
            imageSource = new CanvasVirtualImageSource(device, width, height, dpi);
            imageSource.RegionsInvalidated += ImageSource_RegionsInvalidated;
            image.Source = imageSource.Source;
        }

        private void UserControl_Unloaded(object sender, RoutedEventArgs args)
        {
            CompositionTarget.SurfaceContentsLost -= CompositionTarget_SurfaceContentsLost;
            imageSource.RegionsInvalidated -= ImageSource_RegionsInvalidated;
        }

        private void CompositionTarget_SurfaceContentsLost(object sender, object e)
        {
            RecreateDevice();
        }

        private void RecreateDevice()
        {
            var device = CanvasDevice.GetSharedDevice(false);
            if (imageSource.Device != device)
                imageSource.Recreate(device);
        }

        int regionsInvalidatedEventCount = 0;
        int regionsInvalidatedCount = 0;

        private void ImageSource_RegionsInvalidated(CanvasVirtualImageSource sender, CanvasRegionsInvalidatedEventArgs args)
        {
            ++regionsInvalidatedEventCount;

            var invalidatedRegions = args.InvalidatedRegions;
            regionsInvalidatedCount += invalidatedRegions.Count();

            status.Text = string.Format("{0} RegionsInvalidated events, {1} total regions invalidated", regionsInvalidatedEventCount, regionsInvalidatedCount);

            try
            {
                foreach (var region in invalidatedRegions)
                {
                    DrawRegion(sender, region);
                }
            }
            catch (Exception e)
            {
                if (!sender.Device.IsDeviceLost(e.HResult))
                    throw;

                sender.Device.RaiseDeviceLost();
            }        
        }

        CanvasTextFormat format = new CanvasTextFormat()
        {
            HorizontalAlignment = CanvasHorizontalAlignment.Left,
            VerticalAlignment = CanvasVerticalAlignment.Center
        };

        CanvasTextFormat endFormat = new CanvasTextFormat()
        {
            HorizontalAlignment = CanvasHorizontalAlignment.Right,
            VerticalAlignment = CanvasVerticalAlignment.Bottom,
            FontFamily = "Comic Sans MS",
            FontSize = 48
        };

        CanvasTextFormat coordFormat = new CanvasTextFormat()
        {
            HorizontalAlignment = CanvasHorizontalAlignment.Center,
            VerticalAlignment = CanvasVerticalAlignment.Center,
            FontSize = 10
        };

        const int gridSize = 150;

        private void DrawRegion(CanvasVirtualImageSource sender, Rect region)
        {
            var tryPanningOrZoomingLayout = new CanvasTextLayout(sender.Device, "Try panning or zooming.", format, 500, 0);

            var infoLayout = new CanvasTextLayout(sender.Device,
                "In this demo, each time a region is updated, it is cleared to a different background color.  " +
                "This is to make it possible to see which regions get redrawn.",
                format, 500, 0);
            
            var youMadeIt = new CanvasTextLayout(sender.Device,
                "You made it to the end!", endFormat, 1000, 0);

            using (var ds = sender.CreateDrawingSession(NextColor(), region))
            {
                var left = ((int)(region.X / gridSize) - 1) * gridSize;
                var top = ((int)(region.Y / gridSize) - 1) * gridSize;
                var right = ((int)((region.X + region.Width) / gridSize) + 1) * gridSize;
                var bottom = ((int)((region.Y + region.Height) / gridSize) + 1) * gridSize;
                    
                for (var x = left; x <= right; x += gridSize)
                {
                    for (var y = top; y <= bottom; y += gridSize)
                    {
                        var pos = new Vector2((float)x, (float)y);
                        ds.DrawCircle(pos, 10, Colors.White);

                        ds.DrawText(string.Format("{0}\n{1}", x, y), pos, Colors.DarkBlue, coordFormat);
                    }
                }

                DrawTextWithBackground(ds, tryPanningOrZoomingLayout, gridSize / 2, gridSize / 2);
                DrawTextWithBackground(ds, infoLayout, gridSize * 3.5, gridSize * 5.5);
                DrawTextWithBackground(ds, youMadeIt, width - youMadeIt.RequestedSize.Width, height - youMadeIt.RequestedSize.Height);
            }
        }

        private void DrawTextWithBackground(CanvasDrawingSession ds, CanvasTextLayout layout, double x, double y)
        {
            var backgroundRect = layout.LayoutBounds;
            backgroundRect.X += x;
            backgroundRect.Y += y;

            backgroundRect.X -= 20;
            backgroundRect.Y -= 20;
            backgroundRect.Width += 40;
            backgroundRect.Height += 40;

            ds.FillRectangle(backgroundRect, Colors.White);
            ds.DrawTextLayout(layout, (float)x, (float)y, Colors.Black);
        }

        private Color NextColor()
        {
            var brightness = 3;
            var r = (random.NextDouble() + brightness) / (1 + brightness);
            var g = (random.NextDouble() + brightness) / (1 + brightness);
            var b = (random.NextDouble() + brightness) / (1 + brightness);

            return Color.FromArgb(255, (byte)(r * 255), (byte)(g * 255), (byte)(b * 255));
        }
    }
}
