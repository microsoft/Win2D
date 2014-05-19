// Copyright (c) Microsoft Corporation.  All rights reserved

#include "pch.h"
#include "MathTypeConversion.h"


class QuaternionStatics : public Microsoft::WRL::ActivationFactory<Midl::IQuaternionHelperStatics>
{
    InspectableClassStatic(RuntimeClass_Microsoft_Graphics_Canvas_Math_QuaternionHelper, BaseTrust);

public:
    IFACEMETHODIMP FromValues(float x, float y, float z, float w, _Out_ Midl::Quaternion* ret)
    {
        *asImpl(ret) = Impl::quaternion(x, y, z, w);
        return S_OK;
    }

    IFACEMETHODIMP FromVector3(Midl::Vector3 vectorPart, float scalarPart, _Out_ Midl::Quaternion* ret)
    {
        *asImpl(ret) = Impl::quaternion(asImpl(vectorPart), scalarPart);
        return S_OK;
    }

    IFACEMETHODIMP get_Identity(_Out_ Midl::Quaternion* ret)
    {
        *asImpl(ret) = Impl::quaternion::identity();
        return S_OK;
    }

    IFACEMETHODIMP IsIdentity(Midl::Quaternion value, _Out_ boolean* ret)
    {
        *ret = Impl::is_identity(asImpl(value));
        return S_OK;
    }

    IFACEMETHODIMP Length(Midl::Quaternion value, _Out_ float* ret)
    {
        *ret = Impl::length(asImpl(value));
        return S_OK;
    }

    IFACEMETHODIMP LengthSquared(Midl::Quaternion value, _Out_ float* ret)
    {
        *ret = Impl::length_squared(asImpl(value));
        return S_OK;
    }

    IFACEMETHODIMP Normalize(Midl::Quaternion value, _Out_ Midl::Quaternion* ret)
    {
        *asImpl(ret) = Impl::normalize(asImpl(value));
        return S_OK;
    }

    IFACEMETHODIMP Conjugate(Midl::Quaternion value, _Out_ Midl::Quaternion* ret)
    {
        *asImpl(ret) = Impl::conjugate(asImpl(value));
        return S_OK;
    }

    IFACEMETHODIMP Inverse(Midl::Quaternion value, _Out_ Midl::Quaternion* ret)
    {
        *asImpl(ret) = Impl::inverse(asImpl(value));
        return S_OK;
    }

    IFACEMETHODIMP CreateFromAxisAngle(Midl::Vector3 axis, float angle, _Out_ Midl::Quaternion* ret)
    {
        *asImpl(ret) = Impl::quaternion::from_axis_angle(asImpl(axis), angle);
        return S_OK;
    }

    IFACEMETHODIMP CreateFromYawPitchRoll(float yaw, float pitch, float roll, _Out_ Midl::Quaternion* ret)
    {
        *asImpl(ret) = Impl::quaternion::from_yaw_pitch_roll(yaw, pitch, roll);
        return S_OK;
    }

    IFACEMETHODIMP CreateFromRotationMatrix(Midl::Matrix4x4 matrix, _Out_ Midl::Quaternion* ret)
    {
        *asImpl(ret) = Impl::quaternion::from_rotation_matrix(asImpl(matrix));
        return S_OK;
    }

    IFACEMETHODIMP Dot(Midl::Quaternion quaternion1, Midl::Quaternion quaternion2, _Out_ float* ret)
    {
        *ret = Impl::dot(asImpl(quaternion1), asImpl(quaternion2));
        return S_OK;
    }

    IFACEMETHODIMP Slerp(Midl::Quaternion quaternion1, Midl::Quaternion quaternion2, float amount, _Out_ Midl::Quaternion* ret)
    {
        *asImpl(ret) = Impl::slerp(asImpl(quaternion1), asImpl(quaternion2), amount);
        return S_OK;
    }

    IFACEMETHODIMP Lerp(Midl::Quaternion quaternion1, Midl::Quaternion quaternion2, float amount, _Out_ Midl::Quaternion* ret)
    {
        *asImpl(ret) = Impl::lerp(asImpl(quaternion1), asImpl(quaternion2), amount);
        return S_OK;
    }

    IFACEMETHODIMP Concatenate(Midl::Quaternion value1, Midl::Quaternion value2, _Out_ Midl::Quaternion* ret)
    {
        *asImpl(ret) = Impl::concatenate(asImpl(value1), asImpl(value2));
        return S_OK;
    }

    IFACEMETHODIMP Negate(Midl::Quaternion value, _Out_ Midl::Quaternion* ret)
    {
        *asImpl(ret) = -asImpl(value);
        return S_OK;
    }

    IFACEMETHODIMP Add(Midl::Quaternion value1, Midl::Quaternion value2, _Out_ Midl::Quaternion* ret)
    {
        *asImpl(ret) = asImpl(value1) + asImpl(value2);
        return S_OK;
    }

    IFACEMETHODIMP Subtract(Midl::Quaternion value1, Midl::Quaternion value2, _Out_ Midl::Quaternion* ret)
    {
        *asImpl(ret) = asImpl(value1) - asImpl(value2);
        return S_OK;
    }

    IFACEMETHODIMP Multiply(Midl::Quaternion value1, Midl::Quaternion value2, _Out_ Midl::Quaternion* ret)
    {
        *asImpl(ret) = asImpl(value1) * asImpl(value2);
        return S_OK;
    }

    IFACEMETHODIMP MultiplyByScalar(Midl::Quaternion value1, float value2, _Out_ Midl::Quaternion* ret)
    {
        *asImpl(ret) = asImpl(value1) * value2;
        return S_OK;
    }

    IFACEMETHODIMP Divide(Midl::Quaternion value1, Midl::Quaternion value2, _Out_ Midl::Quaternion* ret)
    {
        *asImpl(ret) = asImpl(value1) / asImpl(value2);
        return S_OK;
    }
};


ActivatableStaticOnlyFactory(QuaternionStatics);
