// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

using Microsoft.Graphics.Canvas;
using Microsoft.Graphics.Canvas.Text;
using System;
using Windows.Storage.Streams;
using Windows.UI;
using Windows.UI.Xaml;

namespace ExampleGallery
{
    interface ICustomThumbnailSource
    {
        IRandomAccessStream Thumbnail { get; }
    }


    static class CustomThumbnailGenerator
    {
        public static IRandomAccessStream GenerateFromString(string str, string fontFamily, Color color)
        {
            float size = 512;

            using (var device = new CanvasDevice())
            using (var renderTarget = new CanvasRenderTarget(device, size, size, 96))
            {
                using (var ds = renderTarget.CreateDrawingSession())
                {
                    ds.DrawText(str, size / 2, size / 2, color,
                        new CanvasTextFormat()
                        {
                            FontFamily = fontFamily,
                            FontSize = size / 2,
                            HorizontalAlignment = CanvasHorizontalAlignment.Center,
                            VerticalAlignment = CanvasVerticalAlignment.Center
                        });
                }

                InMemoryRandomAccessStream stream = new InMemoryRandomAccessStream();
                renderTarget.SaveAsync(stream, CanvasBitmapFileFormat.Png).AsTask().Wait();
                return stream;
            }
        }
    }


    // Extend BackgroundTaskExample to generate a custom thumbnail image (not just a rendering capture like most examples).
    partial class BackgroundTaskExample : ICustomThumbnailSource
    {
        IRandomAccessStream ICustomThumbnailSource.Thumbnail
        {
            get
            {
                // U+23F0 is ALARM CLOCK
                return CustomThumbnailGenerator.GenerateFromString("\u23F0", "Segoe UI", Colors.Orange);
            }
        }
    }


    // CustomControlExample replaces its normal UI with a single glow control when generating thumbnails.
    partial class CustomControlExample
    {
        private void InitializeThumbnail()
        {
            if (ThumbnailGenerator.IsDrawingThumbnail)
            {
                Content = new GlowTextCustomControl()
                {
                    Text = "Glow",
                    GlowAmount = 30,
                    GlowColor = Colors.Yellow,
                    TextColor = Colors.White,
                    HorizontalAlignment = HorizontalAlignment.Center,
                    VerticalAlignment = VerticalAlignment.Center
                };
            }
        }
    }


    // Printing example doesn't have any Win2D content, so we need to draw one
    partial class PrintingExample : ICustomThumbnailSource
    {
        IRandomAccessStream ICustomThumbnailSource.Thumbnail
        {
            get
            {
                // U+2399 is PRINT SCREEN SYMBOL
                return CustomThumbnailGenerator.GenerateFromString("\u2399", "Segoe UI Symbol", Colors.Orchid);
            }
        }
    }
}
