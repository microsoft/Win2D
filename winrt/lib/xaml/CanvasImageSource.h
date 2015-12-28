// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace UI { namespace Xaml
{
    using namespace ABI::Windows::UI;
    using namespace ABI::Windows::Foundation;
    using namespace ABI::Windows::UI::Xaml::Media::Imaging;
    using namespace ::Microsoft::WRL;

    class ICanvasImageSourceDrawingSessionFactory
    {
    public:
        virtual ~ICanvasImageSourceDrawingSessionFactory() = default;

        virtual ComPtr<ICanvasDrawingSession> Create(
            ICanvasDevice* owner,
            ISurfaceImageSourceNativeWithD2D* sisNative,
            std::shared_ptr<bool> hasActiveDrawingSession,
            Color const& clearColor,
            Rect const& updateRectangleInDips,
            float dpi) const = 0;
    };


    class CanvasImageSourceFactory : public ActivationFactory<ICanvasImageSourceFactory>,
                                     private LifespanTracker<CanvasImageSourceFactory>
    {
        InspectableClassStatic(RuntimeClass_Microsoft_Graphics_Canvas_UI_Xaml_CanvasImageSource, BaseTrust);

        std::shared_ptr<ICanvasImageSourceDrawingSessionFactory> m_drawingSessionFactory;

    public:
        CanvasImageSourceFactory();

        IFACEMETHOD(CreateWithSize)(
            ICanvasResourceCreatorWithDpi* resourceCreator,
            Size size,
            ICanvasImageSource** imageSource) override;

        IFACEMETHOD(CreateWithWidthAndHeight)(
            ICanvasResourceCreatorWithDpi* resourceCreator,
            float width,
            float height,
            ICanvasImageSource** imageSource) override;

        IFACEMETHOD(CreateWithWidthAndHeightAndDpi)(
            ICanvasResourceCreator* resourceCreator,
            float width,
            float height,
            float dpi,
            ICanvasImageSource** imageSource) override;

        IFACEMETHOD(CreateWithWidthAndHeightAndDpiAndAlphaMode)(
            ICanvasResourceCreator* resourceCreator,
            float width,
            float height,
            float dpi,
            CanvasAlphaMode alphaMode,
            ICanvasImageSource** imageSource) override;
    };


    class CanvasImageSource
        : public RuntimeClass<
            ICanvasImageSource,
            ICanvasResourceCreator,
            ICanvasResourceCreatorWithDpi,
            ComposableBase<>>,
          private LifespanTracker<CanvasImageSource>
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_UI_Xaml_CanvasImageSource, BaseTrust);

        ComPtr<ICanvasDevice> m_device;
        std::shared_ptr<ICanvasImageSourceDrawingSessionFactory> m_drawingSessionFactory;
        std::shared_ptr<bool> m_hasActiveDrawingSession;
        const float m_width;
        const float m_height;
        const float m_dpi;
        const CanvasAlphaMode m_alphaMode;

    public:
        CanvasImageSource(
            ICanvasResourceCreator* resourceCreator,
            float width,
            float height,
            float dpi,
            CanvasAlphaMode alphaMode,
            ISurfaceImageSourceFactory* surfaceImageSourceFactory,
            std::shared_ptr<ICanvasImageSourceDrawingSessionFactory> drawingSessionFactory);

        IFACEMETHOD(CreateDrawingSession)(
            Color clearColor,
            ICanvasDrawingSession** drawingSession) override;

        IFACEMETHOD(CreateDrawingSessionWithUpdateRectangle)(
            Color clearColor,
            Rect updateRectangle,
            ICanvasDrawingSession** drawingSession) override;

        IFACEMETHOD(get_Device)(
            ICanvasDevice** value) override;

        IFACEMETHOD(Recreate)(
            ICanvasResourceCreator* value) override;

        IFACEMETHOD(get_Dpi)(
            float* dpi) override;

        IFACEMETHOD(ConvertPixelsToDips)(
            int pixels,
            float* dips) override;

        IFACEMETHOD(ConvertDipsToPixels)(
            float dips, 
            CanvasDpiRounding dpiRounding,
            int* pixels) override;

        IFACEMETHOD(get_SizeInPixels)(
            BitmapSize* size) override;

        IFACEMETHOD(get_Size)(
            ABI::Windows::Foundation::Size* size) override;

        IFACEMETHOD(get_AlphaMode)(
            CanvasAlphaMode* value) override;

    private:
        void CreateBaseClass(
            ISurfaceImageSourceFactory* surfaceImageSourceFactory,
            bool isOpaque);

        void SetDevice(ICanvasDevice* device);
    };


    //
    // Drawing session factory
    //

    class CanvasImageSourceDrawingSessionFactory : public ICanvasImageSourceDrawingSessionFactory,
                                                   private LifespanTracker<CanvasImageSourceDrawingSessionFactory>
    {
    public:
        virtual ComPtr<ICanvasDrawingSession> Create(
            ICanvasDevice* owner,
            ISurfaceImageSourceNativeWithD2D* sisNative,
            std::shared_ptr<bool> hasActiveDrawingSession,
            Color const& clearColor,
            Rect const& updateRectangleInDips,
            float dpi) const override;
    };
}}}}}}
