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

#include "MockDispatcher.h"

class StubDispatcher : public MockDispatcher
{
    typedef std::pair<ComPtr<IDispatchedHandler>, ComPtr<MockAsyncAction>> HandlerAndAction;

    std::queue<HandlerAndAction> m_pendingActions;
    bool m_stopped;
    std::function<void()> m_onStop;

public:
    StubDispatcher()
        : m_stopped(false)
    {
    }

    virtual IFACEMETHODIMP RunAsync(
        CoreDispatcherPriority priority,
        IDispatchedHandler *agileCallback,
        IAsyncAction **asyncAction) override
    {
        if (RunAsyncMethod.HasMock())
            return MockDispatcher::RunAsync(priority, agileCallback, asyncAction);

        return ExceptionBoundary(
            [&]
            {
                auto action = Make<MockAsyncAction>();
                m_pendingActions.push(std::make_pair(ComPtr<IDispatchedHandler>(agileCallback), action));
                ThrowIfFailed(action.CopyTo(asyncAction));
            });
    }

    void SetOnStop(std::function<void()> onStop)
    {
        assert(!m_onStop);
        m_onStop = onStop;
    }

    virtual IFACEMETHODIMP StopProcessEvents() override
    {
        if (StopProcessEventsMethod.HasMock())
            return MockDispatcher::StopProcessEvents();

        return ExceptionBoundary(
            [&]
            {
                // Process any remaining actions
                TickAll();

                m_stopped = true;

                // ...but don't process any more that were scheduled
                while (!m_pendingActions.empty())
                    m_pendingActions.pop();

                if (m_onStop)
                    m_onStop();
            });
    }

    void Tick()
    {
        Assert::IsFalse(m_stopped, L"Tick() can't be called after StopProcessEvents()");
        
        if (m_pendingActions.empty())
            return;

        auto nextAction = m_pendingActions.front();
        m_pendingActions.pop();
        Process(nextAction);
    }

    void TickAll()
    {
        Assert::IsFalse(m_stopped);

        // Execute all actions that are currently pending, but not any that are
        // queued as a result of executing one
        decltype(m_pendingActions) pendingActions;
        std::swap(m_pendingActions, pendingActions);

        while (!pendingActions.empty())
        {
            auto nextAction = pendingActions.front();
            pendingActions.pop();
            Process(nextAction);
        }
    }

    bool HasPendingActions()
    {
        return !m_pendingActions.empty();
    }

private:
    void Process(HandlerAndAction const& handlerAndAction)
    {
        HRESULT actionResult = handlerAndAction.first->Invoke();

        //
        // The real dispatcher will detect when an error hasn't been handled (eg
        // because there's no completion handler set).  Unhandled errors are
        // then passed to RoReportUnhandledError (ultimately, in a XAML app,
        // this raises the Application.UnhandledException event.
        //
        // We simulate this by making the Tick function throw the error code
        // immediately, which allows us to write tests that validate that
        // exceptions are bubbled out as expected - so if Tick() throws we know
        // that in real life RoReportUnhandledError would have been called.
        //
        bool unhandledError = false;
        if (FAILED(actionResult))
        {
            ComPtr<IAsyncActionCompletedHandler> handler;
            ThrowIfFailed(handlerAndAction.second->get_Completed(&handler));
            if (!handler)
                unhandledError = true;
        }

        handlerAndAction.second->SetResult(actionResult);

        if (unhandledError)
            ThrowHR(actionResult);
    }
};
