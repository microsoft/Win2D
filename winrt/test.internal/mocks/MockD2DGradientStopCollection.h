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
        std::function<void(D2D1_GRADIENT_STOP *gradientStops, UINT32 count)> MockGetGradientStops1;
        std::function<D2D1_COLOR_SPACE()> MockGetPreInterpolationSpace;
        std::function<D2D1_COLOR_SPACE()> MockGetPostInterpolationSpace;
        std::function<D2D1_BUFFER_PRECISION()> MockGetBufferPrecision;
        std::function<D2D1_COLOR_INTERPOLATION_MODE()> MockGetColorInterpolationMode;
        std::function<D2D1_EXTEND_MODE()> MockGetExtendMode;
        std::function<UINT32()> MockGetGradientStopCount;

        //
        // ID2D1GradientStopCollection1
        //

        STDMETHOD_(void, GetGradientStops1)(
            D2D1_GRADIENT_STOP *gradientStops,
            UINT32 gradientStopsCount
            ) CONST
        {
            if (MockGetGradientStops1)
            {
                MockGetGradientStops1(gradientStops, gradientStopsCount);
                return;
            }
            Assert::Fail(L"Unexpected call to GetGradientStops1");
        }

        STDMETHOD_(D2D1_COLOR_SPACE, GetPreInterpolationSpace)() CONST
        {
            if (MockGetPreInterpolationSpace)
            {
                return MockGetPreInterpolationSpace();
            }
            Assert::Fail(L"Unexpected call to GetPreInterpolationSpace");
            return static_cast<D2D1_COLOR_SPACE>(0);
        }

        STDMETHOD_(D2D1_COLOR_SPACE, GetPostInterpolationSpace)() CONST 
        {
            if (MockGetPostInterpolationSpace)
            {
                return MockGetPostInterpolationSpace();
            }
            Assert::Fail(L"Unexpected call to GetPostInterpolationSpace");
            return static_cast<D2D1_COLOR_SPACE>(0);
        }
        STDMETHOD_(D2D1_BUFFER_PRECISION, GetBufferPrecision)() CONST
        {
            if (MockGetBufferPrecision)
            {
                return MockGetBufferPrecision();
            }
            Assert::Fail(L"Unexpected call to GetBufferPrecision");
            return static_cast<D2D1_BUFFER_PRECISION>(0);
        }

        STDMETHOD_(D2D1_COLOR_INTERPOLATION_MODE, GetColorInterpolationMode)() CONST
        {
            if (MockGetColorInterpolationMode)
            {
                return MockGetColorInterpolationMode();
            }
            Assert::Fail(L"Unexpected call to GetColorInterpolationMode");
            return static_cast<D2D1_COLOR_INTERPOLATION_MODE>(0);
        }

        //
        // ID2D1GradientStopCollection
        //

        STDMETHOD_(UINT32, GetGradientStopCount)() CONST
        {
            if (MockGetGradientStopCount)
            {
                return MockGetGradientStopCount();
            }
            Assert::Fail(L"Unexpected call to GetGradientStopCount");
            return 0;
        }

        STDMETHOD_(void, GetGradientStops)(
           D2D1_GRADIENT_STOP *gradientStops,
           UINT32 gradientStopsCount) CONST
        {
            Assert::Fail(L"Unexpected call to GetGradientStops");
        }

        STDMETHOD_(D2D1_GAMMA, GetColorInterpolationGamma)() CONST
        {
            Assert::Fail(L"Unexpected call to GetColorInterpolationGamma");
            return static_cast<D2D1_GAMMA>(0);
        }

        STDMETHOD_(D2D1_EXTEND_MODE, GetExtendMode)() CONST
        {
            if (MockGetExtendMode)
            {
                return MockGetExtendMode();
            }
            Assert::Fail(L"Unexpected call to GetExtendMode");
            return static_cast<D2D1_EXTEND_MODE>(0);
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
