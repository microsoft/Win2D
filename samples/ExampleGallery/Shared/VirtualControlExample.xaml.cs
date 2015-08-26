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
    public sealed partial class VirtualControlExample : UserControl
    {
        const int step = 32;

        Random random = new Random();

        public VirtualControlExample()
        {
            this.InitializeComponent();
        }

        int regionsInvalidatedEventCount = 0;
        int regionsInvalidatedCount = 0;

        private void OnRegionsInvalidated(CanvasVirtualControl sender, CanvasRegionsInvalidatedEventArgs args)
        {
            ++regionsInvalidatedEventCount;

            var invalidatedRegions = args.InvalidatedRegions;
            regionsInvalidatedCount += invalidatedRegions.Count();

            status.Text = string.Format("{0} RegionsInvalidated events, {1} total regions invalidated", regionsInvalidatedEventCount, regionsInvalidatedCount);

            foreach (var region in invalidatedRegions)
            {
                DrawRegion(sender, region);
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

        private void DrawRegion(CanvasVirtualControl sender, Rect region)
        {
            var tryPanningOrZoomingLayout = new CanvasTextLayout(sender, "Try panning or zooming.", format, 500, 0);

            var infoLayout = new CanvasTextLayout(sender,
                "In this demo, each time a region is updated, it is cleared to a different background color.  " +
                "This is to make it possible to see which regions get redrawn.",
                format, 500, 0);
            
            var youMadeIt = new CanvasTextLayout(sender,
                "You made it to the end!", endFormat, 1000, 0);

            using (var ds = sender.CreateDrawingSession(region))
            {
                ds.Clear(NextColor());

                var left = ((int)(region.X / gridSize) - 1) * gridSize;
                var top = ((int)(region.Y / gridSize) - 1) * gridSize;
                var right = ((int)((region.X + region.Width) / gridSize) + 1) * gridSize;
                var bottom = ((int)((region.Y + region.Height) / gridSize) + 1) * gridSize;
                    
                for (var x = left; x <= right; x += gridSize)
                {
                    for (var y = top; y <= bottom; y += gridSize)
                    {
                        var pos = new Vector2((float)x, (float)y);
                        ds.DrawCircle(pos, 50, Colors.Black, 6);
                        ds.DrawCircle(pos, 50, Colors.White, 4);

                        ds.DrawText(string.Format("{0}\n{1}", x, y), pos, Colors.DarkBlue, coordFormat);
                    }
                }

                if (!ThumbnailGenerator.IsDrawingThumbnail)
                {
                    DrawTextWithBackground(ds, tryPanningOrZoomingLayout, gridSize / 2, gridSize / 2);
                    DrawTextWithBackground(ds, infoLayout, gridSize * 3.5, gridSize * 5.5);
                    DrawTextWithBackground(ds, youMadeIt, sender.ActualWidth - youMadeIt.RequestedSize.Width, sender.ActualHeight - youMadeIt.RequestedSize.Height);
                }
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

        static Color[] colors =
        {
            Colors.SandyBrown,
            Colors.Cornsilk,
            Colors.LightPink,
            Colors.Bisque,
            Colors.LightSalmon,
            Colors.LightGreen,
            Colors.BlanchedAlmond,
            Colors.Wheat,
            Colors.Honeydew
        };

        int lastColorIndex = 0;

        private Color NextColor()
        {
            var color = colors[lastColorIndex];

            lastColorIndex = (lastColorIndex + 1) % colors.Length;

            return color;
        }

        private void UserControl_Unloaded(object sender, RoutedEventArgs args)
        {
            // Explicitly remove references to allow the Win2D controls to get garbage collected
            virtualControl.RemoveFromVisualTree();
            virtualControl = null;
        }
    }
}
