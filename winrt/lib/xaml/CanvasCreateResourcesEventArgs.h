// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace UI
{
    class CanvasCreateResourcesEventArgs : public RuntimeClass<ICanvasCreateResourcesEventArgs>,
                                           private LifespanTracker<CanvasCreateResourcesEventArgs>
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_UI_CanvasCreateResourcesEventArgs, BaseTrust);

        CanvasCreateResourcesReason m_reason;
        std::function<void(IAsyncAction*)> m_trackAsyncActionCallback;

    public:
        CanvasCreateResourcesEventArgs(CanvasCreateResourcesReason reason, std::function<void(IAsyncAction*)> trackAsyncActionCallback);

        IFACEMETHODIMP get_Reason(CanvasCreateResourcesReason* value) override;
        IFACEMETHODIMP TrackAsyncAction(IAsyncAction* action) override;
    };

} } } } }
