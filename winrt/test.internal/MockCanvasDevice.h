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
    using namespace ABI::Microsoft::Graphics::Canvas;

    class MockCanvasDevice : public RuntimeClass<
        RuntimeClassFlags<WinRtClassicComMix>,
        ICanvasDevice,
        CloakedIid<ICanvasDeviceInternal>,
        ICanvasResourceCreator>
    {
    public:        
        std::function<ComPtr<ID2D1Device1>()> MockGetD2DDevice;

        //
        // ICanvasDevice
        //

        IFACEMETHODIMP get_HardwareAcceleration(CanvasHardwareAcceleration *) override
        {
            Assert::Fail(L"Unexpected call to get_HardwareAcceleration");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP get_Direct3DDevice(IDirect3DDevice **) override
        {
            Assert::Fail(L"Unexpected call to get_Direct3DDevice");
            return E_NOTIMPL;
        }

        //
        // ICanvasResourceCreator
        //

        IFACEMETHODIMP get_Device(ICanvasDevice** value)
        {
            Assert::Fail(L"Unexpected call to get_Device");
            return E_NOTIMPL;
        }

        //
        // ICanvasDeviceInternal
        //

        virtual ComPtr<ID2D1Device1> GetD2DDevice() override
        {
            if (!MockGetD2DDevice)
            {
                Assert::Fail(L"Unexpected call to GetDevice");
                ThrowHR(E_NOTIMPL);
            }

            return MockGetD2DDevice();
        }

        virtual ComPtr<ID2D1SolidColorBrush> CreateSolidColorBrush(const D2D1_COLOR_F& color) override
        {
            Assert::Fail(L"Unexpected call to CreateSolidColorBrush");
            return nullptr;
        }
    };
}

