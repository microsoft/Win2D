// Copyright (c) Microsoft Corporation.  All rights reserved

#include "pch.h"

#include "CanvasDrawingSession.h"
#include "CanvasStrokeStyle.h"

namespace canvas
{
    inline ComPtr<ID2D1StrokeStyle1> ToD2DStrokeStyle(ICanvasStrokeStyle* strokeStyle, ID2D1DeviceContext* deviceContext)
    {
        if (!strokeStyle) return nullptr;

        ComPtr<ID2D1Factory> d2dBaseFactory;
        deviceContext->GetFactory(&d2dBaseFactory);

        ComPtr<ID2D1Factory2> d2dFactory;
        ThrowIfFailed(d2dBaseFactory.As(&d2dFactory));

        ComPtr<ICanvasStrokeStyleInternal> internal;
        ThrowIfFailed(strokeStyle->QueryInterface(internal.GetAddressOf()));

        return internal->GetRealizedD2DStrokeStyle(d2dFactory.Get());
    }
    
    IFACEMETHODIMP CanvasDrawingSessionFactory::GetOrCreate(
        IUnknown* resource,
        IInspectable** wrapper)
    {
        return ExceptionBoundary(
            [&]()
            {
                CheckInPointer(resource);
                CheckAndClearOutPointer(wrapper);

                ComPtr<ID2D1DeviceContext1> deviceContext;

                ThrowIfFailed(resource->QueryInterface(deviceContext.GetAddressOf()));

                auto newDrawingSession = GetManager()->GetOrCreate(deviceContext.Get());

                ThrowIfFailed(newDrawingSession.CopyTo(wrapper));
            });
    }


    //
    // This drawing session adapter is used when wrapping an existing
    // ID2D1DeviceContext.  In this wrapper, interop, case we don't want
    // CanvasDrawingSession to call any additional methods in the device context.
    //
    class NoopCanvasDrawingSessionAdapter : public ICanvasDrawingSessionAdapter
    {
    public:
        virtual void EndDraw() override
        {
            // nothing
        }
    };


    CanvasDrawingSessionManager::CanvasDrawingSessionManager()
        : m_adapter(std::make_shared<NoopCanvasDrawingSessionAdapter>())
    {
    }


    ComPtr<CanvasDrawingSession> CanvasDrawingSessionManager::CreateNew(
        ID2D1DeviceContext1* deviceContext,
        std::shared_ptr<ICanvasDrawingSessionAdapter> drawingSessionAdapter)
    {
        return Make<CanvasDrawingSession>(
            shared_from_this(),
            deviceContext, 
            drawingSessionAdapter);
    }


    ComPtr<CanvasDrawingSession> CanvasDrawingSessionManager::CreateWrapper(
        ID2D1DeviceContext1* resource)
    {
        auto drawingSession = Make<CanvasDrawingSession>(shared_from_this(), resource, m_adapter);
        CheckMakeResult(drawingSession);
        return drawingSession;
    }


    CanvasDrawingSession::CanvasDrawingSession(
        std::shared_ptr<CanvasDrawingSessionManager> manager,
        ID2D1DeviceContext1* deviceContext,
        std::shared_ptr<ICanvasDrawingSessionAdapter> adapter)
        : ResourceWrapper(manager, deviceContext)
        , m_adapter(adapter)
    {
        CheckInPointer(adapter.get());
    }


    CanvasDrawingSession::~CanvasDrawingSession()
    {
        // Ignore any errors when closing during destruction
        (void)Close();
    }


    IFACEMETHODIMP CanvasDrawingSession::Close()
    {
        // Base class Close() called outside of ExceptionBoundary since this
        // already has it's own boundary.
        HRESULT hr = ResourceWrapper::Close();
        if (FAILED(hr))
            return hr;

        return ExceptionBoundary(
            [&]()
            {
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
                auto& deviceContext = GetResource();

                auto d2dColor = ToD2DColor(color);
                deviceContext->Clear(&d2dColor);
            });
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawLine(
        ABI::Windows::Foundation::Point point0,
        ABI::Windows::Foundation::Point point1,
        ICanvasBrush* brush) 
    {
        return DrawLineWithStrokeWidthAndStrokeStyle(
            point0,
            point1,
            brush,
            1.0f,
            nullptr);
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawLineWithStrokeWidth(
        ABI::Windows::Foundation::Point point0,
        ABI::Windows::Foundation::Point point1,
        ICanvasBrush* brush,
        float strokeWidth) 
    {
        return DrawLineWithStrokeWidthAndStrokeStyle(
            point0,
            point1,
            brush,
            strokeWidth,
            nullptr);
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawLineWithStrokeWidthAndStrokeStyle(
        ABI::Windows::Foundation::Point point0,
        ABI::Windows::Foundation::Point point1,
        ICanvasBrush* brush,
        float strokeWidth,
        ICanvasStrokeStyle* strokeStyle)
    {
        return ExceptionBoundary(
            [&]()
        {
            auto& deviceContext = GetResource();
            CheckInPointer(brush);

            deviceContext->DrawLine(
                ToD2DPoint(point0),
                ToD2DPoint(point1),
                ToD2DBrush(brush).Get(),
                strokeWidth,
                ToD2DStrokeStyle(strokeStyle, deviceContext.Get()).Get());
        });
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawRectangle(
        ABI::Windows::Foundation::Rect rect,
        ICanvasBrush* brush) 
    {
        return DrawRectangleWithStrokeWidthAndStrokeStyle(
            rect,
            brush,
            1.0f,
            nullptr);
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawRectangleWithStrokeWidth(
        ABI::Windows::Foundation::Rect rect,
        ICanvasBrush* brush,
        float strokeWidth) 
    {
        return DrawRectangleWithStrokeWidthAndStrokeStyle(
            rect,
            brush,
            strokeWidth,
            nullptr);
    }

    IFACEMETHODIMP CanvasDrawingSession::DrawRectangleWithStrokeWidthAndStrokeStyle(
        ABI::Windows::Foundation::Rect rect,
        ICanvasBrush* brush,
        float strokeWidth,
        ICanvasStrokeStyle* strokeStyle)
    {
        return ExceptionBoundary(
            [&]()
        {
            auto& deviceContext = GetResource();
            CheckInPointer(brush);

            deviceContext->DrawRectangle(
                ToD2DRect(rect),
                ToD2DBrush(brush).Get(),
                strokeWidth,
                ToD2DStrokeStyle(strokeStyle, deviceContext.Get()).Get());
        });
    }


    IFACEMETHODIMP CanvasDrawingSession::FillRectangle(
        ABI::Windows::Foundation::Rect rect,
        ICanvasBrush* brush) 
    {
        return ExceptionBoundary(
            [&]()
            {
                auto& deviceContext = GetResource();
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
        return DrawRoundedRectangleWithStrokeWidthAndStrokeStyle(
            roundedRectangle,
            brush,
            1.0f,
            nullptr);
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawRoundedRectangleWithStrokeWidth(
        CanvasRoundedRectangle roundedRectangle,
        ICanvasBrush* brush,
        float strokeWidth) 
    {
        return DrawRoundedRectangleWithStrokeWidthAndStrokeStyle(
            roundedRectangle,
            brush,
            strokeWidth,
            nullptr);
    }

    IFACEMETHODIMP CanvasDrawingSession::DrawRoundedRectangleWithStrokeWidthAndStrokeStyle(
        CanvasRoundedRectangle roundedRectangle,
        ICanvasBrush* brush,
        float strokeWidth,
        ICanvasStrokeStyle* strokeStyle)
    {
        return ExceptionBoundary(
            [&]()
        {
            auto& deviceContext = GetResource();  
            CheckInPointer(brush);

            deviceContext->DrawRoundedRectangle(
                ToD2DRoundedRect(roundedRectangle),
                ToD2DBrush(brush).Get(),
                strokeWidth,
                ToD2DStrokeStyle(strokeStyle, deviceContext.Get()).Get());
        });
    }


    IFACEMETHODIMP CanvasDrawingSession::FillRoundedRectangle(
        CanvasRoundedRectangle roundedRectangle,
        ICanvasBrush* brush) 
    {
        return ExceptionBoundary(
            [&]()
            {
                auto& deviceContext = GetResource();
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
        return DrawEllipseWithStrokeWidthAndStrokeStyle(
            ellipse,
            brush,
            1.0f,
            nullptr);
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawEllipseWithStrokeWidth(
        CanvasEllipse ellipse,
        ICanvasBrush* brush,
        float strokeWidth) 
    {
        return DrawEllipseWithStrokeWidthAndStrokeStyle(
            ellipse,
            brush,
            strokeWidth,
            nullptr);
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawEllipseWithStrokeWidthAndStrokeStyle(
        CanvasEllipse ellipse,
        ICanvasBrush* brush,
        float strokeWidth,
        ICanvasStrokeStyle* strokeStyle)
    {
        return ExceptionBoundary(
            [&]()
        {
            auto& deviceContext = GetResource(); 
            CheckInPointer(brush);

            deviceContext->DrawEllipse(
                ReinterpretAs<D2D1_ELLIPSE*>(&ellipse),
                ToD2DBrush(brush).Get(),
                strokeWidth,
                ToD2DStrokeStyle(strokeStyle, deviceContext.Get()).Get());
        });
    }


    IFACEMETHODIMP CanvasDrawingSession::FillEllipse(
        CanvasEllipse ellipse,
        ICanvasBrush* brush) 
    {
        return ExceptionBoundary(
            [&]()
            {
                auto& deviceContext = GetResource();
                CheckInPointer(brush);

                deviceContext->FillEllipse(
                    ReinterpretAs<D2D1_ELLIPSE*>(&ellipse),
                    ToD2DBrush(brush).Get());
            });
    }

    ActivatableStaticOnlyFactory(CanvasDrawingSessionFactory);
}
