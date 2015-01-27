// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may
// not use these files except in compliance with the License. You may obtain
// a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
// License for the specific language governing permissions and limitations
// under the License.

#if (WINVER > 0x0603)
#error Include Windows.Graphics.DirectX.Direct3D11.interop.h when building for Windows 10
#endif

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

namespace Microsoft { namespace Graphics { namespace Canvas { namespace DirectX { namespace Direct3D11 {

    [uuid(6173F6BA-35C0-46F9-A944-BD7661DA6E6E)]
    class IDirect3DDxgiInterfaceAccess : public IUnknown
    {
    public:
        IFACEMETHOD(GetInterface)(REFIID iid, void** p) = 0;
    };
                        
}}}}}

#if defined(__cplusplus_winrt)

#include <wrl.h>

namespace Microsoft { namespace Graphics { namespace Canvas { namespace DirectX { namespace Direct3D11 {

    inline IDirect3DDevice^ CreateDirect3DDevice(
        _In_ IDXGIDevice* dxgiDevice)
    {
        using Microsoft::WRL::ComPtr;
        using Platform::Object;

        ComPtr<IInspectable> inspectableDevice;
        __abi_ThrowIfFailed(CreateDirect3D11DeviceFromDXGIDevice(
            dxgiDevice,
            &inspectableDevice));

        Object^ objectDevice = reinterpret_cast<Object^>(inspectableDevice.Get());

        return safe_cast<IDirect3DDevice^>(objectDevice);
    }

    inline IDirect3DSurface^ CreateDirect3DSurface(
        _In_ IDXGISurface* dxgiSurface)
    {
        using Microsoft::WRL::ComPtr;
        using Platform::Object;

        ComPtr<IInspectable> inspectableSurface;
        __abi_ThrowIfFailed(CreateDirect3D11SurfaceFromDXGISurface(
            dxgiSurface,
            &inspectableSurface));

        Object^ objectSurface = reinterpret_cast<Object^>(inspectableSurface.Get());

        return safe_cast<IDirect3DSurface^>(objectSurface);
    }

    inline HRESULT GetDXGIInterfaceFromObject(
        _In_         Platform::Object^ object,
        _In_         REFIID iid,
        _COM_Outptr_ void** p)
    {
        using Microsoft::WRL::ComPtr;
        using ::Microsoft::Graphics::Canvas::DirectX::Direct3D11::IDirect3DDxgiInterfaceAccess;

        IInspectable* deviceInspectable = reinterpret_cast<IInspectable*>(object);
        ComPtr<IDirect3DDxgiInterfaceAccess> dxgiInterfaceAccess;
        
        HRESULT hr = deviceInspectable->QueryInterface(IID_PPV_ARGS(&dxgiInterfaceAccess));

        if (SUCCEEDED(hr))
        {
            hr = dxgiInterfaceAccess->GetInterface(iid, p);
        }

        return hr;
    }

    template<typename DXGI_TYPE>
    HRESULT GetDXGIInterface(
        _In_         IDirect3DDevice^ device,
        _COM_Outptr_ DXGI_TYPE** dxgi)
    {
        return GetDXGIInterfaceFromObject(device, IID_PPV_ARGS(dxgi));
    }

    template<typename DXGI_TYPE>
    HRESULT GetDXGIInterface(
        _In_ IDirect3DSurface^ surface,
        _Out_ DXGI_TYPE** dxgi)
    {
        return GetDXGIInterfaceFromObject(surface, IID_PPV_ARGS(dxgi));
    }

} /* Direct3D11 */ } /* DirectX */ } /* Canvas */ } /* Graphics */ } /* Windows */

#endif /* __cplusplus_winrt */

#endif /* WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_APP) */
#pragma endregion

