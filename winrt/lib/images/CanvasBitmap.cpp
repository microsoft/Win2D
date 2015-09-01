// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"
#include <propkey.h>

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas
{
    using namespace ABI::Windows::Storage::Streams;
    using namespace ABI::Windows::Storage;
    using namespace ::Microsoft::WRL::Wrappers;

#if WINVER > _WIN32_WINNT_WINBLUE
    using ABI::Windows::Graphics::Imaging::BitmapPixelFormat;
#endif


    static GUID GetGUIDForFileFormat(CanvasBitmapFileFormat fileFormat)
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

    static GUID GetEncoderFromFileExtension(
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

    static void SaveLockedMemoryToNativeStream(
        IWICImagingFactory2* wicFactory,
        IWICStream* nativeStream,
        GUID encoderGuid,
        float quality,
        unsigned int width,
        unsigned int height,
        float dpiX,
        float dpiY,
        ScopedBitmapMappedPixelAccess* bitmapLock)
    {
        ComPtr<IWICBitmapEncoder> encoder;
        ThrowIfFailed(wicFactory->CreateEncoder(encoderGuid, NULL, &encoder));

        ThrowIfFailed(encoder->Initialize(nativeStream, WICBitmapEncoderNoCache));

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
        ThrowIfFailed(wicFactory->CreateBitmapFromMemory(
            width,
            height,
            GUID_WICPixelFormat32bppBGRA,
            bitmapLock->GetStride(),
            bitmapLock->GetLockedBufferSize(),
            static_cast<BYTE*>(bitmapLock->GetLockedData()),
            &memoryBitmap));

        WICPixelFormatGUID pixelFormat;
        ThrowIfFailed(memoryBitmap->GetPixelFormat(&pixelFormat));
        ThrowIfFailed(frameEncode->SetPixelFormat(&pixelFormat));

        ThrowIfFailed(frameEncode->SetResolution(dpiX, dpiY));

        ThrowIfFailed(frameEncode->WriteSource(memoryBitmap.Get(), NULL));
        ThrowIfFailed(frameEncode->Commit());
        ThrowIfFailed(encoder->Commit());
    }


    //
    // DefaultBitmapResourceCreationAdapter
    //

    class DefaultBitmapResourceCreationAdapter : public ICanvasBitmapResourceCreationAdapter,
                                                 private LifespanTracker<DefaultBitmapResourceCreationAdapter>
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

        void SaveLockedMemoryToStream(
            IRandomAccessStream* randomAccessStream,
            CanvasBitmapFileFormat fileFormat,
            float quality,
            unsigned int width,
            unsigned int height,
            float dpiX,
            float dpiY,
            ScopedBitmapMappedPixelAccess* bitmapLock)
        {
            ComPtr<IWICStream> wicStream;
            ThrowIfFailed(m_wicFactory->CreateStream(&wicStream));

            ComPtr<IStream> iStream;
            ThrowIfFailed(CreateStreamOverRandomAccessStream(randomAccessStream, IID_PPV_ARGS(&iStream)));

            ThrowIfFailed(wicStream->InitializeFromIStream(iStream.Get()));

            SaveLockedMemoryToNativeStream(
                m_wicFactory.Get(), 
                wicStream.Get(), 
                GetGUIDForFileFormat(fileFormat),
                quality, 
                width, 
                height, 
                dpiX,
                dpiY, 
                bitmapLock);
        }

        void SaveLockedMemoryToFile(
            HSTRING fileName,
            CanvasBitmapFileFormat fileFormat,
            float quality,
            unsigned int width,
            unsigned int height,
            float dpiX,
            float dpiY,
            ScopedBitmapMappedPixelAccess* bitmapLock)
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

            ComPtr<IWICStream> wicStream;
            ThrowIfFailed(m_wicFactory->CreateStream(&wicStream));

            ThrowIfFailed(wicStream->InitializeFromFilename(static_cast<const wchar_t*>(fileNameString), GENERIC_WRITE));

            SaveLockedMemoryToNativeStream(
                m_wicFactory.Get(),
                wicStream.Get(), 
                encoderGuid, 
                quality, 
                width, 
                height, 
                dpiX, 
                dpiY, 
                bitmapLock);
        }

        UINT GetOrientationFromFrameDecode(ComPtr<IWICBitmapFrameDecode> const& frameDecode)
        {
            ComPtr<IWICMetadataQueryReader> queryReader;

            HRESULT getMetadataHr = frameDecode->GetMetadataQueryReader(&queryReader);
            if (getMetadataHr == WINCODEC_ERR_UNSUPPORTEDOPERATION)
            {
                // This is expected for some formats, such as BMP.
                return PHOTO_ORIENTATION_NORMAL;
            }
            else
                ThrowIfFailed(getMetadataHr);

            PROPVARIANT orientation;
            PropVariantInit(&orientation);
            
            HRESULT lookupHr = queryReader->GetMetadataByName(L"System.Photo.Orientation", &orientation);

            const bool propertyNotFound = 
                lookupHr == WINCODEC_ERR_PROPERTYNOTFOUND ||
                lookupHr == WINCODEC_ERR_PROPERTYNOTSUPPORTED;

            // Anything other than property-not-found is unexpected
            if (!propertyNotFound)
                ThrowIfFailed(lookupHr);

            // The property wasn't specified, or specifies an out-of-range value
            if (propertyNotFound ||
                orientation.vt != VT_UI2 ||
                orientation.uiVal < 1 ||
                orientation.uiVal > 8)
            {                
                return PHOTO_ORIENTATION_NORMAL;
            }
            else
            {
                return orientation.uiVal;
            }
        }

        WICBitmapTransformOptions GetTransformOptionsFromPhotoOrientation(
            UINT photoOrientation)
        {
            switch (photoOrientation)
            {
            case PHOTO_ORIENTATION_NORMAL: return WICBitmapTransformRotate0;
            case PHOTO_ORIENTATION_ROTATE90: return WICBitmapTransformRotate270;
            case PHOTO_ORIENTATION_ROTATE180: return WICBitmapTransformRotate180;
            case PHOTO_ORIENTATION_ROTATE270: return WICBitmapTransformRotate90;
            case PHOTO_ORIENTATION_FLIPHORIZONTAL: return WICBitmapTransformFlipHorizontal;
            case PHOTO_ORIENTATION_FLIPVERTICAL: return WICBitmapTransformFlipVertical;
            case PHOTO_ORIENTATION_TRANSPOSE: return static_cast<WICBitmapTransformOptions>(WICBitmapTransformRotate270 | WICBitmapTransformFlipHorizontal);
            case PHOTO_ORIENTATION_TRANSVERSE: return static_cast<WICBitmapTransformOptions>(WICBitmapTransformRotate90 | WICBitmapTransformFlipHorizontal);
            default:
                ThrowHR(E_INVALIDARG);
            }
        }

        ComPtr<IWICBitmapSource> CreateFlipRotator(
            IWICImagingFactory* wicImagingFactory,
            IWICBitmapSource* bitmapSource,
            WICBitmapTransformOptions transformOptions)
        {
            ComPtr<IWICBitmap> bitmap;
            if (transformOptions != WICBitmapTransformRotate0)
            {

                ComPtr<IWICBitmapFlipRotator> bitmapFlipRotator;
                ThrowIfFailed(wicImagingFactory->CreateBitmapFlipRotator(&bitmapFlipRotator));
                ThrowIfFailed(bitmapFlipRotator->Initialize(bitmapSource, transformOptions));

                // WICBitmapCacheOnLoad is required so that the entire destination buffer is provided to the 
                // flip/rotator.  Requesting the entire desination in one CopyPixels call enables optimizations.    
                ThrowIfFailed(wicImagingFactory->CreateBitmapFromSource(bitmapFlipRotator.Get(), WICBitmapCacheOnLoad, &bitmap));
            }
            else
            {
                // No rotation needed, just cache the source.
                // Note that because this is passed directly into D2D CreateBitmapFromWicBitmap,
                // this doesn't introduce any additional memory costs. 
                // Otherwise, if at some point this changes, we should remove this step.
                //
                ThrowIfFailed(wicImagingFactory->CreateBitmapFromSource(bitmapSource, WICBitmapCacheOnLoad, &bitmap));
            }

            return bitmap;
        }

        ComPtr<IWICBitmapSource> ApplyRotationIfNeeded(ComPtr<IWICBitmapFrameDecode> const& wicBitmapFrameSource, ComPtr<IWICFormatConverter>& postFormatConversion)
        {
            const UINT orientation = GetOrientationFromFrameDecode(wicBitmapFrameSource);

            const WICBitmapTransformOptions transformOptions = GetTransformOptionsFromPhotoOrientation(orientation);

            const ComPtr<IWICBitmapSource> rotator = CreateFlipRotator(m_wicFactory.Get(), postFormatConversion.Get(), transformOptions);

            return rotator;
        }

        ComPtr<IWICBitmapSource> CreateWICFormatConverter(HSTRING fileName)
        {
            ComPtr<IWICStream> stream;
            ThrowIfFailed(m_wicFactory->CreateStream(&stream));

            WinString fileNameString(fileName);
            ThrowIfFailed(stream->InitializeFromFilename(static_cast<const wchar_t*>(fileNameString), GENERIC_READ));

            return CreateWICFormatConverter(stream.Get());
        }

        ComPtr<IWICBitmapSource> CreateWICFormatConverter(IStream* fileStream)
        {
            ComPtr<IWICFormatConverter> wicFormatConverter;

            ComPtr<IWICBitmapDecoder> wicBitmapDecoder;
            ThrowIfFailed(m_wicFactory->CreateDecoderFromStream(
                fileStream,
                nullptr,
                WICDecodeMetadataCacheOnDemand,
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

            return ApplyRotationIfNeeded(wicBitmapFrameSource, wicFormatConverter);
        }
    };


    //
    // CanvasBitmapManager
    //

    CanvasBitmapManager::CanvasBitmapManager()
        : m_adapter(std::make_shared<DefaultBitmapResourceCreationAdapter>())
    {
    }


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
        float dpi,
        DirectXPixelFormat format,
        CanvasAlphaMode alpha)
    {
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

        auto d2dBitmap = As<ICanvasDeviceInternal>(device)->CreateBitmapFromBytes(
            bytes,
            pitch,
            widthInPixels,
            heightInPixels,
            dpi,
            format,
            alpha);

        auto bitmap = Make<CanvasBitmap>(
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
            dpi,
            PIXEL_FORMAT(B8G8R8A8UIntNormalized),
            alpha);
    }


#if WINVER > _WIN32_WINNT_WINBLUE

    //
    // BitmapPixelFormat's values are intended to be reinterpreted as
    // DXGI_FORMAT values.  Unfortunately, some of the chosen color values map
    // to UINT rather than UNORM formats.  The intended use is UNORM, and so we
    // fudge the BitmapPixelFormat values in these cases.
    //
    static DirectXPixelFormat GetFudgedFormat(BitmapPixelFormat format)
    {
        using namespace ABI::Windows::Graphics::Imaging;
        
        switch (format)
        {
        case BitmapPixelFormat_Rgba16: return PIXEL_FORMAT(R16G16B16A16UIntNormalized);
        case BitmapPixelFormat_Rgba8:  return PIXEL_FORMAT(R8G8B8A8UIntNormalized);
        case BitmapPixelFormat_Gray8:  return PIXEL_FORMAT(A8UIntNormalized);

        case BitmapPixelFormat_Bgra8: // BitmapPixelFormat already uses the UNORM value here
        default:
            return static_cast<DirectXPixelFormat>(format);
        }        
    }


    ComPtr<CanvasBitmap> CanvasBitmapManager::CreateNew(
        ICanvasDevice* device,
        ISoftwareBitmap* sourceBitmap)
    {
        using namespace ABI::Windows::Graphics::Imaging;
        using ::Windows::Foundation::IMemoryBufferByteAccess;

        //
        // Extract information from the SoftwareBitmap
        //

        BitmapPixelFormat bitmapPixelFormat;
        ThrowIfFailed(sourceBitmap->get_BitmapPixelFormat(&bitmapPixelFormat));

        BitmapAlphaMode bitmapAlphaMode;
        ThrowIfFailed(sourceBitmap->get_BitmapAlphaMode(&bitmapAlphaMode));

        double dpiX;
        ThrowIfFailed(sourceBitmap->get_DpiX(&dpiX));

        ComPtr<IBitmapBuffer> bitmapBuffer;
        ThrowIfFailed(sourceBitmap->LockBuffer(BitmapBufferAccessMode_Read, &bitmapBuffer));

        ComPtr<IMemoryBufferReference> memoryBuffer;
        ThrowIfFailed(As<IMemoryBuffer>(bitmapBuffer)->CreateReference(&memoryBuffer));

        uint32_t bufferSize;
        uint8_t* buffer;
        ThrowIfFailed(As<IMemoryBufferByteAccess>(memoryBuffer)->GetBuffer(&buffer, &bufferSize));

        BitmapPlaneDescription bitmapPlaneDescription;
        ThrowIfFailed(bitmapBuffer->GetPlaneDescription(0, &bitmapPlaneDescription));

        auto pixelWidth = static_cast<uint32_t>(bitmapPlaneDescription.Width);
        auto pixelHeight = static_cast<uint32_t>(bitmapPlaneDescription.Height);

        //
        // Create an ID2D1Bitmap1 from the source bitmap's data
        //

        auto d2dBitmap = As<ICanvasDeviceInternal>(device)->CreateBitmapFromBytes(
            buffer + bitmapPlaneDescription.StartIndex,
            bitmapPlaneDescription.Stride,
            pixelWidth,
            pixelHeight,
            static_cast<float>(dpiX),
            GetFudgedFormat(bitmapPixelFormat),
            ToCanvasAlphaMode(bitmapAlphaMode));

        //
        // Wrap a CanvasBitmap around it
        //

        auto bitmap = Make<CanvasBitmap>(
            d2dBitmap.Get(),
            device);
        CheckMakeResult(bitmap);

        return bitmap;
    }

#endif


    ComPtr<CanvasBitmap> CanvasBitmapManager::CreateWrapper(
        ICanvasDevice* device,
        ID2D1Bitmap1* d2dBitmap)
    {
        auto bitmap = Make<CanvasBitmap>(
            d2dBitmap,
            device);
        CheckMakeResult(bitmap);

        return bitmap;
    }


    std::shared_ptr<ICanvasBitmapResourceCreationAdapter> CanvasBitmapManager::GetAdapter()
    {
        return m_adapter;
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
        return CreateFromDirect3D11SurfaceImpl(
            resourceCreator,
            surface,
            DEFAULT_DPI,
            nullptr,
            canvasBitmap);
    }

    IFACEMETHODIMP CanvasBitmapFactory::CreateFromDirect3D11SurfaceWithDpi(
        ICanvasResourceCreator* resourceCreator,
        IDirect3DSurface* surface,
        float dpi,
        ICanvasBitmap** canvasBitmap)
    {
        return CreateFromDirect3D11SurfaceImpl(
            resourceCreator,
            surface,
            dpi,
            nullptr,
            canvasBitmap);
    }

    IFACEMETHODIMP CanvasBitmapFactory::CreateFromDirect3D11SurfaceWithDpiAndAlpha(
        ICanvasResourceCreator* resourceCreator,
        IDirect3DSurface* surface,
        float dpi,
        CanvasAlphaMode alpha,
        ICanvasBitmap** canvasBitmap)
    {
        return CreateFromDirect3D11SurfaceImpl(
            resourceCreator,
            surface,
            dpi,
            &alpha,
            canvasBitmap);
    }

    static bool DoesSurfaceSupportAlpha(IDirect3DSurface* surface)
    {
        Direct3DSurfaceDescription surfaceDescription;
        ThrowIfFailed(surface->get_Description(&surfaceDescription));

        return surfaceDescription.Format != PIXEL_FORMAT(B8G8R8X8UIntNormalized)
            && surfaceDescription.Format != PIXEL_FORMAT(R8G8UIntNormalized)
            && surfaceDescription.Format != PIXEL_FORMAT(R8UIntNormalized);
    }

    HRESULT CanvasBitmapFactory::CreateFromDirect3D11SurfaceImpl(
        ICanvasResourceCreator* resourceCreator,
        IDirect3DSurface* surface,
        float dpi,
        CanvasAlphaMode const* requestedAlpha,
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

                CanvasAlphaMode alpha;
                if (requestedAlpha)
                {
                    alpha = *requestedAlpha;
                }
                else
                {
                    alpha = DoesSurfaceSupportAlpha(surface) ? CanvasAlphaMode::Premultiplied : CanvasAlphaMode::Ignore;
                }

                auto d2dBitmap = As<ICanvasDeviceInternal>(canvasDevice)->CreateBitmapFromSurface(surface, dpi, alpha);

                auto newBitmap = ResourceManager::GetOrCreate<ICanvasBitmap>(canvasDevice.Get(), d2dBitmap.Get());

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

                auto newBitmap = GetManager()->CreateNew(
                    canvasDevice.Get(), 
                    byteCount, 
                    bytes, 
                    widthInPixels,
                    heightInPixels,
                    dpi,
                    format, 
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

                auto newBitmap = GetManager()->CreateNew(
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

#if WINVER > _WIN32_WINNT_WINBLUE

    IFACEMETHODIMP CanvasBitmapFactory::CreateFromSoftwareBitmap(
        ICanvasResourceCreator* resourceCreator,
        ISoftwareBitmap* sourceBitmap,
        ICanvasBitmap** canvasBitmap)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(resourceCreator);
                CheckInPointer(sourceBitmap);
                CheckAndClearOutPointer(canvasBitmap);

                ComPtr<ICanvasDevice> canvasDevice;
                ThrowIfFailed(resourceCreator->get_Device(&canvasDevice));

                auto newBitmap = GetManager()->CreateNew(
                    canvasDevice.Get(),
                    sourceBitmap);

                ThrowIfFailed(newBitmap.CopyTo(canvasBitmap));
            });
    }

#endif

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
                        return GetManager()->CreateNew(canvasDevice.Get(), fileName, dpi, alpha);
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

                    return GetManager()->CreateNew(canvasDevice.Get(), stream.Get(), dpi, alpha);
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

                    return GetManager()->CreateNew(canvasDevice.Get(), nativeStream.Get(), dpi, alpha);
                });

                CheckMakeResult(asyncOperation);
                ThrowIfFailed(asyncOperation.CopyTo(canvasBitmapAsyncOperation));
            });
    }


    //
    // CanvasBitmap
    //

    CanvasBitmap::CanvasBitmap(
        ID2D1Bitmap1* d2dBitmap,
        ICanvasDevice* device)
        : CanvasBitmapImpl(d2dBitmap, device)
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
        std::shared_ptr<ICanvasBitmapResourceCreationAdapter> adapter,
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
        std::shared_ptr<ICanvasBitmapResourceCreationAdapter> adapter,
        ComPtr<IRandomAccessStream> const& stream,
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
                    stream.Get(),
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
