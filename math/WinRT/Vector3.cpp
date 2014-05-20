// Copyright (c) Microsoft Corporation.  All rights reserved

#include "pch.h"
#include "MathTypeConversion.h"


class Vector3Statics : public Microsoft::WRL::ActivationFactory<Midl::IVector3HelperStatics>
{
    InspectableClassStatic(RuntimeClass_Microsoft_DirectX_Math_Vector3Helper, BaseTrust);

public:
    IFACEMETHODIMP FromValues(float x, float y, float z, _Out_ Midl::Vector3* ret)
    {
        *asImpl(ret) = Impl::Vector3(x, y, z);
        return S_OK;
    }

    IFACEMETHODIMP FromVector2(Midl::Vector2 value, float z, _Out_ Midl::Vector3* ret)
    {
        *asImpl(ret) = Impl::Vector3(asImpl(value), z);
        return S_OK;
    }

    IFACEMETHODIMP FromValue(float value, _Out_ Midl::Vector3* ret)
    {
        *asImpl(ret) = Impl::Vector3(value);
        return S_OK;
    }

    IFACEMETHODIMP get_Zero(_Out_ Midl::Vector3* ret)
    {
        *asImpl(ret) = Impl::Vector3::Zero();
        return S_OK;
    }

    IFACEMETHODIMP get_One(_Out_ Midl::Vector3* ret)
    {
        *asImpl(ret) = Impl::Vector3::One();
        return S_OK;
    }

    IFACEMETHODIMP get_UnitX(_Out_ Midl::Vector3* ret)
    {
        *asImpl(ret) = Impl::Vector3::UnitX();
        return S_OK;
    }

    IFACEMETHODIMP get_UnitY(_Out_ Midl::Vector3* ret)
    {
        *asImpl(ret) = Impl::Vector3::UnitY();
        return S_OK;
    }

    IFACEMETHODIMP get_UnitZ(_Out_ Midl::Vector3* ret)
    {
        *asImpl(ret) = Impl::Vector3::UnitZ();
        return S_OK;
    }

    IFACEMETHODIMP Length(Midl::Vector3 value, _Out_ float* ret)
    {
        *ret = asImpl(value).Length();
        return S_OK;
    }

    IFACEMETHODIMP LengthSquared(Midl::Vector3 value, _Out_ float* ret)
    {
        *ret = asImpl(value).LengthSquared();
        return S_OK;
    }

    IFACEMETHODIMP Distance(Midl::Vector3 value1, Midl::Vector3 value2, _Out_ float* ret)
    {
        *ret = Impl::Vector3::Distance(asImpl(value1), asImpl(value2));
        return S_OK;
    }

    IFACEMETHODIMP DistanceSquared(Midl::Vector3 value1, Midl::Vector3 value2, _Out_ float* ret)
    {
        *ret = Impl::Vector3::DistanceSquared(asImpl(value1), asImpl(value2));
        return S_OK;
    }

    IFACEMETHODIMP Dot(Midl::Vector3 vector1, Midl::Vector3 vector2, _Out_ float* ret)
    {
        *ret = Impl::Vector3::Dot(asImpl(vector1), asImpl(vector2));
        return S_OK;
    }

    IFACEMETHODIMP Normalize(Midl::Vector3 value, _Out_ Midl::Vector3* ret)
    {
        *asImpl(ret) = Impl::Vector3::Normalize(asImpl(value));
        return S_OK;
    }

    IFACEMETHODIMP Cross(Midl::Vector3 vector1, Midl::Vector3 vector2, _Out_ Midl::Vector3* ret)
    {
        *asImpl(ret) = Impl::Vector3::Cross(asImpl(vector1), asImpl(vector2));
        return S_OK;
    }

    IFACEMETHODIMP Reflect(Midl::Vector3 vector, Midl::Vector3 normal, _Out_ Midl::Vector3* ret)
    {
        *asImpl(ret) = Impl::Vector3::Reflect(asImpl(vector), asImpl(normal));
        return S_OK;
    }

    IFACEMETHODIMP Min(Midl::Vector3 value1, Midl::Vector3 value2, _Out_ Midl::Vector3* ret)
    {
        *asImpl(ret) = Impl::Vector3::Min(asImpl(value1), asImpl(value2));
        return S_OK;
    }

    IFACEMETHODIMP Max(Midl::Vector3 value1, Midl::Vector3 value2, _Out_ Midl::Vector3* ret)
    {
        *asImpl(ret) = Impl::Vector3::Max(asImpl(value1), asImpl(value2));
        return S_OK;
    }

    IFACEMETHODIMP Clamp(Midl::Vector3 value1, Midl::Vector3 min, Midl::Vector3 max, _Out_ Midl::Vector3* ret)
    {
        *asImpl(ret) = Impl::Vector3::Clamp(asImpl(value1), asImpl(min), asImpl(max));
        return S_OK;
    }

    IFACEMETHODIMP Lerp(Midl::Vector3 value1, Midl::Vector3 value2, float amount, _Out_ Midl::Vector3* ret)
    {
        *asImpl(ret) = Impl::Vector3::Lerp(asImpl(value1), asImpl(value2), amount);
        return S_OK;
    }

    IFACEMETHODIMP Transform(Midl::Vector3 position, Midl::Matrix4x4 matrix, _Out_ Midl::Vector3* ret)
    {
        *asImpl(ret) = Impl::Vector3::Transform(asImpl(position), asImpl(matrix));
        return S_OK;
    }

    IFACEMETHODIMP TransformNormal(Midl::Vector3 normal, Midl::Matrix4x4 matrix, _Out_ Midl::Vector3* ret)
    {
        *asImpl(ret) = Impl::Vector3::TransformNormal(asImpl(normal), asImpl(matrix));
        return S_OK;
    }

    IFACEMETHODIMP TransformByQuaternion(Midl::Vector3 value, Midl::Quaternion rotation, _Out_ Midl::Vector3* ret)
    {
        *asImpl(ret) = Impl::Vector3::Transform(asImpl(value), asImpl(rotation));
        return S_OK;
    }

    IFACEMETHODIMP Negate(Midl::Vector3 value, _Out_ Midl::Vector3* ret)
    {
        *asImpl(ret) = Impl::Vector3::Negate(asImpl(value));
        return S_OK;
    }

    IFACEMETHODIMP Add(Midl::Vector3 value1, Midl::Vector3 value2, _Out_ Midl::Vector3* ret)
    {
        *asImpl(ret) = Impl::Vector3::Add(asImpl(value1), asImpl(value2));
        return S_OK;
    }

    IFACEMETHODIMP Subtract(Midl::Vector3 value1, Midl::Vector3 value2, _Out_ Midl::Vector3* ret)
    {
        *asImpl(ret) = Impl::Vector3::Subtract(asImpl(value1), asImpl(value2));
        return S_OK;
    }

    IFACEMETHODIMP Multiply(Midl::Vector3 value1, Midl::Vector3 value2, _Out_ Midl::Vector3* ret)
    {
        *asImpl(ret) = Impl::Vector3::Multiply(asImpl(value1), asImpl(value2));
        return S_OK;
    }

    IFACEMETHODIMP MultiplyByScalar(Midl::Vector3 value1, float value2, _Out_ Midl::Vector3* ret)
    {
        *asImpl(ret) = Impl::Vector3::Multiply(asImpl(value1), value2);
        return S_OK;
    }

    IFACEMETHODIMP Divide(Midl::Vector3 value1, Midl::Vector3 value2, _Out_ Midl::Vector3* ret)
    {
        *asImpl(ret) = Impl::Vector3::Divide(asImpl(value1), asImpl(value2));
        return S_OK;
    }

    IFACEMETHODIMP DivideByScalar(Midl::Vector3 value1, float value2, _Out_ Midl::Vector3* ret)
    {
        *asImpl(ret) = Impl::Vector3::Divide(asImpl(value1), value2);
        return S_OK;
    }
};


ActivatableStaticOnlyFactory(Vector3Statics);
