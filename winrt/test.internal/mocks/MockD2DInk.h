// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

// This file was automatically generated. Please do not edit it manually.

#pragma once

namespace canvas
{
    class MockD2DInk : public RuntimeClass<
        RuntimeClassFlags<ClassicCom>,
        ChainInterfaces<ID2D1Ink, ID2D1Resource>>
    {
    public:
        MOCK_METHOD1(SetStartPoint, void(CONST D2D1_INK_POINT*));
        MOCK_METHOD0_CONST(GetStartPoint, D2D1_INK_POINT());
        MOCK_METHOD2(AddSegments, HRESULT(CONST D2D1_INK_BEZIER_SEGMENT*, UINT32));
        MOCK_METHOD1(RemoveSegmentsAtEnd, HRESULT(UINT32));
        MOCK_METHOD3(SetSegments, HRESULT(UINT32, CONST D2D1_INK_BEZIER_SEGMENT*, UINT32));
        MOCK_METHOD1(SetSegmentAtEnd, HRESULT(CONST D2D1_INK_BEZIER_SEGMENT*));
        MOCK_METHOD0_CONST(GetSegmentCount, UINT32());
        MOCK_METHOD3_CONST(GetSegments, HRESULT(UINT32, D2D1_INK_BEZIER_SEGMENT*, UINT32));
        MOCK_METHOD4_CONST(StreamAsGeometry, HRESULT(ID2D1InkStyle*, CONST D2D1_MATRIX_3X2_F*, FLOAT, ID2D1SimplifiedGeometrySink*));
        MOCK_METHOD3_CONST(GetBounds, HRESULT(ID2D1InkStyle*, CONST D2D1_MATRIX_3X2_F*, D2D1_RECT_F*));

        MOCK_METHOD1_CONST(GetFactory, void(ID2D1Factory**));

    };
}
