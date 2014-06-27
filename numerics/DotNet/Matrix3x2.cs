//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//

using System;
using System.Globalization;

using SM = System.Math;

namespace System.Numerics
{
    public struct Matrix3x2 : IEquatable<Matrix3x2>
    {
        public float M11;
        public float M12;

        public float M21;
        public float M22;

        public float M31;
        public float M32;


        private static Matrix3x2 _identity = new Matrix3x2
        (
            1f, 0f,
            0f, 1f,
            0f, 0f
        );
 

        public static Matrix3x2 Identity
        { 
            get { return _identity; }
        }


        public bool IsIdentity
        {
            get
            {
                return M11 == 1f && M22 == 1f && // Check diagonal element first for early out.
                                    M12 == 0f &&
                       M21 == 0f              &&
                       M31 == 0f && M32 == 0f;
            }
        }


        public Vector2 Translation
        {
            get
            {
                Vector2 ans;
                
                ans.X = M31; 
                ans.Y = M32;
                
                return ans;
            }
            
            set
            {
                M31 = value.X; 
                M32 = value.Y;
            }
        }


        public Matrix3x2(float m11, float m12,
                         float m21, float m22,
                         float m31, float m32)
        {
            M11 = m11; M12 = m12;
            M21 = m21; M22 = m22;
            M31 = m31; M32 = m32;
        }
        
        
        public static Matrix3x2 CreateTranslation(Vector2 position)
        {
            Matrix3x2 result;

            result.M11 = 1.0f; result.M12 = 0.0f;
            result.M21 = 0.0f; result.M22 = 1.0f;

            result.M31 = position.X;
            result.M32 = position.Y;

            return result;
        }


        public static Matrix3x2 CreateTranslation(float xPosition, float yPosition)
        {
            Matrix3x2 result;

            result.M11 = 1.0f; result.M12 = 0.0f;
            result.M21 = 0.0f; result.M22 = 1.0f;

            result.M31 = xPosition;
            result.M32 = yPosition;

            return result;
        }


        public static Matrix3x2 CreateScale(float xScale, float yScale)
        {
            Matrix3x2 result;

            result.M11 = xScale; result.M12 = 0.0f;
            result.M21 = 0.0f;   result.M22 = yScale;
            result.M31 = 0.0f;   result.M32 = 0.0f;

            return result;
        }


        public static Matrix3x2 CreateScale(float xScale, float yScale, Vector2 centerPoint)
        {
            Matrix3x2 result;

            float tx = centerPoint.X * (1 - xScale);
            float ty = centerPoint.Y * (1 - yScale);

            result.M11 = xScale; result.M12 = 0.0f;
            result.M21 = 0.0f;   result.M22 = yScale;
            result.M31 = tx;     result.M32 = ty;

            return result;
        }


        public static Matrix3x2 CreateScale(Vector2 scales)
        {
            Matrix3x2 result;

            result.M11 = scales.X; result.M12 = 0.0f;
            result.M21 = 0.0f;     result.M22 = scales.Y;
            result.M31 = 0.0f;     result.M32 = 0.0f;
            
            return result;
        }


        public static Matrix3x2 CreateScale(Vector2 scales, Vector2 centerPoint)
        {
            Matrix3x2 result;

            float tx = centerPoint.X * (1 - scales.X);
            float ty = centerPoint.Y * (1 - scales.Y);

            result.M11 = scales.X; result.M12 = 0.0f;
            result.M21 = 0.0f;     result.M22 = scales.Y;
            result.M31 = tx;       result.M32 = ty;

            return result;
        }
        
            
        public static Matrix3x2 CreateScale(float scale)
        {
            Matrix3x2 result;

            result.M11 = scale; result.M12 = 0.0f;
            result.M21 = 0.0f;  result.M22 = scale;
            result.M31 = 0.0f;  result.M32 = 0.0f;

            return result;
        }
        
        
        public static Matrix3x2 CreateScale(float scale, Vector2 centerPoint)
        {
            Matrix3x2 result;

            float tx = centerPoint.X * (1 - scale);
            float ty = centerPoint.Y * (1 - scale);

            result.M11 = scale; result.M12 = 0.0f;
            result.M21 = 0.0f;  result.M22 = scale;
            result.M31 = tx;    result.M32 = ty;

            return result;
        }


        public static Matrix3x2 CreateSkew(float radiansX, float radiansY)
        {
            Matrix3x2 result;

            float xTan = (float)SM.Tan(radiansX);
            float yTan = (float)SM.Tan(radiansY);

            result.M11 = 1.0f; result.M12 = yTan;
            result.M21 = xTan; result.M22 = 1.0f;
            result.M31 = 0.0f; result.M32 = 0.0f;

            return result;
        }


        public static Matrix3x2 CreateSkew(float radiansX, float radiansY, Vector2 centerPoint)
        {
            Matrix3x2 result;

            float xTan = (float)SM.Tan(radiansX);
            float yTan = (float)SM.Tan(radiansY);

            float tx = -centerPoint.Y * xTan;
            float ty = -centerPoint.X * yTan;

            result.M11 = 1.0f; result.M12 = yTan;
            result.M21 = xTan; result.M22 = 1.0f;
            result.M31 = tx;   result.M32 = ty;

            return result;
        }


        public static Matrix3x2 CreateRotation(float radians)
        {
            Matrix3x2 result;

            radians = (float)SM.IEEERemainder(radians, SM.PI * 2);

            float c, s;

            const float epsilon = 0.001f * (float)SM.PI / 180f;     // 0.1% of a degree

            if (radians > -epsilon && radians < epsilon)
            {
                // Exact case for zero rotation.
                c = 1;
                s = 0;
            }
            else if (radians > SM.PI / 2 - epsilon && radians < SM.PI / 2 + epsilon)
            {
                // Exact case for 90 degree rotation.
                c = 0;
                s = 1;
            }
            else if (radians < -SM.PI + epsilon || radians > SM.PI - epsilon)
            {
                // Exact case for 180 degree rotation.
                c = -1;
                s = 0;
            }
            else if (radians > -SM.PI / 2 - epsilon && radians < -SM.PI / 2 + epsilon)
            {
                // Exact case for 270 degree rotation.
                c = 0;
                s = -1;
            }
            else
            {
                // Arbitrary rotation.
                c = (float)SM.Cos(radians);
                s = (float)SM.Sin(radians);
            }

            // [  c  s ]
            // [ -s  c ]
            // [  0  0 ]
            result.M11 =    c; result.M12 =    s;
            result.M21 =   -s; result.M22 =    c;
            result.M31 = 0.0f; result.M32 = 0.0f;

            return result;
        }


        public static Matrix3x2 CreateRotation(float radians, Vector2 centerPoint)
        {
            Matrix3x2 result;

            radians = (float)SM.IEEERemainder(radians, SM.PI * 2);

            float c, s;

            const float epsilon = 0.001f * (float)SM.PI / 180f;     // 0.1% of a degree

            if (radians > -epsilon && radians < epsilon)
            {
                // Exact case for zero rotation.
                c = 1;
                s = 0;
            }
            else if (radians > SM.PI / 2 - epsilon && radians < SM.PI / 2 + epsilon)
            {
                // Exact case for 90 degree rotation.
                c = 0;
                s = 1;
            }
            else if (radians < -SM.PI + epsilon || radians > SM.PI - epsilon)
            {
                // Exact case for 180 degree rotation.
                c = -1;
                s = 0;
            }
            else if (radians > -SM.PI / 2 - epsilon && radians < -SM.PI / 2 + epsilon)
            {
                // Exact case for 270 degree rotation.
                c = 0;
                s = -1;
            }
            else
            {
                // Arbitrary rotation.
                c = (float)SM.Cos(radians);
                s = (float)SM.Sin(radians);
            }

            float x = centerPoint.X * (1 - c) + centerPoint.Y * s;
            float y = centerPoint.Y * (1 - c) - centerPoint.X * s;

            // [  c  s ]
            // [ -s  c ]
            // [  x  y ]
            result.M11 =  c; result.M12 = s;
            result.M21 = -s; result.M22 = c;
            result.M31 =  x; result.M32 = y;

            return result;
        }


        public float Determinant()
        {
            // There isn't actually any such thing as a determinant for a non-square matrix,
            // but this 3x2 type is really just an optimization of a 3x3 where we happen to
            // know the rightmost column is always (0, 0, 1). So we expand to 3x3 format:
            //
            //  [ M11, M12, 0 ]
            //  [ M21, M22, 0 ]
            //  [ M31, M32, 1 ]
            //
            // Sum the diagnonal products:
            //  (M11 * M22 * 1) + (M12 * 0 * M31) + (0 * M21 * M32)
            //
            // Subtract the opposite diagonal products:
            //  (M31 * M22 * 0) + (M32 * 0 * M11) + (1 * M21 * M12)
            //
            // Collapse out the constants and oh look, this is just a 2x2 determinant!

            return (M11 * M22) - (M21 * M12);
        }


        public static bool Invert(Matrix3x2 matrix, out Matrix3x2 result)
        {
            float det = (matrix.M11 * matrix.M22) - (matrix.M21 * matrix.M12);

            if (SM.Abs(det) < float.Epsilon)
            {
                result = new Matrix3x2(float.NaN, float.NaN, float.NaN, float.NaN, float.NaN, float.NaN);
                return false;
            }

            float invDet = 1.0f / det;

            result.M11 = matrix.M22 * invDet;
            result.M12 = -matrix.M12 * invDet;
            result.M21 = -matrix.M21 * invDet;
            result.M22 = matrix.M11 * invDet;
            result.M31 = (matrix.M21 * matrix.M32 - matrix.M31 * matrix.M22) * invDet;
            result.M32 = (matrix.M31 * matrix.M12 - matrix.M11 * matrix.M32) * invDet;

            return true;
        }

        
        public static Matrix3x2 Lerp(Matrix3x2 matrix1, Matrix3x2 matrix2, float amount)
        {
            Matrix3x2 result;
            
            // First row
            result.M11 = matrix1.M11 + (matrix2.M11 - matrix1.M11) * amount;
            result.M12 = matrix1.M12 + (matrix2.M12 - matrix1.M12) * amount;
            
            // Second row
            result.M21 = matrix1.M21 + (matrix2.M21 - matrix1.M21) * amount;
            result.M22 = matrix1.M22 + (matrix2.M22 - matrix1.M22) * amount;
            
            // Third row
            result.M31 = matrix1.M31 + (matrix2.M31 - matrix1.M31) * amount;
            result.M32 = matrix1.M32 + (matrix2.M32 - matrix1.M32) * amount;

            return result;
        }
        
        
        public static Matrix3x2 Negate(Matrix3x2 value)
        {
            Matrix3x2 result;

            result.M11 = -value.M11; result.M12 = -value.M12;
            result.M21 = -value.M21; result.M22 = -value.M22;
            result.M31 = -value.M31; result.M32 = -value.M32;

            return result;
        }


        public static Matrix3x2 Add(Matrix3x2 value1, Matrix3x2 value2)
        {
            Matrix3x2 result;

            result.M11 = value1.M11 + value2.M11; result.M12 = value1.M12 + value2.M12;
            result.M21 = value1.M21 + value2.M21; result.M22 = value1.M22 + value2.M22;
            result.M31 = value1.M31 + value2.M31; result.M32 = value1.M32 + value2.M32;

            return result;
        }


        public static Matrix3x2 Subtract(Matrix3x2 value1, Matrix3x2 value2)
        {
            Matrix3x2 result;

            result.M11 = value1.M11 - value2.M11; result.M12 = value1.M12 - value2.M12;
            result.M21 = value1.M21 - value2.M21; result.M22 = value1.M22 - value2.M22;
            result.M31 = value1.M31 - value2.M31; result.M32 = value1.M32 - value2.M32;

            return result;
        }


        public static Matrix3x2 Multiply(Matrix3x2 value1, Matrix3x2 value2)
        {
            Matrix3x2 result;

            // First row
            result.M11 = value1.M11 * value2.M11 + value1.M12 * value2.M21;
            result.M12 = value1.M11 * value2.M12 + value1.M12 * value2.M22;
            
            // Second row
            result.M21 = value1.M21 * value2.M11 + value1.M22 * value2.M21;
            result.M22 = value1.M21 * value2.M12 + value1.M22 * value2.M22;
            
            // Third row
            result.M31 = value1.M31 * value2.M11 + value1.M32 * value2.M21 + value2.M31;
            result.M32 = value1.M31 * value2.M12 + value1.M32 * value2.M22 + value2.M32;

            return result;
        }
        
        
        public static Matrix3x2 Multiply(Matrix3x2 value1, float value2)
        {
            Matrix3x2 result;

            result.M11 = value1.M11 * value2; result.M12 = value1.M12 * value2;
            result.M21 = value1.M21 * value2; result.M22 = value1.M22 * value2;
            result.M31 = value1.M31 * value2; result.M32 = value1.M32 * value2;

            return result;
        }


        public static Matrix3x2 operator -(Matrix3x2 value)
        {
            Matrix3x2 m;

            m.M11 = -value.M11; m.M12 = -value.M12;
            m.M21 = -value.M21; m.M22 = -value.M22;
            m.M31 = -value.M31; m.M32 = -value.M32;

            return m;
        }


        public static Matrix3x2 operator +(Matrix3x2 value1, Matrix3x2 value2)
        {
            Matrix3x2 m;

            m.M11 = value1.M11 + value2.M11; m.M12 = value1.M12 + value2.M12;
            m.M21 = value1.M21 + value2.M21; m.M22 = value1.M22 + value2.M22;
            m.M31 = value1.M31 + value2.M31; m.M32 = value1.M32 + value2.M32;

            return m;
        }


        public static Matrix3x2 operator -(Matrix3x2 value1, Matrix3x2 value2)
        {
            Matrix3x2 m;

            m.M11 = value1.M11 - value2.M11; m.M12 = value1.M12 - value2.M12;
            m.M21 = value1.M21 - value2.M21; m.M22 = value1.M22 - value2.M22;
            m.M31 = value1.M31 - value2.M31; m.M32 = value1.M32 - value2.M32;

            return m;
        }


        public static Matrix3x2 operator *(Matrix3x2 value1, Matrix3x2 value2)
        {
            Matrix3x2 m;

            // First row
            m.M11 = value1.M11 * value2.M11 + value1.M12 * value2.M21;
            m.M12 = value1.M11 * value2.M12 + value1.M12 * value2.M22;
            
            // Second row
            m.M21 = value1.M21 * value2.M11 + value1.M22 * value2.M21;
            m.M22 = value1.M21 * value2.M12 + value1.M22 * value2.M22;
            
            // Third row
            m.M31 = value1.M31 * value2.M11 + value1.M32 * value2.M21 + value2.M31;
            m.M32 = value1.M31 * value2.M12 + value1.M32 * value2.M22 + value2.M32;

            return m;
        }


        public static Matrix3x2 operator *(Matrix3x2 value1, float value2)
        {
            Matrix3x2 m;

            m.M11 = value1.M11 * value2; m.M12 = value1.M12 * value2;
            m.M21 = value1.M21 * value2; m.M22 = value1.M22 * value2;
            m.M31 = value1.M31 * value2; m.M32 = value1.M32 * value2;

            return m;
        }


        public static bool operator ==(Matrix3x2 value1, Matrix3x2 value2)
        {
            return (value1.M11 == value2.M11 && value1.M22 == value2.M22 && // Check diagonal element first for early out.
                                                value1.M12 == value2.M12 &&
                    value1.M21 == value2.M21 &&
                    value1.M31 == value2.M31 && value1.M32 == value2.M32);
        }


        public static bool operator !=(Matrix3x2 value1, Matrix3x2 value2)
        {
            return (value1.M11 != value2.M11 || value1.M12 != value2.M12 ||
                    value1.M21 != value2.M21 || value1.M22 != value2.M22 ||
                    value1.M31 != value2.M31 || value1.M32 != value2.M32);
        }


        public bool Equals(Matrix3x2 other)
        {
            return (M11 == other.M11 && M22 == other.M22 && // Check diagonal element first for early out.
                                        M12 == other.M12 &&
                    M21 == other.M21 &&
                    M31 == other.M31 && M32 == other.M32);
        }


        public override bool Equals(object obj)
        {
            if (obj is Matrix3x2)
            {
                return Equals((Matrix3x2)obj);
            }

            return false;
        }


        public override string ToString()
        {
            CultureInfo ci = CultureInfo.CurrentCulture;

            return String.Format(ci, "{{ {{M11:{0} M12:{1}}} {{M21:{2} M22:{3}}} {{M31:{4} M32:{5}}} }}",
                                 M11.ToString(ci), M12.ToString(ci),
                                 M21.ToString(ci), M22.ToString(ci),
                                 M31.ToString(ci), M32.ToString(ci));
        }

        
        public override int GetHashCode()
        {
            return M11.GetHashCode() + M12.GetHashCode() +
                   M21.GetHashCode() + M22.GetHashCode() +
                   M31.GetHashCode() + M32.GetHashCode();
        }


#if INCLUDE_WINRT_CANVAS_INTEROP

        public static unsafe implicit operator Microsoft.Graphics.Canvas.Numerics.Matrix3x2(Matrix3x2 value)
        {
            return *(Microsoft.Graphics.Canvas.Numerics.Matrix3x2*)&value;
        }


        public static unsafe implicit operator Matrix3x2(Microsoft.Graphics.Canvas.Numerics.Matrix3x2 value)
        {
            return *(Matrix3x2*)&value;
        }

#endif
    }
}
