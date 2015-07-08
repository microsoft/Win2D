// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

#include "CanvasCreateResourcesEventArgs.h"

using namespace ABI::Microsoft::Graphics::Canvas::UI;
using namespace ABI::Windows::Foundation;

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
            m_trackAsyncActionCallback(action);
        });
}


