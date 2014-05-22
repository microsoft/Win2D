// Copyright (c) Microsoft Corporation.  All rights reserved

#pragma once

#include <inspectable.h>
#include <dxgi.h>

STDAPI CreateDirectX11DeviceFromDXGIDevice(
    _In_ IDXGIDevice* dxgiDevice,
    _Out_ IInspectable** graphicsDevice);

STDAPI CreateDirectX11SurfaceFromDXGISurface(
    _In_ IDXGISurface* dgxiSurface,
    _Out_ IInspectable** graphicsSurface);

STDAPI GetDXGIInterfaceFromDirectX11Device(
    _In_ IInspectable* graphicsDevice,
    _In_ REFIID iid,
    _Out_ void** p);

STDAPI GetDXGIInterfaceFromDirectX11Surface(
    _In_ IInspectable* graphicsSurface,
    _In_ REFIID iid,
    _Out_ void** p);




#if defined(__cplusplus_winrt)

#include <wrl.h>

namespace Microsoft
{
    namespace Graphics
    {
        namespace Canvas
        {
            inline DirectX11Device^ CreateDirectX11Device(
                _In_ IDXGIDevice* dxgiDevice)
            {
                using Microsoft::WRL::ComPtr;

                ComPtr<IInspectable> inspectableDirectX11Device;
                __abi_ThrowIfFailed(CreateDirectX11DeviceFromDXGIDevice(
                    dxgiDevice, 
                    &inspectableDirectX11Device));

                return reinterpret_cast<DirectX11Device^>(inspectableDirectX11Device.Detach());
            }

            inline DirectX11Surface^ CreateDirectX11Surface(
                _In_ IDXGISurface* dxgiSurface)
            {
                using Microsoft::WRL::ComPtr;

                ComPtr<IInspectable> inspectableDirectX11Surface;
                __abi_ThrowIfFailed(CreateDirectX11SurfaceFromDXGISurface(
                    dxgiSurface,
                    &inspectableDirectX11Surface));

                return reinterpret_cast<DirectX11Surface^>(inspectableDirectX11Surface.Detach());
            }

            template<typename DXGI_TYPE>
            inline HRESULT GetDXGIInterface(
                _In_  DirectX11Device^ graphicsDevice, 
                _Out_ DXGI_TYPE** dxgi)
            {
                return GetDXGIInterfaceFromDirectX11Device(
                    reinterpret_cast<IInspectable*>(graphicsDevice),
                    IID_PPV_ARGS(dxgi));
            }

            template<typename DXGI_TYPE>
            inline HRESULT GetDXGIInterface(
                _In_ DirectX11Surface^ graphicsSurface,
                _Out_ DXGI_TYPE** dxgi)
            {
                return GetDXGIInterfaceFromDirectX11Surface(
                    reinterpret_cast<IInspectable*>(graphicsSurface),
                    IID_PPV_ARGS(dxgi));                
            }
        }
    }
}

#endif // __cplusplus_winrt

