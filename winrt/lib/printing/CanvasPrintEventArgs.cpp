// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

#include "CanvasPrintEventArgs.h"

using namespace ABI::Microsoft::Graphics::Canvas::Printing;

CanvasPrintEventArgs::CanvasPrintEventArgs(
    ComPtr<ICanvasDevice> const& device,
    ComPtr<IPrintDocumentPackageTarget> const& target,
    ComPtr<IPrintTaskOptionsCore> const& printTaskOptions,
    float initialDpi)
    : m_device(device)
    , m_target(target)
    , m_printTaskOptions(printTaskOptions)
    , m_dpi(initialDpi)
    , m_currentPage(0)
{
}


IFACEMETHODIMP CanvasPrintEventArgs::get_PrintTaskOptions(IPrintTaskOptionsCore** value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckAndClearOutPointer(value);
                    
            ThrowIfFailed(m_printTaskOptions.CopyTo(value));
        });
}


IFACEMETHODIMP CanvasPrintEventArgs::get_Dpi(float* value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(value);
            *value = m_dpi;
        });
}


IFACEMETHODIMP CanvasPrintEventArgs::put_Dpi(float value)
{
    return ExceptionBoundary(
        [&]
        {
            if (m_printControl)
                ThrowHR(E_FAIL, Strings::CanvasPrintEventArgsDpiCannotBeChangedAfterCreateDrawingSession);
                    
            if (value <= 0.0f)
                ThrowHR(E_INVALIDARG);

            m_dpi = value;
        });
}
        

IFACEMETHODIMP CanvasPrintEventArgs::GetDeferral(ICanvasPrintDeferral** value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckAndClearOutPointer(value);
            ThrowHR(E_NOTIMPL);
        });
}


IFACEMETHODIMP CanvasPrintEventArgs::CreateDrawingSession(ICanvasDrawingSession** value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckAndClearOutPointer(value);

            auto ds = CreateDrawingSessionImpl();
            ThrowIfFailed(ds.CopyTo(value));
        });
}


void CanvasPrintEventArgs::EndPrinting()
{
    if (m_printControl)
        ThrowIfFailed(m_printControl->Close());
}


class CanvasPrintEventArgs::DrawingSessionAdapter : public SimpleCanvasDrawingSessionAdapter
{
    ComPtr<CanvasPrintEventArgs> m_args;
                
public:
    DrawingSessionAdapter(ID2D1DeviceContext1* d2dDeviceContext, CanvasPrintEventArgs* args)
        : SimpleCanvasDrawingSessionAdapter(d2dDeviceContext)
        , m_args(args)
    {
    }

    virtual void EndDraw() override
    {
        __super::EndDraw();
        m_args->DrawingSessionClosed();
    }
};


ComPtr<ICanvasDrawingSession> CanvasPrintEventArgs::CreateDrawingSessionImpl()
{
    auto deviceInternal = As<ICanvasDeviceInternal>(m_device);
            
    if (!m_printControl)
    {
        m_printControl = deviceInternal->CreatePrintControl(m_target.Get(), m_dpi);
    }

    //
    // Ultimately we need to draw to a D2D command list that we can pass
    // to the print control's AddPage().  We call AddPage() when the
    // command list is closed.  CanvasCommandList doesn't provide any
    // way for us to do this, so we'll work directly with D2D command
    // list and create our own drawing session on top of it.
    //

    if (m_currentCommandList)
        ThrowHR(E_FAIL, Strings::CanvasPrintEventArgsCannotCreateDrawingSessionUntilPreviousOneClosed);

    auto d2dCommandList = deviceInternal->CreateCommandList();
    auto d2dDeviceContext = deviceInternal->CreateDeviceContextForDrawingSession();
    d2dDeviceContext->SetTarget(d2dCommandList.Get());
    d2dDeviceContext->SetDpi(m_dpi, m_dpi);

    auto adapter = std::make_shared<DrawingSessionAdapter>(d2dDeviceContext.Get(), this);
    auto ds = CanvasDrawingSession::CreateNew(d2dDeviceContext.Get(), adapter, m_device.Get());

    m_currentCommandList = d2dCommandList;
    m_currentPage++;

    return ds;
}


// TODO #5645: this could run on any thread (fix once we implement deferrals)
void CanvasPrintEventArgs::DrawingSessionClosed()
{
    assert(m_currentPage > 0);
    assert(m_currentCommandList);

    ThrowIfFailed(m_currentCommandList->Close());
            
    PrintPageDescription desc;
    ThrowIfFailed(m_printTaskOptions->GetPageDescription(m_currentPage, &desc));

    auto pageSize = D2D1_SIZE_F{ desc.PageSize.Width, desc.PageSize.Height };

    ThrowIfFailed(m_printControl->AddPage(m_currentCommandList.Get(), pageSize, nullptr, nullptr, nullptr));

    m_currentCommandList.Reset();
}
