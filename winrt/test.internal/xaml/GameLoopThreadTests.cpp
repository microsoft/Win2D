// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

#include <lib/xaml/GameLoopThread.h>

#include "MockCoreIndependentInputSource.h"

class Waiter
{
    Event m_event;

public:
    Waiter()
        : m_event(CreateEventEx(NULL, NULL, CREATE_EVENT_MANUAL_RESET, EVENT_ALL_ACCESS))
    {
    }

    void Set()
    {
        SetEvent(m_event.Get());
    }

    void Wait(int timeout = 5 * 1000)
    {
        Assert::AreEqual(WAIT_OBJECT_0, WaitForSingleObjectEx(m_event.Get(), timeout, false), L"timed out waiting");
    }
};

class FakeDispatcher : public MockDispatcher
{
    std::mutex m_mutex;
    std::condition_variable m_conditionVariable;
    bool m_stopped;
    std::vector<ComPtr<AnimatedControlAsyncAction>> m_pendingActions;    

public:
    CALL_COUNTER_WITH_MOCK(RunAsyncValidation, void(CoreDispatcherPriority));

    FakeDispatcher()
        : m_stopped(false)
    {
        RunAsyncValidation.AllowAnyCall();
    }

    virtual IFACEMETHODIMP RunAsync(
        CoreDispatcherPriority priority,
        IDispatchedHandler* agileCallback,
        IAsyncAction** asyncAction) override
    {
        RunAsyncValidation.WasCalled(priority);

        Lock lock(m_mutex);

        auto action = Make<AnimatedControlAsyncAction>(agileCallback);
        m_pendingActions.push_back(action);
        m_conditionVariable.notify_all();

        return action.CopyTo(asyncAction);
    }

    virtual IFACEMETHODIMP StopProcessEvents() override
    {
        Lock lock(m_mutex);

        m_stopped = true;
        m_conditionVariable.notify_all();

        return S_OK;
    }

    virtual IFACEMETHODIMP ProcessEvents(
        CoreProcessEventsOption options) override
    {
        Assert::IsTrue(options == CoreProcessEventsOption_ProcessUntilQuit);

        Lock lock(m_mutex);
        m_stopped = false;

        while (!m_stopped)
        {
            std::vector<ComPtr<AnimatedControlAsyncAction>> actions;
            std::swap(actions, m_pendingActions);

            if (!actions.empty())
            {
                lock.unlock();
                for (auto& action : actions)
                {
                    action->InvokeAndFireCompletion();
                }
                lock.lock();
            }

            m_conditionVariable.wait(lock);
        }

        return S_OK;
    }
};

struct MockGameLoopClient : public ICanvasGameLoopClient
{
    CALL_COUNTER_WITH_MOCK(GameLoopStarting, void());
    CALL_COUNTER_WITH_MOCK(GameLoopStopped, void());

    virtual void OnGameLoopStarting() override 
    {
        GameLoopStarting.WasCalled();
    }

    virtual void OnGameLoopStopped() override 
    {
        GameLoopStopped.WasCalled();
    }

    virtual bool Tick(CanvasSwapChain* target, bool areResourcesCreated) override 
    {
        return true;
    }

    virtual void OnTickLoopEnded() override 
    {
    }
};

TEST_CLASS(GameLoopThreadTests)
{
    struct Fixture
    {
        ComPtr<FakeDispatcher> Dispatcher;
        ComPtr<StubSwapChainPanel> SwapChainPanel;
        MockGameLoopClient Client;
        std::unique_ptr<IGameLoopThread> Thread;

        Fixture()
            : Dispatcher(Make<FakeDispatcher>())
            , SwapChainPanel(Make<StubSwapChainPanel>())
        {
            SwapChainPanel->CreateCoreIndependentInputSourceMethod.AllowAnyCall(
                [=] (CoreInputDeviceTypes, ICoreInputSourceBase** value)
                {
                    auto inputSource = Make<StubCoreIndependentInputSource>(Dispatcher);
                    return inputSource.CopyTo(value);
                });

            Client.GameLoopStarting.AllowAnyCall();
            Client.GameLoopStopped.AllowAnyCall();
        }

        void CreateThread()
        {
            Thread = CreateGameLoopThread(SwapChainPanel.Get(), &Client);
        }

        void RunAndWait(std::function<void()> fn = nullptr)
        {
            auto handler = Callback<IDispatchedHandler>(
                [=]
                {
                    return ExceptionBoundary(
                        [&]
                        {
                            if (fn)
                                fn();
                        });
                });

            auto action = Thread->RunAsync(handler.Get());

            Waiter w;
            auto completed = Callback<IAsyncActionCompletedHandler>(
                [&] (IAsyncAction*, AsyncStatus)
                {
                    w.Set();
                    return S_OK;
                });

            ThrowIfFailed(action->put_Completed(completed.Get()));

            w.Wait();
        }

        void RunDirectlyOnDispatcherAndWait()
        {
            Waiter w;
            auto handler = Callback<IDispatchedHandler>(
                [&]
                {
                    w.Set();
                    return S_OK;
                });
            ComPtr<IAsyncAction> ignoredAction;
            ThrowIfFailed(Dispatcher->RunAsync(CoreDispatcherPriority_Normal, handler.Get(), &ignoredAction));
            w.Wait();
        }
    };

    TEST_METHOD_EX(GameLoopThread_ConstructionDestruction)
    {
        Fixture f;
        f.CreateThread();
    }

    TEST_METHOD_EX(GameLoopThread_HasThreadAccess_CallsThroughToDispatcher)
    {
        Fixture f;
        f.CreateThread();

        f.RunAndWait();         // give the thread a chance to start

        f.Dispatcher->get_HasThreadAccessMethod.SetExpectedCalls(1,
            [] (boolean* value)
            {
                *value = TRUE;
                return S_OK;
            });
        Assert::IsTrue(f.Thread->HasThreadAccess());

        f.Dispatcher->get_HasThreadAccessMethod.SetExpectedCalls(1,
            [] (boolean* value)
            {
                *value = FALSE;
                return S_OK;
            });        
        Assert::IsFalse(f.Thread->HasThreadAccess());

        f.Dispatcher->get_HasThreadAccessMethod.SetExpectedCalls(1,
            [] (boolean*)
            {
                return E_FAIL;
            });
        ExpectHResultException(E_FAIL, [&] { f.Thread->HasThreadAccess(); });
    }

    TEST_METHOD_EX(GameLoopThread_RunAsync_ExecutesHandler)
    {
        Fixture f;
        f.CreateThread();

        f.RunAndWait();
    }

    TEST_METHOD_EX(GameLoopThread_WhenStartDispatcherCalled_DispatcherStartsProcessingEvents)
    {
        Fixture f;
        f.CreateThread();

        f.Thread->StartDispatcher();
        f.RunDirectlyOnDispatcherAndWait();
    }

    TEST_METHOD_EX(GameLoopThread_WhenStopDispatcherCalled_DispatcherStopsProcessingEvents)
    {
        Fixture f;
        f.CreateThread();

        f.Thread->StartDispatcher();
        f.Thread->StopDispatcher();

        auto handler = Callback<IDispatchedHandler>(
            [&]
            {
                Assert::Fail(L"did not expect to see this");
                return S_OK;
            });
        ComPtr<IAsyncAction> ignoredAction;
        ThrowIfFailed(f.Dispatcher->RunAsync(CoreDispatcherPriority_Normal, handler.Get(), &ignoredAction));

        f.RunAndWait();
    }

    TEST_METHOD_EX(GameLoopThread_TicksAreScheduledOnDispatcherAtLowPriority)
    {
        // If the dispatcher is being shared by input then we want the input
        // events to take priority over the ticks.  We ensure this by scheduling
        // the ticks at low priority.
        Fixture f;
        f.CreateThread();

        f.Thread->StartDispatcher();
        f.RunDirectlyOnDispatcherAndWait();

        f.Dispatcher->RunAsyncValidation.SetExpectedCalls(1,
            [] (CoreDispatcherPriority priority)
            {
                Assert::AreEqual(CoreDispatcherPriority_Low, priority);
            });

        f.RunAndWait();
    }

    TEST_METHOD_EX(GameLoopThread_OnGameLoopStarting_IsCalledBeforeGameLoopStarts)
    {
        Fixture f;

        f.Client.GameLoopStarting.SetExpectedCalls(1);
        f.CreateThread();
        f.Client.GameLoopStarting.SetExpectedCalls(0);

        f.RunAndWait();
    }

    TEST_METHOD_EX(GameLoopThread_OnGameLoopStopped_IsCalledAfterGameLoopStops)
    {
        Fixture f;

        f.Client.GameLoopStopped.SetExpectedCalls(0);

        f.CreateThread();
        f.Thread->StartDispatcher();
        f.Thread->StopDispatcher();
        
        f.Client.GameLoopStopped.SetExpectedCalls(1);
        f.Thread.reset();
    }

    TEST_METHOD_EX(GameLoopThead_WhenCreateCoreIndependentInputSourceFails_ConstructorStillCompletes)
    {
        Fixture f;

        f.SwapChainPanel->CreateCoreIndependentInputSourceMethod.SetExpectedCalls(1,
                [=] (CoreInputDeviceTypes, ICoreInputSourceBase**)
                {
                    // This is what happens when
                    // CreateCoreIndependentInputSource is called from inside
                    // the designer.
                    return E_UNEXPECTED;
                });

        f.CreateThread();
        // If this test fails then CreateThread will never return.
    }
};
