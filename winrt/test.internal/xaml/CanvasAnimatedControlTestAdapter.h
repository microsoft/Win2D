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

#include "MockAsyncAction.h"
#include "BaseControlTestAdapter.h"
#include "CanvasSwapChainPanelTestAdapter.h"
#include "MockCoreIndependentInputSource.h"

class CanvasAnimatedControlTestAdapter : public BaseControlTestAdapter<CanvasAnimatedControlTraits>
{
    std::shared_ptr<CanvasSwapChainPanelTestAdapter> m_swapChainPanelAdapter;
    int64_t m_performanceCounter;
    ComPtr<StubSwapChainPanel> m_swapChainPanel;
    ComPtr<StubCoreIndependentInputSource> m_coreIndependentInputSource;

public:
    CALL_COUNTER_WITH_MOCK(CreateCanvasSwapChainMethod, ComPtr<CanvasSwapChain>(ICanvasDevice*, float, float, float, CanvasAlphaMode));
    std::shared_ptr<CanvasSwapChainManager> SwapChainManager;
    ComPtr<StubCanvasDevice> InitialDevice;

    CanvasAnimatedControlTestAdapter(StubCanvasDevice* initialDevice = nullptr)
        : m_performanceCounter(0)
        , SwapChainManager(std::make_shared<CanvasSwapChainManager>())
        , InitialDevice(initialDevice)
        , m_swapChainPanel(Make<StubSwapChainPanel>())
        , m_coreIndependentInputSource(Make<StubCoreIndependentInputSource>())
    {
        m_swapChainPanel->SetSwapChainMethod.AllowAnyCall(
            [=](IDXGISwapChain*)
            {
                return S_OK;
            });

        m_swapChainPanel->CreateCoreIndependentInputSourceMethod.AllowAnyCall(
            [=](CoreInputDeviceTypes, ICoreInputSourceBase** out)
            {
                return m_coreIndependentInputSource.CopyTo(out);
            });

        m_swapChainPanelAdapter = std::make_shared<CanvasSwapChainPanelTestAdapter>(m_swapChainPanel);
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

    std::function<void()> m_beforeLoopFn;
    std::function<bool()> m_currentTickFn;
    std::function<void()> m_afterLoopFn;
    ComPtr<MockAsyncAction> m_outstandingWorkItemAsyncAction;

    virtual ComPtr<IAsyncAction> StartUpdateRenderLoop(
        std::function<void()> const& beforeLoopFn,
        std::function<bool()> const& tickFn,
        std::function<void()> const& afterLoopFn)
    {
        Assert::IsFalse(static_cast<bool>(m_currentTickFn));
        Assert::IsFalse(m_outstandingWorkItemAsyncAction);

        m_outstandingWorkItemAsyncAction = Make<MockAsyncAction>();
        m_beforeLoopFn = beforeLoopFn;
        m_currentTickFn = tickFn;
        m_afterLoopFn = afterLoopFn;

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
        if (m_beforeLoopFn)
        {
            m_beforeLoopFn();
            m_beforeLoopFn = nullptr;
        }
        if (m_currentTickFn)
        {
            bool continueRunning;
            HRESULT hr = ExceptionBoundary([&] { continueRunning = m_currentTickFn(); });

            if (SUCCEEDED(hr) && continueRunning)
                return;

            if (m_afterLoopFn)
            {
                m_afterLoopFn();
                m_afterLoopFn = nullptr;
            }

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

    void SetCoreIndpendentInputSource(ComPtr<StubCoreIndependentInputSource> const& value)
    {
        m_coreIndependentInputSource = value;
    }

    ComPtr<StubCoreIndependentInputSource> GetCoreIndependentInputSource()
    {
        return m_coreIndependentInputSource;
    }
};
