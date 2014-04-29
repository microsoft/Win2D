
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//

#pragma once

#include <DirectXMath.h>

namespace Windows
{
    namespace Math
    {
        struct Vector2;
        struct Vector3;
        struct Vector4;
        struct Matrix3x2;
        struct Matrix4x4;
        struct Plane;
        struct Quaternion;


        struct Vector2
        {
            float X, Y;

            // Constructors.
            Vector2();
            Vector2(float x, float y);
            explicit Vector2(float value);
            Vector2(Windows::Foundation::Point const& value);
            Vector2(Windows::Foundation::Size const& value);

            // Conversion operators.
            operator Windows::Foundation::Point() const;
            operator Windows::Foundation::Size() const;

            // Common values.
            static Vector2 Zero();
            static Vector2 One();
            static Vector2 UnitX();
            static Vector2 UnitY();

            // Instance methods.
            float Length() const;
            float LengthSquared() const;

            // Static methods.
            static float Distance(Vector2 const& value1, Vector2 const& value2);
            static float DistanceSquared(Vector2 const& value1, Vector2 const& value2);
            static float Dot(Vector2 const& value1, Vector2 const& value2);
            static Vector2 Normalize(Vector2 const& value);
            static Vector2 Reflect(Vector2 const& vector, Vector2 const& normal);
            static Vector2 Min(Vector2 const& value1, Vector2 const& value2);
            static Vector2 Max(Vector2 const& value1, Vector2 const& value2);
            static Vector2 Clamp(Vector2 const& value1, Vector2 const& min, Vector2 const& max);
            static Vector2 Lerp(Vector2 const& value1, Vector2 const& value2, float amount);
            static Vector2 Transform(Vector2 const& position, Matrix3x2 const& matrix);
            static Vector2 Transform(Vector2 const& position, Matrix4x4 const& matrix);
            static Vector2 TransformNormal(Vector2 const& normal, Matrix3x2 const& matrix);
            static Vector2 TransformNormal(Vector2 const& normal, Matrix4x4 const& matrix);
            static Vector2 Transform(Vector2 const& value, Quaternion const& rotation);
            static Vector2 Negate(Vector2 const& value);
            static Vector2 Add(Vector2 const& value1, Vector2 const& value2);
            static Vector2 Subtract(Vector2 const& value1, Vector2 const& value2);
            static Vector2 Multiply(Vector2 const& value1, Vector2 const& value2);
            static Vector2 Multiply(Vector2 const& value1, float value2);
            static Vector2 Divide(Vector2 const& value1, Vector2 const& value2);
            static Vector2 Divide(Vector2 const& value1, float value2);

            // Operator overloads.
            Vector2 operator -() const;
            Vector2& operator +=(Vector2 const& value);
            Vector2& operator -=(Vector2 const& value);
            Vector2& operator *=(Vector2 const& value);
            Vector2& operator *=(float value);
            Vector2& operator /=(Vector2 const& value);
            Vector2& operator /=(float value);
            bool operator ==(Vector2 const& value) const;
            bool operator !=(Vector2 const& value) const;
        };

        // Binary operators.
        Vector2 operator +(Vector2 const& value1, Vector2 const& value2);
        Vector2 operator -(Vector2 const& value1, Vector2 const& value2);
        Vector2 operator *(Vector2 const& value1, Vector2 const& value2);
        Vector2 operator *(Vector2 const& value1, float value2);
        Vector2 operator *(float value1, Vector2 const& value2);
        Vector2 operator /(Vector2 const& value1, Vector2 const& value2);
        Vector2 operator /(Vector2 const& value1, float value2);


        struct Vector3
        {
            float X, Y, Z;

            // Constructors.
            Vector3();
            Vector3(float x, float y, float z);
            Vector3(Vector2 const& value, float z);
            explicit Vector3(float value);

            // Common values.
            static Vector3 Zero();
            static Vector3 One();
            static Vector3 UnitX();
            static Vector3 UnitY();
            static Vector3 UnitZ();

            // Instance methods.
            float Length() const;
            float LengthSquared() const;

            // Static methods.
            static float Distance(Vector3 const& value1, Vector3 const& value2);
            static float DistanceSquared(Vector3 const& value1, Vector3 const& value2);
            static float Dot(Vector3 const& vector1, Vector3 const& vector2);
            static Vector3 Normalize(Vector3 const& value);
            static Vector3 Cross(Vector3 const& vector1, Vector3 const& vector2);
            static Vector3 Reflect(Vector3 const& vector, Vector3 const& normal);
            static Vector3 Min(Vector3 const& value1, Vector3 const& value2);
            static Vector3 Max(Vector3 const& value1, Vector3 const& value2);
            static Vector3 Clamp(Vector3 const& value1, Vector3 const& min, Vector3 const& max);
            static Vector3 Lerp(Vector3 const& value1, Vector3 const& value2, float amount);
            static Vector3 Transform(Vector3 const& position, Matrix4x4 const& matrix);
            static Vector3 TransformNormal(Vector3 const& normal, Matrix4x4 const& matrix);
            static Vector3 Transform(Vector3 const& value, Quaternion const& rotation);
            static Vector3 Negate(Vector3 const& value);
            static Vector3 Add(Vector3 const& value1, Vector3 const& value2);
            static Vector3 Subtract(Vector3 const& value1, Vector3 const& value2);
            static Vector3 Multiply(Vector3 const& value1, Vector3 const& value2);
            static Vector3 Multiply(Vector3 const& value1, float value2);
            static Vector3 Divide(Vector3 const& value1, Vector3 const& value2);
            static Vector3 Divide(Vector3 const& value1, float value2);

            // Operator overloads.
            Vector3 operator -() const;
            Vector3& operator +=(Vector3 const& value);
            Vector3& operator -=(Vector3 const& value);
            Vector3& operator *=(Vector3 const& value);
            Vector3& operator *=(float value);
            Vector3& operator /=(Vector3 const& value);
            Vector3& operator /=(float value);
            bool operator ==(Vector3 const& value) const;
            bool operator !=(Vector3 const& value) const;
        };

        // Binary operators.
        Vector3 operator +(Vector3 const& value1, Vector3 const& value2);
        Vector3 operator -(Vector3 const& value1, Vector3 const& value2);
        Vector3 operator *(Vector3 const& value1, Vector3 const& value2);
        Vector3 operator *(Vector3 const& value1, float value2);
        Vector3 operator *(float value1, Vector3 const& value2);
        Vector3 operator /(Vector3 const& value1, Vector3 const& value2);
        Vector3 operator /(Vector3 const& value1, float value2);


        struct Vector4
        {
            float X, Y, Z, W;

            // Constructors.
            Vector4();
            Vector4(float x, float y, float z, float w);
            Vector4(Vector2 const& value, float z, float w);
            Vector4(Vector3 const& value, float w);
            explicit Vector4(float value);

            // Common values.
            static Vector4 Zero();
            static Vector4 One();
            static Vector4 UnitX();
            static Vector4 UnitY();
            static Vector4 UnitZ();
            static Vector4 UnitW();

            // Instance methods.
            float Length() const;
            float LengthSquared() const;

            // Static methods.
            static float Distance(Vector4 const& value1, Vector4 const& value2);
            static float DistanceSquared(Vector4 const& value1, Vector4 const& value2);
            static float Dot(Vector4 const& vector1, Vector4 const& vector2);
            static Vector4 Normalize(Vector4 const& vector);
            static Vector4 Min(Vector4 const& value1, Vector4 const& value2);
            static Vector4 Max(Vector4 const& value1, Vector4 const& value2);
            static Vector4 Clamp(Vector4 const& value1, Vector4 const& min, Vector4 const& max);
            static Vector4 Lerp(Vector4 const& value1, Vector4 const& value2, float amount);
            static Vector4 Transform(Vector2 const& position, Matrix4x4 const& matrix);
            static Vector4 Transform(Vector3 const& position, Matrix4x4 const& matrix);
            static Vector4 Transform(Vector4 const& vector, Matrix4x4 const& matrix);
            static Vector4 Transform(Vector2 const& value, Quaternion const& rotation);
            static Vector4 Transform(Vector3 const& value, Quaternion const& rotation);
            static Vector4 Transform(Vector4 const& value, Quaternion const& rotation);
            static Vector4 Negate(Vector4 const& value);
            static Vector4 Add(Vector4 const& value1, Vector4 const& value2);
            static Vector4 Subtract(Vector4 const& value1, Vector4 const& value2);
            static Vector4 Multiply(Vector4 const& value1, Vector4 const& value2);
            static Vector4 Multiply(Vector4 const& value1, float value2);
            static Vector4 Divide(Vector4 const& value1, Vector4 const& value2);
            static Vector4 Divide(Vector4 const& value1, float value2);

            // Operator overloads.
            Vector4 operator -() const;
            Vector4& operator +=(Vector4 const& value);
            Vector4& operator -=(Vector4 const& value);
            Vector4& operator *=(Vector4 const& value);
            Vector4& operator *=(float value);
            Vector4& operator /=(Vector4 const& value);
            Vector4& operator /=(float value);
            bool operator ==(Vector4 const& value) const;
            bool operator !=(Vector4 const& value) const;
        };

        // Binary operators.
        Vector4 operator +(Vector4 const& value1, Vector4 const& value2);
        Vector4 operator -(Vector4 const& value1, Vector4 const& value2);
        Vector4 operator *(Vector4 const& value1, Vector4 const& value2);
        Vector4 operator *(Vector4 const& value1, float value2);
        Vector4 operator *(float value1, Vector4 const& value2);
        Vector4 operator /(Vector4 const& value1, Vector4 const& value2);
        Vector4 operator /(Vector4 const& value1, float value2);


        struct Matrix3x2
        {
            float M11, M12;
            float M21, M22;
            float M31, M32;

            // Constructors.
            Matrix3x2();
            Matrix3x2(float m11, float m12, float m21, float m22, float m31, float m32);

            // Common values.
            static Matrix3x2 Identity();

            // Instance methods.
            bool IsIdentity() const;
            float Determinant() const;
            Vector2 Translation() const;
            void SetTranslation(Vector2 const& translation);

            // Static methods.
            static Matrix3x2 CreateTranslation(Vector2 const& position);
            static Matrix3x2 CreateTranslation(float xPosition, float yPosition);
            static Matrix3x2 CreateScale(float xScale, float yScale);
            static Matrix3x2 CreateScale(float xScale, float yScale, Vector2 const& centerPoint);
            static Matrix3x2 CreateScale(Vector2 const& scales);
            static Matrix3x2 CreateScale(Vector2 const& scales, Vector2 const& centerPoint);
            static Matrix3x2 CreateScale(float scale);
            static Matrix3x2 CreateScale(float scale, Vector2 const& centerPoint);
            static Matrix3x2 CreateSkew(float radiansX, float radiansY);
            static Matrix3x2 CreateSkew(float radiansX, float radiansY, Vector2 const& centerPoint);
            static Matrix3x2 CreateRotation(float radians);
            static Matrix3x2 CreateRotation(float radians, Vector2 const& centerPoint);
            static bool Invert(Matrix3x2 const& matrix, _Out_ Matrix3x2* result);
            static Matrix3x2 Lerp(Matrix3x2 const& matrix1, Matrix3x2 const& matrix2, float amount);
            static Matrix3x2 Negate(Matrix3x2 const& value);
            static Matrix3x2 Add(Matrix3x2 const& value1, Matrix3x2 const& value2);
            static Matrix3x2 Subtract(Matrix3x2 const& value1, Matrix3x2 const& value2);
            static Matrix3x2 Multiply(Matrix3x2 const& value1, Matrix3x2 const& value2);
            static Matrix3x2 Multiply(Matrix3x2 const& value1, float value2);

            // Operator overloads.
            Matrix3x2 operator -() const;
            Matrix3x2& operator +=(Matrix3x2 const& value);
            Matrix3x2& operator -=(Matrix3x2 const& value);
            Matrix3x2& operator *=(Matrix3x2 const& value);
            Matrix3x2& operator *=(float value);
            bool operator ==(Matrix3x2 const& value) const;
            bool operator !=(Matrix3x2 const& value) const;
        };

        // Binary operators.
        Matrix3x2 operator +(Matrix3x2 const& value1, Matrix3x2 const& value2);
        Matrix3x2 operator -(Matrix3x2 const& value1, Matrix3x2 const& value2);
        Matrix3x2 operator *(Matrix3x2 const& value1, Matrix3x2 const& value2);
        Matrix3x2 operator *(Matrix3x2 const& value1, float value2);


        struct Matrix4x4
        {
            float M11, M12, M13, M14;
            float M21, M22, M23, M24;
            float M31, M32, M33, M34;
            float M41, M42, M43, M44;

            // Constructors.
            Matrix4x4();
            Matrix4x4(float m11, float m12, float m13, float m14, float m21, float m22, float m23, float m24, float m31, float m32, float m33, float m34, float m41, float m42, float m43, float m44);
            explicit Matrix4x4(Matrix3x2 const& value);

            // Common values.
            static Matrix4x4 Identity();

            // Instance methods.
            bool IsIdentity() const;
            float Determinant() const;
            Vector3 Translation() const;
            void SetTranslation(Vector3 const& translation);

            // Static methods.
            static Matrix4x4 CreateBillboard(Vector3 const& objectPosition, Vector3 const& cameraPosition, Vector3 const& cameraUpVector, Vector3 const& cameraForwardVector);
            static Matrix4x4 CreateConstrainedBillboard(Vector3 const& objectPosition, Vector3 const& cameraPosition, Vector3 const& rotateAxis, Vector3 const& cameraForwardVector, Vector3 const& objectForwardVector);
            static Matrix4x4 CreateTranslation(Vector3 const& position);
            static Matrix4x4 CreateTranslation(float xPosition, float yPosition, float zPosition);
            static Matrix4x4 CreateScale(float xScale, float yScale, float zScale);
            static Matrix4x4 CreateScale(float xScale, float yScale, float zScale, Vector3 const& centerPoint);
            static Matrix4x4 CreateScale(Vector3 const& scales);
            static Matrix4x4 CreateScale(Vector3 const& scales, Vector3 const& centerPoint);
            static Matrix4x4 CreateScale(float scale);
            static Matrix4x4 CreateScale(float scale, Vector3 const& centerPoint);
            static Matrix4x4 CreateRotationX(float radians);
            static Matrix4x4 CreateRotationX(float radians, Vector3 const& centerPoint);
            static Matrix4x4 CreateRotationY(float radians);
            static Matrix4x4 CreateRotationY(float radians, Vector3 const& centerPoint);
            static Matrix4x4 CreateRotationZ(float radians);
            static Matrix4x4 CreateRotationZ(float radians, Vector3 const& centerPoint);
            static Matrix4x4 CreateFromAxisAngle(Vector3 const& axis, float angle);
            static Matrix4x4 CreatePerspectiveFieldOfView(float fieldOfView, float aspectRatio, float nearPlaneDistance, float farPlaneDistance);
            static Matrix4x4 CreatePerspective(float width, float height, float nearPlaneDistance, float farPlaneDistance);
            static Matrix4x4 CreatePerspectiveOffCenter(float left, float right, float bottom, float top, float nearPlaneDistance, float farPlaneDistance);
            static Matrix4x4 CreateOrthographic(float width, float height, float zNearPlane, float zFarPlane);
            static Matrix4x4 CreateOrthographicOffCenter(float left, float right, float bottom, float top, float zNearPlane, float zFarPlane);
            static Matrix4x4 CreateLookAt(Vector3 const& cameraPosition, Vector3 const& cameraTarget, Vector3 const& cameraUpVector);
            static Matrix4x4 CreateWorld(Vector3 const& position, Vector3 const& forward, Vector3 const& up);
            static Matrix4x4 CreateFromQuaternion(Quaternion const& quaternion);
            static Matrix4x4 CreateFromYawPitchRoll(float yaw, float pitch, float roll);
            static Matrix4x4 CreateShadow(Vector3 const& lightDirection, Plane const& plane);
            static Matrix4x4 CreateReflection(Plane const& value);
            static bool Invert(Matrix4x4 const& matrix, _Out_ Matrix4x4* result);
            static bool Decompose(Matrix4x4 const& matrix, _Out_ Vector3* scale, _Out_ Quaternion* rotation, _Out_ Vector3* translation);
            static Matrix4x4 Transform(Matrix4x4 const& value, Quaternion const& rotation);
            static Matrix4x4 Transpose(Matrix4x4 const& matrix);
            static Matrix4x4 Lerp(Matrix4x4 const& matrix1, Matrix4x4 const& matrix2, float amount);
            static Matrix4x4 Negate(Matrix4x4 const& value);
            static Matrix4x4 Add(Matrix4x4 const& value1, Matrix4x4 const& value2);
            static Matrix4x4 Subtract(Matrix4x4 const& value1, Matrix4x4 const& value2);
            static Matrix4x4 Multiply(Matrix4x4 const& value1, Matrix4x4 const& value2);
            static Matrix4x4 Multiply(Matrix4x4 const& value1, float value2);

            // Operator overloads.
            Matrix4x4 operator -() const;
            Matrix4x4& operator +=(Matrix4x4 const& value);
            Matrix4x4& operator -=(Matrix4x4 const& value);
            Matrix4x4& operator *=(Matrix4x4 const& value);
            Matrix4x4& operator *=(float value);
            bool operator ==(Matrix4x4 const& value) const;
            bool operator !=(Matrix4x4 const& value) const;
        };

        // Binary operators.
        Matrix4x4 operator +(Matrix4x4 const& value1, Matrix4x4 const& value2);
        Matrix4x4 operator -(Matrix4x4 const& value1, Matrix4x4 const& value2);
        Matrix4x4 operator *(Matrix4x4 const& value1, Matrix4x4 const& value2);
        Matrix4x4 operator *(Matrix4x4 const& value1, float value2);


        struct Plane
        {
            Vector3 Normal;
            float D;

            // Constructors.
            Plane();
            Plane(float a, float b, float c, float d);
            Plane(Vector3 const& normal, float d);
            explicit Plane(Vector4 const& value);
            Plane(Vector3 const& point1, Vector3 const& point2, Vector3 const& point3);

            // Static methods.
            static Plane Normalize(Plane const& value);
            static Plane Transform(Plane const& plane, Matrix4x4 const& matrix);
            static Plane Transform(Plane const& plane, Quaternion const& rotation);
            static float Dot(Plane const& plane, Vector4 const& value);
            static float DotCoordinate(Plane const& plane, Vector3 const& value);
            static float DotNormal(Plane const& plane, Vector3 const& value);

            // Operator overloads.
            bool operator ==(Plane const& value) const;
            bool operator !=(Plane const& value) const;
        };


        struct Quaternion
        {
            float X, Y, Z, W;

            // Constructors.
            Quaternion();
            Quaternion(float x, float y, float z, float w);
            Quaternion(Vector3 const& vectorPart, float scalarPart);

            // Common values.
            static Quaternion Identity();

            // Instance methods.
            bool IsIdentity() const;
            float Length() const;
            float LengthSquared() const;

            // Static methods.
            static Quaternion Normalize(Quaternion const& value);
            static Quaternion Conjugate(Quaternion const& value);
            static Quaternion Inverse(Quaternion const& value);
            static Quaternion CreateFromAxisAngle(Vector3 const& axis, float angle);
            static Quaternion CreateFromYawPitchRoll(float yaw, float pitch, float roll);
            static Quaternion CreateFromRotationMatrix(Matrix4x4 const& matrix);
            static float Dot(Quaternion const& quaternion1, Quaternion const& quaternion2);
            static Quaternion Slerp(Quaternion const& quaternion1, Quaternion const& quaternion2, float amount);
            static Quaternion Lerp(Quaternion const& quaternion1, Quaternion const& quaternion2, float amount);
            static Quaternion Concatenate(Quaternion const& value1, Quaternion const& value2);
            static Quaternion Negate(Quaternion const& value);
            static Quaternion Add(Quaternion const& value1, Quaternion const& value2);
            static Quaternion Subtract(Quaternion const& value1, Quaternion const& value2);
            static Quaternion Multiply(Quaternion const& value1, Quaternion const& value2);
            static Quaternion Multiply(Quaternion const& value1, float value2);
            static Quaternion Divide(Quaternion const& value1, Quaternion const& value2);

            // Operator overloads.
            Quaternion operator -() const;
            Quaternion& operator +=(Quaternion const& value);
            Quaternion& operator -=(Quaternion const& value);
            Quaternion& operator *=(Quaternion const& value);
            Quaternion& operator *=(float value);
            Quaternion& operator /=(Quaternion const& value);
            bool operator ==(Quaternion const& value) const;
            bool operator !=(Quaternion const& value) const;
        };

        // Binary operators.
        Quaternion operator +(Quaternion const& value1, Quaternion const& value2);
        Quaternion operator -(Quaternion const& value1, Quaternion const& value2);
        Quaternion operator *(Quaternion const& value1, Quaternion const& value2);
        Quaternion operator *(Quaternion const& value1, float value2);
        Quaternion operator /(Quaternion const& value1, Quaternion const& value2);
    }
}

#include "WindowsMath.inl"
