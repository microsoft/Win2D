// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#include "DeferrableTask.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Printing
{
    using namespace ABI::Windows::Graphics::Printing;

    class CanvasPreviewEventArgs
        : public RuntimeClass<ICanvasPreviewEventArgs>
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_Printing_CanvasPreviewEventArgs, BaseTrust);

        DeferrableTaskPtr const m_task;

        uint32_t const m_pageNumber;
        ComPtr<IPrintTaskOptionsCore> const m_printTaskOptions;
        ComPtr<ICanvasDrawingSession> const m_drawingSession;

    public:
        CanvasPreviewEventArgs(            
            DeferrableTaskPtr task,
            uint32_t pageNumber,
            ComPtr<IPrintTaskOptionsCore> const& printTaskOptions,
            ComPtr<ICanvasDrawingSession> const& drawingSession)
            : m_task(task)
            , m_pageNumber(pageNumber)              
            , m_printTaskOptions(printTaskOptions)
            , m_drawingSession(drawingSession)
        {
        }
        
        IFACEMETHODIMP get_PageNumber(uint32_t* value) override
        {
            return ExceptionBoundary(
                [&]
                {
                    CheckInPointer(value);
                    *value = m_pageNumber;
                });
        }
        
        IFACEMETHODIMP get_PrintTaskOptions(IPrintTaskOptionsCore** value) override
        {
            return ExceptionBoundary(
                [&]
                {
                    CheckAndClearOutPointer(value);
                    ThrowIfFailed(m_printTaskOptions.CopyTo(value));
                });
        }
        
        IFACEMETHODIMP GetDeferral(ICanvasPrintDeferral** value) override
        {
            return ExceptionBoundary(
                [&]
                {
                    CheckAndClearOutPointer(value);

                    auto deferral = m_task->GetDeferral();

                    ThrowIfFailed(deferral.CopyTo(value));
                });
        }
        
        IFACEMETHODIMP get_DrawingSession(ICanvasDrawingSession** value) override
        {
            return ExceptionBoundary(
                [&]
                {
                    CheckAndClearOutPointer(value);
                    ThrowIfFailed(m_drawingSession.CopyTo(value));
                });
        }        
    };
    
} } } } }
