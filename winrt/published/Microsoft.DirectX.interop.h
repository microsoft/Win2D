// Copyright (c) Microsoft Corporation.  All rights reserved

#pragma once

#include <inspectable.h>
#include <dxgi.h>

STDAPI CreateGraphicsDeviceFromDXGIDevice(
    _In_ IDXGIDevice* dxgiDevice,
    _Out_ IInspectable** graphicsDevice);

STDAPI CreateGraphicsSurfaceFromDXGISurface(
    _In_ IDXGISurface* dgxiSurface,
    _Out_ IInspectable** graphicsSurface);

STDAPI GetDXGIInterfaceFromGraphicsDevice(
    _In_ IInspectable* graphicsDevice,
    _In_ REFIID iid,
    _Out_ void** p);

STDAPI GetDXGIInterfaceFromGraphicsSurface(
    _In_ IInspectable* graphicsSurface,
    _In_ REFIID iid,
    _Out_ void** p);




#if defined(__cplusplus_winrt)

#include <wrl.h>

namespace Microsoft
{
    namespace DirectX
    {
        inline GraphicsDevice^ CreateGraphicsDevice(
            _In_ IDXGIDevice* dxgiDevice)
        {
            using Microsoft::WRL::ComPtr;

            ComPtr<IInspectable> inspectableGraphicsDevice;
            __abi_ThrowIfFailed(CreateGraphicsDeviceFromDXGIDevice(
                dxgiDevice, 
                &inspectableGraphicsDevice));

            return reinterpret_cast<GraphicsDevice^>(inspectableGraphicsDevice.Detach());
        }

        inline GraphicsSurface^ CreateGraphicsSurface(
            _In_ IDXGISurface* dxgiSurface)
        {
            using Microsoft::WRL::ComPtr;

            ComPtr<IInspectable> inspectableGraphicsSurface;
            __abi_ThrowIfFailed(CreateGraphicsSurfaceFromDXGISurface(
                dxgiSurface,
                &inspectableGraphicsSurface));

            return reinterpret_cast<GraphicsSurface^>(inspectableGraphicsSurface.Detach());
        }

        template<typename DXGI_TYPE>
        inline HRESULT GetDXGIInterface(
            _In_  GraphicsDevice^ graphicsDevice, 
            _Out_ DXGI_TYPE** dxgi)
        {
            return GetDXGIInterfaceFromGraphicsDevice(
                reinterpret_cast<IInspectable*>(graphicsDevice),
                IID_PPV_ARGS(dxgi));
        }

        template<typename DXGI_TYPE>
        inline HRESULT GetDXGIInterface(
            _In_ GraphicsSurface^ graphicsSurface,
            _Out_ DXGI_TYPE** dxgi)
        {
            return GetDXGIInterfaceFromGraphicsSurface(
                reinterpret_cast<IInspectable*>(graphicsSurface),
                IID_PPV_ARGS(dxgi));                
        }
    }
}

#endif // __cplusplus_winrt

