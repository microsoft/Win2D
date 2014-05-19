// Copyright (c) Microsoft Corporation.  All rights reserved

#include "pch.h"
#include "MathTypeConversion.h"


class Vector2Statics : public Microsoft::WRL::ActivationFactory<Midl::IVector2HelperStatics>
{
    InspectableClassStatic(RuntimeClass_Microsoft_Graphics_Canvas_Math_Vector2Helper, BaseTrust);

public:
    IFACEMETHODIMP FromValues(float x, float y, _Out_ Midl::Vector2* ret)
    {
        *asImpl(ret) = Impl::float2(x, y);
        return S_OK;
    }

    IFACEMETHODIMP FromValue(float value, _Out_ Midl::Vector2* ret)
    {
        *asImpl(ret) = Impl::float2(value);
        return S_OK;
    }

    IFACEMETHODIMP FromPoint(ABI::Windows::Foundation::Point value, _Out_ Midl::Vector2* ret)
    {
        *asImpl(ret) = Impl::float2(value.X, value.Y);
        return S_OK;
    }

    IFACEMETHODIMP FromSize(ABI::Windows::Foundation::Size value, _Out_ Midl::Vector2* ret)
    {
        *asImpl(ret) = Impl::float2(value.Width, value.Height);
        return S_OK;
    }

    IFACEMETHODIMP ToPoint(Midl::Vector2 value, _Out_ ABI::Windows::Foundation::Point* ret)
    {
        ret->X = value.X;
        ret->Y = value.Y;
        return S_OK;
    }

    IFACEMETHODIMP ToSize(Midl::Vector2 value, _Out_ ABI::Windows::Foundation::Size* ret)
    {
        ret->Width = value.X;
        ret->Height = value.Y;
        return S_OK;
    }

    IFACEMETHODIMP get_Zero(_Out_ Midl::Vector2* ret)
    {
        *asImpl(ret) = Impl::float2::zero();
        return S_OK;
    }

    IFACEMETHODIMP get_One(_Out_ Midl::Vector2* ret)
    {
        *asImpl(ret) = Impl::float2::one();
        return S_OK;
    }

    IFACEMETHODIMP get_UnitX(_Out_ Midl::Vector2* ret)
    {
        *asImpl(ret) = Impl::float2::unit_x();
        return S_OK;
    }

    IFACEMETHODIMP get_UnitY(_Out_ Midl::Vector2* ret)
    {
        *asImpl(ret) = Impl::float2::unit_y();
        return S_OK;
    }

    IFACEMETHODIMP Length(Midl::Vector2 value, _Out_ float* ret)
    {
        *ret = Impl::length(asImpl(value));
        return S_OK;
    }

    IFACEMETHODIMP LengthSquared(Midl::Vector2 value, _Out_ float* ret)
    {
        *ret = Impl::length_squared(asImpl(value));
        return S_OK;
    }

    IFACEMETHODIMP Distance(Midl::Vector2 value1, Midl::Vector2 value2, _Out_ float* ret)
    {
        *ret = Impl::distance(asImpl(value1), asImpl(value2));
        return S_OK;
    }

    IFACEMETHODIMP DistanceSquared(Midl::Vector2 value1, Midl::Vector2 value2, _Out_ float* ret)
    {
        *ret = Impl::distance_squared(asImpl(value1), asImpl(value2));
        return S_OK;
    }

    IFACEMETHODIMP Dot(Midl::Vector2 value1, Midl::Vector2 value2, _Out_ float* ret)
    {
        *ret = Impl::dot(asImpl(value1), asImpl(value2));
        return S_OK;
    }

    IFACEMETHODIMP Normalize(Midl::Vector2 value, _Out_ Midl::Vector2* ret)
    {
        *asImpl(ret) = Impl::normalize(asImpl(value));
        return S_OK;
    }

    IFACEMETHODIMP Reflect(Midl::Vector2 vector, Midl::Vector2 normal, _Out_ Midl::Vector2* ret)
    {
        *asImpl(ret) = Impl::reflect(asImpl(vector), asImpl(normal));
        return S_OK;
    }

    IFACEMETHODIMP Min(Midl::Vector2 value1, Midl::Vector2 value2, _Out_ Midl::Vector2* ret)
    {
        *asImpl(ret) = Impl::min(asImpl(value1), asImpl(value2));
        return S_OK;
    }

    IFACEMETHODIMP Max(Midl::Vector2 value1, Midl::Vector2 value2, _Out_ Midl::Vector2* ret)
    {
        *asImpl(ret) = Impl::max(asImpl(value1), asImpl(value2));
        return S_OK;
    }

    IFACEMETHODIMP Clamp(Midl::Vector2 value1, Midl::Vector2 min, Midl::Vector2 max, _Out_ Midl::Vector2* ret)
    {
        *asImpl(ret) = Impl::clamp(asImpl(value1), asImpl(min), asImpl(max));
        return S_OK;
    }

    IFACEMETHODIMP Lerp(Midl::Vector2 value1, Midl::Vector2 value2, float amount, _Out_ Midl::Vector2* ret)
    {
        *asImpl(ret) = Impl::lerp(asImpl(value1), asImpl(value2), amount);
        return S_OK;
    }

    IFACEMETHODIMP TransformByMatrix3x2(Midl::Vector2 position, Midl::Matrix3x2 matrix, _Out_ Midl::Vector2* ret)
    {
        *asImpl(ret) = Impl::transform(asImpl(position), asImpl(matrix));
        return S_OK;
    }

    IFACEMETHODIMP Transform(Midl::Vector2 position, Midl::Matrix4x4 matrix, _Out_ Midl::Vector2* ret)
    {
        *asImpl(ret) = Impl::transform(asImpl(position), asImpl(matrix));
        return S_OK;
    }

    IFACEMETHODIMP TransformNormalByMatrix3x2(Midl::Vector2 normal, Midl::Matrix3x2 matrix, _Out_ Midl::Vector2* ret)
    {
        *asImpl(ret) = Impl::transform_normal(asImpl(normal), asImpl(matrix));
        return S_OK;
    }

    IFACEMETHODIMP TransformNormal(Midl::Vector2 normal, Midl::Matrix4x4 matrix, _Out_ Midl::Vector2* ret)
    {
        *asImpl(ret) = Impl::transform_normal(asImpl(normal), asImpl(matrix));
        return S_OK;
    }

    IFACEMETHODIMP TransformByQuaternion(Midl::Vector2 value, Midl::Quaternion rotation, _Out_ Midl::Vector2* ret)
    {
        *asImpl(ret) = Impl::transform(asImpl(value), asImpl(rotation));
        return S_OK;
    }

    IFACEMETHODIMP Negate(Midl::Vector2 value, _Out_ Midl::Vector2* ret)
    {
        *asImpl(ret) = -asImpl(value);
        return S_OK;
    }

    IFACEMETHODIMP Add(Midl::Vector2 value1, Midl::Vector2 value2, _Out_ Midl::Vector2* ret)
    {
        *asImpl(ret) = asImpl(value1) + asImpl(value2);
        return S_OK;
    }

    IFACEMETHODIMP Subtract(Midl::Vector2 value1, Midl::Vector2 value2, _Out_ Midl::Vector2* ret)
    {
        *asImpl(ret) = asImpl(value1) - asImpl(value2);
        return S_OK;
    }

    IFACEMETHODIMP Multiply(Midl::Vector2 value1, Midl::Vector2 value2, _Out_ Midl::Vector2* ret)
    {
        *asImpl(ret) = asImpl(value1) * asImpl(value2);
        return S_OK;
    }

    IFACEMETHODIMP MultiplyByScalar(Midl::Vector2 value1, float value2, _Out_ Midl::Vector2* ret)
    {
        *asImpl(ret) = asImpl(value1) * value2;
        return S_OK;
    }

    IFACEMETHODIMP Divide(Midl::Vector2 value1, Midl::Vector2 value2, _Out_ Midl::Vector2* ret)
    {
        *asImpl(ret) = asImpl(value1) / asImpl(value2);
        return S_OK;
    }

    IFACEMETHODIMP DivideByScalar(Midl::Vector2 value1, float value2, _Out_ Midl::Vector2* ret)
    {
        *asImpl(ret) = asImpl(value1) / value2;
        return S_OK;
    }
};


ActivatableStaticOnlyFactory(Vector2Statics);
