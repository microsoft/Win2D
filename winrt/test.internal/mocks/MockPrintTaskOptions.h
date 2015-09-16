// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

using namespace ABI::Windows::Graphics::Printing;

class MockPrintTaskOptions :
    public RuntimeClass<IPrintTaskOptionsCore>
{
public:
    CALL_COUNTER_WITH_MOCK(GetPageDescriptionMethod, HRESULT(uint32_t, PrintPageDescription*));

    //
    // IPrintTaskOptionsCore
    //

    IFACEMETHODIMP GetPageDescription(uint32_t pageNumber, PrintPageDescription* description) override
    {
        return GetPageDescriptionMethod.WasCalled(pageNumber, description);
    }
};
