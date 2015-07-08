// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace canvas
{
    class MockD2DCommandList : public RuntimeClass<
        RuntimeClassFlags<ClassicCom>,
        ChainInterfaces<ID2D1CommandList, ID2D1Image, ID2D1Resource>>
    {
    public:
        CALL_COUNTER_WITH_MOCK(GetFactoryMethod, void(ID2D1Factory**));
        CALL_COUNTER_WITH_MOCK(StreamMethod, HRESULT(ID2D1CommandSink*));
        CALL_COUNTER_WITH_MOCK(CloseMethod, HRESULT());

        std::shared_ptr<int> Token; // this can be used to help track object lifespan

        IFACEMETHODIMP_(void) GetFactory(ID2D1Factory** factory) const override
        {
            GetFactoryMethod.WasCalled(factory);
        }

        IFACEMETHODIMP Stream(ID2D1CommandSink* sink) override
        {
            return StreamMethod.WasCalled(sink);
        }

        IFACEMETHODIMP Close() override
        {
            return CloseMethod.WasCalled();
        }
    };
}
