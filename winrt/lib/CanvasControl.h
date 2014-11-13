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

#include <RegisteredEvent.h>

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas
{
    using namespace ABI::Windows::ApplicationModel;
    using namespace ABI::Windows::Foundation;
    using namespace ABI::Windows::Graphics::Display;
    using namespace ABI::Windows::UI::Core;
    using namespace ABI::Windows::UI::Xaml::Controls;
    using namespace ABI::Windows::UI::Xaml::Media;
    using namespace ABI::Windows::UI::Xaml;

    class IRecreatableDeviceManager;

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

    typedef ITypedEventHandler<CanvasControl*, IInspectable*> CreateResourcesEventHandler;
    typedef ITypedEventHandler<CanvasControl*, CanvasDrawEventArgs*> DrawEventHandler;
    typedef ITypedEventHandler<DisplayInformation*, IInspectable*> DpiChangedEventHandler;

    class ICanvasControlAdapter
    {
    public:
        virtual std::pair<ComPtr<IInspectable>, ComPtr<IUserControl>> CreateUserControl(IInspectable* canvasControl) = 0;
        virtual std::unique_ptr<IRecreatableDeviceManager> CreateRecreatableDeviceManager() = 0;
        virtual RegisteredEvent AddApplicationSuspendingCallback(IEventHandler<SuspendingEventArgs*>*) = 0;
        virtual RegisteredEvent AddCompositionRenderingCallback(IEventHandler<IInspectable*>*) = 0;
        virtual RegisteredEvent AddSurfaceContentsLostCallback(IEventHandler<IInspectable*>*) = 0;
        virtual RegisteredEvent AddVisibilityChangedCallback(IWindowVisibilityChangedEventHandler*, IWindow*) = 0;
        virtual ComPtr<CanvasImageSource> CreateCanvasImageSource(ICanvasDevice* device, float width, float height, float dpi, CanvasBackground backgroundMode) = 0;
        virtual ComPtr<IImage> CreateImageControl() = 0;
        virtual float GetLogicalDpi() = 0;

        virtual RegisteredEvent AddDpiChangedCallback(DpiChangedEventHandler* handler) = 0;

        virtual ComPtr<IWindow> GetCurrentWindow() = 0;

        //
        // Helpers for adding member-function callbacks
        //

#define CB_HELPER(NAME)                                                 \
        template<typename T, typename METHOD, typename... EXTRA_ARGS>   \
        RegisteredEvent NAME(T* obj, METHOD method, EXTRA_ARGS... extraArgs) \
        {                                                               \
            return AddCallback(&ICanvasControlAdapter::NAME, obj, method, extraArgs...); \
        }

        CB_HELPER(AddApplicationSuspendingCallback);
        CB_HELPER(AddCompositionRenderingCallback);
        CB_HELPER(AddSurfaceContentsLostCallback);
        CB_HELPER(AddDpiChangedCallback);
        CB_HELPER(AddVisibilityChangedCallback);

#undef CB_HELPER

        template<typename DELEGATE, typename T, typename METHOD, typename... EXTRA_ARGS> 
        RegisteredEvent AddCallback(RegisteredEvent (ICanvasControlAdapter::* addMethod)(DELEGATE*, EXTRA_ARGS...), T* obj, METHOD method, EXTRA_ARGS... extraArgs)
        {
            auto callback = Callback<DELEGATE>(obj, method);
            CheckMakeResult(callback);
            return (this->*addMethod)(callback.Get(), extraArgs...);
        }
    };

    class CanvasControl : public RuntimeClass<
        RuntimeClassFlags<WinRtClassicComMix>,
        ICanvasControl,
        ICanvasResourceCreatorWithDpi,
        ICanvasResourceCreator,
        ABI::Windows::UI::Xaml::IFrameworkElementOverrides,
        ComposableBase<ABI::Windows::UI::Xaml::Controls::IUserControl>>
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_CanvasControl, BaseTrust);

        std::shared_ptr<ICanvasControlAdapter> m_adapter;

        // The current window is thread local.  We grab this on construction
        // since this will happen on the correct thread.  From then on we use
        // this stored value since we can't always be sure that we'll always be
        // called from that window's thread.
        ComPtr<IWindow> m_window;

        std::unique_ptr<IRecreatableDeviceManager> m_recreatableDeviceManager;

        EventSource<DrawEventHandler, InvokeModeOptions<StopOnFirstError>> m_drawEventList;

        RegisteredEvent m_applicationSuspendingEventRegistration;
        RegisteredEvent m_surfaceContentsLostEventRegistration;
        RegisteredEvent m_windowVisibilityChangedEventRegistration;
        RegisteredEvent m_dpiChangedEventRegistration;

        ComPtr<IImage> m_imageControl;
        ComPtr<CanvasImageSource> m_canvasImageSource;
        float m_imageSourceDpi;
        float m_imageSourceWidth;
        float m_imageSourceHeight;

        bool m_isLoaded;
        float m_dpi;

        class GuardedState;
        std::unique_ptr<GuardedState> m_guardedState;
        
    public:
        CanvasControl(
            std::shared_ptr<ICanvasControlAdapter> adapter);

        ~CanvasControl();

        //
        // ICanvasControl
        //

        IFACEMETHODIMP add_CreateResources(
            CreateResourcesEventHandler* value,
            EventRegistrationToken *token) override;

        IFACEMETHODIMP remove_CreateResources(
            EventRegistrationToken token) override;

        IFACEMETHODIMP add_Draw(
            DrawEventHandler* value,
            EventRegistrationToken* token) override;

        IFACEMETHODIMP remove_Draw(
            EventRegistrationToken token) override;

        IFACEMETHODIMP put_ClearColor(Color value) override;

        IFACEMETHODIMP get_ClearColor(Color* value) override;

        IFACEMETHODIMP Invalidate() override;

        //
        // ICanvasResourceCreator
        //

        IFACEMETHODIMP get_Device(ICanvasDevice** value) override;

        //
        // ICanvasResourceCreatorWithDpi
        //

        IFACEMETHODIMP get_Dpi(float* dpi) override;

        IFACEMETHODIMP ConvertPixelsToDips(int pixels, float* dips) override;
        IFACEMETHODIMP ConvertDipsToPixels(float dips, int* pixels) override;

        //
        // IFrameworkElementOverrides
        //

        IFACEMETHODIMP MeasureOverride(
            Size availableSize, 
            Size* returnValue) override;

        IFACEMETHODIMP ArrangeOverride(
            Size finalSize, 
            Size* returnValue) override;

        IFACEMETHODIMP OnApplyTemplate() override;

    private:
        enum class InvalidateReason
        {
            Default,
            ImageSourceNeedsReset
        };

        // These variables may be accessed by arbitrary threads.  We guard them
        // with a mutex.
        class GuardedState
        {
            std::mutex m_lock;

            Color m_clearColor;

            bool m_imageSourceNeedsReset;

            RegisteredEvent m_renderingEventRegistration;
            bool m_needToHookCompositionRendering;

        public:
            GuardedState();

            void TriggerRender(CanvasControl* control, InvalidateReason reason = InvalidateReason::Default);

            Color PreDrawAndGetClearColor(CanvasControl* control);

            void OnWindowVisibilityChanged(CanvasControl* control, bool isVisible);
            void SetClearColor(CanvasControl* control, Color const& value);
            Color GetClearColor();

        private:
            void TriggerRenderImpl(CanvasControl* control, InvalidateReason reason = InvalidateReason::Default);
            void HookCompositionRenderingIfNecessary(CanvasControl* control);
        };

        void CreateBaseClass();
        void CreateImageControl();
        void RegisterEventHandlers();

        template<typename T, typename DELEGATE, typename HANDLER>
        void RegisterEventHandlerOnSelf(
            ComPtr<T> const& self, 
            HRESULT (STDMETHODCALLTYPE T::* addMethod)(DELEGATE*, EventRegistrationToken*), 
            HANDLER handler);

        bool IsWindowVisible();

        HRESULT OnApplicationSuspending(IInspectable* sender, ISuspendingEventArgs* args);
        HRESULT OnLoaded(IInspectable* sender, IRoutedEventArgs* args);
        HRESULT OnSizeChanged(IInspectable* sender, ISizeChangedEventArgs* args);
        HRESULT OnDpiChanged(IDisplayInformation* sender, IInspectable* args);
        HRESULT OnSurfaceContentsLost(IInspectable* sender, IInspectable* args);
        HRESULT OnWindowVisibilityChanged(IInspectable* sender, IVisibilityChangedEventArgs* args);

        HRESULT OnCompositionRendering(IInspectable* sender, IInspectable* args);        
        void EnsureSizeDependentResources(ICanvasDevice* device, CanvasBackground backgroundMode);
        void CallDrawHandlers(Color const& clearColor);
    };

}}}}
