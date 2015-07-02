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

class MockCanvasDeviceActivationFactory : public RuntimeClass<IActivationFactory, ICanvasDeviceStatics, ICanvasDeviceFactory>
{
public:
    CALL_COUNTER_WITH_MOCK(ActivateInstanceMethod, HRESULT(IInspectable**));
    CALL_COUNTER_WITH_MOCK(GetSharedDeviceMethod, HRESULT(boolean, ICanvasDevice**));
    CALL_COUNTER_WITH_MOCK(CreateWithForceSoftwareRendererOptionMethod, HRESULT(boolean, ICanvasDevice**));
    CALL_COUNTER_WITH_MOCK(put_DebugLevelMethod, HRESULT(CanvasDebugLevel));
    CALL_COUNTER_WITH_MOCK(get_DebugLevelMethod, HRESULT(CanvasDebugLevel*));

    void ExpectToActivateOne(ComPtr<ICanvasDevice> device = Make<StubCanvasDevice>())
    {
        CreateWithForceSoftwareRendererOptionMethod.SetExpectedCalls(1,
            [=](boolean, ICanvasDevice** out)
            {
                return device.CopyTo(out);
            });
    }

    IFACEMETHODIMP ActivateInstance(IInspectable** value)
    {
        return ExceptionBoundary(
            [=]
            {
                CheckAndClearOutPointer(value);
                ThrowIfFailed(ActivateInstanceMethod.WasCalled(value));

                if (!*value)
                {
                    auto device = Make<StubCanvasDevice>();
                    ThrowIfFailed(device.CopyTo(value));
                }
            });
    }

    IFACEMETHODIMP CreateWithForceSoftwareRendererOption(
        boolean forceSoftwareRenderer,
        ICanvasDevice** canvasDevice)
    {
        return CreateWithForceSoftwareRendererOptionMethod.WasCalled(forceSoftwareRenderer, canvasDevice);
    }

    IFACEMETHODIMP CreateFromDirect3D11Device(
        IDirect3DDevice* direct3DDevice,
        ICanvasDevice** canvasDevice)
    {
        return E_NOTIMPL;
    }

    IFACEMETHODIMP GetSharedDevice(
        boolean forceSoftwareRenderer,
        ICanvasDevice** device)
    {
        return GetSharedDeviceMethod.WasCalled(forceSoftwareRenderer, device);
    }

    IFACEMETHODIMP put_DebugLevel(CanvasDebugLevel debugLevel)
    {
        return put_DebugLevelMethod.WasCalled(debugLevel);
    }

    IFACEMETHODIMP get_DebugLevel(CanvasDebugLevel* debugLevel)
    {
        return get_DebugLevelMethod.WasCalled(debugLevel);
    }
};
