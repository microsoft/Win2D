// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace canvas
{
    class MockD2DRadialGradientBrush : public RuntimeClass<
        RuntimeClassFlags<ClassicCom>,
        ChainInterfaces<ID2D1RadialGradientBrush, ID2D1Brush, ID2D1Resource >>
    {
    public:

        std::function<void(D2D1_POINT_2F pt)> MockSetCenter;
        std::function<void(D2D1_POINT_2F pt)> MockSetGradientOriginOffset;
        std::function<void(float r)> MockSetRadiusX;
        std::function<void(float r)> MockSetRadiusY;
        std::function<D2D1_POINT_2F()> MockGetCenter;
        std::function<D2D1_POINT_2F()> MockGetGradientOriginOffset;
        std::function<float()> MockGetRadiusX;
        std::function<float()> MockGetRadiusY;
        std::function<void(ID2D1GradientStopCollection** stopCollection)> MockGetGradientStopCollection;
        std::function<float()> MockGetOpacity;
        std::function<void(float opacity)> MockSetOpacity;
        std::function<void(D2D1_MATRIX_3X2_F *transform)> MockGetTransform;
        std::function<void(const D2D1_MATRIX_3X2_F *transform)> MockSetTransform;

        //
        // ID2D1RadialGradientBrush
        //

        STDMETHOD_(void, SetCenter)(D2D1_POINT_2F Center)
        {
            if (MockSetCenter)
            {
                MockSetCenter(Center);
                return;
            }

            Assert::Fail(L"Unexpected call to SetCenter");
        }

        STDMETHOD_(void, SetGradientOriginOffset)(D2D1_POINT_2F GradientOriginOffset)
        {
            if (MockSetGradientOriginOffset)
            {
                MockSetGradientOriginOffset(GradientOriginOffset);
                return;
            }

            Assert::Fail(L"Unexpected call to SetGradientOriginOffset");
        }

        STDMETHOD_(void, SetRadiusX)(float r)
        {
            if (MockSetRadiusX)
            {
                MockSetRadiusX(r);
                return;
            }

            Assert::Fail(L"Unexpected call to SetRadiusX");
        }

        STDMETHOD_(void, SetRadiusY)(float r)
        {
            if (MockSetRadiusY)
            {
                MockSetRadiusY(r);
                return;
            }

            Assert::Fail(L"Unexpected call to MockSetRadiusY");
        }

        STDMETHOD_(D2D1_POINT_2F, GetCenter)() CONST
        {
            if (MockGetCenter)
            {
                return MockGetCenter();
            }

            Assert::Fail(L"Unexpected call to GetCenter");
            return D2D1::Point2F(0, 0);
        }

        STDMETHOD_(D2D1_POINT_2F, GetGradientOriginOffset)() CONST
        {
            if (MockGetGradientOriginOffset)
            {
                return MockGetGradientOriginOffset();
            }

            Assert::Fail(L"Unexpected call to GetGradientOriginOffset");
            return D2D1::Point2F(0, 0);
        }

        STDMETHOD_(float, GetRadiusX)() CONST
        {
            if (MockGetRadiusX)
            {
                return MockGetRadiusX();
            }

            Assert::Fail(L"Unexpected call to GetRadiusX");
            return 0.0f;
        }

            STDMETHOD_(float, GetRadiusY)() CONST
        {
            if (MockGetRadiusY)
            {
                return MockGetRadiusY();
            }

            Assert::Fail(L"Unexpected call to GetRadiusY");
            return 0.0f;
        }

        STDMETHOD_(void, GetGradientStopCollection)(ID2D1GradientStopCollection **gradientStopCollection) CONST
        {
            if (MockGetGradientStopCollection)
            {
                MockGetGradientStopCollection(gradientStopCollection);
                return;
            }

            Assert::Fail(L"Unexpected call to GetGradientStopCollection");
        }

            //
            // ID2D1Brush
            //

        STDMETHOD_(void, SetOpacity)(FLOAT opacity)
        {
            if (MockSetOpacity)
            {
                MockSetOpacity(opacity);
                return;
            }
            Assert::Fail(L"Unexpected call to SetOpacity");
        }

        STDMETHOD_(FLOAT, GetOpacity)() CONST
        {
            if (MockGetOpacity)
            {
                return MockGetOpacity();
            }
            Assert::Fail(L"Unexpected call to GetOpacity");
            return 0;
        }

        STDMETHOD_(void, SetTransform)(_In_ CONST D2D1_MATRIX_3X2_F *transform)
        {
            if (MockSetTransform)
            {
                MockSetTransform(transform);
                return;
            }
            Assert::Fail(L"Unexpected call to SetTransform");
        }

        STDMETHOD_(void, GetTransform)(_Out_ D2D1_MATRIX_3X2_F *transform) CONST
        {
            if (MockGetTransform)
            {
                MockGetTransform(transform);
                return;
            }
            Assert::Fail(L"Unexpected call to GetTransform");
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
