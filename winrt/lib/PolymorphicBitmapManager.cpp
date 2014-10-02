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

#include "pch.h"

#include "CanvasBitmap.h"
#include "CanvasDevice.h"
#include "CanvasRenderTarget.h"
#include "PolymorphicBitmapManager.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas
{
    class DefaultBitmapResourceCreationAdapter : public ICanvasBitmapResourceCreationAdapter
    {
        ComPtr<IWICImagingFactory2> m_wicFactory;

    public:
        DefaultBitmapResourceCreationAdapter()
        {
            ThrowIfFailed(
                CoCreateInstance(
                CLSID_WICImagingFactory,
                nullptr,
                CLSCTX_INPROC_SERVER,
                IID_PPV_ARGS(&m_wicFactory)));
        }

        ComPtr<IWICFormatConverter> CreateWICFormatConverter(HSTRING fileName)
        {
            ComPtr<IWICFormatConverter> wicFormatConverter;

            uint32_t fileNameLength = 0;
            wchar_t const* fileNameBuffer = WindowsGetStringRawBuffer(fileName, &fileNameLength);

            if (!fileNameBuffer || fileNameLength==0)
                ThrowHR(E_INVALIDARG);

            ComPtr<IWICBitmapDecoder> wicBitmapDecoder;
            ThrowIfFailed(m_wicFactory->CreateDecoderFromFilename(
                fileNameBuffer, 
                nullptr, 
                GENERIC_READ, 
                WICDecodeMetadataCacheOnLoad, 
                &wicBitmapDecoder));

            ComPtr<IWICBitmapFrameDecode> wicBitmapFrameSource;
            ThrowIfFailed(wicBitmapDecoder->GetFrame(0, &wicBitmapFrameSource));

            ThrowIfFailed(m_wicFactory->CreateFormatConverter(&wicFormatConverter));

            ThrowIfFailed(wicFormatConverter->Initialize(
                wicBitmapFrameSource.Get(), 
                GUID_WICPixelFormat32bppPBGRA, 
                WICBitmapDitherTypeNone, 
                NULL, 
                0, 
                WICBitmapPaletteTypeMedianCut));

            return wicFormatConverter;
        }

        ComPtr<IWICFormatConverter> CreateWICFormatConverter(IStream* fileStream)
        {
            ComPtr<IWICFormatConverter> wicFormatConverter;

            ComPtr<IWICBitmapDecoder> wicBitmapDecoder;
            ThrowIfFailed(m_wicFactory->CreateDecoderFromStream(
                fileStream,
                nullptr,
                WICDecodeMetadataCacheOnLoad,
                &wicBitmapDecoder));

            ComPtr<IWICBitmapFrameDecode> wicBitmapFrameSource;
            ThrowIfFailed(wicBitmapDecoder->GetFrame(0, &wicBitmapFrameSource));

            ThrowIfFailed(m_wicFactory->CreateFormatConverter(&wicFormatConverter));

            ThrowIfFailed(wicFormatConverter->Initialize(
                wicBitmapFrameSource.Get(),
                GUID_WICPixelFormat32bppPBGRA,
                WICBitmapDitherTypeNone,
                NULL,
                0,
                WICBitmapPaletteTypeMedianCut));

            return wicFormatConverter;
        }
    };


    PolymorphicBitmapManager::PolymorphicBitmapManager(std::shared_ptr<ICanvasBitmapResourceCreationAdapter> adapter)
        : m_bitmapManager(std::make_shared<CanvasBitmapManager>(adapter))
        , m_renderTargetManager(std::make_shared<CanvasRenderTargetManager>())
    {
    }


    static ComPtr<ID2D1Bitmap1> CreateD2DBitmap(
        ICanvasDevice* canvasDevice, 
        IDirect3DSurface* surface,
        CanvasAlphaBehavior alpha, 
        float dpi)
    {
        auto dxgiSurface = GetDXGIInterface<IDXGISurface2>(surface);
        auto d2dDevice = As<ICanvasDeviceInternal>(canvasDevice)->GetD2DDevice();

        ComPtr<ID2D1DeviceContext1> deviceContext;
        ThrowIfFailed(d2dDevice->CreateDeviceContext(
            D2D1_DEVICE_CONTEXT_OPTIONS_NONE,
            &deviceContext));

        D2D1_BITMAP_PROPERTIES1 bitmapProperties = D2D1::BitmapProperties1();
        bitmapProperties.pixelFormat.alphaMode = ToD2DAlphaMode(alpha);
        bitmapProperties.dpiX = dpi;
        bitmapProperties.dpiY = dpi;

        // D2D requires bitmap flags that match the surface format, if a
        // D2D1_BITMAP_PROPERTIES1 is specified.
        //
        // GetResource is called in case the interopped texture is actually a
        // subresource. If the texture is not a subresource, this is equivalent
        // to a QI to ID3D11Texture2D and will always yield subresource index 0.
        //
        ComPtr<ID3D11Texture2D> parentResource;
        UINT subresourceIndex;
        ThrowIfFailed(dxgiSurface->GetResource(IID_PPV_ARGS(&parentResource), &subresourceIndex));

        ComPtr<IDXGIResource1> dxgiResource;
        ThrowIfFailed(parentResource.As(&dxgiResource));

        DXGI_USAGE dxgiUsage;
        ThrowIfFailed(dxgiResource->GetUsage(&dxgiUsage));

        D3D11_TEXTURE2D_DESC texture2DDesc;
        parentResource->GetDesc(&texture2DDesc);

        if (texture2DDesc.BindFlags & D3D11_BIND_RENDER_TARGET && !(dxgiUsage & DXGI_USAGE_READ_ONLY))
        {
            bitmapProperties.bitmapOptions |= D2D1_BITMAP_OPTIONS_TARGET;
        }

        if (!(texture2DDesc.BindFlags & D3D11_BIND_SHADER_RESOURCE))
        {
            bitmapProperties.bitmapOptions |= D2D1_BITMAP_OPTIONS_CANNOT_DRAW;
        }

        if (texture2DDesc.Usage & D3D11_USAGE_STAGING && texture2DDesc.CPUAccessFlags & D3D11_CPU_ACCESS_READ)
        {
            bitmapProperties.bitmapOptions |= D2D1_BITMAP_OPTIONS_CPU_READ;
        }
        
        ComPtr<ID2D1Bitmap1> d2dBitmap;
        ThrowIfFailed(deviceContext->CreateBitmapFromDxgiSurface(
            dxgiSurface.Get(),
            &bitmapProperties,
            &d2dBitmap));

        return d2dBitmap;
    }


    ComPtr<ICanvasBitmap> PolymorphicBitmapManager::CreateBitmapFromSurface(
        ICanvasDevice* canvasDevice,
        IDirect3DSurface* surface, 
        CanvasAlphaBehavior alpha, 
        float dpi)
    {
        return GetOrCreateBitmap(
            canvasDevice, 
            CreateD2DBitmap(canvasDevice, surface, alpha, dpi).Get());
    }


    ComPtr<CanvasRenderTarget> PolymorphicBitmapManager::CreateRenderTargetFromSurface(
        ICanvasDevice* canvasDevice,
        IDirect3DSurface* surface,
        CanvasAlphaBehavior alpha,
        float dpi)
    {
        return GetOrCreateRenderTarget(
            canvasDevice, 
            CreateD2DBitmap(canvasDevice, surface, alpha, dpi).Get());
    }


    ComPtr<ICanvasBitmap> PolymorphicBitmapManager::GetOrCreateBitmap(
        ICanvasDevice* device, 
        ID2D1Bitmap1* d2dBitmap)
    {
        if (IsRenderTargetBitmap(d2dBitmap))
            return m_renderTargetManager->GetOrCreate(device, d2dBitmap);
        else
            return m_bitmapManager->GetOrCreate(d2dBitmap);
    }


    ComPtr<CanvasRenderTarget> PolymorphicBitmapManager::GetOrCreateRenderTarget(
        ICanvasDevice* device, 
        ID2D1Bitmap1* d2dBitmap)
    {
        if (IsRenderTargetBitmap(d2dBitmap))
            return m_renderTargetManager->GetOrCreate(device, d2dBitmap);
        else
            ThrowHR(E_INVALIDARG);
    }


    std::shared_ptr<PolymorphicBitmapManager> PerApplicationPolymorphicBitmapManager::CreateManager()
    {
        auto adapter = std::make_shared<DefaultBitmapResourceCreationAdapter>();
        return std::make_shared<PolymorphicBitmapManager>(adapter);
    }


}}}}
