// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

#include "CanvasCreateResourcesEventArgs.h"

using namespace ABI::Microsoft::Graphics::Canvas::UI;
using namespace ABI::Windows::Foundation;

IFACEMETHODIMP CanvasCreateResourcesEventArgsFactory::Create(
    CanvasCreateResourcesReason reason,
    ICanvasCreateResourcesEventArgs** createResourcesEventArgs)
{
    return ExceptionBoundary(
        [&]
        {
            CheckAndClearOutPointer(createResourcesEventArgs);

            auto newCreateResourcesEventArgs = Make<CanvasCreateResourcesEventArgs>(reason, nullptr);
            CheckMakeResult(newCreateResourcesEventArgs);

            ThrowIfFailed(newCreateResourcesEventArgs.CopyTo(createResourcesEventArgs));
        });
}

CanvasCreateResourcesEventArgs::CanvasCreateResourcesEventArgs(CanvasCreateResourcesReason reason, std::function<void(IAsyncAction*)> trackAsyncActionCallback)
    : m_reason(reason)
    , m_trackAsyncActionCallback(trackAsyncActionCallback)
{
}


IFACEMETHODIMP CanvasCreateResourcesEventArgs::get_Reason(CanvasCreateResourcesReason* value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(value);
            *value = m_reason;
        });
}


IFACEMETHODIMP CanvasCreateResourcesEventArgs::TrackAsyncAction(IAsyncAction* action)
{
    return ExceptionBoundary(
        [=]
        {
            if (m_trackAsyncActionCallback)
            {
                m_trackAsyncActionCallback(action);
            }

            if (m_trackedAction)
            {
                ThrowHR(E_FAIL, Strings::MultipleAsyncCreateResourcesNotSupported);
            }

            m_trackedAction = action;
        });
}

IFACEMETHODIMP CanvasCreateResourcesEventArgs::GetTrackedAction(IAsyncAction** action)
{
    return ExceptionBoundary(
        [=]
        {
            CheckAndClearOutPointer(action);

            if (m_trackedAction)
            {
                ThrowIfFailed(m_trackedAction.CopyTo(action));
            }
        });
}

ActivatableClassWithFactory(CanvasCreateResourcesEventArgs, CanvasCreateResourcesEventArgsFactory);