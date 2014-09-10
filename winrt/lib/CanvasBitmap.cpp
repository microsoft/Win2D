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

            const wchar_t* fileNameBuffer = WindowsGetStringRawBuffer(fileName, nullptr);

            if ( wcslen(fileNameBuffer)==0 || fileNameBuffer == NULL)
            {
                // fileName string is invalid
                ThrowHR(E_INVALIDARG);
            }

            ComPtr<IWICBitmapDecoder> wicBitmapDecoder;
            ThrowIfFailed(m_wicFactory->CreateDecoderFromFilename(fileNameBuffer, NULL, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &wicBitmapDecoder));
            ComPtr<IWICBitmapFrameDecode> wicBimapFrameSource;
            ThrowIfFailed(wicBitmapDecoder->GetFrame(0, &wicBimapFrameSource));
            ThrowIfFailed(m_wicFactory->CreateFormatConverter(&wicFormatConverter));

            ThrowIfFailed(wicFormatConverter->Initialize(
                wicBimapFrameSource.Get(), 
                GUID_WICPixelFormat32bppPBGRA, 
                WICBitmapDitherTypeNone, 
                NULL, 
                0, 
                WICBitmapPaletteTypeMedianCut));

            return wicFormatConverter;
        }
    };

    CanvasBitmapFactory::CanvasBitmapFactory()
    {
        m_adapter = std::make_shared<DefaultBitmapResourceCreationAdapter>();
    }

    IFACEMETHODIMP CanvasBitmapFactory::LoadAsync(
        ICanvasResourceCreator* resourceCreator,
        HSTRING rawFileName,
        ABI::Windows::Foundation::IAsyncOperation<CanvasBitmap*>** canvasBitmap)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(resourceCreator);
                CheckInPointer(rawFileName);
                CheckAndClearOutPointer(canvasBitmap);

                ComPtr<ICanvasDevice> canvasDevice;
                resourceCreator->get_Device(&canvasDevice);

                // TODO #2052: use WinString constructor with HSTRING later
                WinString fileName;
                fileName = rawFileName;

                auto asyncOperation = Make<AsyncOperation<CanvasBitmap>>([=]
                {
                    auto bitmap = Make<CanvasBitmap>(canvasDevice.Get(), fileName, m_adapter);
                    CheckMakeResult(bitmap);
                    return bitmap;
                });

                CheckMakeResult(asyncOperation);
                ThrowIfFailed(asyncOperation.CopyTo(canvasBitmap));
            });
    }

    CanvasBitmap::CanvasBitmap(ICanvasDevice* canvasDevice, HSTRING fileName, std::shared_ptr<ICanvasBitmapResourceCreationAdapter> adapter)
        : m_resourceAdapter(adapter)
    {
        ComPtr<ICanvasDeviceInternal> canvasDeviceInternal;
        ThrowIfFailed(canvasDevice->QueryInterface(canvasDeviceInternal.GetAddressOf()));

        m_resource = canvasDeviceInternal->CreateBitmap(adapter->CreateWICFormatConverter(fileName).Get());
    }

    IFACEMETHODIMP CanvasBitmap::get_SizeInPixels(_Out_ ABI::Windows::Foundation::Size* size)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(size);

                auto& resource = m_resource.EnsureNotClosed();
                D2D1_SIZE_U d2dSize = resource->GetPixelSize();
                size->Height = static_cast<float>(d2dSize.height);
                size->Width = static_cast<float>(d2dSize.width);
            });
    }

    IFACEMETHODIMP CanvasBitmap::get_Size(_Out_ ABI::Windows::Foundation::Size* size)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(size);

                auto& resource = m_resource.EnsureNotClosed();
                D2D1_SIZE_F d2dSize = resource->GetSize();
                size->Height = d2dSize.height;
                size->Width = d2dSize.width;
            });
    }

    IFACEMETHODIMP CanvasBitmap::get_Bounds(_Out_ ABI::Windows::Foundation::Rect* bounds)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(bounds);

                auto& resource = m_resource.EnsureNotClosed();
                D2D1_SIZE_F d2dSize = resource->GetSize();
                bounds->X = 0;
                bounds->Y = 0;
                bounds->Width = d2dSize.width;
                bounds->Height = d2dSize.height;
            });
    }

    IFACEMETHODIMP CanvasBitmap::Close()
    {
        return ExceptionBoundary(
            [&]
            {
                m_resource.Close();
            });
    }

    // ICanvasImageInternal

    ComPtr<ID2D1Image> CanvasBitmap::GetD2DImage(ID2D1DeviceContext* deviceContext)
    {
        CheckInPointer(deviceContext);

        auto& resource = m_resource.EnsureNotClosed();

        return resource;
    }

    // ICanvasBitmapInternal
    ComPtr<ID2D1Bitmap1> CanvasBitmap::GetD2DBitmap()
    {
        auto& resource = m_resource.EnsureNotClosed();

        return resource;
    }

    ActivatableClassWithFactory(CanvasBitmap, CanvasBitmapFactory);
}}}}
