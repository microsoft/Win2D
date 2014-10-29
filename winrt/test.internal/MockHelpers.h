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

//
// Helpers for common mocking operations (eg counting how many times a method
// was called, or an event was invoked).
//
// If any of these expectations are used then
// Expectations::Instance()->Validate() must be called at the end of the test.
// Although it is tempting to do this in TEST_METHOD_CLEANUP, this can cause
// surprising errors to be reported.  Instead, an explicit call has to be added
// to the end of a test method.
//
//    CALL_COUNTER(TestCounter);
//
//    TEST_METHOD(MyTest)
//    {
//        TestCounter.SetExpectedCalls(1);
//        Expectations::Instance()->Validate();
//    }
//
//

//
// CALL_COUNTER defines a member variable that can be used to count how many
// times a method is called. eg:
//
// class SomeClass
// {
// public:
//     CALL_COUNTER(FooMethod);
//
//     void Foo() { FooMethod.WasCalled(); }
// }
//
// TEST_METHOD(MyTest)
// {
//     SomeClass c;
//     c.FooMethod.SetExpectedCalls(1);
//     Expectations::Instance()->Validate();
// }
//
// MyTest will fail because "FooMethod" was not called.
//
#define CALL_COUNTER(NAME)                                  \
    struct CALL_COUNTER_TRAITS(NAME)                        \
    {                                                       \
        static const wchar_t* GetName() { return L#NAME; }  \
    };                                                      \
                                                            \
    mutable CallCounter<CALL_COUNTER_TRAITS(NAME)> NAME;

//
// CALL_COUNTER_WITH_MOCK defines a member variable that can be used to count
// calls to a method and mock it as well.  eg.
//
// class SomeClass
// {
// public:
//     CALL_COUNTER_WITH_MOCK(BarMethod, HRESULT(int));
//
//     IFACEMETHODIMP Bar(int value) { return BarMethod.WasCalled(value); }
// }
//
// TEST_METHOD(MyTest)
// {
//     SomeClass c;
//     c.BarMethod.SetExpectedCalls(1, [](int v) { Assert::AreEqual(1, v); return S_OK; });
//     c.Bar(2);
//     Expectations::Instance()->Validate();
// }
//
// MyTest will fail because Bar was called with 2 rather than 1.
//
#define CALL_COUNTER_WITH_MOCK(NAME, FN)                        \
    struct CALL_COUNTER_TRAITS(NAME)                            \
    {                                                           \
        static const wchar_t* GetName() { return L#NAME; }      \
    };                                                          \
                                                                \
    mutable CallCounterWithMock<CALL_COUNTER_TRAITS(NAME), FN> NAME;



//
// Base class for any expectation that can be tracked by Expectations.
//
class Expectation
{
public:
    Expectation() {}

    virtual void Validate() = 0;

    // Expectation is expected to be stored in a shared_ptr, therefore we don't
    // want to allow copying or reassignment.
    Expectation(const Expectation&) = delete;
    Expectation(Expectation&&) = delete;
    Expectation& operator=(const Expectation&) = delete;
    Expectation& operator=(Expectation&&) = delete;
};

//
// Tracks expectations and can call Validate() on all of them.
//
class Expectations
{
    std::vector<std::weak_ptr<Expectation>> m_expectations;

private:
    Expectations()
    {
    }

public:
    static Expectations* Instance()
    {
        static Expectations instance;
        return &instance;
    }

    void Add(std::shared_ptr<Expectation> e)
    {
        m_expectations.push_back(e);
    }

    void Validate()
    {
        for (auto& weake : m_expectations)
        {
            if (auto e = weake.lock())
                e->Validate();
        }

        // Since we expect Validate() to be called at least once for each test,
        // this is a good point to remove stale references.  It does mean that
        // we'll have a build up of stale references from the previous test, but
        // this is better than having an explicit Init() function, or scanning
        // the list on each Add().
        RemoveStaleExpectations();
    }

private:
    void RemoveStaleExpectations()
    {
        auto newEnd = std::remove_if(m_expectations.begin(), m_expectations.end(),
            [](std::weak_ptr<Expectation> const& value)
            {
                return value.expired();
            });

        m_expectations.erase(newEnd, m_expectations.end());
    }
};


//
// This counts how many times a method is called.  This is not intended to be
// used directly -- use CALL_COUNTER instead.
//
class CallCounterExpectation : public Expectation
{
    std::wstring m_name;
    bool m_allowAnyCall;
    int m_expectedCallCount;
    int m_actualCallCount;

public:
    CallCounterExpectation(std::wstring name)
        : m_name(name + L": ")
        , m_allowAnyCall(false)
        , m_expectedCallCount(0)
        , m_actualCallCount(0)
    {
    }

    void AllowAnyCall()
    {
        m_allowAnyCall = true;
    }

    void SetExpectedCalls(int value)
    {
        Validate();
        m_expectedCallCount = value;
        m_actualCallCount = 0;
        m_allowAnyCall = false;
    }

    void WasCalled()
    {
        if (m_allowAnyCall)
            return;

        ++m_actualCallCount;

        Assert::IsTrue(m_actualCallCount <= m_expectedCallCount, Message(L"unexpected call").c_str());
    }


    virtual void Validate() override
    {
        if (m_allowAnyCall)
            return;

        Assert::AreEqual(m_expectedCallCount, m_actualCallCount, Message(L"call count mismatch").c_str());
    }

    std::wstring Message(const wchar_t* msg)
    {
        return m_name + msg;
    }
};


//
// Helper used to generate the name of the traits class that's used to provide a
// name without needing to explicitly pass it to the constructor.
//
#define CALL_COUNTER_TRAITS(NAME) CallCounter_##NAME##_Traits


//
// Helper object that catches when CallCounter is used without an explicit
// traits class to produce a friendlyish error message.
//
struct DefaultCallCounterTraits
{
    template<typename T = int>  // template parameter so assert only fires if this method actually called
    static const wchar_t* GetName()
    {
        static_assert(false, "Use CALL_COUNTER or CALL_COUNTER_WITH_MOCK to get a default-constructable CallCounter.  Otherwise pass the name to the constructor");
        return nullptr;
    }
};

//
// CallCounter can be used to track how many times something is called.
//
// Use the CALL_COUNTER macro to create one.
//
template<typename TRAITS = DefaultCallCounterTraits>
class CallCounter
{
    std::shared_ptr<CallCounterExpectation> m_expectation;

public:
    CallCounter()
        : CallCounter(TRAITS::GetName())
    {
    }

    CallCounter(std::wstring name)
        : m_expectation(std::make_shared<CallCounterExpectation>(name))
    {
        // We need to have stored m_expectation in a shared_ptr before we can
        // add it to the expectations list.
        Expectations::Instance()->Add(m_expectation);
    }

    void AllowAnyCall()
    {
        m_expectation->AllowAnyCall();
    }

    void SetExpectedCalls(int value)
    {
        m_expectation->SetExpectedCalls(value);
    }

    void WasCalled()
    {
        m_expectation->WasCalled();
    }

    void Validate()
    {
        m_expectation->Validate();
    }

    std::wstring Message(const wchar_t* msg)
    {
        return m_expectation->Message(msg);
    }
};


//
// 
//

template<typename TRAITS, typename FN>
class CallCounterWithMock : public CallCounter<TRAITS>
{
    std::function<FN> m_mock;

public:
    void AllowAnyCall(std::function<FN> mock = nullptr)
    {
        CallCounter::AllowAnyCall();
        m_mock = mock;
    }

    template<typename T>
    void AllowAnyCallAlwaysCopyValueToParam(ComPtr<T> value)
    {
        CallCounter::AllowAnyCall();
        m_mock = [value](std::function<FN>::argument_type arg)
            {
                // value.CopyTo returns an HRESULT.  However, the method we're
                // mocking may have a result_type of void.  This little dance
                // through Return() allows us to 'return' a void 'value'.
                return Return<std::function<FN>::result_type>(value.CopyTo(arg));
            };
    }

    void SetExpectedCalls(int value, std::function<FN> mock = nullptr)
    {
        CallCounter::SetExpectedCalls(value);
        m_mock = mock;
    }

    template<typename... ARGS>
    auto WasCalled(ARGS... args) -> decltype(m_mock(args...))
    {
        CallCounter::WasCalled();
        if (m_mock)
            return m_mock(args...);
        else
            return MakeDefaultReturnValue<std::function<FN>::result_type>();
    }

private:
    template<typename RET>
    static RET MakeDefaultReturnValue()
    {
        return RET{};
    }
    
    template<>
    static void MakeDefaultReturnValue()
    {
    }

    template<typename RET, typename T>
    static typename std::enable_if<!std::is_void<RET>::value, RET>::type Return(T value)
    {
        return value;
    }

    template<typename RET, typename T>
    static typename std::enable_if<std::is_void<RET>::value, RET>::type Return(T value)
    {
    }
    

};


//
// Helper for event handlers that track how many times they've been invoked.
//
// eg:
//
//    auto onDraw = MockEventHandler<DrawEventHandlerType>(L"Draw");
//    onDraw.SetExpectedCalls(1);
//    ThrowIfFailed(m_control->add_Draw(onDraw.Get(), &m_ignoredToken));
//
// The expectations are validated on destruction.
//

enum class ExpectedEventParams
{
    DontCare,
    BothNull,
    OnlySender,
    Both
};


template<typename EventHandlerType>
class MockEventHandler
{
    CallCounter<> m_callCounter;
    ComPtr<EventHandlerType> m_callback;
    ExpectedEventParams m_expectedParams;

public:
    MockEventHandler(std::wstring name = L"", ExpectedEventParams expectedParams = ExpectedEventParams::DontCare)
        : m_callCounter(name)
        , m_expectedParams(expectedParams)
    {
    }

    EventHandlerType* Get()
    {
        if (!m_callback)
        {
            m_callback = Callback<EventHandlerType>(this, &MockEventHandler::OnInvoke);
            CheckMakeResult(m_callback);
        }
        return m_callback.Get();
    }

    void AllowAnyCall()
    {
        m_callCounter.AllowAnyCall();
    }

    void SetExpectedCalls(int value)
    {
        m_callCounter.SetExpectedCalls(value);
    }

    void Validate()
    {
        m_callCounter.Validate();
    }

private:
    std::wstring Message(const wchar_t* msg)
    {
        return m_callCounter.Message(msg);
    }

    typedef typename EventHandlerType::TSender_complex TSender;
    typedef typename EventHandlerType::TArgs_complex   TArgs;
    typedef typename ABI::Windows::Foundation::Internal::GetAbiType<TSender>::type TSender_abi;
    typedef typename ABI::Windows::Foundation::Internal::GetAbiType<TArgs>::type   TArgs_abi;

    HRESULT OnInvoke(TSender_abi sender, TArgs_abi args)
    {
        m_callCounter.WasCalled();

        switch (m_expectedParams)
        {
        case ExpectedEventParams::BothNull:
            Assert::IsNull(sender, Message(L"unexpected non-null sender parameter").c_str());
            Assert::IsNull(args, Message(L"unexpected non-null args parameter").c_str());
            break;

        case ExpectedEventParams::OnlySender:
            Assert::IsNotNull(sender, Message(L"unexpected null sender parameter").c_str());
            Assert::IsNull(args, Message(L"unexpected non-null args parameter").c_str());
            break;

        case ExpectedEventParams::Both:
            Assert::IsNotNull(sender, Message(L"unexpected null sender parameter").c_str());
            Assert::IsNotNull(args, Message(L"unexpected null args parameter").c_str());
            break;
        }

        return S_OK;
    }
};


template<typename DELEGATE>
class MockEventSource : public RuntimeClass<RuntimeClassFlags<ClassicCom>, IUnknown>
{
    EventSource<DELEGATE> m_eventList;

public:
    struct Empty{};

    CallCounter<Empty> AddMethod;
    CallCounter<Empty> RemoveMethod;
    
    MockEventSource(std::wstring name)
        : AddMethod(name + L"Add")
        , RemoveMethod(name + L"Remove")
    {
        AddMethod.AllowAnyCall();
        RemoveMethod.AllowAnyCall();
    }

    IFACEMETHODIMP add_Event(DELEGATE* delegate, EventRegistrationToken* token)
    {
        AddMethod.WasCalled();
        return m_eventList.Add(delegate, token);
    }

    IFACEMETHODIMP remove_Event(EventRegistrationToken token)
    {
        RemoveMethod.WasCalled();
        return m_eventList.Remove(token);
    }

    template<typename TSender, typename TArgs>
    HRESULT InvokeAll(TSender sender, TArgs arg)
    {
        return m_eventList.InvokeAll(sender, arg);
    }

    RegisteredEvent Add(DELEGATE* delegate)
    {
        return RegisteredEvent(
            this,
            &MockEventSource::add_Event,
            &MockEventSource::remove_Event,
            delegate);
    }
};

typedef MockEventSource<IEventHandler<IInspectable*>> MockEventSourceUntyped;
