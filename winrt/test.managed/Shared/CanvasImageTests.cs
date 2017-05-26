// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

using Microsoft.Graphics.Canvas;
using Microsoft.Graphics.Canvas.Effects;
using Microsoft.VisualStudio.TestPlatform.UnitTestFramework;
using System;
using System.IO;
using System.Threading.Tasks;
using Windows.Foundation;
using Windows.UI;

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
    }
}
