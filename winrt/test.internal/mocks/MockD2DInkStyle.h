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
        CALL_COUNTER_WITH_MOCK(SetNibTransformMethod, void(CONST D2D1_MATRIX_3X2_F*));
        CALL_COUNTER_WITH_MOCK(GetNibTransformMethod, void(D2D1_MATRIX_3X2_F*));
        CALL_COUNTER_WITH_MOCK(SetNibShapeMethod, void(D2D1_INK_NIB_SHAPE));
        CALL_COUNTER_WITH_MOCK(GetNibShapeMethod, D2D1_INK_NIB_SHAPE());

        CALL_COUNTER_WITH_MOCK(GetFactoryMethod, void(ID2D1Factory**));

        //
        // ID2D1InkStyle
        //

        STDMETHOD_(void, SetNibTransform)(
            CONST D2D1_MATRIX_3X2_F* transform) override
        {
            SetNibTransformMethod.WasCalled(transform);
        }

        STDMETHOD_(void, GetNibTransform)(
            D2D1_MATRIX_3X2_F* transform) const override
        {
            GetNibTransformMethod.WasCalled(transform);
        }

        STDMETHOD_(void, SetNibShape)(
            D2D1_INK_NIB_SHAPE nibShape) override
        {
            SetNibShapeMethod.WasCalled(nibShape);
        }

        STDMETHOD_(D2D1_INK_NIB_SHAPE, GetNibShape)() const override
        {
            return GetNibShapeMethod.WasCalled();
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
