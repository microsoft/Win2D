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

#include <Canvas.abi.h>

namespace canvas
{
    using namespace ABI::Windows::Foundation;
    using namespace ABI::Microsoft::Graphics::Canvas;
    using namespace ABI::Windows::UI::Xaml;
    using namespace ABI::Windows::UI::Xaml::Controls;
    using namespace ABI::Windows::UI::Xaml::Media;
    using namespace ABI::Windows::Graphics::Display;

    // This helps with cases where we need to explicitly qualify types
    namespace canvasABI = ABI::Microsoft::Graphics::Canvas;

    class CanvasDrawEventArgsFactory : public ActivationFactory<ICanvasDrawEventArgsFactory>
    {
        InspectableClassStatic(RuntimeClass_Microsoft_Graphics_Canvas_CanvasDrawEventArgs, BaseTrust);

    public:
        IFACEMETHOD(Create)(
            ICanvasDrawingSession* drawingSession,
            ICanvasDrawEventArgs** drawEventArgs) override;
    };

    class CanvasDrawEventArgs : public RuntimeClass<
        ICanvasDrawEventArgs>
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_CanvasDrawEventArgs, BaseTrust);

        ClosablePtr<ICanvasDrawingSession> m_drawingSession;

     public:
         CanvasDrawEventArgs(ICanvasDrawingSession* drawingSession);

         IFACEMETHODIMP get_DrawingSession(ICanvasDrawingSession** value);
    };

    typedef ITypedEventHandler<canvasABI::CanvasControl*, IInspectable*> CreateResourcesEventHandlerType;
    typedef ITypedEventHandler<canvasABI::CanvasControl*, canvasABI::CanvasDrawEventArgs*> DrawEventHandlerType;

    class ICanvasControlAdapter
    {
    public:
        virtual std::pair<ComPtr<IInspectable>, ComPtr<IUserControl>> CreateUserControl(IInspectable* canvasControl) = 0;
        virtual ComPtr<ICanvasDevice> CreateCanvasDevice() = 0;
        virtual EventRegistrationToken AddCompositionRenderingCallback(IEventHandler<IInspectable*>*) = 0;
        virtual void RemoveCompositionRenderingCallback(EventRegistrationToken token) = 0;
        virtual ComPtr<ICanvasImageSource> CreateCanvasImageSource(ICanvasDevice* device, int width, int height) = 0;
        virtual ComPtr<IImage> CreateImageControl() = 0;
        virtual float GetLogicalDpi() = 0;
        virtual EventRegistrationToken AddDpiChangedCallback(ITypedEventHandler<DisplayInformation*, IInspectable*>* handler) = 0;
        virtual bool IsDesignModeEnabled() = 0;
    };

    class CanvasControl : public RuntimeClass<
        RuntimeClassFlags<WinRtClassicComMix>,
        ICanvasControl,
        ICanvasResourceCreator,
        ABI::Windows::UI::Xaml::IFrameworkElementOverrides,
        ComposableBase<ABI::Windows::UI::Xaml::Controls::IUserControl>>
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_CanvasControl, BaseTrust);

        std::mutex m_drawLock;

        std::shared_ptr<ICanvasControlAdapter> m_adapter;

        EventSource<CreateResourcesEventHandlerType> m_createResourcesEventList;
        EventSource<DrawEventHandlerType> m_drawEventList;

        EventRegistrationToken m_renderingEventToken;

        ComPtr<ICanvasDevice> m_canvasDevice;
        ComPtr<IImage> m_imageControl;
        ComPtr<ICanvasImageSource> m_canvasImageSource;
        bool m_drawNeeded;
        bool m_isLoaded;

        int m_currentWidth;
        int m_currentHeight;
        
    public:
        CanvasControl(
            std::shared_ptr<ICanvasControlAdapter> adapter);

        //
        // ICanvasControl
        //

        IFACEMETHODIMP add_CreateResources(
            CreateResourcesEventHandlerType* value,
            EventRegistrationToken *token);

        IFACEMETHODIMP remove_CreateResources(
            EventRegistrationToken token);

        IFACEMETHODIMP add_Draw(
            DrawEventHandlerType* value,
            EventRegistrationToken* token);

        IFACEMETHODIMP remove_Draw(
            EventRegistrationToken token);

        //
        // ICanvasResourceCreator
        //

        IFACEMETHODIMP get_Device(ICanvasDevice** value);

        IFACEMETHODIMP Invalidate();

        //
        // IFrameworkElementOverrides
        //

        IFACEMETHODIMP MeasureOverride(
            ABI::Windows::Foundation::Size availableSize, 
            ABI::Windows::Foundation::Size* returnValue);

        IFACEMETHODIMP ArrangeOverride(
            ABI::Windows::Foundation::Size finalSize, 
            ABI::Windows::Foundation::Size* returnValue);

        IFACEMETHODIMP OnApplyTemplate();

        HRESULT OnLoaded(IInspectable* sender, IRoutedEventArgs* args);
        HRESULT OnSizeChanged(IInspectable* sender, ISizeChangedEventArgs* args);

    private:
        void CreateBaseClass();
        void CreateImageControl();
        void RegisterEventHandlers();

        void ClearDrawNeeded();
        void EnsureSizeDependentResources();
        void CallDrawHandlers();
        void InvalidateImpl();

        HRESULT OnRenderCallback(IInspectable *sender, IInspectable *args);   
        
        HRESULT OnDpiChangedCallback(IDisplayInformation* sender, IInspectable* args);
    };
}
