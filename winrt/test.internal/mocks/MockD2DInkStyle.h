// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

// This file was automatically generated. Please do not edit it manually.

#pragma once

namespace canvas
{
    class MockD2DInkStyle : public RuntimeClass<
        RuntimeClassFlags<ClassicCom>,
        ChainInterfaces<ID2D1InkStyle, ID2D1Resource>>
    {
    public:
        MOCK_METHOD1(SetNibTransform, void(CONST D2D1_MATRIX_3X2_F*));
        MOCK_METHOD1_CONST(GetNibTransform, void(D2D1_MATRIX_3X2_F*));
        MOCK_METHOD1(SetNibShape, void(D2D1_INK_NIB_SHAPE));
        MOCK_METHOD0_CONST(GetNibShape, D2D1_INK_NIB_SHAPE());

        MOCK_METHOD1_CONST(GetFactory, void(ID2D1Factory**));

    };
}
