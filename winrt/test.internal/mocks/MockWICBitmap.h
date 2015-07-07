// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace canvas
{
    class MockWICBitmap : public RuntimeClass <
        RuntimeClassFlags<ClassicCom>,
        IWICBitmap >
    {
    public:

        std::function<void(unsigned int* width, unsigned int* height)> MockGetSize;

        //
        // IWICBitmap
        //

        virtual HRESULT STDMETHODCALLTYPE Lock(
            const WICRect *lock,
            DWORD flags,
            IWICBitmapLock **bitmaplock) override
        {
            return E_NOTIMPL;
        }

        virtual HRESULT STDMETHODCALLTYPE SetPalette(
            IWICPalette *palette) override
        {
            return E_NOTIMPL;
        }

        virtual HRESULT STDMETHODCALLTYPE SetResolution(
            double dpiX,
            double dpiY) override
        {
            return E_NOTIMPL;
        }

        //
        // IWICBitmapSource
        //
        virtual HRESULT STDMETHODCALLTYPE GetSize(
            UINT* width,
            UINT* height) override
        {
            if (!MockGetSize)
                return E_NOTIMPL;

            MockGetSize(width, height);
            return S_OK;
        }

        virtual HRESULT STDMETHODCALLTYPE GetPixelFormat(
            WICPixelFormatGUID *pixelFormat) override
        {
            return E_NOTIMPL;
        }

        virtual HRESULT STDMETHODCALLTYPE GetResolution(
            double *dpiX,
            double *dpiY) override
        {
            return E_NOTIMPL;
        }

        virtual HRESULT STDMETHODCALLTYPE CopyPalette(
            IWICPalette *palette) override
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
    };
}
