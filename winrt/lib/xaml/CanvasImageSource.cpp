// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

#include "CanvasImageSourceDrawingSessionAdapter.h"

using namespace ABI::Microsoft::Graphics::Canvas;
using namespace ABI::Microsoft::Graphics::Canvas::UI::Xaml;

ComPtr<ICanvasDrawingSession> CanvasImageSourceDrawingSessionFactory::Create(
    ICanvasDevice* owner,
    ISurfaceImageSourceNativeWithD2D* sisNative,
    Color const& clearColor,
    Rect const& updateRectangleInDips,
    float dpi) const
{
    CheckInPointer(sisNative);

    RECT updateRectangle = ToRECT(updateRectangleInDips, dpi);

    ComPtr<ID2D1DeviceContext1> deviceContext;
    auto adapter = CanvasImageSourceDrawingSessionAdapter::Create(
        sisNative,
        ToD2DColor(clearColor),
        updateRectangle,
        dpi,
        &deviceContext);

    return CanvasDrawingSession::CreateNew(
        deviceContext.Get(),
        std::move(adapter),
        owner);
}

//
// CanvasImageSourceFactory implementation
//

CanvasImageSourceFactory::CanvasImageSourceFactory()
    : m_drawingSessionFactory(std::make_shared<CanvasImageSourceDrawingSessionFactory>())
{
}


_Use_decl_annotations_
IFACEMETHODIMP CanvasImageSourceFactory::CreateWithSize(
    ICanvasResourceCreatorWithDpi* resourceCreator,
    Size size,
    ICanvasImageSource** imageSource)
{
    return CreateWithWidthAndHeight(
        resourceCreator, 
        size.Width, 
        size.Height, 
        imageSource);
}


_Use_decl_annotations_
IFACEMETHODIMP CanvasImageSourceFactory::CreateWithWidthAndHeight(
    ICanvasResourceCreatorWithDpi* resourceCreator,
    float width,
    float height,
    ICanvasImageSource** imageSource)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(resourceCreator);

            float dpi;
            ThrowIfFailed(resourceCreator->get_Dpi(&dpi));

            ThrowIfFailed(CreateWithWidthAndHeightAndDpiAndAlphaMode(
                As<ICanvasResourceCreator>(resourceCreator).Get(),
                width,
                height,
                dpi,
                CanvasAlphaMode::Premultiplied,
                imageSource));
        });
}


_Use_decl_annotations_
IFACEMETHODIMP CanvasImageSourceFactory::CreateWithWidthAndHeightAndDpi(
    ICanvasResourceCreator* resourceCreator,
    float width,
    float height,
    float dpi,
    ICanvasImageSource** imageSource)
{
    return CreateWithWidthAndHeightAndDpiAndAlphaMode(
        resourceCreator,
        width,
        height,
        dpi,
        CanvasAlphaMode::Premultiplied,
        imageSource);
}


_Use_decl_annotations_
IFACEMETHODIMP CanvasImageSourceFactory::CreateWithWidthAndHeightAndDpiAndAlphaMode(
    ICanvasResourceCreator* resourceCreator,
    float width,
    float height,
    float dpi,
    CanvasAlphaMode alphaMode,
    ICanvasImageSource** imageSource)
{
    return ExceptionBoundary(
        [&]
        {
            using ::Microsoft::WRL::Wrappers::HStringReference;

            CheckInPointer(resourceCreator);
            CheckAndClearOutPointer(imageSource);

            //
            // Get the factory for CanvasImageSource's base class.  We pass
            // this in to the CanvasImageSource constructor in order to
            // allow test code to inject its own ISurfaceImageSourceFactory
            // implementation.
            //
            ComPtr<ISurfaceImageSourceFactory> baseFactory;
            ThrowIfFailed(GetActivationFactory(
                HStringReference(RuntimeClass_Windows_UI_Xaml_Media_Imaging_SurfaceImageSource).Get(),
                &baseFactory));

            //
            // Now create the object
            //
            auto newCanvasImageSource = Make<CanvasImageSource>(
                resourceCreator,
                width,
                height,
                dpi,
                alphaMode,
                baseFactory.Get(),
                m_drawingSessionFactory);

            CheckMakeResult(newCanvasImageSource);

            ThrowIfFailed(newCanvasImageSource.CopyTo(imageSource));
        });
}

//
// CanvasImageSource implementation
//
    
_Use_decl_annotations_
CanvasImageSource::CanvasImageSource(
    ICanvasResourceCreator* resourceCreator,
    float width,
    float height,
    float dpi,
    CanvasAlphaMode alphaMode,
    ISurfaceImageSourceFactory* surfaceImageSourceFactory,
    std::shared_ptr<ICanvasImageSourceDrawingSessionFactory> drawingSessionFactory)
    : m_drawingSessionFactory(drawingSessionFactory)
    , m_width(width)
    , m_height(height)
    , m_dpi(dpi)
    , m_alphaMode(alphaMode)
{
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

    CreateBaseClass(surfaceImageSourceFactory, isOpaque);

    ComPtr<ICanvasDevice> device;
    ThrowIfFailed(resourceCreator->get_Device(&device));
    SetDevice(device.Get());
}


void CanvasImageSource::CreateBaseClass(
    ISurfaceImageSourceFactory* surfaceImageSourceFactory,
    bool isOpaque)
{
    ComPtr<ISurfaceImageSource> base;
    ComPtr<IInspectable> baseInspectable;

    ThrowIfFailed(surfaceImageSourceFactory->CreateInstanceWithDimensionsAndOpacity(
        SizeDipsToPixels(m_width, m_dpi),
        SizeDipsToPixels(m_height, m_dpi),
        isOpaque,
        this,
        &baseInspectable,
        &base));

    ThrowIfFailed(SetComposableBasePointers(
        baseInspectable.Get(),
        nullptr));
}


void CanvasImageSource::SetDevice(ICanvasDevice* device)
{
    //
    // Get the D2D device and pass this to the underlying surface image
    // source.
    //
    ComPtr<ID2D1Device> d2dDevice;

    if (device)
        d2dDevice = As<ICanvasDeviceInternal>(device)->GetD2DDevice();

    auto sisNative = As<ISurfaceImageSourceNativeWithD2D>(GetComposableBase());

    //
    // Set the device.  SiS does some validation here - for example, if the
    // width/height are 0 then this will fail with E_INVALIDARG.  We don't
    // add additional validation around this since CanvasImageSource derives
    // from SurfaceImageSource and we want to be consistent with the
    // existing XAML behavior.
    //
    ThrowIfFailed(sisNative->SetDevice(d2dDevice.Get()));

    //
    // Remember the canvas device we're now using.  We do this after we're
    // certain that all the previous steps succeeded (so we don't end up
    // with m_device referencing a device that we failed to set).
    //
    m_device = device;
}
    

_Use_decl_annotations_
IFACEMETHODIMP CanvasImageSource::CreateDrawingSession(
    Color clearColor,
    ICanvasDrawingSession** drawingSession)
{
    Rect updateRectangle{ 0, 0, m_width, m_height };

    return CreateDrawingSessionWithUpdateRectangle(
        clearColor,
        updateRectangle,
        drawingSession);
}

    
_Use_decl_annotations_
IFACEMETHODIMP CanvasImageSource::CreateDrawingSessionWithUpdateRectangle(
    Color clearColor,
    Rect updateRectangle,
    ICanvasDrawingSession** drawingSession)
{
    return ExceptionBoundary(
        [&]()
        {
            ComPtr<ISurfaceImageSourceNativeWithD2D> sisNative;
            ThrowIfFailed(GetComposableBase().As(&sisNative));

            auto ds = m_drawingSessionFactory->Create(
                m_device.Get(),
                sisNative.Get(),
                clearColor,
                updateRectangle,
                m_dpi);
            
            ThrowIfFailed(ds.CopyTo(drawingSession));
        });
}


_Use_decl_annotations_
IFACEMETHODIMP CanvasImageSource::get_Device(
    ICanvasDevice** value) 
{
    if (!value)
        return E_INVALIDARG;

    return m_device.CopyTo(value);
}
    

_Use_decl_annotations_
IFACEMETHODIMP CanvasImageSource::Recreate(
    ICanvasResourceCreator* value) 
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(value);

            ComPtr<ICanvasDevice> device;
            ThrowIfFailed(value->get_Device(&device));

            SetDevice(nullptr);
            SetDevice(device.Get());
        });
}


_Use_decl_annotations_
IFACEMETHODIMP CanvasImageSource::get_Dpi(
    float* dpi)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(dpi);
            *dpi = m_dpi;
        });
}


_Use_decl_annotations_
IFACEMETHODIMP CanvasImageSource::ConvertPixelsToDips(
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


_Use_decl_annotations_
IFACEMETHODIMP CanvasImageSource::ConvertDipsToPixels(
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


_Use_decl_annotations_
IFACEMETHODIMP CanvasImageSource::get_SizeInPixels(
    BitmapSize* size)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(size);
            size->Width = SizeDipsToPixels(m_width, m_dpi);
            size->Height = SizeDipsToPixels(m_height, m_dpi);
        });
}


_Use_decl_annotations_
IFACEMETHODIMP CanvasImageSource::get_Size(
    ABI::Windows::Foundation::Size* size)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(size);
            size->Width = m_width;
            size->Height = m_height;
        });
}


_Use_decl_annotations_
IFACEMETHODIMP CanvasImageSource::get_AlphaMode(
    CanvasAlphaMode* value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(value);
            *value = m_alphaMode;
        });
}


//
// Register the class
//

ActivatableClassWithFactory(CanvasImageSource, CanvasImageSourceFactory);

