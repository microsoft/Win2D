// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

using Microsoft.Graphics.Canvas;
using Microsoft.Graphics.Canvas.Effects;
using Microsoft.VisualStudio.TestPlatform.UnitTestFramework;
using System;
using System.Collections.Generic;
using System.IO;
using System.Threading.Tasks;
using Windows.Foundation;
using Windows.UI;

#if WINDOWS_UWP
using Windows.Graphics.DirectX;
#else
using Microsoft.Graphics.Canvas.DirectX;
#endif

namespace test.managed
{
    [TestClass]
    public class CanvasImageTests
    {
        void TestSaveAsync(Func<CanvasDevice, ICanvasImage> createImage)
        {
            const int width = 23;
            const int height = 42;

            var device = new CanvasDevice();
            var sourceImage = createImage(device);
            var stream = new MemoryStream();

            var saveTask = Task.Run(async () =>
            {
                await CanvasImage.SaveAsync(sourceImage, new Rect(0, 0, width, height), 96, device, stream.AsRandomAccessStream(), CanvasBitmapFileFormat.Png);
            });

            saveTask.Wait();

            CanvasBitmap result = null;

            var loadTask = Task.Run(async () =>
            {
                result = await CanvasBitmap.LoadAsync(device, stream.AsRandomAccessStream());
            });

            loadTask.Wait();

            Assert.AreEqual(width, result.Size.Width);
            Assert.AreEqual(height, result.Size.Height);
        }

        [TestMethod]
        public void CanvasImage_SaveAsync_SourceIsBitmap()
        {
            TestSaveAsync(device =>
            {
                return new CanvasRenderTarget(device, 16, 16, 96);
            });
        }

        [TestMethod]
        public void CanvasImage_SaveAsync_SourceIsEffect()
        {
            TestSaveAsync(device =>
            {
                return new ColorSourceEffect();
            });
        }

        [TestMethod]
        public void CanvasImage_SaveAsync_SourceIsCommandList()
        {
            TestSaveAsync(device =>
            {
                var cl = new CanvasCommandList(device);

                using (var ds = cl.CreateDrawingSession())
                {
                    ds.Clear(Colors.CornflowerBlue);
                }

                return cl;
            });
        }

        [TestMethod]
        public void CanvasImage_SaveAsync_HdrFormats()
        {
            var pixelFormats = new Dictionary<CanvasBufferPrecision, DirectXPixelFormat>
            {
                { CanvasBufferPrecision.Precision8UIntNormalized,  DirectXPixelFormat.B8G8R8A8UIntNormalized     },
                { CanvasBufferPrecision.Precision16Float,          DirectXPixelFormat.R16G16B16A16Float          },
                { CanvasBufferPrecision.Precision32Float,          DirectXPixelFormat.R32G32B32A32Float          },
                { CanvasBufferPrecision.Precision16UIntNormalized, DirectXPixelFormat.R16G16B16A16UIntNormalized },
            };

            CanvasBitmapFileFormat[] fileFormats =
            {
                CanvasBitmapFileFormat.Bmp,
                CanvasBitmapFileFormat.Gif,
                CanvasBitmapFileFormat.Jpeg,
                CanvasBitmapFileFormat.JpegXR,
                CanvasBitmapFileFormat.Png,
                CanvasBitmapFileFormat.Tiff,
            };

            var device = new CanvasDevice();
            var sourceBitmap = new CanvasRenderTarget(device, 1, 1, 96);

            foreach (var pixelFormat in pixelFormats)
            {
                foreach (var fileFormat in fileFormats)
                {
                    bool fileFormatSupportsHdr = (fileFormat == CanvasBitmapFileFormat.JpegXR);

                    using (var stream = new MemoryStream())
                    {
                        var saveTask = CanvasImage.SaveAsync(sourceBitmap,
                                                             sourceBitmap.Bounds,
                                                             sourceBitmap.Dpi,
                                                             device,
                                                             stream.AsRandomAccessStream(),
                                                             fileFormat,
                                                             0.9f,
                                                             pixelFormat.Key).AsTask();
                        saveTask.Wait();

                        var loadTask = CanvasBitmap.LoadAsync(device, stream.AsRandomAccessStream()).AsTask();
                        loadTask.Wait();

                        var loadedBitmap = loadTask.Result;

                        var expectedFormat = pixelFormat.Value;

                        if (!fileFormatSupportsHdr || !device.IsPixelFormatSupported(expectedFormat))
                        {
                            expectedFormat = DirectXPixelFormat.B8G8R8A8UIntNormalized;
                        }

                        Assert.AreEqual(expectedFormat, loadedBitmap.Format, "File format: {0}, pixel format: {1}", fileFormat, pixelFormat);
                    }
                }
            }
        }

        [TestMethod]
        public void CanvasImage_ComputeHistogram()
        {
            Color[] colors =
            {
                Colors.Red, Colors.Lime,
                Colors.Blue, Colors.White
            };

            var device = new CanvasDevice();
            var bitmap = CanvasBitmap.CreateFromColors(device, colors, 2, 2);

            // Histogram over the entire 2x2 bitmap (covering red, green, blue, and white pixels).
            TestHistogram(bitmap, new Rect(0, 0, 2, 2), EffectChannelSelect.Red,   0.5f, 0.5f);
            TestHistogram(bitmap, new Rect(0, 0, 2, 2), EffectChannelSelect.Green, 0.5f, 0.5f);
            TestHistogram(bitmap, new Rect(0, 0, 2, 2), EffectChannelSelect.Blue,  0.5f, 0.5f);

            // Histogram of just the top left 1x1 (single red pixel).
            TestHistogram(bitmap, new Rect(0, 0, 1, 1), EffectChannelSelect.Red,   0, 1);
            TestHistogram(bitmap, new Rect(0, 0, 1, 1), EffectChannelSelect.Green, 1, 0);
            TestHistogram(bitmap, new Rect(0, 0, 1, 1), EffectChannelSelect.Blue,  1, 0);

            // Histogram of the top right 1x1 (single green pixel).
            TestHistogram(bitmap, new Rect(1, 0, 1, 1), EffectChannelSelect.Red,   1, 0);
            TestHistogram(bitmap, new Rect(1, 0, 1, 1), EffectChannelSelect.Green, 0, 1);
            TestHistogram(bitmap, new Rect(1, 0, 1, 1), EffectChannelSelect.Blue,  1, 0);

            // Histogram of the bottom row (blue and white pixels).
            TestHistogram(bitmap, new Rect(0, 1, 2, 1), EffectChannelSelect.Red,   0.5f, 0.5f);
            TestHistogram(bitmap, new Rect(0, 1, 2, 1), EffectChannelSelect.Green, 0.5f, 0.5f);
            TestHistogram(bitmap, new Rect(0, 1, 2, 1), EffectChannelSelect.Blue,  0, 1);
        }

        static void TestHistogram(CanvasBitmap bitmap, Rect sourceRectangle, EffectChannelSelect channelSelect, params float[] expected)
        {
            var histogram = CanvasImage.ComputeHistogram(bitmap, sourceRectangle, bitmap.Device, channelSelect, 2);

            CollectionAssert.AreEqual(expected, histogram);
        }
    }
}
