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
                DirectXPixelFormat::B8G8R8A8UIntNormalized,
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

        virtual void StartChangedAction(ComPtr<IWindow> const& window, std::function<void()> changedFn)
        {
            if (IsDesignModeEnabled())
            {
                return;
            }

            ComPtr<ICoreDispatcher> dispatcher;
            ThrowIfFailed(window->get_Dispatcher(&dispatcher));

            auto callback = Callback<AddFtmBase<IDispatchedHandler>::Type>(
                [=]
                {
                    return ExceptionBoundary(
                        [&]
                        {
                            changedFn();
                        });
                });

            ComPtr<IAsyncAction> asyncAction;
            ThrowIfFailed(dispatcher->RunAsync(CoreDispatcherPriority_Normal, callback.Get(), &asyncAction));
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
        , m_stepTimer(adapter)
        , m_hasUpdated(false)
        , m_sharedState{}
    {
        CreateSwapChainPanel();

        auto swapChainPanel = As<ISwapChainPanel>(m_canvasSwapChainPanel);

        m_input = Make<AnimatedControlInput>(swapChainPanel);

        m_sharedState.IsStepTimerFixedStep = m_stepTimer.IsFixedTimeStep();
        m_sharedState.TargetElapsedTime = m_stepTimer.GetTargetElapsedTicks();
        m_sharedState.NeedsDraw = true;
    }

    CanvasAnimatedControl::~CanvasAnimatedControl()
    {
        assert(!IsRenderLoopRunning());
    }

    bool CanvasAnimatedControl::IsRenderLoopRunning() const
    {
        if (!m_renderLoopAction)
            return false;

        AsyncStatus status;
        ThrowIfFailed(As<IAsyncInfo>(m_renderLoopAction)->get_Status(&status));
        return (status == AsyncStatus::Started);
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

                m_sharedState.IsStepTimerFixedStep = !!value;
            });
    }

    IFACEMETHODIMP CanvasAnimatedControl::get_IsFixedTimeStep(boolean* value)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(value);

                auto lock = GetLock();

                *value = m_sharedState.IsStepTimerFixedStep; 
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

                m_sharedState.TargetElapsedTime = value.Duration;
            });
    }

    IFACEMETHODIMP CanvasAnimatedControl::get_TargetElapsedTime(TimeSpan* value)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(value);

                auto lock = GetLock();

                assert(m_sharedState.TargetElapsedTime <= INT64_MAX);

                TimeSpan timeSpan = {};
                timeSpan.Duration = static_cast<INT64>(m_sharedState.TargetElapsedTime);
                *value = timeSpan;
            });
    }

    IFACEMETHODIMP CanvasAnimatedControl::put_Paused(boolean value)
    {
        return ExceptionBoundary(
            [&]
            {
                auto lock = GetLock();

                m_sharedState.IsPaused = !!value;

                bool paused = m_sharedState.IsPaused;

                if (!paused)    // TODO: should this be if (paused != !!value)?
                    Changed(lock);
            });
    }

    IFACEMETHODIMP CanvasAnimatedControl::get_Paused(boolean* value)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(value);

                auto lock = GetLock();

                *value = m_sharedState.IsPaused;
            });
    }

    IFACEMETHODIMP CanvasAnimatedControl::get_Size(Size* value)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(value);

                *value = GetCurrentSize();
            });
    }

    IFACEMETHODIMP CanvasAnimatedControl::ResetElapsedTime()
    {
        return ExceptionBoundary(
            [&]
            {
                auto lock = GetLock();

                m_sharedState.ShouldResetElapsedTime = true;
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

    void CanvasAnimatedControl::Unloaded()
    {
        // Stop the update/render thread if it is running
        if (m_renderLoopAction)
        {
            auto asyncInfo = As<IAsyncInfo>(m_renderLoopAction);

            ThrowIfFailed(asyncInfo->Cancel());
        }
    }

    void CanvasAnimatedControl::ApplicationSuspending(ISuspendingEventArgs* args)
    {
        ComPtr<ISuspendingOperation> suspendingOperation;
        ThrowIfFailed(args->get_SuspendingOperation(&suspendingOperation));

        ThrowIfFailed(suspendingOperation->GetDeferral(&m_suspendingDeferral));

        //
        // We want to call Trim when the application suspends, but can't do that until the
        // render thread has stopped. There are three possible ways this can take place:
        //
        //  1) Render thread is active, so this Changed call is ignored. The render thread
        //     then notices the app is suspended, halts itself, and its completion handler
        //     calls Changed again, which picks up the pending deferral and calls Trim.
        //
        //  2) Render thread is stopped, but another Changed action is already in progress.
        //     The Changed implementation handles that for us and will process this request
        //     after the previous Changed completes.
        //
        //  3) Nothing is active, in which case Changed will process the deferral and call
        //     Trim straight away. In this case we could inline the Trim call and skip the
        //     deferral altogether, but deferring that decision to the existing mechanisms
        //     inside Changed allows sharing a single suspend codepath for all possible states.
        //
        Changed(GetLock());
    }

    void CanvasAnimatedControl::ApplicationResuming()
    {
        Changed(GetLock());
    }

    void CanvasAnimatedControl::Changed(Lock const& lock, ChangeReason reason)
    {
        //
        // This may be called from any thread.
        //

        if (!IsLoaded())
            return;

        MustOwnLock(lock);

        switch (reason)
        {
        case ChangeReason::ClearColor:
            m_sharedState.NeedsDraw = true;
            break;

        case ChangeReason::DeviceLost:
            m_sharedState.NeedsDraw = true;
            m_hasUpdated = false;
            break;

        default:
            // nothing
            break;
        }


        //
        // The remaining work must be done on the UI thread.  There's a chance
        // that the this might result in Changed() being called again, or that
        // Changed() may be called while ChangedImpl() happens.  Because of this
        // we support reentrancy by allowing multiple changed actions to be
        // scheduled at a time.
        //

        ComPtr<CanvasAnimatedControl> self = this;

        GetAdapter()->StartChangedAction(GetWindow(),
            [self]
            {
                self->ChangedImpl();
            });
    }

    void CanvasAnimatedControl::ChangedImpl()
    {
        //
        // This is called when something has happened that should force the
        // control to redraw itself.  For example, DPI has changed, device has
        // been lost, resources have been loaded.
        //
        // This method, as an action, is always run on the UI thread.
        //

        if (!IsLoaded())
            return;

        auto lock = GetLock();
        m_sharedState.ForceUpdate = true;

        bool needsDraw = m_sharedState.NeedsDraw;
        bool isPaused = m_sharedState.IsPaused;

        lock.unlock();

        //
        // Get the status of the update/render thread.
        //
        auto renderLoopInfo = MaybeAs<IAsyncInfo>(m_renderLoopAction);
        auto renderLoopStatus = AsyncStatus::Completed;

        if (renderLoopInfo)
        {
            ThrowIfFailed(renderLoopInfo->get_Status(&renderLoopStatus));

            switch (renderLoopStatus)
            {
            case AsyncStatus::Started:
                // The update/render loop is still running
                return;
                
            case AsyncStatus::Completed:
            case AsyncStatus::Canceled:
            case AsyncStatus::Error:
                m_renderLoopAction.Reset();
                // We still hold renderLoopInfo; this is used when we process
                // the error from within the RunWithRenderTarget call below.
                break;
                
            default:
                assert(false);
            }
        }

        //
        // Figure out if we should try and start the render loop.
        //
        bool ignorePaused = false;

        if (renderLoopStatus == AsyncStatus::Error)
        {
            // If the last render loop ended due to an error (eg lost device)
            // then we want to process the error.  The error is processed below
            // where we try and start the render loop.  In this context we can
            // rethrow the exception that was marshaled from the render loop and
            // have it caught and handled by RunWithRenderTarget.
            ignorePaused = true;
        }

        if (needsDraw)
        {
            // If we've marked that we need to redraw (eg because the clear
            // color has changed) then we'll need to start the loop.
            ignorePaused = true;
        }

        if (isPaused && !ignorePaused)
        {
            // Don't start the render loop if we're paused
            return;
        }


        //
        // Call Trim on application suspend, but only once we are sure the
        // render thread is no longer running.
        //
        if (IsSuspended())
        {
            if (m_suspendingDeferral)
            {
                Trim();

                ThrowIfFailed(m_suspendingDeferral->Complete());
                m_suspendingDeferral.Reset();
            }

            return;
        }

        //
        // Try and start the update/render thread
        //
        RunWithRenderTarget(GetCurrentSize(),
            [&] (CanvasSwapChain* rawTarget, ICanvasDevice*, Color const& clearColor, bool areResourcesCreated)
            {
                // The clearColor passed to us is ignored since this needs to be
                // checked on each tick of the update/render loop.
                UNREFERENCED_PARAMETER(clearColor);

                //
                // Process the results of the previous update/render loop.  We
                // do this here, within RunWithRenderTarget, so that it can
                // handle errors (such as lost device).
                //
                assert(!m_renderLoopAction);
                assert(renderLoopStatus != AsyncStatus::Started);

                if (renderLoopStatus == AsyncStatus::Error)
                {
                    HRESULT hr = S_OK;
                    ThrowIfFailed(renderLoopInfo->get_ErrorCode(&hr));
                    ThrowHR(hr);
                }

                if (!areResourcesCreated && !needsDraw)
                {
                    //
                    // If resources aren't created then there's no point
                    // spinning up the update/render thread.
                    //
                    return;
                }

                ComPtr<CanvasSwapChain> target(rawTarget);
                m_stepTimer.ResetElapsedTime();

                ComPtr<CanvasAnimatedControl> self = this;
                
                auto beforeLoopFn = 
                    [self]()
                    {
                        self->m_input->SetSource();
                    };

                auto onEachTickFn = 
                    [target, areResourcesCreated, self]
                    {
                        return self->Tick(target.Get(), areResourcesCreated);
                    };

                auto afterLoopFn = 
                    [self]()
                    {
                        self->m_input->RemoveSource();
                    };

                m_renderLoopAction = GetAdapter()->StartUpdateRenderLoop(beforeLoopFn, onEachTickFn, afterLoopFn);
                
                auto completed = Callback<IAsyncActionCompletedHandler>(
                    [self](IAsyncAction*, AsyncStatus)
                    {
                        return ExceptionBoundary(
                            [&]
                            {
                                self->Changed(self->GetLock());
                            });
                    });

                CheckMakeResult(completed);

                ThrowIfFailed(m_renderLoopAction->put_Completed(completed.Get()));
            });
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
        CanvasSwapChain*, 
        bool areResourcesCreated)
    {
        RenderTarget* renderTarget = GetCurrentRenderTarget();

        //
        // Process input events; this is always done in order to avoid events
        // getting missed in the case of anything that restarts the
        // update/render loop.
        //
        m_input->ProcessEvents();

        //
        // Access shared state that's shared between the UI thread and the
        // update/render thread.  This is done in one place in order to hold the
        // lock for as little time as possible.
        //

        auto lock = GetLock();

        if (IsSuspended())
            return false;

        m_stepTimer.SetTargetElapsedTicks(m_sharedState.TargetElapsedTime);
        m_stepTimer.SetFixedTimeStep(m_sharedState.IsStepTimerFixedStep);

        if (m_sharedState.ShouldResetElapsedTime)
        {
            m_stepTimer.ResetElapsedTime();
        }

        bool isPaused = m_sharedState.IsPaused;
        bool forceUpdate = m_sharedState.ForceUpdate;
        
        m_sharedState.ShouldResetElapsedTime = false;
        m_sharedState.ForceUpdate = false;

        Color clearColor;
        Size currentSize;
        GetSharedState(lock, &clearColor, &currentSize);

        // If the opacity has changed then the swap chain will need to be
        // recreated before we can draw.

        if (renderTarget->Target &&
            GetBackgroundModeFromClearColor(clearColor) != renderTarget->BackgroundMode)
        {
            // This will cause the update/render thread to stop, giving the UI
            // thread an opportunity to recreate the swap chain.
            return false;
        }
        
        bool forceDraw = m_sharedState.NeedsDraw;

        // At this point we know that we're going to draw (unless there's some
        // kind of failure) so we can reset this flag now.  This is particularly
        // relevant for the ClearColor, since this indicates that we've
        // 'consumed' that color.
        m_sharedState.NeedsDraw = false;
        
        lock.unlock();

        //
        // Now do the update/render for this tick
        //

        bool updatedThisTick = false;
        if (areResourcesCreated && !isPaused)
            updatedThisTick = Update(forceUpdate);

        m_hasUpdated |= updatedThisTick;

        //
        // We only ever Draw/Present if an Update has actually happened.  This
        // results in us effectively busy-waiting for the next time to update.
        // This is desireable since using Present to wait for the vsync can
        // result in missed frames.
        //
        if (updatedThisTick || forceDraw)
        {
            //
            // If the control's size has changed then the swapchain's buffers
            // need to be resized as appropriate.
            //
            if (renderTarget->Size != currentSize)
            {
                if (currentSize.Width <= 0 || currentSize.Height <= 0 || !renderTarget->Target)
                {
                    //
                    // Switching between zero and non-zero sized rendertargets requires calling
                    // CanvasSwapChainPanel::put_SwapChain, so must be done on the UI thread.
                    // We must stop the update/render thread to allow this to happen.
                    //
                    return false;
                }
                else
                {
                    // This can be done on the update/render thread because:
                    //
                    //  - no XAML methods are called
                    //
                    //  - the current render target won't be updated by the UI thread
                    //    while the update/render thread is running
                    //
                    ThrowIfFailed(renderTarget->Target->ResizeBuffersWithSize(currentSize.Width, currentSize.Height));
                    renderTarget->Size = currentSize;
                }
            }

            if (renderTarget->Target)
            {
                bool invokeDrawHandlers = (areResourcesCreated && m_hasUpdated);

                Draw(renderTarget->Target.Get(), clearColor, invokeDrawHandlers);
                ThrowIfFailed(renderTarget->Target->Present());
            }
        }

        return areResourcesCreated && !isPaused;
    }

    bool CanvasAnimatedControl::Update(bool forceUpdate)
    {
        bool updated = false;
        
        m_stepTimer.Tick(forceUpdate, 
            [this, &updated](bool isRunningSlowly)
            {
                auto timing = GetTimingInformationFromTimer();
                timing.IsRunningSlowly = isRunningSlowly;

                auto updateEventArgs = Make<CanvasAnimatedUpdateEventArgs>(timing);
                ThrowIfFailed(m_updateEventList.InvokeAll(this, updateEventArgs.Get()));

                updated = true;
            });

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
