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

namespace canvas
{
    using namespace ABI::Microsoft::Graphics::Canvas;
    using namespace ABI::Windows::Foundation;
    using namespace ABI::Windows::UI::Xaml::Media::Imaging;
    using namespace Microsoft::WRL;

    class ICanvasImageSourceDrawingSessionFactory
    {
    public:
        virtual ComPtr<ICanvasDrawingSession> Create(
            ISurfaceImageSourceNativeWithD2D* sisNative,
            const Rect& updateRect) const = 0;
    };


    class CanvasImageSourceFactory : public ActivationFactory<ICanvasImageSourceFactory>
    {
        InspectableClassStatic(RuntimeClass_Microsoft_Graphics_Canvas_CanvasImageSource, BaseTrust);

        std::shared_ptr<ICanvasImageSourceDrawingSessionFactory> m_drawingSessionFactory;

    public:
        CanvasImageSourceFactory();

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


    class CanvasImageSource : public RuntimeClass<
        ICanvasImageSource,
        ComposableBase<ISurfaceImageSourceFactory>>
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_CanvasImageSource, BaseTrust);

        ComPtr<ICanvasDevice> m_device;
        std::shared_ptr<ICanvasImageSourceDrawingSessionFactory> m_drawingSessionFactory;
        const int32_t m_widthInPixels;
        const int32_t m_heightInPixels;

    public:
        CanvasImageSource(
            _In_ ICanvasDevice* device,
            _In_ int32_t widthInPixels,
            _In_ int32_t heightInPixels,
            _In_ CanvasBackground background,
            _In_ ISurfaceImageSourceFactory* surfaceImageSourceFactory,
            _In_ std::shared_ptr<ICanvasImageSourceDrawingSessionFactory> drawingSessionFactory);

        IFACEMETHOD(CreateDrawingSession)(
            _COM_Outptr_ ICanvasDrawingSession** drawingSession) override;

        IFACEMETHOD(CreateDrawingSessionWithUpdateRectangle)(
            _In_         Rect updateRectangle,
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
