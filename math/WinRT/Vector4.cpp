// Copyright (c) Microsoft Corporation.  All rights reserved

#include "pch.h"
#include "MathTypeConversion.h"


class Vector4Statics : public Microsoft::WRL::ActivationFactory<Midl::IVector4HelperStatics>
{
    InspectableClassStatic(RuntimeClass_Microsoft_Graphics_Canvas_Math_Vector4Helper, BaseTrust);

public:
    IFACEMETHODIMP FromValues(float x, float y, float z, float w, _Out_ Midl::Vector4* ret)
    {
        *asImpl(ret) = Impl::float4(x, y, z, w);
        return S_OK;
    }

    IFACEMETHODIMP FromVector2(Midl::Vector2 value, float z, float w, _Out_ Midl::Vector4* ret)
    {
        *asImpl(ret) = Impl::float4(asImpl(value), z, w);
        return S_OK;
    }

    IFACEMETHODIMP FromVector3(Midl::Vector3 value, float w, _Out_ Midl::Vector4* ret)
    {
        *asImpl(ret) = Impl::float4(asImpl(value), w);
        return S_OK;
    }

    IFACEMETHODIMP FromValue(float value, _Out_ Midl::Vector4* ret)
    {
        *asImpl(ret) = Impl::float4(value);
        return S_OK;
    }

    IFACEMETHODIMP get_Zero(_Out_ Midl::Vector4* ret)
    {
        *asImpl(ret) = Impl::float4::zero();
        return S_OK;
    }

    IFACEMETHODIMP get_One(_Out_ Midl::Vector4* ret)
    {
        *asImpl(ret) = Impl::float4::one();
        return S_OK;
    }

    IFACEMETHODIMP get_UnitX(_Out_ Midl::Vector4* ret)
    {
        *asImpl(ret) = Impl::float4::unit_x();
        return S_OK;
    }

    IFACEMETHODIMP get_UnitY(_Out_ Midl::Vector4* ret)
    {
        *asImpl(ret) = Impl::float4::unit_y();
        return S_OK;
    }

    IFACEMETHODIMP get_UnitZ(_Out_ Midl::Vector4* ret)
    {
        *asImpl(ret) = Impl::float4::unit_z();
        return S_OK;
    }

    IFACEMETHODIMP get_UnitW(_Out_ Midl::Vector4* ret)
    {
        *asImpl(ret) = Impl::float4::unit_w();
        return S_OK;
    }

    IFACEMETHODIMP Length(Midl::Vector4 value, _Out_ float* ret)
    {
        *ret = Impl::length(asImpl(value));
        return S_OK;
    }

    IFACEMETHODIMP LengthSquared(Midl::Vector4 value, _Out_ float* ret)
    {
        *ret = Impl::length_squared(asImpl(value));
        return S_OK;
    }

    IFACEMETHODIMP Distance(Midl::Vector4 value1, Midl::Vector4 value2, _Out_ float* ret)
    {
        *ret = Impl::distance(asImpl(value1), asImpl(value2));
        return S_OK;
    }

    IFACEMETHODIMP DistanceSquared(Midl::Vector4 value1, Midl::Vector4 value2, _Out_ float* ret)
    {
        *ret = Impl::distance_squared(asImpl(value1), asImpl(value2));
        return S_OK;
    }

    IFACEMETHODIMP Dot(Midl::Vector4 vector1, Midl::Vector4 vector2, _Out_ float* ret)
    {
        *ret = Impl::dot(asImpl(vector1), asImpl(vector2));
        return S_OK;
    }

    IFACEMETHODIMP Normalize(Midl::Vector4 vector, _Out_ Midl::Vector4* ret)
    {
        *asImpl(ret) = Impl::normalize(asImpl(vector));
        return S_OK;
    }

    IFACEMETHODIMP Min(Midl::Vector4 value1, Midl::Vector4 value2, _Out_ Midl::Vector4* ret)
    {
        *asImpl(ret) = Impl::min(asImpl(value1), asImpl(value2));
        return S_OK;
    }

    IFACEMETHODIMP Max(Midl::Vector4 value1, Midl::Vector4 value2, _Out_ Midl::Vector4* ret)
    {
        *asImpl(ret) = Impl::max(asImpl(value1), asImpl(value2));
        return S_OK;
    }

    IFACEMETHODIMP Clamp(Midl::Vector4 value1, Midl::Vector4 min, Midl::Vector4 max, _Out_ Midl::Vector4* ret)
    {
        *asImpl(ret) = Impl::clamp(asImpl(value1), asImpl(min), asImpl(max));
        return S_OK;
    }

    IFACEMETHODIMP Lerp(Midl::Vector4 value1, Midl::Vector4 value2, float amount, _Out_ Midl::Vector4* ret)
    {
        *asImpl(ret) = Impl::lerp(asImpl(value1), asImpl(value2), amount);
        return S_OK;
    }

    IFACEMETHODIMP TransformVector2(Midl::Vector2 position, Midl::Matrix4x4 matrix, _Out_ Midl::Vector4* ret)
    {
        *asImpl(ret) = Impl::transform4(asImpl(position), asImpl(matrix));
        return S_OK;
    }

    IFACEMETHODIMP TransformVector3(Midl::Vector3 position, Midl::Matrix4x4 matrix, _Out_ Midl::Vector4* ret)
    {
        *asImpl(ret) = Impl::transform4(asImpl(position), asImpl(matrix));
        return S_OK;
    }

    IFACEMETHODIMP Transform(Midl::Vector4 vector, Midl::Matrix4x4 matrix, _Out_ Midl::Vector4* ret)
    {
        *asImpl(ret) = Impl::transform(asImpl(vector), asImpl(matrix));
        return S_OK;
    }

    IFACEMETHODIMP TransformVector2ByQuaternion(Midl::Vector2 value, Midl::Quaternion rotation, _Out_ Midl::Vector4* ret)
    {
        *asImpl(ret) = Impl::transform4(asImpl(value), asImpl(rotation));
        return S_OK;
    }

    IFACEMETHODIMP TransformVector3ByQuaternion(Midl::Vector3 value, Midl::Quaternion rotation, _Out_ Midl::Vector4* ret)
    {
        *asImpl(ret) = Impl::transform4(asImpl(value), asImpl(rotation));
        return S_OK;
    }

    IFACEMETHODIMP TransformByQuaternion(Midl::Vector4 value, Midl::Quaternion rotation, _Out_ Midl::Vector4* ret)
    {
        *asImpl(ret) = Impl::transform(asImpl(value), asImpl(rotation));
        return S_OK;
    }

    IFACEMETHODIMP Negate(Midl::Vector4 value, _Out_ Midl::Vector4* ret)
    {
        *asImpl(ret) = -asImpl(value);
        return S_OK;
    }

    IFACEMETHODIMP Add(Midl::Vector4 value1, Midl::Vector4 value2, _Out_ Midl::Vector4* ret)
    {
        *asImpl(ret) = asImpl(value1) + asImpl(value2);
        return S_OK;
    }

    IFACEMETHODIMP Subtract(Midl::Vector4 value1, Midl::Vector4 value2, _Out_ Midl::Vector4* ret)
    {
        *asImpl(ret) = asImpl(value1) - asImpl(value2);
        return S_OK;
    }

    IFACEMETHODIMP Multiply(Midl::Vector4 value1, Midl::Vector4 value2, _Out_ Midl::Vector4* ret)
    {
        *asImpl(ret) = asImpl(value1) * asImpl(value2);
        return S_OK;
    }

    IFACEMETHODIMP MultiplyByScalar(Midl::Vector4 value1, float value2, _Out_ Midl::Vector4* ret)
    {
        *asImpl(ret) = asImpl(value1) * value2;
        return S_OK;
    }

    IFACEMETHODIMP Divide(Midl::Vector4 value1, Midl::Vector4 value2, _Out_ Midl::Vector4* ret)
    {
        *asImpl(ret) = asImpl(value1) / asImpl(value2);
        return S_OK;
    }

    IFACEMETHODIMP DivideByScalar(Midl::Vector4 value1, float value2, _Out_ Midl::Vector4* ret)
    {
        *asImpl(ret) = asImpl(value1) / value2;
        return S_OK;
    }
};


ActivatableStaticOnlyFactory(Vector4Statics);
