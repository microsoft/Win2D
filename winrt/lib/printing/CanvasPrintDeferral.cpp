// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

#ifndef WINDOWS_PHONE

#include "CanvasPrintDeferral.h"
#include "DeferrableTask.h"

using namespace ABI::Microsoft::Graphics::Canvas::Printing;

CanvasPrintDeferral::CanvasPrintDeferral(DeferrableTask* task)
    : m_task(task)
{
    assert(m_task);
}


IFACEMETHODIMP CanvasPrintDeferral::Complete()
{
    return ExceptionBoundary(
        [&]
        {
            Lock lock(m_mutex);
            
            if (!m_task)
                ThrowHR(E_FAIL, Strings::CanvasPrintDocumentDeferralCompleteMayOnlyBeCalledOnce);
                
            m_task->DeferredComplete();
            m_task = nullptr;
        });
}

#endif
