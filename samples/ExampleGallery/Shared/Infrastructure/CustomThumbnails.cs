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


    // Extend BackgroundTaskExample to generate a custom thumbnail image (not just a rendering capture like most examples).
    partial class BackgroundTaskExample : ICustomThumbnailSource
    {
        IRandomAccessStream ICustomThumbnailSource.Thumbnail
        {
            get
            {
                float size = 512;

                using (var device = new CanvasDevice())
                using (var renderTarget = new CanvasRenderTarget(device, size, size, 96))
                {
                    using (var ds = renderTarget.CreateDrawingSession())
                    {
                        // U+23F0 is ALARM CLOCK
                        ds.DrawText("\u23F0", size / 2, size / 2, Colors.Orange,
                            new CanvasTextFormat()
                            {
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
}
