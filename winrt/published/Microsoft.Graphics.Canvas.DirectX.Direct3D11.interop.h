// Copyright (c) Microsoft Corporation.  All rights reserved

#if defined(_MSC_VER)
#pragma once
#endif

#include <winapifamily.h>

#pragma region Application Family
#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_APP)

#include <inspectable.h>
#include <dxgi.h>

STDAPI CreateDirect3D11DeviceFromDXGIDevice(
    _In_         IDXGIDevice* dxgiDevice,
    _COM_Outptr_ IInspectable** graphicsDevice);

STDAPI CreateDirect3D11SurfaceFromDXGISurface(
    _In_         IDXGISurface* dgxiSurface,
    _COM_Outptr_ IInspectable** graphicsSurface);

STDAPI GetDXGIInterfaceFromDirect3D11Device(
    _In_         IInspectable* graphicsDevice,
    _In_         REFIID iid,
    _COM_Outptr_ void** p);

STDAPI GetDXGIInterfaceFromDirect3D11Surface(
    _In_         IInspectable* graphicsSurface,
    _In_         REFIID iid,
    _COM_Outptr_ void** p);


#if defined(__cplusplus_winrt)

#include <wrl.h>

namespace Microsoft { namespace Graphics { namespace Canvas { namespace DirectX { namespace Direct3D11 {

    inline Direct3DDevice^ CreateDirect3DDevice(
        _In_ IDXGIDevice* dxgiDevice)
    {
        using Microsoft::WRL::ComPtr;
        using Platform::Object;

        ComPtr<IInspectable> inspectableDevice;
        __abi_ThrowIfFailed(CreateDirect3D11DeviceFromDXGIDevice(
            dxgiDevice,
            &inspectableDevice));

        Object^ objectDevice = reinterpret_cast<Object^>(inspectableDevice.Get());

        return safe_cast<Direct3DDevice^>(objectDevice);
    }

    inline Direct3DSurface^ CreateDirect3DSurface(
        _In_ IDXGISurface* dxgiSurface)
    {
        using Microsoft::WRL::ComPtr;
        using Platform::Object;

        ComPtr<IInspectable> inspectableSurface;
        __abi_ThrowIfFailed(CreateDirect3D11SurfaceFromDXGISurface(
            dxgiSurface,
            &inspectableSurface));

        Object^ objectSurface = reinterpret_cast<Object^>(inspectableSurface.Get());

        return safe_cast<Direct3DSurface^>(objectSurface);
    }

    template<typename DXGI_TYPE>
    HRESULT GetDXGIInterface(
        _In_         Direct3DDevice^ device,
        _COM_Outptr_ DXGI_TYPE** dxgi)
    {
        return GetDXGIInterfaceFromDirect3D11Device(
            reinterpret_cast<IInspectable*>(device),
            IID_PPV_ARGS(dxgi));
    }

    template<typename DXGI_TYPE>
    HRESULT GetDXGIInterface(
        _In_ Direct3DSurface^ surface,
        _Out_ DXGI_TYPE** dxgi)
    {
        return GetDXGIInterfaceFromDirect3D11Surface(
            reinterpret_cast<IInspectable*>(surface),
            IID_PPV_ARGS(dxgi));
    }

} /* Direct3D11 */ } /* DirectX */ } /* Canvas */ } /* Graphics */ } /* Windows */

#endif /* __cplusplus_winrt */

#endif /* WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_APP) */
#pragma endregion

