//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//

using System;
using System.Globalization;

using SM = System.Math;

namespace Windows.Math
{
    public struct Vector3 : IEquatable<Vector3>
    {
        public float X;
        public float Y;
        public float Z;


        public static Vector3 Zero  { get { return new Vector3(); } }
        public static Vector3 One   { get { return new Vector3(1.0f, 1.0f, 1.0f); } }
        public static Vector3 UnitX { get { return new Vector3(1.0f, 0.0f, 0.0f); } }
        public static Vector3 UnitY { get { return new Vector3(0.0f, 1.0f, 0.0f); } }
        public static Vector3 UnitZ { get { return new Vector3(0.0f, 0.0f, 1.0f); } }
        
        
        public Vector3(float x, float y, float z)
        {
            X = x; 
            Y = y; 
            Z = z;
        }


        public Vector3(Vector2 value, float z)
        {
            X = value.X; 
            Y = value.Y; 
            Z = z;
        }


        public Vector3(float value)
        {
            X = Y = Z = value;
        }
        
        
        public float Length()
        {
            float ls = X * X + Y * Y + Z * Z;
            
            return (float)SM.Sqrt((double)ls);
        }


        public float LengthSquared()
        {
            return X * X + Y * Y + Z * Z;
        }


        public static float Distance(Vector3 value1, Vector3 value2)
        {
            float dx = value1.X - value2.X;
            float dy = value1.Y - value2.Y;
            float dz = value1.Z - value2.Z;
            
            float ls = dx * dx + dy * dy + dz * dz;

            return (float)SM.Sqrt((double)ls);
        }


        public static float DistanceSquared(Vector3 value1, Vector3 value2)
        {
            float dx = value1.X - value2.X;
            float dy = value1.Y - value2.Y;
            float dz = value1.Z - value2.Z;
            
            return dx * dx + dy * dy + dz * dz;
        }


        public static float Dot(Vector3 vector1, Vector3 vector2)
        {
            return vector1.X * vector2.X + 
                   vector1.Y * vector2.Y + 
                   vector1.Z * vector2.Z;
        }


        public static Vector3 Normalize(Vector3 value)
        {
            Vector3 ans;
            
            float ls = value.X * value.X + value.Y * value.Y + value.Z * value.Z;
            float invNorm = 1.0f / (float)SM.Sqrt((double)ls);

            ans.X = value.X * invNorm;
            ans.Y = value.Y * invNorm;
            ans.Z = value.Z * invNorm;

            return ans;
        }


        public static Vector3 Cross(Vector3 vector1, Vector3 vector2)
        {
            Vector3 ans;

            ans.X = vector1.Y * vector2.Z - vector1.Z * vector2.Y;
            ans.Y = vector1.Z * vector2.X - vector1.X * vector2.Z;
            ans.Z = vector1.X * vector2.Y - vector1.Y * vector2.X;

            return ans;
        }


        public static Vector3 Reflect(Vector3 vector, Vector3 normal)
        {
            Vector3 ans;

            float dot = vector.X * normal.X + vector.Y * normal.Y + vector.Z * normal.Z;

            ans.X = vector.X - 2.0f * dot * normal.X;
            ans.Y = vector.Y - 2.0f * dot * normal.Y;
            ans.Z = vector.Z - 2.0f * dot * normal.Z;

            return ans;
        }


        public static Vector3 Min(Vector3 value1, Vector3 value2)
        {
            Vector3 ans;

            ans.X = (value1.X < value2.X) ? value1.X : value2.X;
            ans.Y = (value1.Y < value2.Y) ? value1.Y : value2.Y;
            ans.Z = (value1.Z < value2.Z) ? value1.Z : value2.Z;

            return ans;
        }


        public static Vector3 Max(Vector3 value1, Vector3 value2)
        {
            Vector3 ans;

            ans.X = (value1.X > value2.X) ? value1.X : value2.X;
            ans.Y = (value1.Y > value2.Y) ? value1.Y : value2.Y;
            ans.Z = (value1.Z > value2.Z) ? value1.Z : value2.Z;

            return ans;
        }


        public static Vector3 Clamp(Vector3 value1, Vector3 min, Vector3 max)
        {
            // This compare order is very important!!!
            // We must follow HLSL behavior in the case user specfied min value is bigger than max value.
            Vector3 result;

            float x = value1.X;
            x = (x > max.X) ? max.X : x;
            x = (x < min.X) ? min.X : x;

            float y = value1.Y;
            y = (y > max.Y) ? max.Y : y;
            y = (y < min.Y) ? min.Y : y;

            float z = value1.Z;
            z = (z > max.Z) ? max.Z : z;
            z = (z < min.Z) ? min.Z : z;

            result.X = x;
            result.Y = y;
            result.Z = z;

            return result;
        }


        public static Vector3 Lerp(Vector3 value1, Vector3 value2, float amount)
        {
            Vector3 ans;

            ans.X = value1.X + (value2.X - value1.X) * amount;
            ans.Y = value1.Y + (value2.Y - value1.Y) * amount;
            ans.Z = value1.Z + (value2.Z - value1.Z) * amount;

            return ans;
        }


        public static Vector3 Transform(Vector3 position, Matrix4x4 matrix)
        {
            Vector3 result;

            result.X = position.X * matrix.M11 + position.Y * matrix.M21 + position.Z * matrix.M31 + matrix.M41;
            result.Y = position.X * matrix.M12 + position.Y * matrix.M22 + position.Z * matrix.M32 + matrix.M42;
            result.Z = position.X * matrix.M13 + position.Y * matrix.M23 + position.Z * matrix.M33 + matrix.M43;

            return result;
        }


        public static Vector3 TransformNormal(Vector3 normal, Matrix4x4 matrix)
        {
            Vector3 result;

            result.X = normal.X * matrix.M11 + normal.Y * matrix.M21 + normal.Z * matrix.M31;
            result.Y = normal.X * matrix.M12 + normal.Y * matrix.M22 + normal.Z * matrix.M32;
            result.Z = normal.X * matrix.M13 + normal.Y * matrix.M23 + normal.Z * matrix.M33;

            return result;
        }

      
        public static Vector3 Transform(Vector3 value, Quaternion rotation)
        {
            Vector3 result;

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

            result.X = value.X * (1.0f - yy2 - zz2) + value.Y * (xy2 - wz2) + value.Z * (xz2 + wy2);
            result.Y = value.X * (xy2 + wz2) + value.Y * (1.0f - xx2 - zz2) + value.Z * (yz2 - wx2);
            result.Z = value.X * (xz2 - wy2) + value.Y * (yz2 + wx2) + value.Z * (1.0f - xx2 - yy2);

            return result;
        }


        public static Vector3 Negate(Vector3 value)
        {
            Vector3 ans;

            ans.X = -value.X;
            ans.Y = -value.Y;
            ans.Z = -value.Z;

            return ans;
        }


        public static Vector3 Add(Vector3 value1, Vector3 value2)
        {
            Vector3 ans;

            ans.X = value1.X + value2.X;
            ans.Y = value1.Y + value2.Y;
            ans.Z = value1.Z + value2.Z;

            return ans;
        }


        public static Vector3 Subtract(Vector3 value1, Vector3 value2)
        {
            Vector3 ans;

            ans.X = value1.X - value2.X;
            ans.Y = value1.Y - value2.Y;
            ans.Z = value1.Z - value2.Z;

            return ans;
        }


        public static Vector3 Multiply(Vector3 value1, Vector3 value2)
        {
            Vector3 ans;

            ans.X = value1.X * value2.X;
            ans.Y = value1.Y * value2.Y;
            ans.Z = value1.Z * value2.Z;

            return ans;
        }


        public static Vector3 Multiply(Vector3 value1, float scaleFactor)
        {
            Vector3 ans;

            ans.X = value1.X * scaleFactor;
            ans.Y = value1.Y * scaleFactor;
            ans.Z = value1.Z * scaleFactor;

            return ans;
        }


        public static Vector3 Divide(Vector3 value1, Vector3 value2)
        {
            Vector3 ans;

            ans.X = value1.X / value2.X;
            ans.Y = value1.Y / value2.Y;
            ans.Z = value1.Z / value2.Z;

            return ans;
        }


        public static Vector3 Divide(Vector3 value1, float value2)
        {
            Vector3 ans;

            float invDiv = 1.0f / value2;

            ans.X = value1.X * invDiv;
            ans.Y = value1.Y * invDiv;
            ans.Z = value1.Z * invDiv;

            return ans;
        }


        public static Vector3 operator -(Vector3 value)
        {
            Vector3 ans;

            ans.X = -value.X;
            ans.Y = -value.Y;
            ans.Z = -value.Z;

            return ans;
        }


        public static Vector3 operator +(Vector3 value1, Vector3 value2)
        {
            Vector3 ans;

            ans.X = value1.X + value2.X;
            ans.Y = value1.Y + value2.Y;
            ans.Z = value1.Z + value2.Z;

            return ans;
        }


        public static Vector3 operator -(Vector3 value1, Vector3 value2)
        {
            Vector3 ans;

            ans.X = value1.X - value2.X;
            ans.Y = value1.Y - value2.Y;
            ans.Z = value1.Z - value2.Z;

            return ans;
        }


        public static Vector3 operator *(Vector3 value1, Vector3 value2)
        {
            Vector3 ans;

            ans.X = value1.X * value2.X;
            ans.Y = value1.Y * value2.Y;
            ans.Z = value1.Z * value2.Z;

            return ans;
        }


        public static Vector3 operator *(Vector3 value, float scaleFactor)
        {
            Vector3 ans;

            ans.X = value.X * scaleFactor;
            ans.Y = value.Y * scaleFactor;
            ans.Z = value.Z * scaleFactor;

            return ans;
        }


        public static Vector3 operator *(float scaleFactor, Vector3 value)
        {
            Vector3 ans;

            ans.X = value.X * scaleFactor;
            ans.Y = value.Y * scaleFactor;
            ans.Z = value.Z * scaleFactor;

            return ans;
        }


        public static Vector3 operator /(Vector3 value1, Vector3 value2)
        {
            Vector3 ans;

            ans.X = value1.X / value2.X;
            ans.Y = value1.Y / value2.Y;
            ans.Z = value1.Z / value2.Z;

            return ans;
        }


        public static Vector3 operator /(Vector3 value, float divider)
        {
            Vector3 ans;

            float invDiv = 1.0f / divider;
            
            ans.X = value.X * invDiv;
            ans.Y = value.Y * invDiv;
            ans.Z = value.Z * invDiv;

            return ans;
        }


        public static bool operator ==(Vector3 value1, Vector3 value2)
        {
            return (value1.X == value2.X &&
                    value1.Y == value2.Y &&
                    value1.Z == value2.Z);
        }


        public static bool operator !=(Vector3 value1, Vector3 value2)
        {
            return (value1.X != value2.X ||
                    value1.Y != value2.Y ||
                    value1.Z != value2.Z);
        }


        public bool Equals(Vector3 other)
        {
            return X == other.X &&
                   Y == other.Y &&
                   Z == other.Z;
        }


        public override bool Equals(object obj)
        {
            if (obj is Vector3)
            {
                return Equals((Vector3)obj);
            }

            return false;
        }


        public override string ToString()
        {
            CultureInfo ci = CultureInfo.CurrentCulture;

            return String.Format(ci, "{{X:{0} Y:{1} Z:{2}}}", X.ToString(ci), Y.ToString(ci), Z.ToString(ci));
        }


        public override int GetHashCode()
        {
            return X.GetHashCode() + Y.GetHashCode() + Z.GetHashCode();
        }
    }
}
