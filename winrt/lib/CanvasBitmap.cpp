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
#include "CanvasDrawingSession.h"

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


    //
    // CanvasBitmapManager
    //

    CanvasBitmapManager::CanvasBitmapManager(std::shared_ptr<ICanvasBitmapResourceCreationAdapter> adapter)
        : m_adapter(adapter)
    {
    }


    ComPtr<CanvasBitmap> CanvasBitmapManager::CreateNew(
        ICanvasDevice* canvasDevice,
        HSTRING fileName)
    {
        ComPtr<ICanvasDeviceInternal> canvasDeviceInternal;
        ThrowIfFailed(canvasDevice->QueryInterface(canvasDeviceInternal.GetAddressOf()));

        auto d2dBitmap = canvasDeviceInternal->CreateBitmapFromWicResource(m_adapter->CreateWICFormatConverter(fileName).Get());

        auto bitmap = Make<CanvasBitmap>(
            shared_from_this(),
            d2dBitmap.Get());
        CheckMakeResult(bitmap);
        
        return bitmap;
    }

    ComPtr<CanvasBitmap> CanvasBitmapManager::CreateWrapper(
        ID2D1Bitmap1* d2dBitmap)
    {
        // TODO #2473: Need to create CanvasBitmap or CanvasRenderTarget as
        // appropriate, based on the d2dBitmap

        auto bitmap = Make<CanvasBitmap>(
            shared_from_this(),
            d2dBitmap);
        CheckMakeResult(bitmap);

        return bitmap;
    }

    
    //
    // CanvasBitmapFactory
    //


    CanvasBitmapFactory::CanvasBitmapFactory()
    {
    }

    std::shared_ptr<CanvasBitmapManager> CanvasBitmapFactory::CreateManager()
    {
        auto adapter = std::make_shared<DefaultBitmapResourceCreationAdapter>();
        return std::make_shared<CanvasBitmapManager>(adapter);
    }

    //
    // ICanvasBitmapStatics
    //

    IFACEMETHODIMP CanvasBitmapFactory::LoadAsync(
        ICanvasResourceCreator* resourceCreator,
        HSTRING fileUri,
        ABI::Windows::Foundation::IAsyncOperation<CanvasBitmap*>** canvasBitmap)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(resourceCreator);
                CheckInPointer(fileUri);
                CheckAndClearOutPointer(canvasBitmap);

                ComPtr<ICanvasDevice> canvasDevice;
                ThrowIfFailed(resourceCreator->get_Device(&canvasDevice));

                WinString fileName(fileUri);

                auto asyncOperation = Make<AsyncOperation<CanvasBitmap>>(
                    [=]
                    {
                        auto bitmap = GetManager()->Create(canvasDevice.Get(), fileName);
                        CheckMakeResult(bitmap);
                        return bitmap;
                    });

                CheckMakeResult(asyncOperation);
                ThrowIfFailed(asyncOperation.CopyTo(canvasBitmap));
            });
    }

    //
    // ICanvasFactoryNative
    //

    IFACEMETHODIMP CanvasBitmapFactory::GetOrCreate(
        IUnknown* resource,
        IInspectable** wrapper)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(resource);
                CheckAndClearOutPointer(wrapper);

                ComPtr<ID2D1Bitmap1> d2dBitmap;
                ThrowIfFailed(resource->QueryInterface(d2dBitmap.GetAddressOf()));

                auto newCanvasBitmap = GetManager()->GetOrCreate(d2dBitmap.Get());

                ThrowIfFailed(newCanvasBitmap.CopyTo(wrapper));
            });
    }


    //
    // CanvasBitmap
    //


    CanvasBitmap::CanvasBitmap(
        std::shared_ptr<CanvasBitmapManager> manager,
        ID2D1Bitmap1* bitmap)
        : CanvasBitmapImpl(manager, bitmap)
    {
    }


    ActivatableClassWithFactory(CanvasBitmap, CanvasBitmapFactory);
}}}}
