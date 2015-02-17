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

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace DirectX { namespace Direct3D11
{
    class Direct3DDevice : public RuntimeClass<
        RuntimeClassFlags<WinRtClassicComMix>,
        ABI::Microsoft::Graphics::Canvas::DirectX::Direct3D11::IDirect3DDevice,
        ABI::Windows::Foundation::IClosable,
        CloakedIid<IDirect3DDxgiInterfaceAccess>>,
        private LifespanTracker<Direct3DDevice>
    {
        InspectableClass(L"Microsoft.Graphics.Canvas.DirectX.Direct3D11.IDirect3DDevice", BaseTrust);

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
                [&]
                {
                    auto& device = m_DxgiDevice.EnsureNotClosed();
                    device->Trim();
                });
        }

        // IDirect3DDxgiInterfaceAccess
        IFACEMETHODIMP GetInterface(REFIID iid, void** p) override
        {
            return ExceptionBoundary(
                [&]
                {
                    auto& device = m_DxgiDevice.EnsureNotClosed();
                    ThrowIfFailed(device.CopyTo(iid, p));
                });
        }
    };
}}}}}}


using namespace ABI::Microsoft::Graphics::Canvas::DirectX::Direct3D11;


_Use_decl_annotations_
STDAPI CreateDirect3D11DeviceFromDXGIDevice(
    IDXGIDevice* dxgiDevice,
    IInspectable** inspectableDirect3DDevice)
{
    return ExceptionBoundary(
        [&]
        {
            ComPtr<Direct3DDevice> direct3DDevice = Make<Direct3DDevice>(dxgiDevice);
            if (!direct3DDevice)
                ThrowIfFailed(E_OUTOFMEMORY);

            ThrowIfFailed(direct3DDevice.CopyTo(inspectableDirect3DDevice));
        });
}
