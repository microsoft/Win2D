// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#include "CanvasPrintDeferral.h"
#include "DeferrableTask.h"

class DeferrableTaskScheduler
    : private LifespanTracker<DeferrableTaskScheduler>
{
    ComPtr<ICoreDispatcher> const m_dispatcher;

    std::mutex m_mutex;
    std::unique_ptr<DeferrableTask> m_currentTask;
    std::queue<std::unique_ptr<DeferrableTask>> m_pending;
    
public:    
    explicit DeferrableTaskScheduler(ComPtr<ICoreDispatcher> const& dispatcher)
        : m_dispatcher(dispatcher)
    {
    }

    std::unique_ptr<DeferrableTask> CreateTask(DeferrableFn&& fn)
    {
        return std::make_unique<DeferrableTask>(this, std::move(fn));
    }

    void Schedule(std::unique_ptr<DeferrableTask> task)
    {
        Lock lock(m_mutex);

        if (m_currentTask)
        {
            m_pending.push(std::move(task));
        }
        else
        {
            RunAsync(std::move(task));
        }
    }

    void DeferredTaskCompleted(DeferrableTask* task)
    {
        assert(task == m_currentTask.get());
        
        // Deferred completed tasks we dispatch via the dispatcher
        auto handler = Callback<AddFtmBase<IDispatchedHandler>::Type>(
            [task]() mutable
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

    void TaskCompleted(DeferrableTask* task)
    {
        UNREFERENCED_PARAMETER(task); // task is only referenced in an assert

        Lock lock(m_mutex);

        assert(m_currentTask.get() == task);
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
    void RunAsync(std::unique_ptr<DeferrableTask> task)
    {
        assert(!m_currentTask);
        m_currentTask = std::move(task);

        DeferrableTask* t = m_currentTask.get();
        
        auto handler = Callback<AddFtmBase<IDispatchedHandler>::Type>(
            [t]() mutable
            {
                return ExceptionBoundary(
                    [&]
                    {
                        t->Invoke();
                    });
            });
        CheckMakeResult(handler);
        
        ComPtr<IAsyncAction> asyncAction;
        ThrowIfFailed(m_dispatcher->RunAsync(CoreDispatcherPriority_Normal, handler.Get(), &asyncAction));
    }
    
    
    DeferrableTaskScheduler(DeferrableTaskScheduler const&) = delete;
    DeferrableTaskScheduler& operator=(DeferrableTaskScheduler const&) = delete;    
};
