//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//

#pragma once

#include <DirectXMath.h>
#include <stdexcept>


namespace Windows
{
    namespace Math
    {
        struct float2;
        struct float3;
        struct float4;
        struct float3x2;
        struct float4x4;
        struct plane;
        struct quaternion;


        struct float2
        {
            float x, y;

            // Constructors.
            float2() = default;
            float2(float x, float y);
            explicit float2(float value);

            // Conversion operators.
#ifdef __cplusplus_winrt
            float2(Windows::Foundation::Point const& value);
            float2(Windows::Foundation::Size const& value);

            operator Windows::Foundation::Point() const;
            operator Windows::Foundation::Size() const;
#endif

            // Common values.
            static float2 zero();
            static float2 one();
            static float2 unit_x();
            static float2 unit_y();
        };

        // Operators.
        float2 operator +(float2 const& value1, float2 const& value2);
        float2 operator -(float2 const& value1, float2 const& value2);
        float2 operator *(float2 const& value1, float2 const& value2);
        float2 operator *(float2 const& value1, float value2);
        float2 operator *(float value1, float2 const& value2);
        float2 operator /(float2 const& value1, float2 const& value2);
        float2 operator /(float2 const& value1, float value2);
        float2 operator -(float2 const& value);
        float2& operator +=(float2& value1, float2 const& value2);
        float2& operator -=(float2& value1, float2 const& value2);
        float2& operator *=(float2& value1, float2 const& value2);
        float2& operator *=(float2& value1, float value2);
        float2& operator /=(float2& value1, float2 const& value2);
        float2& operator /=(float2& value1, float value2);
        bool operator ==(float2 const& value1, float2 const& value2);
        bool operator !=(float2 const& value1, float2 const& value2);

        // Functions.
        float length(float2 const& value);
        float length_squared(float2 const& value);
        float distance(float2 const& value1, float2 const& value2);
        float distance_squared(float2 const& value1, float2 const& value2);
        float dot(float2 const& value1, float2 const& value2);
        float2 normalize(float2 const& value);
        float2 reflect(float2 const& vector, float2 const& normal);
        float2 min(float2 const& value1, float2 const& value2);
        float2 max(float2 const& value1, float2 const& value2);
        float2 clamp(float2 const& value1, float2 const& min, float2 const& max);
        float2 lerp(float2 const& value1, float2 const& value2, float amount);
        float2 transform(float2 const& position, float3x2 const& matrix);
        float2 transform(float2 const& position, float4x4 const& matrix);
        float2 transform_normal(float2 const& normal, float3x2 const& matrix);
        float2 transform_normal(float2 const& normal, float4x4 const& matrix);
        float2 transform(float2 const& value, quaternion const& rotation);


        struct float3
        {
            float x, y, z;

            // Constructors.
            float3() = default;
            float3(float x, float y, float z);
            float3(float2 const& value, float z);
            explicit float3(float value);

            // Common values.
            static float3 zero();
            static float3 one();
            static float3 unit_x();
            static float3 unit_y();
            static float3 unit_z();
        };

        // Operators.
        float3 operator +(float3 const& value1, float3 const& value2);
        float3 operator -(float3 const& value1, float3 const& value2);
        float3 operator *(float3 const& value1, float3 const& value2);
        float3 operator *(float3 const& value1, float value2);
        float3 operator *(float value1, float3 const& value2);
        float3 operator /(float3 const& value1, float3 const& value2);
        float3 operator /(float3 const& value1, float value2);
        float3 operator -(float3 const& value);
        float3& operator +=(float3& value1, float3 const& value2);
        float3& operator -=(float3& value1, float3 const& value2);
        float3& operator *=(float3& value1, float3 const& value2);
        float3& operator *=(float3& value1, float value2);
        float3& operator /=(float3& value1, float3 const& value2);
        float3& operator /=(float3& value1, float value2);
        bool operator ==(float3 const& value1, float3 const& value2);
        bool operator !=(float3 const& value1, float3 const& value2);

        // Functions.
        float length(float3 const& value);
        float length_squared(float3 const& value);
        float distance(float3 const& value1, float3 const& value2);
        float distance_squared(float3 const& value1, float3 const& value2);
        float dot(float3 const& vector1, float3 const& vector2);
        float3 normalize(float3 const& value);
        float3 cross(float3 const& vector1, float3 const& vector2);
        float3 reflect(float3 const& vector, float3 const& normal);
        float3 min(float3 const& value1, float3 const& value2);
        float3 max(float3 const& value1, float3 const& value2);
        float3 clamp(float3 const& value1, float3 const& min, float3 const& max);
        float3 lerp(float3 const& value1, float3 const& value2, float amount);
        float3 transform(float3 const& position, float4x4 const& matrix);
        float3 transform_normal(float3 const& normal, float4x4 const& matrix);
        float3 transform(float3 const& value, quaternion const& rotation);


        struct float4
        {
            float x, y, z, w;

            // Constructors.
            float4() = default;
            float4(float x, float y, float z, float w);
            float4(float2 const& value, float z, float w);
            float4(float3 const& value, float w);
            explicit float4(float value);

            // Common values.
            static float4 zero();
            static float4 one();
            static float4 unit_x();
            static float4 unit_y();
            static float4 unit_z();
            static float4 unit_w();
        };

        // Operators.
        float4 operator +(float4 const& value1, float4 const& value2);
        float4 operator -(float4 const& value1, float4 const& value2);
        float4 operator *(float4 const& value1, float4 const& value2);
        float4 operator *(float4 const& value1, float value2);
        float4 operator *(float value1, float4 const& value2);
        float4 operator /(float4 const& value1, float4 const& value2);
        float4 operator /(float4 const& value1, float value2);
        float4 operator -(float4 const& value);
        float4& operator +=(float4& value1, float4 const& value2);
        float4& operator -=(float4& value1, float4 const& value2);
        float4& operator *=(float4& value1, float4 const& value2);
        float4& operator *=(float4& value1, float value2);
        float4& operator /=(float4& value1, float4 const& value2);
        float4& operator /=(float4& value1, float value2);
        bool operator ==(float4 const& value1, float4 const& value2);
        bool operator !=(float4 const& value1, float4 const& value2);

        // Functions.
        float length(float4 const& value);
        float length_squared(float4 const& value);
        float distance(float4 const& value1, float4 const& value2);
        float distance_squared(float4 const& value1, float4 const& value2);
        float dot(float4 const& vector1, float4 const& vector2);
        float4 normalize(float4 const& vector);
        float4 min(float4 const& value1, float4 const& value2);
        float4 max(float4 const& value1, float4 const& value2);
        float4 clamp(float4 const& value1, float4 const& min, float4 const& max);
        float4 lerp(float4 const& value1, float4 const& value2, float amount);
        float4 transform(float4 const& vector, float4x4 const& matrix);
        float4 transform4(float3 const& position, float4x4 const& matrix);
        float4 transform4(float2 const& position, float4x4 const& matrix);
        float4 transform(float4 const& value, quaternion const& rotation);
        float4 transform4(float3 const& value, quaternion const& rotation);
        float4 transform4(float2 const& value, quaternion const& rotation);


        struct float3x2
        {
            float m11, m12;
            float m21, m22;
            float m31, m32;

            // Constructors.
            float3x2() = default;
            float3x2(float m11, float m12, float m21, float m22, float m31, float m32);

            // Common values.
            static float3x2 identity();

            // Factory functions.
            static float3x2 translation(float2 const& position);
            static float3x2 translation(float xPosition, float yPosition);
            static float3x2 scale(float xScale, float yScale);
            static float3x2 scale(float xScale, float yScale, float2 const& centerPoint);
            static float3x2 scale(float2 const& scales);
            static float3x2 scale(float2 const& scales, float2 const& centerPoint);
            static float3x2 scale(float scale);
            static float3x2 scale(float scale, float2 const& centerPoint);
            static float3x2 skew(float radiansX, float radiansY);
            static float3x2 skew(float radiansX, float radiansY, float2 const& centerPoint);
            static float3x2 rotation(float radians);
            static float3x2 rotation(float radians, float2 const& centerPoint);
        };

        // Operators.
        float3x2 operator +(float3x2 const& value1, float3x2 const& value2);
        float3x2 operator -(float3x2 const& value1, float3x2 const& value2);
        float3x2 operator *(float3x2 const& value1, float3x2 const& value2);
        float3x2 operator *(float3x2 const& value1, float value2);
        float3x2 operator -(float3x2 const& value);
        float3x2& operator +=(float3x2& value1, float3x2 const& value2);
        float3x2& operator -=(float3x2& value1, float3x2 const& value2);
        float3x2& operator *=(float3x2& value1, float3x2 const& value2);
        float3x2& operator *=(float3x2& value1, float value2);
        bool operator ==(float3x2 const& value1, float3x2 const& value2);
        bool operator !=(float3x2 const& value1, float3x2 const& value2);

        // Functions.
        bool is_identity(float3x2 const& value);
        float determinant(float3x2 const& value);
        float2 translation(float3x2 const& value);
        bool invert(float3x2 const& matrix, _Out_ float3x2* result);
        float3x2 lerp(float3x2 const& matrix1, float3x2 const& matrix2, float amount);


        struct float4x4
        {
            float m11, m12, m13, m14;
            float m21, m22, m23, m24;
            float m31, m32, m33, m34;
            float m41, m42, m43, m44;

            // Constructors.
            float4x4() = default;
            float4x4(float m11, float m12, float m13, float m14, float m21, float m22, float m23, float m24, float m31, float m32, float m33, float m34, float m41, float m42, float m43, float m44);
            explicit float4x4(float3x2 const& value);

            // Common values.
            static float4x4 identity();

            // Factory functions.
            static float4x4 billboard(float3 const& objectPosition, float3 const& cameraPosition, float3 const& cameraUpVector, float3 const& cameraForwardVector);
            static float4x4 constrained_billboard(float3 const& objectPosition, float3 const& cameraPosition, float3 const& rotateAxis, float3 const& cameraForwardVector, float3 const& objectForwardVector);
            static float4x4 translation(float3 const& position);
            static float4x4 translation(float xPosition, float yPosition, float zPosition);
            static float4x4 scale(float xScale, float yScale, float zScale);
            static float4x4 scale(float xScale, float yScale, float zScale, float3 const& centerPoint);
            static float4x4 scale(float3 const& scales);
            static float4x4 scale(float3 const& scales, float3 const& centerPoint);
            static float4x4 scale(float scale);
            static float4x4 scale(float scale, float3 const& centerPoint);
            static float4x4 rotation_x(float radians);
            static float4x4 rotation_x(float radians, float3 const& centerPoint);
            static float4x4 rotation_y(float radians);
            static float4x4 rotation_y(float radians, float3 const& centerPoint);
            static float4x4 rotation_z(float radians);
            static float4x4 rotation_z(float radians, float3 const& centerPoint);
            static float4x4 from_axis_angle(float3 const& axis, float angle);
            static float4x4 perspective_field_of_view(float fieldOfView, float aspectRatio, float nearPlaneDistance, float farPlaneDistance);
            static float4x4 perspective(float width, float height, float nearPlaneDistance, float farPlaneDistance);
            static float4x4 perspective_off_center(float left, float right, float bottom, float top, float nearPlaneDistance, float farPlaneDistance);
            static float4x4 orthographic(float width, float height, float zNearPlane, float zFarPlane);
            static float4x4 orthographic_off_center(float left, float right, float bottom, float top, float zNearPlane, float zFarPlane);
            static float4x4 look_at(float3 const& cameraPosition, float3 const& cameraTarget, float3 const& cameraUpVector);
            static float4x4 world(float3 const& position, float3 const& forward, float3 const& up);
            static float4x4 from_quaternion(quaternion const& quaternion);
            static float4x4 from_yaw_pitch_roll(float yaw, float pitch, float roll);
            static float4x4 shadow(float3 const& lightDirection, plane const& plane);
            static float4x4 reflection(plane const& value);
        };

        // Operators.
        float4x4 operator +(float4x4 const& value1, float4x4 const& value2);
        float4x4 operator -(float4x4 const& value1, float4x4 const& value2);
        float4x4 operator *(float4x4 const& value1, float4x4 const& value2);
        float4x4 operator *(float4x4 const& value1, float value2);
        float4x4 operator -(float4x4 const& value);
        float4x4& operator +=(float4x4& value1, float4x4 const& value2);
        float4x4& operator -=(float4x4& value1, float4x4 const& value2);
        float4x4& operator *=(float4x4& value1, float4x4 const& value2);
        float4x4& operator *=(float4x4& value1, float value2);
        bool operator ==(float4x4 const& value1, float4x4 const& value2);
        bool operator !=(float4x4 const& value1, float4x4 const& value2);

        // Functions.
        bool is_identity(float4x4 const& value);
        float determinant(float4x4 const& value);
        float3 translation(float4x4 const& value);
        bool invert(float4x4 const& matrix, _Out_ float4x4* result);
        bool decompose(float4x4 const& matrix, _Out_ float3* scale, _Out_ quaternion* rotation, _Out_ float3* translation);
        float4x4 transform(float4x4 const& value, quaternion const& rotation);
        float4x4 transpose(float4x4 const& matrix);
        float4x4 lerp(float4x4 const& matrix1, float4x4 const& matrix2, float amount);


        struct plane
        {
            float3 normal;
            float d;

            // Constructors.
            plane() = default;
            plane(float a, float b, float c, float d);
            plane(float3 const& normal, float d);
            explicit plane(float4 const& value);

            // Factory functions.
            static plane from_vertices(float3 const& point1, float3 const& point2, float3 const& point3);
        };

        // Operators.
        bool operator ==(plane const& value1, plane const& value2);
        bool operator !=(plane const& value1, plane const& value2);

        // Functions.
        plane normalize(plane const& value);
        plane transform(plane const& plane, float4x4 const& matrix);
        plane transform(plane const& plane, quaternion const& rotation);
        float dot(plane const& plane, float4 const& value);
        float dot_coordinate(plane const& plane, float3 const& value);
        float dot_normal(plane const& plane, float3 const& value);


        struct quaternion
        {
            float x, y, z, w;

            // Constructors.
            quaternion() = default;
            quaternion(float x, float y, float z, float w);
            quaternion(float3 const& vectorPart, float scalarPart);

            // Common values.
            static quaternion identity();

            // Factory functions.
            static quaternion from_axis_angle(float3 const& axis, float angle);
            static quaternion from_yaw_pitch_roll(float yaw, float pitch, float roll);
            static quaternion from_rotation_matrix(float4x4 const& matrix);
        };

        // Operators.
        quaternion operator +(quaternion const& value1, quaternion const& value2);
        quaternion operator -(quaternion const& value1, quaternion const& value2);
        quaternion operator *(quaternion const& value1, quaternion const& value2);
        quaternion operator *(quaternion const& value1, float value2);
        quaternion operator /(quaternion const& value1, quaternion const& value2);
        quaternion operator -(quaternion const& value);
        quaternion& operator +=(quaternion& value1, quaternion const& value2);
        quaternion& operator -=(quaternion& value1, quaternion const& value2);
        quaternion& operator *=(quaternion& value1, quaternion const& value2);
        quaternion& operator *=(quaternion& value1, float value2);
        quaternion& operator /=(quaternion& value1, quaternion const& value2);
        bool operator ==(quaternion const& value1, quaternion const& value2);
        bool operator !=(quaternion const& value1, quaternion const& value2);

        // Functions.
        bool is_identity(quaternion const& value);
        float length(quaternion const& value);
        float length_squared(quaternion const& value);
        float dot(quaternion const& quaternion1, quaternion const& quaternion2);
        quaternion normalize(quaternion const& value);
        quaternion conjugate(quaternion const& value);
        quaternion inverse(quaternion const& value);
        quaternion slerp(quaternion const& quaternion1, quaternion const& quaternion2, float amount);
        quaternion lerp(quaternion const& quaternion1, quaternion const& quaternion2, float amount);
        quaternion concatenate(quaternion const& value1, quaternion const& value2);
    }
}


// Interop between Windows::Math and DirectXMath.
namespace DirectX
{
    XMVECTOR XM_CALLCONV XMLoadFloat2    (_In_ Windows::Math::float2     const* pSource);
    XMVECTOR XM_CALLCONV XMLoadFloat3    (_In_ Windows::Math::float3     const* pSource);
    XMVECTOR XM_CALLCONV XMLoadFloat4    (_In_ Windows::Math::float4     const* pSource);
    XMMATRIX XM_CALLCONV XMLoadFloat3x2  (_In_ Windows::Math::float3x2   const* pSource);
    XMMATRIX XM_CALLCONV XMLoadFloat4x4  (_In_ Windows::Math::float4x4   const* pSource);
    XMVECTOR XM_CALLCONV XMLoadPlane     (_In_ Windows::Math::plane      const* pSource);
    XMVECTOR XM_CALLCONV XMLoadQuaternion(_In_ Windows::Math::quaternion const* pSource);

    void XM_CALLCONV XMStoreFloat2    (_Out_ Windows::Math::float2*     pDestination, _In_ FXMVECTOR value);
    void XM_CALLCONV XMStoreFloat3    (_Out_ Windows::Math::float3*     pDestination, _In_ FXMVECTOR value);
    void XM_CALLCONV XMStoreFloat4    (_Out_ Windows::Math::float4*     pDestination, _In_ FXMVECTOR value);
    void XM_CALLCONV XMStoreFloat3x2  (_Out_ Windows::Math::float3x2*   pDestination, _In_ FXMMATRIX value);
    void XM_CALLCONV XMStoreFloat4x4  (_Out_ Windows::Math::float4x4*   pDestination, _In_ FXMMATRIX value);
    void XM_CALLCONV XMStorePlane     (_Out_ Windows::Math::plane*      pDestination, _In_ FXMVECTOR value);
    void XM_CALLCONV XMStoreQuaternion(_Out_ Windows::Math::quaternion* pDestination, _In_ FXMVECTOR value);
}


#include "WindowsMath.inl"
