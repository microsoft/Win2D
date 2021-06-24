// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

#ifndef WINDOWS_PHONE

#include "CanvasPrintDocumentAdapter.h"

ComPtr<ICanvasDevice> DefaultPrintDocumentAdapter::GetSharedDevice()
{
    return SharedDeviceState::GetInstance()->GetSharedDevice(false);
}

#ifdef WINUI3

ComPtr<IDispatcherQueue> DefaultPrintDocumentAdapter::GetDispatcherForCurrentThread()
{
    ComPtr<ABI::Microsoft::UI::Dispatching::IDispatcherQueueStatics> dispatcherQueueStatic;
    ThrowIfFailed(GetActivationFactory(HStringReference(RuntimeClass_Microsoft_UI_Dispatching_DispatcherQueue).Get(), &dispatcherQueueStatic));

    ComPtr<IDispatcherQueue> dispatcherQueue;
    ThrowIfFailed(dispatcherQueueStatic->GetForCurrentThread(&dispatcherQueue));

    return dispatcherQueue;
}

#else
ComPtr<ICoreDispatcher> DefaultPrintDocumentAdapter::GetDispatcherForCurrentThread()
{
    ComPtr<ICoreWindowStatic> coreWindowStatic;
    ThrowIfFailed(GetActivationFactory(HStringReference(RuntimeClass_Windows_UI_Core_CoreWindow).Get(), &coreWindowStatic));

    ComPtr<ICoreWindow> coreWindow;
    ThrowIfFailed(coreWindowStatic->GetForCurrentThread(&coreWindow));

    if (!coreWindow)
        return nullptr;

    ComPtr<ICoreDispatcher> coreDispatcher;
    ThrowIfFailed(coreWindow->get_Dispatcher(&coreDispatcher));

    return coreDispatcher;
}
#endif

float DefaultPrintDocumentAdapter::GetLogicalDpi()
{
    ComPtr<IDisplayInformationStatics> displayInformationStatics;

    ThrowIfFailed(GetActivationFactory(
        HStringReference(RuntimeClass_Windows_Graphics_Display_DisplayInformation).Get(),
        &displayInformationStatics));

    ComPtr<IDisplayInformation> displayInformation;
    HRESULT hr = displayInformationStatics->GetForCurrentView(&displayInformation);

    if (FAILED(hr))
    {
        // This might fail because we're not on the UI thread.  We have other
        // checks in place to ensure that we're on a UI thread, so we don't fail
        // here as well.
        return DEFAULT_DPI;
    }

    float logicalDpi;
    ThrowIfFailed(displayInformation->get_LogicalDpi(&logicalDpi));
    return logicalDpi;    
}


bool DefaultPrintDocumentAdapter::ShouldWaitForUIThread()
{
    return true;
}

#endif