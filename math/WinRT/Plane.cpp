// Copyright (c) Microsoft Corporation.  All rights reserved

#include "pch.h"
#include "MathTypeConversion.h"


class PlaneStatics : public Microsoft::WRL::ActivationFactory<Midl::IPlaneHelperStatics>
{
    InspectableClassStatic(RuntimeClass_Microsoft_Graphics_Canvas_Math_PlaneHelper, BaseTrust);

public:
    IFACEMETHODIMP FromValues(FLOAT a, FLOAT b, FLOAT c, FLOAT d, _Out_ Midl::Plane* ret)
    {
        *asImpl(ret) = Impl::plane(a, b, c, d);
        return S_OK;
    }

    IFACEMETHODIMP FromVector3(Midl::Vector3 normal, FLOAT d, _Out_ Midl::Plane* ret)
    {
        *asImpl(ret) = Impl::plane(asImpl(normal), d);
        return S_OK;
    }

    IFACEMETHODIMP FromVector4(Midl::Vector4 value, _Out_ Midl::Plane* ret)
    {
        *asImpl(ret) = Impl::plane(asImpl(value));
        return S_OK;
    }

    IFACEMETHODIMP CreateFromVertices(Midl::Vector3 point1, Midl::Vector3 point2, Midl::Vector3 point3, _Out_ Midl::Plane* ret)
    {
        *asImpl(ret) = Impl::plane::from_vertices(asImpl(point1), asImpl(point2), asImpl(point3));
        return S_OK;
    }

    IFACEMETHODIMP Normalize(Midl::Plane value, _Out_ Midl::Plane* ret)
    {
        *asImpl(ret) = Impl::normalize(asImpl(value));
        return S_OK;
    }

    IFACEMETHODIMP Transform(Midl::Plane plane, Midl::Matrix4x4 matrix, _Out_ Midl::Plane* ret)
    {
        *asImpl(ret) = Impl::transform(asImpl(plane), asImpl(matrix));
        return S_OK;
    }

    IFACEMETHODIMP TransformByQuaternion(Midl::Plane plane, Midl::Quaternion rotation, _Out_ Midl::Plane* ret)
    {
        *asImpl(ret) = Impl::transform(asImpl(plane), asImpl(rotation));
        return S_OK;
    }

    IFACEMETHODIMP Dot(Midl::Plane plane, Midl::Vector4 value, _Out_ FLOAT* ret)
    {
        *ret = Impl::dot(asImpl(plane), asImpl(value));
        return S_OK;
    }

    IFACEMETHODIMP DotCoordinate(Midl::Plane plane, Midl::Vector3 value, _Out_ FLOAT* ret)
    {
        *ret = Impl::dot_coordinate(asImpl(plane), asImpl(value));
        return S_OK;
    }

    IFACEMETHODIMP DotNormal(Midl::Plane plane, Midl::Vector3 value, _Out_ FLOAT* ret)
    {
        *ret = Impl::dot_normal(asImpl(plane), asImpl(value));
        return S_OK;
    }
};


ActivatableStaticOnlyFactory(PlaneStatics);
