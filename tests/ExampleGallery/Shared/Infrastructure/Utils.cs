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

using Microsoft.Graphics.Canvas;
using Microsoft.Graphics.Canvas.Geometry;
using System;
using System.Collections.Generic;
using System.Linq;
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

        public static Matrix3x2 GetDisplayTransform(Vector2 outputSize, Vector2 sourceSize)
        {
            // Scale the display to fill the control.
            var scale = outputSize / sourceSize;
            var offset = Vector2.Zero;

            // Letterbox or pillarbox to preserve aspect ratio.
            if (scale.X > scale.Y)
            {
                scale.X = scale.Y;
                offset.X = (outputSize.X - sourceSize.X * scale.X) / 2;
            }
            else
            {
                scale.Y = scale.X;
                offset.Y = (outputSize.Y - sourceSize.Y * scale.Y) / 2;
            }

            return Matrix3x2.CreateScale(scale) *
                   Matrix3x2.CreateTranslation(offset);
        }

        public static Matrix3x2 GetDisplayTransform(Size controlSize, ICanvasResourceCreatorWithDpi canvas, int designWidth, int designHeight)
        {
            var sourceSize = new Vector2(canvas.ConvertPixelsToDips(designWidth), canvas.ConvertPixelsToDips(designHeight));

            return GetDisplayTransform(controlSize.ToVector2(), sourceSize);
        }

        public static CanvasGeometry CreateStarGeometry(ICanvasResourceCreator resourceCreator, float scale, Vector2 center)
        {
            Vector2[] points =
            {
                new Vector2(-0.24f, -0.24f),
                new Vector2(0, -1),
                new Vector2(0.24f, -0.24f),
                new Vector2(1, -0.2f),
                new Vector2(0.4f, 0.2f),
                new Vector2(0.6f, 1),
                new Vector2(0, 0.56f),
                new Vector2(-0.6f, 1),
                new Vector2(-0.4f, 0.2f),
                new Vector2(-1, -0.2f),
            };

            var transformedPoints = from point in points
                                    select point * scale + center;

#if WINDOWS_UAP
            var convertedPoints = transformedPoints;
#else
            // Convert the System.Numerics.Vector2 type that we normally work with to the
            // Microsoft.Graphics.Canvas.Numerics.Vector2 struct used by WinRT. These casts
            // are usually inserted automatically, but auto conversion does not work for arrays.
            var convertedPoints = from point in transformedPoints
                                  select (Microsoft.Graphics.Canvas.Numerics.Vector2)point;
#endif

            return CanvasGeometry.CreatePolygon(resourceCreator, convertedPoints.ToArray());
        }

        public static float DegreesToRadians(float angle)
        {
            return angle * (float)Math.PI / 180;
        }

        static readonly Random random = new Random();

        public static Random Random
        {
            get { return random; }
        }

        public static float RandomBetween(float min, float max)
        {
            return min + (float)random.NextDouble() * (max - min);
        }
    }
}
