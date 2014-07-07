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

#include "pch.h"

using namespace Microsoft::WRL;

namespace canvas
{
    [uuid(0A55F0AC-0BDD-4CFA-A9E7-8B2743AD33B7)]
    class IDirect3DDeviceInternal : public IUnknown
    {
    public:
        virtual void GetDXGIInterface(REFIID iid, void** p) = 0;
    };

    class Direct3DDevice : public RuntimeClass<
        RuntimeClassFlags<WinRtClassicComMix>,
        ABI::Microsoft::Graphics::Canvas::DirectX::Direct3D11::IDirect3DDevice,
        ABI::Windows::Foundation::IClosable,
        IDirect3DDeviceInternal>
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_DirectX_Direct3D11_Direct3DDevice, BaseTrust);

        ClosablePtr<IDXGIDevice3> m_DxgiDevice;

    public:
        Direct3DDevice(IDXGIDevice* dxgiDevice)
        {
            if (dxgiDevice == nullptr)
                ThrowHR(E_INVALIDARG);

            ComPtr<IDXGIDevice3> dxgiDevice3;
            ThrowIfFailed(dxgiDevice->QueryInterface(dxgiDevice3.GetAddressOf()));

            m_DxgiDevice = dxgiDevice3;
        }

        // IClosable
        IFACEMETHOD(Close)() override
        {
            m_DxgiDevice.Close();
            return S_OK;
        }

        // IDirect3DDevice
        IFACEMETHOD(Trim)() override
        {
            return ExceptionBoundary(
                [&]()
                {
                    auto& device = m_DxgiDevice.EnsureNotClosed();
                    device->Trim();
                });
        }

        // IDirect3DDeviceInternal
        virtual void GetDXGIInterface(REFIID iid, void** p) override
        {
            auto& device = m_DxgiDevice.EnsureNotClosed();
            ThrowIfFailed(device.CopyTo(iid, p));
        }
    };
}


_Use_decl_annotations_
STDAPI CreateDirect3D11DeviceFromDXGIDevice(
    IDXGIDevice* dxgiDevice,
    IInspectable** inspectableDirect3DDevice)
{
    return ExceptionBoundary(
        [&]()
        {
            ComPtr<canvas::Direct3DDevice> direct3DDevice = Make<canvas::Direct3DDevice>(dxgiDevice);
            if (!direct3DDevice)
                ThrowIfFailed(E_OUTOFMEMORY);

            ThrowIfFailed(direct3DDevice.CopyTo(inspectableDirect3DDevice));
        });
}


_Use_decl_annotations_
STDAPI GetDXGIInterfaceFromDirect3D11Device(
    IInspectable* direct3DDevice,
    REFIID iid,
    void** p)
{
    return ExceptionBoundary(
        [&]()
        {
            ComPtr<canvas::IDirect3DDeviceInternal> deviceInternal;
            ThrowIfFailed(direct3DDevice->QueryInterface(deviceInternal.GetAddressOf()));
            deviceInternal->GetDXGIInterface(iid, p);
        });
}
