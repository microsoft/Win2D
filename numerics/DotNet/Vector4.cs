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
    public struct Vector4 : IEquatable<Vector4>
    {
        public float X;
        public float Y;
        public float Z;
        public float W;


        public static Vector4 Zero  { get { return new Vector4(); } }
        public static Vector4 One   { get { return new Vector4(1.0f, 1.0f, 1.0f, 1.0f); } }
        public static Vector4 UnitX { get { return new Vector4(1.0f, 0.0f, 0.0f, 0.0f); } }
        public static Vector4 UnitY { get { return new Vector4(0.0f, 1.0f, 0.0f, 0.0f); } }
        public static Vector4 UnitZ { get { return new Vector4(0.0f, 0.0f, 1.0f, 0.0f); } }
        public static Vector4 UnitW { get { return new Vector4(0.0f, 0.0f, 0.0f, 1.0f); } }


        public Vector4(float x, float y, float z, float w)
        {
            X = x; 
            Y = y; 
            Z = z; 
            W = w;
        }


        public Vector4(Vector2 value, float z, float w)
        {
            X = value.X; 
            Y = value.Y; 
            Z = z; 
            W = w;
        }


        public Vector4(Vector3 value, float w)
        {
            X = value.X; 
            Y = value.Y; 
            Z = value.Z; 
            W = w;
        }


        public Vector4(float value)
        {
            X = Y = Z = W = value;
        }
        
        
        public float Length()
        {
            float ls = X * X + Y * Y + Z * Z + W * W;
            
            return (float)SM.Sqrt((double)ls);
        }


        public float LengthSquared()
        {
            return X * X + Y * Y + Z * Z + W * W;
        }


        public static float Distance(Vector4 value1, Vector4 value2)
        {
            float dx = value1.X - value2.X;
            float dy = value1.Y - value2.Y;
            float dz = value1.Z - value2.Z;
            float dw = value1.W - value2.W;
            
            float ls = dx * dx + dy * dy + dz * dz + dw * dw;

            return (float)SM.Sqrt((double)ls);
        }


        public static float DistanceSquared(Vector4 value1, Vector4 value2)
        {
            float dx = value1.X - value2.X;
            float dy = value1.Y - value2.Y;
            float dz = value1.Z - value2.Z;
            float dw = value1.W - value2.W;
            
            return dx * dx + dy * dy + dz * dz + dw * dw;
        }


        public static float Dot(Vector4 vector1, Vector4 vector2)
        {
            return vector1.X * vector2.X + 
                   vector1.Y * vector2.Y + 
                   vector1.Z * vector2.Z + 
                   vector1.W * vector2.W;
        }


        public static Vector4 Normalize(Vector4 vector)
        {
            Vector4 ans;

            float ls = vector.X * vector.X + vector.Y * vector.Y + vector.Z * vector.Z + vector.W * vector.W;
            float invNorm = 1.0f / (float)SM.Sqrt((double)ls);

            ans.X = vector.X * invNorm;
            ans.Y = vector.Y * invNorm;
            ans.Z = vector.Z * invNorm;
            ans.W = vector.W * invNorm;

            return ans;
        }


        public static Vector4 Min(Vector4 value1, Vector4 value2)
        {
            Vector4 ans;

            ans.X = (value1.X < value2.X) ? value1.X : value2.X;
            ans.Y = (value1.Y < value2.Y) ? value1.Y : value2.Y;
            ans.Z = (value1.Z < value2.Z) ? value1.Z : value2.Z;
            ans.W = (value1.W < value2.W) ? value1.W : value2.W;

            return ans;
        }


        public static Vector4 Max(Vector4 value1, Vector4 value2)
        {
            Vector4 ans;

            ans.X = (value1.X > value2.X) ? value1.X : value2.X;
            ans.Y = (value1.Y > value2.Y) ? value1.Y : value2.Y;
            ans.Z = (value1.Z > value2.Z) ? value1.Z : value2.Z;
            ans.W = (value1.W > value2.W) ? value1.W : value2.W;

            return ans;
        }


        public static Vector4 Clamp(Vector4 value1, Vector4 min, Vector4 max)
        {
            // This compare order is very important!!!
            // We must follow HLSL behavior in the case user specfied min value is bigger than max value.
            Vector4 result;

            float x = value1.X;
            x = (x> max.X) ? max.X : x;
            x = (x< min.X) ? min.X : x;

            float y = value1.Y;
            y = (y > max.Y) ? max.Y : y;
            y = (y < min.Y) ? min.Y : y;

            float z = value1.Z;
            z = (z > max.Z) ? max.Z : z;
            z = (z < min.Z) ? min.Z : z;

            float w = value1.W;
            w = (w > max.W) ? max.W : w;
            w = (w < min.W) ? min.W : w;

            result.X = x;
            result.Y = y;
            result.Z = z;
            result.W = w;

            return result;
        }


        public static Vector4 Lerp(Vector4 value1, Vector4 value2, float amount)
        {
            Vector4 ans;

            ans.X = value1.X + (value2.X - value1.X) * amount;
            ans.Y = value1.Y + (value2.Y - value1.Y) * amount;
            ans.Z = value1.Z + (value2.Z - value1.Z) * amount;
            ans.W = value1.W + (value2.W - value1.W) * amount;

            return ans;
        }


        public static Vector4 Transform(Vector2 position, Matrix4x4 matrix)
        {
            Vector4 result;

            result.X = position.X * matrix.M11 + position.Y * matrix.M21 + matrix.M41;
            result.Y = position.X * matrix.M12 + position.Y * matrix.M22 + matrix.M42;
            result.Z = position.X * matrix.M13 + position.Y * matrix.M23 + matrix.M43;
            result.W = position.X * matrix.M14 + position.Y * matrix.M24 + matrix.M44;

            return result;
        }


        public static Vector4 Transform(Vector3 position, Matrix4x4 matrix)
        {
            Vector4 result;

            result.X = position.X * matrix.M11 + position.Y * matrix.M21 + position.Z * matrix.M31 + matrix.M41;
            result.Y = position.X * matrix.M12 + position.Y * matrix.M22 + position.Z * matrix.M32 + matrix.M42;
            result.Z = position.X * matrix.M13 + position.Y * matrix.M23 + position.Z * matrix.M33 + matrix.M43;
            result.W = position.X * matrix.M14 + position.Y * matrix.M24 + position.Z * matrix.M34 + matrix.M44;

            return result;
        }


        public static Vector4 Transform(Vector4 vector, Matrix4x4 matrix)
        {
            Vector4 result;

            result.X = vector.X * matrix.M11 + vector.Y * matrix.M21 + vector.Z * matrix.M31 + vector.W * matrix.M41;
            result.Y = vector.X * matrix.M12 + vector.Y * matrix.M22 + vector.Z * matrix.M32 + vector.W * matrix.M42;
            result.Z = vector.X * matrix.M13 + vector.Y * matrix.M23 + vector.Z * matrix.M33 + vector.W * matrix.M43;
            result.W = vector.X * matrix.M14 + vector.Y * matrix.M24 + vector.Z * matrix.M34 + vector.W * matrix.M44;

            return result;
        }


        public static Vector4 Transform(Vector2 value, Quaternion rotation)
        {
            Vector4 result;

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

            result.X = value.X * ( 1.0f - yy2 - zz2 ) + value.Y * (        xy2 - wz2 );
            result.Y = value.X * (        xy2 + wz2 ) + value.Y * ( 1.0f - xx2 - zz2 );
            result.Z = value.X * (        xz2 - wy2 ) + value.Y * (        yz2 + wx2 );
            result.W = 1.0f;

            return result;
        }


        public static Vector4 Transform(Vector3 value, Quaternion rotation)
        {
            Vector4 result;

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

            result.X = value.X * ( 1.0f - yy2 - zz2 ) + value.Y * (        xy2 - wz2 ) + value.Z * (        xz2 + wy2 );
            result.Y = value.X * (        xy2 + wz2 ) + value.Y * ( 1.0f - xx2 - zz2 ) + value.Z * (        yz2 - wx2 );
            result.Z = value.X * (        xz2 - wy2 ) + value.Y * (        yz2 + wx2 ) + value.Z * ( 1.0f - xx2 - yy2 );
            result.W = 1.0f;

            return result;
        }
        
        
        public static Vector4 Transform(Vector4 value, Quaternion rotation)
        {
            Vector4 result;

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

            result.X = value.X * ( 1.0f - yy2 - zz2 ) + value.Y * (        xy2 - wz2 ) + value.Z * (        xz2 + wy2 );
            result.Y = value.X * (        xy2 + wz2 ) + value.Y * ( 1.0f - xx2 - zz2 ) + value.Z * (        yz2 - wx2 );
            result.Z = value.X * (        xz2 - wy2 ) + value.Y * (        yz2 + wx2 ) + value.Z * ( 1.0f - xx2 - yy2 );
            result.W = value.W;

            return result;
        }


        public static Vector4 Negate(Vector4 value)
        {
            Vector4 ans;

            ans.X = -value.X;
            ans.Y = -value.Y;
            ans.Z = -value.Z;
            ans.W = -value.W;

            return ans;
        }


        public static Vector4 Add(Vector4 value1, Vector4 value2)
        {
            Vector4 ans;

            ans.X = value1.X + value2.X;
            ans.Y = value1.Y + value2.Y;
            ans.Z = value1.Z + value2.Z;
            ans.W = value1.W + value2.W;

            return ans;
        }


        public static Vector4 Subtract(Vector4 value1, Vector4 value2)
        {
            Vector4 ans;

            ans.X = value1.X - value2.X;
            ans.Y = value1.Y - value2.Y;
            ans.Z = value1.Z - value2.Z;
            ans.W = value1.W - value2.W;

            return ans;
        }


        public static Vector4 Multiply(Vector4 value1, Vector4 value2)
        {
            Vector4 ans;

            ans.X = value1.X * value2.X;
            ans.Y = value1.Y * value2.Y;
            ans.Z = value1.Z * value2.Z;
            ans.W = value1.W * value2.W;

            return ans;
        }


        public static Vector4 Multiply(Vector4 value1, float value2)
        {
            Vector4 ans;

            ans.X = value1.X * value2;
            ans.Y = value1.Y * value2;
            ans.Z = value1.Z * value2;
            ans.W = value1.W * value2;

            return ans;
        }


        public static Vector4 Divide(Vector4 value1, Vector4 value2)
        {
            Vector4 ans;

            ans.X = value1.X / value2.X;
            ans.Y = value1.Y / value2.Y;
            ans.Z = value1.Z / value2.Z;
            ans.W = value1.W / value2.W;

            return ans;
        }


        public static Vector4 Divide(Vector4 value1, float value2)
        {
            Vector4 ans;

            float invDiv = 1.0f / value2;

            ans.X = value1.X * invDiv;
            ans.Y = value1.Y * invDiv;
            ans.Z = value1.Z * invDiv;
            ans.W = value1.W * invDiv;

            return ans;
        }


        public static Vector4 operator -(Vector4 value)
        {
            Vector4 ans;

            ans.X = -value.X;
            ans.Y = -value.Y;
            ans.Z = -value.Z;
            ans.W = -value.W;

            return ans;
        }


        public static Vector4 operator +(Vector4 value1, Vector4 value2)
        {
            Vector4 ans;

            ans.X = value1.X + value2.X;
            ans.Y = value1.Y + value2.Y;
            ans.Z = value1.Z + value2.Z;
            ans.W = value1.W + value2.W;

            return ans;
        }


        public static Vector4 operator -(Vector4 value1, Vector4 value2)
        {
            Vector4 ans;

            ans.X = value1.X - value2.X;
            ans.Y = value1.Y - value2.Y;
            ans.Z = value1.Z - value2.Z;
            ans.W = value1.W - value2.W;

            return ans;
        }


        public static Vector4 operator *(Vector4 value1, Vector4 value2)
        {
            Vector4 ans;

            ans.X = value1.X * value2.X;
            ans.Y = value1.Y * value2.Y;
            ans.Z = value1.Z * value2.Z;
            ans.W = value1.W * value2.W;

            return ans;
        }


        public static Vector4 operator *(Vector4 value1, float value2)
        {
            Vector4 ans;

            ans.X = value1.X * value2;
            ans.Y = value1.Y * value2;
            ans.Z = value1.Z * value2;
            ans.W = value1.W * value2;

            return ans;
        }


        public static Vector4 operator *(float value1, Vector4 value2)
        {
            Vector4 ans;

            ans.X = value2.X * value1;
            ans.Y = value2.Y * value1;
            ans.Z = value2.Z * value1;
            ans.W = value2.W * value1;

            return ans;
        }


        public static Vector4 operator /(Vector4 value1, Vector4 value2)
        {
            Vector4 ans;

            ans.X = value1.X / value2.X;
            ans.Y = value1.Y / value2.Y;
            ans.Z = value1.Z / value2.Z;
            ans.W = value1.W / value2.W;

            return ans;
        }


        public static Vector4 operator /(Vector4 value1, float value2)
        {
            Vector4 ans;

            float invDiv = 1.0f / value2;
            
            ans.X = value1.X * invDiv;
            ans.Y = value1.Y * invDiv;
            ans.Z = value1.Z * invDiv;
            ans.W = value1.W * invDiv;

            return ans;
        }


        public static bool operator ==(Vector4 value1, Vector4 value2)
        {
            return (value1.X == value2.X &&
                    value1.Y == value2.Y &&
                    value1.Z == value2.Z &&
                    value1.W == value2.W);
        }


        public static bool operator !=(Vector4 value1, Vector4 value2)
        {
            return (value1.X != value2.X ||
                    value1.Y != value2.Y ||
                    value1.Z != value2.Z ||
                    value1.W != value2.W);
        }


        public bool Equals(Vector4 other)
        {
            return X == other.X &&
                   Y == other.Y &&
                   Z == other.Z &&
                   W == other.W;
        }


        public override bool Equals(object obj)
        {
            if (obj is Vector4)
            {
                return Equals((Vector4)obj);
            }

            return false;
        }


        public override string ToString()
        {
            CultureInfo ci = CultureInfo.CurrentCulture;

            return String.Format(ci, "{{X:{0} Y:{1} Z:{2} W:{3}}}", X.ToString(ci), Y.ToString(ci), Z.ToString(ci), W.ToString(ci));
        }


        public override int GetHashCode()
        {
            return X.GetHashCode() + Y.GetHashCode() + Z.GetHashCode() + W.GetHashCode();
        }


#if INCLUDE_WINRT_CANVAS_INTEROP

        public static unsafe implicit operator Microsoft.Graphics.Canvas.Numerics.Vector4(Vector4 value)
        {
            return *(Microsoft.Graphics.Canvas.Numerics.Vector4*)&value;
        }


        public static unsafe implicit operator Vector4(Microsoft.Graphics.Canvas.Numerics.Vector4 value)
        {
            return *(Vector4*)&value;
        }

#endif
    }
}
