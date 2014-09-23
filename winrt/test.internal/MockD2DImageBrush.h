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
    class MockD2DImageBrush : public RuntimeClass<
        RuntimeClassFlags<ClassicCom>,
        ChainInterfaces<ID2D1ImageBrush, ID2D1Brush, ID2D1Resource>>
    {
    public:
        std::function<D2D1_INTERPOLATION_MODE()> MockGetInterpolationMode;
        std::function<void(D2D1_INTERPOLATION_MODE interpolationMode)> MockSetInterpolationMode;
        std::function<void(D2D1_RECT_F *rect)> MockGetSourceRectangle;
        std::function<void(const D2D1_RECT_F *rect)> MockSetSourceRectangle;
        std::function<D2D1_EXTEND_MODE()> MockGetExtendModeX;
        std::function<void(D2D1_EXTEND_MODE extendModeX)> MockSetExtendModeX;
        std::function<D2D1_EXTEND_MODE()> MockGetExtendModeY;
        std::function<void(D2D1_EXTEND_MODE extendModeX)> MockSetExtendModeY;
        std::function<void(ID2D1Image **image)> MockGetImage;
        std::function<void(ID2D1Image *image)> MockSetImage;
        std::function<float()> MockGetOpacity;
        std::function<void(float opacity)> MockSetOpacity;
        std::function<void(D2D1_MATRIX_3X2_F *transform)> MockGetTransform;
        std::function<void(const D2D1_MATRIX_3X2_F *transform)> MockSetTransform;

        STDMETHOD_(void, SetImage)(
            _In_opt_ ID2D1Image *image
            ) 
        {
            if (!MockSetImage)
            {
                Assert::Fail(L"Unexpected call to SetBitmap");
                return;
            }

            MockSetImage(image);
		}


        STDMETHOD_(void, SetExtendModeX)(
            D2D1_EXTEND_MODE extendModeX
            )
        {
            if (!MockSetExtendModeX)
            {
                Assert::Fail(L"Unexpected call to SetExtendModeX");
                return;
            }

            MockSetExtendModeX(extendModeX);
        }

        STDMETHOD_(void, SetExtendModeY)(
            D2D1_EXTEND_MODE extendModeY
            )
        {
            if (!MockSetExtendModeY)
            {
                Assert::Fail(L"Unexpected call to SetExtendModeY");
                return;
            }

            MockSetExtendModeY(extendModeY);
        }


        STDMETHOD_(void, SetInterpolationMode)(
            D2D1_INTERPOLATION_MODE interpolationMode
            ) 
        {
            if (!MockSetInterpolationMode)
            {
                Assert::Fail(L"Unexpected call to SetInterpolationMode");
                return;
            }

            MockSetInterpolationMode(interpolationMode);
		}


        STDMETHOD_(void, SetSourceRectangle)(
            _In_ CONST D2D1_RECT_F *sourceRectangle
            ) 
        {
            if (!MockSetSourceRectangle)
            {
                Assert::Fail(L"Unexpected call to SetSourceRectangle");
                return;
            }

            MockSetSourceRectangle(sourceRectangle);
		}


        STDMETHOD_(void, GetImage)(
            _Outptr_ ID2D1Image **image
            ) CONST 
        {
            if (!MockGetImage)
            {
                Assert::Fail(L"Unexpected call to GetImage");
                return;
            }

            MockGetImage(image);
		}


        STDMETHOD_(D2D1_EXTEND_MODE, GetExtendModeX)(
            ) CONST 
        {
            if (!MockGetExtendModeX)
            {
                Assert::Fail(L"Unexpected call to GetExtendModeX");
                return static_cast<D2D1_EXTEND_MODE>(0);
            }

            return MockGetExtendModeX();
		}


        STDMETHOD_(D2D1_EXTEND_MODE, GetExtendModeY)(
            ) CONST 
        {
            if (!MockGetExtendModeY)
            {
                Assert::Fail(L"Unexpected call to GetExtendModeY");
                return static_cast<D2D1_EXTEND_MODE>(0);
            }

            return MockGetExtendModeY();
		}


        STDMETHOD_(D2D1_INTERPOLATION_MODE, GetInterpolationMode)(
            ) CONST 
        {
            if (!MockGetInterpolationMode)
            {
                Assert::Fail(L"Unexpected call to GetInterpolationMode");
                return static_cast<D2D1_INTERPOLATION_MODE>(0);
            }

            return MockGetInterpolationMode();
		}


        STDMETHOD_(void, GetSourceRectangle)(
            _Out_ D2D1_RECT_F *sourceRectangle
            ) CONST 
        {
            if (!MockGetSourceRectangle)
            {
                Assert::Fail(L"Unexpected call to GetSourceRectangle");
                return;
            }

            MockGetSourceRectangle(sourceRectangle);
		}        

        //
        // ID2D1Brush
        //

        STDMETHOD_(void, SetOpacity)(FLOAT opacity)
        {
            if (!MockSetOpacity)
            {
                Assert::Fail(L"Unexpected call to SetOpacity");
            }

            MockSetOpacity(opacity);
        }

        STDMETHOD_(FLOAT, GetOpacity)() CONST
        {
            if (!MockGetOpacity)
            {
                Assert::Fail(L"Unexpected call to GetOpacity");
                return 0;
            }

            return MockGetOpacity();
        }

        STDMETHOD_(void, SetTransform)(_In_ CONST D2D1_MATRIX_3X2_F *transform)
        {
            if (!MockSetTransform)
            {
                Assert::Fail(L"Unexpected call to SetTransform");
                return;
            }

            MockSetTransform(transform);
        }

        STDMETHOD_(void, GetTransform)(_Out_ D2D1_MATRIX_3X2_F *transform) CONST
        {
            if (!MockGetTransform)
            {
                Assert::Fail(L"Unexpected call to GetTransform");
                return;
            }

            MockGetTransform(transform);
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