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
    std::mutex m_mutex;

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

    void ScheduleTick(Lock const& lock);
    void EndTickLoop(Lock const& lock);
};
