// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

#if WINVER > _WIN32_WINNT_WINBLUE

#include "CanvasSvgElement.h"
#include "CanvasSvgDocument.h"
#include "BufferStreamWrapper.h"

using namespace ABI::Microsoft::Graphics::Canvas::Svg;
using namespace Microsoft::WRL::Wrappers;

// static
ComPtr<CanvasSvgElement> CanvasSvgElement::CreateNew(CanvasSvgDocument* parent, IStream* stream)
{
    ComPtr<ID2D1SvgElement> d2dSvgElement;
    ThrowIfFailed(parent->GetResource()->Deserialize(stream, &d2dSvgElement));
    
    auto canvasSvgElement = Make<CanvasSvgElement>(
        parent->GetDevice().Get(),
        d2dSvgElement.Get());
    CheckMakeResult(canvasSvgElement);

    return canvasSvgElement;
}

CanvasSvgElement::CanvasSvgElement(
    ICanvasDevice* canvasDevice,
    ID2D1SvgElement* d2dSvgElement)
    : ResourceWrapper(d2dSvgElement)
    , m_canvasDevice(canvasDevice)
{
}

IFACEMETHODIMP CanvasSvgElement::Close()
{
    m_canvasDevice.Close();
    return ResourceWrapper::Close();
}

IFACEMETHODIMP CanvasSvgElement::get_Device(ICanvasDevice** device)
{
    return ExceptionBoundary(
        [&]
        {
            CheckAndClearOutPointer(device);

            ThrowIfFailed(m_canvasDevice.EnsureNotClosed().CopyTo(device));
        });
}

IFACEMETHODIMP CanvasSvgElement::AppendChild(ICanvasSvgElement* child)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(child);

            auto& resource = GetResource();

            auto childImplementation = static_cast<CanvasSvgElement*>(child);
            VerifyDeviceBoundary(m_canvasDevice, childImplementation);

            ThrowIfFailed(resource->AppendChild(childImplementation->GetResource().Get()));
        });
}

#endif