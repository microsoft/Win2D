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

class MockDispatcher : public RuntimeClass<ICoreDispatcher, ICoreDispatcherWithTaskPriority>
{
public:
    CALL_COUNTER_WITH_MOCK(ProcessEventsMethod, HRESULT(CoreProcessEventsOption));
    CALL_COUNTER_WITH_MOCK(RunAsyncMethod, HRESULT(CoreDispatcherPriority, IDispatchedHandler*, IAsyncAction**));
    CALL_COUNTER_WITH_MOCK(get_HasThreadAccessMethod, HRESULT(boolean*));
    CALL_COUNTER_WITH_MOCK(StopProcessEventsMethod, HRESULT());

    virtual IFACEMETHODIMP get_HasThreadAccess(boolean *value) override
    {
        return get_HasThreadAccessMethod.WasCalled(value);
    }

    virtual IFACEMETHODIMP ProcessEvents(
        CoreProcessEventsOption options) override
    {
        return ProcessEventsMethod.WasCalled(options);
    }

    virtual IFACEMETHODIMP RunAsync(
        CoreDispatcherPriority priority,
        IDispatchedHandler *agileCallback,
        IAsyncAction **asyncAction) override
    {
        return RunAsyncMethod.WasCalled(priority, agileCallback, asyncAction);
    }

    virtual IFACEMETHODIMP RunIdleAsync(
        IIdleDispatchedHandler *agileCallback,
        IAsyncAction **asyncAction) override
    {
        Assert::Fail(L"Unexpected call to ICoreDispatcher::RunIdleAsync");
        return E_UNEXPECTED;
    }

    virtual IFACEMETHODIMP get_CurrentPriority(CoreDispatcherPriority* value) override
    {
        Assert::Fail(L"Unexpected call to ICoreDispatcherWithTaskPriority::get_CurrentPriority");
        return E_UNEXPECTED;
    }

    virtual IFACEMETHODIMP put_CurrentPriority(CoreDispatcherPriority value) override
    {
        Assert::Fail(L"Unexpected call to ICoreDispatcherWithTaskPriority::put_CurrentPriority");
        return E_UNEXPECTED;
    }

    virtual IFACEMETHODIMP ShouldYield(boolean* value) override
    {
        Assert::Fail(L"Unexpected call to ICoreDispatcherWithTaskPriority::ShouldYield");
        return E_UNEXPECTED;
    }

    virtual IFACEMETHODIMP ShouldYieldToPriority(CoreDispatcherPriority priority, boolean* value) override
    {
        Assert::Fail(L"Unexpected call to ICoreDispatcherWithTaskPriority::ShouldYieldToPriority");
        return E_UNEXPECTED;
    }

    virtual IFACEMETHODIMP StopProcessEvents() override
    {
        return StopProcessEventsMethod.WasCalled();
    }


};