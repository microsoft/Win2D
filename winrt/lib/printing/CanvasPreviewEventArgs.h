// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Printing
{
    using namespace ABI::Windows::Graphics::Printing;

    class CanvasPreviewEventArgs
        : public RuntimeClass<ICanvasPreviewEventArgs>
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_Printing_CanvasPreviewEventArgs, BaseTrust);

        uint32_t m_pageNumber;
        ComPtr<IPrintTaskOptionsCore> m_printTaskOptions;
        ComPtr<ICanvasDrawingSession> m_drawingSession;

    public:
        CanvasPreviewEventArgs(
            uint32_t pageNumber,
            ComPtr<IPrintTaskOptionsCore> const& printTaskOptions,
            ComPtr<ICanvasDrawingSession> const& drawingSession)
            : m_pageNumber(pageNumber)              
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
                    ThrowHR(E_NOTIMPL);
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
