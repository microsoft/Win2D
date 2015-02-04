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

#include "MockHelpers.h"

class MockCanvasDeviceActivationFactory : public RuntimeClass<IActivationFactory>
{
public:
    CALL_COUNTER_WITH_MOCK(ActivateInstanceMethod, HRESULT(IInspectable**));

    void ExpectToActivateOne(ComPtr<ICanvasDevice> device = Make<StubCanvasDevice>())
    {
        ActivateInstanceMethod.SetExpectedCalls(1,
            [=](IInspectable** value)
            {
                return device.CopyTo(value);
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
};
