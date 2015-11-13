// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace canvas
{
    class MockD2DRadialGradientBrush : public RuntimeClass<
        RuntimeClassFlags<ClassicCom>,
        ChainInterfaces<ID2D1RadialGradientBrush, ID2D1Brush, ID2D1Resource>>
    {
    public:
        // ID2D1RadialGradientBrush
        MOCK_METHOD1(SetCenter, void(D2D1_POINT_2F));
        MOCK_METHOD1(SetGradientOriginOffset, void(D2D1_POINT_2F));
        MOCK_METHOD1(SetRadiusX, void(float));
        MOCK_METHOD1(SetRadiusY, void(float));
        MOCK_METHOD0_CONST(GetCenter, D2D1_POINT_2F());
        MOCK_METHOD0_CONST(GetGradientOriginOffset, D2D1_POINT_2F());
        MOCK_METHOD0_CONST(GetRadiusX, float());
        MOCK_METHOD0_CONST(GetRadiusY, float());
        MOCK_METHOD1_CONST(GetGradientStopCollection, void(ID2D1GradientStopCollection**));

        // ID2D1Brush
        MOCK_METHOD1(SetOpacity, void(float));
        MOCK_METHOD0_CONST(GetOpacity, float());
        MOCK_METHOD1(SetTransform, void(D2D1_MATRIX_3X2_F const*));
        MOCK_METHOD1_CONST(GetTransform, void(D2D1_MATRIX_3X2_F*));

        // ID2D1Resource
        MOCK_METHOD1_CONST(GetFactory, void(ID2D1Factory**));
    };
}
