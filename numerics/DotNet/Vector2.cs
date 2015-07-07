// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

using System.Globalization;

using SM = System.Math;

namespace System.Numerics
{
    public struct Vector2 : IEquatable<Vector2>
    {
        public float X;
        public float Y;


        public static Vector2 Zero  { get { return new Vector2(); } }
        public static Vector2 One   { get { return new Vector2(1.0f, 1.0f); } }
        public static Vector2 UnitX { get { return new Vector2(1.0f, 0.0f); } }
        public static Vector2 UnitY { get { return new Vector2(0.0f, 1.0f); } }


        public Vector2(float x, float y)
        {
            X = x; 
            Y = y;
        }


        public Vector2(float value)
        {
            X = Y = value;
        }


        public float Length()
        {
            float ls = X * X + Y * Y;
            
            return (float)SM.Sqrt((double)ls);
        }


        public float LengthSquared()
        {
            return X * X + Y * Y;
        }


        public static float Distance(Vector2 value1, Vector2 value2)
        {
            float dx = value1.X - value2.X;
            float dy = value1.Y - value2.Y;
            
            float ls = dx * dx + dy * dy;

            return (float)SM.Sqrt((double)ls);
        }


        public static float DistanceSquared(Vector2 value1, Vector2 value2)
        {
            float dx = value1.X - value2.X;
            float dy = value1.Y - value2.Y;
            
            return dx * dx + dy * dy;
        }


        public static float Dot(Vector2 value1, Vector2 value2)
        {
            return value1.X * value2.X + 
                   value1.Y * value2.Y;
        }


        public static Vector2 Normalize(Vector2 value)
        {
            Vector2 ans;
            
            float ls = value.X * value.X + value.Y * value.Y;
            float invNorm = 1.0f / (float)SM.Sqrt((double)ls);

            ans.X = value.X * invNorm;
            ans.Y = value.Y * invNorm;

            return ans;
        }


        public static Vector2 Reflect(Vector2 vector, Vector2 normal)
        {
            Vector2 ans;

            float dot = vector.X * normal.X + vector.Y * normal.Y;

            ans.X = vector.X - 2.0f * dot * normal.X;
            ans.Y = vector.Y - 2.0f * dot * normal.Y;

            return ans;
        }


        public static Vector2 Min(Vector2 value1, Vector2 value2)
        {
            Vector2 ans;
            
            ans.X = (value1.X < value2.X) ? value1.X : value2.X;
            ans.Y = (value1.Y < value2.Y) ? value1.Y : value2.Y;
            
            return ans;
        }


        public static Vector2 Max(Vector2 value1, Vector2 value2)
        {
            Vector2 ans;
            
            ans.X = (value1.X > value2.X) ? value1.X : value2.X;
            ans.Y = (value1.Y > value2.Y) ? value1.Y : value2.Y;
            
            return ans;
        }


        public static Vector2 Clamp(Vector2 value1, Vector2 min, Vector2 max)
        {
            Vector2 result;

            // This compare order is very important!!!
            // We must follow HLSL behavior in the case user specfied min value is bigger than max value.
            float x = value1.X;
            x = (x > max.X) ? max.X : x;
            x = (x < min.X) ? min.X : x;

            float y = value1.Y;
            y = (y > max.Y) ? max.Y : y;
            y = (y < min.Y) ? min.Y : y;

            result.X = x;
            result.Y = y;

            return result;
        }


        public static Vector2 Lerp(Vector2 value1, Vector2 value2, float amount)
        {
            Vector2 ans;

            ans.X = value1.X + (value2.X - value1.X) * amount;
            ans.Y = value1.Y + (value2.Y - value1.Y) * amount;

            return ans;
        }


        public static Vector2 Transform(Vector2 position, Matrix3x2 matrix)
        {
            Vector2 result;

            result.X = position.X * matrix.M11 + position.Y * matrix.M21 + matrix.M31;
            result.Y = position.X * matrix.M12 + position.Y * matrix.M22 + matrix.M32;

            return result;
        }


        public static Vector2 Transform(Vector2 position, Matrix4x4 matrix)
        {
            Vector2 result;

            result.X = position.X * matrix.M11 + position.Y * matrix.M21 + matrix.M41;
            result.Y = position.X * matrix.M12 + position.Y * matrix.M22 + matrix.M42;

            return result;
        }


        public static Vector2 TransformNormal(Vector2 normal, Matrix3x2 matrix)
        {
            Vector2 result;

            result.X = normal.X * matrix.M11 + normal.Y * matrix.M21;
            result.Y = normal.X * matrix.M12 + normal.Y * matrix.M22;

            return result;
        }


        public static Vector2 TransformNormal(Vector2 normal, Matrix4x4 matrix)
        {
            Vector2 result;

            result.X = normal.X * matrix.M11 + normal.Y * matrix.M21;
            result.Y = normal.X * matrix.M12 + normal.Y * matrix.M22;

            return result;
        }


        public static Vector2 Transform(Vector2 value, Quaternion rotation)
        {
            Vector2 result;

            float x2 = rotation.X + rotation.X;
            float y2 = rotation.Y + rotation.Y;
            float z2 = rotation.Z + rotation.Z;

            float wz2 = rotation.W * z2;
            float xx2 = rotation.X * x2;
            float xy2 = rotation.X * y2;
            float yy2 = rotation.Y * y2;
            float zz2 = rotation.Z * z2;

            result.X = value.X * (1.0f - yy2 - zz2) + value.Y * (xy2 - wz2);
            result.Y = value.X * (xy2 + wz2) + value.Y * (1.0f - xx2 - zz2);

            return result;
        }


        public static Vector2 Negate(Vector2 value)
        {
            Vector2 ans;

            ans.X = -value.X;
            ans.Y = -value.Y;

            return ans;
        }


        public static Vector2 Add(Vector2 value1, Vector2 value2)
        {
            Vector2 ans;

            ans.X = value1.X + value2.X;
            ans.Y = value1.Y + value2.Y;

            return ans;
        }


        public static Vector2 Subtract(Vector2 value1, Vector2 value2)
        {
            Vector2 ans;

            ans.X = value1.X - value2.X;
            ans.Y = value1.Y - value2.Y;

            return ans;
        }


        public static Vector2 Multiply(Vector2 value1, Vector2 value2)
        {
            Vector2 ans;

            ans.X = value1.X * value2.X;
            ans.Y = value1.Y * value2.Y;

            return ans;
        }


        public static Vector2 Multiply(Vector2 value1, float value2)
        {
            Vector2 ans;

            ans.X = value1.X * value2;
            ans.Y = value1.Y * value2;

            return ans;
        }


        public static Vector2 Divide(Vector2 value1, Vector2 value2)
        {
            Vector2 ans;

            ans.X = value1.X / value2.X;
            ans.Y = value1.Y / value2.Y;

            return ans;
        }


        public static Vector2 Divide(Vector2 value1, float value2)
        {
            Vector2 ans;

            float invDiv = 1.0f / value2;

            ans.X = value1.X * invDiv;
            ans.Y = value1.Y * invDiv;

            return ans;
        }


        public static Vector2 operator -(Vector2 value)
        {
            Vector2 ans;

            ans.X = -value.X;
            ans.Y = -value.Y;

            return ans;
        }


        public static Vector2 operator +(Vector2 value1, Vector2 value2)
        {
            Vector2 ans;

            ans.X = value1.X + value2.X;
            ans.Y = value1.Y + value2.Y;

            return ans;
        }


        public static Vector2 operator -(Vector2 value1, Vector2 value2)
        {
            Vector2 ans;

            ans.X = value1.X - value2.X;
            ans.Y = value1.Y - value2.Y;

            return ans;
        }


        public static Vector2 operator *(Vector2 value1, Vector2 value2)
        {
            Vector2 ans;

            ans.X = value1.X * value2.X;
            ans.Y = value1.Y * value2.Y;

            return ans;
        }


        public static Vector2 operator *(Vector2 value1, float value2)
        {
            Vector2 ans;

            ans.X = value1.X * value2;
            ans.Y = value1.Y * value2;

            return ans;
        }


        public static Vector2 operator *(float value1, Vector2 value2)
        {
            Vector2 ans;

            ans.X = value2.X * value1;
            ans.Y = value2.Y * value1;

            return ans;
        }


        public static Vector2 operator /(Vector2 value1, Vector2 value2)
        {
            Vector2 ans;

            ans.X = value1.X / value2.X;
            ans.Y = value1.Y / value2.Y;

            return ans;
        }


        public static Vector2 operator /(Vector2 value1, float value2)
        {
            Vector2 ans;

            float invDiv = 1.0f / value2;
            
            ans.X = value1.X * invDiv;
            ans.Y = value1.Y * invDiv;

            return ans;
        }


        public static bool operator ==(Vector2 value1, Vector2 value2)
        {
            return (value1.X == value2.X &&
                    value1.Y == value2.Y);
        }


        public static bool operator !=(Vector2 value1, Vector2 value2)
        {
            return (value1.X != value2.X ||
                    value1.Y != value2.Y);
        }


        public bool Equals(Vector2 other)
        {
            return X == other.X &&
                   Y == other.Y;
        }


        public override bool Equals(object obj)
        {
            if (obj is Vector2)
            {
                return Equals((Vector2)obj);
            }

            return false;
        }


        public override string ToString()
        {
            CultureInfo ci = CultureInfo.CurrentCulture;

            return String.Format(ci, "{{X:{0} Y:{1}}}", X.ToString(ci), Y.ToString(ci));
        }


        public override int GetHashCode()
        {
            return X.GetHashCode() + Y.GetHashCode();
        }

        
#if INCLUDE_WINRT_CANVAS_INTEROP

        public static unsafe implicit operator Microsoft.Graphics.Canvas.Numerics.Vector2(Vector2 value)
        {
            return *(Microsoft.Graphics.Canvas.Numerics.Vector2*)&value;
        }


        public static unsafe implicit operator Vector2(Microsoft.Graphics.Canvas.Numerics.Vector2 value)
        {
            return *(Vector2*)&value;
        }

#endif
    }
}
