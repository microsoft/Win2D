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

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace UI { namespace Xaml
{
    class CanvasAnimatedControl;

    class CanvasGameLoop : public std::enable_shared_from_this<CanvasGameLoop>
    {
        std::mutex m_mutex;

        ComPtr<IAsyncAction> m_threadAction;
        ComPtr<ICoreDispatcher> m_dispatcher;

        ComPtr<IAsyncAction> m_tickLoopAction;
        
        ComPtr<IDispatchedHandler> m_tickHandler;
        ComPtr<IAsyncActionCompletedHandler> m_tickCompletedHandler;
        bool m_tickLoopShouldContinue;

    public:
        CanvasGameLoop(ComPtr<IAsyncAction>&& action, ComPtr<ICoreDispatcher>&& dispatcher, ComPtr<AnimatedControlInput> input);

        ~CanvasGameLoop();

        void StartTickLoop(
            CanvasAnimatedControl* control,
            std::function<bool(CanvasAnimatedControl*)> const& tickFn,
            std::function<void(CanvasAnimatedControl*)> const& completedFn);

        void TakeTickLoopState(bool* isRunning, ComPtr<IAsyncInfo>* asyncInfo);

    private:
        void ScheduleTick(Lock const& lock);
    };

} } } } } }
