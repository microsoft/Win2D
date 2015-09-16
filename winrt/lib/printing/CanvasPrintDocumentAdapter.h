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
    virtual ComPtr<ICoreDispatcher> GetDispatcherForCurrentThread() = 0;
    virtual float GetLogicalDpi() = 0;
};


class DefaultPrintDocumentAdapter : public CanvasPrintDocumentAdapter
{
public:
    virtual ComPtr<ICanvasDevice> GetSharedDevice() override;
    virtual ComPtr<ICoreDispatcher> GetDispatcherForCurrentThread() override;
    virtual float GetLogicalDpi() override;
};
