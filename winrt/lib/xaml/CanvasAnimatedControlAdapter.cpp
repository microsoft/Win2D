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

#include "CanvasAnimatedControl.h"
#include "CanvasGameLoop.h"

using namespace ::ABI::Microsoft::Graphics::Canvas;
using namespace ::ABI::Microsoft::Graphics::Canvas::UI;
using namespace ::ABI::Microsoft::Graphics::Canvas::UI::Xaml;
using namespace ::Microsoft::WRL::Wrappers;

//
// Helper for CreateAndStartGameLoop, tracks state shared between the UI thread
// and the newly created game loop thread.  The game loop thread needs to
// communicate back the ICoreDispatcher.
//
class GameLoopStartup
{
    // This mutex & condition variable are used to block the incoming
    // thread while waiting for the game loop thread to start up and set
    // the dispatcher.
    std::mutex m_mutex;
    std::condition_variable m_conditionVariable;
            
    ComPtr<ICoreDispatcher> m_dispatcher;

public:
    void SetDispatcher(ICoreDispatcher* dispatcher)
    {
        Lock lock(m_mutex);
        m_dispatcher = dispatcher;
        lock.unlock();
        m_conditionVariable.notify_one();
    }

    void NotifyOne()
    {
        m_conditionVariable.notify_one();
    }

    ComPtr<ICoreDispatcher> WaitAndGetDispatcher(IAsyncAction* action)
    {
        auto info = As<IAsyncInfo>(action);

        Lock lock(m_mutex);
        m_conditionVariable.wait(lock,
            [&]
            {
                if (m_dispatcher)
                {
                    // The dispatcher has been sent to us; we're done
                    return true;
                }
                        
                AsyncStatus status;
                ThrowIfFailed(info->get_Status(&status));

                switch (status)
                {
                case AsyncStatus::Started:
                    // We're still waiting to find out
                    return false;

                case AsyncStatus::Error:
                {
                    HRESULT hr = S_OK;
                    ThrowIfFailed(info->get_ErrorCode(&hr));
                    ThrowHR(hr);
                }

                case AsyncStatus::Completed:
                    return true;

                default:
                    ThrowHR(E_UNEXPECTED);
                }
            });

        // We should only get here if the dispatcher was actually set.
        // All other cases should have resulted in an exception.
        assert(m_dispatcher);
        return m_dispatcher;
    }
};

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
// CanvasAnimatedControlAdapter
//

class CanvasAnimatedControlAdapter : public BaseControlAdapter<CanvasAnimatedControlTraits>,
    public std::enable_shared_from_this<CanvasAnimatedControlAdapter>
{
    ComPtr<IThreadPoolStatics> m_threadPoolStatics;
    ComPtr<ICanvasSwapChainFactory> m_canvasSwapChainFactory;
    std::shared_ptr<CanvasSwapChainPanelAdapter> m_canvasSwapChainPanelAdapter;
    ComPtr<IActivationFactory> m_canvasSwapChainPanelActivationFactory;

public:
    CanvasAnimatedControlAdapter(IThreadPoolStatics* threadPoolStatics)
        : m_threadPoolStatics(threadPoolStatics)        
        , m_canvasSwapChainPanelAdapter(std::make_shared<CanvasSwapChainPanelAdapter>())        
    {
        auto& module = Module<InProc>::GetModule();

        ComPtr<IActivationFactory> swapChainActivationFactory;

        ThrowIfFailed(module.GetActivationFactory(
            HStringReference(RuntimeClass_Microsoft_Graphics_Canvas_CanvasSwapChain).Get(),
            &swapChainActivationFactory));

        m_canvasSwapChainFactory = As<ICanvasSwapChainFactory>(swapChainActivationFactory);

        ThrowIfFailed(module.GetActivationFactory(
            HStringReference(RuntimeClass_Microsoft_Graphics_Canvas_UI_Xaml_CanvasSwapChainPanel).Get(),
            &m_canvasSwapChainPanelActivationFactory));
    }        

    virtual ComPtr<CanvasSwapChainPanel> CreateCanvasSwapChainPanel() override
    {
        ComPtr<IInspectable> canvasSwapChainPanelInspectable;
        ThrowIfFailed(m_canvasSwapChainPanelActivationFactory->ActivateInstance(&canvasSwapChainPanelInspectable));

        auto canvasSwapChainPanel = As<ICanvasSwapChainPanel>(canvasSwapChainPanelInspectable);

        return static_cast<CanvasSwapChainPanel*>(canvasSwapChainPanel.Get());
    }

    virtual ComPtr<CanvasSwapChain> CreateCanvasSwapChain(
        ICanvasDevice* device,
        float width,
        float height,
        float dpi,
        CanvasAlphaMode alphaMode) override
    {
        ComPtr<ICanvasSwapChain> swapChain;

        ThrowIfFailed(m_canvasSwapChainFactory->CreateWithAllOptions(
            As<ICanvasResourceCreator>(device).Get(),
            width, 
            height, 
            dpi,
            PIXEL_FORMAT(B8G8R8A8UIntNormalized),
            2, 
            alphaMode,
            &swapChain));

        return static_cast<CanvasSwapChain*>(swapChain.Get());
    }

    virtual std::shared_ptr<CanvasGameLoop> CreateAndStartGameLoop(ComPtr<ISwapChainPanel> swapChainPanel, ComPtr<AnimatedControlInput> input) override
    {
        //
        // This needs to start a new thread and, while executing code on that
        // thread, then get a CoreDispatcher set up on that thread, and then, on
        // the original thread, create a CanvasGameLoop that has access to that
        // dispatcher.
        //

        auto gameLoopStartup = std::make_shared<GameLoopStartup>();

        auto startThreadHandler = Callback<AddFtmBase<IWorkItemHandler>::Type>(
            [=](IAsyncAction*) mutable
            {
                return ExceptionBoundary(
                    [&]
                    {
                        auto notifyWarden = MakeScopeWarden([&] { gameLoopStartup->NotifyOne(); });

                        auto dispatcher = CreateCoreDispatcher(swapChainPanel.Get());

                        // Now notify the original thread that we've got the
                        // dispatcher
                        gameLoopStartup->SetDispatcher(dispatcher.Get());
                        notifyWarden.Dismiss();
                        gameLoopStartup.reset();

                        // At this point the original thread is likely to go
                        // about its business - so we cannot access anything we
                        // captured by reference from it.
                        //
                        // Which is ok, because all we want to do here is to
                        // start processing events.

                        ThrowIfFailed(dispatcher->ProcessEvents(CoreProcessEventsOption_ProcessUntilQuit));
                    });
            });
        CheckMakeResult(startThreadHandler);

        auto action = StartThread(std::move(startThreadHandler));
        auto dispatcher = gameLoopStartup->WaitAndGetDispatcher(action.Get());

        return std::make_shared<CanvasGameLoop>(std::move(action), std::move(dispatcher), input);
    }

    virtual ComPtr<IInspectable> CreateSwapChainPanel(IInspectable* canvasSwapChainPanel) override
    {
        return m_canvasSwapChainPanelAdapter->CreateSwapChainPanel(canvasSwapChainPanel);
    }

    virtual void Sleep(DWORD timeInMs) override
    {
        ::Sleep(timeInMs);
    }

    virtual LARGE_INTEGER GetPerformanceCounter() override
    {
        LARGE_INTEGER counter;
        if (QueryPerformanceCounter(&counter) == 0)
        {
            ThrowHR(E_FAIL);
        }
        return counter;
    }

    virtual LARGE_INTEGER GetPerformanceFrequency() override
    {
        LARGE_INTEGER frequency;
        if (QueryPerformanceFrequency(&frequency) == 0)
        {
            ThrowHR(E_FAIL);
        }
        return frequency;
    }

private:
    ComPtr<IAsyncAction> StartThread(ComPtr<IWorkItemHandler>&& handler)
    {
        ComPtr<IAsyncAction> action;
        ThrowIfFailed(m_threadPoolStatics->RunWithPriorityAndOptionsAsync(
            handler.Get(),
            WorkItemPriority_Normal,
            WorkItemOptions_TimeSliced,
            &action));

        return action;        
    }
};


namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace UI { namespace Xaml
{

    std::shared_ptr<ICanvasAnimatedControlAdapter> CreateCanvasAnimatedControlAdapter()
    {
        ComPtr<IThreadPoolStatics> threadPoolStatics;
        ThrowIfFailed(GetActivationFactory(
            HStringReference(RuntimeClass_Windows_System_Threading_ThreadPool).Get(),
            &threadPoolStatics));

        return std::make_shared<CanvasAnimatedControlAdapter>(threadPoolStatics.Get());
    }

} } } } } }