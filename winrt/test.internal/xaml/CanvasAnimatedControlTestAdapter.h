// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#include <lib/xaml/CanvasGameLoop.h>

#include "BaseControlTestAdapter.h"
#include "CanvasSwapChainPanelTestAdapter.h"
#include "MockShape.h"
#include "StubDispatcher.h"

class FakeGameThread : public IGameLoopThread
{
    ComPtr<StubDispatcher> m_dispatcher;
    bool m_dispatcherActive;
    std::vector<ComPtr<AnimatedControlAsyncAction>> m_pendingActions;

public:
    FakeGameThread()
        : m_dispatcher(Make<StubDispatcher>())
        , m_dispatcherActive(false)
    {
    }

    void Tick()
    {
        for (auto& action : m_pendingActions)
        {
            action->InvokeAndFireCompletion();
        }

        m_pendingActions.clear();

        if (m_dispatcherActive)
        {
            m_dispatcher->TickAll();
        }
    }

    StubDispatcher* GetDispatcher()
    {
        return m_dispatcher.Get();
    }

    bool HasPendingActions()
    {
        return m_dispatcher->HasPendingActions();
    }

    virtual void StartDispatcher() override
    {
        m_dispatcherActive = true;
    }

    virtual void StopDispatcher() override
    {
        m_dispatcherActive = false;
    }

    virtual ComPtr<IAsyncAction> RunAsync(IDispatcherQueueHandler* handler) override
    {
        auto action = Make<AnimatedControlAsyncAction>(handler);

        if (m_dispatcherActive)
        {
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
            ThrowIfFailed(m_dispatcher->TryEnqueueWithPriority(DispatcherQueuePriority_Low, callback.Get(), &result));
        }
        else
        {
            m_pendingActions.push_back(action);
        }

        return action;
    }

    virtual bool HasThreadAccess() override
    {
        boolean result;
        ThrowIfFailed(m_dispatcher->get_HasThreadAccess(&result));
        return !!result;
    }
};

class CanvasAnimatedControlTestAdapter : public BaseControlTestAdapter<CanvasAnimatedControlTraits>
{
    std::shared_ptr<CanvasSwapChainPanelTestAdapter> m_swapChainPanelAdapter;
    int64_t m_performanceCounter;
    ComPtr<StubSwapChainPanel> m_swapChainPanel;
    std::weak_ptr<FakeGameThread> m_gameThread;
    ComPtr<MockShape> m_shape;

public:
    CALL_COUNTER_WITH_MOCK(CreateCanvasSwapChainMethod, ComPtr<CanvasSwapChain>(ICanvasDevice*, float, float, float, CanvasAlphaMode));
    ComPtr<StubCanvasDevice> InitialDevice;

    CanvasAnimatedControlTestAdapter(StubCanvasDevice* initialDevice = nullptr)
        : m_performanceCounter(1)
        , InitialDevice(initialDevice)
        , m_swapChainPanel(Make<StubSwapChainPanel>())
    {
        m_swapChainPanel->SetSwapChainMethod.AllowAnyCall(
            [=](IDXGISwapChain*)
            {
                return S_OK;
            });

        m_swapChainPanelAdapter = std::make_shared<CanvasSwapChainPanelTestAdapter>(m_swapChainPanel);
    }

    virtual ComPtr<CanvasSwapChain> CreateCanvasSwapChain(
        ICanvasDevice* device,
        float width,
        float height,
        float dpi,
        CanvasAlphaMode alphaMode) override
    {
        return CreateCanvasSwapChainMethod.WasCalled(device, width, height, dpi, alphaMode);
    }

    virtual ComPtr<CanvasSwapChainPanel> CreateCanvasSwapChainPanel() override
    {
        auto swapChainPanel = Make<CanvasSwapChainPanel>(m_swapChainPanelAdapter);

        return swapChainPanel;
    }

    virtual ComPtr<IShape> CreateDesignModeShape() override
    {
        m_shape = Make<MockShape>();
        return m_shape;
    }

    StubSwapChainPanel* GetSwapChainPanel()
    {
        return m_swapChainPanel.Get();
    }

    MockShape* GetShape()
    {
        return m_shape.Get();
    }

    StubDispatcher* GetGameThreadDispatcher()
    {
        if (auto thread = m_gameThread.lock())
            return thread->GetDispatcher();
        else
            return nullptr;
    }

    bool GameThreadHasPendingWork()
    {
        if (auto thread = m_gameThread.lock())
            return thread->HasPendingActions();
        else
            return false;
    }

    virtual std::unique_ptr<CanvasGameLoop> CreateAndStartGameLoop(CanvasAnimatedControl* control, ISwapChainPanel*) override
    {
        // CanvasGameLoop takes ownership of the IGameLoopThread we give it.
        // However, for our tests we also want to allow the adapter to hold on
        // to the thread.  So we store the underlying FakeGameThread in a
        // shared_ptr, and pass a unique_ptr to this proxy to CanvasGameLoop.
        class GameThreadProxy : public IGameLoopThread
        {
            std::shared_ptr<IGameLoopThread> m_thread;
            ICanvasGameLoopClient* m_client;

        public:
            GameThreadProxy(std::shared_ptr<IGameLoopThread> thread, ICanvasGameLoopClient* client)
                : m_thread(thread)
                , m_client(client)
            {
                m_client->OnGameLoopStarting();
            }

            ~GameThreadProxy()
            {
                m_client->OnGameLoopStopped();
            }

            virtual void StartDispatcher() { m_thread->StartDispatcher(); }
            virtual void StopDispatcher() { m_thread->StopDispatcher(); }
            virtual ComPtr<IAsyncAction> RunAsync(IDispatcherQueueHandler* handler) { return m_thread->RunAsync(handler); }
            virtual bool HasThreadAccess() { return m_thread->HasThreadAccess(); }
        };

        auto gameThread = std::make_shared<FakeGameThread>();

        auto gameLoop = std::make_unique<CanvasGameLoop>(control, std::make_unique<GameThreadProxy>(gameThread, control));

        m_gameThread = gameThread;
        
        return gameLoop;
    }

    void DoChanged()
    {
        TickUiThread();
    }

    void Tick()
    {
        if (auto thread = m_gameThread.lock())
            thread->Tick();
    }

    ComPtr<IInspectable> CreateSwapChainPanel(IInspectable* canvasSwapChainPanel)
    {
        return m_swapChainPanelAdapter->CreateSwapChainPanel(canvasSwapChainPanel);
    }

    std::function<void(DWORD)> m_sleepFn;
    virtual void Sleep(DWORD timeInMs) override
    {
        if (m_sleepFn) m_sleepFn(timeInMs);
    }

    void SetTime(int64_t time)
    {
        m_performanceCounter = time;
    }

    void ProgressTime(int64_t ticks)
    {
        m_performanceCounter += ticks;
    }

    virtual int64_t GetPerformanceCounter() override
    {
        return m_performanceCounter;
    }

    virtual int64_t GetPerformanceFrequency() override
    {
        return StepTimer::TicksPerSecond;
    }
};
