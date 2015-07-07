// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

using System.Globalization;

using SM = System.Math;

namespace System.Numerics
{
    public struct Matrix4x4 : IEquatable<Matrix4x4>
    {
        public float M11;
        public float M12;
        public float M13;
        public float M14;

        public float M21;
        public float M22;
        public float M23;
        public float M24;

        public float M31;
        public float M32;
        public float M33;
        public float M34;

        public float M41;
        public float M42;
        public float M43;
        public float M44;


        private static Matrix4x4 _identity = new Matrix4x4
        (
            1f, 0f, 0f, 0f,
            0f, 1f, 0f, 0f,
            0f, 0f, 1f, 0f,
            0f, 0f, 0f, 1f
        );
 

        public static Matrix4x4 Identity
        { 
            get { return _identity; }
        }


        public bool IsIdentity
        {
            get
            {
                return M11 == 1f && M22 == 1f && M33 == 1f && M44 == 1f && // Check diagonal element first for early out.
                                    M12 == 0f && M13 == 0f && M14 == 0f &&
                       M21 == 0f              && M23 == 0f && M24 == 0f &&
                       M31 == 0f && M32 == 0f              && M34 == 0f &&
                       M41 == 0f && M42 == 0f && M43 == 0f;
            }
        }


        public Vector3 Translation
        {
            get
            {
                Vector3 ans;
                
                ans.X = M41; 
                ans.Y = M42; 
                ans.Z = M43;
                
                return ans;
            }
            
            set
            {
                M41 = value.X; 
                M42 = value.Y; 
                M43 = value.Z;
            }
        }


        public Matrix4x4(float m11, float m12, float m13, float m14,
                         float m21, float m22, float m23, float m24,
                         float m31, float m32, float m33, float m34,
                         float m41, float m42, float m43, float m44)
        {
            M11 = m11; M12 = m12; M13 = m13; M14 = m14;
            M21 = m21; M22 = m22; M23 = m23; M24 = m24;
            M31 = m31; M32 = m32; M33 = m33; M34 = m34;
            M41 = m41; M42 = m42; M43 = m43; M44 = m44;
        }


        public Matrix4x4(Matrix3x2 value)
        {
            M11 = value.M11; M12 = value.M12; M13 = 0f; M14 = 0f;
            M21 = value.M21; M22 = value.M22; M23 = 0f; M24 = 0f;
            M31 = 0f;        M32 = 0f;        M33 = 1f; M34 = 0f;
            M41 = value.M31; M42 = value.M32; M43 = 0f; M44 = 1f;
        }


        public static Matrix4x4 CreateBillboard(Vector3 objectPosition, Vector3 cameraPosition, Vector3 cameraUpVector, Vector3 cameraForwardVector)
        {
            const float epsilon = 1e-4f;

            Vector3 zaxis;

            zaxis.X = objectPosition.X - cameraPosition.X;
            zaxis.Y = objectPosition.Y - cameraPosition.Y;
            zaxis.Z = objectPosition.Z - cameraPosition.Z;

            float norm = zaxis.LengthSquared();

            if (norm < epsilon)
            {
                zaxis = -cameraForwardVector;
            }
            else
            {
                zaxis = Vector3.Multiply(zaxis, 1.0f / (float)SM.Sqrt(norm));
            }

            Vector3 xaxis = Vector3.Normalize(Vector3.Cross(cameraUpVector, zaxis));

            Vector3 yaxis = Vector3.Cross(zaxis, xaxis);

            Matrix4x4 result;

            result.M11 = xaxis.X; result.M12 = xaxis.Y; result.M13 = xaxis.Z; result.M14 = 0.0f;
            result.M21 = yaxis.X; result.M22 = yaxis.Y; result.M23 = yaxis.Z; result.M24 = 0.0f;
            result.M31 = zaxis.X; result.M32 = zaxis.Y; result.M33 = zaxis.Z; result.M34 = 0.0f;

            result.M41 = objectPosition.X;
            result.M42 = objectPosition.Y;
            result.M43 = objectPosition.Z;
            result.M44 = 1.0f;

            return result;
        }


        public static Matrix4x4 CreateConstrainedBillboard(Vector3 objectPosition, Vector3 cameraPosition, Vector3 rotateAxis, Vector3 cameraForwardVector, Vector3 objectForwardVector )
        {
            const float epsilon = 1e-4f;
            const float minAngle = 1.0f - (0.1f * ((float)SM.PI / 180.0f)); // 0.1 degrees

            // Treat the case when object and camera positions are too close.
            Vector3 faceDir;

            faceDir.X = objectPosition.X - cameraPosition.X;
            faceDir.Y = objectPosition.Y - cameraPosition.Y;
            faceDir.Z = objectPosition.Z - cameraPosition.Z;

            float norm = faceDir.LengthSquared();

            if (norm < epsilon)
            {
                faceDir = -cameraForwardVector;
            }
            else
            {
                faceDir = Vector3.Multiply(faceDir, (1.0f / (float)SM.Sqrt(norm)));
            }

            Vector3 yaxis = rotateAxis;
            Vector3 xaxis;
            Vector3 zaxis;

            // Treat the case when angle between faceDir and rotateAxis is too close to 0.
            float dot = Vector3.Dot(rotateAxis, faceDir);

            if (SM.Abs(dot) > minAngle)
            {
                zaxis = objectForwardVector;

                // Make sure passed values are useful for compute.
                dot = Vector3.Dot(rotateAxis, zaxis);

                if (SM.Abs(dot) > minAngle)
                {
                    zaxis = (SM.Abs(rotateAxis.Z) > minAngle) ? new Vector3(1, 0, 0) : new Vector3(0, 0, -1);
                }

                xaxis = Vector3.Normalize(Vector3.Cross(rotateAxis, zaxis));
                zaxis = Vector3.Normalize(Vector3.Cross(xaxis, rotateAxis));
            }
            else
            {
                xaxis = Vector3.Normalize(Vector3.Cross(rotateAxis, faceDir));
                zaxis = Vector3.Normalize(Vector3.Cross(xaxis, yaxis));
            }

            Matrix4x4 result;

            result.M11 = xaxis.X; result.M12 = xaxis.Y; result.M13 = xaxis.Z; result.M14 = 0.0f;
            result.M21 = yaxis.X; result.M22 = yaxis.Y; result.M23 = yaxis.Z; result.M24 = 0.0f;
            result.M31 = zaxis.X; result.M32 = zaxis.Y; result.M33 = zaxis.Z; result.M34 = 0.0f;

            result.M41 = objectPosition.X;
            result.M42 = objectPosition.Y;
            result.M43 = objectPosition.Z;
            result.M44 = 1.0f;

            return result;
        }


        public static Matrix4x4 CreateTranslation(Vector3 position)
        {
            Matrix4x4 result;

            result.M11 = 1.0f; result.M12 = 0.0f; result.M13 = 0.0f; result.M14 = 0.0f;
            result.M21 = 0.0f; result.M22 = 1.0f; result.M23 = 0.0f; result.M24 = 0.0f;
            result.M31 = 0.0f; result.M32 = 0.0f; result.M33 = 1.0f; result.M34 = 0.0f;

            result.M41 = position.X;
            result.M42 = position.Y;
            result.M43 = position.Z;
            result.M44 = 1.0f;

            return result;
        }


        public static Matrix4x4 CreateTranslation(float xPosition, float yPosition, float zPosition)
        {
            Matrix4x4 result;

            result.M11 = 1.0f; result.M12 = 0.0f; result.M13 = 0.0f; result.M14 = 0.0f;
            result.M21 = 0.0f; result.M22 = 1.0f; result.M23 = 0.0f; result.M24 = 0.0f;
            result.M31 = 0.0f; result.M32 = 0.0f; result.M33 = 1.0f; result.M34 = 0.0f;

            result.M41 = xPosition;
            result.M42 = yPosition;
            result.M43 = zPosition;
            result.M44 = 1.0f;

            return result;
        }


        public static Matrix4x4 CreateScale(float xScale, float yScale, float zScale)
        {
            Matrix4x4 result;
            
            result.M11 = xScale; result.M12 = 0.0f;   result.M13 = 0.0f;   result.M14 = 0.0f;
            result.M21 = 0.0f;   result.M22 = yScale; result.M23 = 0.0f;   result.M24 = 0.0f;
            result.M31 = 0.0f;   result.M32 = 0.0f;   result.M33 = zScale; result.M34 = 0.0f;
            result.M41 = 0.0f;   result.M42 = 0.0f;   result.M43 = 0.0f;   result.M44 = 1.0f;

            return result;
        }


        public static Matrix4x4 CreateScale(float xScale, float yScale, float zScale, Vector3 centerPoint)
        { 
            Matrix4x4 result;

            float tx = centerPoint.X * (1 - xScale);
            float ty = centerPoint.Y * (1 - yScale);
            float tz = centerPoint.Z * (1 - zScale);

            result.M11 = xScale; result.M12 = 0.0f;   result.M13 = 0.0f;   result.M14 = 0.0f;
            result.M21 = 0.0f;   result.M22 = yScale; result.M23 = 0.0f;   result.M24 = 0.0f;
            result.M31 = 0.0f;   result.M32 = 0.0f;   result.M33 = zScale; result.M34 = 0.0f;
            result.M41 = tx;     result.M42 = ty;     result.M43 = tz;     result.M44 = 1.0f;

            return result;
        }


        public static Matrix4x4 CreateScale(Vector3 scales)
        {
            Matrix4x4 result;

            result.M11 = scales.X; result.M12 = 0.0f;     result.M13 = 0.0f;     result.M14 = 0.0f;
            result.M21 = 0.0f;     result.M22 = scales.Y; result.M23 = 0.0f;     result.M24 = 0.0f;
            result.M31 = 0.0f;     result.M32 = 0.0f;     result.M33 = scales.Z; result.M34 = 0.0f;
            result.M41 = 0.0f;     result.M42 = 0.0f;     result.M43 = 0.0f;     result.M44 = 1.0f;

            return result;
        }


        public static Matrix4x4 CreateScale(Vector3 scales, Vector3 centerPoint)
        {
            Matrix4x4 result;

            float tx = centerPoint.X * (1 - scales.X);
            float ty = centerPoint.Y * (1 - scales.Y);
            float tz = centerPoint.Z * (1 - scales.Z);

            result.M11 = scales.X; result.M12 = 0.0f;     result.M13 = 0.0f;     result.M14 = 0.0f;
            result.M21 = 0.0f;     result.M22 = scales.Y; result.M23 = 0.0f;     result.M24 = 0.0f;
            result.M31 = 0.0f;     result.M32 = 0.0f;     result.M33 = scales.Z; result.M34 = 0.0f;
            result.M41 = tx;       result.M42 = ty;       result.M43 = tz;       result.M44 = 1.0f;

            return result;
        }


        public static Matrix4x4 CreateScale(float scale)
        {
            Matrix4x4 result;

            result.M11 = scale; result.M12 = 0.0f;  result.M13 = 0.0f;  result.M14 = 0.0f;
            result.M21 = 0.0f;  result.M22 = scale; result.M23 = 0.0f;  result.M24 = 0.0f;
            result.M31 = 0.0f;  result.M32 = 0.0f;  result.M33 = scale; result.M34 = 0.0f;
            result.M41 = 0.0f;  result.M42 = 0.0f;  result.M43 = 0.0f;  result.M44 = 1.0f;

            return result;
        }


        public static Matrix4x4 CreateScale(float scale, Vector3 centerPoint)
        {
            Matrix4x4 result;

            float tx = centerPoint.X * (1 - scale);
            float ty = centerPoint.Y * (1 - scale);
            float tz = centerPoint.Z * (1 - scale);

            result.M11 = scale; result.M12 = 0.0f;  result.M13 = 0.0f;  result.M14 = 0.0f;
            result.M21 = 0.0f;  result.M22 = scale; result.M23 = 0.0f;  result.M24 = 0.0f;
            result.M31 = 0.0f;  result.M32 = 0.0f;  result.M33 = scale; result.M34 = 0.0f;
            result.M41 = tx;    result.M42 = ty;    result.M43 = tz;    result.M44 = 1.0f;

            return result;
        }


        public static Matrix4x4 CreateRotationX(float radians)
        {
            Matrix4x4 result;

            float c = (float)SM.Cos(radians);
            float s = (float)SM.Sin(radians);

            // [  1  0  0  0 ]
            // [  0  c  s  0 ]
            // [  0 -s  c  0 ]
            // [  0  0  0  1 ]
            result.M11 = 1.0f; result.M12 = 0.0f; result.M13 = 0.0f; result.M14 = 0.0f;
            result.M21 = 0.0f; result.M22 =    c; result.M23 =    s; result.M24 = 0.0f;
            result.M31 = 0.0f; result.M32 =   -s; result.M33 =    c; result.M34 = 0.0f;
            result.M41 = 0.0f; result.M42 = 0.0f; result.M43 = 0.0f; result.M44 = 1.0f;

            return result;
        }


        public static Matrix4x4 CreateRotationX(float radians, Vector3 centerPoint)
        {
            Matrix4x4 result;

            float c = (float)SM.Cos(radians);
            float s = (float)SM.Sin(radians);

            float y = centerPoint.Y * (1 - c) + centerPoint.Z * s;
            float z = centerPoint.Z * (1 - c) - centerPoint.Y * s;

            // [  1  0  0  0 ]
            // [  0  c  s  0 ]
            // [  0 -s  c  0 ]
            // [  0  y  z  1 ]
            result.M11 = 1.0f; result.M12 = 0.0f; result.M13 = 0.0f; result.M14 = 0.0f;
            result.M21 = 0.0f; result.M22 =    c; result.M23 =    s; result.M24 = 0.0f;
            result.M31 = 0.0f; result.M32 =   -s; result.M33 =    c; result.M34 = 0.0f;
            result.M41 = 0.0f; result.M42 =    y; result.M43 =    z; result.M44 = 1.0f;

            return result;
        }


        public static Matrix4x4 CreateRotationY(float radians)
        {
            Matrix4x4 result;

            float c = (float)SM.Cos(radians);
            float s = (float)SM.Sin(radians);

            // [  c  0 -s  0 ]
            // [  0  1  0  0 ]
            // [  s  0  c  0 ]
            // [  0  0  0  1 ]
            result.M11 =    c; result.M12 = 0.0f; result.M13 =   -s; result.M14 = 0.0f;
            result.M21 = 0.0f; result.M22 = 1.0f; result.M23 = 0.0f; result.M24 = 0.0f;
            result.M31 =    s; result.M32 = 0.0f; result.M33 =    c; result.M34 = 0.0f;
            result.M41 = 0.0f; result.M42 = 0.0f; result.M43 = 0.0f; result.M44 = 1.0f;

            return result;
        }


        public static Matrix4x4 CreateRotationY(float radians, Vector3 centerPoint)
        {
            Matrix4x4 result;

            float c = (float)SM.Cos(radians);
            float s = (float)SM.Sin(radians);

            float x = centerPoint.X * (1 - c) - centerPoint.Z * s;
            float z = centerPoint.Z * (1 - c) + centerPoint.X * s;

            // [  c  0 -s  0 ]
            // [  0  1  0  0 ]
            // [  s  0  c  0 ]
            // [  x  0  z  1 ]
            result.M11 =    c; result.M12 = 0.0f; result.M13 =   -s; result.M14 = 0.0f;
            result.M21 = 0.0f; result.M22 = 1.0f; result.M23 = 0.0f; result.M24 = 0.0f;
            result.M31 =    s; result.M32 = 0.0f; result.M33 =    c; result.M34 = 0.0f;
            result.M41 =    x; result.M42 = 0.0f; result.M43 =    z; result.M44 = 1.0f;

            return result;
        }


        public static Matrix4x4 CreateRotationZ(float radians)
        {
            Matrix4x4 result;

            float c = (float)SM.Cos(radians);
            float s = (float)SM.Sin(radians);

            // [  c  s  0  0 ]
            // [ -s  c  0  0 ]
            // [  0  0  1  0 ]
            // [  0  0  0  1 ]
            result.M11 =    c; result.M12 =    s; result.M13 = 0.0f; result.M14 = 0.0f;
            result.M21 =   -s; result.M22 =    c; result.M23 = 0.0f; result.M24 = 0.0f;
            result.M31 = 0.0f; result.M32 = 0.0f; result.M33 = 1.0f; result.M34 = 0.0f;
            result.M41 = 0.0f; result.M42 = 0.0f; result.M43 = 0.0f; result.M44 = 1.0f;

            return result;
        }


        public static Matrix4x4 CreateRotationZ(float radians, Vector3 centerPoint)
        {
            Matrix4x4 result;

            float c = (float)SM.Cos(radians);
            float s = (float)SM.Sin(radians);

            float x = centerPoint.X * (1 - c) + centerPoint.Y * s;
            float y = centerPoint.Y * (1 - c) - centerPoint.X * s;

            // [  c  s  0  0 ]
            // [ -s  c  0  0 ]
            // [  0  0  1  0 ]
            // [  x  y  0  1 ]
            result.M11 =    c; result.M12 =    s; result.M13 = 0.0f; result.M14 = 0.0f;
            result.M21 =   -s; result.M22 =    c; result.M23 = 0.0f; result.M24 = 0.0f;
            result.M31 = 0.0f; result.M32 = 0.0f; result.M33 = 1.0f; result.M34 = 0.0f;
            result.M41 =    x; result.M42 =    y; result.M43 = 0.0f; result.M44 = 1.0f;

            return result;
        }


        public static Matrix4x4 CreateFromAxisAngle(Vector3 axis, float angle)
        {
            // a: angle
            // x, y, z: unit vector for axis.
            //
            // Rotation matrix M can compute by using below equation.
            //
            //        T               T
            //  M = uu + (cos a)( I-uu ) + (sin a)S
            //
            // Where:
            //
            //  u = ( x, y, z )
            //
            //      [  0 -z  y ]
            //  S = [  z  0 -x ]
            //      [ -y  x  0 ]
            //
            //      [ 1 0 0 ]
            //  I = [ 0 1 0 ]
            //      [ 0 0 1 ]
            //
            //
            //     [  xx+cosa*(1-xx)   yx-cosa*yx-sina*z zx-cosa*xz+sina*y ]
            // M = [ xy-cosa*yx+sina*z    yy+cosa(1-yy)  yz-cosa*yz-sina*x ]
            //     [ zx-cosa*zx-sina*y zy-cosa*zy+sina*x   zz+cosa*(1-zz)  ]
            //
            float x = axis.X, y = axis.Y, z = axis.Z;
            float sa = (float)SM.Sin(angle), ca = (float)SM.Cos(angle);
            float xx = x * x, yy = y * y, zz = z * z;
            float xy = x * y, xz = x * z, yz = y * z;

            Matrix4x4 result;

            result.M11 = xx + ca * ( 1.0f - xx );   result.M12 = xy - ca * xy + sa * z;     result.M13 = xz - ca * xz - sa * y;     result.M14 = 0.0f;
            result.M21 = xy - ca * xy - sa * z;     result.M22 = yy + ca * ( 1.0f - yy );   result.M23 = yz - ca * yz + sa * x;     result.M24 = 0.0f;
            result.M31 = xz - ca * xz + sa * y;     result.M32 = yz - ca * yz - sa * x;     result.M33 = zz + ca * ( 1.0f - zz );   result.M34 = 0.0f;
            result.M41 = 0.0f;                      result.M42 = 0.0f;                      result.M43 = 0.0f;                      result.M44 = 1.0f;

            return result;
        }


        public static Matrix4x4 CreatePerspectiveFieldOfView(float fieldOfView, float aspectRatio, float nearPlaneDistance, float farPlaneDistance)
        {
            if (fieldOfView <= 0.0f || fieldOfView >= SM.PI)
                throw new ArgumentOutOfRangeException("fieldOfView");

            if (nearPlaneDistance <= 0.0f)
                throw new ArgumentOutOfRangeException("nearPlaneDistance");

            if (farPlaneDistance <= 0.0f)
                throw new ArgumentOutOfRangeException("farPlaneDistance");

            if (nearPlaneDistance >= farPlaneDistance )
                throw new ArgumentOutOfRangeException("nearPlaneDistance");

            float yScale = 1.0f / (float)SM.Tan(fieldOfView * 0.5f);
            float xScale = yScale / aspectRatio;

            Matrix4x4 result;

            result.M11 = xScale;
            result.M12 = result.M13 = result.M14 = 0.0f;

            result.M22 = yScale;
            result.M21 = result.M23 = result.M24 = 0.0f;

            result.M31 = result.M32 = 0.0f;
            result.M33 = farPlaneDistance / (nearPlaneDistance - farPlaneDistance);
            result.M34 = -1.0f;

            result.M41 = result.M42 = result.M44 = 0.0f;
            result.M43 = nearPlaneDistance * farPlaneDistance / (nearPlaneDistance - farPlaneDistance);

            return result;
        }


        public static Matrix4x4 CreatePerspective(float width, float height, float nearPlaneDistance, float farPlaneDistance)
        {
            if (nearPlaneDistance <= 0.0f)
                throw new ArgumentOutOfRangeException("nearPlaneDistance");

            if (farPlaneDistance <= 0.0f)
                throw new ArgumentOutOfRangeException("farPlaneDistance");

            if (nearPlaneDistance >= farPlaneDistance)
                throw new ArgumentOutOfRangeException("nearPlaneDistance");

            Matrix4x4 result;

            result.M11 = 2.0f * nearPlaneDistance / width;
            result.M12 = result.M13 = result.M14 = 0.0f;

            result.M22 = 2.0f * nearPlaneDistance / height;
            result.M21 = result.M23 = result.M24 = 0.0f;

            result.M33 = farPlaneDistance / (nearPlaneDistance - farPlaneDistance);
            result.M31 = result.M32 = 0.0f;
            result.M34 = -1.0f;

            result.M41 = result.M42 = result.M44 = 0.0f;
            result.M43 = nearPlaneDistance * farPlaneDistance / (nearPlaneDistance - farPlaneDistance);

            return result;
        }


        public static Matrix4x4 CreatePerspectiveOffCenter(float left, float right, float bottom, float top, float nearPlaneDistance, float farPlaneDistance)
        {
            if (nearPlaneDistance <= 0.0f)
                throw new ArgumentOutOfRangeException("nearPlaneDistance");

            if (farPlaneDistance <= 0.0f)
                throw new ArgumentOutOfRangeException("farPlaneDistance");

            if (nearPlaneDistance >= farPlaneDistance)
                throw new ArgumentOutOfRangeException("nearPlaneDistance");

            Matrix4x4 result;

            result.M11 = 2.0f * nearPlaneDistance / (right - left);
            result.M12 = result.M13 = result.M14 = 0.0f;

            result.M22 = 2.0f * nearPlaneDistance / (top - bottom);
            result.M21 = result.M23 = result.M24 = 0.0f;

            result.M31 = (left + right) / (right - left);
            result.M32 = (top + bottom) / (top - bottom);
            result.M33 = farPlaneDistance / (nearPlaneDistance - farPlaneDistance);
            result.M34 = -1.0f;

            result.M43 = nearPlaneDistance * farPlaneDistance / (nearPlaneDistance - farPlaneDistance);
            result.M41 = result.M42 = result.M44 = 0.0f;

            return result;
        }


        public static Matrix4x4 CreateOrthographic(float width, float height, float zNearPlane, float zFarPlane)
        {
            Matrix4x4 result;

            result.M11 = 2.0f / width;
            result.M12 = result.M13 = result.M14 = 0.0f;

            result.M22 = 2.0f / height;
            result.M21 = result.M23 = result.M24 = 0.0f;

            result.M33 = 1.0f / (zNearPlane - zFarPlane);
            result.M31 = result.M32 = result.M34 = 0.0f;

            result.M41 = result.M42 = 0.0f;
            result.M43 = zNearPlane / (zNearPlane - zFarPlane);
            result.M44 = 1.0f;

            return result;
        }


        public static Matrix4x4 CreateOrthographicOffCenter(float left, float right, float bottom, float top, float zNearPlane, float zFarPlane)
        {
            Matrix4x4 result;

            result.M11 = 2.0f / (right - left);
            result.M12 = result.M13 = result.M14 = 0.0f;

            result.M22 = 2.0f / (top - bottom);
            result.M21 = result.M23 = result.M24 = 0.0f;

            result.M33 = 1.0f / (zNearPlane - zFarPlane);
            result.M31 = result.M32 = result.M34 = 0.0f;

            result.M41 = (left + right) / (left - right);
            result.M42 = (top + bottom) / (bottom - top);
            result.M43 = zNearPlane / (zNearPlane - zFarPlane);
            result.M44 = 1.0f;

            return result;
        }


        public static Matrix4x4 CreateLookAt(Vector3 cameraPosition, Vector3 cameraTarget, Vector3 cameraUpVector)
        {
            Vector3 zaxis = Vector3.Normalize(cameraPosition - cameraTarget);
            Vector3 xaxis = Vector3.Normalize(Vector3.Cross(cameraUpVector, zaxis));
            Vector3 yaxis = Vector3.Cross(zaxis, xaxis);

            Matrix4x4 result;

            result.M11 = xaxis.X; result.M12 = yaxis.X; result.M13 = zaxis.X; result.M14 = 0.0f;
            result.M21 = xaxis.Y; result.M22 = yaxis.Y; result.M23 = zaxis.Y; result.M24 = 0.0f;
            result.M31 = xaxis.Z; result.M32 = yaxis.Z; result.M33 = zaxis.Z; result.M34 = 0.0f;
            result.M41 = -Vector3.Dot(xaxis, cameraPosition);
            result.M42 = -Vector3.Dot(yaxis, cameraPosition);
            result.M43 = -Vector3.Dot(zaxis, cameraPosition);
            result.M44 = 1.0f;

            return result;
        }


        public static Matrix4x4 CreateWorld(Vector3 position, Vector3 forward, Vector3 up)
        {
            Vector3 zaxis = Vector3.Normalize(-forward);
            Vector3 xaxis = Vector3.Normalize(Vector3.Cross(up, zaxis));
            Vector3 yaxis = Vector3.Cross(zaxis, xaxis);

            Matrix4x4 result;

            result.M11 = xaxis.X; result.M12 = xaxis.Y; result.M13 = xaxis.Z; result.M14 = 0.0f;
            result.M21 = yaxis.X; result.M22 = yaxis.Y; result.M23 = yaxis.Z; result.M24 = 0.0f;
            result.M31 = zaxis.X; result.M32 = zaxis.Y; result.M33 = zaxis.Z; result.M34 = 0.0f;
            result.M41 = position.X;
            result.M42 = position.Y;
            result.M43 = position.Z;
            result.M44 = 1.0f;

            return result;
        }


        public static Matrix4x4 CreateFromQuaternion(Quaternion quaternion)
        {
            Matrix4x4 result;

            float xx = quaternion.X * quaternion.X;
            float yy = quaternion.Y * quaternion.Y;
            float zz = quaternion.Z * quaternion.Z;

            float xy = quaternion.X * quaternion.Y;
            float wz = quaternion.Z * quaternion.W;
            float xz = quaternion.Z * quaternion.X;
            float wy = quaternion.Y * quaternion.W;
            float yz = quaternion.Y * quaternion.Z;
            float wx = quaternion.X * quaternion.W;

            result.M11 = 1.0f - 2.0f * (yy + zz);   result.M12 = 2.0f * (xy + wz);          result.M13 = 2.0f * (xz - wy);          result.M14 = 0.0f;
            result.M21 = 2.0f * (xy - wz);          result.M22 = 1.0f - 2.0f * (zz + xx);   result.M23 = 2.0f * (yz + wx);          result.M24 = 0.0f;
            result.M31 = 2.0f * (xz + wy);          result.M32 = 2.0f * (yz - wx);          result.M33 = 1.0f - 2.0f * (yy + xx);   result.M34 = 0.0f;
            result.M41 = 0.0f;                      result.M42 = 0.0f;                      result.M43 = 0.0f;                      result.M44 = 1.0f;

            return result;
        }


        public static Matrix4x4 CreateFromYawPitchRoll(float yaw, float pitch, float roll)
        {
            Quaternion q = Quaternion.CreateFromYawPitchRoll(yaw, pitch, roll);

            return Matrix4x4.CreateFromQuaternion(q);
        }


        public static Matrix4x4 CreateShadow(Vector3 lightDirection, Plane plane)
        {
            Plane p = Plane.Normalize(plane);

            float dot = p.Normal.X * lightDirection.X + p.Normal.Y * lightDirection.Y + p.Normal.Z * lightDirection.Z;
            float a = -p.Normal.X;
            float b = -p.Normal.Y;
            float c = -p.Normal.Z;
            float d = -p.D;

            Matrix4x4 result;

            result.M11 = a * lightDirection.X + dot;
            result.M21 = b * lightDirection.X;
            result.M31 = c * lightDirection.X;
            result.M41 = d * lightDirection.X;

            result.M12 = a * lightDirection.Y;
            result.M22 = b * lightDirection.Y + dot;
            result.M32 = c * lightDirection.Y;
            result.M42 = d * lightDirection.Y;

            result.M13 = a * lightDirection.Z;
            result.M23 = b * lightDirection.Z;
            result.M33 = c * lightDirection.Z + dot;
            result.M43 = d * lightDirection.Z;

            result.M14 = 0.0f;
            result.M24 = 0.0f;
            result.M34 = 0.0f;
            result.M44 = dot;

            return result;
        }


        public static Matrix4x4 CreateReflection(Plane value)
        {
            value = Plane.Normalize(value);

            float a = value.Normal.X;
            float b = value.Normal.Y;
            float c = value.Normal.Z;

            float fa = -2.0f * a;
            float fb = -2.0f * b;
            float fc = -2.0f * c;

            Matrix4x4 result;

            result.M11 = fa * a + 1.0f;
            result.M12 = fb * a;
            result.M13 = fc * a;
            result.M14 = 0.0f;

            result.M21 = fa * b;
            result.M22 = fb * b + 1.0f;
            result.M23 = fc * b;
            result.M24 = 0.0f;

            result.M31 = fa * c;
            result.M32 = fb * c;
            result.M33 = fc * c + 1.0f;
            result.M34 = 0.0f;

            result.M41 = fa * value.D;
            result.M42 = fb * value.D;
            result.M43 = fc * value.D;
            result.M44 = 1.0f;

            return result;
        }


        public float GetDeterminant()
        {
            // | a b c d |     | f g h |     | e g h |     | e f h |     | e f g |
            // | e f g h | = a | j k l | - b | i k l | + c | i j l | - d | i j k |
            // | i j k l |     | n o p |     | m o p |     | m n p |     | m n o |
            // | m n o p |
            //
            //   | f g h |
            // a | j k l | = a ( f ( kp - lo ) - g ( jp - ln ) + h ( jo - kn ) )
            //   | n o p |
            //
            //   | e g h |     
            // b | i k l | = b ( e ( kp - lo ) - g ( ip - lm ) + h ( io - km ) )
            //   | m o p |     
            //
            //   | e f h |
            // c | i j l | = c ( e ( jp - ln ) - f ( ip - lm ) + h ( in - jm ) )
            //   | m n p |
            //
            //   | e f g |
            // d | i j k | = d ( e ( jo - kn ) - f ( io - km ) + g ( in - jm ) )
            //   | m n o |
            //
            // Cost of operation
            // 17 adds and 28 muls.
            //
            // add: 6 + 8 + 3 = 17
            // mul: 12 + 16 = 28

            float a = M11, b = M12, c = M13, d = M14;
            float e = M21, f = M22, g = M23, h = M24;
            float i = M31, j = M32, k = M33, l = M34;
            float m = M41, n = M42, o = M43, p = M44;

            float kp_lo = k * p - l * o;
            float jp_ln = j * p - l * n;
            float jo_kn = j * o - k * n;
            float ip_lm = i * p - l * m;
            float io_km = i * o - k * m;
            float in_jm = i * n - j * m;

            return a * (f * kp_lo - g * jp_ln + h * jo_kn) -
                   b * (e * kp_lo - g * ip_lm + h * io_km) +
                   c * (e * jp_ln - f * ip_lm + h * in_jm) -
                   d * (e * jo_kn - f * io_km + g * in_jm);
        }


        public static bool Invert(Matrix4x4 matrix, out Matrix4x4 result)
        {
            //                                       -1
            // If you have matrix M, inverse Matrix M   can compute
            //
            //     -1       1      
            //    M   = --------- A
            //            det(M)
            //
            // A is adjugate (adjoint) of M, where,
            //
            //      T
            // A = C
            //
            // C is Cofactor matrix of M, where,
            //           i + j
            // C   = (-1)      * det(M  )
            //  ij                    ij
            //
            //     [ a b c d ]
            // M = [ e f g h ]
            //     [ i j k l ]
            //     [ m n o p ]
            //
            // First Row
            //           2 | f g h |
            // C   = (-1)  | j k l | = + ( f ( kp - lo ) - g ( jp - ln ) + h ( jo - kn ) )
            //  11         | n o p |
            //
            //           3 | e g h |
            // C   = (-1)  | i k l | = - ( e ( kp - lo ) - g ( ip - lm ) + h ( io - km ) )
            //  12         | m o p |
            //
            //           4 | e f h |
            // C   = (-1)  | i j l | = + ( e ( jp - ln ) - f ( ip - lm ) + h ( in - jm ) )
            //  13         | m n p |
            //
            //           5 | e f g |
            // C   = (-1)  | i j k | = - ( e ( jo - kn ) - f ( io - km ) + g ( in - jm ) )
            //  14         | m n o |
            //
            // Second Row
            //           3 | b c d |
            // C   = (-1)  | j k l | = - ( b ( kp - lo ) - c ( jp - ln ) + d ( jo - kn ) )
            //  21         | n o p |
            //
            //           4 | a c d |
            // C   = (-1)  | i k l | = + ( a ( kp - lo ) - c ( ip - lm ) + d ( io - km ) )
            //  22         | m o p |
            //
            //           5 | a b d |
            // C   = (-1)  | i j l | = - ( a ( jp - ln ) - b ( ip - lm ) + d ( in - jm ) )
            //  23         | m n p |
            //
            //           6 | a b c |
            // C   = (-1)  | i j k | = + ( a ( jo - kn ) - b ( io - km ) + c ( in - jm ) )
            //  24         | m n o |
            //
            // Third Row
            //           4 | b c d |
            // C   = (-1)  | f g h | = + ( b ( gp - ho ) - c ( fp - hn ) + d ( fo - gn ) )
            //  31         | n o p |
            //
            //           5 | a c d |
            // C   = (-1)  | e g h | = - ( a ( gp - ho ) - c ( ep - hm ) + d ( eo - gm ) )
            //  32         | m o p |
            //
            //           6 | a b d |
            // C   = (-1)  | e f h | = + ( a ( fp - hn ) - b ( ep - hm ) + d ( en - fm ) )
            //  33         | m n p |
            //
            //           7 | a b c |
            // C   = (-1)  | e f g | = - ( a ( fo - gn ) - b ( eo - gm ) + c ( en - fm ) )
            //  34         | m n o |
            //
            // Fourth Row
            //           5 | b c d |
            // C   = (-1)  | f g h | = - ( b ( gl - hk ) - c ( fl - hj ) + d ( fk - gj ) )
            //  41         | j k l |
            //
            //           6 | a c d |
            // C   = (-1)  | e g h | = + ( a ( gl - hk ) - c ( el - hi ) + d ( ek - gi ) )
            //  42         | i k l |
            //
            //           7 | a b d |
            // C   = (-1)  | e f h | = - ( a ( fl - hj ) - b ( el - hi ) + d ( ej - fi ) )
            //  43         | i j l |
            //
            //           8 | a b c |
            // C   = (-1)  | e f g | = + ( a ( fk - gj ) - b ( ek - gi ) + c ( ej - fi ) )
            //  44         | i j k |
            //
            // Cost of operation
            // 53 adds, 104 muls, and 1 div.
            float a = matrix.M11, b = matrix.M12, c = matrix.M13, d = matrix.M14;
            float e = matrix.M21, f = matrix.M22, g = matrix.M23, h = matrix.M24;
            float i = matrix.M31, j = matrix.M32, k = matrix.M33, l = matrix.M34;
            float m = matrix.M41, n = matrix.M42, o = matrix.M43, p = matrix.M44;

            float kp_lo = k * p - l * o;
            float jp_ln = j * p - l * n;
            float jo_kn = j * o - k * n;
            float ip_lm = i * p - l * m;
            float io_km = i * o - k * m;
            float in_jm = i * n - j * m;

            float a11 = +(f * kp_lo - g * jp_ln + h * jo_kn);
            float a12 = -(e * kp_lo - g * ip_lm + h * io_km);
            float a13 = +(e * jp_ln - f * ip_lm + h * in_jm);
            float a14 = -(e * jo_kn - f * io_km + g * in_jm);

            float det = a * a11 + b * a12 + c * a13 + d * a14;

            if (SM.Abs(det) < float.Epsilon)
            {
                result = new Matrix4x4(float.NaN, float.NaN, float.NaN, float.NaN,
                                       float.NaN, float.NaN, float.NaN, float.NaN,
                                       float.NaN, float.NaN, float.NaN, float.NaN,
                                       float.NaN, float.NaN, float.NaN, float.NaN);
                return false;
            }

            float invDet = 1.0f / det;

            result.M11 = a11 * invDet;
            result.M21 = a12 * invDet;
            result.M31 = a13 * invDet;
            result.M41 = a14 * invDet;

            result.M12 = -(b * kp_lo - c * jp_ln + d * jo_kn) * invDet;
            result.M22 = +(a * kp_lo - c * ip_lm + d * io_km) * invDet;
            result.M32 = -(a * jp_ln - b * ip_lm + d * in_jm) * invDet;
            result.M42 = +(a * jo_kn - b * io_km + c * in_jm) * invDet;

            float gp_ho = g * p - h * o;
            float fp_hn = f * p - h * n;
            float fo_gn = f * o - g * n;
            float ep_hm = e * p - h * m;
            float eo_gm = e * o - g * m;
            float en_fm = e * n - f * m;

            result.M13 = +(b * gp_ho - c * fp_hn + d * fo_gn) * invDet;
            result.M23 = -(a * gp_ho - c * ep_hm + d * eo_gm) * invDet;
            result.M33 = +(a * fp_hn - b * ep_hm + d * en_fm) * invDet;
            result.M43 = -(a * fo_gn - b * eo_gm + c * en_fm) * invDet;

            float gl_hk = g * l - h * k;
            float fl_hj = f * l - h * j;
            float fk_gj = f * k - g * j;
            float el_hi = e * l - h * i;
            float ek_gi = e * k - g * i;
            float ej_fi = e * j - f * i;

            result.M14 = -(b * gl_hk - c * fl_hj + d * fk_gj) * invDet;
            result.M24 = +(a * gl_hk - c * el_hi + d * ek_gi) * invDet;
            result.M34 = -(a * fl_hj - b * el_hi + d * ej_fi) * invDet;
            result.M44 = +(a * fk_gj - b * ek_gi + c * ej_fi) * invDet;

            return true;
        }


        struct CanonicalBasis
        {
            public Vector3 Row0;
            public Vector3 Row1;
            public Vector3 Row2;
        };


        struct VectorBasis
        {
            public unsafe Vector3* Element0;
            public unsafe Vector3* Element1;
            public unsafe Vector3* Element2;
        }


        public static bool Decompose(Matrix4x4 matrix, out Vector3 scale, out Quaternion rotation, out Vector3 translation)
        {
            bool result = true;

            unsafe
            {
                fixed (float* pfScales = &scale.X)
                {
                    const float EPSILON = 0.0001f;
                    float det;

                    VectorBasis vectorBasis;
                    Vector3** pVectorBasis = (Vector3**)&vectorBasis;

                    Matrix4x4 matTemp = Matrix4x4.Identity;
                    CanonicalBasis canonicalBasis = new CanonicalBasis();
                    Vector3* pCanonicalBasis = &canonicalBasis.Row0;

                    canonicalBasis.Row0 = new Vector3(1.0f, 0.0f, 0.0f);
                    canonicalBasis.Row1 = new Vector3(0.0f, 1.0f, 0.0f);
                    canonicalBasis.Row2 = new Vector3(0.0f, 0.0f, 1.0f);

                    translation.X = matrix.M41;
                    translation.Y = matrix.M42;
                    translation.Z = matrix.M43;

                    pVectorBasis[0] = (Vector3*)&matTemp.M11;
                    pVectorBasis[1] = (Vector3*)&matTemp.M21;
                    pVectorBasis[2] = (Vector3*)&matTemp.M31;

                    *(pVectorBasis[0]) = new Vector3(matrix.M11, matrix.M12, matrix.M13);
                    *(pVectorBasis[1]) = new Vector3(matrix.M21, matrix.M22, matrix.M23);
                    *(pVectorBasis[2]) = new Vector3(matrix.M31, matrix.M32, matrix.M33);

                    scale.X = pVectorBasis[0]->Length();
                    scale.Y = pVectorBasis[1]->Length();
                    scale.Z = pVectorBasis[2]->Length();

                    uint a, b, c;
                    #region Ranking
                    float x = pfScales[0], y = pfScales[1], z = pfScales[2];
                    if (x < y)
                    {
                        if (y < z)
                        {
                            a = 2;
                            b = 1;
                            c = 0;
                        }
                        else
                        {
                            a = 1;

                            if (x < z)
                            {
                                b = 2;
                                c = 0;
                            }
                            else
                            {
                                b = 0;
                                c = 2;
                            }
                        }
                    }
                    else
                    {
                        if (x < z)
                        {
                            a = 2;
                            b = 0;
                            c = 1;
                        }
                        else
                        {
                            a = 0;

                            if (y < z)
                            {
                                b = 2;
                                c = 1;
                            }
                            else
                            {
                                b = 1;
                                c = 2;
                            }
                        }
                    }
                    #endregion

                    if (pfScales[a] < EPSILON)
                    {
                        *(pVectorBasis[a]) = pCanonicalBasis[a];
                    }

                    *pVectorBasis[a] = Vector3.Normalize(*pVectorBasis[a]);

                    if (pfScales[b] < EPSILON)
                    {
                        uint cc;
                        float fAbsX, fAbsY, fAbsZ;

                        fAbsX = (float)SM.Abs(pVectorBasis[a]->X);
                        fAbsY = (float)SM.Abs(pVectorBasis[a]->Y);
                        fAbsZ = (float)SM.Abs(pVectorBasis[a]->Z);

                        #region Ranking
                        if (fAbsX < fAbsY)
                        {
                            if (fAbsY < fAbsZ)
                            {
                                cc = 0;
                            }
                            else
                            {
                                if (fAbsX < fAbsZ)
                                {
                                    cc = 0;
                                }
                                else
                                {
                                    cc = 2;
                                }
                            }
                        }
                        else
                        {
                            if (fAbsX < fAbsZ)
                            {
                                cc = 1;
                            }
                            else
                            {
                                if (fAbsY < fAbsZ)
                                {
                                    cc = 1;
                                }
                                else
                                {
                                    cc = 2;
                                }
                            }
                        }
                        #endregion

                        *pVectorBasis[b] = Vector3.Cross(*pVectorBasis[a], *(pCanonicalBasis + cc));
                    }

                    *pVectorBasis[b] = Vector3.Normalize(*pVectorBasis[b]);

                    if (pfScales[c] < EPSILON)
                    {
                        *pVectorBasis[c] = Vector3.Cross(*pVectorBasis[a], *pVectorBasis[b]);
                    }

                    *pVectorBasis[c] = Vector3.Normalize(*pVectorBasis[c]);

                    det = matTemp.GetDeterminant();

                    // use Kramer's rule to check for handedness of coordinate system
                    if (det < 0.0f)
                    {
                        // switch coordinate system by negating the scale and inverting the basis vector on the x-axis
                        pfScales[a] = -pfScales[a];
                        *pVectorBasis[a] = -(*pVectorBasis[a]);

                        det = -det;
                    }

                    det -= 1.0f;
                    det *= det;

                    if ((EPSILON < det))
                    {
                        // Non-SRT matrix encountered
                        rotation = Quaternion.Identity;
                        result = false;
                    }
                    else
                    {
                        // generate the quaternion from the matrix
                        rotation = Quaternion.CreateFromRotationMatrix(matTemp);
                    }
                }
            }

            return result;
        }


        public static Matrix4x4 Transform(Matrix4x4 value, Quaternion rotation)
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

            float q11 = 1.0f - yy2 - zz2;
            float q21 = xy2 - wz2;
            float q31 = xz2 + wy2;

            float q12 = xy2 + wz2;
            float q22 = 1.0f - xx2 - zz2;
            float q32 = yz2 - wx2;

            float q13 = xz2 - wy2;
            float q23 = yz2 + wx2;
            float q33 = 1.0f - xx2 - yy2;

            Matrix4x4 result;

            // First row
            result.M11 = value.M11 * q11 + value.M12 * q21 + value.M13 * q31;
            result.M12 = value.M11 * q12 + value.M12 * q22 + value.M13 * q32;
            result.M13 = value.M11 * q13 + value.M12 * q23 + value.M13 * q33;
            result.M14 = value.M14;

            // Second row
            result.M21 = value.M21 * q11 + value.M22 * q21 + value.M23 * q31;
            result.M22 = value.M21 * q12 + value.M22 * q22 + value.M23 * q32;
            result.M23 = value.M21 * q13 + value.M22 * q23 + value.M23 * q33;
            result.M24 = value.M24;

            // Third row
            result.M31 = value.M31 * q11 + value.M32 * q21 + value.M33 * q31;
            result.M32 = value.M31 * q12 + value.M32 * q22 + value.M33 * q32;
            result.M33 = value.M31 * q13 + value.M32 * q23 + value.M33 * q33;
            result.M34 = value.M34;

            // Fourth row
            result.M41 = value.M41 * q11 + value.M42 * q21 + value.M43 * q31;
            result.M42 = value.M41 * q12 + value.M42 * q22 + value.M43 * q32;
            result.M43 = value.M41 * q13 + value.M42 * q23 + value.M43 * q33;
            result.M44 = value.M44;

            return result;
        }


        public static Matrix4x4 Transpose(Matrix4x4 matrix)
        {
            Matrix4x4 result;

            result.M11 = matrix.M11; result.M12 = matrix.M21; result.M13 = matrix.M31; result.M14 = matrix.M41;
            result.M21 = matrix.M12; result.M22 = matrix.M22; result.M23 = matrix.M32; result.M24 = matrix.M42;
            result.M31 = matrix.M13; result.M32 = matrix.M23; result.M33 = matrix.M33; result.M34 = matrix.M43;
            result.M41 = matrix.M14; result.M42 = matrix.M24; result.M43 = matrix.M34; result.M44 = matrix.M44;

            return result;
        }


        public static Matrix4x4 Lerp(Matrix4x4 matrix1, Matrix4x4 matrix2, float amount)
        {
            Matrix4x4 result;
            
            // First row
            result.M11 = matrix1.M11 + (matrix2.M11 - matrix1.M11) * amount;
            result.M12 = matrix1.M12 + (matrix2.M12 - matrix1.M12) * amount;
            result.M13 = matrix1.M13 + (matrix2.M13 - matrix1.M13) * amount;
            result.M14 = matrix1.M14 + (matrix2.M14 - matrix1.M14) * amount;
            
            // Second row
            result.M21 = matrix1.M21 + (matrix2.M21 - matrix1.M21) * amount;
            result.M22 = matrix1.M22 + (matrix2.M22 - matrix1.M22) * amount;
            result.M23 = matrix1.M23 + (matrix2.M23 - matrix1.M23) * amount;
            result.M24 = matrix1.M24 + (matrix2.M24 - matrix1.M24) * amount;
            
            // Third row
            result.M31 = matrix1.M31 + (matrix2.M31 - matrix1.M31) * amount;
            result.M32 = matrix1.M32 + (matrix2.M32 - matrix1.M32) * amount;
            result.M33 = matrix1.M33 + (matrix2.M33 - matrix1.M33) * amount;
            result.M34 = matrix1.M34 + (matrix2.M34 - matrix1.M34) * amount;
            
            // Fourth row
            result.M41 = matrix1.M41 + (matrix2.M41 - matrix1.M41) * amount;
            result.M42 = matrix1.M42 + (matrix2.M42 - matrix1.M42) * amount;
            result.M43 = matrix1.M43 + (matrix2.M43 - matrix1.M43) * amount;
            result.M44 = matrix1.M44 + (matrix2.M44 - matrix1.M44) * amount;

            return result;
        }
        
        
        public static Matrix4x4 Negate(Matrix4x4 value)
        {
            Matrix4x4 result;

            result.M11 = -value.M11; result.M12 = -value.M12; result.M13 = -value.M13; result.M14 = -value.M14;
            result.M21 = -value.M21; result.M22 = -value.M22; result.M23 = -value.M23; result.M24 = -value.M24;
            result.M31 = -value.M31; result.M32 = -value.M32; result.M33 = -value.M33; result.M34 = -value.M34;
            result.M41 = -value.M41; result.M42 = -value.M42; result.M43 = -value.M43; result.M44 = -value.M44;

            return result;
        }


        public static Matrix4x4 Add(Matrix4x4 value1, Matrix4x4 value2)
        {
            Matrix4x4 result;

            result.M11 = value1.M11 + value2.M11; result.M12 = value1.M12 + value2.M12; result.M13 = value1.M13 + value2.M13; result.M14 = value1.M14 + value2.M14;
            result.M21 = value1.M21 + value2.M21; result.M22 = value1.M22 + value2.M22; result.M23 = value1.M23 + value2.M23; result.M24 = value1.M24 + value2.M24;
            result.M31 = value1.M31 + value2.M31; result.M32 = value1.M32 + value2.M32; result.M33 = value1.M33 + value2.M33; result.M34 = value1.M34 + value2.M34;
            result.M41 = value1.M41 + value2.M41; result.M42 = value1.M42 + value2.M42; result.M43 = value1.M43 + value2.M43; result.M44 = value1.M44 + value2.M44;

            return result;
        }


        public static Matrix4x4 Subtract(Matrix4x4 value1, Matrix4x4 value2)
        {
            Matrix4x4 result;

            result.M11 = value1.M11 - value2.M11; result.M12 = value1.M12 - value2.M12; result.M13 = value1.M13 - value2.M13; result.M14 = value1.M14 - value2.M14;
            result.M21 = value1.M21 - value2.M21; result.M22 = value1.M22 - value2.M22; result.M23 = value1.M23 - value2.M23; result.M24 = value1.M24 - value2.M24;
            result.M31 = value1.M31 - value2.M31; result.M32 = value1.M32 - value2.M32; result.M33 = value1.M33 - value2.M33; result.M34 = value1.M34 - value2.M34;
            result.M41 = value1.M41 - value2.M41; result.M42 = value1.M42 - value2.M42; result.M43 = value1.M43 - value2.M43; result.M44 = value1.M44 - value2.M44;

            return result;
        }


        public static Matrix4x4 Multiply(Matrix4x4 value1, Matrix4x4 value2)
        {
            Matrix4x4 result;

            // First row
            result.M11 = value1.M11 * value2.M11 + value1.M12 * value2.M21 + value1.M13 * value2.M31 + value1.M14 * value2.M41;
            result.M12 = value1.M11 * value2.M12 + value1.M12 * value2.M22 + value1.M13 * value2.M32 + value1.M14 * value2.M42;
            result.M13 = value1.M11 * value2.M13 + value1.M12 * value2.M23 + value1.M13 * value2.M33 + value1.M14 * value2.M43;
            result.M14 = value1.M11 * value2.M14 + value1.M12 * value2.M24 + value1.M13 * value2.M34 + value1.M14 * value2.M44;

            // Second row
            result.M21 = value1.M21 * value2.M11 + value1.M22 * value2.M21 + value1.M23 * value2.M31 + value1.M24 * value2.M41;
            result.M22 = value1.M21 * value2.M12 + value1.M22 * value2.M22 + value1.M23 * value2.M32 + value1.M24 * value2.M42;
            result.M23 = value1.M21 * value2.M13 + value1.M22 * value2.M23 + value1.M23 * value2.M33 + value1.M24 * value2.M43;
            result.M24 = value1.M21 * value2.M14 + value1.M22 * value2.M24 + value1.M23 * value2.M34 + value1.M24 * value2.M44;

            // Third row
            result.M31 = value1.M31 * value2.M11 + value1.M32 * value2.M21 + value1.M33 * value2.M31 + value1.M34 * value2.M41;
            result.M32 = value1.M31 * value2.M12 + value1.M32 * value2.M22 + value1.M33 * value2.M32 + value1.M34 * value2.M42;
            result.M33 = value1.M31 * value2.M13 + value1.M32 * value2.M23 + value1.M33 * value2.M33 + value1.M34 * value2.M43;
            result.M34 = value1.M31 * value2.M14 + value1.M32 * value2.M24 + value1.M33 * value2.M34 + value1.M34 * value2.M44;

            // Fourth row
            result.M41 = value1.M41 * value2.M11 + value1.M42 * value2.M21 + value1.M43 * value2.M31 + value1.M44 * value2.M41;
            result.M42 = value1.M41 * value2.M12 + value1.M42 * value2.M22 + value1.M43 * value2.M32 + value1.M44 * value2.M42;
            result.M43 = value1.M41 * value2.M13 + value1.M42 * value2.M23 + value1.M43 * value2.M33 + value1.M44 * value2.M43;
            result.M44 = value1.M41 * value2.M14 + value1.M42 * value2.M24 + value1.M43 * value2.M34 + value1.M44 * value2.M44;

            return result;
        }
        
        
        public static Matrix4x4 Multiply(Matrix4x4 value1, float value2)
        {
            Matrix4x4 result;

            result.M11 = value1.M11 * value2; result.M12 = value1.M12 * value2; result.M13 = value1.M13 * value2; result.M14 = value1.M14 * value2;
            result.M21 = value1.M21 * value2; result.M22 = value1.M22 * value2; result.M23 = value1.M23 * value2; result.M24 = value1.M24 * value2;
            result.M31 = value1.M31 * value2; result.M32 = value1.M32 * value2; result.M33 = value1.M33 * value2; result.M34 = value1.M34 * value2;
            result.M41 = value1.M41 * value2; result.M42 = value1.M42 * value2; result.M43 = value1.M43 * value2; result.M44 = value1.M44 * value2;

            return result;
        }


        public static Matrix4x4 operator -(Matrix4x4 value)
        {
            Matrix4x4 m;

            m.M11 = -value.M11; m.M12 = -value.M12; m.M13 = -value.M13; m.M14 = -value.M14;
            m.M21 = -value.M21; m.M22 = -value.M22; m.M23 = -value.M23; m.M24 = -value.M24;
            m.M31 = -value.M31; m.M32 = -value.M32; m.M33 = -value.M33; m.M34 = -value.M34;
            m.M41 = -value.M41; m.M42 = -value.M42; m.M43 = -value.M43; m.M44 = -value.M44;

            return m;
        }


        public static Matrix4x4 operator +(Matrix4x4 value1, Matrix4x4 value2)
        {
            Matrix4x4 m;

            m.M11 = value1.M11 + value2.M11; m.M12 = value1.M12 + value2.M12; m.M13 = value1.M13 + value2.M13; m.M14 = value1.M14 + value2.M14;
            m.M21 = value1.M21 + value2.M21; m.M22 = value1.M22 + value2.M22; m.M23 = value1.M23 + value2.M23; m.M24 = value1.M24 + value2.M24;
            m.M31 = value1.M31 + value2.M31; m.M32 = value1.M32 + value2.M32; m.M33 = value1.M33 + value2.M33; m.M34 = value1.M34 + value2.M34;
            m.M41 = value1.M41 + value2.M41; m.M42 = value1.M42 + value2.M42; m.M43 = value1.M43 + value2.M43; m.M44 = value1.M44 + value2.M44;

            return m;
        }


        public static Matrix4x4 operator -(Matrix4x4 value1, Matrix4x4 value2)
        {
            Matrix4x4 m;

            m.M11 = value1.M11 - value2.M11; m.M12 = value1.M12 - value2.M12; m.M13 = value1.M13 - value2.M13; m.M14 = value1.M14 - value2.M14;
            m.M21 = value1.M21 - value2.M21; m.M22 = value1.M22 - value2.M22; m.M23 = value1.M23 - value2.M23; m.M24 = value1.M24 - value2.M24;
            m.M31 = value1.M31 - value2.M31; m.M32 = value1.M32 - value2.M32; m.M33 = value1.M33 - value2.M33; m.M34 = value1.M34 - value2.M34;
            m.M41 = value1.M41 - value2.M41; m.M42 = value1.M42 - value2.M42; m.M43 = value1.M43 - value2.M43; m.M44 = value1.M44 - value2.M44;

            return m;
        }


        public static Matrix4x4 operator *(Matrix4x4 value1, Matrix4x4 value2)
        {
            Matrix4x4 m;

            // First row
            m.M11 = value1.M11 * value2.M11 + value1.M12 * value2.M21 + value1.M13 * value2.M31 + value1.M14 * value2.M41;
            m.M12 = value1.M11 * value2.M12 + value1.M12 * value2.M22 + value1.M13 * value2.M32 + value1.M14 * value2.M42;
            m.M13 = value1.M11 * value2.M13 + value1.M12 * value2.M23 + value1.M13 * value2.M33 + value1.M14 * value2.M43;
            m.M14 = value1.M11 * value2.M14 + value1.M12 * value2.M24 + value1.M13 * value2.M34 + value1.M14 * value2.M44;
            
            // Second row
            m.M21 = value1.M21 * value2.M11 + value1.M22 * value2.M21 + value1.M23 * value2.M31 + value1.M24 * value2.M41;
            m.M22 = value1.M21 * value2.M12 + value1.M22 * value2.M22 + value1.M23 * value2.M32 + value1.M24 * value2.M42;
            m.M23 = value1.M21 * value2.M13 + value1.M22 * value2.M23 + value1.M23 * value2.M33 + value1.M24 * value2.M43;
            m.M24 = value1.M21 * value2.M14 + value1.M22 * value2.M24 + value1.M23 * value2.M34 + value1.M24 * value2.M44;
            
            // Third row
            m.M31 = value1.M31 * value2.M11 + value1.M32 * value2.M21 + value1.M33 * value2.M31 + value1.M34 * value2.M41;
            m.M32 = value1.M31 * value2.M12 + value1.M32 * value2.M22 + value1.M33 * value2.M32 + value1.M34 * value2.M42;
            m.M33 = value1.M31 * value2.M13 + value1.M32 * value2.M23 + value1.M33 * value2.M33 + value1.M34 * value2.M43;
            m.M34 = value1.M31 * value2.M14 + value1.M32 * value2.M24 + value1.M33 * value2.M34 + value1.M34 * value2.M44;
            
            // Fourth row
            m.M41 = value1.M41 * value2.M11 + value1.M42 * value2.M21 + value1.M43 * value2.M31 + value1.M44 * value2.M41;
            m.M42 = value1.M41 * value2.M12 + value1.M42 * value2.M22 + value1.M43 * value2.M32 + value1.M44 * value2.M42;
            m.M43 = value1.M41 * value2.M13 + value1.M42 * value2.M23 + value1.M43 * value2.M33 + value1.M44 * value2.M43;
            m.M44 = value1.M41 * value2.M14 + value1.M42 * value2.M24 + value1.M43 * value2.M34 + value1.M44 * value2.M44;

            return m;
        }


        public static Matrix4x4 operator *(Matrix4x4 value1, float value2)
        {
            Matrix4x4 m;

            m.M11 = value1.M11 * value2; m.M12 = value1.M12 * value2; m.M13 = value1.M13 * value2; m.M14 = value1.M14 * value2;
            m.M21 = value1.M21 * value2; m.M22 = value1.M22 * value2; m.M23 = value1.M23 * value2; m.M24 = value1.M24 * value2;
            m.M31 = value1.M31 * value2; m.M32 = value1.M32 * value2; m.M33 = value1.M33 * value2; m.M34 = value1.M34 * value2;
            m.M41 = value1.M41 * value2; m.M42 = value1.M42 * value2; m.M43 = value1.M43 * value2; m.M44 = value1.M44 * value2;

            return m;
        }


        public static bool operator ==(Matrix4x4 value1, Matrix4x4 value2)
        {
            return (value1.M11 == value2.M11 && value1.M22 == value2.M22 && value1.M33 == value2.M33 && value1.M44 == value2.M44 && // Check diagonal element first for early out.
                                                value1.M12 == value2.M12 && value1.M13 == value2.M13 && value1.M14 == value2.M14 &&
                    value1.M21 == value2.M21                             && value1.M23 == value2.M23 && value1.M24 == value2.M24 &&
                    value1.M31 == value2.M31 && value1.M32 == value2.M32                             && value1.M34 == value2.M34 &&
                    value1.M41 == value2.M41 && value1.M42 == value2.M42 && value1.M43 == value2.M43);
        }


        public static bool operator !=(Matrix4x4 value1, Matrix4x4 value2)
        {
            return (value1.M11 != value2.M11 || value1.M12 != value2.M12 || value1.M13 != value2.M13 || value1.M14 != value2.M14 ||
                    value1.M21 != value2.M21 || value1.M22 != value2.M22 || value1.M23 != value2.M23 || value1.M24 != value2.M24 ||
                    value1.M31 != value2.M31 || value1.M32 != value2.M32 || value1.M33 != value2.M33 || value1.M34 != value2.M34 ||
                    value1.M41 != value2.M41 || value1.M42 != value2.M42 || value1.M43 != value2.M43 || value1.M44 != value2.M44);
        }


        public bool Equals(Matrix4x4 other)
        {
            return (M11 == other.M11 && M22 == other.M22 && M33 == other.M33 && M44 == other.M44 && // Check diagonal element first for early out.
                                        M12 == other.M12 && M13 == other.M13 && M14 == other.M14 &&
                    M21 == other.M21                     && M23 == other.M23 && M24 == other.M24 &&
                    M31 == other.M31 && M32 == other.M32                     && M34 == other.M34 &&
                    M41 == other.M41 && M42 == other.M42 && M43 == other.M43);
        }


        public override bool Equals(object obj)
        {
            if (obj is Matrix4x4)
            {
                return Equals((Matrix4x4)obj);
            }

            return false;
        }


        public override string ToString()
        {
            CultureInfo ci = CultureInfo.CurrentCulture;

            return String.Format(ci, "{{ {{M11:{0} M12:{1} M13:{2} M14:{3}}} {{M21:{4} M22:{5} M23:{6} M24:{7}}} {{M31:{8} M32:{9} M33:{10} M34:{11}}} {{M41:{12} M42:{13} M43:{14} M44:{15}}} }}",
                                 M11.ToString(ci), M12.ToString(ci), M13.ToString(ci), M14.ToString(ci),
                                 M21.ToString(ci), M22.ToString(ci), M23.ToString(ci), M24.ToString(ci),
                                 M31.ToString(ci), M32.ToString(ci), M33.ToString(ci), M34.ToString(ci),
                                 M41.ToString(ci), M42.ToString(ci), M43.ToString(ci), M44.ToString(ci));
        }

        
        public override int GetHashCode()
        {
            return M11.GetHashCode() + M12.GetHashCode() + M13.GetHashCode() + M14.GetHashCode() +
                   M21.GetHashCode() + M22.GetHashCode() + M23.GetHashCode() + M24.GetHashCode() +
                   M31.GetHashCode() + M32.GetHashCode() + M33.GetHashCode() + M34.GetHashCode() +
                   M41.GetHashCode() + M42.GetHashCode() + M43.GetHashCode() + M44.GetHashCode();
        }


#if INCLUDE_WINRT_CANVAS_INTEROP

        public static unsafe implicit operator Microsoft.Graphics.Canvas.Numerics.Matrix4x4(Matrix4x4 value)
        {
            return *(Microsoft.Graphics.Canvas.Numerics.Matrix4x4*)&value;
        }


        public static unsafe implicit operator Matrix4x4(Microsoft.Graphics.Canvas.Numerics.Matrix4x4 value)
        {
            return *(Matrix4x4*)&value;
        }

#endif
    }
}
