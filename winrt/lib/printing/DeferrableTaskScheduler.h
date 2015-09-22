// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#include "CanvasPrintDeferral.h"
#include "DeferrableTask.h"

class DeferrableTaskScheduler
{
    ComPtr<ICoreDispatcher> const m_dispatcher;

    std::mutex m_mutex;
    DeferrableTaskPtr m_currentTask;
    std::queue<DeferrableTaskPtr> m_pending;
    
public:    
    explicit DeferrableTaskScheduler(ComPtr<ICoreDispatcher> const& dispatcher)
        : m_dispatcher(dispatcher)
    {
    }

    DeferrableTaskPtr CreateTask(DeferrableFn&& fn)
    {
        return std::make_shared<DeferrableTask>(this, std::move(fn));
    }

    void Schedule(DeferrableTaskPtr task)
    {
        Lock lock(m_mutex);

        if (m_currentTask)
        {
            m_pending.push(task);
        }
        else
        {
            RunAsync(task);
        }
    }

    void DeferredTaskCompleted(DeferrableTaskPtr task)
    {
        // Deferred completed tasks we dispatch via the dispatcher
        auto handler = Callback<AddFtmBase<IDispatchedHandler>::Type>(
            [task] () mutable
            {
                return ExceptionBoundary(
                    [&]
                    {
                        task->Completed();
                    });
            });
        CheckMakeResult(handler);

        ComPtr<IAsyncAction> asyncAction;
        ThrowIfFailed(m_dispatcher->RunAsync(CoreDispatcherPriority_Normal, handler.Get(), &asyncAction));
    }

    void TaskCompleted(DeferrableTaskPtr task)
    {
        Lock lock(m_mutex);
        assert(m_currentTask == task);
        m_currentTask.reset();

        if (!m_pending.empty())
        {
            // It would be possible to immediately execute the pending task,
            // rather than go via the dispatcher.  However, this complicates the
            // code for a minimal (to non-existent) perf gain.
            RunAsync(std::move(m_pending.front()));
            m_pending.pop();
        }
    }


private:
    void RunAsync(DeferrableTaskPtr task)
    {
        assert(!m_currentTask);
        m_currentTask = task;
        
        auto handler = Callback<AddFtmBase<IDispatchedHandler>::Type>(
            [task] () mutable
            {
                return ExceptionBoundary(
                    [&]
                    {
                        task->Invoke();
                    });
            });
        CheckMakeResult(handler);
        
        ComPtr<IAsyncAction> asyncAction;
        ThrowIfFailed(m_dispatcher->RunAsync(CoreDispatcherPriority_Normal, handler.Get(), &asyncAction));
    }
    
    
    DeferrableTaskScheduler(DeferrableTaskScheduler const&) = delete;
    DeferrableTaskScheduler& operator=(DeferrableTaskScheduler const&) = delete;    
};
