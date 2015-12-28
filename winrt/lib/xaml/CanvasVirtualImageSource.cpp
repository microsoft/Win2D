// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

#include "CanvasVirtualImageSource.h"

using namespace ABI::Microsoft::Graphics::Canvas;
using namespace ABI::Microsoft::Graphics::Canvas::UI::Xaml;

ActivatableClassWithFactory(CanvasVirtualImageSource, CanvasVirtualImageSourceFactory);


//
// CanvasVirtualImageSourceFactory implementation
//


static ComPtr<IVirtualSurfaceImageSourceFactory> GetVirtualSurfaceImageSourceFactory()
{
    ComPtr<IVirtualSurfaceImageSourceFactory> f;
    ThrowIfFailed(GetActivationFactory(
        HStringReference(RuntimeClass_Windows_UI_Xaml_Media_Imaging_VirtualSurfaceImageSource).Get(),
        &f));

    return f;
}


CanvasVirtualImageSourceFactory::CanvasVirtualImageSourceFactory()
    : CanvasVirtualImageSourceFactory(
        GetVirtualSurfaceImageSourceFactory().Get(),
        std::make_shared<CanvasImageSourceDrawingSessionFactory>())
{
}


CanvasVirtualImageSourceFactory::CanvasVirtualImageSourceFactory(
    IVirtualSurfaceImageSourceFactory* vsisFactory,
    std::shared_ptr<ICanvasImageSourceDrawingSessionFactory> drawingSessionFactory)
    : m_virtualSurfaceImageSourceFactory(vsisFactory)
    , m_drawingSessionFactory(drawingSessionFactory)
{
}


IFACEMETHODIMP CanvasVirtualImageSourceFactory::CreateWithSize(
    ICanvasResourceCreatorWithDpi* resourceCreator,
    Size size,
    ICanvasVirtualImageSource** virtualImageSource)
{
    return CreateWithWidthAndHeight(resourceCreator, size.Width, size.Height, virtualImageSource);
}


IFACEMETHODIMP CanvasVirtualImageSourceFactory::CreateWithWidthAndHeight(
    ICanvasResourceCreatorWithDpi* resourceCreator,
    float width,
    float height,
    ICanvasVirtualImageSource** virtualImageSource)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(resourceCreator);
            CheckAndClearOutPointer(virtualImageSource);

            float dpi;
            ThrowIfFailed(resourceCreator->get_Dpi(&dpi));

            Create(
                As<ICanvasResourceCreator>(resourceCreator).Get(),
                width,
                height,
                dpi,
                CanvasAlphaMode::Premultiplied,
                virtualImageSource);
        });
}


IFACEMETHODIMP CanvasVirtualImageSourceFactory::CreateWithWidthAndHeightAndDpi(
    ICanvasResourceCreator* resourceCreator,
    float width,
    float height,
    float dpi,
    ICanvasVirtualImageSource** virtualImageSource)
{
    return CreateWithWidthAndHeightAndDpiAndAlphaMode(
        resourceCreator,
        width,
        height,
        dpi,
        CanvasAlphaMode::Premultiplied,
        virtualImageSource);
}


IFACEMETHODIMP CanvasVirtualImageSourceFactory::CreateWithWidthAndHeightAndDpiAndAlphaMode(
    ICanvasResourceCreator* resourceCreator,
    float width,
    float height,
    float dpi,
    CanvasAlphaMode alphaMode,
    ICanvasVirtualImageSource** virtualImageSource)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(resourceCreator);
            CheckAndClearOutPointer(virtualImageSource);

            Create(
                resourceCreator,
                width,
                height,
                dpi,
                alphaMode,
                virtualImageSource);
        });
}


void CanvasVirtualImageSourceFactory::Create(
    ICanvasResourceCreator* resourceCreator,
    float width,
    float height,
    float dpi,
    CanvasAlphaMode alphaMode,
    ICanvasVirtualImageSource** virtualImageSource)
{
    int pixelWidth = SizeDipsToPixels(width, dpi);
    int pixelHeight = SizeDipsToPixels(height, dpi);

    bool isOpaque;
    switch (alphaMode)
    {
    case CanvasAlphaMode::Ignore:
        isOpaque = true;
        break;

    case CanvasAlphaMode::Premultiplied:
        isOpaque = false;
        break;

    default:
        ThrowHR(E_INVALIDARG, Strings::InvalidAlphaModeForImageSource);
    }

    ComPtr<IVirtualSurfaceImageSource> vsis;
    ThrowIfFailed(m_virtualSurfaceImageSourceFactory->CreateInstanceWithDimensionsAndOpacity(
        pixelWidth,
        pixelHeight,
        isOpaque,
        &vsis));

    auto imageSource = Make<CanvasVirtualImageSource>(
        m_drawingSessionFactory,
        resourceCreator,
        vsis.Get(),
        dpi,
        Size{ width, height },
        alphaMode);
    CheckMakeResult(imageSource);
    ThrowIfFailed(imageSource.CopyTo(virtualImageSource));
}


//
// CanvasVirtualImageSource implementation
//


CanvasVirtualImageSource::CanvasVirtualImageSource(
    std::shared_ptr<ICanvasImageSourceDrawingSessionFactory> drawingSessionFactory,
    ICanvasResourceCreator* resourceCreator,
    IVirtualSurfaceImageSource* vsis,
    float dpi,
    Size size,
    CanvasAlphaMode alphaMode)
    : m_drawingSessionFactory(drawingSessionFactory)
    , m_hasActiveDrawingSession(std::make_shared<bool>())
    , m_vsis(vsis)
    , m_dpi(dpi)
    , m_size(size)
    , m_alphaMode(alphaMode)
    , m_registeredForUpdates(false)
{
    SetDevice(GetCanvasDevice(resourceCreator).Get());
}


void CanvasVirtualImageSource::SetDevice(ICanvasDevice* device)
{
    ComPtr<ID2D1Device> d2dDevice;

    if (device)
        d2dDevice = As<ICanvasDeviceInternal>(device)->GetD2DDevice();

    auto vsisNative = As<ISurfaceImageSourceNativeWithD2D>(m_vsis);

    ThrowIfFailed(vsisNative->SetDevice(d2dDevice.Get()));

    m_device = device;
}


IFACEMETHODIMP CanvasVirtualImageSource::get_Source(
    IVirtualSurfaceImageSource** value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckAndClearOutPointer(value);
            ThrowIfFailed(m_vsis.CopyTo(value));
        });
}


IFACEMETHODIMP CanvasVirtualImageSource::CreateDrawingSession(
    Color clearColor,
    Rect updateRectangle,
    ICanvasDrawingSession** drawingSession)
{
    return ExceptionBoundary(
        [&]
        {
            CheckAndClearOutPointer(drawingSession);

            auto ds = CreateDrawingSession(clearColor, updateRectangle);

            ThrowIfFailed(ds.CopyTo(drawingSession));
        });
}


ComPtr<ICanvasDrawingSession> CanvasVirtualImageSource::CreateDrawingSession(Color clearColor, Rect updateRectangle)
{
    auto sisNative = As<ISurfaceImageSourceNativeWithD2D>(m_vsis);

    try
    {
        // First attempt.
        return m_drawingSessionFactory->Create(m_device.Get(), sisNative.Get(), m_hasActiveDrawingSession, clearColor, updateRectangle, m_dpi);
    }
    catch (HResultException const& e)
    {
        if (e.GetHr() == E_SURFACE_CONTENTS_LOST)
        {
            // Handle surface lost by recreating the surface, then retry creating the drawing session.
            //
            // The app may be in the middle of looping over a set of rectangles previously reported by
            // the RegionsInvalidatedEvent, but we don't bother trying to bubble up an exception that
            // would break out of that loop because we don't want to complicate app code due to this
            // hopefully rare error case. Keeping the recovery hidden inside Win2D may leave the app
            // with a stale or partial list of dirty rects, but this is ok - if any additional rects
            // now also need redrawing, XAML will report those momentarily via a separate event.

            ThrowIfFailed(Recreate(As<ICanvasResourceCreator>(m_device).Get()));

            return m_drawingSessionFactory->Create(m_device.Get(), sisNative.Get(), m_hasActiveDrawingSession, clearColor, updateRectangle, m_dpi);
        }
        else
        {
            // Rethrow any other errors.
            throw;
        }
    }
}


IFACEMETHODIMP CanvasVirtualImageSource::SuspendDrawingSession(
    ICanvasDrawingSession* drawingSession)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(drawingSession);
            auto sisNative = As<ISurfaceImageSourceNativeWithD2D>(m_vsis);

            ThrowIfFailed(sisNative->SuspendDraw());
        });
}


IFACEMETHODIMP CanvasVirtualImageSource::ResumeDrawingSession(
    ICanvasDrawingSession* drawingSession)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(drawingSession);
            auto sisNative = As<ISurfaceImageSourceNativeWithD2D>(m_vsis);

            ThrowIfFailed(sisNative->ResumeDraw());
        });
}


IFACEMETHODIMP CanvasVirtualImageSource::Invalidate()
{
    return ExceptionBoundary(
        [&]
        {
            RECT updateRect = ToRECT(Rect{ 0, 0, m_size.Width, m_size.Height }, m_dpi);

            auto sisNative = As<IVirtualSurfaceImageSourceNative>(m_vsis);
            ThrowIfFailed(sisNative->Invalidate(updateRect));
        });
}


IFACEMETHODIMP CanvasVirtualImageSource::InvalidateRegion(
    Rect region)
{
    return ExceptionBoundary(
        [&]
        {
            RECT updateRectangle = ToRECT(region, m_dpi);

            auto sisNative = As<IVirtualSurfaceImageSourceNative>(m_vsis);
            ThrowIfFailed(sisNative->Invalidate(updateRectangle));            
        });
}


IFACEMETHODIMP CanvasVirtualImageSource::RaiseRegionsInvalidatedIfAny()
{
    return ExceptionBoundary(
        [&]
        {
            // This must be called on the UI thread
            ComPtr<ICoreDispatcher> dispatcher;
            ThrowIfFailed(As<IDependencyObject>(m_vsis)->get_Dispatcher(&dispatcher));

            boolean hasThreadAccess;
            ThrowIfFailed(dispatcher->get_HasThreadAccess(&hasThreadAccess));

            if (!hasThreadAccess)
                ThrowHR(RPC_E_WRONG_THREAD);

            // The UpdatesNeeded handler will raise RegionsInvalidated for us.
            ThrowIfFailed(UpdatesNeeded());
        });
}


IFACEMETHODIMP CanvasVirtualImageSource::add_RegionsInvalidated(
    ImageSourceRegionsInvalidatedHandler* value,
    EventRegistrationToken* token)
{
    return ExceptionBoundary(
        [&]
        {
            if (!m_registeredForUpdates)
            {
                auto sisNative = As<IVirtualSurfaceImageSourceNative>(m_vsis);
                ThrowIfFailed(sisNative->RegisterForUpdatesNeeded(this));
                m_registeredForUpdates = true;
            }

            ThrowIfFailed(m_regionsInvalidatedEventSource.Add(value, token));
        });
}


IFACEMETHODIMP CanvasVirtualImageSource::remove_RegionsInvalidated(
    EventRegistrationToken token)
{
    return ExceptionBoundary(
        [&]
        {
            ThrowIfFailed(m_regionsInvalidatedEventSource.Remove(token));
        });
}


IFACEMETHODIMP CanvasVirtualImageSource::ResizeWithSize(
    Size size)
{
    return ResizeWithWidthAndHeight(size.Width, size.Height);
}


IFACEMETHODIMP CanvasVirtualImageSource::ResizeWithWidthAndHeight(
    float width,
    float height)
{
    return ResizeWithWidthAndHeightAndDpi(width, height, m_dpi);
}


IFACEMETHODIMP CanvasVirtualImageSource::ResizeWithWidthAndHeightAndDpi(
    float width,
    float height,
    float dpi)
{
    return ExceptionBoundary(
        [&]
        {
            auto sisNative = As<IVirtualSurfaceImageSourceNative>(m_vsis);

            auto widthInPixels = SizeDipsToPixels(width, dpi);
            auto heightInPixels = SizeDipsToPixels(height, dpi);
            
            ThrowIfFailed(sisNative->Resize(widthInPixels, heightInPixels));

            m_dpi = dpi;
            m_size = Size{ width, height };
        });
}


IFACEMETHODIMP CanvasVirtualImageSource::get_Device(
    ICanvasDevice** value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckAndClearOutPointer(value);

            ThrowIfFailed(m_device.CopyTo(value));
        });
}


IFACEMETHODIMP CanvasVirtualImageSource::Recreate(
    ICanvasResourceCreator* resourceCreator)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(resourceCreator);

            auto device = GetCanvasDevice(resourceCreator);

            //
            // Force the underlying XAML virtual image source to recreate itself
            // by first setting no device and then setting the new device.  This
            // avoids tripping up on any early outs (eg if the new device is
            // actually the same as the old device).
            //
            
            SetDevice(nullptr);
            SetDevice(device.Get());
        });
}


IFACEMETHODIMP CanvasVirtualImageSource::get_Dpi(
    float* dpi)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(dpi);

            *dpi = m_dpi;
        });
}


IFACEMETHODIMP CanvasVirtualImageSource::ConvertPixelsToDips(
    int pixels,
    float* dips)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(dips);
            *dips = PixelsToDips(pixels, m_dpi);
        });
}


IFACEMETHODIMP CanvasVirtualImageSource::ConvertDipsToPixels(
    float dips,
    CanvasDpiRounding dpiRounding,
    int* pixels)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(pixels);
            *pixels = DipsToPixels(dips, m_dpi, dpiRounding);
        });
}


IFACEMETHODIMP CanvasVirtualImageSource::get_Size(
    Size* value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(value);
            *value = m_size;
        });
}


IFACEMETHODIMP CanvasVirtualImageSource::get_SizeInPixels(
    BitmapSize* value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(value);

            auto width = SizeDipsToPixels(m_size.Width, m_dpi);
            auto height = SizeDipsToPixels(m_size.Height, m_dpi);
            
            *value = BitmapSize{ static_cast<uint32_t>(width), static_cast<uint32_t>(height) };
        });
}


IFACEMETHODIMP CanvasVirtualImageSource::get_AlphaMode(
    CanvasAlphaMode* value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(value);

            *value = m_alphaMode;
        });
}


IFACEMETHODIMP CanvasVirtualImageSource::UpdatesNeeded()
{
    return ExceptionBoundary(
        [&]
        {
            auto vsisNative = As<IVirtualSurfaceImageSourceNative>(m_vsis);

            DWORD updateRectCount;
            ThrowIfFailed(vsisNative->GetUpdateRectCount(&updateRectCount));

            if (updateRectCount == 0)
            {
                // When there are no regions to update we don't raise
                // RegionsInvalidated.
                return;
            }

            std::vector<RECT> updateRECTs(updateRectCount);
            ThrowIfFailed(vsisNative->GetUpdateRects(updateRECTs.data(), updateRectCount));

            std::vector<Rect> updateRects;
            updateRects.reserve(updateRECTs.size());
            
            std::transform(updateRECTs.begin(), updateRECTs.end(), std::back_inserter(updateRects),
                [=] (RECT const& r) { return ToRect(r, m_dpi); });

            RECT visibleBounds;
            ThrowIfFailed(vsisNative->GetVisibleBounds(&visibleBounds));
            
            auto args = Make<CanvasRegionsInvalidatedEventArgs>(std::move(updateRects), ToRect(visibleBounds, m_dpi));
            CheckMakeResult(args);
            
            ThrowIfFailed(m_regionsInvalidatedEventSource.InvokeAll(this, args.Get()));
        });
}


//
// CanvasRegionsInvalidatedEventArgs
//

CanvasRegionsInvalidatedEventArgs::CanvasRegionsInvalidatedEventArgs(
    std::vector<Rect>&& invalidatedRegions,
    Rect const& visibleRegion)
    : m_invalidatedRegions(std::move(invalidatedRegions))
    , m_visibleRegion(visibleRegion)
{
}


IFACEMETHODIMP CanvasRegionsInvalidatedEventArgs::get_InvalidatedRegions(uint32_t* count, Rect** value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(count);
            CheckAndClearOutPointer(value);

            ComArray<Rect> array(m_invalidatedRegions.begin(), m_invalidatedRegions.end());
            array.Detach(count, value);
        });
}


IFACEMETHODIMP CanvasRegionsInvalidatedEventArgs::get_VisibleRegion(Rect* value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(value);
            *value = m_visibleRegion;
        });
}


