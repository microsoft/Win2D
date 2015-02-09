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

#include <RecreatableDeviceManager.impl.h>

#include "MockCanvasDeviceActivationFactory.h"
#include "MockHelpers.h"
#include "MockWindow.h"
#include "MockDispatcher.h"

template<typename TRAITS>
class BaseControlTestAdapter : public TRAITS::adapter_t
{
    ComPtr<MockWindow> m_mockWindow;
    bool m_hasUIThreadAccess;

public:
    ComPtr<MockEventSource<DpiChangedEventHandler>> DpiChangedEventSource;
    ComPtr<MockEventSource<IEventHandler<SuspendingEventArgs*>>> SuspendingEventSource;
    ComPtr<MockEventSource<IEventHandler<IInspectable*>>> ResumingEventSource;
    CALL_COUNTER_WITH_MOCK(CreateRecreatableDeviceManagerMethod, std::unique_ptr<IRecreatableDeviceManager<TRAITS>>());

    ComPtr<MockCanvasDeviceActivationFactory> DeviceFactory;

    float LogicalDpi;

    BaseControlTestAdapter()
        : DeviceFactory(Make<MockCanvasDeviceActivationFactory>())
        , m_mockWindow(Make<MockWindow>())
        , DpiChangedEventSource(Make<MockEventSource<DpiChangedEventHandler>>(L"DpiChanged"))
        , SuspendingEventSource(Make<MockEventSource<IEventHandler<SuspendingEventArgs*>>>(L"Suspending"))
        , ResumingEventSource(Make<MockEventSource<IEventHandler<IInspectable*>>>(L"Resuming"))
        , LogicalDpi(DEFAULT_DPI)
        , m_hasUIThreadAccess(true)
    {
        CreateRecreatableDeviceManagerMethod.AllowAnyCall();
        DeviceFactory->ActivateInstanceMethod.AllowAnyCall();

        GetCurrentMockWindow()->get_DispatcherMethod.AllowAnyCall(
            [=](ICoreDispatcher** out)
            {
                auto dispatcher = Make<MockDispatcher>();

                dispatcher->RunAsyncMethod.AllowAnyCall(
                    [=](CoreDispatcherPriority priority, IDispatchedHandler *agileCallback, IAsyncAction **asyncAction)
                    {
                        // This just launches the action right away, and doesn't copy out an async action object.
                        // Currently, product code doesn't use it anyway.

                        *asyncAction = nullptr;

                        Assert::AreEqual(S_OK, agileCallback->Invoke());

                        return S_OK;
                    });

                dispatcher->get_HasThreadAccessMethod.AllowAnyCall(
                    [=](boolean* out)
                    {
                        *out = m_hasUIThreadAccess;

                        return S_OK;
                    });

                return dispatcher.CopyTo(out);
            });
    }

    virtual std::pair<ComPtr<IInspectable>, ComPtr<IUserControl>> CreateUserControl(IInspectable* canvasControl) override
    {
        auto control = Make<StubUserControl>();

        ComPtr<IInspectable> inspectableControl;
        ThrowIfFailed(control.As(&inspectableControl));

        return std::pair<ComPtr<IInspectable>, ComPtr<IUserControl>>(inspectableControl, control);
    }

    virtual RegisteredEvent AddApplicationSuspendingCallback(IEventHandler<SuspendingEventArgs*>* value) override
    {
        return SuspendingEventSource->Add(value);
    }

    virtual RegisteredEvent AddApplicationResumingCallback(IEventHandler<IInspectable*>* value) override
    {
        return ResumingEventSource->Add(value);
    }

    virtual float GetLogicalDpi() override
    {
        return LogicalDpi;
    }

    virtual RegisteredEvent AddDpiChangedCallback(DpiChangedEventHandler* value) override
    {
        return DpiChangedEventSource->Add(value);
    }

    void RaiseDpiChangedEvent()
    {
        ThrowIfFailed(DpiChangedEventSource->InvokeAll(nullptr, nullptr));
    }

    virtual ComPtr<IWindow> GetWindowOfCurrentThread() override
    {
        return m_mockWindow;
    }

    ComPtr<MockWindow> GetCurrentMockWindow()
    {
        return m_mockWindow;
    }

    virtual std::unique_ptr<IRecreatableDeviceManager<TRAITS>> CreateRecreatableDeviceManager() override
    {
        auto manager = CreateRecreatableDeviceManagerMethod.WasCalled();
        if (manager)
            return manager;

        return std::make_unique<RecreatableDeviceManager<TRAITS>>(DeviceFactory.Get());
    }

    void SetHasUIThreadAccess(bool value)
    {
        m_hasUIThreadAccess = value;
    }
};

#define VERIFY_THREADING_RESTRICTION(EXPECTED_HR, FUNC) \
    f.Adapter->SetHasUIThreadAccess(false);             \
    Assert::AreEqual(EXPECTED_HR, FUNC);                \
    f.Adapter->SetHasUIThreadAccess(true);              \
    Assert::AreEqual(S_OK, FUNC);                      
