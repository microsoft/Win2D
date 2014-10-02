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
#include "CanvasRenderTarget.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas
{
    using namespace ABI::Windows::Storage::Streams;
    using namespace ::Microsoft::WRL::Wrappers;

    //
    // CanvasBitmapManager
    //

    CanvasBitmapManager::CanvasBitmapManager(std::shared_ptr<ICanvasBitmapResourceCreationAdapter> adapter)
        : m_adapter(adapter)
    {
    }


    ComPtr<CanvasBitmap> CanvasBitmapManager::CreateNew(
        ICanvasDevice* canvasDevice,
        HSTRING fileName,
        CanvasAlphaBehavior alpha)
    {
        ComPtr<ICanvasDeviceInternal> canvasDeviceInternal;
        ThrowIfFailed(canvasDevice->QueryInterface(canvasDeviceInternal.GetAddressOf()));

        auto d2dBitmap = canvasDeviceInternal->CreateBitmapFromWicResource(m_adapter->CreateWICFormatConverter(fileName).Get(), alpha);

        auto bitmap = Make<CanvasBitmap>(
            shared_from_this(),
            d2dBitmap.Get());
        CheckMakeResult(bitmap);
        
        return bitmap;
    }


    ComPtr<CanvasBitmap> CanvasBitmapManager::CreateNew(
        ICanvasDevice* canvasDevice,
        IStream* fileStream,
        CanvasAlphaBehavior alpha)
    {
        ComPtr<ICanvasDeviceInternal> canvasDeviceInternal;
        ThrowIfFailed(canvasDevice->QueryInterface(canvasDeviceInternal.GetAddressOf()));

        auto d2dBitmap = canvasDeviceInternal->CreateBitmapFromWicResource(m_adapter->CreateWICFormatConverter(fileStream).Get(), alpha);

        auto bitmap = Make<CanvasBitmap>(
            shared_from_this(),
            d2dBitmap.Get());
        CheckMakeResult(bitmap);
        
        return bitmap;
    }


    ComPtr<CanvasBitmap> CanvasBitmapManager::CreateWrapper(
        ID2D1Bitmap1* d2dBitmap)
    {
        auto bitmap = Make<CanvasBitmap>(
            shared_from_this(),
            d2dBitmap);
        CheckMakeResult(bitmap);

        return bitmap;
    }

    class DefaultCanvasBitmapAdapter : public ICanvasBitmapAdapter
    {
        ComPtr<IRandomAccessStreamReferenceStatics> m_randomAccessStreamReferenceStatics;
    public:
        DefaultCanvasBitmapAdapter()
        {
            ThrowIfFailed(GetActivationFactory(HStringReference(RuntimeClass_Windows_Storage_Streams_RandomAccessStreamReference).Get(), &m_randomAccessStreamReferenceStatics));
        }

        ComPtr<IRandomAccessStreamReference> CreateRandomAccessStreamFromUri(ComPtr<IUriRuntimeClass> const& uri) override
        {
            ComPtr<IRandomAccessStreamReference> randomAccessStreamReference;
            ThrowIfFailed(m_randomAccessStreamReferenceStatics->CreateFromUri(uri.Get(), &randomAccessStreamReference));

            return randomAccessStreamReference;
        }
    };

    
    //
    // CanvasBitmapFactory
    //


    CanvasBitmapFactory::CanvasBitmapFactory()
        : m_adapter(std::make_shared<DefaultCanvasBitmapAdapter>())
    {
    }

    //
    // ICanvasBitmapStatics
    //

    IFACEMETHODIMP CanvasBitmapFactory::CreateFromDirect3D11Surface(
        ICanvasResourceCreator* resourceCreator,
        IDirect3DSurface* surface,
        ICanvasBitmap** canvasBitmap)
    {
        return CreateFromDirect3D11SurfaceWithAlphaAndDpi(
            resourceCreator,
            surface,
            CanvasAlphaBehavior::Premultiplied,
            DEFAULT_DPI,
            canvasBitmap);
    }

    IFACEMETHODIMP CanvasBitmapFactory::CreateFromDirect3D11SurfaceWithAlpha(
        ICanvasResourceCreator* resourceCreator,
        IDirect3DSurface* surface,
        CanvasAlphaBehavior alpha,
        ICanvasBitmap** canvasBitmap)
    {
        return CreateFromDirect3D11SurfaceWithAlphaAndDpi(
            resourceCreator,
            surface,
            alpha,
            DEFAULT_DPI,
            canvasBitmap);
    }

    IFACEMETHODIMP CanvasBitmapFactory::CreateFromDirect3D11SurfaceWithDpi(
        ICanvasResourceCreator* resourceCreator,
        IDirect3DSurface* surface,
        float dpi,
        ICanvasBitmap** canvasBitmap)
    {
        return CreateFromDirect3D11SurfaceWithAlphaAndDpi(
            resourceCreator,
            surface,
            CanvasAlphaBehavior::Premultiplied,
            dpi,
            canvasBitmap);
    }

    IFACEMETHODIMP CanvasBitmapFactory::CreateFromDirect3D11SurfaceWithAlphaAndDpi(
        ICanvasResourceCreator* resourceCreator,
        IDirect3DSurface* surface,
        CanvasAlphaBehavior alpha,
        float dpi,
        ICanvasBitmap** canvasBitmap)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(resourceCreator);
                CheckInPointer(surface);
                CheckAndClearOutPointer(canvasBitmap);

                ComPtr<ICanvasDevice> canvasDevice;
                ThrowIfFailed(resourceCreator->get_Device(&canvasDevice));

                auto newBitmap = GetManager()->CreateBitmapFromSurface(canvasDevice.Get(), surface, alpha, dpi);
                ThrowIfFailed(newBitmap.CopyTo(canvasBitmap));
            });
    }

    IFACEMETHODIMP CanvasBitmapFactory::CreateFromBytes(
        ICanvasResourceCreator* resourceCreator,
        uint32_t byteCount,
        BYTE* bytes,
        float width,
        float height,
        DirectXPixelFormat format,
        CanvasAlphaBehavior alpha,
        ICanvasBitmap** canvasBitmap)
    {
        return CreateFromBytesWithDpi(
            resourceCreator,
            byteCount,
            bytes,
            width,
            height,
            format,
            alpha,
            DEFAULT_DPI,
            canvasBitmap);
    }

    IFACEMETHODIMP CanvasBitmapFactory::CreateFromBytesWithDpi(
        ICanvasResourceCreator* resourceCreator,
        uint32_t byteCount,
        BYTE* bytes,
        float width,
        float height,
        DirectXPixelFormat format,
        CanvasAlphaBehavior alpha,
        float dpi,
        ICanvasBitmap** canvasBitmap)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(resourceCreator);
                CheckInPointer(bytes);
                CheckAndClearOutPointer(canvasBitmap);

                ComPtr<ICanvasDevice> canvasDevice;
                ThrowIfFailed(resourceCreator->get_Device(&canvasDevice));

                auto newBitmap = GetManager()->CreateBitmapFromBytes(
                    canvasDevice.Get(), 
                    byteCount, 
                    bytes, 
                    width, 
                    height, 
                    format, 
                    alpha,
                    dpi);

                ThrowIfFailed(newBitmap.CopyTo(canvasBitmap));
            });
    }

    IFACEMETHODIMP CanvasBitmapFactory::CreateFromColors(
        ICanvasResourceCreator* resourceCreator,
        uint32_t colorCount,
        ABI::Windows::UI::Color* colors,
        float width,
        float height,
        CanvasAlphaBehavior alpha,
        ICanvasBitmap** canvasBitmap)
    {
        return CreateFromColorsWithDpi(
            resourceCreator,
            colorCount,
            colors,
            width,
            height,
            alpha,
            DEFAULT_DPI,
            canvasBitmap);
    }

    IFACEMETHODIMP CanvasBitmapFactory::CreateFromColorsWithDpi(
        ICanvasResourceCreator* resourceCreator,
        uint32_t colorCount,
        ABI::Windows::UI::Color* colors,
        float width,
        float height,
        CanvasAlphaBehavior alpha,
        float dpi,
        ICanvasBitmap** canvasBitmap)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(resourceCreator);
                CheckInPointer(colors);
                CheckAndClearOutPointer(canvasBitmap);

                ComPtr<ICanvasDevice> canvasDevice;
                ThrowIfFailed(resourceCreator->get_Device(&canvasDevice));

                auto newBitmap = GetManager()->CreateBitmapFromColors(
                    canvasDevice.Get(), 
                    colorCount,
                    colors,
                    width, 
                    height, 
                    alpha,
                    dpi);

                ThrowIfFailed(newBitmap.CopyTo(canvasBitmap));
            });
    }

    IFACEMETHODIMP CanvasBitmapFactory::LoadAsyncFromHstring(
        ICanvasResourceCreator* resourceCreator,
        HSTRING fileName,
        ABI::Windows::Foundation::IAsyncOperation<CanvasBitmap*>** canvasBitmapAsyncOperation)
    {
        return LoadAsyncFromHstringWithAlpha(
            resourceCreator,
            fileName,
            CanvasAlphaBehavior::Premultiplied,
            canvasBitmapAsyncOperation);
    }

    IFACEMETHODIMP CanvasBitmapFactory::LoadAsyncFromHstringWithAlpha(
        ICanvasResourceCreator* resourceCreator,
        HSTRING fileName,
        CanvasAlphaBehavior alpha,
        ABI::Windows::Foundation::IAsyncOperation<CanvasBitmap*>** canvasBitmapAsyncOperation)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(resourceCreator);
                CheckInPointer(fileName);
                CheckAndClearOutPointer(canvasBitmapAsyncOperation);

                ComPtr<ICanvasDevice> canvasDevice;
                ThrowIfFailed(resourceCreator->get_Device(&canvasDevice));

                WinString fileName(fileName);

                auto asyncOperation = Make<AsyncOperation<CanvasBitmap>>(
                    [=]
                    {
                        auto bitmap = GetManager()->CreateBitmap(canvasDevice.Get(), fileName, alpha);
                        return bitmap;
                    });

                CheckMakeResult(asyncOperation);
                ThrowIfFailed(asyncOperation.CopyTo(canvasBitmapAsyncOperation));
            });
    }

    IFACEMETHODIMP CanvasBitmapFactory::LoadAsyncFromUri(
        ICanvasResourceCreator* resourceCreator,
        ABI::Windows::Foundation::IUriRuntimeClass* uri,
        ABI::Windows::Foundation::IAsyncOperation<CanvasBitmap*>** canvasBitmapAsyncOperation)
    {
        return LoadAsyncFromUriWithAlpha(
            resourceCreator, 
            uri, 
            CanvasAlphaBehavior::Premultiplied,
            canvasBitmapAsyncOperation);
    }

    template<typename T>
    HRESULT WaitForOperation(IAsyncOperation<T*>* asyncOperation, T** ret)
    {
        // TODO #2617:Investigate making PPL work with async tasks.

        Event emptyEvent(CreateEventEx(NULL, NULL, CREATE_EVENT_MANUAL_RESET, EVENT_ALL_ACCESS));
        if (!emptyEvent.IsValid())
            return E_OUTOFMEMORY;

        ComPtr<T> taskResult;
        HRESULT taskHr = S_OK;
           
        auto callback = Callback<IAsyncOperationCompletedHandler<T*>>(
            [&emptyEvent, &taskResult, &taskHr](IAsyncOperation<T*>* asyncInfo, AsyncStatus status) -> HRESULT
            {
                taskHr = asyncInfo->GetResults(taskResult.GetAddressOf());
                SetEvent(emptyEvent.Get());
                return S_OK;
            });
        asyncOperation->put_Completed(callback.Get());

        auto timeout = 1000 * 5;
        auto waitResult = WaitForSingleObjectEx(emptyEvent.Get(), timeout, true);

        if (waitResult != WAIT_OBJECT_0)
        {
            return E_INVALIDARG;
        }

        taskResult.CopyTo(ret);

        return taskHr;
    };

    IFACEMETHODIMP CanvasBitmapFactory::LoadAsyncFromUriWithAlpha(
        ICanvasResourceCreator* resourceCreator,
        ABI::Windows::Foundation::IUriRuntimeClass* rawUri,
        CanvasAlphaBehavior alpha,
        ABI::Windows::Foundation::IAsyncOperation<CanvasBitmap*>** canvasBitmapAsyncOperation)
    {

        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(resourceCreator);
                CheckInPointer(rawUri);
                CheckAndClearOutPointer(canvasBitmapAsyncOperation);

                ComPtr<ICanvasDevice> canvasDevice;
                ThrowIfFailed(resourceCreator->get_Device(&canvasDevice));

                ComPtr<IRandomAccessStreamReferenceStatics> streamRefStatics;
                ThrowIfFailed(GetActivationFactory(HStringReference(RuntimeClass_Windows_Storage_Streams_RandomAccessStreamReference).Get(), &streamRefStatics));

                ComPtr<IUriRuntimeClass> uri = rawUri;
                
                auto asyncOperation = Make<AsyncOperation<CanvasBitmap>>(
                    [=]
                    {
                        ComPtr<IRandomAccessStreamReference> randomAccessStreamReference = m_adapter->CreateRandomAccessStreamFromUri(uri);

                        ComPtr<IAsyncOperation<IRandomAccessStreamWithContentType*>> randomAccessStreamWithContentTypeOperation;
                        ThrowIfFailed(randomAccessStreamReference->OpenReadAsync(&randomAccessStreamWithContentTypeOperation));

                        ComPtr<IRandomAccessStreamWithContentType> randomAccessStream;                         
                        ThrowIfFailed(WaitForOperation<IRandomAccessStreamWithContentType>(randomAccessStreamWithContentTypeOperation.Get(), &randomAccessStream));

                        ComPtr<IStream> stream;
                        ThrowIfFailed(CreateStreamOverRandomAccessStream(randomAccessStream.Get(), IID_PPV_ARGS(&stream)));

                        auto bitmap = GetManager()->CreateBitmap(canvasDevice.Get(), stream.Get(), alpha);
                        return bitmap;
                    });

                CheckMakeResult(asyncOperation);
                ThrowIfFailed(asyncOperation.CopyTo(canvasBitmapAsyncOperation));
            });
    }

    //
    // ICanvasFactoryNative
    //


    IFACEMETHODIMP CanvasBitmapFactory::GetOrCreate(
        ICanvasDevice* device,
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

                auto newCanvasBitmap = GetManager()->GetOrCreateBitmap(device, d2dBitmap.Get());

                ThrowIfFailed(newCanvasBitmap.CopyTo(wrapper));
            });
    }


    //
    // CanvasBitmap
    //


    CanvasBitmap::CanvasBitmap(
        std::shared_ptr<CanvasBitmapManager> manager,
        ID2D1Bitmap1* d2dBitmap)
        : CanvasBitmapImpl(manager, d2dBitmap)
    {
        assert(!IsRenderTargetBitmap(d2dBitmap) 
            && "CanvasBitmap should never be constructed with a render-target bitmap.  This should have been validated before construction.");
    }


    ActivatableClassWithFactory(CanvasBitmap, CanvasBitmapFactory);
}}}}
