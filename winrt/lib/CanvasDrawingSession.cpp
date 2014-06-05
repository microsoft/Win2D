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


    IFACEMETHODIMP CanvasDrawingSession::Clear(
        ABI::Windows::UI::Color color)
    {
        return ExceptionBoundary(
            [&]()
            {
                auto& deviceContext = m_deviceContext.EnsureNotClosed();

                auto d2dColor = ToD2DColor(color);
                deviceContext->Clear(&d2dColor);
            });
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawLine(
        ABI::Windows::Foundation::Point point0,
        ABI::Windows::Foundation::Point point1,
        ICanvasBrush* brush) 
    {
        return DrawLineWithStrokeWidth(
            point0,
            point1,
            brush,
            1.0f);
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawLineWithStrokeWidth(
        ABI::Windows::Foundation::Point point0,
        ABI::Windows::Foundation::Point point1,
        ICanvasBrush* brush,
        float strokeWidth) 
    {
        return ExceptionBoundary(
            [&]()
            {
                auto& deviceContext = m_deviceContext.EnsureNotClosed();
                CheckInPointer(brush);

                deviceContext->DrawLine(
                    ToD2DPoint(point0),
                    ToD2DPoint(point1),
                    ToD2DBrush(brush).Get(),
                    strokeWidth);
            });
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawRectangle(
        ABI::Windows::Foundation::Rect rect,
        ICanvasBrush* brush) 
    {
        return DrawRectangleWithStrokeWidth(
            rect,
            brush,
            1.0f);
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawRectangleWithStrokeWidth(
        ABI::Windows::Foundation::Rect rect,
        ICanvasBrush* brush,
        float strokeWidth) 
    {
        return ExceptionBoundary(
            [&]()
            {
                auto& deviceContext = m_deviceContext.EnsureNotClosed();
                CheckInPointer(brush);

                deviceContext->DrawRectangle(
                    ToD2DRect(rect),
                    ToD2DBrush(brush).Get(),
                    strokeWidth);
            });
    }


    IFACEMETHODIMP CanvasDrawingSession::FillRectangle(
        ABI::Windows::Foundation::Rect rect,
        ICanvasBrush* brush) 
    {
        return ExceptionBoundary(
            [&]()
            {
                auto& deviceContext = m_deviceContext.EnsureNotClosed();
                CheckInPointer(brush);

                deviceContext->FillRectangle(
                    ToD2DRect(rect),
                    ToD2DBrush(brush).Get());
            });
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawRoundedRectangle(
        CanvasRoundedRectangle roundedRectangle,
        ICanvasBrush* brush) 
    {
        return DrawRoundedRectangleWithStrokeWidth(
            roundedRectangle,
            brush,
            1.0f);
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawRoundedRectangleWithStrokeWidth(
        CanvasRoundedRectangle roundedRectangle,
        ICanvasBrush* brush,
        float strokeWidth) 
    {
        return ExceptionBoundary(
            [&]()
            {
                auto& deviceContext = m_deviceContext.EnsureNotClosed();
                CheckInPointer(brush);
                
                deviceContext->DrawRoundedRectangle(
                    ToD2DRoundedRect(roundedRectangle),
                    ToD2DBrush(brush).Get(),
                    strokeWidth);
            });
    }


    IFACEMETHODIMP CanvasDrawingSession::FillRoundedRectangle(
        CanvasRoundedRectangle roundedRectangle,
        ICanvasBrush* brush) 
    {
        return ExceptionBoundary(
            [&]()
            {
                auto& deviceContext = m_deviceContext.EnsureNotClosed();
                CheckInPointer(brush);

                deviceContext->FillRoundedRectangle(
                    ToD2DRoundedRect(roundedRectangle),
                    ToD2DBrush(brush).Get());
            });
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawEllipse(
        CanvasEllipse ellipse,
        ICanvasBrush* brush) 
    {
        return DrawEllipseWithStrokeWidth(
            ellipse,
            brush,
            1.0f);
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawEllipseWithStrokeWidth(
        CanvasEllipse ellipse,
        ICanvasBrush* brush,
        float strokeWidth) 
    {
        return ExceptionBoundary(
            [&]()
            {
                auto& deviceContext = m_deviceContext.EnsureNotClosed();
                CheckInPointer(brush);

                deviceContext->DrawEllipse(
                    ToD2DEllipse(&ellipse),
                    ToD2DBrush(brush).Get(),
                    strokeWidth);
            });
    }


    IFACEMETHODIMP CanvasDrawingSession::FillEllipse(
        CanvasEllipse ellipse,
        ICanvasBrush* brush) 
    {
        return ExceptionBoundary(
            [&]()
            {
                auto& deviceContext = m_deviceContext.EnsureNotClosed();
                CheckInPointer(brush);

                deviceContext->FillEllipse(
                    ToD2DEllipse(&ellipse),
                    ToD2DBrush(brush).Get());
            });
    }
}
