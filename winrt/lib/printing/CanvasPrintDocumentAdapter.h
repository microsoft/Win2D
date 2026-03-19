// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

class DefaultPrintDocumentAdapter;

class CanvasPrintDocumentAdapter : public Singleton<CanvasPrintDocumentAdapter, DefaultPrintDocumentAdapter>
{
public:
    virtual ~CanvasPrintDocumentAdapter() = default;

    virtual ComPtr<ICanvasDevice> GetSharedDevice() = 0;
#ifdef WINUI3
    virtual ComPtr<IDispatcherQueue> GetDispatcherForCurrentThread() = 0;
#else
    virtual ComPtr<ICoreDispatcher> GetDispatcherForCurrentThread() = 0;
#endif
    virtual float GetLogicalDpi() = 0;
    virtual bool ShouldWaitForUIThread() = 0;
};


class DefaultPrintDocumentAdapter : public CanvasPrintDocumentAdapter
{
public:
    virtual ComPtr<ICanvasDevice> GetSharedDevice() override;
#ifdef WINUI3
    virtual ComPtr<IDispatcherQueue> GetDispatcherForCurrentThread() override;
#else
    virtual ComPtr<ICoreDispatcher> GetDispatcherForCurrentThread() override;
#endif
    virtual float GetLogicalDpi() override;
    virtual bool ShouldWaitForUIThread() override;
};
