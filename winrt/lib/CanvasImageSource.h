// Copyright (c) Microsoft Corporation.  All rights reserved

#pragma once

namespace canvas
{
    using namespace ABI::Microsoft::Graphics::Canvas;
    using namespace ABI::Windows::Foundation;
    using namespace ABI::Windows::UI::Xaml::Media::Imaging;
    using namespace Microsoft::WRL;

    class CanvasImageSourceFactory : public ActivationFactory<ICanvasImageSourceFactory>
    {
        InspectableClassStatic(RuntimeClass_Microsoft_Graphics_Canvas_CanvasImageSource, BaseTrust);

    public:
        IFACEMETHOD(Create)(
            _In_         ICanvasDevice* device,
            _In_         int32_t widthInPixels,
            _In_         int32_t heightInPixels,
            _COM_Outptr_ ICanvasImageSource** imageSource) override;

        IFACEMETHOD(CreateWithBackground)(
            _In_         ICanvasDevice* device,
            _In_         int32_t widthInPixels,
            _In_         int32_t heightInPixels,
            _In_         CanvasBackground background,
            _COM_Outptr_ ICanvasImageSource** imageSource) override;
    };


    class CanvasImageSourceDrawingSessionFactory
    {
    public:
        virtual ComPtr<ICanvasDrawingSession> Create(
            ICanvasDeviceInternal* canvasDevice,
            const Rect& updateRect) = 0;
    };


    class CanvasImageSource : public RuntimeClass<
        ICanvasImageSource,
        ComposableBase<ISurfaceImageSourceFactory>>
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_CanvasImageSource, BaseTrust);

        ComPtr<ICanvasDevice> m_Device;
        std::shared_ptr<CanvasImageSourceDrawingSessionFactory> m_DrawingSessionFactory;
        const int32_t m_WidthInPixels;
        const int32_t m_HeightInPixels;

    public:
        CanvasImageSource(
            _In_ ICanvasDevice* device,
            _In_ int32_t widthInPixels,
            _In_ int32_t heightInPixels,
            _In_ CanvasBackground background,
            _In_ ISurfaceImageSourceFactory* surfaceImageSourceFactory,
            _In_ std::shared_ptr<CanvasImageSourceDrawingSessionFactory> drawingSessionFactory);

        IFACEMETHOD(CreateDrawingSession)(
            _COM_Outptr_ ICanvasDrawingSession** drawingSession) override;

        IFACEMETHOD(CreateDrawingSessionWithUpdateRegion)(
            _In_         Rect updateRegion,
            _COM_Outptr_ ICanvasDrawingSession** drawingSession) override;

        IFACEMETHOD(get_Device)(
            _COM_Outptr_ ICanvasDevice** value) override;

        IFACEMETHOD(put_Device)(
            _In_ ICanvasDevice* value) override;

    private:
        void CreateBaseClass(
            ISurfaceImageSourceFactory* surfaceImageSourceFactory,
            bool isOpaque);

        void SetDevice(ICanvasDevice* device);
    };
}
