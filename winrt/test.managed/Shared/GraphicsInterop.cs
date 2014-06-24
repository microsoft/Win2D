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
            Direct3DDevice graphicsDevice = GraphicsDeviceComponent.DeviceCreator.CreateDevice();

            // We should be able to call Trim() without anything bad happening
            graphicsDevice.Trim();

            // Now create a surface using this device. 
            int expectedWidth = 128;
            int expectedHeight = 256;
            var expectedGraphicsFormat = DirectXPixelFormat.R32Float;

            Direct3DSurface surface = GraphicsDeviceComponent.SurfaceCreator.CreateSurface(
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
