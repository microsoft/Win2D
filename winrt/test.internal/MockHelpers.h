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
//    TEST_METHOD(MyTest)
//    {
//        CallCounter counter(L"TestCounter");
//        counter.SetExpectedCalls(1);
//        Expectations::Instance()->Validate();
//    }
//
//


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
    Expectation& operator=(const Expectation&) = delete;
    Expectation(Expectation&&) = delete;
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
// used directly -- use CallCounter instead.
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
// CallCounter can be used to track how many times something is called.
//
class CallCounter
{
    std::shared_ptr<CallCounterExpectation> m_expectation;

public:
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
// Helper for event handlers that track how many times they've been invoked.
//
// eg:
//
//    auto onDraw = MockEventHandler<DrawEventHandlerType>(L"Draw");
//    onDraw.SetExpectedInvokes(1);
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
    CallCounter m_callCounter;
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
