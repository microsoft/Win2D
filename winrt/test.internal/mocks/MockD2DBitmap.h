// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace canvas
{
    class MockD2DBitmap : public RuntimeClass<
        RuntimeClassFlags<ClassicCom>,
        ChainInterfaces<ID2D1Bitmap1, ID2D1Bitmap, ID2D1Image, ID2D1Resource >>
    {
    public:
        CALL_COUNTER_WITH_MOCK(GetPixelSizeMethod, D2D1_SIZE_U());
        CALL_COUNTER_WITH_MOCK(GetSizeMethod, D2D1_SIZE_F());
        CALL_COUNTER_WITH_MOCK(GetPixelFormatMethod, D2D1_PIXEL_FORMAT());
        CALL_COUNTER_WITH_MOCK(GetDpiMethod, HRESULT(float*, float*));
        CALL_COUNTER_WITH_MOCK(CopyFromBitmapMethod, HRESULT(D2D1_POINT_2U const*, ID2D1Bitmap*, D2D1_RECT_U const*));

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
            return GetSizeMethod.WasCalled();
        }


        STDMETHOD_(D2D1_SIZE_U, GetPixelSize)() CONST
        {
            return GetPixelSizeMethod.WasCalled();
        }

        STDMETHOD_(D2D1_PIXEL_FORMAT, GetPixelFormat)() CONST 
        {
            return GetPixelFormatMethod.WasCalled();
        }

        STDMETHOD_(void, GetDpi)(
            FLOAT *dpiX,
            FLOAT *dpiY) CONST
        {
            GetDpiMethod.WasCalled(dpiX, dpiY);
        }

        STDMETHOD(CopyFromBitmap)(
            CONST D2D1_POINT_2U *destinationPoint,
            ID2D1Bitmap *bitmap,
            CONST D2D1_RECT_U *sourceRect)
        {
            return CopyFromBitmapMethod.WasCalled(destinationPoint, bitmap, sourceRect);
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
