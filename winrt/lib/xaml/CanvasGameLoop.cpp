// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

#include "CanvasGameLoop.h"

using namespace ::ABI::Microsoft::Graphics::Canvas;
using namespace ::ABI::Microsoft::Graphics::Canvas::UI;
using namespace ::ABI::Microsoft::Graphics::Canvas::UI::Xaml;
using namespace ::Microsoft::WRL::Wrappers;

CanvasGameLoop::CanvasGameLoop(
    ICanvasGameLoopClient* client,
    std::unique_ptr<IGameLoopThread> gameLoopThread)
    : m_client(client)
    , m_gameLoopThread(std::move(gameLoopThread))
{ 
    m_tickHandler = Callback<AddFtmBase<IDispatchedHandler>::Type>(
        [this]
        {
            return ExceptionBoundary([&] { Tick(); });
        });
    CheckMakeResult(m_tickHandler);

    m_tickCompletedHandler = Callback<AddFtmBase<IAsyncActionCompletedHandler>::Type>(
        [this] (IAsyncAction* action, AsyncStatus status)
        {
            return ExceptionBoundary([&] { TickCompleted(action, status); });
        });
    CheckMakeResult(m_tickCompletedHandler);
}

CanvasGameLoop::~CanvasGameLoop()
{
}

void CanvasGameLoop::Tick()
{
    m_tickLoopShouldContinue = false;
    m_tickLoopShouldContinue = m_client->Tick(m_target.Get(), m_areResourcesCreated);
}

void CanvasGameLoop::TickCompleted(IAsyncAction* action, AsyncStatus status)
{
#ifdef NDEBUG
    UNREFERENCED_PARAMETER(action);
#endif
    assert(action == m_tickLoopAction.Get());

    // This is called when the Tick completes, even if Tick threw an exception.
    
    auto lock = Lock(m_mutex);
    
    if (m_tickLoopShouldContinue && status == AsyncStatus::Completed)
    {
        ScheduleTick(lock);
    }
    else
    {
        EndTickLoop(lock);
        m_client->OnTickLoopEnded();
    }
}

void CanvasGameLoop::StartTickLoop(
    CanvasSwapChain* target,
    bool areResourcesCreated)
{
    auto lock = Lock(m_mutex);

    assert(!m_tickLoopAction);
    m_completedTickLoopInfo.Reset();

    m_target = target;
    m_areResourcesCreated = areResourcesCreated;

    ScheduleTick(lock);
}

void CanvasGameLoop::ScheduleTick(Lock const& lock)
{
    MustOwnLock(lock);

    m_tickLoopAction = m_gameLoopThread->RunAsync(m_tickHandler.Get());
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

void CanvasGameLoop::StartDispatcher()
{
    return m_gameLoopThread->StartDispatcher();
}

void CanvasGameLoop::StopDispatcher()
{
    return m_gameLoopThread->StopDispatcher();
}

bool CanvasGameLoop::HasThreadAccess()
{
    return m_gameLoopThread->HasThreadAccess();
}
