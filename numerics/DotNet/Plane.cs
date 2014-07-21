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
using System.Globalization;

using SM = System.Math;

namespace System.Numerics
{
    public struct Plane : IEquatable<Plane>
    {
        public Vector3 Normal;
        public float D;


        public Plane(float x, float y, float z, float d)
        {
            Normal.X = x;
            Normal.Y = y;
            Normal.Z = z;
            D = d;
        }


        public Plane(Vector3 normal, float d)
        {
            Normal = normal;
            D = d;
        }


        public Plane(Vector4 value)
        {
            Normal.X = value.X;
            Normal.Y = value.Y;
            Normal.Z = value.Z;
            D = value.W;
        }


        public static Plane CreateFromVertices(Vector3 point1, Vector3 point2, Vector3 point3)
        {
            Plane result;

            float ax = point2.X - point1.X;
            float ay = point2.Y - point1.Y;
            float az = point2.Z - point1.Z;

            float bx = point3.X - point1.X;
            float by = point3.Y - point1.Y;
            float bz = point3.Z - point1.Z;

            // N=Cross(a,b)
            float nx = ay * bz - az * by;
            float ny = az * bx - ax * bz;
            float nz = ax * by - ay * bx;

            // Normalize(N)
            float ls = nx * nx + ny * ny + nz * nz;
            float invNorm = 1.0f / (float)SM.Sqrt((double)ls);

            result.Normal.X = nx * invNorm;
            result.Normal.Y = ny * invNorm;
            result.Normal.Z = nz * invNorm;

            // D = - Dot(N, point1)
            result.D = -(result.Normal.X * point1.X + result.Normal.Y * point1.Y + result.Normal.Z * point1.Z);

            return result;
        }


        public static Plane Normalize(Plane value)
        {
            const float FLT_EPSILON = 1.192092896e-07f; // smallest such that 1.0+FLT_EPSILON != 1.0

            Plane result;

            float f = value.Normal.X * value.Normal.X + value.Normal.Y * value.Normal.Y + value.Normal.Z * value.Normal.Z;

            if (SM.Abs(f - 1.0f) < FLT_EPSILON)
            {
                result.Normal = value.Normal;
                result.D = value.D;
                return result; // It already normalized, so we don't need to farther process.
            }

            float fInv = 1.0f / (float)SM.Sqrt(f);

            result.Normal.X = value.Normal.X * fInv;
            result.Normal.Y = value.Normal.Y * fInv;
            result.Normal.Z = value.Normal.Z * fInv;

            result.D = value.D * fInv;

            return result;
        }


        public static Plane Transform(Plane plane, Matrix4x4 matrix)
        {
            Matrix4x4 m;
            Matrix4x4.Invert(matrix, out m);

            Plane result;

            float x = plane.Normal.X, y = plane.Normal.Y, z = plane.Normal.Z, w = plane.D;

            result.Normal.X = x * m.M11 + y * m.M12 + z * m.M13 + w * m.M14;
            result.Normal.Y = x * m.M21 + y * m.M22 + z * m.M23 + w * m.M24;
            result.Normal.Z = x * m.M31 + y * m.M32 + z * m.M33 + w * m.M34;

            result.D = x * m.M41 + y * m.M42 + z * m.M43 + w * m.M44;

            return result;
        }


        public static Plane Transform(Plane plane, Quaternion rotation)
        {
            // Compute rotation matrix.
            float x2 = rotation.X + rotation.X;
            float y2 = rotation.Y + rotation.Y;
            float z2 = rotation.Z + rotation.Z;

            float wx2 = rotation.W * x2;
            float wy2 = rotation.W * y2;
            float wz2 = rotation.W * z2;
            float xx2 = rotation.X * x2;
            float xy2 = rotation.X * y2;
            float xz2 = rotation.X * z2;
            float yy2 = rotation.Y * y2;
            float yz2 = rotation.Y * z2;
            float zz2 = rotation.Z * z2;

            float m11 = 1.0f - yy2 - zz2;
            float m21 = xy2 - wz2;
            float m31 = xz2 + wy2;

            float m12 = xy2 + wz2;
            float m22 = 1.0f - xx2 - zz2;
            float m32 = yz2 - wx2;

            float m13 = xz2 - wy2;
            float m23 = yz2 + wx2;
            float m33 = 1.0f - xx2 - yy2;

            Plane result;

            float x = plane.Normal.X, y = plane.Normal.Y, z = plane.Normal.Z;

            result.Normal.X = x * m11 + y * m21 + z * m31;
            result.Normal.Y = x * m12 + y * m22 + z * m32;
            result.Normal.Z = x * m13 + y * m23 + z * m33;

            result.D = plane.D;

            return result;
        }


        public static float Dot(Plane plane, Vector4 value)
        {
            return plane.Normal.X * value.X + 
                   plane.Normal.Y * value.Y + 
                   plane.Normal.Z * value.Z + 
                   plane.D * value.W;
        }


        public static float DotCoordinate(Plane plane, Vector3 value)
        {
            return plane.Normal.X * value.X + 
                   plane.Normal.Y * value.Y + 
                   plane.Normal.Z * value.Z + 
                   plane.D;
        }


        public static float DotNormal(Plane plane, Vector3 value)
        {
            return plane.Normal.X * value.X + 
                   plane.Normal.Y * value.Y + 
                   plane.Normal.Z * value.Z;
        }


        public static bool operator ==(Plane value1, Plane value2)
        {
            return (value1.Normal.X == value2.Normal.X &&
                    value1.Normal.Y == value2.Normal.Y &&
                    value1.Normal.Z == value2.Normal.Z &&
                    value1.D == value2.D);
        }


        public static bool operator !=(Plane value1, Plane value2)
        {
            return (value1.Normal.X != value2.Normal.X || 
                    value1.Normal.Y != value2.Normal.Y || 
                    value1.Normal.Z != value2.Normal.Z || 
                    value1.D != value2.D);
        }


        public bool Equals(Plane other)
        {
            return (Normal.X == other.Normal.X &&
                    Normal.Y == other.Normal.Y &&
                    Normal.Z == other.Normal.Z &&
                    D == other.D);
        }


        public override bool Equals(object obj)
        {
            if (obj is Plane)
            {
                return Equals((Plane)obj);
            }

            return false;
        }


        public override string ToString()
        {
            CultureInfo ci = CultureInfo.CurrentCulture;

            return String.Format(ci, "{{Normal:{0} D:{1}}}", Normal.ToString(), D.ToString(ci));
        }


        public override int GetHashCode()
        {
            return Normal.GetHashCode() + D.GetHashCode();
        }


#if INCLUDE_WINRT_CANVAS_INTEROP

        public static unsafe implicit operator Microsoft.Graphics.Canvas.Numerics.Plane(Plane value)
        {
            return *(Microsoft.Graphics.Canvas.Numerics.Plane*)&value;
        }


        public static unsafe implicit operator Plane(Microsoft.Graphics.Canvas.Numerics.Plane value)
        {
            return *(Plane*)&value;
        }

#endif
    }
}
