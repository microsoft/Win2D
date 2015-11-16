// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace canvas
{
    class MockD2DLinearGradientBrush : public RuntimeClass<
        RuntimeClassFlags<ClassicCom>,
        ChainInterfaces<ID2D1LinearGradientBrush, ID2D1Brush, ID2D1Resource>>
    {
    public:

        std::function<void(D2D1_POINT_2F pt)> MockSetStartPoint;
        std::function<void(D2D1_POINT_2F pt)> MockSetEndPoint;
        std::function<D2D1_POINT_2F()> MockGetStartPoint;
        std::function<D2D1_POINT_2F()> MockGetEndPoint;
        std::function<void(ID2D1GradientStopCollection** stopCollection)> MockGetGradientStopCollection;
        std::function<float()> MockGetOpacity;
        std::function<void(float opacity)> MockSetOpacity;
        std::function<void(D2D1_MATRIX_3X2_F *transform)> MockGetTransform;
        std::function<void(const D2D1_MATRIX_3X2_F *transform)> MockSetTransform;

        //
        // ID2D1LinearGradientBrush
        //
        
        STDMETHOD_(void, SetStartPoint)(D2D1_POINT_2F startPoint)
        {
            if (MockSetStartPoint)
            {
                MockSetStartPoint(startPoint);
                return;
            }

            Assert::Fail(L"Unexpected call to SetStartPoint");
        }

        STDMETHOD_(void, SetEndPoint)(D2D1_POINT_2F endPoint)
        {
            if (MockSetEndPoint)
            {
                MockSetEndPoint(endPoint);
                return;
            }

            Assert::Fail(L"Unexpected call to SetEndPoint");
        }
    
        STDMETHOD_(D2D1_POINT_2F, GetStartPoint)() CONST
        {
            if (MockGetStartPoint)
            {
                return MockGetStartPoint();
            }

            Assert::Fail(L"Unexpected call to GetStartPoint");
            return D2D1::Point2F(0, 0);
        }
    
        STDMETHOD_(D2D1_POINT_2F, GetEndPoint)() CONST
        {
            if (MockGetEndPoint)
            {
                return MockGetEndPoint();
            }

            Assert::Fail(L"Unexpected call to GetEndPoint");
            return D2D1::Point2F(0, 0);
        }
    
        STDMETHOD_(void, GetGradientStopCollection)(ID2D1GradientStopCollection **gradientStopCollection ) CONST
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
