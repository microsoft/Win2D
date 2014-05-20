// Copyright (c) Microsoft Corporation.  All rights reserved

#include "pch.h"
#include "MathTypeConversion.h"


class Vector4Statics : public Microsoft::WRL::ActivationFactory<Midl::IVector4HelperStatics>
{
    InspectableClassStatic(RuntimeClass_Microsoft_DirectX_Math_Vector4Helper, BaseTrust);

public:
    IFACEMETHODIMP FromValues(float x, float y, float z, float w, _Out_ Midl::Vector4* ret)
    {
        *asImpl(ret) = Impl::Vector4(x, y, z, w);
        return S_OK;
    }

    IFACEMETHODIMP FromVector2(Midl::Vector2 value, float z, float w, _Out_ Midl::Vector4* ret)
    {
        *asImpl(ret) = Impl::Vector4(asImpl(value), z, w);
        return S_OK;
    }

    IFACEMETHODIMP FromVector3(Midl::Vector3 value, float w, _Out_ Midl::Vector4* ret)
    {
        *asImpl(ret) = Impl::Vector4(asImpl(value), w);
        return S_OK;
    }

    IFACEMETHODIMP FromValue(float value, _Out_ Midl::Vector4* ret)
    {
        *asImpl(ret) = Impl::Vector4(value);
        return S_OK;
    }

    IFACEMETHODIMP get_Zero(_Out_ Midl::Vector4* ret)
    {
        *asImpl(ret) = Impl::Vector4::Zero();
        return S_OK;
    }

    IFACEMETHODIMP get_One(_Out_ Midl::Vector4* ret)
    {
        *asImpl(ret) = Impl::Vector4::One();
        return S_OK;
    }

    IFACEMETHODIMP get_UnitX(_Out_ Midl::Vector4* ret)
    {
        *asImpl(ret) = Impl::Vector4::UnitX();
        return S_OK;
    }

    IFACEMETHODIMP get_UnitY(_Out_ Midl::Vector4* ret)
    {
        *asImpl(ret) = Impl::Vector4::UnitY();
        return S_OK;
    }

    IFACEMETHODIMP get_UnitZ(_Out_ Midl::Vector4* ret)
    {
        *asImpl(ret) = Impl::Vector4::UnitZ();
        return S_OK;
    }

    IFACEMETHODIMP get_UnitW(_Out_ Midl::Vector4* ret)
    {
        *asImpl(ret) = Impl::Vector4::UnitW();
        return S_OK;
    }

    IFACEMETHODIMP Length(Midl::Vector4 value, _Out_ float* ret)
    {
        *ret = asImpl(value).Length();
        return S_OK;
    }

    IFACEMETHODIMP LengthSquared(Midl::Vector4 value, _Out_ float* ret)
    {
        *ret = asImpl(value).LengthSquared();
        return S_OK;
    }

    IFACEMETHODIMP Distance(Midl::Vector4 value1, Midl::Vector4 value2, _Out_ float* ret)
    {
        *ret = Impl::Vector4::Distance(asImpl(value1), asImpl(value2));
        return S_OK;
    }

    IFACEMETHODIMP DistanceSquared(Midl::Vector4 value1, Midl::Vector4 value2, _Out_ float* ret)
    {
        *ret = Impl::Vector4::DistanceSquared(asImpl(value1), asImpl(value2));
        return S_OK;
    }

    IFACEMETHODIMP Dot(Midl::Vector4 vector1, Midl::Vector4 vector2, _Out_ float* ret)
    {
        *ret = Impl::Vector4::Dot(asImpl(vector1), asImpl(vector2));
        return S_OK;
    }

    IFACEMETHODIMP Normalize(Midl::Vector4 vector, _Out_ Midl::Vector4* ret)
    {
        *asImpl(ret) = Impl::Vector4::Normalize(asImpl(vector));
        return S_OK;
    }

    IFACEMETHODIMP Min(Midl::Vector4 value1, Midl::Vector4 value2, _Out_ Midl::Vector4* ret)
    {
        *asImpl(ret) = Impl::Vector4::Min(asImpl(value1), asImpl(value2));
        return S_OK;
    }

    IFACEMETHODIMP Max(Midl::Vector4 value1, Midl::Vector4 value2, _Out_ Midl::Vector4* ret)
    {
        *asImpl(ret) = Impl::Vector4::Max(asImpl(value1), asImpl(value2));
        return S_OK;
    }

    IFACEMETHODIMP Clamp(Midl::Vector4 value1, Midl::Vector4 min, Midl::Vector4 max, _Out_ Midl::Vector4* ret)
    {
        *asImpl(ret) = Impl::Vector4::Clamp(asImpl(value1), asImpl(min), asImpl(max));
        return S_OK;
    }

    IFACEMETHODIMP Lerp(Midl::Vector4 value1, Midl::Vector4 value2, float amount, _Out_ Midl::Vector4* ret)
    {
        *asImpl(ret) = Impl::Vector4::Lerp(asImpl(value1), asImpl(value2), amount);
        return S_OK;
    }

    IFACEMETHODIMP TransformVector2(Midl::Vector2 position, Midl::Matrix4x4 matrix, _Out_ Midl::Vector4* ret)
    {
        *asImpl(ret) = Impl::Vector4::Transform(asImpl(position), asImpl(matrix));
        return S_OK;
    }

    IFACEMETHODIMP TransformVector3(Midl::Vector3 position, Midl::Matrix4x4 matrix, _Out_ Midl::Vector4* ret)
    {
        *asImpl(ret) = Impl::Vector4::Transform(asImpl(position), asImpl(matrix));
        return S_OK;
    }

    IFACEMETHODIMP Transform(Midl::Vector4 vector, Midl::Matrix4x4 matrix, _Out_ Midl::Vector4* ret)
    {
        *asImpl(ret) = Impl::Vector4::Transform(asImpl(vector), asImpl(matrix));
        return S_OK;
    }

    IFACEMETHODIMP TransformVector2ByQuaternion(Midl::Vector2 value, Midl::Quaternion rotation, _Out_ Midl::Vector4* ret)
    {
        *asImpl(ret) = Impl::Vector4::Transform(asImpl(value), asImpl(rotation));
        return S_OK;
    }

    IFACEMETHODIMP TransformVector3ByQuaternion(Midl::Vector3 value, Midl::Quaternion rotation, _Out_ Midl::Vector4* ret)
    {
        *asImpl(ret) = Impl::Vector4::Transform(asImpl(value), asImpl(rotation));
        return S_OK;
    }

    IFACEMETHODIMP TransformByQuaternion(Midl::Vector4 value, Midl::Quaternion rotation, _Out_ Midl::Vector4* ret)
    {
        *asImpl(ret) = Impl::Vector4::Transform(asImpl(value), asImpl(rotation));
        return S_OK;
    }

    IFACEMETHODIMP Negate(Midl::Vector4 value, _Out_ Midl::Vector4* ret)
    {
        *asImpl(ret) = Impl::Vector4::Negate(asImpl(value));
        return S_OK;
    }

    IFACEMETHODIMP Add(Midl::Vector4 value1, Midl::Vector4 value2, _Out_ Midl::Vector4* ret)
    {
        *asImpl(ret) = Impl::Vector4::Add(asImpl(value1), asImpl(value2));
        return S_OK;
    }

    IFACEMETHODIMP Subtract(Midl::Vector4 value1, Midl::Vector4 value2, _Out_ Midl::Vector4* ret)
    {
        *asImpl(ret) = Impl::Vector4::Subtract(asImpl(value1), asImpl(value2));
        return S_OK;
    }

    IFACEMETHODIMP Multiply(Midl::Vector4 value1, Midl::Vector4 value2, _Out_ Midl::Vector4* ret)
    {
        *asImpl(ret) = Impl::Vector4::Multiply(asImpl(value1), asImpl(value2));
        return S_OK;
    }

    IFACEMETHODIMP MultiplyByScalar(Midl::Vector4 value1, float value2, _Out_ Midl::Vector4* ret)
    {
        *asImpl(ret) = Impl::Vector4::Multiply(asImpl(value1), value2);
        return S_OK;
    }

    IFACEMETHODIMP Divide(Midl::Vector4 value1, Midl::Vector4 value2, _Out_ Midl::Vector4* ret)
    {
        *asImpl(ret) = Impl::Vector4::Divide(asImpl(value1), asImpl(value2));
        return S_OK;
    }

    IFACEMETHODIMP DivideByScalar(Midl::Vector4 value1, float value2, _Out_ Midl::Vector4* ret)
    {
        *asImpl(ret) = Impl::Vector4::Divide(asImpl(value1), value2);
        return S_OK;
    }
};


ActivatableStaticOnlyFactory(Vector4Statics);
