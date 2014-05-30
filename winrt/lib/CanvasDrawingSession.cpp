// Copyright (c) Microsoft Corporation.  All rights reserved

#include "pch.h"

#include "CanvasDrawingSession.h"

namespace canvas
{
    CanvasDrawingSession::CanvasDrawingSession(
        ID2D1DeviceContext1* deviceContext,
        std::shared_ptr<ICanvasDrawingSessionAdapter> adapter)
        : m_deviceContext(deviceContext)
        , m_adapter(adapter)
    {
        CheckInPointer(deviceContext);
        CheckInPointer(adapter.get());
    }


    CanvasDrawingSession::~CanvasDrawingSession()
    {
        try
        {
            if (m_adapter)
                m_adapter->EndDraw();
        }
        catch (...)
        {
            // Ignore any exceptions thrown during destruction.

            //
            // TODO: should it be an error for the destructor to be called on
            // something that hasn't been closed?  I'm pretty sure that this
            // would always be an indication of an error.  Maybe something to
            // warn about somehow?
            //
        }
    }


    IFACEMETHODIMP CanvasDrawingSession::Close()
    {
        return ExceptionBoundary(
            [&]()
            {
                m_deviceContext.Close();

                if (m_adapter)
                {
                    // Arrange it so that m_adapter will always get
                    // reset, even if EndDraw throws.
                    auto adapter = m_adapter;
                    m_adapter.reset();

                    adapter->EndDraw();
                }        
            });
    }


    IFACEMETHODIMP CanvasDrawingSession::Clear(ABI::Windows::UI::Color color)
    {
        return ExceptionBoundary(
            [&]()
            {
                auto& deviceContext = m_deviceContext.EnsureNotClosed();

                auto d2dColor = WindowsColorToD2DColor(color);
                deviceContext->Clear(&d2dColor);
            });
    }
}
