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

CanvasGameLoop::CanvasGameLoop(ComPtr<IAsyncAction>&& action, ComPtr<ICoreDispatcher>&& dispatcher)
    : m_threadAction(std::move(action))
    , m_dispatcher(std::move(dispatcher))
{ 
}

CanvasGameLoop::~CanvasGameLoop()
{
    // Kill the game thread by stopping the dispatcher
    As<ICoreDispatcherWithTaskPriority>(m_dispatcher)->StopProcessEvents();
}

void CanvasGameLoop::StartTickLoop(
    CanvasAnimatedControl* control,
    std::function<bool(CanvasAnimatedControl*)> const& tickFn,
    std::function<void(CanvasAnimatedControl*)> const& completedFn)
{
    auto lock = Lock(m_mutex);

    assert(!m_tickLoopAction);
    m_completedTickLoopInfo.Reset();

    std::weak_ptr<CanvasGameLoop> weakSelf(shared_from_this());
    auto weakControl = AsWeak(control);

    m_tickHandler = Callback<AddFtmBase<IDispatchedHandler>::Type>(
        [weakSelf, weakControl, tickFn] () mutable
        {
            return ExceptionBoundary(
                [&]
                {
                    auto self = weakSelf.lock();

                    if (!self)
                        return;

                    self->m_tickLoopShouldContinue = false;

                    auto strongControl = LockWeakRef<ICanvasAnimatedControl>(weakControl);
                    auto control = static_cast<CanvasAnimatedControl*>(strongControl.Get());

                    if (!control)
                        return;

                    self->m_tickLoopShouldContinue = tickFn(control);
                });
        });
    CheckMakeResult(m_tickHandler);

    m_tickCompletedHandler = Callback<AddFtmBase<IAsyncActionCompletedHandler>::Type>(
        [weakSelf, weakControl, completedFn] (IAsyncAction* action, AsyncStatus status) mutable
        {
#ifdef NDEBUG
            UNREFERENCED_PARAMETER(action);
            UNREFERENCED_PARAMETER(status);
#endif

            return ExceptionBoundary(
                [&] 
                {
                    auto self = weakSelf.lock();

                    if (!self)
                        return;

                    auto strongControl = LockWeakRef<ICanvasAnimatedControl>(weakControl);
                    auto control = static_cast<CanvasAnimatedControl*>(strongControl.Get());
                
                    auto lock = Lock(self->m_mutex);

                    if (self->m_tickLoopShouldContinue)
                    {
                        assert(status == AsyncStatus::Completed);

                        self->ScheduleTick(lock);
                    }
                    else
                    {
                        assert(action == self->m_tickLoopAction.Get());
                        
                        self->EndTickLoop(lock);
                        completedFn(control);
                    }
                });
        });
    CheckMakeResult(m_tickCompletedHandler);

    ScheduleTick(lock);
}

void CanvasGameLoop::ScheduleTick(Lock const& lock)
{
    MustOwnLock(lock);

    ThrowIfFailed(m_dispatcher->RunAsync(CoreDispatcherPriority_Normal, m_tickHandler.Get(), &m_tickLoopAction));
    ThrowIfFailed(m_tickLoopAction->put_Completed(m_tickCompletedHandler.Get()));
    
}

void CanvasGameLoop::EndTickLoop(Lock const& lock)
{
    MustOwnLock(lock);

    m_completedTickLoopInfo = As<IAsyncInfo>(m_tickLoopAction);
    m_tickLoopAction.Reset();
}

void CanvasGameLoop::TakeTickLoopState(bool* isRunning, ComPtr<IAsyncInfo>* completedTickLoopState)
{
    auto lock = Lock(m_mutex);
    *isRunning = static_cast<bool>(m_tickLoopAction);
    *completedTickLoopState = m_completedTickLoopInfo;
    m_completedTickLoopInfo.Reset();
}


bool CanvasGameLoop::HasThreadAccess()
{
    boolean hasThreadAccess;
    ThrowIfFailed(m_dispatcher->get_HasThreadAccess(&hasThreadAccess));
    return !!hasThreadAccess;
}
