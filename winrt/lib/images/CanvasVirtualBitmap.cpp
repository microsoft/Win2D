// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

#if WINVER > _WIN32_WINNT_WINBLUE

#include <WindowsNumerics.h>

#include "CanvasVirtualBitmap.h"


using namespace ABI::Microsoft::Graphics::Canvas;

ActivatableClassWithFactory(CanvasVirtualBitmap, CanvasVirtualBitmapFactory);


IFACEMETHODIMP CanvasVirtualBitmapFactory::LoadAsyncFromFileName(
    ICanvasResourceCreator* resourceCreator,
    HSTRING fileName,
    IAsyncOperation<CanvasVirtualBitmap*>** result)
{
    return LoadAsyncFromFileNameWithOptions(resourceCreator, fileName, CanvasVirtualBitmapOptions::None, result);
}


IFACEMETHODIMP CanvasVirtualBitmapFactory::LoadAsyncFromFileNameWithOptions(
    ICanvasResourceCreator* resourceCreator,
    HSTRING fileName,
    CanvasVirtualBitmapOptions options,
    IAsyncOperation<CanvasVirtualBitmap*>** result)
{
    return LoadAsyncFromFileNameWithOptionsAndAlpha(resourceCreator, fileName, options, CanvasAlphaMode::Premultiplied, result);
}


IFACEMETHODIMP CanvasVirtualBitmapFactory::LoadAsyncFromFileNameWithOptionsAndAlpha(
    ICanvasResourceCreator* rawResourceCreator,
    HSTRING rawFileName,
    CanvasVirtualBitmapOptions options,
    CanvasAlphaMode alphaMode,
    IAsyncOperation<CanvasVirtualBitmap*>** result)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(rawResourceCreator);
            CheckAndClearOutPointer(result);

            WinString fileName(rawFileName);
            ComPtr<ICanvasResourceCreator> resourceCreator(rawResourceCreator);

            auto operation = Make<AsyncOperation<CanvasVirtualBitmap>>(
                [=]
                {
                    auto device = GetCanvasDevice(resourceCreator.Get());
                    auto source = CanvasBitmapAdapter::GetInstance()->CreateWicBitmapSource(device.Get(), fileName, true);
                    return CanvasVirtualBitmap::CreateNew(resourceCreator, source, options, alphaMode);
                });
            CheckMakeResult(operation);
            ThrowIfFailed(operation.CopyTo(result));
        });
}


IFACEMETHODIMP CanvasVirtualBitmapFactory::LoadAsyncFromUri(
    ICanvasResourceCreator* resourceCreator,
    IUriRuntimeClass* uri,
    IAsyncOperation<CanvasVirtualBitmap*>** result)
{
    return LoadAsyncFromUriWithOptions(resourceCreator, uri, CanvasVirtualBitmapOptions::None, result);
}


IFACEMETHODIMP CanvasVirtualBitmapFactory::LoadAsyncFromUriWithOptions(
    ICanvasResourceCreator* resourceCreator,
    IUriRuntimeClass* uri,
    CanvasVirtualBitmapOptions options,
    IAsyncOperation<CanvasVirtualBitmap*>** result)
{
    return LoadAsyncFromUriWithOptionsAndAlpha(resourceCreator, uri, options, CanvasAlphaMode::Premultiplied, result);
}


IFACEMETHODIMP CanvasVirtualBitmapFactory::LoadAsyncFromUriWithOptionsAndAlpha(
    ICanvasResourceCreator* rawResourceCreator,
    IUriRuntimeClass* uri,
    CanvasVirtualBitmapOptions options,
    CanvasAlphaMode alphaMode,
    IAsyncOperation<CanvasVirtualBitmap*>** result)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(rawResourceCreator);
            CheckInPointer(uri);
            CheckAndClearOutPointer(result);

            ComPtr<ICanvasResourceCreator> resourceCreator(rawResourceCreator);
            
            ComPtr<IRandomAccessStreamReferenceStatics> streamReferenceStatics;
            ThrowIfFailed(GetActivationFactory(HStringReference(RuntimeClass_Windows_Storage_Streams_RandomAccessStreamReference).Get(), &streamReferenceStatics));
            
            ComPtr<IRandomAccessStreamReference> streamReference;
            ThrowIfFailed(streamReferenceStatics->CreateFromUri(uri, &streamReference));
            
            // Start opening the file.
            ComPtr<IAsyncOperation<IRandomAccessStreamWithContentType*>> openOperation;
            ThrowIfFailed(streamReference->OpenReadAsync(&openOperation));            

            auto operation = Make<AsyncOperation<CanvasVirtualBitmap>>(openOperation,
                [=]
                {
                    ComPtr<IRandomAccessStreamWithContentType> randomAccessStream;
                    ThrowIfFailed(openOperation->GetResults(&randomAccessStream));

                    ComPtr<IStream> stream;
                    ThrowIfFailed(CreateStreamOverRandomAccessStream(randomAccessStream.Get(), IID_PPV_ARGS(&stream)));

                    auto device = GetCanvasDevice(resourceCreator.Get());
                    auto source = CanvasBitmapAdapter::GetInstance()->CreateWicBitmapSource(device.Get(), stream.Get(), true);
                    return CanvasVirtualBitmap::CreateNew(resourceCreator, source, options, alphaMode);
                });
            CheckMakeResult(operation);
            ThrowIfFailed(operation.CopyTo(result));
        });
}


IFACEMETHODIMP CanvasVirtualBitmapFactory::LoadAsyncFromStream(
    ICanvasResourceCreator* resourceCreator,
    IRandomAccessStream* stream,
    IAsyncOperation<CanvasVirtualBitmap*>** result)
{
    return LoadAsyncFromStreamWithOptions(resourceCreator, stream, CanvasVirtualBitmapOptions::None, result);
}


IFACEMETHODIMP CanvasVirtualBitmapFactory::LoadAsyncFromStreamWithOptions(
    ICanvasResourceCreator* resourceCreator,
    IRandomAccessStream* stream,
    CanvasVirtualBitmapOptions options,
    IAsyncOperation<CanvasVirtualBitmap*>** result)
{
    return LoadAsyncFromStreamWithOptionsAndAlpha(resourceCreator, stream, options, CanvasAlphaMode::Premultiplied, result);
}


IFACEMETHODIMP CanvasVirtualBitmapFactory::LoadAsyncFromStreamWithOptionsAndAlpha(
    ICanvasResourceCreator* rawResourceCreator,
    IRandomAccessStream* rawRandomAccessStream,
    CanvasVirtualBitmapOptions options,
    CanvasAlphaMode alphaMode,
    IAsyncOperation<CanvasVirtualBitmap*>** result)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(rawResourceCreator);
            CheckInPointer(rawRandomAccessStream);
            CheckAndClearOutPointer(result);

            ComPtr<ICanvasResourceCreator> resourceCreator(rawResourceCreator);
            ComPtr<IRandomAccessStream> randomAccessStream(rawRandomAccessStream);

            auto operation = Make<AsyncOperation<CanvasVirtualBitmap>>(
                [=]
                {
                    ComPtr<IStream> stream;
                    ThrowIfFailed(CreateStreamOverRandomAccessStream(randomAccessStream.Get(), IID_PPV_ARGS(&stream)));
                    
                    auto device = GetCanvasDevice(resourceCreator.Get());
                    auto source = CanvasBitmapAdapter::GetInstance()->CreateWicBitmapSource(device.Get(), stream.Get(), true);
                    return CanvasVirtualBitmap::CreateNew(resourceCreator, source, options, alphaMode);
                });
            CheckMakeResult(operation);
            ThrowIfFailed(operation.CopyTo(result));
        });
}


static D2D1_ORIENTATION ToD2DOrientation(WICBitmapTransformOptions transform)
{
    switch (static_cast<uint32_t>(transform))
    {
    case WICBitmapTransformRotate0:                                      return D2D1_ORIENTATION_DEFAULT;
    case WICBitmapTransformRotate270:                                    return D2D1_ORIENTATION_ROTATE_CLOCKWISE270;
    case WICBitmapTransformRotate180:                                    return D2D1_ORIENTATION_ROTATE_CLOCKWISE180;
    case WICBitmapTransformRotate90:                                     return D2D1_ORIENTATION_ROTATE_CLOCKWISE90;
    case WICBitmapTransformFlipHorizontal:                               return D2D1_ORIENTATION_FLIP_HORIZONTAL;
    case WICBitmapTransformFlipVertical:                                 return D2D1_ORIENTATION_ROTATE_CLOCKWISE180_FLIP_HORIZONTAL;
    case WICBitmapTransformRotate270 | WICBitmapTransformFlipHorizontal: return D2D1_ORIENTATION_ROTATE_CLOCKWISE90_FLIP_HORIZONTAL;
    case WICBitmapTransformRotate90 | WICBitmapTransformFlipHorizontal:  return D2D1_ORIENTATION_ROTATE_CLOCKWISE270_FLIP_HORIZONTAL;
    default:
        assert(false);
        return D2D1_ORIENTATION{};
    }
}


ComPtr<CanvasVirtualBitmap> CanvasVirtualBitmap::CreateNew(
    ComPtr<ICanvasResourceCreator> const& resourceCreator,
    WicBitmapSource const& source,
    CanvasVirtualBitmapOptions options,
    CanvasAlphaMode alphaMode)
{
    if (alphaMode == CanvasAlphaMode::Straight)
        ThrowHR(E_INVALIDARG, Strings::InvalidAlphaModeForImageSource);
    
    ComPtr<ICanvasDevice> device;
    ThrowIfFailed(resourceCreator->get_Device(&device));

    auto deviceContext = As<ICanvasDeviceInternal>(device)->GetResourceCreationDeviceContext();
    auto deviceContext2 = As<ID2D1DeviceContext2>(deviceContext.Get());

    D2D1_IMAGE_SOURCE_LOADING_OPTIONS loadingOptions = D2D1_IMAGE_SOURCE_LOADING_OPTIONS_NONE;

    switch (options)
    {
    case CanvasVirtualBitmapOptions::ReleaseSource:
        loadingOptions = D2D1_IMAGE_SOURCE_LOADING_OPTIONS_RELEASE_SOURCE;
        break;
        
    case CanvasVirtualBitmapOptions::CacheOnDemand:
        if (source.Indexed)
        {
            // We only honor CacheOnDemand if the provided image source is
            // actually indexed, since CacheOnDemand results in extremely poor
            // performance when used with unindexed images.
            loadingOptions = D2D1_IMAGE_SOURCE_LOADING_OPTIONS_CACHE_ON_DEMAND;
        }
        break;
    }

    ComPtr<ID2D1ImageSourceFromWic> imageSourceFromWic;
    ThrowIfFailed(deviceContext2->CreateImageSourceFromWic(
        source.Source.Get(),
        loadingOptions,
        ToD2DAlphaMode(alphaMode),
        &imageSourceFromWic));

    auto d2dOrientation = ToD2DOrientation(source.Transform);

    ComPtr<ID2D1Image> imageSource;
    if (d2dOrientation == D2D1_ORIENTATION_DEFAULT)
    {
        imageSource = imageSourceFromWic;
    }
    else
    {
        D2D1_TRANSFORMED_IMAGE_SOURCE_PROPERTIES properties
        {
            ToD2DOrientation(source.Transform),
            1.0f,
            1.0f,
            D2D1_INTERPOLATION_MODE_LINEAR,
            D2D1_TRANSFORMED_IMAGE_SOURCE_OPTIONS_NONE
        };
        
        ComPtr<ID2D1TransformedImageSource> transformedImageSource;
        ThrowIfFailed(deviceContext2->CreateTransformedImageSource(
            imageSourceFromWic.Get(),
            &properties,
            &transformedImageSource));

        imageSource = transformedImageSource;
    }

    D2D1_RECT_F localBounds;
    ThrowIfFailed(deviceContext->GetImageLocalBounds(imageSource.Get(), &localBounds));

    auto virtualBitmap = Make<CanvasVirtualBitmap>(
        device.Get(),
        imageSource.Get(),
        imageSourceFromWic.Get(),
        FromD2DRect(localBounds),
        d2dOrientation);
    CheckMakeResult(virtualBitmap);
    
    return virtualBitmap;
}


// This constructor is called by CreateNew
CanvasVirtualBitmap::CanvasVirtualBitmap(
    ICanvasDevice* device,
    ID2D1Image* imageSource,
    ID2D1ImageSourceFromWic* imageSourceFromWic,
    Rect localBounds,
    D2D1_ORIENTATION orientation)
    : ResourceWrapper(imageSource)
    , m_device(device)
    , m_imageSourceFromWic(imageSourceFromWic)
    , m_localBounds(localBounds)
    , m_orientation(orientation)
{
}


// The next constructor is called by interop; these helpers probe the wrapped
// resource to figure out things we need to know about them.

static ComPtr<ID2D1ImageSourceFromWic> FindImageSourceFromWic(ID2D1Image* image)
{
    auto imageSource = MaybeAs<ID2D1ImageSourceFromWic>(image);
    if (imageSource)
        return imageSource;

    auto transformed = MaybeAs<ID2D1TransformedImageSource>(image);
    if (transformed)
    {
        ComPtr<ID2D1ImageSource> transformedSource;
        transformed->GetSource(&transformedSource);

        imageSource = MaybeAs<ID2D1ImageSourceFromWic>(transformedSource);
        if (imageSource)
            return imageSource;
    }

    return nullptr;
}


static Rect GetLocalBounds(ICanvasDevice* device, ID2D1Image* image)
{
    auto deviceContext = As<ICanvasDeviceInternal>(device)->GetResourceCreationDeviceContext();

    D2D1_RECT_F bounds;
    ThrowIfFailed(deviceContext->GetImageLocalBounds(image, &bounds));

    return FromD2DRect(bounds);
}


static D2D1_ORIENTATION GetOrientation(ID2D1Image* image)
{
    auto transformed = MaybeAs<ID2D1TransformedImageSource>(image);

    if (!transformed)
        return D2D1_ORIENTATION_DEFAULT;

    D2D1_TRANSFORMED_IMAGE_SOURCE_PROPERTIES properties;
    transformed->GetProperties(&properties);

    return properties.orientation;
}


CanvasVirtualBitmap::CanvasVirtualBitmap(
    ICanvasDevice* device,
    ID2D1Image* imageSource)
    : CanvasVirtualBitmap(
        device,
        imageSource,
        FindImageSourceFromWic(imageSource).Get(),
        GetLocalBounds(device, imageSource),
        GetOrientation(imageSource))
{
}


IFACEMETHODIMP CanvasVirtualBitmap::Close()
{
    HRESULT hr = ResourceWrapper::Close();
    if (FAILED(hr))
        return hr;

    m_device.Reset();
    m_imageSourceFromWic.Reset();
    
    return S_OK;
}


IFACEMETHODIMP CanvasVirtualBitmap::get_Device(ICanvasDevice** value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckAndClearOutPointer(value);
            ThrowIfFailed(m_device.CopyTo(value));
        });
}

//
// ICanvasImageInterop
//

IFACEMETHODIMP CanvasVirtualBitmap::GetDevice(ICanvasDevice** device, WIN2D_GET_DEVICE_ASSOCIATION_TYPE* type)
{
    return ExceptionBoundary(
        [&]
        {
            CheckAndClearOutPointer(device);
            CheckInPointer(type);

            *type = WIN2D_GET_DEVICE_ASSOCIATION_TYPE_UNSPECIFIED;

            ThrowIfFailed(m_device.CopyTo(device));

            // Just like a normal bitmap, a virtualized bitmap is also uniquely tied to its owning device.
            *type = WIN2D_GET_DEVICE_ASSOCIATION_TYPE_CREATION_DEVICE;
        });
}


IFACEMETHODIMP CanvasVirtualBitmap::get_IsCachedOnDemand(boolean* value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(value);

            if (!m_imageSourceFromWic)
                ThrowHR(E_FAIL);

            // We probe the image source to determine whether or not it supports
            // demand caching by calling EnsureCached on a zero sized region.
            //
            // This will fail quickly if the image source doesn't support cache
            // on demand.
            HRESULT hr = m_imageSourceFromWic->EnsureCached(D2D1_RECT_U{ 0, 0, 0, 0 });

            if (hr == D2DERR_UNSUPPORTED_OPERATION)
                *value = FALSE;
            else
                *value = TRUE;
        });
}


IFACEMETHODIMP CanvasVirtualBitmap::get_SizeInPixels(BitmapSize* value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(value);

            // CanvasVirtualBitmap is always 96 DPI, so we can directly convert
            // from DIPS to pixels.
            *value = BitmapSize{
                static_cast<uint32_t>(m_localBounds.Width),
                static_cast<uint32_t>(m_localBounds.Height)
            };
        });
}


IFACEMETHODIMP CanvasVirtualBitmap::get_Size(Size* value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(value);

            *value = Size{ m_localBounds.Width, m_localBounds.Height };
        });
}


IFACEMETHODIMP CanvasVirtualBitmap::get_Bounds(Rect* value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(value);

            *value = m_localBounds;
        });
}


IFACEMETHODIMP CanvasVirtualBitmap::GetBounds(ICanvasResourceCreator* rc, Rect* bounds)
{
    return GetImageBoundsImpl(this, rc, nullptr, bounds);
}


IFACEMETHODIMP CanvasVirtualBitmap::GetBoundsWithTransform(ICanvasResourceCreator* rc, Matrix3x2 transform, Rect* bounds)
{
    return GetImageBoundsImpl(this, rc, &transform, bounds);
}


ComPtr<ID2D1Image> CanvasVirtualBitmap::GetD2DImage(ICanvasDevice* , ID2D1DeviceContext*, WIN2D_GET_D2D_IMAGE_FLAGS, float, float* realizedDpi)
{
    if (realizedDpi)
        *realizedDpi = 0;

    return GetResource();
}


#endif