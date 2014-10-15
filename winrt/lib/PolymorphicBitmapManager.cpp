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
    using ::Microsoft::WRL::Wrappers::HStringReference;

    GUID GetGUIDForFileFormat(CanvasBitmapFileFormat fileFormat)
    {
        switch (fileFormat)
        {
            case CanvasBitmapFileFormat::Bmp: return GUID_ContainerFormatBmp;
            case CanvasBitmapFileFormat::Png: return GUID_ContainerFormatPng;
            case CanvasBitmapFileFormat::Jpeg: return GUID_ContainerFormatJpeg;
            case CanvasBitmapFileFormat::Tiff: return GUID_ContainerFormatTiff;
            case CanvasBitmapFileFormat::Gif: return GUID_ContainerFormatGif;
            case CanvasBitmapFileFormat::JpegXR: return GUID_ContainerFormatWmp;
            default:
            {
                ThrowHR(E_INVALIDARG);
            }
        }
    }

    GUID GetEncoderFromFileExtension(
        std::wstring const& fileName)
    {
        static const struct ExtensionTable
        {
            wchar_t* Extension;
            GUID Guid;
        } extensionTable[] =
        {
            { L"bmp", GUID_ContainerFormatBmp },
            { L"dib", GUID_ContainerFormatBmp },
            { L"rle", GUID_ContainerFormatBmp },
            { L"png", GUID_ContainerFormatPng },
            { L"jpg", GUID_ContainerFormatJpeg },
            { L"jpe", GUID_ContainerFormatJpeg },
            { L"jpeg", GUID_ContainerFormatJpeg },
            { L"jfif", GUID_ContainerFormatJpeg },
            { L"exif", GUID_ContainerFormatJpeg },
            { L"tif", GUID_ContainerFormatTiff },
            { L"tiff", GUID_ContainerFormatTiff },
            { L"gif", GUID_ContainerFormatGif },
            { L"wdp", GUID_ContainerFormatWmp },
            { L"jxr", GUID_ContainerFormatWmp },
        };

        size_t separatorIndex = fileName.find_last_of('.');
        std::wstring fileExtension(fileName.substr(separatorIndex + 1));
        for (auto const& tableEntry : extensionTable)
        {
            if (_wcsicmp(fileExtension.c_str(), tableEntry.Extension) == 0)
            {
                return tableEntry.Guid;
            }
        }
        WinStringBuilder message;
        message.Format(Strings::UnrecognizedImageFileExtension);
        ThrowHR(E_INVALIDARG, message.Get());
    }

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

        void SaveLockedMemoryToFile(
            HSTRING fileName,
            CanvasBitmapFileFormat fileFormat,
            float quality,
            unsigned int width,
            unsigned int height,
            float dpiX,
            float dpiY,
            ScopedBitmapLock* bitmapLock)
        {
            WinString fileNameString(fileName);
            
            GUID encoderGuid;
            if (fileFormat == CanvasBitmapFileFormat::Auto)
            {
                encoderGuid = GetEncoderFromFileExtension(static_cast<const wchar_t*>(fileNameString));
            }
            else
            {
                encoderGuid = GetGUIDForFileFormat(fileFormat);
            }

            ComPtr<IWICBitmapEncoder> encoder;
            ThrowIfFailed(m_wicFactory->CreateEncoder(encoderGuid, NULL, &encoder));

            ComPtr<IWICStream> stream;
            ThrowIfFailed(m_wicFactory->CreateStream(&stream));

            ThrowIfFailed(stream->InitializeFromFilename(static_cast<const wchar_t*>(fileNameString), GENERIC_WRITE));

            ThrowIfFailed(encoder->Initialize(stream.Get(), WICBitmapEncoderNoCache));

            ComPtr<IWICBitmapFrameEncode> frameEncode;
            ComPtr<IPropertyBag2> frameProperties;
            encoder->CreateNewFrame(&frameEncode, &frameProperties);

            if (quality < 0.0f || quality > 1.0f)
            {
                ThrowHR(E_INVALIDARG);
            }

            bool supportsQualityOption = 
                encoderGuid == GUID_ContainerFormatJpeg || 
                encoderGuid == GUID_ContainerFormatWmp;
            
            if (supportsQualityOption)
            {
                PROPBAG2 option = { 0 };
                option.pstrName = L"ImageQuality";
                VARIANT value;
                value.vt = VT_R4;
                value.fltVal = quality;
                ThrowIfFailed(frameProperties->Write(1, &option, &value));
            }

            ThrowIfFailed(frameEncode->Initialize(frameProperties.Get()));

            ThrowIfFailed(frameEncode->SetSize(width, height));

            // TODO: #2767 Support saving formats other than 32bppBGRA.
            ComPtr<IWICBitmap> memoryBitmap;
            m_wicFactory->CreateBitmapFromMemory(
                width, 
                height, 
                GUID_WICPixelFormat32bppBGRA, 
                bitmapLock->GetStride(), 
                bitmapLock->GetLockedBufferSize(),
                static_cast<BYTE*>(bitmapLock->GetLockedData()),
                &memoryBitmap);

            WICPixelFormatGUID pixelFormat;
            ThrowIfFailed(memoryBitmap->GetPixelFormat(&pixelFormat));
            ThrowIfFailed(frameEncode->SetPixelFormat(&pixelFormat));

            ThrowIfFailed(frameEncode->SetResolution(dpiX, dpiY));

            ThrowIfFailed(frameEncode->WriteSource(memoryBitmap.Get(), NULL));
            ThrowIfFailed(frameEncode->Commit());
            ThrowIfFailed(encoder->Commit());
        }

        ComPtr<IWICFormatConverter> CreateWICFormatConverter(HSTRING fileName)
        {
            ComPtr<IWICFormatConverter> wicFormatConverter;

            WinString fileNameString(fileName);

            ComPtr<IWICBitmapDecoder> wicBitmapDecoder;
            ThrowIfFailed(m_wicFactory->CreateDecoderFromFilename(
                static_cast<const wchar_t*>(fileNameString),
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
        , m_renderTargetManager(std::make_shared<CanvasRenderTargetManager>(adapter))
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
        ComPtr<ID3D11Texture2D> parentResource = GetTexture2DForDXGISurface(dxgiSurface);

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
