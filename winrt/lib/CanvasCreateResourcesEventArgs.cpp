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

#include "pch.h"

#include "CanvasCreateResourcesEventArgs.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas
{

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

} } } }
