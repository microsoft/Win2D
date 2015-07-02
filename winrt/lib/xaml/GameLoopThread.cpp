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

#include "pch.h"

#include "GameLoopThread.h"

using namespace ABI::Microsoft::Graphics::Canvas;

//
// Creates a CoreDispatcher for the current thread.
//
// We do a little dance here to create the dispatcher.  We first create a core
// independent input source (via the swapChainPanel), which will create a
// dispatcher.  We then discard the input source we created.
//
static ComPtr<ICoreDispatcher> CreateCoreDispatcher(ISwapChainPanel* swapChainPanel)
{
    ComPtr<ICoreInputSourceBase> inputSource;
    ThrowIfFailed(swapChainPanel->CreateCoreIndependentInputSource(
        CoreInputDeviceTypes_Touch | CoreInputDeviceTypes_Pen | CoreInputDeviceTypes_Mouse, 
        &inputSource));

    ComPtr<ICoreDispatcher> dispatcher;
    ThrowIfFailed(inputSource->get_Dispatcher(&dispatcher));

    ThrowIfFailed(swapChainPanel->CreateCoreIndependentInputSource(
        CoreInputDeviceTypes_None,
        &inputSource));

    return dispatcher;
}

//
// Creates a long-running thread using the threadpool API.
//
static ComPtr<IAsyncAction> StartThread(ComPtr<IWorkItemHandler>&& handler)
{
    ComPtr<IThreadPoolStatics> threadPoolStatics;
    ThrowIfFailed(GetActivationFactory(
        HStringReference(RuntimeClass_Windows_System_Threading_ThreadPool).Get(),
        &threadPoolStatics));

    ComPtr<IAsyncAction> action;
    ThrowIfFailed(threadPoolStatics->RunWithPriorityAndOptionsAsync(
        handler.Get(),
        WorkItemPriority_Normal,
        WorkItemOptions_TimeSliced,
        &action));
    
    return action;        
}

//
// The implementation of IGameLoopThread.
//
class GameLoopThread : public IGameLoopThread
                     , private LifespanTracker<GameLoopThread>
{
    ICanvasGameLoopClient* m_client;
    std::mutex m_mutex;
    std::condition_variable m_conditionVariable;

    ComPtr<ICoreDispatcher> m_dispatcher;
    bool m_started;
    bool m_startDispatcher;
    bool m_dispatcherStarted;
    bool m_shutdownRequested;
    bool m_shutdown;
    std::vector<ComPtr<AnimatedControlAsyncAction>> m_pendingActions;

public:
    GameLoopThread(ComPtr<ISwapChainPanel> swapChainPanel, ICanvasGameLoopClient* client)
        : m_client(client)
        , m_started(false)
        , m_startDispatcher(false)
        , m_dispatcherStarted(false)
        , m_shutdownRequested(false)
        , m_shutdown(false)
    {
        auto threadHandler = Callback<AddFtmBase<IWorkItemHandler>::Type>(
            [=] (IAsyncAction*)
            {
                return ExceptionBoundary([&] { ThreadMain(swapChainPanel); });
            });
        auto action = StartThread(std::move(threadHandler));

        auto threadCompletedHandler = Callback<AddFtmBase<IAsyncActionCompletedHandler>::Type>(
            [=] (IAsyncAction*, AsyncStatus)
            {
                return ExceptionBoundary([&] { ThreadCompleted(); });
            });

        ThrowIfFailed(action->put_Completed(threadCompletedHandler.Get()));

        auto lock = GetLock();
        m_conditionVariable.wait(lock, [=] { return m_started || m_shutdown; });
    }

    ~GameLoopThread()
    {
        auto lock = GetLock();
        
        if (m_dispatcher)
            As<ICoreDispatcherWithTaskPriority>(m_dispatcher)->StopProcessEvents();

        m_shutdownRequested = true;
        m_conditionVariable.notify_all();

        m_conditionVariable.wait(lock, [=] { return m_shutdown; });
    }

    virtual void StartDispatcher() override
    {
        auto lock = GetLock();
        m_startDispatcher = true;
        m_conditionVariable.notify_all();
    }

    virtual void StopDispatcher() override
    {
        auto lock = GetLock();

        m_startDispatcher = false;

        if (m_dispatcherStarted)
        {
            assert(m_dispatcher);
            ThrowIfFailed(As<ICoreDispatcherWithTaskPriority>(m_dispatcher)->StopProcessEvents());
        }

        m_conditionVariable.notify_all();
        m_conditionVariable.wait(lock, [=] { return !m_dispatcherStarted; });
    }

    virtual ComPtr<IAsyncAction> RunAsync(IDispatchedHandler* handler) override
    {
        auto lock = GetLock();

        if (m_startDispatcher)
        {
            ComPtr<IAsyncAction> action;
            ThrowIfFailed(m_dispatcher->RunAsync(CoreDispatcherPriority_Low, handler, &action));
            return action;
        }
        else
        {
            auto action = Make<AnimatedControlAsyncAction>(handler);
            m_pendingActions.push_back(action);
            m_conditionVariable.notify_all();
            return action;
        }
    }

    virtual bool HasThreadAccess() override
    {
        auto lock = GetLock();

        if (m_dispatcher)
        {
            boolean result;
            ThrowIfFailed(m_dispatcher->get_HasThreadAccess(&result));
            return !!result;
        }
        else
        {
            return false;
        }
    }

private:
    Lock GetLock()
    {
        return Lock(m_mutex);
    }

    void ThreadMain(ComPtr<ISwapChainPanel> swapChainPanel)
    {
        auto lock = GetLock();

        m_dispatcher = CreateCoreDispatcher(swapChainPanel.Get());
        swapChainPanel.Reset(); // we only needed this to create the dispatcher
        m_conditionVariable.notify_all();

        lock.unlock();
        m_client->OnGameLoopStarting();
        lock.lock();

        m_started = true;
        m_conditionVariable.notify_all();        

        for (;;)
        {
            m_conditionVariable.wait(lock, [=] { return m_shutdownRequested || !m_pendingActions.empty() || m_startDispatcher; });

            if (m_shutdownRequested)
                break;

            if (!m_pendingActions.empty())
            {
                std::vector<ComPtr<AnimatedControlAsyncAction>> actions;
                std::swap(actions, m_pendingActions);
                
                lock.unlock();
                RunActions(std::move(actions));
                lock.lock();
            }
            else if (m_startDispatcher)
            {
                m_dispatcherStarted = true;
                m_conditionVariable.notify_all();
                
                lock.unlock();
                ThrowIfFailed(m_dispatcher->ProcessEvents(CoreProcessEventsOption_ProcessUntilQuit));
                lock.lock();

                m_dispatcherStarted = false;
                m_conditionVariable.notify_all();
            }
        }

        // Cancel any remaining actions
        CancelActions(lock);

        lock.unlock();
        m_client->OnGameLoopStopped();

        // falling out of ThreadMain will cause ThreadCompleted to be called,
        // which will mark the thread as shutdown.
    }

    void ThreadCompleted()
    {
        // This method is called whenever the thread action completes (even if
        // it threw an exception).

        auto lock = GetLock();
        m_shutdown = true;
        m_conditionVariable.notify_all();        
        // as soon as the lock is released the destructor might run, so 'this'
        // is essentially invalid now.
    }

    void RunActions(std::vector<ComPtr<AnimatedControlAsyncAction>>&& actions)
    {
        for (auto& action : actions)
        {
            action->InvokeAndFireCompletion();
        }

        actions.clear();
    }

    void CancelActions(Lock const& lock)
    {
        MustOwnLock(lock);

        for (auto& action : m_pendingActions)
        {
            action->CancelAndFireCompletion();
        }

        m_pendingActions.clear();
    }
};


std::unique_ptr<IGameLoopThread> CreateGameLoopThread(ISwapChainPanel* swapChainPanel, ICanvasGameLoopClient* client)
{
    return std::make_unique<GameLoopThread>(swapChainPanel, client);
}
