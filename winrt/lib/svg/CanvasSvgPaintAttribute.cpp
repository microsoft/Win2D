// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

#include "CanvasSvgPaintAttribute.h"
#include "AttributeHelpers.h"
    
CanvasSvgPaintAttribute::CanvasSvgPaintAttribute(
    ICanvasDevice* canvasDevice,
    ID2D1SvgPaint* d2dSvgPaint)
    : ResourceWrapper(d2dSvgPaint)
    , m_canvasDevice(canvasDevice)
{
}

IFACEMETHODIMP CanvasSvgPaintAttribute::Close()
{
    m_canvasDevice.Close();
    return ResourceWrapper::Close();
}


IFACEMETHODIMP CanvasSvgPaintAttribute::get_PaintType(CanvasSvgPaintType* value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(value);

            *value = StaticCastAs<CanvasSvgPaintType>(GetResource()->GetPaintType());
        });
}

IFACEMETHODIMP CanvasSvgPaintAttribute::put_PaintType(CanvasSvgPaintType value)
{
    return ExceptionBoundary(
        [&]
        {
            ThrowIfFailed(GetResource()->SetPaintType(StaticCastAs<D2D1_SVG_PAINT_TYPE>(value)));
        });
}

IFACEMETHODIMP CanvasSvgPaintAttribute::get_Color(Color* value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(value);

            D2D1_COLOR_F paintColor;
            GetResource()->GetColor(&paintColor);

            *value = ToWindowsColor(paintColor);
        });
}

IFACEMETHODIMP CanvasSvgPaintAttribute::put_Color(Color value)
{
    return ExceptionBoundary(
        [&]
        {                
            GetResource()->SetColor(ToD2DColor(value));
        });
}


IFACEMETHODIMP CanvasSvgPaintAttribute::get_Id(HSTRING* value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(value);

            auto& resource = GetResource();

            uint32_t bufferLength = resource->GetIdLength() + 1; // Account for null
            WinStringBuilder stringBuilder;
            auto buffer = stringBuilder.Allocate(bufferLength);

            ThrowIfFailed(resource->GetId(buffer, bufferLength));

            stringBuilder.Get().CopyTo(value);
        });
}

IFACEMETHODIMP CanvasSvgPaintAttribute::put_Id(HSTRING value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(value);

            auto& resource = GetResource();

            ThrowIfFailed(resource->SetId(GetStringBuffer(value)));
        });
}


IFACEMETHODIMP CanvasSvgPaintAttribute::Clone(ICanvasSvgAttribute** result)
{
    return ExceptionBoundary(
        [&]
        {
            AttributeHelpers::CloneImpl<ID2D1SvgPaint, CanvasSvgPaintAttribute>(m_canvasDevice, GetResource(), result);
        });
}

IFACEMETHODIMP CanvasSvgPaintAttribute::GetElement(ICanvasSvgNamedElement** result)
{
    return ExceptionBoundary(
        [&]
        {
            AttributeHelpers::GetElementImpl(m_canvasDevice, GetResource(), result);
        });
}


IFACEMETHODIMP CanvasSvgPaintAttribute::get_Device(ICanvasDevice** device)
{
    return ExceptionBoundary(
        [&]
        {
            CheckAndClearOutPointer(device);

            ThrowIfFailed(m_canvasDevice.EnsureNotClosed().CopyTo(device));
        });
}