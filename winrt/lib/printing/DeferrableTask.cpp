// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

#include "DeferrableTask.h"

#include "DeferrableTaskScheduler.h"


DeferrableTask::DeferrableTask(DeferrableTaskScheduler* owner, DeferrableFn fn)
    : m_owner(owner)
    , m_deferred(false)
    , m_code(fn)
{
    assert(m_owner);
}


void DeferrableTask::Invoke()
{
    try
    {
        m_code(this);
    }
    catch (...)
    {
        m_promise.set_exception(std::current_exception());
        m_owner->TaskCompleted(this);
    }
}


void DeferrableTask::SetCompletionFn(std::function<void()>&& fn)
{
    assert(!m_completionFn);
    m_completionFn = fn;
}


void DeferrableTask::NonDeferredComplete()
{
    if (!m_deferred)
    {
        Completed();
    }
}


void DeferrableTask::Completed()
{
    try
    {
        m_completionFn();
        m_promise.set_value();
    }
    catch (...)
    {
        m_promise.set_exception(std::current_exception());
    }
    m_owner->TaskCompleted(this);
}


void DeferrableTask::DeferredComplete()
{
    assert(m_deferred);
    m_owner->DeferredTaskCompleted(this);
}


ComPtr<CanvasPrintDeferral> DeferrableTask::GetDeferral()
{
    if (m_deferred)
        ThrowHR(E_FAIL, Strings::CanvasPrintDocumentGetDeferralMayOnlyBeCalledOnce);
    
    auto deferral = Make<CanvasPrintDeferral>(this);
    CheckMakeResult(deferral);

    m_deferred = true;

    return deferral;
}


std::future<void> DeferrableTask::GetFuture()
{
    return m_promise.get_future();
}
