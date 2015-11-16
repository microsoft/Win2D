// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace canvas
{
    class MockD2DGradientStopCollection : public RuntimeClass<
        RuntimeClassFlags<ClassicCom>,
        ChainInterfaces<ID2D1GradientStopCollection1, ID2D1GradientStopCollection, ID2D1Resource>>
    {
    public:
        // ID2D1GradientStopCollection1
        MOCK_METHOD2_CONST(GetGradientStops1, void(D2D1_GRADIENT_STOP*, UINT32));
        MOCK_METHOD0_CONST(GetPreInterpolationSpace, D2D1_COLOR_SPACE());
        MOCK_METHOD0_CONST(GetPostInterpolationSpace, D2D1_COLOR_SPACE());
        MOCK_METHOD0_CONST(GetBufferPrecision, D2D1_BUFFER_PRECISION());
        MOCK_METHOD0_CONST(GetColorInterpolationMode, D2D1_COLOR_INTERPOLATION_MODE());

        // ID2D1GradientStopCollection
        MOCK_METHOD0_CONST(GetGradientStopCount, UINT32());
        MOCK_METHOD2_CONST(GetGradientStops, void(D2D1_GRADIENT_STOP*, UINT32));
        MOCK_METHOD0_CONST(GetColorInterpolationGamma, D2D1_GAMMA());
        MOCK_METHOD0_CONST(GetExtendMode, D2D1_EXTEND_MODE());

        // ID2D1Resource
        MOCK_METHOD1_CONST(GetFactory, void(ID2D1Factory**));
    };
}
