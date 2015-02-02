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
#include "BaseControlAdapter.h"
#include "CanvasSwapChain.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas
{
    using namespace ::Microsoft::WRL::Wrappers;

    IFACEMETHODIMP CanvasAnimatedUpdateEventArgsFactory::Create(
        CanvasTimingInformation timingInformation,
        ICanvasAnimatedUpdateEventArgs** updateEventArgs)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckAndClearOutPointer(updateEventArgs);

                auto newCanvasUpdateEventArgs = Make<CanvasAnimatedUpdateEventArgs>(timingInformation);
                CheckMakeResult(newCanvasUpdateEventArgs);

                ThrowIfFailed(newCanvasUpdateEventArgs.CopyTo(updateEventArgs));
            });
    }

    CanvasAnimatedUpdateEventArgs::CanvasAnimatedUpdateEventArgs(CanvasTimingInformation timing)
        : m_timingInformation(timing)
    {
    }

    IFACEMETHODIMP CanvasAnimatedUpdateEventArgs::get_Timing(CanvasTimingInformation* value)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(value);

                *value = m_timingInformation;
            });
    }

    IFACEMETHODIMP CanvasAnimatedDrawEventArgsFactory::Create(
        ICanvasDrawingSession* drawingSession,
        CanvasTimingInformation timingInformation,
        ICanvasAnimatedDrawEventArgs** drawEventArgs)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckAndClearOutPointer(drawEventArgs);

                auto newCanvasDrawEventArgs = Make<CanvasAnimatedDrawEventArgs>(drawingSession, timingInformation);
                CheckMakeResult(newCanvasDrawEventArgs);

                ThrowIfFailed(newCanvasDrawEventArgs.CopyTo(drawEventArgs));
            });
    }

    CanvasAnimatedDrawEventArgs::CanvasAnimatedDrawEventArgs(
        ICanvasDrawingSession* drawingSession,
        CanvasTimingInformation timingInformation)
        : m_drawingSession(drawingSession)
        , m_timingInformation(timingInformation)
    {
    }

    IFACEMETHODIMP CanvasAnimatedDrawEventArgs::get_DrawingSession(ICanvasDrawingSession** value)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckAndClearOutPointer(value);

                auto drawingSession = m_drawingSession.EnsureNotClosed();

                ThrowIfFailed(drawingSession.CopyTo(value));
            });
    }

    IFACEMETHODIMP CanvasAnimatedDrawEventArgs::get_Timing(CanvasTimingInformation* value)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(value);

                *value = m_timingInformation;
            });
    }


    class CanvasAnimatedControlAdapter : public BaseControlAdapter<CanvasAnimatedControlTraits>,
        public std::enable_shared_from_this<CanvasAnimatedControlAdapter>
    {
        ComPtr<IThreadPoolStatics> m_threadPoolStatics;
        ComPtr<ICanvasSwapChainFactory> m_canvasSwapChainFactory;
        std::shared_ptr<CanvasSwapChainPanelAdapter> m_canvasSwapChainPanelAdapter;
        ComPtr<IActivationFactory> m_canvasSwapChainPanelActivationFactory;

    public:
        CanvasAnimatedControlAdapter(
            IThreadPoolStatics* threadPoolStatics)
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
                HStringReference(RuntimeClass_Microsoft_Graphics_Canvas_CanvasSwapChainPanel).Get(),
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

            ThrowIfFailed(m_canvasSwapChainFactory->CreateWithAllOptionsAndDpi(
                As<ICanvasResourceCreator>(device).Get(),
                width, 
                height, 
                PIXEL_FORMAT(B8G8R8A8UIntNormalized),
                2, 
                alphaMode,
                dpi,
                &swapChain));

            return static_cast<CanvasSwapChain*>(swapChain.Get());
        }

        virtual ComPtr<IAsyncAction> StartUpdateRenderLoop(
            std::function<void()> const& beforeLoopFn,
            std::function<bool()> const& tickFn,
            std::function<void()> const& afterLoopFn)
        {
            auto self = shared_from_this();

            auto handler = Callback<AddFtmBase<IWorkItemHandler>::Type>(
                [self, beforeLoopFn, tickFn, afterLoopFn](IAsyncAction* action)
                {
                    return ExceptionBoundary(
                        [&]
                        {
                            self->UpdateRenderLoop(action, beforeLoopFn, tickFn, afterLoopFn);
                        });
                });

            ComPtr<IAsyncAction> action;
            ThrowIfFailed(m_threadPoolStatics->RunWithPriorityAndOptionsAsync(
                handler.Get(),
                WorkItemPriority_Normal,
                WorkItemOptions_TimeSliced,
                &action));

            return action;
        }

        virtual ComPtr<IAsyncAction> StartChangedAction(ComPtr<IWindow> const& window, std::function<void()> changedFn)
        {
            if (IsDesignModeEnabled())
            {
                return nullptr;
            }

            ComPtr<ICoreDispatcher> dispatcher;
            ThrowIfFailed(window->get_Dispatcher(&dispatcher));

            auto callback = Callback<AddFtmBase<IDispatchedHandler>::Type>(
                [=]()->HRESULT
                {
                    return ExceptionBoundary(
                        [&]
                        {
                            changedFn();
                        });
                });

            ComPtr<IAsyncAction> asyncAction;
            ThrowIfFailed(dispatcher->RunAsync(CoreDispatcherPriority_Normal, callback.Get(), &asyncAction));

            return asyncAction;
        }

        void UpdateRenderLoop(
            IAsyncAction* action,
            std::function<void()> const& beforeLoopFn,
            std::function<bool()> const& tickFn,
            std::function<void()> const& afterLoopFn)
        {
            auto asyncInfo = As<IAsyncInfo>(action);

            beforeLoopFn();

            auto afterLoopWarden = MakeScopeWarden([&] { afterLoopFn(); });

            for (;;)
            {
                AsyncStatus status;
                ThrowIfFailed(asyncInfo->get_Status(&status));

                if (status != AsyncStatus::Started)
                    break;

                if (!tickFn())
                    break;
            };
            
        }

        virtual std::pair<ComPtr<IInspectable>, ComPtr<ISwapChainPanel>> CreateSwapChainPanel(IInspectable* canvasSwapChainPanel) override
        {
            return m_canvasSwapChainPanelAdapter->CreateSwapChainPanel(canvasSwapChainPanel);
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
    };

    std::shared_ptr<ICanvasAnimatedControlAdapter> CreateCanvasAnimatedControlAdapter(
        IThreadPoolStatics* threadPoolStatics)
    {
        return std::make_shared<CanvasAnimatedControlAdapter>(threadPoolStatics);
    }

    static std::shared_ptr<ICanvasAnimatedControlAdapter> CreateCanvasAnimatedControlAdapter()
    {
        ComPtr<IThreadPoolStatics> threadPoolStatics;
        ThrowIfFailed(GetActivationFactory(
            HStringReference(RuntimeClass_Windows_System_Threading_ThreadPool).Get(),
            &threadPoolStatics));

        return CreateCanvasAnimatedControlAdapter(threadPoolStatics.Get());
    }

    class CanvasAnimatedControlFactory : public ActivationFactory<>
    {
        std::shared_ptr<ICanvasAnimatedControlAdapter> m_adapter;

    public:
        CanvasAnimatedControlFactory()
            : m_adapter(CreateCanvasAnimatedControlAdapter())
        {
        }

        IFACEMETHODIMP ActivateInstance(IInspectable** obj)
        {
            return ExceptionBoundary(
                [&]
                {
                    CheckAndClearOutPointer(obj);

                    auto control = Make<CanvasAnimatedControl>(m_adapter);
                    CheckMakeResult(control);

                    ThrowIfFailed(control.CopyTo(obj));
                });
        }
    };

    CanvasAnimatedControl::CanvasAnimatedControl(std::shared_ptr<ICanvasAnimatedControlAdapter> adapter)
        : BaseControl<CanvasAnimatedControlTraits>(adapter)
        , m_isPaused(false)
        , m_stepTimer(adapter)
        , m_forceUpdate(false)
        , m_needToRestartRenderThread(false)
        , m_shouldResetElapsedTime(false)
    {
        CreateSwapChainPanel();

        auto swapChainPanel = As<ISwapChainPanel>(m_canvasSwapChainPanel);

        m_input = Make<AnimatedControlInput>(swapChainPanel);

        m_isStepTimerFixedStep = m_stepTimer.IsFixedTimeStep();

        m_targetElapsedTime = m_stepTimer.GetTargetElapsedTicks();
    }

    CanvasAnimatedControl::~CanvasAnimatedControl()
    {
        assert(!m_renderLoopAction);

        assert(!m_changedAction);


    }

    //
    // ICanvasControl
    //

    IFACEMETHODIMP CanvasAnimatedControl::add_Update(
        Animated_UpdateEventHandler* value,
        EventRegistrationToken *token)
    {
        return ExceptionBoundary(
            [&]
            {
                ThrowIfFailed(m_updateEventList.Add(value, token));
            });
    }

    IFACEMETHODIMP CanvasAnimatedControl::remove_Update(
        EventRegistrationToken token)
    {
        return ExceptionBoundary(
            [&]
            {
                ThrowIfFailed(m_updateEventList.Remove(token));
            });
    }

    IFACEMETHODIMP CanvasAnimatedControl::put_IsFixedTimeStep(boolean value)
    {
        return ExceptionBoundary(
            [&]
            {
                auto lock = GetLock();

                m_isStepTimerFixedStep = !!value;
            });
    }

    IFACEMETHODIMP CanvasAnimatedControl::get_IsFixedTimeStep(boolean* value)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(value);

                auto lock = GetLock();

                *value = m_isStepTimerFixedStep; 
            });
    }

    IFACEMETHODIMP CanvasAnimatedControl::put_TargetElapsedTime(TimeSpan value)
    {
        return ExceptionBoundary(
            [&]
            {
                auto lock = GetLock();
                
                if (value.Duration <= 0)
                {
                    ThrowHR(E_INVALIDARG, HStringReference(Strings::ExpectedPositiveNonzero).Get());
                }

                m_targetElapsedTime = value.Duration;
            });
    }

    IFACEMETHODIMP CanvasAnimatedControl::get_TargetElapsedTime(TimeSpan* value)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(value);

                auto lock = GetLock();

                assert(m_targetElapsedTime <= INT64_MAX);

                TimeSpan timeSpan = {};
                timeSpan.Duration = static_cast<INT64>(m_targetElapsedTime);
                *value = timeSpan;
            });
    }

    IFACEMETHODIMP CanvasAnimatedControl::put_Paused(boolean value)
    {
        return ExceptionBoundary(
            [&]
            {
                auto lock = GetLock();

                m_isPaused = !!value;

                bool paused = m_isPaused;

                lock.unlock();

                if (!paused)
                    Changed();
            });
    }

    IFACEMETHODIMP CanvasAnimatedControl::get_Paused(boolean* value)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(value);

                auto lock = GetLock();

                *value = m_isPaused;
            });
    }

    IFACEMETHODIMP CanvasAnimatedControl::get_Size(Size* value)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(value);

                auto lock = GetLock();

                *value = m_currentSize;
            });
    }

    IFACEMETHODIMP CanvasAnimatedControl::ResetElapsedTime()
    {
        return ExceptionBoundary(
            [&]
            {
                auto lock = GetLock();

                m_shouldResetElapsedTime = true;
            });
    }


    IFACEMETHODIMP CanvasAnimatedControl::get_Input(ICorePointerInputSource** value)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckAndClearOutPointer(value);

                ThrowIfFailed(m_input.CopyTo(value));
            });
    }

    void CanvasAnimatedControl::CreateOrUpdateRenderTarget(
        ICanvasDevice* device,
        CanvasBackground newBackgroundMode,
        float newDpi,
        Size newSize,
        RenderTarget* renderTarget)
    {
        bool needsTarget = (renderTarget->Target == nullptr);
        bool backgroundModeChanged = (renderTarget->BackgroundMode != newBackgroundMode);
        bool dpiChanged = (renderTarget->Dpi != newDpi);
        bool sizeChanged = (renderTarget->Size != newSize);
        bool needsCreate = needsTarget || backgroundModeChanged || dpiChanged;

        if (!needsCreate && !sizeChanged)
            return;

        if (newSize.Width <= 0 || newSize.Height <= 0)
        {
            // Zero-sized controls don't have swap chain objects
            *renderTarget = RenderTarget{};
            ThrowIfFailed(m_canvasSwapChainPanel->put_SwapChain(nullptr));
        }
        else if (sizeChanged && !needsCreate)
        {
            ThrowIfFailed(renderTarget->Target->ResizeBuffersWithSize(newSize.Width, newSize.Height));

            renderTarget->Size = newSize;
        }
        else
        {
            CanvasAlphaMode alphaMode = CanvasAlphaMode::Premultiplied;

            if (newBackgroundMode == CanvasBackground::Opaque)
            {
                alphaMode = CanvasAlphaMode::Ignore;
            }

            renderTarget->Target = GetAdapter()->CreateCanvasSwapChain(
                device,
                newSize.Width,
                newSize.Height,
                newDpi,
                alphaMode);
                
            renderTarget->BackgroundMode = newBackgroundMode;
            renderTarget->Dpi = newDpi;
            renderTarget->Size = newSize;
                
            ThrowIfFailed(m_canvasSwapChainPanel->put_SwapChain(renderTarget->Target.Get()));            
        }
    }

    ComPtr<CanvasAnimatedDrawEventArgs> CanvasAnimatedControl::CreateDrawEventArgs(ICanvasDrawingSession* drawingSession)
    {
        auto timing = GetTimingInformationFromTimer();

        auto drawEventArgs = Make<CanvasAnimatedDrawEventArgs>(drawingSession, timing);
        CheckMakeResult(drawEventArgs);
        return drawEventArgs;
    }

    void CanvasAnimatedControl::ChangedClearColor(bool differentAlphaMode)
    {
        if (differentAlphaMode)
        {
            {
                auto lock = GetLock();
                m_needToRestartRenderThread = true;
            }

            Changed();
        }
    }

    void CanvasAnimatedControl::ChangedSize()
    {
        {
            auto lock = GetLock();
            m_needToRestartRenderThread = true;
        }

        Changed();
    }

    void CanvasAnimatedControl::Unloaded()
    {
        //
        // ChangedImpl is always executed on the UI thread, and
        // we expect the unloaded event on the UI thread, so 
        // m_renderLoopAction does not need locking.
        //
        if (m_renderLoopAction)
        {
            auto asyncInfo = As<IAsyncInfo>(m_renderLoopAction);

            ThrowIfFailed(asyncInfo->Cancel());
        }
        
        //
        // Changed (as opposed to ChangedImpl) touches m_changedAction
        // and may be called from any thread, and so there is a lock here.
        //
        auto lock = GetLock();

        if (m_changedAction)
        {
            auto asyncInfo = As<IAsyncInfo>(m_changedAction);

            ThrowIfFailed(asyncInfo->Cancel());
        }
    }

    void CanvasAnimatedControl::Changed()
    {
        //
        // This may be called from any thread.
        //

        if (!IsLoaded())
            return;

        auto lock = GetLock();

        if (m_changedAction)
            return;

        ComPtr<CanvasAnimatedControl> self = this;

        auto window = GetWindow();

        m_changedAction = GetAdapter()->StartChangedAction(window,
            [self]
            {
                self->ChangedImpl();
            });

        if (!m_changedAction)
            return;

        auto completed = Callback<IAsyncActionCompletedHandler>(
            [self](IAsyncAction*, AsyncStatus)
            {
                auto lock = self->GetLock();

                self->m_changedAction.Reset();

                return S_OK;
            });

        CheckMakeResult(completed);

        ThrowIfFailed(m_changedAction->put_Completed(completed.Get()));
    }

    void CanvasAnimatedControl::ChangedImpl()
    {
        //
        // This is called when something has happened that should force the
        // control to redraw itself.  For example, DPI has changed, device has
        // been lost, resources have been loaded.
        //
        // In this case we want to force an Update() to happen, even if it
        // wouldn't normally, so that we can draw something.
        //
        // This method, as an action, is always run on the UI thread.
        //

        m_currentSize = GetActualSize();

        // Changed() already verified whether the control is loaded.
        assert(IsLoaded());

        {
            auto lock = GetLock();

            m_forceUpdate = true;
        }

        if (m_renderLoopAction)
        {
            // 
            // If the render thread is already running, this check prevents
            // it from being launched again. Anything needing to stop the
            // render thread is co-ordinated through m_needToRestartRenderThread.
            //
            return;
        }

        RunWithRenderTarget(GetActualSize(),
            [&] (CanvasSwapChain* rawTarget, ICanvasDevice*, Color const& clearColor, bool areResourcesCreated)
            {
                // 
                // The early exit above, with the design of AnimatedControl, makes
                // it impossible to ever have two worker threads at the same time.
                //
                assert(!m_renderLoopAction);

                ComPtr<CanvasSwapChain> target(rawTarget);
                m_stepTimer.ResetElapsedTime();

                ComPtr<CanvasAnimatedControl> self = this;
                
                auto beforeLoopFn = 
                    [self]()
                    {
                        self->m_input->SetSource();
                    };

                auto onEachTickFn = 
                    [target, clearColor, areResourcesCreated, self]
                    {
                        return self->Tick(target.Get(), clearColor, areResourcesCreated);
                    };

                auto afterLoopFn = 
                    [self]()
                    {
                        self->m_input->RemoveSource();
                    };

                m_renderLoopAction = GetAdapter()->StartUpdateRenderLoop(beforeLoopFn, onEachTickFn, afterLoopFn);
                
                auto completed = Callback<IAsyncActionCompletedHandler>(
                    [self](IAsyncAction* asyncAction, AsyncStatus asyncStatus)
                    {
                        return self->OnRenderLoopCompleted(asyncAction, asyncStatus);
                    });

                CheckMakeResult(completed);

                ThrowIfFailed(m_renderLoopAction->put_Completed(completed.Get()));
            });
    }

    HRESULT CanvasAnimatedControl::OnRenderLoopCompleted(IAsyncAction* asyncAction, AsyncStatus asyncStatus)
    {
        m_renderLoopAction.Reset();

        if (asyncStatus == AsyncStatus::Error)
        {
            auto asyncInfo = As<IAsyncInfo>(asyncAction);

            HRESULT errorCode;
            ThrowIfFailed(asyncInfo->get_ErrorCode(&errorCode));
            
            return errorCode;
        }
        
        auto lock = GetLock();

        bool needsRenderThreadRestart = m_needToRestartRenderThread;

        m_needToRestartRenderThread = false;

        lock.unlock();

        if (needsRenderThreadRestart)
        {
            Changed();
        }

        return S_OK;
    }   

    void CanvasAnimatedControl::CreateSwapChainPanel()
    {
        auto swapChainPanelAdapter = GetAdapter();
        m_canvasSwapChainPanel = swapChainPanelAdapter->CreateCanvasSwapChainPanel();

        auto swapChainPanelAsUIElement = As<IUIElement>(m_canvasSwapChainPanel);

        auto thisAsUserControl = As<IUserControl>(GetComposableBase());
        ThrowIfFailed(thisAsUserControl->put_Content(swapChainPanelAsUIElement.Get()));
    }

    bool CanvasAnimatedControl::Tick(
        CanvasSwapChain* target, 
        Color const& clearColor, 
        bool areResourcesCreated)
    {
        m_input->ProcessEvents();

        auto lock = GetLock();

        m_stepTimer.SetTargetElapsedTicks(m_targetElapsedTime);

        m_stepTimer.SetFixedTimeStep(m_isStepTimerFixedStep);

        if (m_shouldResetElapsedTime)
        {
            m_stepTimer.ResetElapsedTime();
            m_shouldResetElapsedTime = false;
        }

        if (m_isPaused)
        {
            // Prevents drawing logic from occurring while control is paused.
            areResourcesCreated = false;
        }

        bool needToRestartRenderThread = m_needToRestartRenderThread;
        lock.unlock();

        bool updated = true;

        if (areResourcesCreated)
            updated = Update();

        if (updated)
        {
            //
            // An app's draw handlers may call methods like get_Size, and so the lock definitely
            // shouldn't cover the invocation of them. 
            //
            Draw(target, clearColor, areResourcesCreated);

            ThrowIfFailed(target->Present());
        }

        if (needToRestartRenderThread)
        {
            // Returning false out of here will stop the worker thread.
            return false;
        }

        return areResourcesCreated;
    }

    bool CanvasAnimatedControl::Update()
    {
        bool updated = false;
        
        auto updateFunction = [this, &updated](bool isRunningSlowly)
        {
            auto timing = GetTimingInformationFromTimer();
            timing.IsRunningSlowly = isRunningSlowly;

            auto updateEventArgs = Make<CanvasAnimatedUpdateEventArgs>(timing);
            ThrowIfFailed(m_updateEventList.InvokeAll(this, updateEventArgs.Get()));

            m_forceUpdate = false;
            updated = true;
        };

        m_stepTimer.Tick(updateFunction);
                
        if (!updated && m_forceUpdate)
        {
            updateFunction(false);
        }

        return updated;
    }

    CanvasTimingInformation CanvasAnimatedControl::GetTimingInformationFromTimer()
    {
        CanvasTimingInformation timing;
        timing.UpdateCount = m_stepTimer.GetFrameCount();
        timing.ElapsedTime.Duration = m_stepTimer.GetElapsedTicks();
        timing.TotalTime.Duration = m_stepTimer.GetTotalTicks();
        timing.IsRunningSlowly = false;

        return timing;
    }

    ActivatableClassWithFactory(CanvasAnimatedUpdateEventArgs, CanvasAnimatedUpdateEventArgsFactory);
    ActivatableClassWithFactory(CanvasAnimatedDrawEventArgs, CanvasAnimatedDrawEventArgsFactory);
    ActivatableClassWithFactory(CanvasAnimatedControl, CanvasAnimatedControlFactory);
}}}}
