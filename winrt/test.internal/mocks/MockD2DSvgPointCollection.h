// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

// This file was automatically generated. Please do not edit it manually.

#pragma once

namespace canvas
{
    class MockD2DSvgPointCollection : public RuntimeClass<
        RuntimeClassFlags<ClassicCom>,
        ChainInterfaces<ID2D1SvgPointCollection, ID2D1SvgAttribute, ID2D1Resource>>
    {
    public:
        MOCK_METHOD1(RemovePointsAtEnd, HRESULT(UINT32));
        MOCK_METHOD3(UpdatePoints, HRESULT(CONST D2D1_POINT_2F*, UINT32, UINT32));
        MOCK_METHOD3(GetPoints, HRESULT(D2D1_POINT_2F*, UINT32, UINT32));
        MOCK_METHOD0(GetPointsCount, UINT32());

        MOCK_METHOD1(GetElement, void(ID2D1SvgElement**));
        MOCK_METHOD1(Clone, HRESULT(ID2D1SvgAttribute**));

        MOCK_METHOD1_CONST(GetFactory, void(ID2D1Factory**));

    };
}
