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

#include "..\MockAsyncAction.h"
#include "BaseControlTestAdapter.h"
#include "CanvasSwapChainPanelTestAdapter.h"
#include "MockDispatcher.h"

class CanvasAnimatedControlTestAdapter : public BaseControlTestAdapter < CanvasAnimatedControlTraits >
{
    std::shared_ptr<CanvasSwapChainPanelTestAdapter> m_swapChainPanelAdapter;
    int64_t m_performanceCounter;
    bool m_hasUIThreadAccess;
    ComPtr<StubSwapChainPanel> m_swapChainPanel;
    FLOAT m_compositionScaleX;
    FLOAT m_compositionScaleY;

public:
    CALL_COUNTER_WITH_MOCK(CreateCanvasSwapChainMethod, ComPtr<CanvasSwapChain>(ICanvasDevice*, float, float, float, CanvasAlphaMode));
    std::shared_ptr<CanvasSwapChainManager> SwapChainManager;
    ComPtr<StubCanvasDevice> InitialDevice;
    ComPtr<MockEventSource<CompositionScaleChangedEventHandler>> CompositionScaleChangedEventSource;


    CanvasAnimatedControlTestAdapter(StubCanvasDevice* initialDevice = nullptr)
        : m_performanceCounter(0)
        , SwapChainManager(std::make_shared<CanvasSwapChainManager>())
        , InitialDevice(initialDevice)
        , m_hasUIThreadAccess(true)
        , CompositionScaleChangedEventSource(Make<MockEventSource<CompositionScaleChangedEventHandler>>(L"CompositionScaleChanged"))
        , m_compositionScaleX(1.0f)
        , m_compositionScaleY(1.0f)
        , m_swapChainPanel(Make<StubSwapChainPanel>())
    {
        m_swapChainPanel->SetSwapChainMethod.AllowAnyCall(
            [=](IDXGISwapChain*)
            {
                return S_OK;
            });

        m_swapChainPanel->get_CompositionScaleXMethod.AllowAnyCall(
            [=](FLOAT* value)
            {
                *value = m_compositionScaleX;
                return S_OK;
            });

        m_swapChainPanel->get_CompositionScaleYMethod.AllowAnyCall(
            [=](FLOAT* value)
            {
                *value = m_compositionScaleY;
                return S_OK;
            });

        m_swapChainPanelAdapter = std::make_shared<CanvasSwapChainPanelTestAdapter>(m_swapChainPanel);
        
        GetCurrentMockWindow()->get_DispatcherMethod.AllowAnyCall(
            [=](ICoreDispatcher** out)
            {
                auto dispatcher = Make<MockDispatcher>();

                dispatcher->RunAsyncMethod.AllowAnyCall(
                    [=](CoreDispatcherPriority priority, IDispatchedHandler *agileCallback, IAsyncAction **asyncAction)
                    {
                        // This just launches the action right away, and doesn't copy out an async action object.
                        // Currently, product code doesn't use it anyway.

                        *asyncAction = nullptr;

                        ThrowIfFailed(agileCallback->Invoke());

                        return S_OK;
                    });

                dispatcher->get_HasThreadAccessMethod.AllowAnyCall(
                    [=](boolean* out)
                    {
                        *out = m_hasUIThreadAccess;

                        return S_OK;
                    });

                return dispatcher.CopyTo(out);
            });
    }

    ComPtr<CanvasSwapChain> CreateCanvasSwapChain(
        ICanvasDevice* device,
        float width,
        float height,
        float dpi,
        CanvasAlphaMode alphaMode) override
    {
        return CreateCanvasSwapChainMethod.WasCalled(device, width, height, dpi, alphaMode);
    }

    ComPtr<CanvasSwapChainPanel> CreateCanvasSwapChainPanel() override
    {
        auto swapChainPanel = Make<CanvasSwapChainPanel>(m_swapChainPanelAdapter);

        return swapChainPanel;
    }

    std::function<bool()> m_currentTickFn;
    ComPtr<MockAsyncAction> m_outstandingWorkItemAsyncAction;

    virtual ComPtr<IAsyncAction> StartUpdateRenderLoop(std::function<bool()> tickFn)
    {
        Assert::IsFalse(static_cast<bool>(m_currentTickFn));
        Assert::IsFalse(m_outstandingWorkItemAsyncAction);

        m_outstandingWorkItemAsyncAction = Make<MockAsyncAction>();
        m_currentTickFn = tickFn;

        return m_outstandingWorkItemAsyncAction;
    }


    std::function<void()> m_changedFn;
    ComPtr<MockAsyncAction> m_changedAsyncAction;
    virtual ComPtr<IAsyncAction> StartChangedAction(ComPtr<IWindow> const& window, std::function<void()> changedFn)
    {
        Assert::IsFalse(static_cast<bool>(m_changedFn));
        Assert::IsFalse(m_changedAsyncAction);

        m_changedAsyncAction = Make<MockAsyncAction>();
        m_changedFn = changedFn;

        return m_changedAsyncAction;
    }

    virtual RegisteredEvent AddCompositionScaleChangedCallback(
        ComPtr<ISwapChainPanel> const& swapChainPanel, 
        CompositionScaleChangedEventHandler* handler)
    {
        return CompositionScaleChangedEventSource->Add(handler);
    }

    void DoChanged()
    {
        if (m_changedFn)
        {
            m_changedFn();

            auto action = m_changedAsyncAction;
            m_changedFn = nullptr;
            m_changedAsyncAction.Reset();

            action->SetResult(S_OK);
        }
    }

    void Tick()
    {
        if (m_currentTickFn)
        {
            bool continueRunning;
            HRESULT hr = ExceptionBoundary([&] { continueRunning = m_currentTickFn(); });

            if (SUCCEEDED(hr) && continueRunning)
                return;

            auto action = m_outstandingWorkItemAsyncAction;
            m_currentTickFn = nullptr;
            m_outstandingWorkItemAsyncAction.Reset();

            action->SetResult(hr);
        }
    }

    std::pair<ComPtr<IInspectable>, ComPtr<ISwapChainPanel>> CreateSwapChainPanel(IInspectable* canvasSwapChainPanel)
    {
        return m_swapChainPanelAdapter->CreateSwapChainPanel(canvasSwapChainPanel);
    }

    void SetTime(int64_t time)
    {
        m_performanceCounter = time;
    }

    void ProgressTime(int64_t ticks)
    {
        m_performanceCounter += ticks;
    }

    virtual LARGE_INTEGER GetPerformanceCounter() override
    {
        LARGE_INTEGER l;
        l.QuadPart = m_performanceCounter;
        return l;
    }

    virtual LARGE_INTEGER GetPerformanceFrequency() override
    {
        LARGE_INTEGER l;
        l.QuadPart = StepTimer::TicksPerSecond;
        return l;
    }

    void SetHasUIThreadAccess(bool value)
    {
        m_hasUIThreadAccess = value;
    }

    void RaiseCompositionScaleChangedEvent()
    {
        ThrowIfFailed(CompositionScaleChangedEventSource->InvokeAll(m_swapChainPanel.Get(), nullptr));
    }

    void SetCompositionScaleAndRaiseEvent(FLOAT x, FLOAT y)
    {
        m_compositionScaleX = x;

        m_compositionScaleY = y;

        RaiseCompositionScaleChangedEvent();
    }
};
