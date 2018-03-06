// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

using Microsoft.VisualStudio.TestTools.UnitTesting;
using Microsoft.Graphics.Canvas;
using Microsoft.Graphics.Canvas.Geometry;

#if WINDOWS_UWP
using System.Numerics;
#else
using Microsoft.Graphics.Canvas.Numerics;
#endif

namespace test.managed
{
    [TestClass]
    public class PathReceiver
    {
        private class MyGeometryStreamReader : ICanvasPathReceiver
        {
            bool wasBeginFigureCalled;
            bool wasAddArcCalled;
            bool wasAddCubicBezierCalled;
            bool wasAddQuadraticBezierCalled;
            bool wasAddLineCalled;
            bool wasSetFilledRegionDeterminationCalled;
            bool wasSetSegmentOptionsCalled;
            bool wasEndFigureCalled;

            public void BeginFigure(Vector2 point, CanvasFigureFill fill)
            {
                wasBeginFigureCalled = true;
            }
            public void AddArc(Vector2 point, float x, float y, float z, CanvasSweepDirection sweepDirection, CanvasArcSize arcSize)
            {
                wasAddArcCalled = true;
            }
            public void AddCubicBezier(Vector2 controlPoint1, Vector2 controlPoint2, Vector2 endPoint)
            {
                wasAddCubicBezierCalled = true;
            }
            public void AddLine(Vector2 endPoint)
            {
                wasAddLineCalled = true;
            }
            public void AddQuadraticBezier(Vector2 controlPoint, Vector2 endPoint)
            {
                wasAddQuadraticBezierCalled = true;
            }

            public void SetFilledRegionDetermination(CanvasFilledRegionDetermination filledRegionDetermination)
            {
                wasSetFilledRegionDeterminationCalled = true;
            }

            public void SetSegmentOptions(CanvasFigureSegmentOptions figureSegmentOptions)
            {
                wasSetSegmentOptionsCalled = true;
            }

            public void EndFigure(CanvasFigureLoop figureLoop)
            {
                wasEndFigureCalled = true;
            }

            public void Verify()
            {
                Assert.IsTrue(wasBeginFigureCalled);
                Assert.IsTrue(wasAddArcCalled);
                Assert.IsTrue(wasAddCubicBezierCalled);
                Assert.IsTrue(wasAddQuadraticBezierCalled);
                Assert.IsTrue(wasAddLineCalled);
                Assert.IsTrue(wasSetFilledRegionDeterminationCalled);
                Assert.IsTrue(wasSetSegmentOptionsCalled);
                Assert.IsTrue(wasEndFigureCalled);
            }
        }

        [TestMethod]
        public void VerifySendPathTo()
        {
            //
            // This calls each of the path functions, once, and verfies that it works.
            // Sink behavior is verified in more detail in the geometry unit tests.
            //
            CanvasDevice device = new CanvasDevice();

            CanvasPathBuilder pathBuilder = new CanvasPathBuilder(device);
            pathBuilder.SetFilledRegionDetermination(CanvasFilledRegionDetermination.Alternate);
            pathBuilder.BeginFigure(0, 0);
            pathBuilder.AddLine(0, 0);
            pathBuilder.AddQuadraticBezier(new Vector2(), new Vector2());
            pathBuilder.AddCubicBezier(new Vector2(), new Vector2(), new Vector2());

            // D2D tries to be smart about degenerate arcs and redundant set segment options, and may sometimes actually
            // surpress them. Therefore, these calls use non-defaults.
            pathBuilder.AddArc(new Vector2 { X = 100, Y = 100 }, 10, 10, 90, CanvasSweepDirection.Clockwise, CanvasArcSize.Small);
            pathBuilder.SetSegmentOptions(CanvasFigureSegmentOptions.ForceUnstroked);

            pathBuilder.EndFigure(CanvasFigureLoop.Closed);

            CanvasGeometry pathGeometry = CanvasGeometry.CreatePath(pathBuilder);

            MyGeometryStreamReader myStreamReader = new MyGeometryStreamReader();
            pathGeometry.SendPathTo(myStreamReader);

            myStreamReader.Verify();
        }
    }
}
