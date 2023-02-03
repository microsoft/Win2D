// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

using Microsoft.Graphics.Canvas;
using Microsoft.Graphics.Canvas.Effects;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.Numerics;
using System.Runtime.InteropServices;
using Windows.Foundation;
using Windows.UI;

namespace test.managed
{
    [TestClass]
    public unsafe class NativeExportTests
    {
        private static Guid UuidOfICanvasImageInterop = new Guid("E042D1F7-F9AD-4479-A713-67627EA31863");
        private static Guid UuidOfICanvasResourceCreator = new Guid("8F6D8AA8-492F-4BC6-B3D0-E7F5EAE84B11");
        private static Guid UuidOfICanvasResourceCreatorWithDpi = new Guid("1A75B512-E9FA-49E6-A876-38CAE194013E");
        private static Guid UuidOfICanvasEffect = new Guid("0EF96F8C-9B5E-4BF0-A399-AAD8CE53DB55");

        [TestMethod]
        public void GetBoundsForICanvasImageInterop_IsExportedCorrectly()
        {
            CanvasDevice canvasDevice = new CanvasDevice();

            // Create a dummy 256x256 image
            CanvasBitmap canvasBitmap = CanvasBitmap.CreateFromColors(
                resourceCreator: canvasDevice,
                colors: new Color[256 * 256],
                widthInPixels: 256,
                heightInPixels: 256,
                dpi: 96.0f,
                alpha: CanvasAlphaMode.Ignore);

            // Create some effect around it
            InvertEffect invertEffect = new InvertEffect { Source = canvasBitmap };

            // Get the ICanvasResourceCreator and ICanvasImageInterop objects for the device and effect
            using (ComPtr canvasDeviceUnknown = ComPtr.FromObject(canvasDevice))
            using (ComPtr invertEffectUnknown = ComPtr.FromObject(invertEffect))
            using (ComPtr canvasDeviceResourceCreator = canvasDeviceUnknown.AsRIID(UuidOfICanvasResourceCreator))
            using (ComPtr invertEffectInterop = invertEffectUnknown.AsRIID(UuidOfICanvasImageInterop))
            {
                Rect rect;

                // Invoke GetBoundsForICanvasImageInterop with the test objects
                int hresult = GetBoundsForICanvasImageInterop(
                    resourceCreator: canvasDeviceResourceCreator.ThisPtr,
                    image: invertEffectInterop.ThisPtr,
                    transform: null,
                    rect: &rect);

                // The call must have been successful, and the size must match
                Assert.AreEqual(0, hresult);
                Assert.AreEqual(new Rect(0, 0, canvasBitmap.Size.Width, canvasBitmap.Size.Height), rect);
            }
        }

        [TestMethod]
        public void InvalidateSourceRectangleForICanvasImageInterop_IsExportedCorrectly()
        {
            CanvasDevice canvasDevice = new CanvasDevice();

            // Create a dummy 256x256 image
            CanvasBitmap canvasBitmap = CanvasBitmap.CreateFromColors(
                resourceCreator: canvasDevice,
                colors: new Color[256 * 256],
                widthInPixels: 256,
                heightInPixels: 256,
                dpi: 96.0f,
                alpha: CanvasAlphaMode.Ignore);

            // Create some effect around it
            InvertEffect invertEffect = new InvertEffect { Source = canvasBitmap };

            // Create a render target and a drawing surface on it
            CanvasRenderTarget canvasRenderTarget = new CanvasRenderTarget(
                resourceCreator: canvasDevice,
                width: 256,
                height: 256,
                dpi: 96.0f);

            // Get the ICanvasResourceCreator and ICanvasImageInterop objects for the device and effect
            using (CanvasDrawingSession canvasDrawingSession = canvasRenderTarget.CreateDrawingSession())
            using (ComPtr canvasDrawingSessionUnknown = ComPtr.FromObject(canvasDrawingSession))
            using (ComPtr invertEffectUnknown = ComPtr.FromObject(invertEffect))
            using (ComPtr canvasDrawingSessionResourceCreatorWithDpi = canvasDrawingSessionUnknown.AsRIID(UuidOfICanvasResourceCreatorWithDpi))
            using (ComPtr invertEffectInterop = invertEffectUnknown.AsRIID(UuidOfICanvasImageInterop))
            {
                Rect invalidRectangle = default;

                // Invoke InvalidateSourceRectangleForICanvasImageInterop with the test objects
                int hresult = InvalidateSourceRectangleForICanvasImageInterop(
                    resourceCreator: canvasDrawingSessionResourceCreatorWithDpi.ThisPtr,
                    image: invertEffectInterop.ThisPtr,
                    sourceIndex: 0,
                    invalidRectangle: &invalidRectangle);

                // The call must have been successful
                Assert.AreEqual(0, hresult);
            }
        }

        [TestMethod]
        public void GetInvalidRectanglesForICanvasImageInterop_IsExportedCorrectly()
        {
            CanvasDevice canvasDevice = new CanvasDevice();

            // Create a dummy 256x256 image
            CanvasBitmap canvasBitmap = CanvasBitmap.CreateFromColors(
                resourceCreator: canvasDevice,
                colors: new Color[256 * 256],
                widthInPixels: 256,
                heightInPixels: 256,
                dpi: 96.0f,
                alpha: CanvasAlphaMode.Ignore);

            // Create some effect around it
            InvertEffect invertEffect = new InvertEffect { Source = canvasBitmap };

            // Create a render target and a drawing surface on it
            CanvasRenderTarget canvasRenderTarget = new CanvasRenderTarget(
                resourceCreator: canvasDevice,
                width: 256,
                height: 256,
                dpi: 96.0f);

            // Get the ICanvasResourceCreator and ICanvasImageInterop objects for the device and effect
            using (CanvasDrawingSession canvasDrawingSession = canvasRenderTarget.CreateDrawingSession())
            using (ComPtr canvasDrawingSessionUnknown = ComPtr.FromObject(canvasDrawingSession))
            using (ComPtr invertEffectUnknown = ComPtr.FromObject(invertEffect))
            using (ComPtr canvasDrawingSessionResourceCreatorWithDpi = canvasDrawingSessionUnknown.AsRIID(UuidOfICanvasResourceCreatorWithDpi))
            using (ComPtr invertEffectInterop = invertEffectUnknown.AsRIID(UuidOfICanvasImageInterop))
            {
                uint count = 1;
                Rect rect = default;
                Rect* rectPtr = &rect;

                // Invoke GetInvalidRectanglesForICanvasImageInterop with the test objects
                int hresult = GetInvalidRectanglesForICanvasImageInterop(
                    resourceCreator: canvasDrawingSessionResourceCreatorWithDpi.ThisPtr,
                    image: invertEffectInterop.ThisPtr,
                    valueCount: &count,
                    valueElements: &rectPtr);

                // The call must have been successful
                Assert.AreEqual(0, hresult);
            }
        }

        [TestMethod]
        public void GetRequiredSourceRectanglesForICanvasImageInterop_IsExportedCorrectly()
        {
            CanvasDevice canvasDevice = new CanvasDevice();

            // Create a dummy 256x256 image
            CanvasBitmap canvasBitmap = CanvasBitmap.CreateFromColors(
                resourceCreator: canvasDevice,
                colors: new Color[256 * 256],
                widthInPixels: 256,
                heightInPixels: 256,
                dpi: 96.0f,
                alpha: CanvasAlphaMode.Ignore);

            // Create some effects around it
            InvertEffect invertEffect1 = new InvertEffect { Source = canvasBitmap };
            InvertEffect invertEffect2 = new InvertEffect { Source = invertEffect1 };

            // Create a render target and a drawing surface on it
            CanvasRenderTarget canvasRenderTarget = new CanvasRenderTarget(
                resourceCreator: canvasDevice,
                width: 256,
                height: 256,
                dpi: 96.0f);

            // Get the ICanvasResourceCreator and ICanvasImageInterop objects for the device and effects
            using (CanvasDrawingSession canvasDrawingSession = canvasRenderTarget.CreateDrawingSession())
            using (ComPtr canvasDrawingSessionUnknown = ComPtr.FromObject(canvasDrawingSession))
            using (ComPtr invertEffect1Unknown = ComPtr.FromObject(invertEffect1))
            using (ComPtr invertEffect2Unknown = ComPtr.FromObject(invertEffect2))
            using (ComPtr canvasDrawingSessionResourceCreatorWithDpi = canvasDrawingSessionUnknown.AsRIID(UuidOfICanvasResourceCreatorWithDpi))
            using (ComPtr invertEffect2Interop = invertEffect2Unknown.AsRIID(UuidOfICanvasImageInterop))
            using (ComPtr invertEffect1Abi = invertEffect1Unknown.AsRIID(UuidOfICanvasEffect))
            {
                Rect outputRect = new Rect(0, 0, 256, 256);
                uint sourceIndices = 0;
                Rect sourceBounds = new Rect(0, 0, 256, 256);
                Rect resultRect;

                // Invoke GetRequiredSourceRectanglesForICanvasImageInterop with the test objects
                int hresult = GetRequiredSourceRectanglesForICanvasImageInterop(
                    resourceCreator: canvasDrawingSessionResourceCreatorWithDpi.ThisPtr,
                    image: invertEffect2Interop.ThisPtr,
                    outputRectangle: &outputRect,
                    sourceEffectCount: 1,
                    sourceEffects: &invertEffect1Abi.ThisPtr,
                    sourceIndexCount: 1,
                    sourceIndices: &sourceIndices,
                    sourceBoundsCount: 1,
                    sourceBounds: &sourceBounds,
                    valueCount: 1,
                    valueElements: &resultRect);

                // The call must have been successful
                Assert.AreEqual(0, hresult);
            }
        }

        [DllImport("Microsoft.Graphics.Canvas.dll", PreserveSig = true, ExactSpelling = true)]
        public static extern int GetBoundsForICanvasImageInterop(void* resourceCreator, void* image, Matrix3x2* transform, Rect* rect);

        [DllImport("Microsoft.Graphics.Canvas.dll", PreserveSig = true, ExactSpelling = true)]
        public static extern int InvalidateSourceRectangleForICanvasImageInterop(
            void* resourceCreator,
            void* image,
            uint sourceIndex,
            Rect* invalidRectangle);

        [DllImport("Microsoft.Graphics.Canvas.dll", PreserveSig = true, ExactSpelling = true)]
        public static extern int GetInvalidRectanglesForICanvasImageInterop(
            void* resourceCreator,
            void* image,
            uint* valueCount,
            Rect** valueElements);

        [DllImport("Microsoft.Graphics.Canvas.dll", PreserveSig = true, ExactSpelling = true)]
        public static extern int GetRequiredSourceRectanglesForICanvasImageInterop(
            void* resourceCreator,
            void* image,
            Rect* outputRectangle,
            uint sourceEffectCount,
            void** sourceEffects,
            uint sourceIndexCount,
            uint* sourceIndices,
            uint sourceBoundsCount,
            Rect* sourceBounds,
            uint valueCount,
            Rect* valueElements);

        /// <summary>
        /// A small helper for working with COM types.
        /// </summary>
        private struct ComPtr : IDisposable
        {
            /// <summary>
            /// The underlying <c>IUnknown</c> object.
            /// </summary>
            public void* ThisPtr;

            /// <summary>
            /// Creates a new <see cref="ComPtr"/> instance from the input RCW.
            /// </summary>
            /// <param name="obj">The input RCW to unwrap.</param>
            /// <returns>The resulting <see cref="ComPtr"/> instance.</returns>
            public static ComPtr FromObject(object obj)
            {
                return new ComPtr { ThisPtr = (void*)Marshal.GetIUnknownForObject(obj) };
            }

            /// <summary>
            /// Invokes <c>QueryInterface</c> for a given RIID.
            /// </summary>
            /// <param name="guid">The target RIID.</param>
            /// <returns>The resulting <see cref="ComPtr"/> instance.</returns>
            public ComPtr AsRIID(Guid guid)
            {
                Marshal.ThrowExceptionForHR(Marshal.QueryInterface((IntPtr)ThisPtr, ref guid, out IntPtr ppv));

                return new ComPtr { ThisPtr = (void*)ppv };
            }

            /// <inheritdoc/>
            public void Dispose()
            {
                if (ThisPtr != null)
                {
                    void* thisPtr = ThisPtr;

                    ThisPtr = null;

                    Marshal.Release((IntPtr)thisPtr);
                }
            }
        }
    }
}
