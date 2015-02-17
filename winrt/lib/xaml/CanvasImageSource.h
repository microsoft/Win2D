// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may
// not use these files except in compliance with the License. You may obtain
// a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
// License for the specific language governing permissions and limitations
// under the License.

#pragma once

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas
{
    using namespace ABI::Windows::Foundation;
    using namespace ABI::Windows::UI::Xaml::Media::Imaging;
    using namespace ::Microsoft::WRL;

    class ICanvasImageSourceDrawingSessionFactory
    {
    public:
        virtual ComPtr<ICanvasDrawingSession> Create(
            ICanvasDevice* owner,
            ISurfaceImageSourceNativeWithD2D* sisNative,
            Color const& clearColor,
            RECT const& updateRectangle,
            float dpi) const = 0;
    };


    class CanvasImageSourceFactory : public ActivationFactory<ICanvasImageSourceFactory>,
                                     private LifespanTracker<CanvasImageSourceFactory>
    {
        InspectableClassStatic(RuntimeClass_Microsoft_Graphics_Canvas_CanvasImageSource, BaseTrust);

        std::shared_ptr<ICanvasImageSourceDrawingSessionFactory> m_drawingSessionFactory;

    public:
        CanvasImageSourceFactory();

        IFACEMETHOD(Create)(
            _In_         ICanvasResourceCreatorWithDpi* resourceCreator,
            _In_         float width,
            _In_         float height,
            _COM_Outptr_ ICanvasImageSource** imageSource) override;

        IFACEMETHOD(CreateWithDpi)(
            _In_         ICanvasResourceCreator* resourceCreator,
            _In_         float width,
            _In_         float height,
            _In_         float dpi,
            _COM_Outptr_ ICanvasImageSource** imageSource) override;

        IFACEMETHOD(CreateWithDpiAndBackground)(
            _In_         ICanvasResourceCreator* resourceCreator,
            _In_         float width,
            _In_         float height,
            _In_         float dpi,
            _In_         CanvasBackground background,
            _COM_Outptr_ ICanvasImageSource** imageSource) override;
    };


    class CanvasImageSource : public RuntimeClass<ICanvasImageSource, ComposableBase<>>,
                              private LifespanTracker<CanvasImageSource>
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_CanvasImageSource, BaseTrust);

        ComPtr<ICanvasDevice> m_device;
        std::shared_ptr<ICanvasImageSourceDrawingSessionFactory> m_drawingSessionFactory;
        const float m_width;
        const float m_height;
        const float m_dpi;
        const CanvasBackground m_background;

    public:
        CanvasImageSource(
            _In_ ICanvasResourceCreator* resourceCreator,
            _In_ float width,
            _In_ float height,
            _In_ float dpi,
            _In_ CanvasBackground background,
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

        IFACEMETHOD(put_Device)(
            _In_ ICanvasDevice* value) override;

        IFACEMETHOD(get_Dpi)(
            _Out_ float* dpi) override;

        IFACEMETHOD(ConvertPixelsToDips)(
            _In_  int pixels,
            _Out_ float* dips) override;

        IFACEMETHOD(ConvertDipsToPixels)(
            _In_  float dips, 
            _Out_ int* pixels) override;

        IFACEMETHOD(get_SizeInPixels)(
            _Out_ BitmapSize* size) override;

        IFACEMETHOD(get_Size)(
            _Out_ ABI::Windows::Foundation::Size* size) override;

        IFACEMETHOD(get_Background)(
            _Out_ CanvasBackground* value) override;

    private:
        void CreateBaseClass(
            ISurfaceImageSourceFactory* surfaceImageSourceFactory,
            bool isOpaque);

        void SetResourceCreator(ICanvasResourceCreator* resourceCreator);
    };


    //
    // Drawing session factory
    //

    class CanvasDrawingSessionManager;

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
            RECT const& updateRectangle,
            float dpi) const override;
    };
}}}}
