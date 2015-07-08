// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#include "../test.external/MockDxgiDevice.h"
#include "stubs/StubDxgiAdapter.h"

namespace canvas
{
    class StubDxgiDevice : public MockDxgiDevice
    {
        HRESULT STDMETHODCALLTYPE GetAdapter(IDXGIAdapter** adapter) override
        {
            auto stubDxgiAdapter = Make<StubDxgiAdapter>();

            return stubDxgiAdapter.CopyTo(adapter);
        }
    };
}
