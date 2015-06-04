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

using namespace ABI::Windows::UI::Core;

class MockCoreIndependentInputSource : public RuntimeClass<ICorePointerInputSource, ICoreInputSourceBase>
{
public:
    CALL_COUNTER_WITH_MOCK(get_DispatcherMethod, HRESULT(ICoreDispatcher**));
    CALL_COUNTER_WITH_MOCK(ReleasePointerCaptureMethod, HRESULT(void));
    CALL_COUNTER_WITH_MOCK(SetPointerCaptureMethod, HRESULT(void));
    CALL_COUNTER_WITH_MOCK(get_HasCaptureMethod, HRESULT(boolean*));
    CALL_COUNTER_WITH_MOCK(get_PointerPositionMethod, HRESULT(Point*));
    CALL_COUNTER_WITH_MOCK(get_PointerCursorMethod, HRESULT(ICoreCursor**));
    CALL_COUNTER_WITH_MOCK(put_PointerCursorMethod, HRESULT(ICoreCursor*));

    CALL_COUNTER_WITH_MOCK(add_PointerCaptureLostMethod, HRESULT(EventHandlerWithPointerArgs*, EventRegistrationToken*));
    CALL_COUNTER_WITH_MOCK(remove_PointerCaptureLostMethod, HRESULT(EventRegistrationToken));

    CALL_COUNTER_WITH_MOCK(add_PointerEnteredMethod, HRESULT(EventHandlerWithPointerArgs*, EventRegistrationToken*));
    CALL_COUNTER_WITH_MOCK(remove_PointerEnteredMethod, HRESULT(EventRegistrationToken));

    CALL_COUNTER_WITH_MOCK(add_PointerExitedMethod, HRESULT(EventHandlerWithPointerArgs*, EventRegistrationToken*));
    CALL_COUNTER_WITH_MOCK(remove_PointerExitedMethod, HRESULT(EventRegistrationToken));

    CALL_COUNTER_WITH_MOCK(add_PointerMovedMethod, HRESULT(EventHandlerWithPointerArgs*, EventRegistrationToken*));
    CALL_COUNTER_WITH_MOCK(remove_PointerMovedMethod, HRESULT(EventRegistrationToken));

    CALL_COUNTER_WITH_MOCK(add_PointerPressedMethod, HRESULT(EventHandlerWithPointerArgs*, EventRegistrationToken*));
    CALL_COUNTER_WITH_MOCK(remove_PointerPressedMethod, HRESULT(EventRegistrationToken));

    CALL_COUNTER_WITH_MOCK(add_PointerReleasedMethod, HRESULT(EventHandlerWithPointerArgs*, EventRegistrationToken*));
    CALL_COUNTER_WITH_MOCK(remove_PointerReleasedMethod, HRESULT(EventRegistrationToken));

    CALL_COUNTER_WITH_MOCK(add_PointerWheelChangedMethod, HRESULT(EventHandlerWithPointerArgs*, EventRegistrationToken*));
    CALL_COUNTER_WITH_MOCK(remove_PointerWheelChangedMethod, HRESULT(EventRegistrationToken));

    // 
    // ICoreInputSourceBase methods
    //

    virtual IFACEMETHODIMP get_Dispatcher(ICoreDispatcher** value) override
    {
        return get_DispatcherMethod.WasCalled(value);
    }

    virtual IFACEMETHODIMP get_IsInputEnabled(boolean* value) override
    {
        Assert::Fail(L"Unexpected call to ICoreInputSourceBase::get_IsInputEnabled");
        return E_UNEXPECTED;
    }

    virtual IFACEMETHODIMP put_IsInputEnabled(boolean value) override
    {
        Assert::Fail(L"Unexpected call to ICoreInputSourceBase::put_IsInputEnabled");
        return E_UNEXPECTED;
    }

    virtual IFACEMETHODIMP add_InputEnabled(__FITypedEventHandler_2_IInspectable_Windows__CUI__CCore__CInputEnabledEventArgs*, EventRegistrationToken*) override
    {
        Assert::Fail(L"Unexpected call to ICoreInputSourceBase::add_InputEnabled");
        return E_UNEXPECTED;
    }

    virtual IFACEMETHODIMP remove_InputEnabled(EventRegistrationToken) override
    {
        Assert::Fail(L"Unexpected call to ICoreInputSourceBase::remove_InputEnabled");
        return E_UNEXPECTED;
    }

    //
    // ICorePointerInputSource
    //

    virtual IFACEMETHODIMP ReleasePointerCapture(void) override
    {
        return ReleasePointerCaptureMethod.WasCalled();
    }

    virtual IFACEMETHODIMP SetPointerCapture(void) override
    {
        return SetPointerCaptureMethod.WasCalled();
    }

    virtual IFACEMETHODIMP get_HasCapture(boolean* value) override
    {
        return get_HasCaptureMethod.WasCalled(value);
    }

    virtual IFACEMETHODIMP get_PointerPosition(Point* value) override
    {
        return get_PointerPositionMethod.WasCalled(value);
    }

    virtual IFACEMETHODIMP get_PointerCursor(ICoreCursor** value) override
    {
        return get_PointerCursorMethod.WasCalled(value);
    }

    virtual IFACEMETHODIMP put_PointerCursor(ICoreCursor* value) override
    {
        return put_PointerCursorMethod.WasCalled(value);
    }

    virtual IFACEMETHODIMP add_PointerCaptureLost(EventHandlerWithPointerArgs* args, EventRegistrationToken* token) override
    {
        return add_PointerCaptureLostMethod.WasCalled(args, token);
    }

    virtual IFACEMETHODIMP remove_PointerCaptureLost(EventRegistrationToken token) override
    {
        return remove_PointerCaptureLostMethod.WasCalled(token);
    }

    virtual IFACEMETHODIMP add_PointerEntered(EventHandlerWithPointerArgs* args, EventRegistrationToken* token) override
    {
        return add_PointerEnteredMethod.WasCalled(args, token);
    }

    virtual IFACEMETHODIMP remove_PointerEntered(EventRegistrationToken token) override
    {
        return remove_PointerEnteredMethod.WasCalled(token);
    }

    virtual IFACEMETHODIMP add_PointerExited(EventHandlerWithPointerArgs* args, EventRegistrationToken* token) override
    {
        return add_PointerExitedMethod.WasCalled(args, token);
    }

    virtual IFACEMETHODIMP remove_PointerExited(EventRegistrationToken token) override
    {
        return remove_PointerExitedMethod.WasCalled(token);
    }

    virtual IFACEMETHODIMP add_PointerMoved(EventHandlerWithPointerArgs* args, EventRegistrationToken* token) override
    {
        return add_PointerMovedMethod.WasCalled(args, token);
    }

    virtual IFACEMETHODIMP remove_PointerMoved(EventRegistrationToken token) override
    {
        return remove_PointerMovedMethod.WasCalled(token);
    }

    virtual IFACEMETHODIMP add_PointerPressed(EventHandlerWithPointerArgs* args, EventRegistrationToken* token) override
    {
        return add_PointerPressedMethod.WasCalled(args, token);
    }

    virtual IFACEMETHODIMP remove_PointerPressed(EventRegistrationToken token) override
    {
        return remove_PointerPressedMethod.WasCalled(token);
    }

    virtual IFACEMETHODIMP add_PointerReleased(EventHandlerWithPointerArgs* args, EventRegistrationToken* token) override
    {
        return add_PointerReleasedMethod.WasCalled(args, token);
    }

    virtual IFACEMETHODIMP remove_PointerReleased(EventRegistrationToken token) override
    {
        return remove_PointerReleasedMethod.WasCalled(token);
    }

    virtual IFACEMETHODIMP add_PointerWheelChanged(EventHandlerWithPointerArgs* args, EventRegistrationToken* token) override
    {
        return add_PointerWheelChangedMethod.WasCalled(args, token);
    }

    virtual IFACEMETHODIMP remove_PointerWheelChanged(EventRegistrationToken token) override
    {
        return remove_PointerWheelChangedMethod.WasCalled(token);
    }

};

class StubCoreIndependentInputSource : public MockCoreIndependentInputSource
{
    ComPtr<ICoreDispatcher> m_dispatcher;

public:
    MockEventSource<EventHandlerWithPointerArgs> PointerCaptureLost;
    MockEventSource<EventHandlerWithPointerArgs> PointerEntered;
    MockEventSource<EventHandlerWithPointerArgs> PointerExited;
    MockEventSource<EventHandlerWithPointerArgs> PointerMoved;
    MockEventSource<EventHandlerWithPointerArgs> PointerPressed;
    MockEventSource<EventHandlerWithPointerArgs> PointerReleased;
    MockEventSource<EventHandlerWithPointerArgs> PointerWheelChanged;

    StubCoreIndependentInputSource(ComPtr<ICoreDispatcher> dispatcher)
        : m_dispatcher(dispatcher)
        , PointerCaptureLost(L"PointerCaptureLost")
        , PointerEntered(L"PointerEntered")
        , PointerExited(L"PointerExited")
        , PointerMoved(L"PointerMoved")
        , PointerPressed(L"PointerPressed")
        , PointerReleased(L"PointerReleased")
        , PointerWheelChanged(L"PointerWheelChanged")
    {
        put_PointerCursorMethod.AllowAnyCall();

        get_DispatcherMethod.AllowAnyCall(
            [=](ICoreDispatcher** out)
            {
                return m_dispatcher.CopyTo(out);
            });

#define ALLOW(EVENT) \
        add_##EVENT##Method.AllowAnyCall([=] (EventHandlerWithPointerArgs* delegate, EventRegistrationToken* token) { return EVENT.add_Event(delegate, token); }); \
        remove_##EVENT##Method.AllowAnyCall([=] (EventRegistrationToken token) { return EVENT.remove_Event(token); })

        ALLOW(PointerCaptureLost);
        ALLOW(PointerEntered);
        ALLOW(PointerExited);
        ALLOW(PointerMoved);
        ALLOW(PointerPressed);
        ALLOW(PointerReleased);
        ALLOW(PointerWheelChanged);

#undef ALLOW
    }

    //
    // Raises the event (specified as pointer to member - eg
    // &StubCoreIndependentInputSource::PointerMoved) via the dispatcher.  This
    // simulates how these events are raised on the real
    // CoreIndependentInputSource.
    //
    template<typename EVENT>
    ComPtr<IAsyncAction> InvokeViaDispatcher(EVENT eventMember)
    {
        auto handler = Callback<IDispatchedHandler>(
            [=]
            {
                return ExceptionBoundary(
                    [&]
                    {
                        auto& event = (this->*eventMember);
                        ThrowIfFailed(event.InvokeAll(nullptr, nullptr));
                    });
            });
        CheckMakeResult(handler);

        ComPtr<IAsyncAction> action;
        ThrowIfFailed(m_dispatcher->RunAsync(CoreDispatcherPriority_Normal, handler.Get(), &action));

        return action;
    }
};
