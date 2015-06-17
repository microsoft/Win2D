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

#include <lib/xaml/CanvasGameLoop.h>

#include "BaseControlTestAdapter.h"
#include "CanvasSwapChainPanelTestAdapter.h"
#include "MockCoreIndependentInputSource.h"
#include "StubDispatcher.h"

class CanvasAnimatedControlTestAdapter : public BaseControlTestAdapter<CanvasAnimatedControlTraits>
{
    std::shared_ptr<CanvasSwapChainPanelTestAdapter> m_swapChainPanelAdapter;
    int64_t m_performanceCounter;
    ComPtr<MockAsyncAction> m_gameThreadAction;
    ComPtr<StubDispatcher> m_gameThreadDispatcher;
    ComPtr<StubSwapChainPanel> m_swapChainPanel;
    ComPtr<StubCoreIndependentInputSource> m_coreIndependentInputSource;

public:
    CALL_COUNTER_WITH_MOCK(CreateCanvasSwapChainMethod, ComPtr<CanvasSwapChain>(ICanvasDevice*, float, float, float, CanvasAlphaMode));
    std::shared_ptr<CanvasSwapChainManager> SwapChainManager;
    ComPtr<StubCanvasDevice> InitialDevice;

    CanvasAnimatedControlTestAdapter(StubCanvasDevice* initialDevice = nullptr)
        : m_performanceCounter(1)
        , SwapChainManager(std::make_shared<CanvasSwapChainManager>())
        , InitialDevice(initialDevice)
        , m_gameThreadDispatcher(Make<StubDispatcher>())
        , m_swapChainPanel(Make<StubSwapChainPanel>())
        , m_coreIndependentInputSource(Make<StubCoreIndependentInputSource>(m_gameThreadDispatcher.Get()))
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

    StubSwapChainPanel* GetSwapChainPanel()
    {
        return m_swapChainPanel.Get();
    }

    StubDispatcher* GetGameThreadDispatcher()
    {
        return m_gameThreadDispatcher.Get();
    }

    bool GameThreadHasPendingWork()
    {
        return m_gameThreadDispatcher->HasPendingActions();
    }

    virtual std::shared_ptr<CanvasGameLoop> CreateAndStartGameLoop(ComPtr<ISwapChainPanel> swapChainPanel) override
    {
        m_gameThreadAction = Make<MockAsyncAction>();

        m_gameThreadDispatcher->RunAsyncMethod.AllowAnyCall();

        // Simulate the real behavior where the game loop's action is completed
        // when the dispatcher stops processing events.
        m_gameThreadDispatcher->SetOnStop(
            [=] ()
            {
                m_gameThreadAction->SetResult(S_OK);
            });

        // Now create the game loop
        auto gameLoop = std::make_shared<CanvasGameLoop>(m_gameThreadAction, m_gameThreadDispatcher);

        // Allow any work the game loop constructor queued up to execute before
        // returning
        m_gameThreadDispatcher->TickAll();

        return gameLoop;
    }

    void DoChanged()
    {
        TickUiThread();
    }

    void Tick()
    {
        m_gameThreadDispatcher->Tick();
    }

    void TickAll()
    {
        m_gameThreadDispatcher->TickAll();
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

    ComPtr<StubCoreIndependentInputSource> GetCoreIndependentInputSource()
    {
        return m_coreIndependentInputSource;
    }
};
