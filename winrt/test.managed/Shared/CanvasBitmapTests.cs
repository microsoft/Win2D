// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

using Microsoft.Graphics.Canvas;
using Microsoft.VisualStudio.TestPlatform.UnitTestFramework;
using System.IO;
using System.Threading.Tasks;
using System.Threading;
using Windows.ApplicationModel.Core;
using Windows.Storage;
using System;

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

            var task = Task.Run( async () =>
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
}
