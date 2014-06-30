// Copyright (c) Microsoft Corporation.  All rights reserved

#include "pch.h"

#include "CanvasDevice.h"
#include "CanvasImageSource.h"
#include "CanvasImageSourceDrawingSessionAdapter.h"

namespace canvas
{
    using namespace Microsoft::WRL::Wrappers;

    //
    // Drawing session factory
    //

    class CanvasImageSourceDrawingSessionFactory : public ICanvasImageSourceDrawingSessionFactory
    {
        std::shared_ptr<CanvasDrawingSessionManager> m_drawingSessionManager;

    public:
        CanvasImageSourceDrawingSessionFactory()
            : m_drawingSessionManager(CanvasDrawingSessionFactory::GetOrCreateManager())
        {
        }

        virtual ComPtr<ICanvasDrawingSession> Create(
            ISurfaceImageSourceNativeWithD2D* sisNative,
            const Rect& updateRect) const override
        {
            CheckInPointer(sisNative);

            ComPtr<ID2D1DeviceContext1> deviceContext;
            auto adapter = CanvasImageSourceDrawingSessionAdapter::Create(
                sisNative,
                ToRECT(updateRect),
                &deviceContext);

            return m_drawingSessionManager->Create(
                deviceContext.Get(),
                std::move(adapter));
        }        
    };


    //
    // CanvasImageSourceFactory implementation
    //

    CanvasImageSourceFactory::CanvasImageSourceFactory()
        : m_drawingSessionFactory(std::make_shared<CanvasImageSourceDrawingSessionFactory>())
    {
    }
    

    _Use_decl_annotations_
    IFACEMETHODIMP CanvasImageSourceFactory::Create(
        ICanvasDevice* device,
        int32_t widthInPixels,
        int32_t heightInPixels,
        ICanvasImageSource** imageSource)
    {
        return CreateWithBackground(
            device,
            widthInPixels,
            heightInPixels,
            CanvasBackground::Transparent,
            imageSource);
    }


    _Use_decl_annotations_
    IFACEMETHODIMP CanvasImageSourceFactory::CreateWithBackground(
        ICanvasDevice* device,
        int32_t widthInPixels,
        int32_t heightInPixels,
        CanvasBackground background,
        ICanvasImageSource** imageSource)
    {
        return ExceptionBoundary(
            [&]()
            {
                CheckInPointer(device);
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
                    device,
                    widthInPixels,
                    heightInPixels,
                    background,
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
        ICanvasDevice* device,
        int32_t widthInPixels,
        int32_t heightInPixels,
        CanvasBackground background,
        ISurfaceImageSourceFactory* surfaceImageSourceFactory,
        std::shared_ptr<ICanvasImageSourceDrawingSessionFactory> drawingSessionFactory)
        : m_drawingSessionFactory(drawingSessionFactory)
        , m_widthInPixels(widthInPixels)
        , m_heightInPixels(heightInPixels)
    {
        bool isOpaque = (background == CanvasBackground::Opaque);

        CreateBaseClass(surfaceImageSourceFactory, isOpaque);
        SetDevice(device);
    }


    void CanvasImageSource::CreateBaseClass(
        ISurfaceImageSourceFactory* surfaceImageSourceFactory,
        bool isOpaque)
    {
        ComPtr<ISurfaceImageSource> base;
        ComPtr<IInspectable> baseInspectable;

        ThrowIfFailed(surfaceImageSourceFactory->CreateInstanceWithDimensionsAndOpacity(
            m_widthInPixels,
            m_heightInPixels,
            isOpaque,
            this,
            &baseInspectable,
            &base));

        ThrowIfFailed(SetComposableBasePointers(
            baseInspectable.Get(),
            surfaceImageSourceFactory));
    }


    void CanvasImageSource::SetDevice(ICanvasDevice* device)
    {
        CheckInPointer(device);

        //
        // Get the D2D device and pass this to the underlying surface image
        // source.
        //
        ComPtr<ICanvasDeviceInternal> deviceInternal;
        ThrowIfFailed(device->QueryInterface(deviceInternal.GetAddressOf()));
        ComPtr<ID2D1Device1> d2dDevice = deviceInternal->GetD2DDevice();

        ComPtr<ISurfaceImageSourceNativeWithD2D> sisNative;
        ThrowIfFailed(GetComposableBase().As(&sisNative));

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
        ICanvasDrawingSession** drawingSession)
    {
        Rect updateRectangle = {};
        updateRectangle.Width = static_cast<float>(m_widthInPixels);
        updateRectangle.Height = static_cast<float>(m_heightInPixels);

        return CreateDrawingSessionWithUpdateRectangle(
            updateRectangle,
            drawingSession);
    }

    
    _Use_decl_annotations_
    IFACEMETHODIMP CanvasImageSource::CreateDrawingSessionWithUpdateRectangle(
        Rect updateRectangle,
        ICanvasDrawingSession** drawingSession)
    {
        return ExceptionBoundary(
            [&]()
            {
                CheckAndClearOutPointer(drawingSession);
                                
                ComPtr<ISurfaceImageSourceNativeWithD2D> sisNative;
                ThrowIfFailed(GetComposableBase().As(&sisNative));

                auto newDrawingSession = m_drawingSessionFactory->Create(
                    sisNative.Get(),
                    updateRectangle);

                ThrowIfFailed(newDrawingSession.CopyTo(drawingSession));
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
    IFACEMETHODIMP CanvasImageSource::put_Device(
        ICanvasDevice* value) 
    {
        return ExceptionBoundary(
            [&]()
            {
                SetDevice(value);
            });
    }


    //
    // Register the class
    //

    ActivatableClassWithFactory(CanvasImageSource, CanvasImageSourceFactory);
}
