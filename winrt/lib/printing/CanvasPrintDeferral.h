// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

class DeferrableTask;

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Printing
{

    class CanvasPrintDeferral
        : public RuntimeClass<ICanvasPrintDeferral>
        , private LifespanTracker<CanvasPrintDeferral>
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_Printing_CanvasPrintDeferral, BaseTrust);

        std::mutex m_mutex;
        DeferrableTask* m_task;
        
    public:
        CanvasPrintDeferral(DeferrableTask* task);

        IFACEMETHODIMP Complete() override;
    };
    
} } } } }
