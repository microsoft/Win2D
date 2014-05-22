// Copyright (c) Microsoft Corporation.  All rights reserved

#include "pch.h"
#include "MathTypeConversion.h"


class QuaternionStatics : public Microsoft::WRL::ActivationFactory<Midl::IQuaternionHelperStatics>
{
    InspectableClassStatic(RuntimeClass_Microsoft_Graphics_Canvas_Math_QuaternionHelper, BaseTrust);

public:
    IFACEMETHODIMP FromValues(float x, float y, float z, float w, _Out_ Midl::Quaternion* ret)
    {
        *asImpl(ret) = Impl::Quaternion(x, y, z, w);
        return S_OK;
    }

    IFACEMETHODIMP FromVector3(Midl::Vector3 vectorPart, float scalarPart, _Out_ Midl::Quaternion* ret)
    {
        *asImpl(ret) = Impl::Quaternion(asImpl(vectorPart), scalarPart);
        return S_OK;
    }

    IFACEMETHODIMP get_Identity(_Out_ Midl::Quaternion* ret)
    {
        *asImpl(ret) = Impl::Quaternion::Identity();
        return S_OK;
    }

    IFACEMETHODIMP IsIdentity(Midl::Quaternion value, _Out_ boolean* ret)
    {
        *ret = asImpl(value).IsIdentity();
        return S_OK;
    }

    IFACEMETHODIMP Length(Midl::Quaternion value, _Out_ float* ret)
    {
        *ret = asImpl(value).Length();
        return S_OK;
    }

    IFACEMETHODIMP LengthSquared(Midl::Quaternion value, _Out_ float* ret)
    {
        *ret = asImpl(value).LengthSquared();
        return S_OK;
    }

    IFACEMETHODIMP Normalize(Midl::Quaternion value, _Out_ Midl::Quaternion* ret)
    {
        *asImpl(ret) = Impl::Quaternion::Normalize(asImpl(value));
        return S_OK;
    }

    IFACEMETHODIMP Conjugate(Midl::Quaternion value, _Out_ Midl::Quaternion* ret)
    {
        *asImpl(ret) = Impl::Quaternion::Conjugate(asImpl(value));
        return S_OK;
    }

    IFACEMETHODIMP Inverse(Midl::Quaternion value, _Out_ Midl::Quaternion* ret)
    {
        *asImpl(ret) = Impl::Quaternion::Inverse(asImpl(value));
        return S_OK;
    }

    IFACEMETHODIMP CreateFromAxisAngle(Midl::Vector3 axis, float angle, _Out_ Midl::Quaternion* ret)
    {
        *asImpl(ret) = Impl::Quaternion::CreateFromAxisAngle(asImpl(axis), angle);
        return S_OK;
    }

    IFACEMETHODIMP CreateFromYawPitchRoll(float yaw, float pitch, float roll, _Out_ Midl::Quaternion* ret)
    {
        *asImpl(ret) = Impl::Quaternion::CreateFromYawPitchRoll(yaw, pitch, roll);
        return S_OK;
    }

    IFACEMETHODIMP CreateFromRotationMatrix(Midl::Matrix4x4 matrix, _Out_ Midl::Quaternion* ret)
    {
        *asImpl(ret) = Impl::Quaternion::CreateFromRotationMatrix(asImpl(matrix));
        return S_OK;
    }

    IFACEMETHODIMP Dot(Midl::Quaternion quaternion1, Midl::Quaternion quaternion2, _Out_ float* ret)
    {
        *ret = Impl::Quaternion::Dot(asImpl(quaternion1), asImpl(quaternion2));
        return S_OK;
    }

    IFACEMETHODIMP Slerp(Midl::Quaternion quaternion1, Midl::Quaternion quaternion2, float amount, _Out_ Midl::Quaternion* ret)
    {
        *asImpl(ret) = Impl::Quaternion::Slerp(asImpl(quaternion1), asImpl(quaternion2), amount);
        return S_OK;
    }

    IFACEMETHODIMP Lerp(Midl::Quaternion quaternion1, Midl::Quaternion quaternion2, float amount, _Out_ Midl::Quaternion* ret)
    {
        *asImpl(ret) = Impl::Quaternion::Lerp(asImpl(quaternion1), asImpl(quaternion2), amount);
        return S_OK;
    }

    IFACEMETHODIMP Concatenate(Midl::Quaternion value1, Midl::Quaternion value2, _Out_ Midl::Quaternion* ret)
    {
        *asImpl(ret) = Impl::Quaternion::Concatenate(asImpl(value1), asImpl(value2));
        return S_OK;
    }

    IFACEMETHODIMP Negate(Midl::Quaternion value, _Out_ Midl::Quaternion* ret)
    {
        *asImpl(ret) = Impl::Quaternion::Negate(asImpl(value));
        return S_OK;
    }

    IFACEMETHODIMP Add(Midl::Quaternion value1, Midl::Quaternion value2, _Out_ Midl::Quaternion* ret)
    {
        *asImpl(ret) = Impl::Quaternion::Add(asImpl(value1), asImpl(value2));
        return S_OK;
    }

    IFACEMETHODIMP Subtract(Midl::Quaternion value1, Midl::Quaternion value2, _Out_ Midl::Quaternion* ret)
    {
        *asImpl(ret) = Impl::Quaternion::Subtract(asImpl(value1), asImpl(value2));
        return S_OK;
    }

    IFACEMETHODIMP Multiply(Midl::Quaternion value1, Midl::Quaternion value2, _Out_ Midl::Quaternion* ret)
    {
        *asImpl(ret) = Impl::Quaternion::Multiply(asImpl(value1), asImpl(value2));
        return S_OK;
    }

    IFACEMETHODIMP MultiplyByScalar(Midl::Quaternion value1, float value2, _Out_ Midl::Quaternion* ret)
    {
        *asImpl(ret) = Impl::Quaternion::Multiply(asImpl(value1),  value2);
        return S_OK;
    }

    IFACEMETHODIMP Divide(Midl::Quaternion value1, Midl::Quaternion value2, _Out_ Midl::Quaternion* ret)
    {
        *asImpl(ret) = Impl::Quaternion::Divide(asImpl(value1), asImpl(value2));
        return S_OK;
    }
};


ActivatableStaticOnlyFactory(QuaternionStatics);
