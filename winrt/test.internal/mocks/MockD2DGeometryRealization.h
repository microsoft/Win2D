// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

// This file was automatically generated. Please do not edit it manually.

#pragma once

namespace canvas
{
    class MockD2DGeometryRealization : public RuntimeClass<
        RuntimeClassFlags<ClassicCom>,
        ChainInterfaces<ID2D1GeometryRealization, ID2D1Resource>>
    {
    public:

        CALL_COUNTER_WITH_MOCK(GetFactoryMethod, void(ID2D1Factory**));

        //
        // ID2D1GeometryRealization
        //

        //
        // ID2D1Resource
        //

        STDMETHOD_(void, GetFactory)(
            ID2D1Factory** factory) const override
        {
            GetFactoryMethod.WasCalled(factory);
        }

    };
}
