// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

#include "CanvasGameLoop.h"

using namespace ::ABI::Microsoft::Graphics::Canvas;
using namespace ::ABI::Microsoft::Graphics::Canvas::UI;
using namespace ::ABI::Microsoft::Graphics::Canvas::UI::Xaml;
using namespace ::ABI::Microsoft::Graphics::Canvas::UI::Xaml::Media;
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

class CanvasAnimatedControlFactory : public AgileActivationFactory<>,
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
    : BaseControlWithDrawHandler<CanvasAnimatedControlTraits>(adapter, false)
    , m_stepTimer(adapter)
    , m_hasUpdated(false)
{
    CreateContentControl();

    m_sharedState.IsStepTimerFixedStep = m_stepTimer.IsFixedTimeStep();
    m_sharedState.TargetElapsedTime = m_stepTimer.GetTargetElapsedTicks();
}

CanvasAnimatedControl::~CanvasAnimatedControl()
{
    // These should all have been canceled on unload
    assert(m_sharedState.PendingAsyncActions.empty());
}

IFACEMETHODIMP CanvasAnimatedControl::put_ClearColor(
    Color value)
{
    if (m_designModeShape)
    {
        return ExceptionBoundary(
            [&]
            {
                ComPtr<IBrush> brush;
                ThrowIfFailed(m_designModeShape->get_Fill(&brush));

                ThrowIfFailed(As<ISolidColorBrush>(brush)->put_Color(value));
            });
    }
    else
    {
        return BaseControlWithDrawHandler::put_ClearColor(value);
    }
}

IFACEMETHODIMP CanvasAnimatedControl::get_ClearColor(
    Color* value)
{
    if (m_designModeShape)
    {
        return ExceptionBoundary(
            [&]
            {
                ComPtr<IBrush> brush;
                ThrowIfFailed(m_designModeShape->get_Fill(&brush));

                ThrowIfFailed(As<ISolidColorBrush>(brush)->get_Color(value));
            });
    }
    else
    {
        return BaseControlWithDrawHandler::get_ClearColor(value);
    }
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

IFACEMETHODIMP CanvasAnimatedControl::add_GameLoopStarting(
    ITypedEventHandler<ICanvasAnimatedControl*, IInspectable*>* value,
    EventRegistrationToken* token)
{
    return ExceptionBoundary(
        [&]
        {
            ThrowIfFailed(m_gameLoopStartingEventList.Add(value, token));
        });
}

IFACEMETHODIMP CanvasAnimatedControl::remove_GameLoopStarting(
    EventRegistrationToken token)
{
    return ExceptionBoundary(
        [&]
        {
            ThrowIfFailed(m_gameLoopStartingEventList.Remove(token));
        });
}

IFACEMETHODIMP CanvasAnimatedControl::add_GameLoopStopped(
    ITypedEventHandler<ICanvasAnimatedControl*, IInspectable*>* value,
    EventRegistrationToken* token)
{
    return ExceptionBoundary(
        [&]
        {
            ThrowIfFailed(m_gameLoopStoppedEventList.Add(value, token));
        });
}

IFACEMETHODIMP CanvasAnimatedControl::remove_GameLoopStopped(
    EventRegistrationToken token)
{
    return ExceptionBoundary(
        [&]
        {
            ThrowIfFailed(m_gameLoopStoppedEventList.Remove(token));
        });
}

IFACEMETHODIMP CanvasAnimatedControl::put_IsFixedTimeStep(boolean value)
{
    return ExceptionBoundary(
        [&]
        {
            auto lock = Lock(m_sharedStateMutex);
            m_sharedState.IsStepTimerFixedStep = !!value;
        });
}

IFACEMETHODIMP CanvasAnimatedControl::get_IsFixedTimeStep(boolean* value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(value);

            auto lock = Lock(m_sharedStateMutex);
            *value = m_sharedState.IsStepTimerFixedStep; 
        });
}

IFACEMETHODIMP CanvasAnimatedControl::put_TargetElapsedTime(TimeSpan value)
{
    return ExceptionBoundary(
        [&]
        {
            if (value.Duration <= 0)
            {
                ThrowHR(E_INVALIDARG, Strings::ExpectedPositiveNonzero);
            }

            auto lock = Lock(m_sharedStateMutex);
            m_sharedState.TargetElapsedTime = value.Duration;
        });
}

IFACEMETHODIMP CanvasAnimatedControl::get_TargetElapsedTime(TimeSpan* value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(value);

            auto lock = Lock(m_sharedStateMutex);
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
            auto lock = Lock(m_sharedStateMutex);

            bool oldState = m_sharedState.IsPaused;
            m_sharedState.IsPaused = !!value;

            if (oldState != m_sharedState.IsPaused)
            {
                auto currentTime = GetAdapter()->GetPerformanceCounter();

                if (!m_sharedState.IsPaused)
                {
                    // Accumulate the time spent paused.  This will be accounted
                    // for when the game loop timer ticks.
                    //
                    // Since different threads may set/unset Paused, and these
                    // don't necessarily have tightly synchronized results from
                    // QueryPerformanceCounter there's a possibility that we
                    // could see Paused to false 'before' it was set to true.
                    // The 'max' below guards against this.
                    m_sharedState.TimeSpentPaused += std::max(0LL, (currentTime - m_sharedState.TimeWhenPausedWasSet));
                    assert(m_sharedState.TimeSpentPaused >= 0);

                    lock.unlock();
                    Changed(ChangeReason::Other);
                }
                else
                {
                    m_sharedState.TimeWhenPausedWasSet = currentTime;
                }
            }
        });
}

IFACEMETHODIMP CanvasAnimatedControl::get_Paused(boolean* value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(value);

            auto lock = Lock(m_sharedStateMutex);
            *value = m_sharedState.IsPaused;
        });
}

IFACEMETHODIMP CanvasAnimatedControl::get_Size(Size* value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(value);

            // While a game loop tick is running we want to only return the size
            // that this tick is operating with.  This avoids a race condition
            // where, mid tick, the control is resized, resulting in get_Size
            // returning a value that's different to the size of the swap chain
            // that'll be drawn to.
            auto lock = Lock(m_sharedStateMutex);
            if (m_sharedState.IsInTick)
            {
                *value = m_sharedState.SizeSeenByGameLoop;
                return;
            }
            lock.unlock();

            *value = GetCurrentSize();
        });
}

IFACEMETHODIMP CanvasAnimatedControl::Invalidate()
{
    return ExceptionBoundary(
        [&]
        {
            auto lock = Lock(m_sharedStateMutex);

            auto wasInvalidated = m_sharedState.Invalidated;
            
            m_sharedState.Invalidated = true;

            lock.unlock();

            if (!wasInvalidated)
            {
                Changed(ChangeReason::Other);
            }
        });
}

IFACEMETHODIMP CanvasAnimatedControl::ResetElapsedTime()
{
    return ExceptionBoundary(
        [&]
        {
            auto lock = Lock(m_sharedStateMutex);

            m_sharedState.ShouldResetElapsedTime = true;
        });
}

IFACEMETHODIMP CanvasAnimatedControl::CreateCoreIndependentInputSource(
    CoreInputDeviceTypes deviceTypes,
    ICoreInputSourceBase** returnValue)
{
    return ExceptionBoundary(
        [&]
        {
            if (!m_canvasSwapChainPanel)
            {
                assert(GetAdapter()->IsDesignModeEnabled());
                // When running in the designer there is no swap chain panel and
                // we don't support CreateCoreIndependentInputSource.
                ThrowHR(E_NOTIMPL);
            }
            
            auto swapChainPanel = As<ISwapChainPanel>(m_canvasSwapChainPanel);
            ThrowIfFailed(swapChainPanel->CreateCoreIndependentInputSource(deviceTypes, returnValue));
        });
}

IFACEMETHODIMP CanvasAnimatedControl::RemoveFromVisualTree()
{
    HRESULT hr = ExceptionBoundary(
        [&]
        {
            // In the designer there is no swap chain panel.
            if (!m_canvasSwapChainPanel)
            {
                assert(GetAdapter()->IsDesignModeEnabled());
                return;
            }
            
            ThrowIfFailed(m_canvasSwapChainPanel->RemoveFromVisualTree());
            m_canvasSwapChainPanel.Reset();
        });

    if (FAILED(hr))
        return hr;

    return BaseControlWithDrawHandler::RemoveFromVisualTree();
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

            if (!m_gameLoop)
                *value = false;
            else
                *value = m_gameLoop->HasThreadAccess();
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

            auto lock = Lock(m_sharedStateMutex);

            auto newAsyncAction = Make<AnimatedControlAsyncAction>(callback);
            CheckMakeResult(newAsyncAction);

            //
            // If we're not loaded then there's a chance we'll never get loaded
            // before this control is destroyed.  Since the game loop thread is
            // only started when we load there's a chance we'll never have an
            // opportunity to run this action.  So we only track the action if
            // we're currently loaded.
            //
            if (IsLoaded())
            {
                m_sharedState.PendingAsyncActions.push_back(newAsyncAction);
            }
            else
            {
                // This action won't ever get a chance to run, so we cancel it
                // now.

                newAsyncAction->Cancel();

                // Note: no need to fire completion here since the action is
                // newly created and there's no way a completion handler could
                // have been added.  This means we don't need to worry about
                // releasing the lock or this method taking too long to
                // complete.  When a completion handler is added to a canceled
                // action the completion handler is invoked immediately.
            }

            ThrowIfFailed(newAsyncAction.CopyTo(asyncAction));

            // If we're paused then we need to arrange to reschedule the tick
            // loop, otherwise we won't get around to running the callback.
            if (m_sharedState.IsPaused)
            {
                lock.unlock();
                Changed(ChangeReason::Other);
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
    bool needsCreate = needsTarget || alphaModeChanged;

    if (!needsCreate && !sizeChanged && !dpiChanged)
        return;

    if (newSize.Width <= 0 || newSize.Height <= 0)
    {
        // Zero-sized controls don't have swap chain objects
        *renderTarget = RenderTarget{};
        ThrowIfFailed(m_canvasSwapChainPanel->put_SwapChain(nullptr));
    }
    else if ((sizeChanged || dpiChanged) && !needsCreate)
    {
        ThrowIfFailed(renderTarget->Target->ResizeBuffersWithWidthAndHeightAndDpi(newSize.Width, newSize.Height, newDpi));

        renderTarget->Size = newSize;
        renderTarget->Dpi = newDpi;
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

    // When running in the designer there isn't a swap chain panel - and we
    // don't create the game loop.
    if (!m_canvasSwapChainPanel)
    {
        assert(GetAdapter()->IsDesignModeEnabled());
        return;
    }
    
    m_gameLoop = GetAdapter()->CreateAndStartGameLoop(this, As<ISwapChainPanel>(m_canvasSwapChainPanel).Get());
}

void CanvasAnimatedControl::Unloaded()
{    
    m_gameLoop.reset();

    // Any remaining async actions won't get executed, so we cancel them
    CancelAsyncActions();
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
    Changed(ChangeReason::Other);
}

void CanvasAnimatedControl::ApplicationResuming()
{
    Changed(ChangeReason::Other);
}

void CanvasAnimatedControl::WindowVisibilityChanged()
{
    //
    // Note that we don't stop the game loop thread here, because
    // we still expect Update to be called.
    //
}

void CanvasAnimatedControl::Changed(ChangeReason reason)
{
    //
    // This may be called from any thread.
    //

    //
    // Early out if we are in an inactive state where there is no possible work to be done.
    //
    if (!IsLoaded() && !m_suspendingDeferral)
        return;

    auto lock = Lock(m_sharedStateMutex);

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
        [weakSelf]() mutable
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

    auto lock = Lock(m_sharedStateMutex);

    bool needsDraw = m_sharedState.NeedsDraw || m_sharedState.Invalidated;
    bool isPaused = m_sharedState.IsPaused;
    bool hasPendingActions = !m_sharedState.PendingAsyncActions.empty();

    lock.unlock();

    //
    // Get the status of the update/render thread.
    //
    ComPtr<IAsyncInfo> completedTickLoopInfo;
    AsyncStatus tickLoopStatus = AsyncStatus::Completed;

    if (m_gameLoop)
    {
        bool isRunning;
        m_gameLoop->TakeTickLoopState(&isRunning, &completedTickLoopInfo);

        if (isRunning)
        {
            // The tick loop is still running, so we don't do anything here.
            // Ultimately, we rely on the tick loop noticing that something
            // needs to change.  When it does, it will stop itself and trigger
            // another Changed().
            return;
        }

        // The first time we consider starting the tick loop it won't be
        // running, but we won't have a completedTickLoopInfo either.
        if (completedTickLoopInfo)
        {
            ThrowIfFailed(completedTickLoopInfo->get_Status(&tickLoopStatus));
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

    if (tickLoopStatus != AsyncStatus::Completed)
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
    // The UI thread is about to manipulate device resources (eg it might be
    // about to call CreateResources).  While it is doing this we don't want
    // anything running on the game loop thread, so we stop the dispatcher from
    // processing events.
    //
    m_gameLoop->StopDispatcher();

    //
    // Try and start the update/render thread
    //
    RunWithRenderTarget(
        [&] (CanvasSwapChain* target, ICanvasDevice*, Color const& clearColor, bool areResourcesCreated)
        {
            // The clearColor passed to us is ignored since this needs to be
            // checked on each tick of the update/render loop.
            UNREFERENCED_PARAMETER(clearColor);

            //
            // Process the results of the previous update/render loop.  We
            // do this here, within RunWithRenderTarget, so that it can
            // handle errors (such as lost device).
            //
            if (completedTickLoopInfo)
            {
                HRESULT hr;
                ThrowIfFailed(completedTickLoopInfo->get_ErrorCode(&hr));
                ThrowIfFailed(hr);
            }

            if (areResourcesCreated)
            {
                // Once resources have been created we can start the dispatcher,
                // since the game loop owns the device & resources at this
                // point.
                m_gameLoop->StartDispatcher();
            }

            if (!areResourcesCreated && !needsDraw)
            {
                //
                // If resources aren't created then there's no point
                // spinning up the update/render thread.
                //
                return;
            }

            m_gameLoop->StartTickLoop(target, areResourcesCreated);
        });
}

void CanvasAnimatedControl::CreateContentControl()
{
    auto adapter = GetAdapter();

    ComPtr<IUIElement> content;

    if (adapter->IsDesignModeEnabled())
    {
        m_designModeShape = adapter->CreateDesignModeShape();
        content = As<IUIElement>(m_designModeShape);
    }
    else
    {
        m_canvasSwapChainPanel = adapter->CreateCanvasSwapChainPanel();
        content = As<IUIElement>(m_canvasSwapChainPanel);
    }

    auto thisAsUserControl = As<IUserControl>(GetComposableBase());
    ThrowIfFailed(thisAsUserControl->put_Content(content.Get()));
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
            auto lock = Lock(m_sharedStateMutex);

            m_sharedState.PendingAsyncActions.insert(m_sharedState.PendingAsyncActions.begin(), actionIter+1, pendingActions.end());

            lock.unlock();

            ThrowHR(actionsResult);
        }
    }
}

void CanvasAnimatedControl::CancelAsyncActions()
{
    std::vector<ComPtr<AnimatedControlAsyncAction>> actions;

    auto lock = Lock(m_sharedStateMutex);
    std::swap(actions, m_sharedState.PendingAsyncActions);
    lock.unlock();

    for (auto& action : actions)
    {
        action->CancelAndFireCompletion();
    }
}

void CanvasAnimatedControl::OnGameLoopStarting()
{
    ThrowIfFailed(m_gameLoopStartingEventList.InvokeAll(this, nullptr));
}

void CanvasAnimatedControl::OnGameLoopStopped()
{
    ThrowIfFailed(m_gameLoopStoppedEventList.InvokeAll(this, nullptr));
}

bool CanvasAnimatedControl::Tick(
    CanvasSwapChain* swapChain, 
    bool areResourcesCreated)
{
    EventWrite_CanvasAnimatedControl_Tick_Start();
    auto tickEnd = MakeScopeWarden([] { EventWrite_CanvasAnimatedControl_Tick_Stop(); });

    RenderTarget* renderTarget = GetCurrentRenderTarget();

    if (IsSuspended())
        return false;

    if (!IsLoaded())
        return false;

    //
    // Access shared state that's shared between the UI thread and the
    // update/render thread.  This is done in one place in order to hold the
    // lock for as little time as possible.
    //

    Color clearColor;
    Size currentSize;
    float currentDpi;
    GetClearColorSizeAndDpi(&clearColor, &currentSize, &currentDpi);

    auto lock = Lock(m_sharedStateMutex);

    m_sharedState.SizeSeenByGameLoop = currentSize;
    m_sharedState.IsInTick = true;

    auto tickEnd2 = MakeScopeWarden(
        [&]
        {
            if (!lock.owns_lock())
                lock.lock();
            
            m_sharedState.IsInTick = false;
        });

    m_stepTimer.SetTargetElapsedTicks(m_sharedState.TargetElapsedTime);
    m_stepTimer.SetFixedTimeStep(m_sharedState.IsStepTimerFixedStep);

    if (m_sharedState.ShouldResetElapsedTime)
    {
        m_stepTimer.ResetElapsedTime();
    }

    bool deviceNeedsReCreationWithNewOptions = m_sharedState.DeviceNeedsReCreationWithNewOptions;
    m_sharedState.DeviceNeedsReCreationWithNewOptions = false;
    m_sharedState.ShouldResetElapsedTime = false;

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

    // Handle Paused state
    bool isPaused = m_sharedState.IsPaused;
    int64_t timeSpentPaused = 0;

    if (!isPaused)
    {
        timeSpentPaused = m_sharedState.TimeSpentPaused;

        if (areResourcesCreated)
        {
            m_sharedState.TimeWhenPausedWasSet = 0;
            m_sharedState.TimeSpentPaused = 0;
        }
    } 

    // We update, but forego drawing if the control is not in a visible state.
    // Force-draws, like those due to device lost, are not performed either.
    // Drawing behavior resumes when the control becomes visible once again.
    bool isVisible = IsVisible();

    //
    // Copy out the list of async actions, to avoid retaining the lock while
    // they are being fired.
    //
    // The async actions are only executed after resources have been created.
    // This means that:
    //
    // - the actions can assume that there's a valid device and
    // - there's no risk of them running concurrently with
    //   CreateResources.
    //
    std::vector<ComPtr<AnimatedControlAsyncAction>> pendingActions;
    
    if (areResourcesCreated)
        std::swap(pendingActions, m_sharedState.PendingAsyncActions);

    lock.unlock();

    //
    // Run any async actions
    //
    if (!pendingActions.empty())
    {
        IssueAsyncActions(pendingActions);

        // One of the async actions may have changed the shared state, in which case
        // we want to respond immediately.
        if (!invalidated)
        {
            auto lock2 = Lock(m_sharedStateMutex);
            
            invalidated = m_sharedState.Invalidated;
            if (areResourcesCreated)
                m_sharedState.Invalidated = false;
        }
    }

    //
    // Now do the update/render for this tick
    //

    UpdateResult updateResult{};

    EventWrite_CanvasAnimatedControl_Update_Start(areResourcesCreated, isPaused);
    if (areResourcesCreated && !isPaused)
    {
        bool forceUpdate = false;

        if (!m_hasUpdated)
        {
            // For the first update we reset the timer.  This handles the
            // possibility of there being a long delay between construction and
            // the first update.
            m_stepTimer.ResetElapsedTime();
            forceUpdate = true;
        }

        updateResult = Update(forceUpdate, timeSpentPaused);

        m_hasUpdated |= updateResult.Updated;
    }
    EventWrite_CanvasAnimatedControl_Update_Stop(updateResult.Updated);

    //
    // We only ever Draw/Present if an Update has actually happened.  This
    // results in us waiting until the next vblank to update.
    // This is desireable since using Present to wait for the vsync can
    // result in missed frames.
    //
    bool drew = false;
    if ((updateResult.Updated || forceDraw || invalidated) && isVisible)
    {
        bool zeroSizedTarget = currentSize.Width <= 0 || currentSize.Height <= 0;
        
        // A dpi change doesn't matter on a zero-sized target.
        bool dpiChangedOnNonZeroSizedTarget = renderTarget->Dpi != currentDpi && !zeroSizedTarget;

        bool sizeChanged = renderTarget->Size != currentSize;

        //
        // If the control's size or dpi has changed then the swapchain's buffers
        // need to be resized as appropriate.
        //
        if (sizeChanged || dpiChangedOnNonZeroSizedTarget)
        {
            if (zeroSizedTarget || !renderTarget->Target)
            {
                //
                // Switching between zero and non-zero sized rendertargets requires calling
                // CanvasSwapChainPanel::put_SwapChain, so must be done on the UI thread.
                // We must stop the update/render thread to allow this to happen.
                //
                return false;
            }
            else if (dpiChangedOnNonZeroSizedTarget)
            {
                //
                // A DPI change should stop and start the render thread, and
                // raise a CreateResources.
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
                ThrowIfFailed(renderTarget->Target->ResizeBuffersWithWidthAndHeightAndDpi(currentSize.Width, currentSize.Height, currentDpi));

                //
                // The size and dpi fields of the render target object represent the real, committed state of the render
                // target, while currentSize/currentDpi represent the thing last requested by the app.
                //
                renderTarget->Size = currentSize;
                renderTarget->Dpi = currentDpi;
            }
        }

        if (renderTarget->Target)
        {
            bool invokeDrawHandlers = (areResourcesCreated && (m_hasUpdated || invalidated));

            EventWrite_CanvasAnimatedControl_Draw_Start(invokeDrawHandlers, updateResult.IsRunningSlowly);
            Draw(renderTarget->Target.Get(), clearColor, invokeDrawHandlers, updateResult.IsRunningSlowly);
            EventWrite_CanvasAnimatedControl_Draw_Stop();
            EventWrite_CanvasAnimatedControl_Present_Start();            
            ThrowIfFailed(renderTarget->Target->Present());
            EventWrite_CanvasAnimatedControl_Present_Stop();

            drew = true;
        }
    }

    //
    // The call to Present() usually blocks until a previous frame has been
    // composed into the scene.  The happens because the swap chain has a
    // limited number of buffers available, and Present() won't return until
    // there's a buffer available to draw the next frame on.
    //
    // In some cases the Present() may return quickly.  For example, there may
    // already be a buffer free because the GPU is starting to catch up with the
    // GPU.  In fixed time step mode this will result in the next Tick happening
    // soon enough that there is no work to do, and so we don't do a
    // Draw/Present.
    //
    // Without the Present() call to block the CPU the game loop thread ends up
    // busy waiting, pegging a CPU, drawing more power and draining the battery
    // on a mobile device.  This is undesireable!
    //
    // To prevent this from happening we call WaitForVerticalBlank to delay the
    // next tick.
    //
    // Some caveats here:
    //
    //   - software devices do not support WaitForVerticalBlank. In this case
    //     the CanvasSwapChain does a Sleep(0).
    //
    //   - if there's no swap chain (eg the window is invisible) then we just
    //     sleep
    //
    if (!drew || !m_stepTimer.IsFixedTimeStep())
    {
        EventWrite_CanvasAnimatedControl_WaitForVerticalBlank_Start();
        if (swapChain)
        {
            ThrowIfFailed(swapChain->WaitForVerticalBlank());
        }
        else
        {
            GetAdapter()->Sleep(static_cast<DWORD>(StepTimer::TicksToMilliseconds(StepTimer::DefaultTargetElapsedTime)));
        }
        EventWrite_CanvasAnimatedControl_WaitForVerticalBlank_Stop();
    }
    
    return areResourcesCreated && !isPaused;
}

void CanvasAnimatedControl::OnTickLoopEnded()
{
    Changed(ChangeReason::Other);
}

CanvasAnimatedControl::UpdateResult CanvasAnimatedControl::Update(bool forceUpdate, int64_t timeSpentPaused)
{
    UpdateResult result{};

    m_stepTimer.Tick(forceUpdate, timeSpentPaused,
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
