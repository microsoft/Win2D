// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas
{
    using namespace ABI::Windows::Storage::Streams;
    using namespace ABI::Windows::Storage;
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
        float dpi,
        CanvasAlphaMode alpha)
    {
        ComPtr<ICanvasDeviceInternal> canvasDeviceInternal;
        ThrowIfFailed(canvasDevice->QueryInterface(canvasDeviceInternal.GetAddressOf()));

        auto d2dBitmap = canvasDeviceInternal->CreateBitmapFromWicResource(m_adapter->CreateWICFormatConverter(fileName).Get(), dpi, alpha);

        auto bitmap = Make<CanvasBitmap>(
            shared_from_this(),
            d2dBitmap.Get(),
            canvasDevice);
        CheckMakeResult(bitmap);
        
        return bitmap;
    }


    ComPtr<CanvasBitmap> CanvasBitmapManager::CreateNew(
        ICanvasDevice* canvasDevice,
        IStream* fileStream,
        float dpi,
        CanvasAlphaMode alpha)
    {
        ComPtr<ICanvasDeviceInternal> canvasDeviceInternal;
        ThrowIfFailed(canvasDevice->QueryInterface(canvasDeviceInternal.GetAddressOf()));

        auto d2dBitmap = canvasDeviceInternal->CreateBitmapFromWicResource(m_adapter->CreateWICFormatConverter(fileStream).Get(), dpi, alpha);

        auto bitmap = Make<CanvasBitmap>(
            shared_from_this(),
            d2dBitmap.Get(),
            canvasDevice);
        CheckMakeResult(bitmap);
        
        return bitmap;
    }


    ComPtr<CanvasBitmap> CanvasBitmapManager::CreateNew(
        ICanvasDevice* device,
        uint32_t byteCount,
        BYTE* bytes,
        int32_t widthInPixels,
        int32_t heightInPixels,
        DirectXPixelFormat format,
        float dpi,
        CanvasAlphaMode alpha)
    {
        auto deviceContext = As<ICanvasDeviceInternal>(device)->CreateDeviceContext();

        D2D1_BITMAP_PROPERTIES1 bitmapProperties = D2D1::BitmapProperties1();
        bitmapProperties.pixelFormat.alphaMode = ToD2DAlphaMode(alpha);
        bitmapProperties.pixelFormat.format = static_cast<DXGI_FORMAT>(format);
        bitmapProperties.dpiX = dpi;
        bitmapProperties.dpiY = dpi;

        // D2D does not fail attempts to create zero-sized bitmaps. Neither does this.
        uint32_t pitch = 0;
        if (heightInPixels > 0)
        {
            pitch = byteCount / static_cast<uint32_t>(heightInPixels);
        }
        else
        {
            pitch = byteCount;
        }

        ComPtr<ID2D1Bitmap1> d2dBitmap;
        ThrowIfFailed(deviceContext->CreateBitmap(D2D1::SizeU(widthInPixels, heightInPixels), bytes, pitch, &bitmapProperties, &d2dBitmap));

        auto bitmap = Make<CanvasBitmap>(
            shared_from_this(),
            d2dBitmap.Get(),
            device);
        CheckMakeResult(bitmap);

        return bitmap;
    }


    ComPtr<CanvasBitmap> CanvasBitmapManager::CreateNew(
        ICanvasDevice* device,
        uint32_t colorCount,
        Color* colors,
        int32_t widthInPixels,
        int32_t heightInPixels,
        float dpi,
        CanvasAlphaMode alpha)
    {
        // Convert color array to bytes according to the default format, B8G8R8A8_UNORM.
        std::vector<uint8_t> convertedBytes;
        convertedBytes.resize(colorCount * 4);

        for (uint32_t i = 0; i < colorCount; i++)
        {
            convertedBytes[i * 4 + 0] = colors[i].B;
            convertedBytes[i * 4 + 1] = colors[i].G;
            convertedBytes[i * 4 + 2] = colors[i].R;
            convertedBytes[i * 4 + 3] = colors[i].A;
        }

        assert(convertedBytes.size() <= UINT_MAX);

        return CreateNew(
            device,
            static_cast<uint32_t>(convertedBytes.size()),
            convertedBytes.empty() ? nullptr : &convertedBytes[0],
            widthInPixels,
            heightInPixels,
            PIXEL_FORMAT(B8G8R8A8UIntNormalized),
            dpi,
            alpha);
    }


    ComPtr<CanvasBitmap> CanvasBitmapManager::CreateWrapper(
        ICanvasDevice* device,
        ID2D1Bitmap1* d2dBitmap)
    {
        auto bitmap = Make<CanvasBitmap>(
            shared_from_this(),
            d2dBitmap,
            device);
        CheckMakeResult(bitmap);

        return bitmap;
    }


    ICanvasBitmapResourceCreationAdapter* CanvasBitmapManager::GetAdapter()
    {
        return m_adapter.get();
    }

    class DefaultCanvasBitmapAdapter : public ICanvasBitmapAdapter,
                                       private LifespanTracker<DefaultCanvasBitmapAdapter>
    {
        ComPtr<IRandomAccessStreamReferenceStatics> m_randomAccessStreamReferenceStatics;
        ComPtr<IStorageFileStatics> m_storageFileStatics;
    public:
        DefaultCanvasBitmapAdapter()
        {
            ThrowIfFailed(GetActivationFactory(HStringReference(RuntimeClass_Windows_Storage_Streams_RandomAccessStreamReference).Get(), &m_randomAccessStreamReferenceStatics));
            ThrowIfFailed(GetActivationFactory(HStringReference(RuntimeClass_Windows_Storage_StorageFile).Get(), &m_storageFileStatics));
        }

        ComPtr<IRandomAccessStreamReference> CreateRandomAccessStreamFromUri(IUriRuntimeClass* uri) override
        {
            ComPtr<IRandomAccessStreamReference> randomAccessStreamReference;
            ThrowIfFailed(m_randomAccessStreamReferenceStatics->CreateFromUri(uri, &randomAccessStreamReference));

            return randomAccessStreamReference;
        }

        ComPtr<IAsyncOperation<StorageFile*>> GetFileFromPathAsync(HSTRING path) override
        {
            ComPtr<IAsyncOperation<StorageFile*>> task;
            ThrowIfFailed(m_storageFileStatics->GetFileFromPathAsync(path, &task));
            return task;
        }
    };

    
    //
    // ICanvasBitmapStatics
    //

    IFACEMETHODIMP CanvasBitmapFactory::CreateFromDirect3D11Surface(
        ICanvasResourceCreator* resourceCreator,
        IDirect3DSurface* surface,
        ICanvasBitmap** canvasBitmap)
    {
        return CreateFromDirect3D11SurfaceWithDpiAndAlpha(
            resourceCreator,
            surface,
            DEFAULT_DPI,
            CanvasAlphaMode::Premultiplied,
            canvasBitmap);
    }

    IFACEMETHODIMP CanvasBitmapFactory::CreateFromDirect3D11SurfaceWithDpi(
        ICanvasResourceCreator* resourceCreator,
        IDirect3DSurface* surface,
        float dpi,
        ICanvasBitmap** canvasBitmap)
    {
        return CreateFromDirect3D11SurfaceWithDpiAndAlpha(
            resourceCreator,
            surface,
            dpi,
            CanvasAlphaMode::Premultiplied,
            canvasBitmap);
    }

    IFACEMETHODIMP CanvasBitmapFactory::CreateFromDirect3D11SurfaceWithDpiAndAlpha(
        ICanvasResourceCreator* resourceCreator,
        IDirect3DSurface* surface,
        float dpi,
        CanvasAlphaMode alpha,
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

                auto newBitmap = GetManager()->CreateBitmapFromSurface(canvasDevice.Get(), surface, dpi, alpha);
                ThrowIfFailed(newBitmap.CopyTo(canvasBitmap));
            });
    }

    IFACEMETHODIMP CanvasBitmapFactory::CreateFromBytes(
        ICanvasResourceCreator* resourceCreator,
        uint32_t byteCount,
        BYTE* bytes,
        int32_t widthInPixels,
        int32_t heightInPixels,
        DirectXPixelFormat format,
        ICanvasBitmap** canvasBitmap)
    {
        return CreateFromBytesWithDpiAndAlpha(
            resourceCreator,
            byteCount,
            bytes,
            widthInPixels,
            heightInPixels,
            format,
            DEFAULT_DPI,
            CanvasAlphaMode::Premultiplied,
            canvasBitmap);
    }

    IFACEMETHODIMP CanvasBitmapFactory::CreateFromBytesWithDpi(
        ICanvasResourceCreator* resourceCreator,
        uint32_t byteCount,
        BYTE* bytes,
        int32_t widthInPixels,
        int32_t heightInPixels,
        DirectXPixelFormat format,
        float dpi,
        ICanvasBitmap** canvasBitmap)
    {
        return CreateFromBytesWithDpiAndAlpha(
            resourceCreator,
            byteCount,
            bytes,
            widthInPixels,
            heightInPixels,
            format,
            dpi,
            CanvasAlphaMode::Premultiplied,
            canvasBitmap);
    }

    IFACEMETHODIMP CanvasBitmapFactory::CreateFromBytesWithDpiAndAlpha(
        ICanvasResourceCreator* resourceCreator,
        uint32_t byteCount,
        BYTE* bytes,
        int32_t widthInPixels,
        int32_t heightInPixels,
        DirectXPixelFormat format,
        float dpi,
        CanvasAlphaMode alpha,
        ICanvasBitmap** canvasBitmap)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(resourceCreator);
                if (byteCount)
                    CheckInPointer(bytes);
                CheckAndClearOutPointer(canvasBitmap);

                ComPtr<ICanvasDevice> canvasDevice;
                ThrowIfFailed(resourceCreator->get_Device(&canvasDevice));

                auto newBitmap = GetManager()->CreateBitmap(
                    canvasDevice.Get(), 
                    byteCount, 
                    bytes, 
                    widthInPixels,
                    heightInPixels,
                    format, 
                    dpi,
                    alpha);

                ThrowIfFailed(newBitmap.CopyTo(canvasBitmap));
            });
    }

    IFACEMETHODIMP CanvasBitmapFactory::CreateFromColors(
        ICanvasResourceCreator* resourceCreator,
        uint32_t colorCount,
        ABI::Windows::UI::Color* colors,
        int32_t widthInPixels,
        int32_t heightInPixels,
        ICanvasBitmap** canvasBitmap)
    {
        return CreateFromColorsWithDpiAndAlpha(
            resourceCreator,
            colorCount,
            colors,
            widthInPixels,
            heightInPixels,
            DEFAULT_DPI,
            CanvasAlphaMode::Premultiplied,
            canvasBitmap);
    }

    IFACEMETHODIMP CanvasBitmapFactory::CreateFromColorsWithDpi(
        ICanvasResourceCreator* resourceCreator,
        uint32_t colorCount,
        ABI::Windows::UI::Color* colors,
        int32_t widthInPixels,
        int32_t heightInPixels,
        float dpi,
        ICanvasBitmap** canvasBitmap)
    {
        return CreateFromColorsWithDpiAndAlpha(
            resourceCreator,
            colorCount,
            colors,
            widthInPixels,
            heightInPixels,
            dpi,
            CanvasAlphaMode::Premultiplied,
            canvasBitmap);
    }

    IFACEMETHODIMP CanvasBitmapFactory::CreateFromColorsWithDpiAndAlpha(
        ICanvasResourceCreator* resourceCreator,
        uint32_t colorCount,
        ABI::Windows::UI::Color* colors,
        int32_t widthInPixels,
        int32_t heightInPixels,
        float dpi,
        CanvasAlphaMode alpha,
        ICanvasBitmap** canvasBitmap)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(resourceCreator);
                if (colorCount)
                    CheckInPointer(colors);
                CheckAndClearOutPointer(canvasBitmap);

                ComPtr<ICanvasDevice> canvasDevice;
                ThrowIfFailed(resourceCreator->get_Device(&canvasDevice));

                auto newBitmap = GetManager()->CreateBitmap(
                    canvasDevice.Get(), 
                    colorCount,
                    colors,
                    widthInPixels,
                    heightInPixels,
                    dpi,
                    alpha);

                ThrowIfFailed(newBitmap.CopyTo(canvasBitmap));
            });
    }

    IFACEMETHODIMP CanvasBitmapFactory::LoadAsyncFromHstring(
        ICanvasResourceCreator* resourceCreator,
        HSTRING fileName,
        ABI::Windows::Foundation::IAsyncOperation<CanvasBitmap*>** canvasBitmapAsyncOperation)
    {
        return LoadAsyncFromHstringWithDpiAndAlpha(
            resourceCreator,
            fileName,
            DEFAULT_DPI,
            CanvasAlphaMode::Premultiplied,
            canvasBitmapAsyncOperation);
    }

    IFACEMETHODIMP CanvasBitmapFactory::LoadAsyncFromHstringWithDpi(
        ICanvasResourceCreator* resourceCreator,
        HSTRING fileName,
        float dpi,
        ABI::Windows::Foundation::IAsyncOperation<CanvasBitmap*>** canvasBitmapAsyncOperation)
    {
        return LoadAsyncFromHstringWithDpiAndAlpha(
            resourceCreator,
            fileName,
            dpi,
            CanvasAlphaMode::Premultiplied,
            canvasBitmapAsyncOperation);
    }

    IFACEMETHODIMP CanvasBitmapFactory::LoadAsyncFromHstringWithDpiAndAlpha(
        ICanvasResourceCreator* resourceCreator,
        HSTRING rawFileName,
        float dpi,
        CanvasAlphaMode alpha,
        ABI::Windows::Foundation::IAsyncOperation<CanvasBitmap*>** canvasBitmapAsyncOperation)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(resourceCreator);
                CheckInPointer(rawFileName);
                CheckAndClearOutPointer(canvasBitmapAsyncOperation);

                ComPtr<ICanvasDevice> canvasDevice;
                ThrowIfFailed(resourceCreator->get_Device(&canvasDevice));

                WinString fileName(rawFileName);

                auto asyncOperation = Make<AsyncOperation<CanvasBitmap>>(
                    [=]
                    {
                        return GetManager()->CreateBitmap(canvasDevice.Get(), fileName, dpi, alpha);
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
        return LoadAsyncFromUriWithDpiAndAlpha(
            resourceCreator, 
            uri, 
            DEFAULT_DPI,
            CanvasAlphaMode::Premultiplied,
            canvasBitmapAsyncOperation);
    }

    IFACEMETHODIMP CanvasBitmapFactory::LoadAsyncFromUriWithDpi(
        ICanvasResourceCreator* resourceCreator,
        ABI::Windows::Foundation::IUriRuntimeClass* uri,
        float dpi,
        ABI::Windows::Foundation::IAsyncOperation<CanvasBitmap*>** canvasBitmapAsyncOperation)
    {
        return LoadAsyncFromUriWithDpiAndAlpha(
            resourceCreator,
            uri,
            dpi,
            CanvasAlphaMode::Premultiplied,
            canvasBitmapAsyncOperation);
    }
     
    IFACEMETHODIMP CanvasBitmapFactory::LoadAsyncFromUriWithDpiAndAlpha(
        ICanvasResourceCreator* resourceCreator,
        ABI::Windows::Foundation::IUriRuntimeClass* uri,
        float dpi,
        CanvasAlphaMode alpha,
        ABI::Windows::Foundation::IAsyncOperation<CanvasBitmap*>** canvasBitmapAsyncOperation)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(resourceCreator);
                CheckInPointer(uri);
                CheckAndClearOutPointer(canvasBitmapAsyncOperation);

                ComPtr<ICanvasDevice> canvasDevice;
                ThrowIfFailed(resourceCreator->get_Device(&canvasDevice));

                auto adapter = std::make_unique<DefaultCanvasBitmapAdapter>();
                ComPtr<IRandomAccessStreamReference> streamReference = adapter->CreateRandomAccessStreamFromUri(uri);

                // Start opening the file.
                ComPtr<IAsyncOperation<IRandomAccessStreamWithContentType*>> openOperation;
                ThrowIfFailed(streamReference->OpenReadAsync(&openOperation));

                // Our main async operation is constructed as a continuation of the file open,
                // so the lambda will only start executing once the file is ready.
                auto asyncOperation = Make<AsyncOperation<CanvasBitmap>>(openOperation, [=]
                {
                    ComPtr<IRandomAccessStreamWithContentType> randomAccessStream;
                    ThrowIfFailed(openOperation->GetResults(&randomAccessStream));

                    ComPtr<IStream> stream;
                    ThrowIfFailed(CreateStreamOverRandomAccessStream(randomAccessStream.Get(), IID_PPV_ARGS(&stream)));

                    return GetManager()->CreateBitmap(canvasDevice.Get(), stream.Get(), dpi, alpha);
                });

                CheckMakeResult(asyncOperation);
                ThrowIfFailed(asyncOperation.CopyTo(canvasBitmapAsyncOperation));
            });
    }

    IFACEMETHODIMP CanvasBitmapFactory::LoadAsyncFromStream(
        ICanvasResourceCreator* resourceCreator,
        IRandomAccessStream* stream,
        ABI::Windows::Foundation::IAsyncOperation<CanvasBitmap*>** canvasBitmapAsyncOperation)
    {
        return LoadAsyncFromStreamWithDpiAndAlpha(
            resourceCreator,
            stream,
            DEFAULT_DPI,
            CanvasAlphaMode::Premultiplied,
            canvasBitmapAsyncOperation);
    }

    IFACEMETHODIMP CanvasBitmapFactory::LoadAsyncFromStreamWithDpi(
        ICanvasResourceCreator* resourceCreator,
        IRandomAccessStream* stream,
        float dpi,
        ABI::Windows::Foundation::IAsyncOperation<CanvasBitmap*>** canvasBitmapAsyncOperation)
    {
        return LoadAsyncFromStreamWithDpiAndAlpha(
            resourceCreator,
            stream,
            dpi,
            CanvasAlphaMode::Premultiplied,
            canvasBitmapAsyncOperation);
    }

    IFACEMETHODIMP CanvasBitmapFactory::LoadAsyncFromStreamWithDpiAndAlpha(
        ICanvasResourceCreator* resourceCreator,
        IRandomAccessStream* rawStream,
        float dpi,
        CanvasAlphaMode alpha,
        ABI::Windows::Foundation::IAsyncOperation<CanvasBitmap*>** canvasBitmapAsyncOperation)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(resourceCreator);
                CheckInPointer(rawStream);
                CheckAndClearOutPointer(canvasBitmapAsyncOperation);

                ComPtr<ICanvasDevice> canvasDevice;
                ThrowIfFailed(resourceCreator->get_Device(&canvasDevice));

                ComPtr<IRandomAccessStream> stream = rawStream;

                auto asyncOperation = Make<AsyncOperation<CanvasBitmap>>(
                [=]
                {
                    ComPtr<IStream> nativeStream;
                    ThrowIfFailed(CreateStreamOverRandomAccessStream(stream.Get(), IID_PPV_ARGS(&nativeStream)));

                    return GetManager()->CreateBitmap(canvasDevice.Get(), nativeStream.Get(), dpi, alpha);
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
        ID2D1Bitmap1* d2dBitmap,
        ICanvasDevice* device)
        : CanvasBitmapImpl(manager, d2dBitmap, device)
    {
        assert(!IsRenderTargetBitmap(d2dBitmap) 
            && "CanvasBitmap should never be constructed with a render-target bitmap.  This should have been validated before construction.");
    }

    void VerifyWellFormedSubrectangle(D2D1_RECT_U subRectangle, D2D1_SIZE_U targetSize)
    {
        if (subRectangle.right <= subRectangle.left ||
            subRectangle.bottom <= subRectangle.top)
        {
            ThrowHR(E_INVALIDARG);
        }

        if (subRectangle.right > targetSize.width || subRectangle.bottom > targetSize.height)
        {
            ThrowHR(E_INVALIDARG);
        }
    }

    void GetPixelBytesImpl(
        ComPtr<ID2D1Bitmap1> const& d2dBitmap,
        D2D1_RECT_U const& subRectangle,
        uint32_t* valueCount,
        uint8_t** valueElements)
    {
        CheckInPointer(valueCount);
        CheckAndClearOutPointer(valueElements);

        VerifyWellFormedSubrectangle(subRectangle, d2dBitmap->GetPixelSize());

        ScopedBitmapMappedPixelAccess bitmapPixelAccess(d2dBitmap.Get(), D3D11_MAP_READ, &subRectangle);

        const unsigned int bytesPerPixel = GetBytesPerPixel(d2dBitmap->GetPixelFormat().format);
        const unsigned int bytesPerRow = (subRectangle.right - subRectangle.left) * bytesPerPixel;
        const unsigned int destSizeInBytes =
            bytesPerRow * (subRectangle.bottom - subRectangle.top);

        ComArray<BYTE> array(destSizeInBytes);

        byte* destRowStart = array.GetData();
        byte* sourceRowStart = static_cast<byte*>(bitmapPixelAccess.GetLockedData());
        for (unsigned int y = subRectangle.top; y < subRectangle.bottom; y++)
        {
            const unsigned int byteCount = (subRectangle.right - subRectangle.left) * bytesPerPixel;

            assert(destRowStart - array.GetData() < UINT_MAX);
            const unsigned int positionInBuffer = static_cast<unsigned int>(destRowStart - array.GetData());
            const unsigned int bytesLeftInBuffer = destSizeInBytes - positionInBuffer;

            memcpy_s(destRowStart, bytesLeftInBuffer, sourceRowStart, byteCount);

            destRowStart += bytesPerRow;
            sourceRowStart += bitmapPixelAccess.GetStride();
        }

        array.Detach(valueCount, valueElements);
    }

    void GetPixelColorsImpl(
        ComPtr<ID2D1Bitmap1> const& d2dBitmap,
        D2D1_RECT_U const& subRectangle,
        uint32_t* valueCount,
        Color **valueElements)
    {
        CheckInPointer(valueCount);
        CheckAndClearOutPointer(valueElements);

        VerifyWellFormedSubrectangle(subRectangle, d2dBitmap->GetPixelSize());

        if (d2dBitmap->GetPixelFormat().format != DXGI_FORMAT_B8G8R8A8_UNORM)
        {
            ThrowHR(E_INVALIDARG, HStringReference(Strings::PixelColorsFormatRestriction).Get());
        }

        ScopedBitmapMappedPixelAccess bitmapPixelAccess(d2dBitmap.Get(), D3D11_MAP_READ, &subRectangle);

        const unsigned int subRectangleWidth = subRectangle.right - subRectangle.left;
        const unsigned int subRectangleHeight = subRectangle.bottom - subRectangle.top;
        const unsigned int destSizeInPixels = subRectangleWidth * subRectangleHeight;
        ComArray<Color> array(destSizeInPixels);

        byte* sourceRowStart = static_cast<byte*>(bitmapPixelAccess.GetLockedData());

        for (unsigned int y = 0; y < subRectangleHeight; y++)
        {
            for (unsigned int x = 0; x < subRectangleWidth; x++)
            {
                uint32_t sourcePixel = *(reinterpret_cast<uint32_t*>(&sourceRowStart[x * 4]));
                Color& destColor = array[y * subRectangleWidth + x];
                destColor.B = (sourcePixel >> 0) & 0xFF;
                destColor.G = (sourcePixel >> 8) & 0xFF;
                destColor.R = (sourcePixel >> 16) & 0xFF;
                destColor.A = (sourcePixel >> 24) & 0xFF;
            }
            sourceRowStart += bitmapPixelAccess.GetStride();
        }

        array.Detach(valueCount, valueElements);
    }

    void SaveBitmapToFileImpl(
        ComPtr<ID2D1Bitmap1> const& d2dBitmap,
        ICanvasBitmapResourceCreationAdapter* adapter,
        HSTRING rawfileName,
        CanvasBitmapFileFormat fileFormat,
        float quality,
        IAsyncAction **resultAsyncAction)
    {
        WinString fileName(rawfileName);

        auto asyncAction = Make<AsyncAction>(
            [=]
            {
                const D2D1_SIZE_U size = d2dBitmap->GetPixelSize();
                float dpiX, dpiY;
                d2dBitmap->GetDpi(&dpiX, &dpiY);

                ScopedBitmapMappedPixelAccess bitmapPixelAccess(d2dBitmap.Get(), D3D11_MAP_READ);

                adapter->SaveLockedMemoryToFile(
                    fileName,
                    fileFormat,
                    quality,
                    size.width,
                    size.height,
                    dpiX,
                    dpiY,
                    &bitmapPixelAccess);
            });

        CheckMakeResult(asyncAction);
        ThrowIfFailed(asyncAction.CopyTo(resultAsyncAction));
    }

    void SaveBitmapToStreamImpl(
        ComPtr<ID2D1Bitmap1> const& d2dBitmap,
        ICanvasBitmapResourceCreationAdapter* adapter,
        IRandomAccessStream* stream,
        CanvasBitmapFileFormat fileFormat,
        float quality,
        IAsyncAction **resultAsyncAction)
    {
        if (fileFormat == CanvasBitmapFileFormat::Auto)
        {
            ThrowHR(E_INVALIDARG, HStringReference(Strings::AutoFileFormatNotAllowed).Get());
        }

        auto asyncAction = Make<AsyncAction>(
            [=]
            {
                const D2D1_SIZE_U size = d2dBitmap->GetPixelSize();
                float dpiX, dpiY;
                d2dBitmap->GetDpi(&dpiX, &dpiY);

                ScopedBitmapMappedPixelAccess bitmapPixelAccess(d2dBitmap.Get(), D3D11_MAP_READ);

                adapter->SaveLockedMemoryToStream(
                    stream,
                    fileFormat,
                    quality,
                    size.width,
                    size.height,
                    dpiX,
                    dpiY,
                    &bitmapPixelAccess);
            });

        CheckMakeResult(asyncAction);
        ThrowIfFailed(asyncAction.CopyTo(resultAsyncAction));
    }

    void SetPixelBytesImpl(
        ComPtr<ID2D1Bitmap1> const& d2dBitmap,
        D2D1_RECT_U const& subRectangle,
        uint32_t valueCount,
        uint8_t* valueElements)
    {
        CheckInPointer(valueElements);

        VerifyWellFormedSubrectangle(subRectangle, d2dBitmap->GetPixelSize());

        const unsigned int subRectangleWidth = subRectangle.right - subRectangle.left;
        const unsigned int subRectangleHeight = subRectangle.bottom - subRectangle.top;

        const unsigned int bytesPerPixel = GetBytesPerPixel(d2dBitmap->GetPixelFormat().format);
        const unsigned int bytesPerRow = bytesPerPixel * subRectangleWidth;
        uint32_t expectedArraySize = subRectangleWidth * subRectangleHeight * bytesPerPixel;
        if (valueCount != expectedArraySize)
        {
            WinStringBuilder message;
            message.Format(Strings::WrongArrayLength, expectedArraySize, valueCount);
            ThrowHR(E_INVALIDARG, message.Get());
        }

        ScopedBitmapMappedPixelAccess bitmapPixelAccess(d2dBitmap.Get(), D3D11_MAP_WRITE, &subRectangle);

        byte* destRowStart = static_cast<byte*>(bitmapPixelAccess.GetLockedData());
        byte* sourceRowStart = valueElements;

        for (unsigned int y = subRectangle.top; y < subRectangle.bottom; y++)
        {
            const unsigned int byteCount = (subRectangle.right - subRectangle.left) * bytesPerPixel;
            const unsigned int positionInBuffer = static_cast<unsigned int>(sourceRowStart - valueElements);
            const unsigned int bytesLeftInBuffer = valueCount - positionInBuffer;

            memcpy_s(destRowStart, bytesLeftInBuffer, sourceRowStart, byteCount);

            destRowStart += bitmapPixelAccess.GetStride();
            sourceRowStart += bytesPerRow;
        }
    }

    void SetPixelColorsImpl(
        ComPtr<ID2D1Bitmap1> const& d2dBitmap,
        D2D1_RECT_U const& subRectangle,
        uint32_t valueCount,
        Color *valueElements)
    {
        CheckInPointer(valueElements);

        VerifyWellFormedSubrectangle(subRectangle, d2dBitmap->GetPixelSize());

        const unsigned int subRectangleWidth = subRectangle.right - subRectangle.left;
        const unsigned int subRectangleHeight = subRectangle.bottom - subRectangle.top;

        const uint32_t expectedArraySize = subRectangleWidth * subRectangleHeight;
        if (valueCount != expectedArraySize)
        {
            WinStringBuilder message;
            message.Format(Strings::WrongArrayLength, expectedArraySize, valueCount);
            ThrowHR(E_INVALIDARG, message.Get());
        }

        if (d2dBitmap->GetPixelFormat().format != DXGI_FORMAT_B8G8R8A8_UNORM)
        {
            ThrowHR(E_INVALIDARG, HStringReference(Strings::PixelColorsFormatRestriction).Get());
        }

        ScopedBitmapMappedPixelAccess bitmapPixelAccess(d2dBitmap.Get(), D3D11_MAP_WRITE, &subRectangle);

        const unsigned int destSizeInPixels = subRectangleWidth * subRectangleHeight;
        ComArray<Color> array(destSizeInPixels);

        byte* destRowStart = static_cast<byte*>(bitmapPixelAccess.GetLockedData());

        for (unsigned int y = 0; y < subRectangleHeight; y++)
        {
            for (unsigned int x = 0; x < subRectangleWidth; x++)
            {
                uint32_t* destPixel = reinterpret_cast<uint32_t*>(&destRowStart[x * 4]);
                Color& sourceColor = valueElements[y * subRectangleWidth + x];
                *destPixel = 
                    (static_cast<uint32_t>(sourceColor.B) << 0) | 
                    (static_cast<uint32_t>(sourceColor.G) << 8) |
                    (static_cast<uint32_t>(sourceColor.R) << 16) |
                    (static_cast<uint32_t>(sourceColor.A) << 24);
            }
            destRowStart += bitmapPixelAccess.GetStride();
        }
    }


    HRESULT CopyPixelsFromBitmapImpl(
        ICanvasBitmap* to,
        ICanvasBitmap* from,
        int32_t* destX,
        int32_t* destY,
        int32_t* sourceRectLeft,
        int32_t* sourceRectTop,
        int32_t* sourceRectWidth,
        int32_t* sourceRectHeight)
    {
        return ExceptionBoundary(
            [&]
            {
                assert(to);
                CheckInPointer(from);

                auto toBitmapInternal = As<ICanvasBitmapInternal>(to);
                auto toD2dBitmap = toBitmapInternal->GetD2DBitmap();

                auto fromBitmapInternal = As<ICanvasBitmapInternal>(from);
                auto fromD2dBitmap = fromBitmapInternal->GetD2DBitmap();

                bool useDestPt = false;
                D2D1_POINT_2U destPoint;                
                if (destX)
                {
                    assert(destY);
                    useDestPt = true;
                    destPoint = ToD2DPointU(*destX, *destY);
                }

                bool useSourceRect = false;
                D2D1_RECT_U sourceRect;
                if (sourceRectLeft)
                {
                    assert(sourceRectTop && sourceRectWidth && sourceRectHeight && useDestPt);
                    useSourceRect = true;
                    sourceRect = ToD2DRectU(*sourceRectLeft, *sourceRectTop, *sourceRectWidth, *sourceRectHeight);
                }                

                ThrowIfFailed(toD2dBitmap->CopyFromBitmap(
                    useDestPt? &destPoint : nullptr,
                    fromD2dBitmap.Get(),
                    useSourceRect? &sourceRect : nullptr));
            });

    }

    ActivatableClassWithFactory(CanvasBitmap, CanvasBitmapFactory);
}}}}
