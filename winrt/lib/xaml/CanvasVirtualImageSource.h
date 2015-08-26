// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace UI { namespace Xaml
{
    class CanvasVirtualImageSourceFactory
        : public ActivationFactory<ICanvasVirtualImageSourceFactory>,
          private LifespanTracker<CanvasVirtualImageSourceFactory>
    {
        InspectableClassStatic(RuntimeClass_Microsoft_Graphics_Canvas_UI_Xaml_CanvasVirtualImageSource, BaseTrust);

        ComPtr<IVirtualSurfaceImageSourceFactory> m_virtualSurfaceImageSourceFactory;
        std::shared_ptr<ICanvasImageSourceDrawingSessionFactory> m_drawingSessionFactory;

    public:
        CanvasVirtualImageSourceFactory();

        // This constructor used by tests to inject mock factories
        CanvasVirtualImageSourceFactory(
            IVirtualSurfaceImageSourceFactory* vsisFactory,
            std::shared_ptr<ICanvasImageSourceDrawingSessionFactory> drawingSessionFactory);
        

        IFACEMETHOD(CreateWithSize)(
            ICanvasResourceCreatorWithDpi* resourceCreator,
            Size size,
            ICanvasVirtualImageSource** virtualImageSource) override;
        
        IFACEMETHOD(CreateWithWidthAndHeight)(
            ICanvasResourceCreatorWithDpi* resourceCreator,
            float width,
            float height,
            ICanvasVirtualImageSource** virtualImageSource) override;
        
        IFACEMETHOD(CreateWithWidthAndHeightAndDpi)(
            ICanvasResourceCreator* resourceCreator,
            float width,
            float height,
            float dpi,
            ICanvasVirtualImageSource** virtualImageSource) override;
        
        IFACEMETHOD(CreateWithWidthAndHeightAndDpiAndAlphaMode)(
            ICanvasResourceCreator* resourceCreator,
            float width,
            float height,
            float dpi,
            CanvasAlphaMode alphaMode,
            ICanvasVirtualImageSource** virtualImageSource) override;

    private:
        void Create(
            ICanvasResourceCreator* resourceCreator,
            float width,
            float height,
            float dpi,
            CanvasAlphaMode alphaMode,
            ICanvasVirtualImageSource** virtualImageSource);
    };


    typedef ITypedEventHandler<CanvasVirtualImageSource*, CanvasRegionsInvalidatedEventArgs*> ImageSourceRegionsInvalidatedHandler;

    class CanvasVirtualImageSource
        : public RuntimeClass<
            RuntimeClassFlags<WinRtClassicComMix>,
            ICanvasVirtualImageSource,
            IVirtualSurfaceUpdatesCallbackNative>,
          private LifespanTracker<CanvasVirtualImageSource>
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_UI_Xaml_CanvasVirtualImageSource, BaseTrust);

        std::shared_ptr<ICanvasImageSourceDrawingSessionFactory> m_drawingSessionFactory;
        ComPtr<ICanvasDevice> m_device;
        ComPtr<IVirtualSurfaceImageSource> m_vsis;
        float m_dpi;
        Size m_size;
        CanvasAlphaMode m_alphaMode;
        bool m_registeredForUpdates;
        EventSource<ImageSourceRegionsInvalidatedHandler, InvokeModeOptions<StopOnFirstError>> m_regionsInvalidatedEventSource;

    public:
        CanvasVirtualImageSource(
            std::shared_ptr<ICanvasImageSourceDrawingSessionFactory> drawingSessionFactory,
            ICanvasResourceCreator* resourceCreator,
            IVirtualSurfaceImageSource* vsis,
            float dpi,
            Size size,
            CanvasAlphaMode alphaMode);

        //
        // ICanvasVirtualImageSource
        //
        
        IFACEMETHOD(get_Source)(
            IVirtualSurfaceImageSource** value) override;

        IFACEMETHOD(CreateDrawingSession)(
            Color clearColor,
            Rect updateRectangle,
            ICanvasDrawingSession** drawingSession) override;

        IFACEMETHOD(SuspendDrawingSession)(
            ICanvasDrawingSession* drawingSession) override;

        IFACEMETHOD(ResumeDrawingSession)(
            ICanvasDrawingSession* drawingSession) override;

        IFACEMETHOD(Invalidate)() override;

        IFACEMETHOD(InvalidateRegion)(
            Rect region) override;

        IFACEMETHOD(RaiseRegionsInvalidatedIfAny)() override;

        IFACEMETHOD(add_RegionsInvalidated)(
            ImageSourceRegionsInvalidatedHandler* value,
            EventRegistrationToken* token) override;

        IFACEMETHOD(remove_RegionsInvalidated)(
            EventRegistrationToken token) override;

        IFACEMETHOD(ResizeWithSize)(
            Size size) override;

        IFACEMETHOD(ResizeWithWidthAndHeight)(
            float width,
            float height) override;

        IFACEMETHOD(ResizeWithWidthAndHeightAndDpi)(
            float width,
            float height,
            float dpi) override;

        IFACEMETHOD(get_Device)(
            ICanvasDevice** value) override;

        IFACEMETHOD(Recreate)(
            ICanvasResourceCreator* resourceCreator) override;

        IFACEMETHOD(get_Dpi)(
            float* dpi) override;

        IFACEMETHOD(ConvertPixelsToDips)(
            INT32 pixels,
            float* dips) override;

        IFACEMETHOD(ConvertDipsToPixels)(
            float dips,
            CanvasDpiRounding dpiRounding,
            INT32* pixels) override;

        IFACEMETHOD(get_Size)(
            Size* value) override;

        IFACEMETHOD(get_SizeInPixels)(
            BitmapSize* value) override;

        IFACEMETHOD(get_AlphaMode)(
            CanvasAlphaMode* value) override;

        //
        // IVirtualSurfaceUpdatesCallbackNative
        //
        
        IFACEMETHOD(UpdatesNeeded)() override;
        
    private:
        void SetDevice(ICanvasDevice* device);
    };


    class CanvasRegionsInvalidatedEventArgs
        : public RuntimeClass<ICanvasRegionsInvalidatedEventArgs>
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_UI_Xaml_CanvasRegionsInvalidatedEventArgs, BaseTrust);

        std::vector<Rect> m_invalidatedRegions;
        Rect m_visibleRegion;

    public:
        CanvasRegionsInvalidatedEventArgs(std::vector<Rect>&& invalidatedRegions, Rect const& visibleRegion);
        
        IFACEMETHOD(get_InvalidatedRegions)(uint32_t*, Rect**) override;
        IFACEMETHOD(get_VisibleRegion)(Rect*) override;
    };
    
} } } } } }
