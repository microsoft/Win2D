// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

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

#define TEST_METHOD_EX(METHOD_NAME)                                             \
    TEST_METHOD(METHOD_NAME)                                                    \
    {                                                                           \
        LifespanInfo::Reset();                                                  \
                                                                                \
        Expectations e;                                                         \
        METHOD_NAME##_();                                                       \
        e.Validate();                                                           \
                                                                                \
        auto liveObjectCount = LifespanInfo::ReportLiveObjects();               \
        Assert::AreEqual<size_t>(0, liveObjectCount, L"LifespanTracker leak");  \
    }                                                                           \
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
    virtual ~Expectation() = default;

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
        SetExpectedCalls(value, value);
    }

    void SetExpectedCalls(int minCalls, int maxCalls)
    {
        Validate();
        m_minimumCallCount = minCalls;
        m_maximumCallCount = maxCalls;
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

    void SetExpectedCalls(int minCalls, int maxCalls)
    {
        m_expectation->SetExpectedCalls(minCalls, maxCalls);
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
    CallCounterWithMock()
    {
    }

    CallCounterWithMock(std::wstring name)
        : CallCounter(name)
    {
    }

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
        SetExpectedCalls(value, value, mock);
    }

    void SetExpectedCalls(int minCalls, int maxCalls, std::function<FN> mock = nullptr)
    {
        CallCounter::SetExpectedCalls(minCalls, maxCalls);
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
// Helpers for generating mock methods
//

template<typename F>
struct FuncInfo;

template<typename R>
struct FuncInfo<R()>
{
    typedef R ReturnType;
};

template<typename R, typename A0>
struct FuncInfo<R(A0)>
{
    typedef R ReturnType;
    typedef A0 Arg0Type;
};

template<typename R, typename A0, typename A1>
struct FuncInfo<R(A0, A1)>
{
    typedef R ReturnType;
    typedef A0 Arg0Type;
    typedef A1 Arg1Type;
};

template<typename R, typename A0, typename A1, typename A2>
struct FuncInfo<R(A0, A1, A2)>
{
    typedef R ReturnType;
    typedef A0 Arg0Type;
    typedef A1 Arg1Type;
    typedef A2 Arg2Type;
};

template<typename R, typename A0, typename A1, typename A2, typename A3>
struct FuncInfo<R(A0, A1, A2, A3)>
{
    typedef R ReturnType;
    typedef A0 Arg0Type;
    typedef A1 Arg1Type;
    typedef A2 Arg2Type;
    typedef A3 Arg3Type;
};

template<typename R, typename A0, typename A1, typename A2, typename A3, typename A4>
struct FuncInfo<R(A0, A1, A2, A3, A4)>
{
    typedef R ReturnType;
    typedef A0 Arg0Type;
    typedef A1 Arg1Type;
    typedef A2 Arg2Type;
    typedef A3 Arg3Type;
    typedef A4 Arg4Type;
};

template<typename R, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5>
struct FuncInfo<R(A0, A1, A2, A3, A4, A5)>
{
    typedef R ReturnType;
    typedef A0 Arg0Type;
    typedef A1 Arg1Type;
    typedef A2 Arg2Type;
    typedef A3 Arg3Type;
    typedef A4 Arg4Type;
    typedef A5 Arg5Type;
};

template<typename R, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
struct FuncInfo<R(A0, A1, A2, A3, A4, A5, A6)>
{
    typedef R ReturnType;
    typedef A0 Arg0Type;
    typedef A1 Arg1Type;
    typedef A2 Arg2Type;
    typedef A3 Arg3Type;
    typedef A4 Arg4Type;
    typedef A5 Arg5Type;
    typedef A6 Arg6Type;
};

template<typename R, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
struct FuncInfo<R(A0, A1, A2, A3, A4, A5, A6, A7)>
{
    typedef R ReturnType;
    typedef A0 Arg0Type;
    typedef A1 Arg1Type;
    typedef A2 Arg2Type;
    typedef A3 Arg3Type;
    typedef A4 Arg4Type;
    typedef A5 Arg5Type;
    typedef A6 Arg6Type;
    typedef A7 Arg7Type;
};

template<typename R, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
struct FuncInfo<R(A0, A1, A2, A3, A4, A5, A6, A7, A8)>
{
    typedef R ReturnType;
    typedef A0 Arg0Type;
    typedef A1 Arg1Type;
    typedef A2 Arg2Type;
    typedef A3 Arg3Type;
    typedef A4 Arg4Type;
    typedef A5 Arg5Type;
    typedef A6 Arg6Type;
    typedef A7 Arg7Type;
    typedef A8 Arg8Type;
};

template<typename R, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
struct FuncInfo<R(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9)>
{
    typedef R ReturnType;
    typedef A0 Arg0Type;
    typedef A1 Arg1Type;
    typedef A2 Arg2Type;
    typedef A3 Arg3Type;
    typedef A4 Arg4Type;
    typedef A5 Arg5Type;
    typedef A6 Arg6Type;
    typedef A7 Arg7Type;
    typedef A8 Arg8Type;
    typedef A9 Arg9Type;
};

#define MOCK_METHOD_IMPL0(name, modifier, ...)                          \
    virtual FuncInfo<__VA_ARGS__>::ReturnType STDMETHODCALLTYPE name() modifier override \
    {                                                                   \
        return name ## Method.WasCalled();                              \
    }                                                                   \
    CALL_COUNTER_WITH_MOCK(name ## Method, __VA_ARGS__);

#define MOCK_METHOD_IMPL1(name, modifier, ...)                          \
    virtual FuncInfo<__VA_ARGS__>::ReturnType STDMETHODCALLTYPE name(   \
        FuncInfo<__VA_ARGS__>::Arg0Type a0) modifier override           \
    {                                                                   \
        return name ## Method.WasCalled(a0);                            \
    }                                                                   \
    CALL_COUNTER_WITH_MOCK(name ## Method, __VA_ARGS__);

#define MOCK_METHOD_IMPL2(name, modifier, ...)                          \
    virtual FuncInfo<__VA_ARGS__>::ReturnType STDMETHODCALLTYPE name(   \
        FuncInfo<__VA_ARGS__>::Arg0Type a0,                             \
        FuncInfo<__VA_ARGS__>::Arg1Type a1) modifier override           \
    {                                                                   \
        return name ## Method.WasCalled(a0, a1);                        \
    }                                                                   \
    CALL_COUNTER_WITH_MOCK(name ## Method, __VA_ARGS__);

#define MOCK_METHOD_IMPL3(name, modifier, ...)                          \
    virtual FuncInfo<__VA_ARGS__>::ReturnType STDMETHODCALLTYPE name(   \
        FuncInfo<__VA_ARGS__>::Arg0Type a0,                             \
        FuncInfo<__VA_ARGS__>::Arg1Type a1,                             \
        FuncInfo<__VA_ARGS__>::Arg2Type a2) modifier override           \
    {                                                                   \
        return name ## Method.WasCalled(a0, a1, a2);                    \
    }                                                                   \
    CALL_COUNTER_WITH_MOCK(name ## Method, __VA_ARGS__);

#define MOCK_METHOD_IMPL4(name, modifier, ...)                          \
    virtual FuncInfo<__VA_ARGS__>::ReturnType STDMETHODCALLTYPE name(   \
        FuncInfo<__VA_ARGS__>::Arg0Type a0,                             \
        FuncInfo<__VA_ARGS__>::Arg1Type a1,                             \
        FuncInfo<__VA_ARGS__>::Arg2Type a2,                             \
        FuncInfo<__VA_ARGS__>::Arg3Type a3) modifier override           \
    {                                                                   \
        return name ## Method.WasCalled(a0, a1, a2, a3);                \
    }                                                                   \
    CALL_COUNTER_WITH_MOCK(name ## Method, __VA_ARGS__);

#define MOCK_METHOD_IMPL5(name, modifier, ...)                          \
    virtual FuncInfo<__VA_ARGS__>::ReturnType STDMETHODCALLTYPE name(   \
        FuncInfo<__VA_ARGS__>::Arg0Type a0,                             \
        FuncInfo<__VA_ARGS__>::Arg1Type a1,                             \
        FuncInfo<__VA_ARGS__>::Arg2Type a2,                             \
        FuncInfo<__VA_ARGS__>::Arg3Type a3,                             \
        FuncInfo<__VA_ARGS__>::Arg4Type a4) modifier override           \
    {                                                                   \
        return name ## Method.WasCalled(a0, a1, a2, a3, a4);            \
    }                                                                   \
    CALL_COUNTER_WITH_MOCK(name ## Method, __VA_ARGS__);

#define MOCK_METHOD_IMPL6(name, modifier, ...)                          \
    virtual FuncInfo<__VA_ARGS__>::ReturnType STDMETHODCALLTYPE name(   \
        FuncInfo<__VA_ARGS__>::Arg0Type a0,                             \
        FuncInfo<__VA_ARGS__>::Arg1Type a1,                             \
        FuncInfo<__VA_ARGS__>::Arg2Type a2,                             \
        FuncInfo<__VA_ARGS__>::Arg3Type a3,                             \
        FuncInfo<__VA_ARGS__>::Arg4Type a4,                             \
        FuncInfo<__VA_ARGS__>::Arg5Type a5) modifier override           \
    {                                                                   \
        return name ## Method.WasCalled(a0, a1, a2, a3, a4, a5);        \
    }                                                                   \
    CALL_COUNTER_WITH_MOCK(name ## Method, __VA_ARGS__);

#define MOCK_METHOD_IMPL7(name, modifier, ...)                          \
    virtual FuncInfo<__VA_ARGS__>::ReturnType STDMETHODCALLTYPE name(   \
        FuncInfo<__VA_ARGS__>::Arg0Type a0,                             \
        FuncInfo<__VA_ARGS__>::Arg1Type a1,                             \
        FuncInfo<__VA_ARGS__>::Arg2Type a2,                             \
        FuncInfo<__VA_ARGS__>::Arg3Type a3,                             \
        FuncInfo<__VA_ARGS__>::Arg4Type a4,                             \
        FuncInfo<__VA_ARGS__>::Arg5Type a5,                             \
        FuncInfo<__VA_ARGS__>::Arg6Type a6) modifier override           \
    {                                                                   \
        return name ## Method.WasCalled(a0, a1, a2, a3, a4, a5, a6);    \
    }                                                                   \
    CALL_COUNTER_WITH_MOCK(name ## Method, __VA_ARGS__);

#define MOCK_METHOD_IMPL8(name, modifier, ...)                          \
    virtual FuncInfo<__VA_ARGS__>::ReturnType STDMETHODCALLTYPE name(   \
        FuncInfo<__VA_ARGS__>::Arg0Type a0,                             \
        FuncInfo<__VA_ARGS__>::Arg1Type a1,                             \
        FuncInfo<__VA_ARGS__>::Arg2Type a2,                             \
        FuncInfo<__VA_ARGS__>::Arg3Type a3,                             \
        FuncInfo<__VA_ARGS__>::Arg4Type a4,                             \
        FuncInfo<__VA_ARGS__>::Arg5Type a5,                             \
        FuncInfo<__VA_ARGS__>::Arg6Type a6,                             \
        FuncInfo<__VA_ARGS__>::Arg7Type a7) modifier override           \
    {                                                                   \
        return name ## Method.WasCalled(a0, a1, a2, a3, a4, a5, a6, a7); \
    }                                                                   \
    CALL_COUNTER_WITH_MOCK(name ## Method, __VA_ARGS__);

#define MOCK_METHOD_IMPL9(name, modifier, ...)                          \
    virtual FuncInfo<__VA_ARGS__>::ReturnType STDMETHODCALLTYPE name(   \
        FuncInfo<__VA_ARGS__>::Arg0Type a0,                             \
        FuncInfo<__VA_ARGS__>::Arg1Type a1,                             \
        FuncInfo<__VA_ARGS__>::Arg2Type a2,                             \
        FuncInfo<__VA_ARGS__>::Arg3Type a3,                             \
        FuncInfo<__VA_ARGS__>::Arg4Type a4,                             \
        FuncInfo<__VA_ARGS__>::Arg5Type a5,                             \
        FuncInfo<__VA_ARGS__>::Arg6Type a6,                             \
        FuncInfo<__VA_ARGS__>::Arg7Type a7,                             \
        FuncInfo<__VA_ARGS__>::Arg8Type a8) modifier override           \
    {                                                                   \
        return name ## Method.WasCalled(a0, a1, a2, a3, a4, a5, a6, a7, a8); \
    }                                                                   \
    CALL_COUNTER_WITH_MOCK(name ## Method, __VA_ARGS__);

#define MOCK_METHOD_IMPL10(name, modifier, ...)                         \
    virtual FuncInfo<__VA_ARGS__>::ReturnType STDMETHODCALLTYPE name(   \
        FuncInfo<__VA_ARGS__>::Arg0Type a0,                             \
        FuncInfo<__VA_ARGS__>::Arg1Type a1,                             \
        FuncInfo<__VA_ARGS__>::Arg2Type a2,                             \
        FuncInfo<__VA_ARGS__>::Arg3Type a3,                             \
        FuncInfo<__VA_ARGS__>::Arg4Type a4,                             \
        FuncInfo<__VA_ARGS__>::Arg5Type a5,                             \
        FuncInfo<__VA_ARGS__>::Arg6Type a6,                             \
        FuncInfo<__VA_ARGS__>::Arg7Type a7,                             \
        FuncInfo<__VA_ARGS__>::Arg8Type a8,                             \
        FuncInfo<__VA_ARGS__>::Arg9Type a9) modifier override           \
    {                                                                   \
        return name ## Method.WasCalled(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9); \
    }                                                                   \
    CALL_COUNTER_WITH_MOCK(name ## Method, __VA_ARGS__);


#define MOCK_METHOD0(name, ...) MOCK_METHOD_IMPL0(name, , __VA_ARGS__)
#define MOCK_METHOD1(name, ...) MOCK_METHOD_IMPL1(name, , __VA_ARGS__)
#define MOCK_METHOD2(name, ...) MOCK_METHOD_IMPL2(name, , __VA_ARGS__)
#define MOCK_METHOD3(name, ...) MOCK_METHOD_IMPL3(name, , __VA_ARGS__)
#define MOCK_METHOD4(name, ...) MOCK_METHOD_IMPL4(name, , __VA_ARGS__)
#define MOCK_METHOD5(name, ...) MOCK_METHOD_IMPL5(name, , __VA_ARGS__)
#define MOCK_METHOD6(name, ...) MOCK_METHOD_IMPL6(name, , __VA_ARGS__)
#define MOCK_METHOD7(name, ...) MOCK_METHOD_IMPL7(name, , __VA_ARGS__)
#define MOCK_METHOD8(name, ...) MOCK_METHOD_IMPL8(name, , __VA_ARGS__)
#define MOCK_METHOD9(name, ...) MOCK_METHOD_IMPL9(name, , __VA_ARGS__)
#define MOCK_METHOD10(name, ...) MOCK_METHOD_IMPL10(name, , __VA_ARGS__)

#define MOCK_METHOD0_CONST(name, ...) MOCK_METHOD_IMPL0(name, const, __VA_ARGS__)
#define MOCK_METHOD1_CONST(name, ...) MOCK_METHOD_IMPL1(name, const, __VA_ARGS__)
#define MOCK_METHOD2_CONST(name, ...) MOCK_METHOD_IMPL2(name, const, __VA_ARGS__)
#define MOCK_METHOD3_CONST(name, ...) MOCK_METHOD_IMPL3(name, const, __VA_ARGS__)
#define MOCK_METHOD4_CONST(name, ...) MOCK_METHOD_IMPL4(name, const, __VA_ARGS__)
#define MOCK_METHOD5_CONST(name, ...) MOCK_METHOD_IMPL5(name, const, __VA_ARGS__)
#define MOCK_METHOD6_CONST(name, ...) MOCK_METHOD_IMPL6(name, const, __VA_ARGS__)
#define MOCK_METHOD7_CONST(name, ...) MOCK_METHOD_IMPL7(name, const, __VA_ARGS__)
#define MOCK_METHOD8_CONST(name, ...) MOCK_METHOD_IMPL8(name, const, __VA_ARGS__)
#define MOCK_METHOD9_CONST(name, ...) MOCK_METHOD_IMPL9(name, const, __VA_ARGS__)
#define MOCK_METHOD10_CONST(name, ...) MOCK_METHOD_IMPL10(name, const, __VA_ARGS__)


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
    
    void ExpectAtLeastOneCall()
    {
        m_callCounter.ExpectAtLeastOneCall();
        m_function = nullptr;
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
