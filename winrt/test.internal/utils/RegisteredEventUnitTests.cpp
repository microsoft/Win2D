// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

class TestEventSource : public RuntimeClass<RuntimeClassFlags<ClassicCom>, IUnknown>
{
    EventSource<IEventHandler<IInspectable*>> m_eventSource;

public:
    IFACEMETHODIMP add_Event(IEventHandler<IInspectable*>* theValue, EventRegistrationToken* token)
    {
        return m_eventSource.Add(theValue, token);
    }

    IFACEMETHODIMP remove_Event(EventRegistrationToken token)
    {
        return m_eventSource.Remove(token);
    }

    void Raise()
    {
        ThrowIfFailed(m_eventSource.InvokeAll(nullptr, nullptr));
    }
};


TEST_CLASS(RegisteredEventTests)
{
    TEST_METHOD_EX(RegisteredEvent_WhenDefaultConstructed_CanBeDestructed)
    {
        RegisteredEvent r;
    }

    TEST_METHOD_EX(RegisteredEvent_WhenDefaultConstructed_CanBeDetached)
    {
        RegisteredEvent r;
        r.Detach();
    }

    TEST_METHOD_EX(RegisteredEvent_WhenDefaultConstructed_CanBeReleased)
    {
        RegisteredEvent r;
        r.Release();
    }

    class Counter
    {
        CALL_COUNTER(m_counter);

    public:
        void SetExpectedCalls(int n) { m_counter.SetExpectedCalls(n); }
        void WasCalled() { m_counter.WasCalled(); }
    };

    TEST_METHOD_EX(RegisteredEvent_WhenDestructed_CallsFunction)
    {
        Counter fn;
        fn.SetExpectedCalls(1);

        {
            RegisteredEvent r([&]() { fn.WasCalled(); });
        }
    }

    TEST_METHOD_EX(RegisteredEvent_WhenReleased_CallsFunction)
    {
        Counter fn;
        fn.SetExpectedCalls(1);

        {
            RegisteredEvent r([&]() { fn.WasCalled(); });
            r.Release();
            fn.SetExpectedCalls(0);
        }
    }

    TEST_METHOD_EX(RegisteredEvent_WhenDetached_DoesNotCallFunction)
    {
        Counter fn;
        fn.SetExpectedCalls(0);

        {
            RegisteredEvent r([&]() { fn.WasCalled(); });
            r.Detach();
            r.Release();
        }
    }

    TEST_METHOD_EX(RegisteredEvent_AddsAndRemovesHandler)
    {
        Counter fn;
        fn.SetExpectedCalls(1);

        auto s = Make<TestEventSource>();

        auto callback = Callback<IEventHandler<IInspectable*>>(
            [&](IInspectable*, IInspectable*) { fn.WasCalled(); return S_OK; });

        auto r = RegisteredEvent(
            s.Get(),
            &TestEventSource::add_Event,
            &TestEventSource::remove_Event,
            callback.Get());

        Assert::IsTrue(static_cast<bool>(r));

        s->Raise();

        fn.SetExpectedCalls(0);

        r.Release();        
        s->Raise();
    }
};

