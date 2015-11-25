// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

#if WINVER > _WIN32_WINNT_WINBLUE

#include <WindowsNumerics.h>

#include "CanvasVirtualBitmap.h"


using namespace ABI::Microsoft::Graphics::Canvas;

ActivatableClassWithFactory(CanvasVirtualBitmap, CanvasVirtualBitmapFactory);


class ABI::Microsoft::Graphics::Canvas::DefaultVirtualBitmapAdapter : public CanvasVirtualBitmapAdapter
{
public:
    virtual ComPtr<IAsyncAction> RunAsync(std::function<void()>&& fn) override
    {
        auto action = Make<AsyncAction>(std::move(fn));
        CheckMakeResult(action);
        return action;
    }
};


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
                    auto source = CanvasBitmapAdapter::GetInstance()->CreateWicBitmapSource(fileName, true);
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

                    auto source = CanvasBitmapAdapter::GetInstance()->CreateWicBitmapSource(stream.Get(), true);
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
                    
                    auto source = CanvasBitmapAdapter::GetInstance()->CreateWicBitmapSource(stream.Get(), true);
                    return CanvasVirtualBitmap::CreateNew(resourceCreator, source, options, alphaMode);
                });
            CheckMakeResult(operation);
            ThrowIfFailed(operation.CopyTo(result));
        });
}


static D2D1_ORIENTATION ToD2DOrientation(WICBitmapTransformOptions transform)
{
    switch (transform)
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
    bool cacheMethodsEnabled = false;

    switch (options)
    {
    case CanvasVirtualBitmapOptions::ReleaseSource:
        loadingOptions = D2D1_IMAGE_SOURCE_LOADING_OPTIONS_RELEASE_SOURCE;
        break;
        
    case CanvasVirtualBitmapOptions::CacheOnDemand:
        cacheMethodsEnabled = true;
        if (source.Indexed)
        {
            // We only honor CacheOnDemand if the provided image source is
            // actually indexed, since CacheOnDemand results in extremely poor
            // performance.
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
        d2dOrientation,
        cacheMethodsEnabled);
    CheckMakeResult(virtualBitmap);
    
    return virtualBitmap;
}


// This constructor is called by CreateNew
CanvasVirtualBitmap::CanvasVirtualBitmap(
    ICanvasDevice* device,
    ID2D1Image* imageSource,
    ID2D1ImageSourceFromWic* imageSourceFromWic,
    Rect localBounds,
    D2D1_ORIENTATION orientation,
    bool cacheMethodsEnabled)
    : ResourceWrapper(imageSource)
    , m_device(device)
    , m_imageSourceFromWic(imageSourceFromWic)
    , m_localBounds(localBounds)
    , m_orientation(orientation)
    , m_cacheMethodsEnabled(cacheMethodsEnabled)
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
        GetOrientation(imageSource),
        true)
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


IFACEMETHODIMP CanvasVirtualBitmap::EnsureCachedAsync(IAsyncAction** result)
{
    return ExceptionBoundary(
        [&]
        {
            ValidateCanCallCacheMethods();
            
            ComPtr<CanvasVirtualBitmap> self(this);
                
            auto action = CanvasVirtualBitmapAdapter::GetInstance()->RunAsync(
                [self]
                {
                    ThrowIfFailed(self->m_imageSourceFromWic->EnsureCached(nullptr));
                });
            ThrowIfFailed(action.CopyTo(result));
        });
}


IFACEMETHODIMP CanvasVirtualBitmap::EnsureCachedAsyncWithRegion(Rect region, IAsyncAction** result)
{
    return ExceptionBoundary(
        [&]
        {
            ValidateCanCallCacheMethods();
            
            ComPtr<CanvasVirtualBitmap> self(this);
                
            auto action = CanvasVirtualBitmapAdapter::GetInstance()->RunAsync(
                [self, region]
                {
                    auto d2dRect = self->CalculateBackTransformedRegion(region);
            
                    ThrowIfFailed(self->m_imageSourceFromWic->EnsureCached(&d2dRect));
                });
            ThrowIfFailed(action.CopyTo(result));
        });
}


IFACEMETHODIMP CanvasVirtualBitmap::TrimCache()
{
    return ExceptionBoundary(
        [&]
        {
            ValidateCanCallCacheMethods();
            
            ThrowIfFailed(m_imageSourceFromWic->TrimCache(nullptr));
        });
}


IFACEMETHODIMP CanvasVirtualBitmap::TrimCacheWithRegion(Rect regionToKeep)
{
    return ExceptionBoundary(
        [&]
        {
            ValidateCanCallCacheMethods();
            
            auto d2dRect = CalculateBackTransformedRegion(regionToKeep);
            
            ThrowIfFailed(m_imageSourceFromWic->TrimCache(&d2dRect));
        });
}


D2D1_RECT_U CanvasVirtualBitmap::CalculateBackTransformedRegion(Rect r) const
{
    auto rect = ToD2DRectU(
        static_cast<int32_t>(r.X),
        static_cast<int32_t>(r.Y),
        static_cast<int32_t>(r.Width),
        static_cast<int32_t>(r.Height));

    auto a = BackTransform(rect.left,  rect.top);
    auto b = BackTransform(rect.right, rect.bottom);

    return { a.x, a.y, b.x, b.y };
}


using ::Windows::Foundation::Numerics::float2;

static float2 BackTransform(D2D1_ORIENTATION orientation, float2 p)
{   
    switch (orientation)
    {
    case D2D1_ORIENTATION_DEFAULT:                             return {  p.x,  p.y };
    case D2D1_ORIENTATION_ROTATE_CLOCKWISE270:                 return {  p.y, -p.x };
    case D2D1_ORIENTATION_ROTATE_CLOCKWISE180:                 return { -p.x, -p.y };
    case D2D1_ORIENTATION_ROTATE_CLOCKWISE90:                  return { -p.y,  p.x };
    case D2D1_ORIENTATION_FLIP_HORIZONTAL:                     return { -p.x,  p.y };
    case D2D1_ORIENTATION_ROTATE_CLOCKWISE180_FLIP_HORIZONTAL: return {  p.x, -p.y };
    case D2D1_ORIENTATION_ROTATE_CLOCKWISE90_FLIP_HORIZONTAL:  return { -p.y, -p.x };
    case D2D1_ORIENTATION_ROTATE_CLOCKWISE270_FLIP_HORIZONTAL: return {  p.y,  p.x };

    default:
        assert(false);
        return { p.x, p.y };
    }
}

D2D1_POINT_2U CanvasVirtualBitmap::BackTransform(uint32_t x, uint32_t y) const
{
    float2 center(m_localBounds.X + m_localBounds.Width / 2.0f, m_localBounds.Y + m_localBounds.Height / 2.0f);

    float2 p(static_cast<float>(x), static_cast<float>(y));

    p = ::BackTransform(m_orientation, p - center) + center;

    return { static_cast<uint32_t>(p.x), static_cast<uint32_t>(p.y) };
}


IFACEMETHODIMP CanvasVirtualBitmap::get_Device(ICanvasDevice** value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(value);
            ThrowIfFailed(m_device.CopyTo(value));
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


void CanvasVirtualBitmap::ValidateCanCallCacheMethods() const
{
    if (!m_cacheMethodsEnabled)
        ThrowHR(E_FAIL, Strings::CacheOnDemandNotSet);

    if (!m_imageSourceFromWic)
        ThrowHR(E_FAIL);
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


IFACEMETHODIMP CanvasVirtualBitmap::GetBounds(ICanvasDrawingSession* ds, Rect* bounds)
{
    return GetImageBoundsImpl(this, ds, nullptr, bounds);
}


IFACEMETHODIMP CanvasVirtualBitmap::GetBoundsWithTransform(ICanvasDrawingSession* ds, Matrix3x2 transform, Rect* bounds)
{
    return GetImageBoundsImpl(this, ds, &transform, bounds);
}


ComPtr<ID2D1Image> CanvasVirtualBitmap::GetD2DImage(ICanvasDevice* , ID2D1DeviceContext*, GetImageFlags, float, float* realizedDpi)
{
    if (realizedDpi)
        *realizedDpi = 96.0f;

    return GetResource();
}



#endif