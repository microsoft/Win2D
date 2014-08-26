// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may
// not use these files except in compliance with the License. You may obtain
// a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
// License for the specific language governing permissions and limitations
// under the License.

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Microsoft.VisualStudio.TestPlatform.UnitTestFramework;
using Microsoft.Graphics.Canvas;
using Microsoft.Graphics.Canvas.DirectX;
using Microsoft.Graphics.Canvas.DirectX.Direct3D11;

namespace test.managed
{
    [TestClass]
    public class GraphicsInterop
    {
        [TestMethod]
        public void GraphicsInteropFromManaged()
        {
            //
            // This test interops a C# component (ie this one) with a C++/CX 
            // component (GraphicsDeviceComponent) using Direct3DDevice and 
            // Direct3DSurface to pass a IDXGIDevice and IDXGISurface between 
            // them.
            //

            // Create a device.
            Direct3DDevice graphicsDevice = NativeComponent.DeviceCreator.CreateDevice();

            // We should be able to call Trim() without anything bad happening
            graphicsDevice.Trim();

            // Now create a surface using this device. 
            int expectedWidth = 128;
            int expectedHeight = 256;
            var expectedGraphicsFormat = DirectXPixelFormat.R32Float;

            Direct3DSurface surface = NativeComponent.SurfaceCreator.CreateSurface(
                graphicsDevice,
                expectedWidth,
                expectedHeight,
                expectedGraphicsFormat);

            // Look at the surface description to make sure it seems valid.
            var desc = surface.Description;

            Assert.AreEqual(expectedWidth, desc.Width);
            Assert.AreEqual(expectedHeight, desc.Height);
            Assert.AreEqual(expectedGraphicsFormat, desc.Format);
            Assert.AreEqual(1, desc.MultisampleDescription.Count);
            Assert.AreEqual(0, desc.MultisampleDescription.Quality);
        }
    }
}
