// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

// This file was automatically generated. Please do not edit it manually.

#pragma once

namespace canvas
{
    class MockD2DGradientMesh : public RuntimeClass<
        RuntimeClassFlags<ClassicCom>,
        ChainInterfaces<ID2D1GradientMesh, ID2D1Resource>>
    {
    public:

        CALL_COUNTER_WITH_MOCK(GetPatchCountMethod, UINT32());
        CALL_COUNTER_WITH_MOCK(GetPatchesMethod, HRESULT(UINT32, D2D1_GRADIENT_MESH_PATCH*, UINT32));
        CALL_COUNTER_WITH_MOCK(GetFactoryMethod, void(ID2D1Factory**));

        //
        // ID2D1GradientMesh
        //

        STDMETHOD_(UINT32, GetPatchCount)() const override
        {
            return GetPatchCountMethod.WasCalled();
        }

        STDMETHOD(GetPatches)(
            UINT32 startIndex,
            D2D1_GRADIENT_MESH_PATCH* patches,
            UINT32 patchesCount) const override
        {
            return GetPatchesMethod.WasCalled(startIndex, patches, patchesCount);
        }

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
