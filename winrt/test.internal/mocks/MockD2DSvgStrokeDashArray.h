// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

// This file was automatically generated. Please do not edit it manually.

#pragma once

namespace canvas
{
    class MockD2DSvgStrokeDashArray : public RuntimeClass<
        RuntimeClassFlags<ClassicCom>,
        ChainInterfaces<ID2D1SvgStrokeDashArray, ID2D1SvgAttribute, ID2D1Resource>>
    {
    public:
        MOCK_METHOD1(RemoveDashesAtEnd, HRESULT(UINT32));

        CALL_COUNTER_WITH_MOCK(UpdateDashesMethod1, HRESULT(CONST FLOAT*, UINT32, UINT32));
        CALL_COUNTER_WITH_MOCK(UpdateDashesMethod2, HRESULT(CONST D2D1_SVG_LENGTH*, UINT32, UINT32));

        IFACEMETHODIMP UpdateDashes(CONST FLOAT* a1, UINT32 a2, UINT32 a3) override
        {
            return UpdateDashesMethod1.WasCalled(a1, a2, a3);
        }

        IFACEMETHODIMP UpdateDashes(CONST D2D1_SVG_LENGTH* a1, UINT32 a2, UINT32 a3) override
        {
            return UpdateDashesMethod2.WasCalled(a1, a2, a3);
        }

        CALL_COUNTER_WITH_MOCK(GetDashesMethod1, HRESULT(FLOAT*, UINT32, UINT32));
        CALL_COUNTER_WITH_MOCK(GetDashesMethod2, HRESULT(D2D1_SVG_LENGTH*, UINT32, UINT32));

        IFACEMETHODIMP GetDashes(FLOAT* a1, UINT32 a2, UINT32 a3) override
        {
            return GetDashesMethod1.WasCalled(a1, a2, a3);
        }

        IFACEMETHODIMP GetDashes(D2D1_SVG_LENGTH* a1, UINT32 a2, UINT32 a3) override
        {
            return GetDashesMethod2.WasCalled(a1, a2, a3);
        }

        MOCK_METHOD0(GetDashesCount, UINT32());

        MOCK_METHOD1(GetElement, void(ID2D1SvgElement**));
        MOCK_METHOD1(Clone, HRESULT(ID2D1SvgAttribute**));

        MOCK_METHOD1_CONST(GetFactory, void(ID2D1Factory**));

    };
}
