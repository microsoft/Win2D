// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#include "mocks/MockDxgiAdapter.h"
#include "mocks/MockDxgiOutput.h"

namespace canvas
{
    class StubDxgiAdapter : public MockDxgiAdapter
    {
    public:
        StubDxgiAdapter()
        {
            EnumOutputsMethod.AllowAnyCall(
                [=](UINT, IDXGIOutput** output)
                {
                    auto mockDxgiOutput = Make<MockDxgiOutput>();

                    mockDxgiOutput->WaitForVBlankMethod.AllowAnyCall();

                    return mockDxgiOutput.CopyTo(output);
                });
        }
    };
};
