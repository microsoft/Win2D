// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace canvas
{
    class MockD2DSolidColorBrush : public RuntimeClass<
        RuntimeClassFlags<ClassicCom>,
        ChainInterfaces<ID2D1SolidColorBrush, ID2D1Brush, ID2D1Resource>>
    {
    public:
        std::function<void(const D2D1_COLOR_F* color)> MockSetColor;

        //
        // ID2D1SolidColorBrush
        //
        STDMETHOD_(void, SetColor)(
            _In_ CONST D2D1_COLOR_F *color
            )
        {
            if (MockSetColor)
            {
                MockSetColor(color);
            }
            else
            {
                Assert::Fail(L"Unexpected call to SetColor");
            }
        }

        STDMETHOD_(D2D1_COLOR_F, GetColor)(
            ) CONST
        {
            Assert::Fail(L"Unexpected call to GetColor");
            return D2D1::ColorF(0, 0, 0, 0);
        }

        //
        // ID2D1Brush
        //

        STDMETHOD_(void, SetOpacity)(FLOAT opacity)
        {
            Assert::Fail(L"Unexpected call to SetOpacity");
        }

        STDMETHOD_(FLOAT, GetOpacity)() CONST
        {
            Assert::Fail(L"Unexpected call to GetOpacity");
            return 0;
        }

        STDMETHOD_(void, SetTransform)(_In_ CONST D2D1_MATRIX_3X2_F *transform)
        {
            Assert::Fail(L"Unexpected call to SetTransform");
        }

        STDMETHOD_(void, GetTransform)(_Out_ D2D1_MATRIX_3X2_F *transform) CONST
        {
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
