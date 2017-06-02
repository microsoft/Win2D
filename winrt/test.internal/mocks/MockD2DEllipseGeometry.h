// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

// This file was automatically generated. Please do not edit it manually.

#pragma once

namespace canvas
{
    class MockD2DEllipseGeometry : public RuntimeClass<
        RuntimeClassFlags<ClassicCom>,
        ChainInterfaces<ID2D1EllipseGeometry, ID2D1Geometry, ID2D1Resource>>
    {
    public:
        MOCK_METHOD1_CONST(GetEllipse, void(D2D1_ELLIPSE*));

        MOCK_METHOD2_CONST(GetBounds, HRESULT(CONST D2D1_MATRIX_3X2_F*, D2D1_RECT_F*));
        MOCK_METHOD5_CONST(GetWidenedBounds, HRESULT(FLOAT, ID2D1StrokeStyle*, CONST D2D1_MATRIX_3X2_F*, FLOAT, D2D1_RECT_F*));
        MOCK_METHOD6_CONST(StrokeContainsPoint, HRESULT(D2D1_POINT_2F, FLOAT, ID2D1StrokeStyle*, CONST D2D1_MATRIX_3X2_F*, FLOAT, BOOL*));
        MOCK_METHOD4_CONST(FillContainsPoint, HRESULT(D2D1_POINT_2F, CONST D2D1_MATRIX_3X2_F*, FLOAT, BOOL*));
        MOCK_METHOD4_CONST(CompareWithGeometry, HRESULT(ID2D1Geometry*, CONST D2D1_MATRIX_3X2_F*, FLOAT, D2D1_GEOMETRY_RELATION*));
        MOCK_METHOD4_CONST(Simplify, HRESULT(D2D1_GEOMETRY_SIMPLIFICATION_OPTION, CONST D2D1_MATRIX_3X2_F*, FLOAT, ID2D1SimplifiedGeometrySink*));
        MOCK_METHOD3_CONST(Tessellate, HRESULT(CONST D2D1_MATRIX_3X2_F*, FLOAT, ID2D1TessellationSink*));
        MOCK_METHOD5_CONST(CombineWithGeometry, HRESULT(ID2D1Geometry*, D2D1_COMBINE_MODE, CONST D2D1_MATRIX_3X2_F*, FLOAT, ID2D1SimplifiedGeometrySink*));
        MOCK_METHOD3_CONST(Outline, HRESULT(CONST D2D1_MATRIX_3X2_F*, FLOAT, ID2D1SimplifiedGeometrySink*));
        MOCK_METHOD3_CONST(ComputeArea, HRESULT(CONST D2D1_MATRIX_3X2_F*, FLOAT, FLOAT*));
        MOCK_METHOD3_CONST(ComputeLength, HRESULT(CONST D2D1_MATRIX_3X2_F*, FLOAT, FLOAT*));
        MOCK_METHOD5_CONST(ComputePointAtLength, HRESULT(FLOAT, CONST D2D1_MATRIX_3X2_F*, FLOAT, D2D1_POINT_2F*, D2D1_POINT_2F*));
        MOCK_METHOD5_CONST(Widen, HRESULT(FLOAT, ID2D1StrokeStyle*, CONST D2D1_MATRIX_3X2_F*, FLOAT, ID2D1SimplifiedGeometrySink*));

        MOCK_METHOD1_CONST(GetFactory, void(ID2D1Factory**));

    };
}
