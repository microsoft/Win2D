// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

using Windows.Foundation;

namespace System.Numerics
{
    public static class VectorExtensions
    {
        public static Point ToPoint(this Vector2 value)
        {
            return new Point(value.X, value.Y);
        }


        public static Size ToSize(this Vector2 value)
        {
            return new Size(value.X, value.Y);
        }


        public static Vector2 ToVector2(this Point value)
        {
            Vector2 result;

            result.X = (float)value.X;
            result.Y = (float)value.Y;

            return result;
        }


        public static Vector2 ToVector2(this Size value)
        {
            Vector2 result;

            result.X = (float)value.Width;
            result.Y = (float)value.Height;

            return result;
        }
    }
}
