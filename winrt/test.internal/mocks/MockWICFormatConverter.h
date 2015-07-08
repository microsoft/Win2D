// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace canvas
{
    class MockWICFormatConverter : public RuntimeClass<
        RuntimeClassFlags<ClassicCom>,
        IWICFormatConverter>
    {
        int m_width;
        int m_height;
    public:
        std::function<void(unsigned int* width, unsigned int* height)> MockGetSize;

        virtual HRESULT STDMETHODCALLTYPE GetSize(
            /* [out] */ __RPC__out UINT* width,
            /* [out] */ __RPC__out UINT* height) override
        {
            if (!MockGetSize)
                return E_NOTIMPL;

            MockGetSize(width, height);
            return S_OK;
        }

        virtual HRESULT STDMETHODCALLTYPE GetPixelFormat(
            /* [out] */ __RPC__out WICPixelFormatGUID *pixelFormat) override
        {
            return E_NOTIMPL;
        }

        virtual HRESULT STDMETHODCALLTYPE GetResolution(
            /* [out] */ __RPC__out double *dpiX,
            /* [out] */ __RPC__out double *dpiY) override
        {
            return E_NOTIMPL;
        }

        virtual HRESULT STDMETHODCALLTYPE CopyPalette(
            /* [in] */ __RPC__in_opt IWICPalette *palette) override
        {
            return E_NOTIMPL;
        }

        virtual HRESULT STDMETHODCALLTYPE CopyPixels(
            /* [unique][in] */ __RPC__in_opt const WICRect *rectangle,
            /* [in] */ UINT stride,
            /* [in] */ UINT bufferSize,
            /* [size_is][out] */ __RPC__out_ecount_full(cbBufferSize) BYTE *buffer) override
        {
            return E_NOTIMPL;
        }

        virtual HRESULT STDMETHODCALLTYPE Initialize(
            /* [in] */ __RPC__in_opt IWICBitmapSource *source,
            /* [in] */ __RPC__in REFWICPixelFormatGUID destinationFormat,
            /* [in] */ WICBitmapDitherType dither,
            /* [unique][in] */ __RPC__in_opt IWICPalette *palette,
            /* [in] */ double alphaThresholdPercent,
            /* [in] */ WICBitmapPaletteType paletteTranslate) override
        {
            return E_NOTIMPL;
        }

        virtual HRESULT STDMETHODCALLTYPE CanConvert(
            /* [in] */ __RPC__in REFWICPixelFormatGUID sourcePixelFormat,
            /* [in] */ __RPC__in REFWICPixelFormatGUID destinationPixelFormat,
            /* [out] */ __RPC__out BOOL *canConvert) override
        {
            return E_NOTIMPL;
        }
    };
}
