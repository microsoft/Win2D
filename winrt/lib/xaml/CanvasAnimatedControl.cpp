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

#include "CanvasGameLoop.h"

using namespace ::ABI::Microsoft::Graphics::Canvas;
using namespace ::ABI::Microsoft::Graphics::Canvas::UI;
using namespace ::ABI::Microsoft::Graphics::Canvas::UI::Xaml;
using namespace ::Microsoft::WRL::Wrappers;

//
// CanvasAnimatedUpdateEventArgsFactory implementation
//

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

//
// CanvasAnimatedUpdateEventArgs implementation
//

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

//
// CanvasAnimatedDrawEventArgsFactory implementation
//

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

//
// CanvasAnimatedDrawEventArgs implementation
//

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

//
// CanvasAnimatedControlFactory
//

class CanvasAnimatedControlFactory : public ActivationFactory<>,
                                     private LifespanTracker<CanvasAnimatedControlFactory>
{
    std::weak_ptr<ICanvasAnimatedControlAdapter> m_adapter;

public:
    IFACEMETHODIMP ActivateInstance(IInspectable** obj)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckAndClearOutPointer(obj);

                auto adapter = m_adapter.lock();

                if (!adapter)
                    m_adapter = adapter = CreateCanvasAnimatedControlAdapter();

                auto control = Make<CanvasAnimatedControl>(adapter);
                CheckMakeResult(control);

                ThrowIfFailed(control.CopyTo(obj));
            });
    }
};

//
// CanvasAnimatedControl implementation
//

CanvasAnimatedControl::CanvasAnimatedControl(std::shared_ptr<ICanvasAnimatedControlAdapter> adapter)
    : BaseControl<CanvasAnimatedControlTraits>(adapter, false)
    , m_stepTimer(adapter)
    , m_hasUpdated(false)
{
    CreateSwapChainPanel();

    auto swapChainPanel = As<ISwapChainPanel>(m_canvasSwapChainPanel);

    m_input = Make<AnimatedControlInput>(swapChainPanel);

    m_sharedState.IsStepTimerFixedStep = m_stepTimer.IsFixedTimeStep();
    m_sharedState.TargetElapsedTime = m_stepTimer.GetTargetElapsedTicks();
}

CanvasAnimatedControl::~CanvasAnimatedControl()
{
}

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

            if (!m_sharedState.IsPaused)
            {
                m_sharedState.FirstTickAfterWasPaused = true;
                Changed(lock);
            }
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

IFACEMETHODIMP CanvasAnimatedControl::Invalidate()
{
    return ExceptionBoundary(
        [&]
        {
            auto lock = GetLock();

            auto wasInvalidated = m_sharedState.Invalidated;
            
            m_sharedState.Invalidated = true;

            if (!wasInvalidated)
            {
                Changed(lock);
            }
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

IFACEMETHODIMP CanvasAnimatedControl::RemoveFromVisualTree()
{
    HRESULT hr = ExceptionBoundary(
        [&]
        {
            ThrowIfFailed(m_canvasSwapChainPanel->RemoveFromVisualTree());
            m_canvasSwapChainPanel.Reset();
        });

    if (FAILED(hr))
        return hr;

    return BaseControl::RemoveFromVisualTree();
}

IFACEMETHODIMP CanvasAnimatedControl::get_HasGameLoopThreadAccess(boolean* value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(value);

            //
            // This control's input objects are always created for the
            // render thread, and they have their own dispatcher object
            // created for them. This simply routes to that dispatcher.
            //
            // The AnimatedControlInput object's own lock protects against 
            // hazards where the input object is created or destroyed 
            // before this method completes.
            //

            *value = m_input->GetHasThreadAccess();
        });
}

IFACEMETHODIMP CanvasAnimatedControl::RunOnGameLoopThreadAsync(
    IDispatchedHandler* callback,
    IAsyncAction** asyncAction)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(callback);
            CheckAndClearOutPointer(asyncAction);

            auto lock = GetLock();

            auto newAsyncAction = Make<AnimatedControlAsyncAction>(callback);
            CheckMakeResult(newAsyncAction);
            m_sharedState.PendingAsyncActions.push_back(newAsyncAction);

            ThrowIfFailed(newAsyncAction.CopyTo(asyncAction));

            // If we're paused then we need to arrange to reschedule the tick
            // loop, otherwise we won't get around to running the callback.
            if (m_sharedState.IsPaused)
            {
                Changed(lock);
            }
        });
}

void CanvasAnimatedControl::CreateOrUpdateRenderTarget(
    ICanvasDevice* device,
    CanvasAlphaMode newAlphaMode,
    float newDpi,
    Size newSize,
    RenderTarget* renderTarget)
{
    bool needsTarget = (renderTarget->Target == nullptr);
    bool alphaModeChanged = (renderTarget->AlphaMode != newAlphaMode);
    bool dpiChanged = (renderTarget->Dpi != newDpi);
    bool sizeChanged = (renderTarget->Size != newSize);
    bool needsCreate = needsTarget || alphaModeChanged || dpiChanged;

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
        ThrowIfFailed(renderTarget->Target->ResizeBuffersWithWidthAndHeight(newSize.Width, newSize.Height));

        renderTarget->Size = newSize;
    }
    else
    {
        renderTarget->Target = GetAdapter()->CreateCanvasSwapChain(
            device,
            newSize.Width,
            newSize.Height,
            newDpi,
            newAlphaMode);

        renderTarget->AlphaMode = newAlphaMode;
        renderTarget->Dpi = newDpi;
        renderTarget->Size = newSize;

        ThrowIfFailed(m_canvasSwapChainPanel->put_SwapChain(renderTarget->Target.Get()));            
    }
}

ComPtr<CanvasAnimatedDrawEventArgs> CanvasAnimatedControl::CreateDrawEventArgs(
    ICanvasDrawingSession* drawingSession,
    bool isRunningSlowly)
{
    auto timing = GetTimingInformationFromTimer();
    timing.IsRunningSlowly = isRunningSlowly;

    auto drawEventArgs = Make<CanvasAnimatedDrawEventArgs>(drawingSession, timing);
    CheckMakeResult(drawEventArgs);
    return drawEventArgs;
}

void CanvasAnimatedControl::Loaded()
{
    assert(!m_gameLoop);
    m_gameLoop = GetAdapter()->CreateAndStartGameLoop(As<ISwapChainPanel>(m_canvasSwapChainPanel), m_input);
}

void CanvasAnimatedControl::Unloaded()
{    
    m_gameLoop.reset();
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

void CanvasAnimatedControl::WindowVisibilityChanged()
{
    //
    // Note that we don't stop the game loop thread here, because
    // we still expect Update to be called.
    //
}

void CanvasAnimatedControl::Changed(Lock const& lock, ChangeReason reason)
{
    //
    // This may be called from any thread.
    //

    MustOwnLock(lock);

    //
    // Early out if we are in an inactive state where there is no possible work to be done.
    //
    if (!IsLoaded() && !m_suspendingDeferral)
        return;

    switch (reason)
    {
    case ChangeReason::ClearColor:
    case ChangeReason::Size:
        m_sharedState.NeedsDraw = true;
        break;

    case ChangeReason::DeviceLost:
        m_sharedState.NeedsDraw = true;
        m_hasUpdated = false;
        break;

    case ChangeReason::DeviceCreationOptions:
        m_sharedState.DeviceNeedsReCreationWithNewOptions = true;
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

    if (GetAdapter()->IsDesignModeEnabled())
        return;

    ComPtr<ICoreDispatcher> dispatcher;
    ThrowIfFailed(GetWindow()->get_Dispatcher(&dispatcher));

    WeakRef weakSelf = AsWeak(this);
    auto callback = Callback<AddFtmBase<IDispatchedHandler>::Type>(
        [weakSelf] () mutable
        {
            return ExceptionBoundary(
                [&]
                {
                    auto strongSelf = LockWeakRef<ICanvasAnimatedControl>(weakSelf);
                    auto self = static_cast<CanvasAnimatedControl*>(strongSelf.Get());
                    
                    if (self)
                        self->ChangedImpl();
                });
        });

    ComPtr<IAsyncAction> asyncAction;
    ThrowIfFailed(dispatcher->RunAsync(CoreDispatcherPriority_Normal, callback.Get(), &asyncAction));
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

    auto lock = GetLock();

    bool needsDraw = m_sharedState.NeedsDraw || m_sharedState.Invalidated;
    bool isPaused = m_sharedState.IsPaused;
    bool wasPaused = m_sharedState.FirstTickAfterWasPaused;
    bool hasPendingActions = !m_sharedState.PendingAsyncActions.empty();
    DeviceCreationOptions deviceCreationOptions = GetDeviceCreationOptions(lock);

    lock.unlock();

    //
    // Get the status of the update/render thread.
    //
    bool tickLoopIsRunning = false;
    ComPtr<IAsyncInfo> tickLoopErrorInfo;

    if (m_gameLoop)
    {
        m_gameLoop->TakeTickLoopState(&tickLoopIsRunning, &tickLoopErrorInfo);

        if (tickLoopIsRunning)
        {
            // The tick loop is still running, so we don't do anything here.
            // Ultimately, we rely on the tick loop noticing that something
            // needs to change.  When it does, it will stop itself and trigger
            // another Changed().
            return;
        }
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
    // Changed already checked IsLoaded before queueing this call to ChangedImpl,
    // but we could have become unloaded between then and now, so must check again.
    //
    if (!IsLoaded())
        return;

    //
    // Figure out if we should try and start the render loop.
    //
    bool ignorePaused = false;

    if (tickLoopErrorInfo)
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

    if (hasPendingActions)
    {
        // If there are actions pending then we want to give the loop a chance
        // to run those actions.
        ignorePaused = true;
    }

    if (isPaused && !ignorePaused)
    {
        // Don't start the render loop if we're paused
        return;
    }

    //
    // Try and start the update/render thread
    //
    RunWithRenderTarget(GetCurrentSize(), deviceCreationOptions,
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
            if (tickLoopErrorInfo)
            {
                HRESULT hr;
                ThrowIfFailed(tickLoopErrorInfo->get_ErrorCode(&hr));
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

            if (wasPaused)      // TODO: figure out the right thing here; resetting the timer probably isn't it.
                m_stepTimer.ResetElapsedTime();

            ComPtr<CanvasSwapChain> target(rawTarget);

            m_gameLoop->StartTickLoop(this,
                [target, areResourcesCreated] (CanvasAnimatedControl* control)
                { 
                    return control->Tick(target.Get(), areResourcesCreated); 
                },
                [] (CanvasAnimatedControl* control)
                { 
                    control->Changed(control->GetLock()); 
                });
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

void CanvasAnimatedControl::IssueAsyncActions(
    std::vector<ComPtr<AnimatedControlAsyncAction>> const& pendingActions)
{
    //
    // Fire all the async actions.
    //
    for (auto actionIter = pendingActions.begin(); actionIter != pendingActions.end(); ++actionIter)
    {
        HRESULT actionsResult = S_OK;

        auto invocationResult = (*actionIter)->InvokeAndFireCompletion();

        if (DeviceLostException::IsDeviceLostHResult(invocationResult.ActionResult))
        {
            // Expected to be rethrown.
            actionsResult = invocationResult.ActionResult;
        }            
        else if (FAILED(invocationResult.CompletedHandlerResult))
        {
            // Expected to be rethrown, regardless of whether it's device lost.
            //
            // Note that device lost from the handler, above, is taking precedence 
            // over any errors reported out of the completed handler. This is by design.
            //
            // Reporting device lost out of here plus an error out of a completed
            // handler requires some extra infrastructure, which is arguably not
            // worth implementing.
            //
            actionsResult = invocationResult.CompletedHandlerResult;
        }

        // 
        // If this async action failed to run, the remaining async actions
        // cannot run, and need to be re-queued.
        //
        if (FAILED(actionsResult))
        {
            auto lock = GetLock();

            m_sharedState.PendingAsyncActions.insert(m_sharedState.PendingAsyncActions.begin(), actionIter+1, pendingActions.end());

            lock.unlock();

            ThrowHR(actionsResult);
        }
    }
}

bool CanvasAnimatedControl::Tick(
    CanvasSwapChain* swapChain, 
    bool areResourcesCreated)
{
    RenderTarget* renderTarget = GetCurrentRenderTarget();

    //
    // On hardware rendering, the control synchronizes to vertical blank 
    // in order to
    //
    // 1) Reduce the amount of busy-spinning for targetElapsedTime on fixed timestep 
    // 2) Avoid performing imperceptible work on variable timestep.
    //
    // Unlike an awaitable timer for the swap chain object, this
    // behaves identically regardless of whether any Present was
    // actually issued.
    //
    // The exact behavior depends on the device of this control- whether
    // it is hardware or software. CanvasAnimatedControl does not typically 
    // run on software 'render-only' mode. It may run on basic display, but that has 
    // no special implications for synching with v-blank. 
    //
    // The only way it may be initialized with WARP render-only is if
    // something is malfunctioning in the system's driver, causing
    // WARP to be used as a fallback (see CanvasDeviceManager::MakeD3D11Device). 
    // 
    // In the future, we may allow the app to use its own device with
    // the control, making the software path to be more accessible to an app.
    //
    // On a software render-only device, the display is not accessible and
    // IDXGIAdapter::EnumOutputs will fail. In this case, WaitForVerticalBlank
    // will yield.
    //
    if (swapChain)
    {
        ThrowIfFailed(swapChain->WaitForVerticalBlank());
    }
    else
    {
        //
        // If there is no swap chain available, 
        // a sleep is used instead to avoid pegging the CPU.
        //
        GetAdapter()->Sleep(static_cast<DWORD>(StepTimer::TicksToMilliseconds(StepTimer::DefaultTargetElapsedTime)));
    }

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
    bool firstTickAfterWasPaused = m_sharedState.FirstTickAfterWasPaused;
    bool deviceNeedsReCreationWithNewOptions = m_sharedState.DeviceNeedsReCreationWithNewOptions;

    m_sharedState.ShouldResetElapsedTime = false;
    m_sharedState.FirstTickAfterWasPaused = false;
    m_sharedState.DeviceNeedsReCreationWithNewOptions = false;

    Color clearColor;
    Size currentSize;
    GetSharedState(lock, &clearColor, &currentSize);

    // If the opacity has changed then the swap chain will need to be
    // recreated before we can draw.

    if (renderTarget->Target &&
        GetAlphaModeFromClearColor(clearColor) != renderTarget->AlphaMode)
    {
        // This will cause the update/render thread to stop, giving the UI
        // thread an opportunity to recreate the swap chain.
        return false;
    }

    // If the device needs to be re-created with different options, this 
    // needs to happen before we can draw.
    if (deviceNeedsReCreationWithNewOptions)
    {
        return false;
    }

    bool forceDraw = m_sharedState.NeedsDraw;

    // At this point we know that we're going to handle the NeedsDraw (unless
    // there's some kind of failure) so we can reset this flag now.  This is
    // particularly relevant for the ClearColor, since this indicates that we've
    // 'consumed' that color.
    m_sharedState.NeedsDraw = false;

    bool invalidated = m_sharedState.Invalidated; 

    // If resources are created then we know that we'll be able to process the
    // Invalidated request, so we can reset this flag now.
    if (areResourcesCreated)
        m_sharedState.Invalidated = false;
    
    // We update, but forego drawing if the control is not in a visible state.
    // Force-draws, like those due to device lost, are not performed either.
    // Drawing behavior resumes when the control becomes visible once again.
    bool isVisible = IsVisible();

    //
    // Copy out the list of async actions, to avoid retaining the lock while
    // they are being fired.
    //
    std::vector<ComPtr<AnimatedControlAsyncAction>> pendingActions;
    std::swap(pendingActions, m_sharedState.PendingAsyncActions);

    lock.unlock();

    //
    // Run any async actions
    //
    bool hadPendingActions = !pendingActions.empty();
    IssueAsyncActions(pendingActions);

    // One of the async actions may have changed the shared state, in which case
    // we want to respond immediately.
    if (hadPendingActions && !invalidated)
    {
        auto lock2 = GetLock();

        invalidated = m_sharedState.Invalidated;
        if (areResourcesCreated)
            m_sharedState.Invalidated = false;
    }

    //
    // Now do the update/render for this tick
    //

    UpdateResult updateResult{};

    if (areResourcesCreated && !isPaused)
    {
        bool forceUpdate = (firstTickAfterWasPaused || !m_hasUpdated);

        updateResult = Update(forceUpdate);

        m_hasUpdated |= updateResult.Updated;
    }

    //
    // We only ever Draw/Present if an Update has actually happened.  This
    // results in us waiting until the next vblank to update.
    // This is desireable since using Present to wait for the vsync can
    // result in missed frames.
    //
    if ((updateResult.Updated || forceDraw || invalidated) && isVisible)
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
                ThrowIfFailed(renderTarget->Target->ResizeBuffersWithWidthAndHeight(currentSize.Width, currentSize.Height));
                renderTarget->Size = currentSize;
            }
        }

        if (renderTarget->Target)
        {
            bool invokeDrawHandlers = (areResourcesCreated && (m_hasUpdated || invalidated));

            Draw(renderTarget->Target.Get(), clearColor, invokeDrawHandlers, updateResult.IsRunningSlowly);
            ThrowIfFailed(renderTarget->Target->Present());
        }
    }

    return areResourcesCreated && !isPaused;
}

CanvasAnimatedControl::UpdateResult CanvasAnimatedControl::Update(bool forceUpdate)
{
    UpdateResult result{};

    m_stepTimer.Tick(forceUpdate,
        [this, &result](bool isRunningSlowly)
        {
            auto timing = GetTimingInformationFromTimer();
            timing.IsRunningSlowly = isRunningSlowly;

            auto updateEventArgs = Make<CanvasAnimatedUpdateEventArgs>(timing);
            ThrowIfFailed(m_updateEventList.InvokeAll(this, updateEventArgs.Get()));

            result.IsRunningSlowly = isRunningSlowly;
            result.Updated = true;
        });

    return result;
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
