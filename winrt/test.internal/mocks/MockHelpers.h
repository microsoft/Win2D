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

#include <RegisteredEvent.h>

//
// Helpers for common mocking operations (eg counting how many times a method
// was called, or an event was invoked).
//
// If any of these expectations are used then TEST_METHOD_EX must be used.
//
//    CALL_COUNTER(TestCounter);
//
//    TEST_METHOD_EX(MyTest)
//    {
//        TestCounter.SetExpectedCalls(1);
//    }
//

#define TEST_METHOD_EX(METHOD_NAME)             \
    TEST_METHOD(METHOD_NAME)                    \
    {                                           \
        Expectations e;                         \
        METHOD_NAME##_();                       \
        e.Validate();                           \
    }                                           \
    void METHOD_NAME##_()

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
// TEST_METHOD_EX(MyTest)
// {
//     SomeClass c;
//     c.FooMethod.SetExpectedCalls(1);
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
// TEST_METHOD_EX(MyTest)
// {
//     SomeClass c;
//     c.BarMethod.SetExpectedCalls(1, [](int v) { Assert::AreEqual(1, v); return S_OK; });
//     c.Bar(2);
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
    virtual bool TryValidate() = 0;

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
    static Expectations* s_instance;

    std::vector<std::shared_ptr<Expectation>> m_expectations;

public:
    static Expectations* Instance()
    {
        Assert::IsNotNull(s_instance, L"Expectations not initialized: make sure TEST_METHOD_EX is used and no expectations are set in TEST_METHOD_INITIALIZE");
        return s_instance;
    }

    Expectations()
    {
        Assert::IsNull(s_instance, L"Multiple Expectations instances found.  Only a single instance may be active at any one time.");
        s_instance = this;
    }

    ~Expectations()
    {
        assert(s_instance == this);
        s_instance = nullptr;
    }
    
    void Add(std::shared_ptr<Expectation> e)
    {
        m_expectations.push_back(e);
    }

    void Validate()
    {
        for (auto& e : m_expectations)
        {
            e->Validate();
        }
    }

    void ValidateNotSatisfied()
    {
        bool allSatisfied = true;
        for (auto& e : m_expectations)
        {
                allSatisfied = allSatisfied && e->TryValidate();
        }

        Assert::IsFalse(allSatisfied, L"All expectations were satisfied when at least one expected not to be");
    }
};

__declspec(selectany) Expectations* Expectations::s_instance;


//
// This counts how many times a method is called.  This is not intended to be
// used directly -- use CALL_COUNTER instead.
//
class CallCounterExpectation : public Expectation
{
    std::wstring m_name;
    int m_minimumCallCount;
    int m_maximumCallCount;
    int m_actualCallCount;

public:
    CallCounterExpectation(std::wstring name)
        : m_name(name + L": ")
        , m_minimumCallCount(-1)
        , m_maximumCallCount(0)
        , m_actualCallCount(0)
    {
    }

    void AllowAnyCall()
    {
        Validate();
        m_minimumCallCount = 0;
        m_maximumCallCount = INT_MAX;
        m_actualCallCount = 0;
    }

    void SetExpectedCalls(int value)
    {
        Validate();
        m_minimumCallCount = value;
        m_maximumCallCount = value;
        m_actualCallCount = 0;
    }

    void ExpectAtLeastOneCall()
    {
        Validate();
        m_minimumCallCount = 1;
        m_maximumCallCount = INT_MAX;
        m_actualCallCount = 0;
    }

    void WasCalled()
    {
        ++m_actualCallCount;

        Assert::IsTrue(m_actualCallCount <= m_maximumCallCount, Message(L"unexpected call").c_str());
    }

    int GetCurrentCallCount() const
    {
        return m_actualCallCount;
    }

    virtual void Validate() override
    {
        if (m_minimumCallCount == m_maximumCallCount)
        {
            // Special case when min==max so we can have a better assert message
            Assert::AreEqual(m_maximumCallCount, m_actualCallCount, Message(L"call count mismatch").c_str());
        }
        else
        {
            Assert::IsTrue(m_actualCallCount >= m_minimumCallCount, Message(L"minimum call count not met").c_str());
            Assert::IsTrue(m_actualCallCount <= m_maximumCallCount, Message(L"maximum call count not met").c_str());
        }
    }

    virtual bool TryValidate() override
    {
        if (m_actualCallCount < m_minimumCallCount)
            return false;

        if (m_actualCallCount > m_maximumCallCount)
            return false;

        return true;
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

    void ExpectAtLeastOneCall()
    {
        m_expectation->ExpectAtLeastOneCall();
    }

    void WasCalled()
    {
        m_expectation->WasCalled();
    }

    int GetCurrentCallCount() const
    {
        return m_expectation->GetCurrentCallCount();
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

    void ExpectAtLeastOneCall(std::function<FN> mock = nullptr)
    {
        CallCounter::ExpectAtLeastOneCall();
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

    bool HasMock()
    {
        return static_cast<bool>(m_mock);
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
// Figure out sender/argument types for IEventHandler and ITypedEventHandler.
//

template<typename EVENTHANDLER>
struct EventHandlerAbiTypes {};

template<typename A>
struct EventHandlerAbiTypes<ABI::Windows::Foundation::IEventHandler<A>>
{
    typedef typename IEventHandler<A>::T_complex Args_complex;

    typedef IInspectable* Sender;
    typedef typename ABI::Windows::Foundation::Internal::GetAbiType<Args_complex>::type Args;
};

template<typename S, typename A>
struct EventHandlerAbiTypes<ABI::Windows::Foundation::ITypedEventHandler<S,A>>
{
    typedef typename ITypedEventHandler<S,A>::TSender_complex Sender_complex;
    typedef typename ITypedEventHandler<S,A>::TArgs_complex Args_complex;

    typedef typename ABI::Windows::Foundation::Internal::GetAbiType<Sender_complex>::type Sender;
    typedef typename ABI::Windows::Foundation::Internal::GetAbiType<Args_complex>::type Args;
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
    typedef typename EventHandlerAbiTypes<EventHandlerType>::Sender TSender_abi;
    typedef typename EventHandlerAbiTypes<EventHandlerType>::Args   TArgs_abi;

    CallCounter<> m_callCounter;
    ComPtr<EventHandlerType> m_callback;
    ExpectedEventParams m_expectedParams;
    HRESULT m_nextReturnValue;
    std::function<HRESULT(TSender_abi, TArgs_abi)> m_function;

public:
    MockEventHandler(std::wstring name = L"", ExpectedEventParams expectedParams = ExpectedEventParams::DontCare)
        : m_callCounter(name)
        , m_expectedParams(expectedParams)
        , m_nextReturnValue(S_OK)
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

    void AllowAnyCall(std::function<HRESULT(TSender_abi, TArgs_abi)> fn)
    {
        m_callCounter.AllowAnyCall();
        m_function = fn;
    }

    void SetExpectedCalls(int value, HRESULT nextReturnValue = S_OK)
    {
        m_callCounter.SetExpectedCalls(value);
        m_nextReturnValue = nextReturnValue;
        m_function = nullptr;
    }

    void SetExpectedCalls(int value, std::function<HRESULT(TSender_abi, TArgs_abi)> fn)
    {
        SetExpectedCalls(value);
        m_function = fn;
        m_nextReturnValue = S_OK;
    }
    
    int GetCurrentCallCount() const
    {
        return m_callCounter.GetCurrentCallCount();
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

        if (m_function)
            return m_function(sender, args);
        else
            return m_nextReturnValue;
    }
};


template<typename DELEGATE>
class MockEventSource : public RuntimeClass<RuntimeClassFlags<ClassicCom>, IUnknown>
{
    EventSource<DELEGATE, InvokeModeOptions<StopOnFirstError>> m_eventList;

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
