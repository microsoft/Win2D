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
    class MockD2DBitmap : public RuntimeClass<
        RuntimeClassFlags<ClassicCom>,
        ChainInterfaces<ID2D1Bitmap1, ID2D1Image, ID2D1Resource >>
    {
    public:
        std::function<void(unsigned int* width, unsigned int* height)> MockGetPixelSize;
        std::function<void(float* width, float* height)> MockGetSize;
        std::function<HRESULT(CONST D2D1_POINT_2U*, ID2D1Bitmap *bitmap, CONST D2D1_RECT_U*)> MockCopyFromBitmap;

        //
        // ID2D1Bitmap1
        //

        STDMETHOD_(void, GetColorContext)(
            _Outptr_result_maybenull_ ID2D1ColorContext **colorContext
            ) CONST
        {
            Assert::Fail(L"Unexpected call to GetColorContext");
        }

        STDMETHOD_(D2D1_BITMAP_OPTIONS, GetOptions)(
            ) CONST
        {
            Assert::Fail(L"Unexpected call to GetOptions");

            D2D1_BITMAP_OPTIONS options{};
            return options;
        }

        STDMETHOD(GetSurface)(
            _Outptr_result_maybenull_ IDXGISurface **dxgiSurface
            ) CONST
        {
            Assert::Fail(L"Unexpected call to GetSurface");
            return E_NOTIMPL;
        }

        STDMETHOD(Map)(
            D2D1_MAP_OPTIONS options,
            _Out_ D2D1_MAPPED_RECT *mappedRect
            )
        {
            Assert::Fail(L"Unexpected call to Map");
            return E_NOTIMPL;
        }

        STDMETHOD(Unmap)()
        {
            Assert::Fail(L"Unexpected call to Unmap");
            return E_NOTIMPL;
        }

        //
        // ID2D1Bitmap1
        //

        STDMETHOD_(D2D1_SIZE_F, GetSize)() CONST
        {
            D2D1_SIZE_F size{};

            if (!MockGetSize)
            {
                Assert::Fail(L"Unexpected call to GetSize");
                return size;
            }
            MockGetSize(&size.width, &size.height);
            return size;
        }


        STDMETHOD_(D2D1_SIZE_U, GetPixelSize)() CONST
        {
            D2D1_SIZE_U size{};

            if (!MockGetPixelSize)
            {
                Assert::Fail(L"Unexpected call to GetSize");
                return size;
            }
            MockGetPixelSize(&size.width, &size.height);
            return size;
        }

        STDMETHOD_(D2D1_PIXEL_FORMAT, GetPixelFormat)() CONST 
        {
            Assert::Fail(L"Unexpected call to GetPixelFormat");

            D2D1_PIXEL_FORMAT format{};
            return format;
        }


        STDMETHOD_(void, GetDpi)(
            FLOAT *dpiX,
            FLOAT *dpiY) CONST
        {
            Assert::Fail(L"Unexpected call to GetDpi");
        }

        STDMETHOD(CopyFromBitmap)(
            CONST D2D1_POINT_2U *destinationPoint,
            ID2D1Bitmap *bitmap,
            CONST D2D1_RECT_U *sourceRect)
        {
            if (!MockCopyFromBitmap)
            {
                Assert::Fail(L"Unexpected call to CopyFromBitmap");
                return E_NOTIMPL;
            }

            return MockCopyFromBitmap(destinationPoint, bitmap, sourceRect);
        }

        STDMETHOD(CopyFromRenderTarget)(
            CONST D2D1_POINT_2U *destinationPoint,
            ID2D1RenderTarget *renderTarget,
            CONST D2D1_RECT_U *sourceRect) 
        {
            Assert::Fail(L"Unexpected call to CopyFromRenderTarget");
            return E_NOTIMPL;
        }

        STDMETHOD(CopyFromMemory)(
            CONST D2D1_RECT_U *destinationRect,
            CONST void *sourceData,
            UINT32 pitch) 
        {
            Assert::Fail(L"Unexpected call to CopyFromMemory");
            return E_NOTIMPL;
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
