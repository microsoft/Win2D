// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#ifdef WINUI3

class MockDispatcherQueue : public RuntimeClass<ABI::Microsoft::System::IDispatcherQueue, ABI::Microsoft::System::IDispatcherQueue2>
{
public:
    CALL_COUNTER_WITH_MOCK(TryEnqueueWithPriorityMethod, HRESULT(ABI::Microsoft::System::DispatcherQueuePriority, ABI::Microsoft::System::IDispatcherQueueHandler*, boolean*));
    CALL_COUNTER_WITH_MOCK(get_HasThreadAccessMethod, HRESULT(boolean*));

    virtual IFACEMETHODIMP get_HasThreadAccess(boolean* value) override
    {
        return get_HasThreadAccessMethod.WasCalled(value);
    }

    virtual IFACEMETHODIMP TryEnqueue(
        ABI::Microsoft::System::IDispatcherQueueHandler* agileCallback,
        boolean* result) override
    {
        Assert::Fail(L"Unexpected call to IDispatcherQueue::TryEnqueue");
        return E_UNEXPECTED;
    }

    virtual IFACEMETHODIMP TryEnqueueWithPriority(
        ABI::Microsoft::System::DispatcherQueuePriority priority,
        ABI::Microsoft::System::IDispatcherQueueHandler* agileCallback,
        boolean* result) override
    {
        return TryEnqueueWithPriorityMethod.WasCalled(priority, agileCallback, result);
    }

    virtual IFACEMETHODIMP CreateTimer(ABI::Microsoft::System::IDispatcherQueueTimer** result) override
    {
        Assert::Fail(L"Unexpected call to IDispatcherQueue::CreateTimer");
        return E_UNEXPECTED;
    }

    virtual IFACEMETHODIMP add_ShutdownStarting(
        ITypedEventHandler<ABI::Microsoft::System::DispatcherQueue*, ABI::Microsoft::System::DispatcherQueueShutdownStartingEventArgs*>* handler,
        EventRegistrationToken* token) override
    {
        Assert::Fail(L"Unexpected call to IDispatcherQueue::add_ShutdownStarting");
        return E_UNEXPECTED;
    }

    virtual IFACEMETHODIMP remove_ShutdownStarting(EventRegistrationToken token) override
    {
        Assert::Fail(L"Unexpected call to IDispatcherQueue::remove_ShutdownStarting");
        return E_UNEXPECTED;
    }

    virtual IFACEMETHODIMP add_ShutdownCompleted(
        ITypedEventHandler<ABI::Microsoft::System::DispatcherQueue*, IInspectable*>* handler,
        EventRegistrationToken* token) override
    {
        Assert::Fail(L"Unexpected call to IDispatcherQueue::add_ShutdownCompleted");
        return E_UNEXPECTED;
    }

    virtual IFACEMETHODIMP remove_ShutdownCompleted(EventRegistrationToken token) override
    {
        Assert::Fail(L"Unexpected call to IDispatcherQueue::remove_ShutdownCompleted");
        return E_UNEXPECTED;
    }
};

#else

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

#endif
