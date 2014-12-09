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

#pragma once

#include "StubD2DResources.h"

namespace canvas
{
    //
    // The stub CanvasDevice provides basic functionality to allow it to be used
    // in tests that don't care about interactions with CanvasDevice.
    //
    class StubCanvasDevice : public MockCanvasDevice
    {
        ComPtr<ID2D1Device1> m_d2DDevice;
        ComPtr<MockD3D11Device> m_d3dDevice;

    public:
        StubCanvasDevice(ComPtr<ID2D1Device1> device = Make<StubD2DDevice>())
            : m_d2DDevice(device)
        {
            GetDXGIInterfaceMethod.AllowAnyCall();
            CreateDeviceContextMethod.AllowAnyCall(
                [=]
                {
                    ComPtr<ID2D1DeviceContext1> dc;
                    ThrowIfFailed(m_d2DDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &dc));
                    return dc;
                });
        }

        void MarkAsLost()
        {
            auto d3dDevice = GetDXGIInterfaceFromResourceCreator<ID3D11Device>(this);
            auto mockD3DDevice = dynamic_cast<MockD3D11Device*>(d3dDevice.Get());
            mockD3DDevice->GetDeviceRemovedReasonMethod.AllowAnyCall(
                [] { return DXGI_ERROR_DEVICE_REMOVED; });
        }

        virtual ComPtr<ID2D1Device1> GetD2DDevice() override
        {
            return m_d2DDevice;
        }

        IFACEMETHODIMP get_Device(ICanvasDevice** value) override
        {
            ComPtr<ICanvasDevice> device(this);

            *value = device.Detach();

            return S_OK;
        }

        IFACEMETHODIMP GetDXGIInterface(REFIID iid, void** p) override
        {
            HRESULT hr = __super::GetDXGIInterface(iid, p);
            if (SUCCEEDED(hr) && !*p)
            {
                if (!m_d3dDevice)
                    m_d3dDevice = Make<MockD3D11Device>();
                return m_d3dDevice.CopyTo(iid, p);
            }
            else
            {
                return hr;
            }
        }
    };
}
