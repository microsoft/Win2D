// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#include "stubs/StubDxgiAdapter.h"

namespace canvas
{
    class StubD3D11Device : public MockD3D11Device
    {
        HRESULT STDMETHODCALLTYPE GetAdapter(IDXGIAdapter** pAdapter) override
        {
            auto stubDxgiAdapter = Make<StubDxgiAdapter>();

            return stubDxgiAdapter.CopyTo(pAdapter);
        }
    };
}
