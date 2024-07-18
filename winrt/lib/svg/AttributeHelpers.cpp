// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

namespace AttributeHelpers
{
    void GetElementImpl(
        ClosablePtr<ICanvasDevice> closableDevice,
        ComPtr<ID2D1SvgAttribute> d2dResource,
        ICanvasSvgNamedElement** result)
    {
        CheckAndClearOutPointer(result);

        auto& device = closableDevice.EnsureNotClosed();

        ComPtr<ID2D1SvgElement> d2dElement;
        d2dResource->GetElement(&d2dElement);

        if (d2dElement)
        {
            ComPtr<ICanvasSvgNamedElement> wrapped = ResourceManager::GetOrCreate<ICanvasSvgNamedElement>(device.Get(), d2dElement.Get());
            ThrowIfFailed(wrapped.CopyTo(result));
        }
    }

    void ValidateRange(int32_t startIndex, int32_t elementCount, uint32_t available)
    {
        ThrowIfNegative(startIndex);
        ThrowIfZeroOrNegative(elementCount);

        if (static_cast<uint32_t>(startIndex + elementCount) > available)
        {
            ThrowHR(E_INVALIDARG);
        }
    }
}