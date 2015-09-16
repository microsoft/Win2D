// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

class MockPrintControl
    : public RuntimeClass<
        RuntimeClassFlags<ClassicCom>,
        ID2D1PrintControl>
{
public:
    CALL_COUNTER_WITH_MOCK(AddPageMethod, HRESULT(ID2D1CommandList*, D2D_SIZE_F, IStream*, D2D1_TAG*, D2D1_TAG*));
    CALL_COUNTER_WITH_MOCK(CloseMethod, HRESULT());
    
    IFACEMETHODIMP AddPage(ID2D1CommandList* commandList, D2D_SIZE_F pageSize, IStream* pagePrintTicketStream, D2D1_TAG* tag1, D2D1_TAG* tag2) override
    {
        return AddPageMethod.WasCalled(commandList, pageSize, pagePrintTicketStream, tag1, tag2);
    }
    
    IFACEMETHODIMP Close() override
    {
        return CloseMethod.WasCalled();
    }
};
