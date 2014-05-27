// Copyright (c) Microsoft Corporation.  All rights reserved

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
