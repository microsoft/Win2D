// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace AttributeHelpers
{
    class __declspec(uuid("52046A90-F359-4BF8-9126-07CAF2F51077"))
    IAttributeInternal : public IUnknown
    {
    public:
        virtual ComPtr<ID2D1SvgAttribute> GetD2DBaseAttribute() = 0;
    };

    template<typename NATIVE_TYPE, typename WRAPPED_TYPE>
    void CloneImpl(
        ClosablePtr<ICanvasDevice> closableDevice,
        ComPtr<ID2D1SvgAttribute> d2dResource,
        ICanvasSvgAttribute** result)
    {
        CheckAndClearOutPointer(result);

        auto& device = closableDevice.EnsureNotClosed();

        ComPtr<ID2D1SvgAttribute> newAttribute;
        ThrowIfFailed(d2dResource->Clone(&newAttribute));
        
        ComPtr<NATIVE_TYPE> newAttributeStrongTyped;
        ThrowIfFailed(newAttribute.As(&newAttributeStrongTyped));

        auto wrapped = Make<WRAPPED_TYPE>(device.Get(), newAttributeStrongTyped.Get());

        ThrowIfFailed(wrapped.CopyTo(result));
    }

    void GetElementImpl(
        ClosablePtr<ICanvasDevice> closableDevice,
        ComPtr<ID2D1SvgAttribute> d2dResource,
        ICanvasSvgNamedElement** result);

    void ValidateRange(int32_t startIndex, int32_t elementCount, uint32_t available);
}