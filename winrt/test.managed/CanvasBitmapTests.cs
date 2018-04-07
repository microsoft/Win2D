// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

using Microsoft.Graphics.Canvas;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.Collections.Generic;
using System.IO;
using System.Runtime.InteropServices.WindowsRuntime;
using System.Threading.Tasks;
using Windows.Foundation;
using Windows.Graphics.Imaging;
using Windows.Storage;
using Windows.Storage.Streams;
using Windows.UI;

#if WINDOWS_UWP
using Windows.Graphics.DirectX;
#else
using Microsoft.Graphics.Canvas.DirectX;
#endif

namespace test.managed
{
    [TestClass]
    public class CanvasBitmapTests
    {
        [TestMethod]
        public void MultithreadedSaveToFile()
        {
            //
            // This test can create a deadlock if the SaveAsync code holds on to the
            // ID2D1Multithread resource lock longer than necessary.
            //
            // A previous implementation waited until destruction of the IAsyncAction before calling Leave().  
            // In managed code this can happen on an arbitrary thread and so could result in deadlock situations
            // where other worker threads were waiting for a Leave on the original thread that never arrived.
            //

            using (new DisableDebugLayer()) // 6184116 causes the debug layer to fail when CanvasBitmap::SaveAsync is called
            {
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
        }

        [TestMethod]
        public void MultithreadedSaveToStream()
        {
            // This is the stream version of the above test

            using (new DisableDebugLayer()) // 6184116 causes the debug layer to fail when CanvasBitmap::SaveAsync is called
            {
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

        [TestMethod]
        public void SaveAndLoadHdrBitmapFormats()
        {
            var device = new CanvasDevice();

            var pixelFormats = new List<DirectXPixelFormat>
            {
                DirectXPixelFormat.B8G8R8A8UIntNormalized,
                DirectXPixelFormat.R8G8B8A8UIntNormalized,
                DirectXPixelFormat.A8UIntNormalized,
            };

            if (device.IsPixelFormatSupported(DirectXPixelFormat.R16G16B16A16Float))
            {
                pixelFormats.Add(DirectXPixelFormat.R16G16B16A16Float);
            }

            if (device.IsPixelFormatSupported(DirectXPixelFormat.R32G32B32A32Float))
            {
                pixelFormats.Add(DirectXPixelFormat.R32G32B32A32Float);
            }

            if (device.IsPixelFormatSupported(DirectXPixelFormat.R16G16B16A16UIntNormalized))
            {
                pixelFormats.Add(DirectXPixelFormat.R16G16B16A16UIntNormalized);
            }

            CanvasBitmapFileFormat[] fileFormats =
            {
                CanvasBitmapFileFormat.Bmp,
                CanvasBitmapFileFormat.Gif,
                CanvasBitmapFileFormat.Jpeg,
                CanvasBitmapFileFormat.JpegXR,
                CanvasBitmapFileFormat.Png,
                CanvasBitmapFileFormat.Tiff,
            };

            foreach (var pixelFormat in pixelFormats)
            {
                bool pixelFormatSupportsHdr = (pixelFormat == DirectXPixelFormat.R16G16B16A16Float) ||
                                              (pixelFormat == DirectXPixelFormat.R32G32B32A32Float) ||
                                              (pixelFormat == DirectXPixelFormat.R16G16B16A16UIntNormalized);

                foreach (var fileFormat in fileFormats)
                {
                    bool fileFormatSupportsHdr = (fileFormat == CanvasBitmapFileFormat.JpegXR);

                    using (var sourceBitmap = new CanvasRenderTarget(device, 1, 1, 96, pixelFormat, CanvasAlphaMode.Premultiplied))
                    using (var stream = new MemoryStream())
                    {
                        var saveTask = sourceBitmap.SaveAsync(stream.AsRandomAccessStream(), fileFormat).AsTask();
                        saveTask.Wait();

                        var loadTask = CanvasBitmap.LoadAsync(device, stream.AsRandomAccessStream()).AsTask();
                        loadTask.Wait();

                        var loadedBitmap = loadTask.Result;

                        if (pixelFormatSupportsHdr && fileFormatSupportsHdr)
                        {
                            Assert.AreEqual(pixelFormat, loadedBitmap.Format, "File format: {0}, pixel format: {1}", fileFormat, pixelFormat);
                        }
                        else
                        {
                            Assert.AreEqual(DirectXPixelFormat.B8G8R8A8UIntNormalized, loadedBitmap.Format);
                        }
                    }
                }
            }
        }

        [TestMethod]
        public void CreateFromBuffer()
        {
            var device = new CanvasDevice();

            byte[] somePixelData = { 1, 2, 3, 4 };

            var buffer = somePixelData.AsBuffer();

            // Overload #1
            var bitmap = CanvasBitmap.CreateFromBytes(device, buffer, 2, 2, DirectXPixelFormat.A8UIntNormalized);

            Assert.AreEqual(2u, bitmap.SizeInPixels.Width);
            Assert.AreEqual(2u, bitmap.SizeInPixels.Height);
            Assert.AreEqual(96, bitmap.Dpi);
            Assert.AreEqual(DirectXPixelFormat.A8UIntNormalized, bitmap.Format);
            Assert.AreEqual(CanvasAlphaMode.Premultiplied, bitmap.AlphaMode);

            CollectionAssert.AreEqual(somePixelData, bitmap.GetPixelBytes());

            // Overload #2
            const float someDpi = 123.0f;

            bitmap = CanvasBitmap.CreateFromBytes(device, buffer, 2, 2, DirectXPixelFormat.A8UIntNormalized, someDpi);

            Assert.AreEqual(2u, bitmap.SizeInPixels.Width);
            Assert.AreEqual(2u, bitmap.SizeInPixels.Height);
            Assert.AreEqual(someDpi, bitmap.Dpi);
            Assert.AreEqual(DirectXPixelFormat.A8UIntNormalized, bitmap.Format);
            Assert.AreEqual(CanvasAlphaMode.Premultiplied, bitmap.AlphaMode);

            CollectionAssert.AreEqual(somePixelData, bitmap.GetPixelBytes());

            // Overload #3
            bitmap = CanvasBitmap.CreateFromBytes(device, buffer, 2, 2, DirectXPixelFormat.A8UIntNormalized, someDpi, CanvasAlphaMode.Straight);

            Assert.AreEqual(2u, bitmap.SizeInPixels.Width);
            Assert.AreEqual(2u, bitmap.SizeInPixels.Height);
            Assert.AreEqual(someDpi, bitmap.Dpi);
            Assert.AreEqual(DirectXPixelFormat.A8UIntNormalized, bitmap.Format);
            Assert.AreEqual(CanvasAlphaMode.Straight, bitmap.AlphaMode);

            CollectionAssert.AreEqual(somePixelData, bitmap.GetPixelBytes());

            // Null device.
            Assert.ThrowsException<ArgumentException>(() => CanvasBitmap.CreateFromBytes(null, buffer, 2, 2, DirectXPixelFormat.A8UIntNormalized));
            Assert.ThrowsException<ArgumentException>(() => CanvasBitmap.CreateFromBytes(null, buffer, 2, 2, DirectXPixelFormat.A8UIntNormalized, someDpi));
            Assert.ThrowsException<ArgumentException>(() => CanvasBitmap.CreateFromBytes(null, buffer, 2, 2, DirectXPixelFormat.A8UIntNormalized, someDpi, CanvasAlphaMode.Straight));

            // Null buffer.
            IBuffer nullBuffer = null;

            Assert.ThrowsException<ArgumentException>(() => CanvasBitmap.CreateFromBytes(device, nullBuffer, 2, 2, DirectXPixelFormat.A8UIntNormalized));
            Assert.ThrowsException<ArgumentException>(() => CanvasBitmap.CreateFromBytes(device, nullBuffer, 2, 2, DirectXPixelFormat.A8UIntNormalized, someDpi));
            Assert.ThrowsException<ArgumentException>(() => CanvasBitmap.CreateFromBytes(device, nullBuffer, 2, 2, DirectXPixelFormat.A8UIntNormalized, someDpi, CanvasAlphaMode.Straight));

            // Too small a buffer.
            Assert.ThrowsException<ArgumentException>(() => CanvasBitmap.CreateFromBytes(device, buffer, 3, 3, DirectXPixelFormat.A8UIntNormalized));
        }

        [TestMethod]
        public void SetPixelBytesUsingBuffer()
        {
            var device = new CanvasDevice();
            var bitmap = CanvasBitmap.CreateFromBytes(device, new byte[4], 2, 2, DirectXPixelFormat.A8UIntNormalized);

            // SetPixelBytes(buffer)
            bitmap.SetPixelBytes(new byte[] { 1, 2, 3, 4 }.AsBuffer());

            CollectionAssert.AreEqual(new byte[] { 1, 2, 3, 4 }, bitmap.GetPixelBytes());

            // SetPixelBytes(buffer, rectangle)
            bitmap.SetPixelBytes(new byte[] { 5, 6 }.AsBuffer(), 1, 0, 1, 2);

            CollectionAssert.AreEqual(new byte[] { 1, 5, 3, 6 }, bitmap.GetPixelBytes());

            // Null buffer.
            IBuffer nullBuffer = null;

            Assert.ThrowsException<ArgumentException>(() => bitmap.SetPixelBytes(nullBuffer));
            Assert.ThrowsException<ArgumentException>(() => bitmap.SetPixelBytes(nullBuffer, 0, 0, 1, 1));

            // Too small buffer.
            var oneByteBuffer = new byte[1].AsBuffer();

            Utils.AssertThrowsException<ArgumentException>(() => bitmap.SetPixelBytes(oneByteBuffer), "The array was expected to be of size 4; actual array was of size 1.");
            Utils.AssertThrowsException<ArgumentException>(() => bitmap.SetPixelBytes(oneByteBuffer, 1, 0, 1, 2), "The array was expected to be of size 2; actual array was of size 1.");

            // Bad rectangles.
            Assert.ThrowsException<ArgumentException>(() => bitmap.SetPixelBytes(oneByteBuffer, -1, 0, 1, 1));
            Assert.ThrowsException<ArgumentException>(() => bitmap.SetPixelBytes(oneByteBuffer, 0, -1, 1, 1));
            Assert.ThrowsException<ArgumentException>(() => bitmap.SetPixelBytes(oneByteBuffer, 2, 0, 1, 1));
            Assert.ThrowsException<ArgumentException>(() => bitmap.SetPixelBytes(oneByteBuffer, 0, 2, 1, 1));
            Assert.ThrowsException<ArgumentException>(() => bitmap.SetPixelBytes(oneByteBuffer, 0, 0, 0, 1));
            Assert.ThrowsException<ArgumentException>(() => bitmap.SetPixelBytes(oneByteBuffer, 0, 0, 1, 0));
        }

        [TestMethod]
        public void GetPixelBytesUsingBuffer()
        {
            var device = new CanvasDevice();
            var bitmap = CanvasBitmap.CreateFromBytes(device, new byte[4] { 1, 2, 3, 4 }, 2, 2, DirectXPixelFormat.A8UIntNormalized);

            var array = new byte[4];
            var buffer = array.AsBuffer();

            // GetPixelBytes(buffer)
            bitmap.GetPixelBytes(buffer);

            Assert.AreEqual(4u, buffer.Length);
            Assert.AreEqual(4u, buffer.Capacity);

            CollectionAssert.AreEqual(new byte[] { 1, 2, 3, 4 }, array);

            // GetPixelBytes(buffer, rectangle)
            bitmap.GetPixelBytes(buffer, 1, 0, 1, 2);

            Assert.AreEqual(2u, buffer.Length);
            Assert.AreEqual(4u, buffer.Capacity);

            CollectionAssert.AreEqual(new byte[] { 2, 4, 3, 4 }, array);

            // Null buffer.
            IBuffer nullBuffer = null;

            Assert.ThrowsException<ArgumentException>(() => bitmap.GetPixelBytes(nullBuffer));
            Assert.ThrowsException<ArgumentException>(() => bitmap.GetPixelBytes(nullBuffer, 0, 0, 1, 1));

            // Too small buffer.
            var oneByteBuffer = new byte[1].AsBuffer();

            Utils.AssertThrowsException<ArgumentException>(() => bitmap.GetPixelBytes(oneByteBuffer), "The array was expected to be of size 4; actual array was of size 1.");
            Utils.AssertThrowsException<ArgumentException>(() => bitmap.GetPixelBytes(oneByteBuffer, 1, 0, 1, 2), "The array was expected to be of size 2; actual array was of size 1.");

            // Bad rectangles.
            Assert.ThrowsException<ArgumentException>(() => bitmap.GetPixelBytes(oneByteBuffer, -1, 0, 1, 1));
            Assert.ThrowsException<ArgumentException>(() => bitmap.GetPixelBytes(oneByteBuffer, 0, -1, 1, 1));
            Assert.ThrowsException<ArgumentException>(() => bitmap.GetPixelBytes(oneByteBuffer, 2, 0, 1, 1));
            Assert.ThrowsException<ArgumentException>(() => bitmap.GetPixelBytes(oneByteBuffer, 0, 2, 1, 1));
            Assert.ThrowsException<ArgumentException>(() => bitmap.GetPixelBytes(oneByteBuffer, 0, 0, 0, 1));
            Assert.ThrowsException<ArgumentException>(() => bitmap.GetPixelBytes(oneByteBuffer, 0, 0, 1, 0));
        }

        [TestMethod]
        public void SetPixelBytesReadHazards()
        {
            var device = new CanvasDevice();
            var bitmap = CanvasBitmap.CreateFromBytes(device, new byte[4], 1, 1, DirectXPixelFormat.B8G8R8A8UIntNormalized);
            var renderTarget = new CanvasRenderTarget(device, 2, 1, 96);

            using (var ds = renderTarget.CreateDrawingSession())
            {
                bitmap.SetPixelBytes(new byte[] { 255, 0, 0, 255 });
                ds.DrawImage(bitmap, 0, 0);

                bitmap.SetPixelBytes(new byte[] { 0, 0, 255, 255 });
                ds.DrawImage(bitmap, 1, 0);
            }

            CollectionAssert.AreEqual(new Color[] { Colors.Blue, Colors.Red }, renderTarget.GetPixelColors());
        }

        [TestMethod]
        public void SetPixelColorsReadHazards()
        {
            var device = new CanvasDevice();
            var bitmap = CanvasBitmap.CreateFromColors(device, new Color[1], 1, 1);
            var renderTarget = new CanvasRenderTarget(device, 2, 1, 96);

            using (var ds = renderTarget.CreateDrawingSession())
            {
                bitmap.SetPixelColors(new Color[] { Colors.Blue });
                ds.DrawImage(bitmap, 0, 0);

                bitmap.SetPixelColors(new Color[] { Colors.Red });
                ds.DrawImage(bitmap, 1, 0);
            }

            CollectionAssert.AreEqual(new Color[] { Colors.Blue, Colors.Red }, renderTarget.GetPixelColors());
        }
    }

#if WINDOWS_UWP
    [TestClass]
    public class CanvasBitmapCreateFromSoftwareBitmapTests
    {
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

            var device = new CanvasDevice(true);

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
            using (new DisableDebugLayer())
            {
                var device = new CanvasDevice(true);

                foreach (BitmapPixelFormat e in Enum.GetValues(typeof(BitmapPixelFormat)))
                {
                    // The P010 format is currently marked prerelease. The C# compiler can't see it
                    // (so BitmapPixelFormat.P010 does not resolve) yet Enum.GetValues includes it.
                    // We should skip this type, because SoftwareBitmap does not recognize it.
                    if (e.ToString() == "P010")
                        continue;

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
                        TestCreateFromSoftwareBitmap(device, e, BitmapAlphaMode.Premultiplied);
                        TestCreateFromSoftwareBitmap(device, e, BitmapAlphaMode.Straight);
                        TestCreateFromSoftwareBitmap(device, e, BitmapAlphaMode.Ignore);
                    }
                    else
                    {
                        TestCreateFromSoftwareBitmap(device, e, BitmapAlphaMode.Ignore);
                    }
                }
            }
        }


        void TestCreateFromSoftwareBitmap(CanvasDevice device, BitmapPixelFormat pixelFormat, BitmapAlphaMode alphaMode)
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
            Assert.AreEqual(GetDirectXPixelFormatUsedForBitmapPixelFormat(pixelFormat), canvasBitmap.Format);

            CanvasAlphaMode expectedAlphaMode = CanvasAlphaMode.Straight;
            switch (alphaMode)
            {
                case BitmapAlphaMode.Ignore: expectedAlphaMode = CanvasAlphaMode.Ignore; break;
                case BitmapAlphaMode.Premultiplied: expectedAlphaMode = CanvasAlphaMode.Premultiplied; break;
                case BitmapAlphaMode.Straight: expectedAlphaMode = CanvasAlphaMode.Straight; break;
            }

            Assert.AreEqual(expectedAlphaMode, canvasBitmap.AlphaMode);
        }

        DirectXPixelFormat GetDirectXPixelFormatUsedForBitmapPixelFormat(BitmapPixelFormat format)
        {
            //
            // Although BitmapPixelFormat looks like it corresponds directly to DirectXPixelFormat,
            // it turns out that some of the types are generally intended to be used differently.  Win2D
            // provides these conversions.
            //
            switch (format)
            {
                // The BitmapPixelFormat entry for these types use the plain UInt version.  However,
                // these really were meant to use the UIntNormalized types so Win2D treats them as such.
                case BitmapPixelFormat.Rgba16: return DirectXPixelFormat.R16G16B16A16UIntNormalized;
                case BitmapPixelFormat.Rgba8: return DirectXPixelFormat.R8G8B8A8UIntNormalized;

                // The BitmapPixelFormat entry for Gray8 suggests R8Uint.  However, it's intended to be
                // used as UIntNormalized, plus D2D only supports A8UintNormalized, so that's what is
                // used here.
                case BitmapPixelFormat.Gray8: return DirectXPixelFormat.A8UIntNormalized;

                // Other pixel formats are directly castable to the DirectXPixelFormat.
                default: return (DirectXPixelFormat)format;
            }
        }

        bool IsFormatSupportedByWin2D(BitmapPixelFormat format, BitmapAlphaMode alphaMode)
        {
            // Win2D only supports A8UintNormalized with straight alpha.  SoftwareBitmap doesn't
            // support A8UintNormalized.
            if (alphaMode == BitmapAlphaMode.Straight)
                return false;

            switch (format)
            {
                case BitmapPixelFormat.Gray16:
                case BitmapPixelFormat.Nv12:
                case BitmapPixelFormat.Yuy2:
                    // Direct2D doesn't support these formats
                    return false;

                case BitmapPixelFormat.Gray8:
                    if (alphaMode == BitmapAlphaMode.Ignore)
                        return false;
                    else
                        return true;
            }

            return true;
        }
    }
#endif
}