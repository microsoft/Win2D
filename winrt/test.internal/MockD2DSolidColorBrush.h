// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may
// not use these files except in compliance with the License. You may obtain
// a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
// License for the specific language governing permissions and limitations
// under the License.

#pragma once

namespace canvas
{
    class MockD2DSolidColorBrush : public RuntimeClass<
        RuntimeClassFlags<ClassicCom>,
        ChainInterfaces<ID2D1SolidColorBrush, ID2D1Brush, ID2D1Resource>>
    {
    public:
        //
        // ID2D1SolidColorBrush
        //
        STDMETHOD_(void, SetColor)(
            _In_ CONST D2D1_COLOR_F *color
            )
        {
            Assert::Fail(L"Unexpected call to SetColor");
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
