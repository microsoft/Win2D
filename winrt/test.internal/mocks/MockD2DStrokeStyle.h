// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace canvas
{
    class MockD2DStrokeStyle : public RuntimeClass<
        RuntimeClassFlags<ClassicCom>,
        ChainInterfaces<ID2D1StrokeStyle1, ID2D1StrokeStyle, ID2D1Resource >>
    {
    public:

        //
        // ID2D1StrokeStyle
        //

        IFACEMETHODIMP_(D2D1_CAP_STYLE) GetStartCap() CONST
        {
            Assert::Fail(L"Unexpected call to GetStartCap");
            return static_cast<D2D1_CAP_STYLE>(0);
        }

        IFACEMETHODIMP_(D2D1_CAP_STYLE) GetEndCap() CONST
        {
            Assert::Fail(L"Unexpected call to GetEndCap");
            return static_cast<D2D1_CAP_STYLE>(0);
        }

        IFACEMETHODIMP_(D2D1_CAP_STYLE) GetDashCap(
        ) CONST
        {
            Assert::Fail(L"Unexpected call to GetDashCap");
            return static_cast<D2D1_CAP_STYLE>(0);
        }

        IFACEMETHODIMP_(FLOAT) GetMiterLimit() CONST
        {
            Assert::Fail(L"Unexpected call to GetMiterLimit");
            return 0.0f;
        }

        IFACEMETHODIMP_(D2D1_LINE_JOIN) GetLineJoin() CONST
        {
            Assert::Fail(L"Unexpected call to GetLineJoin");
            return static_cast<D2D1_LINE_JOIN>(0);
        }


        IFACEMETHODIMP_(FLOAT) GetDashOffset() CONST
        {
            Assert::Fail(L"Unexpected call to GetDashOffset");
            return 0.0f;
        }

        IFACEMETHODIMP_(D2D1_DASH_STYLE) GetDashStyle() CONST
        {
            Assert::Fail(L"Unexpected call to GetDashStyle");
            return static_cast<D2D1_DASH_STYLE>(0);
        }

        IFACEMETHODIMP_(UINT32) GetDashesCount() CONST
        {
            Assert::Fail(L"Unexpected call to GetDashesCount");
            return 0;
        }

        IFACEMETHODIMP_(void) GetDashes(
            _Out_writes_(dashesCount) FLOAT *dashes,
            UINT32 dashesCount
            ) CONST
        {
            Assert::Fail(L"Unexpected call to GetDashes");
        }

        //
        // ID2D1StrokeStyle1
        //

        IFACEMETHODIMP_(D2D1_STROKE_TRANSFORM_TYPE) GetStrokeTransformType() CONST
        {
            Assert::Fail(L"Unexpected call to GetStrokeTransformType");
            return static_cast<D2D1_STROKE_TRANSFORM_TYPE>(0);
        }

        //
        // ID2D1Resource
        //

        IFACEMETHODIMP_(void) GetFactory(ID2D1Factory **factory) const override
        {
            Assert::Fail(L"Unexpected call to GetFactory");
        }
    };
}
