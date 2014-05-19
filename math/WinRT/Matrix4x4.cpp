// Copyright (c) Microsoft Corporation.  All rights reserved

#include "pch.h"
#include "MathTypeConversion.h"


class Matrix4x4Statics : public Microsoft::WRL::ActivationFactory<Midl::IMatrix4x4HelperStatics>
{
    InspectableClassStatic(RuntimeClass_Microsoft_Graphics_Canvas_Math_Matrix4x4Helper, BaseTrust);

public:
    IFACEMETHODIMP FromValues(float m11, float m12, float m13, float m14, float m21, float m22, float m23, float m24, float m31, float m32, float m33, float m34, float m41, float m42, float m43, float m44, _Out_ Midl::Matrix4x4* ret)
    {
        *asImpl(ret) = Impl::float4x4(m11, m12, m13, m14, m21, m22, m23, m24, m31, m32, m33, m34, m41, m42, m43, m44);
        return S_OK;
    }

    IFACEMETHODIMP FromMatrix3x2(Midl::Matrix3x2 value, _Out_ Midl::Matrix4x4* ret)
    {
        *asImpl(ret) = Impl::float4x4(asImpl(value));
        return S_OK;
    }

    IFACEMETHODIMP get_Identity(_Out_ Midl::Matrix4x4* ret)
    {
        *asImpl(ret) = Impl::float4x4::identity();
        return S_OK;
    }

    IFACEMETHODIMP IsIdentity(Midl::Matrix4x4 value, _Out_ boolean* ret)
    {
        *ret = Impl::is_identity(asImpl(value));
        return S_OK;
    }

    IFACEMETHODIMP Determinant(Midl::Matrix4x4 value, _Out_ float* ret)
    {
        *ret = Impl::determinant(asImpl(value));
        return S_OK;
    }

    IFACEMETHODIMP Translation(Midl::Matrix4x4 value, _Out_ Midl::Vector3* ret)
    {
        *asImpl(ret) = Impl::translation(asImpl(value));
        return S_OK;
    }

    IFACEMETHODIMP CreateBillboard(Midl::Vector3 objectPosition, Midl::Vector3 cameraPosition, Midl::Vector3 cameraUpVector, Midl::Vector3 cameraForwardVector, _Out_ Midl::Matrix4x4* ret)
    {
        *asImpl(ret) = Impl::float4x4::billboard(asImpl(objectPosition), asImpl(cameraPosition), asImpl(cameraUpVector), asImpl(cameraForwardVector));
        return S_OK;
    }

    IFACEMETHODIMP CreateConstrainedBillboard(Midl::Vector3 objectPosition, Midl::Vector3 cameraPosition, Midl::Vector3 rotateAxis, Midl::Vector3 cameraForwardVector, Midl::Vector3 objectForwardVector, _Out_ Midl::Matrix4x4* ret)
    {
        *asImpl(ret) = Impl::float4x4::constrained_billboard(asImpl(objectPosition), asImpl(cameraPosition), asImpl(rotateAxis), asImpl(cameraForwardVector), asImpl(objectForwardVector));
        return S_OK;
    }

    IFACEMETHODIMP CreateTranslation(Midl::Vector3 position, _Out_ Midl::Matrix4x4* ret)
    {
        *asImpl(ret) = Impl::float4x4::translation(asImpl(position));
        return S_OK;
    }

    IFACEMETHODIMP CreateTranslationFromValues(float xPosition, float yPosition, float zPosition, _Out_ Midl::Matrix4x4* ret)
    {
        *asImpl(ret) = Impl::float4x4::translation(xPosition, yPosition, zPosition);
        return S_OK;
    }

    IFACEMETHODIMP CreateScale(float scale, _Out_ Midl::Matrix4x4* ret)
    {
        *asImpl(ret) = Impl::float4x4::scale(scale);
        return S_OK;
    }

    IFACEMETHODIMP CreateScaleFromValues(float xScale, float yScale, float zScale, _Out_ Midl::Matrix4x4* ret)
    {
        *asImpl(ret) = Impl::float4x4::scale(xScale, yScale, zScale);
        return S_OK;
    }

    IFACEMETHODIMP CreateScaleFromValuesAroundCenter(float xScale, float yScale, float zScale, Midl::Vector3 centerPoint, _Out_ Midl::Matrix4x4* ret)
    {
        *asImpl(ret) = Impl::float4x4::scale(xScale, yScale, zScale, asImpl(centerPoint));
        return S_OK;
    }

    IFACEMETHODIMP CreateRotationX(float radians, _Out_ Midl::Matrix4x4* ret)
    {
        *asImpl(ret) = Impl::float4x4::rotation_x(radians);
        return S_OK;
    }

    IFACEMETHODIMP CreateRotationXAroundCenter(float radians, Midl::Vector3 centerPoint, _Out_ Midl::Matrix4x4* ret)
    {
        *asImpl(ret) = Impl::float4x4::rotation_x(radians, asImpl(centerPoint));
        return S_OK;
    }

    IFACEMETHODIMP CreateRotationY(float radians, _Out_ Midl::Matrix4x4* ret)
    {
        *asImpl(ret) = Impl::float4x4::rotation_y(radians);
        return S_OK;
    }

    IFACEMETHODIMP CreateRotationYAroundCenter(float radians, Midl::Vector3 centerPoint, _Out_ Midl::Matrix4x4* ret)
    {
        *asImpl(ret) = Impl::float4x4::rotation_y(radians, asImpl(centerPoint));
        return S_OK;
    }

    IFACEMETHODIMP CreateRotationZ(float radians, _Out_ Midl::Matrix4x4* ret)
    {
        *asImpl(ret) = Impl::float4x4::rotation_z(radians);
        return S_OK;
    }

    IFACEMETHODIMP CreateRotationZAroundCenter(float radians, Midl::Vector3 centerPoint, _Out_ Midl::Matrix4x4* ret)
    {
        *asImpl(ret) = Impl::float4x4::rotation_z(radians, asImpl(centerPoint));
        return S_OK;
    }

    IFACEMETHODIMP CreateFromAxisAngle(Midl::Vector3 axis, float angle, _Out_ Midl::Matrix4x4* ret)
    {
        *asImpl(ret) = Impl::float4x4::from_axis_angle(asImpl(axis), angle);
        return S_OK;
    }

    IFACEMETHODIMP CreatePerspectiveFieldOfView(float fieldOfView, float aspectRatio, float nearPlaneDistance, float farPlaneDistance, _Out_ Midl::Matrix4x4* ret)
    {
        try
        {
            *asImpl(ret) = Impl::float4x4::perspective_field_of_view(fieldOfView, aspectRatio, nearPlaneDistance, farPlaneDistance);
            return S_OK;
        }
        catch (std::invalid_argument const&)
        {
            *asImpl(ret) = Impl::float4x4(NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN);
            return E_INVALIDARG;
        }
    }

    IFACEMETHODIMP CreatePerspective(float width, float height, float nearPlaneDistance, float farPlaneDistance, _Out_ Midl::Matrix4x4* ret)
    {
        try
        {
            *asImpl(ret) = Impl::float4x4::perspective(width, height, nearPlaneDistance, farPlaneDistance);
            return S_OK;
        }
        catch (std::invalid_argument const&)
        {
            *asImpl(ret) = Impl::float4x4(NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN);
            return E_INVALIDARG;
        }
    }

    IFACEMETHODIMP CreatePerspectiveOffCenter(float left, float right, float bottom, float top, float nearPlaneDistance, float farPlaneDistance, _Out_ Midl::Matrix4x4* ret)
    {
        try
        {
            *asImpl(ret) = Impl::float4x4::perspective_off_center(left, right, bottom, top, nearPlaneDistance, farPlaneDistance);
            return S_OK;
        }
        catch (std::invalid_argument const&)
        {
            *asImpl(ret) = Impl::float4x4(NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN);
            return E_INVALIDARG;
        }
    }

    IFACEMETHODIMP CreateOrthographic(float width, float height, float zNearPlane, float zFarPlane, _Out_ Midl::Matrix4x4* ret)
    {
        *asImpl(ret) = Impl::float4x4::orthographic(width, height, zNearPlane, zFarPlane);
        return S_OK;
    }

    IFACEMETHODIMP CreateOrthographicOffCenter(float left, float right, float bottom, float top, float zNearPlane, float zFarPlane, _Out_ Midl::Matrix4x4* ret)
    {
        *asImpl(ret) = Impl::float4x4::orthographic_off_center(left, right, bottom, top, zNearPlane, zFarPlane);
        return S_OK;
    }

    IFACEMETHODIMP CreateLookAt(Midl::Vector3 cameraPosition, Midl::Vector3 cameraTarget, Midl::Vector3 cameraUpVector, _Out_ Midl::Matrix4x4* ret)
    {
        *asImpl(ret) = Impl::float4x4::look_at(asImpl(cameraPosition), asImpl(cameraTarget), asImpl(cameraUpVector));
        return S_OK;
    }

    IFACEMETHODIMP CreateWorld(Midl::Vector3 position, Midl::Vector3 forward, Midl::Vector3 up, _Out_ Midl::Matrix4x4* ret)
    {
        *asImpl(ret) = Impl::float4x4::world(asImpl(position), asImpl(forward), asImpl(up));
        return S_OK;
    }

    IFACEMETHODIMP CreateFromQuaternion(Midl::Quaternion quaternion, _Out_ Midl::Matrix4x4* ret)
    {
        *asImpl(ret) = Impl::float4x4::from_quaternion(asImpl(quaternion));
        return S_OK;
    }

    IFACEMETHODIMP CreateFromYawPitchRoll(float yaw, float pitch, float roll, _Out_ Midl::Matrix4x4* ret)
    {
        *asImpl(ret) = Impl::float4x4::from_yaw_pitch_roll(yaw, pitch, roll);
        return S_OK;
    }

    IFACEMETHODIMP CreateShadow(Midl::Vector3 lightDirection, Midl::Plane plane, _Out_ Midl::Matrix4x4* ret)
    {
        *asImpl(ret) = Impl::float4x4::shadow(asImpl(lightDirection), asImpl(plane));
        return S_OK;
    }

    IFACEMETHODIMP CreateReflection(Midl::Plane value, _Out_ Midl::Matrix4x4* ret)
    {
        *asImpl(ret) = Impl::float4x4::reflection(asImpl(value));
        return S_OK;
    }

    IFACEMETHODIMP Invert(Midl::Matrix4x4 matrix, _Out_ Midl::Matrix4x4* result, _Out_ boolean* ret)
    {
        *ret = Impl::invert(asImpl(matrix), asImpl(result));
        return S_OK;
    }

    IFACEMETHODIMP Decompose(Midl::Matrix4x4 matrix, _Out_ Midl::Vector3* scale, _Out_ Midl::Quaternion* rotation, _Out_ Midl::Vector3* translation, _Out_ boolean* ret)
    {
        *ret = Impl::decompose(asImpl(matrix), asImpl(scale), asImpl(rotation), asImpl(translation));
        return S_OK;
    }

    IFACEMETHODIMP Transform(Midl::Matrix4x4 value, Midl::Quaternion rotation, _Out_ Midl::Matrix4x4* ret)
    {
        *asImpl(ret) = Impl::transform(asImpl(value), asImpl(rotation));
        return S_OK;
    }

    IFACEMETHODIMP Transpose(Midl::Matrix4x4 matrix, _Out_ Midl::Matrix4x4* ret)
    {
        *asImpl(ret) = Impl::transpose(asImpl(matrix));
        return S_OK;
    }

    IFACEMETHODIMP Lerp(Midl::Matrix4x4 matrix1, Midl::Matrix4x4 matrix2, float amount, _Out_ Midl::Matrix4x4* ret)
    {
        *asImpl(ret) = Impl::lerp(asImpl(matrix1), asImpl(matrix2), amount);
        return S_OK;
    }

    IFACEMETHODIMP Negate(Midl::Matrix4x4 value, _Out_ Midl::Matrix4x4* ret)
    {
        *asImpl(ret) = -asImpl(value);
        return S_OK;
    }

    IFACEMETHODIMP Add(Midl::Matrix4x4 value1, Midl::Matrix4x4 value2, _Out_ Midl::Matrix4x4* ret)
    {
        *asImpl(ret) = asImpl(value1) + asImpl(value2);
        return S_OK;
    }

    IFACEMETHODIMP Subtract(Midl::Matrix4x4 value1, Midl::Matrix4x4 value2, _Out_ Midl::Matrix4x4* ret)
    {
        *asImpl(ret) = asImpl(value1) - asImpl(value2);
        return S_OK;
    }

    IFACEMETHODIMP Multiply(Midl::Matrix4x4 value1, Midl::Matrix4x4 value2, _Out_ Midl::Matrix4x4* ret)
    {
        *asImpl(ret) = asImpl(value1) * asImpl(value2);
        return S_OK;
    }

    IFACEMETHODIMP MultiplyByScalar(Midl::Matrix4x4 value1, float value2, _Out_ Midl::Matrix4x4* ret)
    {
        *asImpl(ret) = asImpl(value1) * value2;
        return S_OK;
    }
};


ActivatableStaticOnlyFactory(Matrix4x4Statics);
