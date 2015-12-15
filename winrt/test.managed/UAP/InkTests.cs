// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

using Microsoft.Graphics.Canvas;
using Microsoft.VisualStudio.TestPlatform.UnitTestFramework;
using System.Linq;
using System.Reflection;
using Windows.Foundation;
using Windows.UI;
using Windows.UI.Input.Inking;

namespace test.managed
{
    [TestClass]
    public class InkTests
    {
        [TestMethod]
        public void DrawInkDoesNotModifyContextState()
        {
            Point[] points =
            {
                new Point(0, 0),
                new Point(100, 100),
            };

            var strokeBuilder = new InkStrokeBuilder();

            var stroke1 = strokeBuilder.CreateStroke(points);

            strokeBuilder.SetDefaultDrawingAttributes(new InkDrawingAttributes
            {
                Color = Colors.Yellow,
                DrawAsHighlighter = true
            });

            var stroke2 = strokeBuilder.CreateStroke(points);

            using (var device = new CanvasDevice())
            using (var target = new CanvasCommandList(device))
            using (var drawingSession = target.CreateDrawingSession())
            {
                var originalProperties = GetPropertyValues(drawingSession);

                drawingSession.DrawInk(new InkStroke[] { stroke1 });

                var propertiesAfterDraw1 = GetPropertyValues(drawingSession);

                drawingSession.DrawInk(new InkStroke[] { stroke2 });

                var propertiesAfterDraw2 = GetPropertyValues(drawingSession);

                CollectionAssert.AreEqual(originalProperties, propertiesAfterDraw1);
                CollectionAssert.AreEqual(originalProperties, propertiesAfterDraw2);
            }
        }


        static object[] GetPropertyValues(object instance)
        {
            var typeInfo = instance.GetType().GetTypeInfo();

            var values = from property in typeInfo.DeclaredProperties
                         select property.GetValue(instance);

            return values.ToArray();
        }
    }
}
