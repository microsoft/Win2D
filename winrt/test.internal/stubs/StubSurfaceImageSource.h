// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace canvas
{
    class StubSurfaceImageSource : public MockSurfaceImageSource
    {
    public:
        IFACEMETHODIMP SetDevice(IUnknown*) override 
        {
            return S_OK;
        }
    };
}
