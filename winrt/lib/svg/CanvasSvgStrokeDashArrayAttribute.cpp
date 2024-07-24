// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

#include "CanvasSvgStrokeDashArrayAttribute.h"
#include "AttributeHelpers.h"
    
CanvasSvgStrokeDashArrayAttribute::CanvasSvgStrokeDashArrayAttribute(
    ICanvasDevice* canvasDevice,
    ID2D1SvgStrokeDashArray* d2dSvgStrokeDashArray)
    : ResourceWrapper(d2dSvgStrokeDashArray)
    , m_canvasDevice(canvasDevice)
{
}

IFACEMETHODIMP CanvasSvgStrokeDashArrayAttribute::Close()
{
    m_canvasDevice.Close();
    return ResourceWrapper::Close();
}

IFACEMETHODIMP CanvasSvgStrokeDashArrayAttribute::GetDashes(uint32_t* outputValueCount, float** outputValues)
{
    return ExceptionBoundary(
        [&]
        {            
            CheckInPointer(outputValueCount);
            CheckAndClearOutPointer(outputValues);

            auto& resource = GetResource();

            auto dashesCount = resource->GetDashesCount();
            
            ComArray<float> array(dashesCount);
            ThrowIfFailed(resource->GetDashes(array.GetData(), dashesCount, 0u));

            array.Detach(outputValueCount, outputValues);
        });
}

IFACEMETHODIMP CanvasSvgStrokeDashArrayAttribute::GetDashesWithUnits(
    int32_t startIndex, 
    int32_t elementCount, 
    uint32_t* outputUnitsCount, 
    CanvasSvgLengthUnits** outputUnitsElements, 
    uint32_t* outputValueCount, 
    float**outputValueElements)
{
    return ExceptionBoundary(
        [&]
        {            
            CheckInPointer(outputUnitsCount);
            CheckInPointer(outputValueCount);
            CheckAndClearOutPointer(outputUnitsElements);
            CheckAndClearOutPointer(outputValueElements);

            auto& resource = GetResource();

            AttributeHelpers::ValidateRange(startIndex, elementCount, resource->GetDashesCount());
            
            std::vector<D2D1_SVG_LENGTH> d2dLengths;
            d2dLengths.resize(elementCount);

            ThrowIfFailed(resource->GetDashes(d2dLengths.data(), elementCount, startIndex));

            ComArray<CanvasSvgLengthUnits> unitsArray(elementCount);
            ComArray<float> valuesArray(elementCount);

            for (int32_t i = 0; i < elementCount; ++i)
            {
                unitsArray[i] = StaticCastAs<CanvasSvgLengthUnits>(d2dLengths[i].units);
                valuesArray[i] = d2dLengths[i].value;
            }

            unitsArray.Detach(outputUnitsCount, outputUnitsElements);
            valuesArray.Detach(outputValueCount, outputValueElements);
        });
}


IFACEMETHODIMP CanvasSvgStrokeDashArrayAttribute::RemoveDashesAtEnd(int32_t count)
{
    return ExceptionBoundary(
        [&]
        {            
            ThrowIfZeroOrNegative(count);

            auto& resource = GetResource();

            resource->RemoveDashesAtEnd(count);
        });
}

IFACEMETHODIMP CanvasSvgStrokeDashArrayAttribute::SetDashes(int32_t startIndex, uint32_t dashCount, float* dashes)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(dashes);

            auto& resource = GetResource();

            resource->UpdateDashes(dashes, dashCount, startIndex);
        });
}

IFACEMETHODIMP CanvasSvgStrokeDashArrayAttribute::SetDashesWithUnit(int32_t startIndex, uint32_t dashCount, float* dashValues, CanvasSvgLengthUnits dashUnits)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(dashValues);

            auto& resource = GetResource();

            std::vector<D2D1_SVG_LENGTH> d2dSvgLengths;
            d2dSvgLengths.resize(dashCount);

            auto d2dUnits = StaticCastAs<D2D1_SVG_LENGTH_UNITS>(dashUnits);

            for (uint32_t i = 0; i < dashCount; ++i)
            {
                d2dSvgLengths[i].units = d2dUnits;
                d2dSvgLengths[i].value = dashValues[i];
            }

            resource->UpdateDashes(d2dSvgLengths.data(), dashCount, startIndex);
        });
}

std::vector<D2D1_SVG_LENGTH> CanvasSvgStrokeDashArrayAttribute::GetD2DSvgLengths(uint32_t dashCount, float* dashValues, CanvasSvgLengthUnits* dashUnits)
{
    std::vector<D2D1_SVG_LENGTH> d2dSvgLengths;
    d2dSvgLengths.resize(dashCount);

    for (uint32_t i = 0; i < dashCount; ++i)
    {
        d2dSvgLengths[i].units = StaticCastAs<D2D1_SVG_LENGTH_UNITS>(dashUnits[i]);
        d2dSvgLengths[i].value = dashValues[i];
    }

    return std::move(d2dSvgLengths);
}

IFACEMETHODIMP CanvasSvgStrokeDashArrayAttribute::SetDashesWithUnits(int32_t startIndex, uint32_t dashCount, float* dashValues, uint32_t unitsCount, CanvasSvgLengthUnits* dashUnits)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(dashValues);
            CheckInPointer(dashUnits);

            auto& resource = GetResource();

            if (dashCount != unitsCount)
            {
                ThrowHR(E_INVALIDARG, Strings::SvgStrokeDashArrayMismatchingArraySizes);
            }

            std::vector<D2D1_SVG_LENGTH> d2dSvgLengths = GetD2DSvgLengths(dashCount, dashValues, dashUnits);

            resource->UpdateDashes(d2dSvgLengths.data(), dashCount, startIndex);
        });
}

IFACEMETHODIMP CanvasSvgStrokeDashArrayAttribute::Clone(ICanvasSvgAttribute** result)
{
    return ExceptionBoundary(
        [&]
        {
            AttributeHelpers::CloneImpl<ID2D1SvgStrokeDashArray, CanvasSvgStrokeDashArrayAttribute>(m_canvasDevice, GetResource(), result);
        });
}

IFACEMETHODIMP CanvasSvgStrokeDashArrayAttribute::GetElement(ICanvasSvgNamedElement** result)
{
    return ExceptionBoundary(
        [&]
        {
            AttributeHelpers::GetElementImpl(m_canvasDevice, GetResource(), result);
        });
}


IFACEMETHODIMP CanvasSvgStrokeDashArrayAttribute::get_Device(ICanvasDevice** device)
{
    return ExceptionBoundary(
        [&]
        {
            CheckAndClearOutPointer(device);

            ThrowIfFailed(m_canvasDevice.EnsureNotClosed().CopyTo(device));
        });
}