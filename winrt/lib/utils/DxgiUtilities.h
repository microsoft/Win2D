// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas
{
    using namespace ::Microsoft::WRL;
    using namespace WinRTDirectX;

    inline ComPtr<ICanvasDevice> GetCanvasDevice(ICanvasResourceCreator* resourceCreator)
    {
        ComPtr<ICanvasDevice> device;
        ThrowIfFailed(resourceCreator->get_Device(&device));

        return device;
    }

    template<typename T, typename U>
    ComPtr<T> GetDXGIInterfaceFromResourceCreator(U* resourceCreator)
    {
        auto device = GetCanvasDevice(resourceCreator);
        return GetDXGIInterface<T>(device.Get());
    }

    template<typename T>
    ComPtr<T> GetDXGIInterface(IDirect3DDxgiInterfaceAccess* dxgiInterfaceAccess)
    {
        ComPtr<T> dxgiInterface;
        ThrowIfFailed(dxgiInterfaceAccess->GetInterface(IID_PPV_ARGS(&dxgiInterface)));
        return dxgiInterface;
    }

    template<typename T, typename U>
    ComPtr<T> GetDXGIInterface(U* obj)
    {
        return GetDXGIInterface<T>(As<IDirect3DDxgiInterfaceAccess>(obj).Get());
    }

    template<typename T, typename U>
    ComPtr<T> GetDXGIInterface(ComPtr<U> obj)
    {
        return GetDXGIInterface<T>(As<IDXGIInterfaceAccess>(obj).Get());
    }

    template<typename T>
    float GetDpi(T const& obj)
    {
        float dpiX, dpiY;

        obj->GetDpi(&dpiX, &dpiY);

        return dpiX;
    }

    inline bool TargetIsCommandList(ID2D1DeviceContext* deviceContext)
    {
        ComPtr<ID2D1Image> target;
        deviceContext->GetTarget(&target);

        return MaybeAs<ID2D1CommandList>(target) != nullptr;
    }

}}}}
