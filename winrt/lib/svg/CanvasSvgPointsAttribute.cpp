// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

#include "CanvasSvgPointsAttribute.h"
#include "AttributeHelpers.h"
    
CanvasSvgPointsAttribute::CanvasSvgPointsAttribute(
    ICanvasDevice* canvasDevice,
    ID2D1SvgPointCollection* d2dSvgPoints)
    : ResourceWrapper(d2dSvgPoints)
    , m_canvasDevice(canvasDevice)
{
}

IFACEMETHODIMP CanvasSvgPointsAttribute::Close()
{
    m_canvasDevice.Close();
    return ResourceWrapper::Close();
}

IFACEMETHODIMP CanvasSvgPointsAttribute::get_Points(uint32_t* outputValueCount, Vector2** outputValues)
{
    return ExceptionBoundary(
        [&]
        {            
            CheckInPointer(outputValueCount);
            CheckAndClearOutPointer(outputValues);

            auto& resource = GetResource();

            auto pointsCount = resource->GetPointsCount();
            
            ComArray<Vector2> array(pointsCount);
            ThrowIfFailed(resource->GetPoints(ReinterpretAs<D2D1_POINT_2F*>(array.GetData()), pointsCount, 0u));

            array.Detach(outputValueCount, outputValues);
        });
}

IFACEMETHODIMP CanvasSvgPointsAttribute::GetPoints(int32_t startIndex, int32_t elementCount, uint32_t* outputValueCount, Vector2** outputValues)
{
    return ExceptionBoundary(
        [&]
        {            
            CheckInPointer(outputValueCount);
            CheckAndClearOutPointer(outputValues);

            auto& resource = GetResource();

            AttributeHelpers::ValidateRange(startIndex, elementCount, resource->GetPointsCount());

            ComArray<Vector2> array(elementCount);
            ThrowIfFailed(resource->GetPoints(ReinterpretAs<D2D1_POINT_2F*>(array.GetData()), elementCount, startIndex));

            array.Detach(outputValueCount, outputValues);
        });
}

IFACEMETHODIMP CanvasSvgPointsAttribute::RemovePointsAtEnd(int32_t count)
{
    return ExceptionBoundary(
        [&]
        {            
            ThrowIfZeroOrNegative(count);

            auto& resource = GetResource();

            resource->RemovePointsAtEnd(count);
        });
}

IFACEMETHODIMP CanvasSvgPointsAttribute::SetPoints(int32_t startIndex, uint32_t pointCount, Vector2* points)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(points);

            auto& resource = GetResource();

            resource->UpdatePoints(ReinterpretAs<D2D1_POINT_2F const*>(points), pointCount, startIndex);
        });

}

IFACEMETHODIMP CanvasSvgPointsAttribute::Clone(ICanvasSvgAttribute** result)
{
    return ExceptionBoundary(
        [&]
        {
            AttributeHelpers::CloneImpl<ID2D1SvgPointCollection, CanvasSvgPointsAttribute>(m_canvasDevice, GetResource(), result);
        });
}

IFACEMETHODIMP CanvasSvgPointsAttribute::GetElement(ICanvasSvgNamedElement** result)
{
    return ExceptionBoundary(
        [&]
        {
            AttributeHelpers::GetElementImpl(m_canvasDevice, GetResource(), result);
        });
}


IFACEMETHODIMP CanvasSvgPointsAttribute::get_Device(ICanvasDevice** device)
{
    return ExceptionBoundary(
        [&]
        {
            CheckAndClearOutPointer(device);

            ThrowIfFailed(m_canvasDevice.EnsureNotClosed().CopyTo(device));
        });
}