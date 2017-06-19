// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

#if WINVER > _WIN32_WINNT_WINBLUE

#include "CanvasSvgPathAttribute.h"
#include "geometry/CanvasGeometry.h"
#include "AttributeHelpers.h"
    
CanvasSvgPathAttribute::CanvasSvgPathAttribute(
    ICanvasDevice* canvasDevice,
    ID2D1SvgPathData* d2dSvgPath)
    : ResourceWrapper(d2dSvgPath)
    , m_canvasDevice(canvasDevice)
{
}

IFACEMETHODIMP CanvasSvgPathAttribute::Close()
{
    m_canvasDevice.Close();
    return ResourceWrapper::Close();
}

IFACEMETHODIMP CanvasSvgPathAttribute::CreatePathGeometry(ICanvasGeometry** result)
{
    return CreatePathGeometryWithFill(CanvasFilledRegionDetermination::Alternate, result);
}

IFACEMETHODIMP CanvasSvgPathAttribute::CreatePathGeometryWithFill(CanvasFilledRegionDetermination filledRegionDetermination, ICanvasGeometry** result)
{
    return ExceptionBoundary(
        [&]
        {
            CheckAndClearOutPointer(result);

            auto& resource = GetResource();

            auto& device = m_canvasDevice.EnsureNotClosed();

            ComPtr<ID2D1PathGeometry1> d2dGeometry;
            ThrowIfFailed(resource->CreatePathGeometry(StaticCastAs<D2D1_FILL_MODE>(filledRegionDetermination), &d2dGeometry));

            auto canvasGeometry = Make<CanvasGeometry>(device.Get(), d2dGeometry.Get());
            CheckMakeResult(canvasGeometry);

            ThrowIfFailed(canvasGeometry.CopyTo(result));
        });
}

IFACEMETHODIMP CanvasSvgPathAttribute::get_Commands(uint32_t* outputValueCount, CanvasSvgPathCommand** outputValues)
{
    return ExceptionBoundary(
        [&]
        {            
            CheckInPointer(outputValueCount);
            CheckAndClearOutPointer(outputValues);

            auto& resource = GetResource();

            auto commandCount = resource->GetCommandsCount();
            
            ComArray<CanvasSvgPathCommand> array(commandCount);
            ThrowIfFailed(resource->GetCommands(ReinterpretAs<D2D1_SVG_PATH_COMMAND*>(array.GetData()), commandCount, 0u));

            array.Detach(outputValueCount, outputValues);
        });
}

IFACEMETHODIMP CanvasSvgPathAttribute::GetCommands(int32_t startIndex, int32_t elementCount, uint32_t* outputValueCount, CanvasSvgPathCommand** outputValues)
{
    return ExceptionBoundary(
        [&]
        {            
            CheckInPointer(outputValueCount);
            CheckAndClearOutPointer(outputValues);

            auto& resource = GetResource();

            AttributeHelpers::ValidateRange(startIndex, elementCount, resource->GetCommandsCount());

            ComArray<CanvasSvgPathCommand> array(elementCount);
            ThrowIfFailed(resource->GetCommands(ReinterpretAs<D2D1_SVG_PATH_COMMAND*>(array.GetData()), elementCount, startIndex));

            array.Detach(outputValueCount, outputValues);
        });
}

IFACEMETHODIMP CanvasSvgPathAttribute::get_SegmentData(uint32_t* outputValueCount, float** outputValues)
{
    return ExceptionBoundary(
        [&]
        {            
            CheckInPointer(outputValueCount);
            CheckAndClearOutPointer(outputValues);

            auto& resource = GetResource();

            auto segmentDataCount = resource->GetSegmentDataCount();

            ComArray<float> array(segmentDataCount);
            ThrowIfFailed(resource->GetSegmentData(array.GetData(), segmentDataCount, 0u));

            array.Detach(outputValueCount, outputValues);
        });
}

IFACEMETHODIMP CanvasSvgPathAttribute::GetSegmentData(int32_t startIndex, int32_t elementCount, uint32_t* outputValueCount, float** outputValues)
{
    return ExceptionBoundary(
        [&]
        {            
            CheckInPointer(outputValueCount);
            CheckAndClearOutPointer(outputValues);

            auto& resource = GetResource();

            AttributeHelpers::ValidateRange(startIndex, elementCount, resource->GetSegmentDataCount());

            ComArray<float> array(elementCount);
            ThrowIfFailed(resource->GetSegmentData(array.GetData(), elementCount, startIndex));

            array.Detach(outputValueCount, outputValues);
        });
}

IFACEMETHODIMP CanvasSvgPathAttribute::RemoveCommandsAtEnd(int32_t commandCount)
{
    return ExceptionBoundary(
        [&]
        {            
            ThrowIfZeroOrNegative(commandCount);

            auto& resource = GetResource();

            resource->RemoveCommandsAtEnd(commandCount);
        });
}

IFACEMETHODIMP CanvasSvgPathAttribute::RemoveSegmentDataAtEnd(int32_t segmentDataCount)
{
    return ExceptionBoundary(
        [&]
        {            
            ThrowIfZeroOrNegative(segmentDataCount);

            auto& resource = GetResource();

            resource->RemoveSegmentDataAtEnd(segmentDataCount);
        });
}

IFACEMETHODIMP CanvasSvgPathAttribute::SetCommands(int32_t startIndex, uint32_t commandCount, CanvasSvgPathCommand* commands)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(commands);

            auto& resource = GetResource();

            resource->UpdateCommands(ReinterpretAs<D2D1_SVG_PATH_COMMAND const*>(commands), commandCount, startIndex);
        });
}

IFACEMETHODIMP CanvasSvgPathAttribute::SetSegmentData(int32_t startIndex, uint32_t commandCount, float* segmentData)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(segmentData);

            auto& resource = GetResource();

            resource->UpdateSegmentData(segmentData, commandCount, startIndex);
        });
}

IFACEMETHODIMP CanvasSvgPathAttribute::Clone(ICanvasSvgAttribute** result)
{
    return ExceptionBoundary(
        [&]
        {
            AttributeHelpers::CloneImpl<ID2D1SvgPathData, CanvasSvgPathAttribute>(m_canvasDevice, GetResource(), result);
        });
}

IFACEMETHODIMP CanvasSvgPathAttribute::GetElement(ICanvasSvgNamedElement** result)
{
    return ExceptionBoundary(
        [&]
        {
            AttributeHelpers::GetElementImpl(m_canvasDevice, GetResource(), result);
        });
}


IFACEMETHODIMP CanvasSvgPathAttribute::get_Device(ICanvasDevice** device)
{
    return ExceptionBoundary(
        [&]
        {
            CheckAndClearOutPointer(device);

            ThrowIfFailed(m_canvasDevice.EnsureNotClosed().CopyTo(device));
        });
}

#endif