// Copyright (c) Microsoft Corporation.  All rights reserved

#include "pch.h"
#include "MathTypeConversion.h"


class Matrix3x2Statics : public Microsoft::WRL::ActivationFactory<Midl::IMatrix3x2HelperStatics>
{
    InspectableClassStatic(RuntimeClass_Microsoft_Graphics_Canvas_Math_Matrix3x2Helper, BaseTrust);

public:
    IFACEMETHODIMP FromValues(float m11, float m12, float m21, float m22, float m31, float m32, _Out_ Midl::Matrix3x2* ret)
    {
        *asImpl(ret) = Impl::float3x2(m11, m12, m21, m22, m31, m32);
        return S_OK;
    }

    IFACEMETHODIMP get_Identity(_Out_ Midl::Matrix3x2* ret)
    {
        *asImpl(ret) = Impl::float3x2::identity();
        return S_OK;
    }

    IFACEMETHODIMP IsIdentity(Midl::Matrix3x2 value, _Out_ boolean* ret)
    {
        *ret = Impl::is_identity(asImpl(value));
        return S_OK;
    }

    IFACEMETHODIMP Determinant(Midl::Matrix3x2 value, _Out_ float* ret)
    {
        *ret = Impl::determinant(asImpl(value));
        return S_OK;
    }

    IFACEMETHODIMP Translation(Midl::Matrix3x2 value, _Out_ Midl::Vector2* ret)
    {
        *asImpl(ret) = Impl::translation(asImpl(value));
        return S_OK;
    }

    IFACEMETHODIMP CreateTranslation(Midl::Vector2 position, _Out_ Midl::Matrix3x2* ret)
    {
        *asImpl(ret) = Impl::float3x2::translation(asImpl(position));
        return S_OK;
    }

    IFACEMETHODIMP CreateTranslationFromValues(float xPosition, float yPosition, _Out_ Midl::Matrix3x2* ret)
    {
        *asImpl(ret) = Impl::float3x2::translation(xPosition, yPosition);
        return S_OK;
    }

    IFACEMETHODIMP CreateScale(float scale, _Out_ Midl::Matrix3x2* ret)
    {
        *asImpl(ret) = Impl::float3x2::scale(scale);
        return S_OK;
    }

    IFACEMETHODIMP CreateScaleFromValues(float xScale, float yScale, _Out_ Midl::Matrix3x2* ret)
    {
        *asImpl(ret) = Impl::float3x2::scale(xScale, yScale);
        return S_OK;
    }

    IFACEMETHODIMP CreateScaleFromValuesAroundCenter(float xScale, float yScale, Midl::Vector2 centerPoint, _Out_ Midl::Matrix3x2* ret)
    {
        *asImpl(ret) = Impl::float3x2::scale(xScale, yScale, asImpl(centerPoint));
        return S_OK;
    }

    IFACEMETHODIMP CreateSkew(float radiansX, float radiansY, _Out_ Midl::Matrix3x2* ret)
    {
        *asImpl(ret) = Impl::float3x2::skew(radiansX, radiansY);
        return S_OK;
    }

    IFACEMETHODIMP CreateSkewAroundCenter(float radiansX, float radiansY, Midl::Vector2 centerPoint, _Out_ Midl::Matrix3x2* ret)
    {
        *asImpl(ret) = Impl::float3x2::skew(radiansX, radiansY, asImpl(centerPoint));
        return S_OK;
    }

    IFACEMETHODIMP CreateRotation(float radians, _Out_ Midl::Matrix3x2* ret)
    {
        *asImpl(ret) = Impl::float3x2::rotation(radians);
        return S_OK;
    }

    IFACEMETHODIMP CreateRotationAroundCenter(float radians, Midl::Vector2 centerPoint, _Out_ Midl::Matrix3x2* ret)
    {
        *asImpl(ret) = Impl::float3x2::rotation(radians, asImpl(centerPoint));
        return S_OK;
    }

    IFACEMETHODIMP Invert(Midl::Matrix3x2 matrix, _Out_ Midl::Matrix3x2* result, _Out_ boolean* ret)
    {
        *ret = Impl::invert(asImpl(matrix), asImpl(result));
        return S_OK;
    }

    IFACEMETHODIMP Lerp(Midl::Matrix3x2 matrix1, Midl::Matrix3x2 matrix2, float amount, _Out_ Midl::Matrix3x2* ret)
    {
        *asImpl(ret) = Impl::lerp(asImpl(matrix1), asImpl(matrix2), amount);
        return S_OK;
    }

    IFACEMETHODIMP Negate(Midl::Matrix3x2 value, _Out_ Midl::Matrix3x2* ret)
    {
        *asImpl(ret) = -asImpl(value);
        return S_OK;
    }

    IFACEMETHODIMP Add(Midl::Matrix3x2 value1, Midl::Matrix3x2 value2, _Out_ Midl::Matrix3x2* ret)
    {
        *asImpl(ret) = asImpl(value1) + asImpl(value2);
        return S_OK;
    }

    IFACEMETHODIMP Subtract(Midl::Matrix3x2 value1, Midl::Matrix3x2 value2, _Out_ Midl::Matrix3x2* ret)
    {
        *asImpl(ret) = asImpl(value1) - asImpl(value2);
        return S_OK;
    }

    IFACEMETHODIMP Multiply(Midl::Matrix3x2 value1, Midl::Matrix3x2 value2, _Out_ Midl::Matrix3x2* ret)
    {
        *asImpl(ret) = asImpl(value1) * asImpl(value2);
        return S_OK;
    }

    IFACEMETHODIMP MultiplyByScalar(Midl::Matrix3x2 value1, float value2, _Out_ Midl::Matrix3x2* ret)
    {
        *asImpl(ret) = asImpl(value1) * value2;
        return S_OK;
    }
};


ActivatableStaticOnlyFactory(Matrix3x2Statics);
