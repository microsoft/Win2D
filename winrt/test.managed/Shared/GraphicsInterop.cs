using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Microsoft.VisualStudio.TestPlatform.UnitTestFramework;
using Microsoft.Graphics.Canvas;

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
            // component (GraphicsDeviceComponent) using DirectX11Device and 
            // DirectX11Surface to pass a IDXGIDevice and IDXGISurface between 
            // them.
            //

            // Create a device.
            DirectX11Device graphicsDevice = GraphicsDeviceComponent.DeviceCreator.CreateDevice();

            // We should be able to call Trim() without anything bad happening
            graphicsDevice.Trim();

            // Now create a surface using this device. 
            uint expectedWidth = 128;
            uint expectedHeight = 256;
            var expectedGraphicsFormat = GraphicsResourceFormat.R32_FLOAT;

            DirectX11Surface surface = GraphicsDeviceComponent.SurfaceCreator.CreateSurface(
                graphicsDevice,
                expectedWidth,
                expectedHeight,
                expectedGraphicsFormat);

            // Look at the surface description to make sure it seems valid.
            var desc = surface.Description;

            Assert.AreEqual(expectedWidth, desc.Width);
            Assert.AreEqual(expectedHeight, desc.Height);
            Assert.AreEqual(expectedGraphicsFormat, desc.Format);
            Assert.AreEqual(1U, desc.MultisampleDescription.Count);
            Assert.AreEqual(0U, desc.MultisampleDescription.Quality);
        }
    }
}
