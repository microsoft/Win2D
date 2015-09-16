// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

class MockPrintPreviewDxgiPackageTarget 
    : public RuntimeClass<
        RuntimeClassFlags<ClassicCom>,
        IPrintPreviewDxgiPackageTarget>
{
public:
    CALL_COUNTER_WITH_MOCK(SetJobPageCountMethod, HRESULT(PageCountType,uint32_t));
    CALL_COUNTER_WITH_MOCK(DrawPageMethod, HRESULT(uint32_t,IDXGISurface*,FLOAT,FLOAT));
    CALL_COUNTER_WITH_MOCK(InvalidatePreviewMethod, HRESULT());

    IFACEMETHODIMP SetJobPageCount(PageCountType t, uint32_t c) override
    {
        return SetJobPageCountMethod.WasCalled(t, c);
    }

    IFACEMETHODIMP DrawPage(uint32_t p, IDXGISurface* s, float dpiX, float dpiY) override
    {
        return DrawPageMethod.WasCalled(p, s, dpiX, dpiY);
    }

    IFACEMETHODIMP InvalidatePreview() override
    {
        return InvalidatePreviewMethod.WasCalled();
    }
};
