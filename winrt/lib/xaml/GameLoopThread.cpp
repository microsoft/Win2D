// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

#include "GameLoopThread.h"

using namespace ABI::Microsoft::Graphics::Canvas;
//
// Creates a DispatcherQueueController for the current thread.
//
static ComPtr<IDispatcherQueueController> CreateDispatcherQueueController()
{
    ComPtr<IDispatcherQueueControllerStatics> dispatcherQueueControllerStatics;
    ThrowIfFailed(GetActivationFactory(
        HStringReference(RuntimeClass_Microsoft_UI_Dispatching_DispatcherQueueController).Get(),
        &dispatcherQueueControllerStatics));

    ComPtr<IDispatcherQueueController> dispatcherQueueController;
    ThrowIfFailed(dispatcherQueueControllerStatics->CreateOnCurrentThread(&dispatcherQueueController));

    return dispatcherQueueController;
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
// As per https://msdn.microsoft.com/en-us/library/xcb2z8hs.aspx
//
static void SetThreadName(LPCSTR szThreadName)
{
    #pragma pack(push, 8)
    struct THREADNAME_INFO
    {
        DWORD dwType;
        LPCSTR szName;
        DWORD dwThreadID;
        DWORD dwFlags;
    };
    #pragma pack(pop)

    THREADNAME_INFO info{ 0x1000, szThreadName, 0xFFFFFFFF, 0 };

    __try
    {
        RaiseException(0x406D1388, 0, sizeof(info) / sizeof(ULONG_PTR), reinterpret_cast<ULONG_PTR*>(&info));
    }
    __except(EXCEPTION_CONTINUE_EXECUTION) { }
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

    ComPtr<IDispatcherQueueController> m_dispatcherQueueController;
    ComPtr<IDispatcherQueue> m_dispatcherQueue;
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
        
        if (m_dispatcherQueue)
        {
            ThrowIfFailed(As<IDispatcherQueue3>(m_dispatcherQueue)->EnqueueEventLoopExit());
        }

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
            assert(m_dispatcherQueue);
            ThrowIfFailed(As<IDispatcherQueue3>(m_dispatcherQueue)->EnqueueEventLoopExit());
        }

        m_conditionVariable.notify_all();
        m_conditionVariable.wait(lock, [=] { return !m_dispatcherStarted; });
    }

    virtual ComPtr<IAsyncAction> RunAsync(IDispatcherQueueHandler* handler) override
    {
        auto lock = GetLock();

        auto action = Make<AnimatedControlAsyncAction>(handler);

        if (m_startDispatcher)
        {
            // RunAsync needs to return an IAsyncAction that fires a Completed event.
            // For example, CanvasGameLoop's Tick uses each Completed event to fire the next Tick.
            // DispatcherQueue, unlike UWP's CoreDispatcher, does not return an IAsyncAction, so we
            // wrap the passed in IDispatcherQueueHandler in our own custom AnimatedControlAsyncAction.
            // Then we wrap that AnimatedControlAsyncAction in a new IDispatcherQueueHandler to give
            // to the DispatcherQueue.
            auto callback = Callback<AddFtmBase<IDispatcherQueueHandler>::Type>(
                [action]() {
                    auto result = action->InvokeAndFireCompletion();

                    if (SUCCEEDED(result.ActionResult))
                    {
                        return result.ActionResult;
                    }
                    else
                    {
                        return result.CompletedHandlerResult;
                    }
                });

            boolean result;
            HRESULT hr = m_dispatcherQueue->TryEnqueueWithPriority(DispatcherQueuePriority_Low, callback.Get(), &result);

            ThrowIfFailed(hr);
        }
        else
        {
            m_pendingActions.push_back(action);
            m_conditionVariable.notify_all();
        }

        return action;
    }

    virtual bool HasThreadAccess() override
    {
        auto lock = GetLock();

        if (m_dispatcherQueue)
        {
            boolean result;
            ThrowIfFailed(As<IDispatcherQueue2>(m_dispatcherQueue)->get_HasThreadAccess(&result));

            
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
        SetThreadName(Strings::GameLoopThreadName);

        auto lock = GetLock();

        m_dispatcherQueueController = CreateDispatcherQueueController();
        ThrowIfFailed(m_dispatcherQueueController->get_DispatcherQueue(&m_dispatcherQueue));

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
                ThrowIfFailed(As<IDispatcherQueue3>(m_dispatcherQueue)->RunEventLoop());
                ComPtr<IAsyncAction> action;
                m_dispatcherQueueController->ShutdownQueueAsync(&action);
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
