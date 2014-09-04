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

namespace canvas
{
    //
    // The stub CanvasDevice provides basic functionality to allow it to be used
    // in tests that don't care about interactions with CanvasDevice.
    //
    class StubCanvasDevice : public MockCanvasDevice
    {
        ComPtr<ID2D1Device1> m_d2DDevice;

    public:
        StubCanvasDevice(ComPtr<ID2D1Device1> device = Make<MockD2DDevice>())
            : m_d2DDevice(device)
        {
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
    };
}
