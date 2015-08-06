// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

using Microsoft.Graphics.Canvas;
using Microsoft.VisualStudio.TestPlatform.UnitTestFramework;
using System;
using System.IO;
using System.Threading.Tasks;
using Windows.Graphics.Imaging;
using Windows.Storage;
using Windows.UI;

namespace test.managed
{
    [TestClass]
    public class CanvasBitmapTests
    {
        [TestMethod]
        public void MultithreadedSaveToFile()
        {
            //
            // This test can create a deadlock if that SaveAsync code holds on to the
            // ID2D1Multithread resource lock longer than necessary.
            //
            // A previous implementation waited until destruction of the IAsyncAction before calling Leave().  
            // In managed code this can happen on an arbitrary thread and so could result in deadlock situations
            // where other worker threads were waiting for a Leave on the original thread that never arrived.
            //

            var task = Task.Run(async () =>
           {
               var device = new CanvasDevice();
               var rt = new CanvasRenderTarget(device, 16, 16, 96);

               var filename = Path.Combine(ApplicationData.Current.TemporaryFolder.Path, "testfile.jpg");

               await rt.SaveAsync(filename);

               rt.Dispose();
           });

            task.Wait();
        }

        [TestMethod]
        public void MultithreadedSaveToStream()
        {
            // This is the stream version of the above test

            var task = Task.Run(async () =>
            {
                var device = new CanvasDevice();
                var rt = new CanvasRenderTarget(device, 16, 16, 96);

                var stream = new MemoryStream();
                await rt.SaveAsync(stream.AsRandomAccessStream(), CanvasBitmapFileFormat.Bmp);

                rt.Dispose();
            });

            task.Wait();
        }

    }

#if WINDOWS_UWP
    [TestClass]
    public class CanvasBitmapCreateFromSoftwareBitmapTests
    {
        CanvasDevice device = new CanvasDevice();

        [TestMethod]
        public void CanvasBitmap_CreateFromSoftwareBitmap_Roundtrip()
        {
            var colors = new Color[]
            {
                Colors.Red, Colors.Green, Colors.Yellow,
                Colors.Green, Colors.Yellow, Colors.Red,
                Colors.Yellow, Colors.Red, Colors.Green
            };

            float anyDpi = 10;

            var originalBitmap = CanvasBitmap.CreateFromColors(device, colors, 3, 3, anyDpi);

            var softwareBitmap = SoftwareBitmap.CreateCopyFromSurfaceAsync(originalBitmap, BitmapAlphaMode.Premultiplied).AsTask().Result;
            softwareBitmap.DpiX = anyDpi;
            softwareBitmap.DpiY = anyDpi;

            var roundtrippedBitmap = CanvasBitmap.CreateFromSoftwareBitmap(device, softwareBitmap);

            Assert.AreEqual(originalBitmap.Format, roundtrippedBitmap.Format);
            Assert.AreEqual(anyDpi, roundtrippedBitmap.Dpi);

            var roundtrippedColors = roundtrippedBitmap.GetPixelColors();

            Assert.AreEqual(colors.Length, roundtrippedColors.Length);
            for (var i = 0; i < colors.Length; ++i)
            {
                Assert.AreEqual(colors[i], roundtrippedColors[i]);
            }
        }

        [TestMethod]
        public void CanvasBitmap_CreateFromSoftwareBitmap_PixelFormats()
        {
            foreach (BitmapPixelFormat e in Enum.GetValues(typeof(BitmapPixelFormat)))
            {
                bool hasAlpha = true;

                switch (e)
                {
                    case BitmapPixelFormat.Gray16:
                    case BitmapPixelFormat.Gray8:
                    case BitmapPixelFormat.Nv12:
                    case BitmapPixelFormat.Yuy2:
                        hasAlpha = false;
                        break;
                }

                if (hasAlpha)
                {
                    TestCreateFromSoftwareBitmap(e, BitmapAlphaMode.Premultiplied);
                    TestCreateFromSoftwareBitmap(e, BitmapAlphaMode.Straight);
                    TestCreateFromSoftwareBitmap(e, BitmapAlphaMode.Ignore);
                }
                else
                {
                    TestCreateFromSoftwareBitmap(e, BitmapAlphaMode.Ignore);
                }
            }
        }


        void TestCreateFromSoftwareBitmap(BitmapPixelFormat pixelFormat, BitmapAlphaMode alphaMode)
        {
            if (pixelFormat == BitmapPixelFormat.Unknown)
                return;

            int anyWidth = 3;
            int anyHeight = 5;

            var softwareBitmap = new SoftwareBitmap(pixelFormat, anyWidth, anyHeight, alphaMode);

            if (!IsFormatSupportedByWin2D(pixelFormat, alphaMode))
            {
                Assert.ThrowsException<Exception>(() =>
                {
                    CanvasBitmap.CreateFromSoftwareBitmap(device, softwareBitmap);
                });
                return;
            }

            var canvasBitmap = CanvasBitmap.CreateFromSoftwareBitmap(device, softwareBitmap);

            Assert.AreEqual(anyWidth, (int)canvasBitmap.SizeInPixels.Width);
            Assert.AreEqual(anyHeight, (int)canvasBitmap.SizeInPixels.Height);
            Assert.AreEqual((int)pixelFormat, (int)canvasBitmap.Format);

            CanvasAlphaMode expectedAlphaMode = CanvasAlphaMode.Straight;
            switch (alphaMode)
            {
                case BitmapAlphaMode.Ignore: expectedAlphaMode = CanvasAlphaMode.Ignore; break;
                case BitmapAlphaMode.Premultiplied: expectedAlphaMode = CanvasAlphaMode.Premultiplied; break;
                case BitmapAlphaMode.Straight: expectedAlphaMode = CanvasAlphaMode.Straight; break;
            }

            Assert.AreEqual(expectedAlphaMode, canvasBitmap.AlphaMode);
        }

        bool IsFormatSupportedByWin2D(BitmapPixelFormat format, BitmapAlphaMode alphaMode)
        {
            // Win2D only supports A8UintNormalized with straight alpha.  SoftwareBitmap doesn't
            // support A8UintNormalized.
            if (alphaMode == BitmapAlphaMode.Straight)
                return false;

            switch (format)
            {
                case BitmapPixelFormat.Rgba16:
                    // Rgba16 is DirectXPixelFormat.R16G16B16A16UInt.  Direct2D only supports
                    // the normalized version of this, so we don't expect this format to work.
                    return false;

                case BitmapPixelFormat.Rgba8:
                    // As for Rgba16, this is the non-normalized version which isn't supported by
                    // Direct2D.
                    return false;

                case BitmapPixelFormat.Gray16:
                    return false;

                case BitmapPixelFormat.Gray8:
                    // Direct2D does support R8UintNormalized, but this is the non-normalized one
                    return false;

                case BitmapPixelFormat.Nv12:
                case BitmapPixelFormat.Yuy2:
                    // Direct2D doesn't support these formats
                    return false;
            }

            return true;
        }
    }
#endif
}