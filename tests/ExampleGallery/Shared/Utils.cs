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
using System.Linq;
using System.Collections.Generic;
using Microsoft.Graphics.Canvas;
using Windows.UI.Xaml;
using System.Numerics;
using Windows.Foundation;

namespace ExampleGallery
{
    static class Utils
    {
        public static List<T> GetEnumAsList<T>()
        {
            return Enum.GetValues(typeof(T)).Cast<T>().ToList();
        }

        public static Matrix3x2 GetDisplayTransform(Size controlSize, ICanvasResourceCreatorWithDpi canvas, int designWidth, int designHeight)
        {
            // Scale the display to fill the control.
            Vector2 canvasSize = controlSize.ToVector2();
            var simulationSize = new Vector2(canvas.ConvertPixelsToDips(designWidth), canvas.ConvertPixelsToDips(designHeight));
            var scale = canvasSize / simulationSize;
            var offset = Vector2.Zero;

            // Letterbox or pillarbox to preserve aspect ratio.
            if (scale.X > scale.Y)
            {
                scale.X = scale.Y;
                offset.X = (canvasSize.X - simulationSize.X * scale.X) / 2;
            }
            else
            {
                scale.Y = scale.X;
                offset.Y = (canvasSize.Y - simulationSize.Y * scale.Y) / 2;
            }

            return Matrix3x2.CreateScale(scale) *
                   Matrix3x2.CreateTranslation(offset);
        }

        public static CanvasGeometry CreateStarGeometry(ICanvasResourceCreator resourceCreator, float scale, Vector2 center)
        {
            var pathBuilder = new CanvasPathBuilder(resourceCreator);

            pathBuilder.BeginFigure(new Vector2(-0.24f, -0.24f) * scale + center);
            pathBuilder.AddLine(new Vector2(0, -1) * scale + center);
            pathBuilder.AddLine(new Vector2(0.24f, -0.24f) * scale + center);
            pathBuilder.AddLine(new Vector2(1, -0.2f) * scale + center);
            pathBuilder.AddLine(new Vector2(0.4f, 0.2f) * scale + center);
            pathBuilder.AddLine(new Vector2(0.6f, 1) * scale + center);
            pathBuilder.AddLine(new Vector2(0, 0.56f) * scale + center);
            pathBuilder.AddLine(new Vector2(-0.6f, 1) * scale + center);
            pathBuilder.AddLine(new Vector2(-0.4f, 0.2f) * scale + center);
            pathBuilder.AddLine(new Vector2(-1, -0.2f) * scale + center);
            pathBuilder.EndFigure(CanvasFigureLoop.Closed);

            return CanvasGeometry.CreatePath(pathBuilder);
        }

        public static float DegreesToRadians(int angle)
        {
            return angle * (float)Math.PI / 180;
        }
    }
}
