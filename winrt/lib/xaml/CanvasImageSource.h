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
            _In_         ICanvasResourceCreatorWithDpi* resourceCreator,
            _In_         Size size,
            _COM_Outptr_ ICanvasImageSource** imageSource) override;

        IFACEMETHOD(CreateWithWidthAndHeight)(
            _In_         ICanvasResourceCreatorWithDpi* resourceCreator,
            _In_         float width,
            _In_         float height,
            _COM_Outptr_ ICanvasImageSource** imageSource) override;

        IFACEMETHOD(CreateWithWidthAndHeightAndDpi)(
            _In_         ICanvasResourceCreator* resourceCreator,
            _In_         float width,
            _In_         float height,
            _In_         float dpi,
            _COM_Outptr_ ICanvasImageSource** imageSource) override;

        IFACEMETHOD(CreateWithWidthAndHeightAndDpiAndAlphaMode)(
            _In_         ICanvasResourceCreator* resourceCreator,
            _In_         float width,
            _In_         float height,
            _In_         float dpi,
            _In_         CanvasAlphaMode alphaMode,
            _COM_Outptr_ ICanvasImageSource** imageSource) override;
    };


    class CanvasImageSource : public RuntimeClass<ICanvasImageSource, ComposableBase<>>,
                              private LifespanTracker<CanvasImageSource>
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_UI_Xaml_CanvasImageSource, BaseTrust);

        ComPtr<ICanvasDevice> m_device;
        std::shared_ptr<ICanvasImageSourceDrawingSessionFactory> m_drawingSessionFactory;
        const float m_width;
        const float m_height;
        const float m_dpi;
        const CanvasAlphaMode m_alphaMode;

    public:
        CanvasImageSource(
            _In_ ICanvasResourceCreator* resourceCreator,
            _In_ float width,
            _In_ float height,
            _In_ float dpi,
            _In_ CanvasAlphaMode alphaMode,
            _In_ ISurfaceImageSourceFactory* surfaceImageSourceFactory,
            _In_ std::shared_ptr<ICanvasImageSourceDrawingSessionFactory> drawingSessionFactory);

        IFACEMETHOD(CreateDrawingSession)(
            _In_         Color clearColor,
            _COM_Outptr_ ICanvasDrawingSession** drawingSession) override;

        IFACEMETHOD(CreateDrawingSessionWithUpdateRectangle)(
            _In_         Color clearColor,
            _In_         Rect updateRectangle,
            _COM_Outptr_ ICanvasDrawingSession** drawingSession) override;

        IFACEMETHOD(get_Device)(
            _COM_Outptr_ ICanvasDevice** value) override;

        IFACEMETHOD(Recreate)(
            _In_ ICanvasResourceCreator* value) override;

        IFACEMETHOD(get_Dpi)(
            _Out_ float* dpi) override;

        IFACEMETHOD(ConvertPixelsToDips)(
            _In_  int pixels,
            _Out_ float* dips) override;

        IFACEMETHOD(ConvertDipsToPixels)(
            _In_  float dips, 
            _In_ CanvasDpiRounding dpiRounding,
            _Out_ int* pixels) override;

        IFACEMETHOD(get_SizeInPixels)(
            _Out_ BitmapSize* size) override;

        IFACEMETHOD(get_Size)(
            _Out_ ABI::Windows::Foundation::Size* size) override;

        IFACEMETHOD(get_AlphaMode)(
            _Out_ CanvasAlphaMode* value) override;

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
        std::shared_ptr<CanvasDrawingSessionManager> m_drawingSessionManager;

    public:
        CanvasImageSourceDrawingSessionFactory();

        virtual ComPtr<ICanvasDrawingSession> Create(
            ICanvasDevice* owner,
            ISurfaceImageSourceNativeWithD2D* sisNative,
            Color const& clearColor,
            Rect const& updateRectangleInDips,
            float dpi) const override;
    };
}}}}}}
