// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

// This file was automatically generated. Please do not edit it manually.

#pragma once

namespace canvas
{
    class MockD2DGeometrySink : public RuntimeClass<
        RuntimeClassFlags<ClassicCom>,
        ChainInterfaces<ID2D1GeometrySink, ID2D1SimplifiedGeometrySink>>
    {
    public:
        MOCK_METHOD1(AddLine, void(D2D1_POINT_2F));
        MOCK_METHOD1(AddBezier, void(CONST D2D1_BEZIER_SEGMENT*));
        MOCK_METHOD1(AddQuadraticBezier, void(CONST D2D1_QUADRATIC_BEZIER_SEGMENT*));
        MOCK_METHOD2(AddQuadraticBeziers, void(CONST D2D1_QUADRATIC_BEZIER_SEGMENT*, UINT32));
        MOCK_METHOD1(AddArc, void(CONST D2D1_ARC_SEGMENT*));

        MOCK_METHOD1(SetFillMode, void(D2D1_FILL_MODE));
        MOCK_METHOD1(SetSegmentFlags, void(D2D1_PATH_SEGMENT));
        MOCK_METHOD2(BeginFigure, void(D2D1_POINT_2F, D2D1_FIGURE_BEGIN));
        MOCK_METHOD2(AddLines, void(CONST D2D1_POINT_2F*, UINT32));
        MOCK_METHOD2(AddBeziers, void(CONST D2D1_BEZIER_SEGMENT*, UINT32));
        MOCK_METHOD1(EndFigure, void(D2D1_FIGURE_END));
        MOCK_METHOD0(Close, HRESULT());

    };
}
