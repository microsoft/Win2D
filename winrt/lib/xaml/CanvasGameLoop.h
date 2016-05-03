// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#include "GameLoopThread.h"

using namespace ABI::Microsoft::Graphics::Canvas;

class ICanvasGameLoopClient
{
public:
    virtual ~ICanvasGameLoopClient() = default;

    virtual void OnGameLoopStarting() = 0;
    virtual void OnGameLoopStopped() = 0;

    virtual bool Tick(CanvasSwapChain* target, bool areResourcesCreated) = 0;
    virtual void OnTickLoopEnded() = 0;
};


class CanvasGameLoop : private LifespanTracker<CanvasGameLoop>
{
    std::recursive_mutex m_mutex;

    // The client (ie the CanvasAnimatedControl) owns the CanvasGameLoop,
    // and we ensure that this object never outlives the clietn, so we can
    // hold a raw pointer here.
    ICanvasGameLoopClient* m_client;

    // This is set by StartTickLoop, and is Reset when the tick loop decides
    // that it has finished running.
    ComPtr<IAsyncAction> m_tickLoopAction;

    // This is set when the tick loop finishes running and is used to query
    // the status (ie to marshal exceptions from the tick loop to the ui
    // thread).  It is reset by StartTickLoop.
    ComPtr<IAsyncInfo> m_completedTickLoopInfo;
        
    ComPtr<IDispatchedHandler> m_tickHandler;
    ComPtr<IAsyncActionCompletedHandler> m_tickCompletedHandler;

    ComPtr<CanvasSwapChain> m_target;
    bool m_areResourcesCreated;
    bool m_tickLoopShouldContinue;

    std::unique_ptr<IGameLoopThread> m_gameLoopThread;

public:
    CanvasGameLoop(
        ICanvasGameLoopClient* client,
        std::unique_ptr<IGameLoopThread> gameLoopThread);

    ~CanvasGameLoop();

    void StartTickLoop(
        CanvasSwapChain* target,
        bool areResourcesCreated);

    void TakeTickLoopState(bool* isRunning, ComPtr<IAsyncInfo>* completedTickLoopState);

    void StartDispatcher();
    void StopDispatcher();
    bool HasThreadAccess();

private:
    void Tick();
    void TickCompleted(IAsyncAction* action, AsyncStatus status);

    void ScheduleTick(RecursiveLock const& lock);
    void EndTickLoop(RecursiveLock const& lock);
};
