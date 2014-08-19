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
