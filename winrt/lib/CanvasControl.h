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

    typedef ITypedEventHandler<CanvasControl*, IInspectable*> CreateResourcesEventHandlerType;
    typedef ITypedEventHandler<CanvasControl*, CanvasDrawEventArgs*> DrawEventHandlerType;

    class ICanvasControlAdapter
    {
    public:
        virtual std::pair<ComPtr<IInspectable>, ComPtr<IUserControl>> CreateUserControl(IInspectable* canvasControl) = 0;
        virtual ComPtr<ICanvasDevice> CreateCanvasDevice() = 0;
        virtual RegisteredEvent AddApplicationSuspendingCallback(IEventHandler<SuspendingEventArgs*>*) = 0;
        virtual RegisteredEvent AddCompositionRenderingCallback(IEventHandler<IInspectable*>*) = 0;
        virtual RegisteredEvent AddSurfaceContentsLostCallback(IEventHandler<IInspectable*>*) = 0;
        virtual RegisteredEvent AddVisibilityChangedCallback(IWindowVisibilityChangedEventHandler*, IWindow*) = 0;
        virtual ComPtr<CanvasImageSource> CreateCanvasImageSource(ICanvasDevice* device, int width, int height) = 0;
        virtual ComPtr<IImage> CreateImageControl() = 0;
        virtual float GetLogicalDpi() = 0;

        typedef ITypedEventHandler<DisplayInformation*, IInspectable*> DpiChangedHandler;
        virtual RegisteredEvent AddDpiChangedCallback(DpiChangedHandler* handler) = 0;

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
        ICanvasResourceCreator,
        ABI::Windows::UI::Xaml::IFrameworkElementOverrides,
        ComposableBase<ABI::Windows::UI::Xaml::Controls::IUserControl>>
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_CanvasControl, BaseTrust);

        std::mutex m_drawLock;

        std::shared_ptr<ICanvasControlAdapter> m_adapter;

        // The current window is thread local.  We grab this on construction
        // since this will happen on the correct thread.  From then on we use
        // this stored value since we can't always be sure that we'll always be
        // called from that window's thread.
        ComPtr<IWindow> m_window;

        EventSource<CreateResourcesEventHandlerType> m_createResourcesEventList;
        EventSource<DrawEventHandlerType> m_drawEventList;

        RegisteredEvent m_applicationSuspendingEventRegistration;
        RegisteredEvent m_surfaceContentsLostEventRegistration;
        RegisteredEvent m_windowVisibilityChangedEventRegistration;
        RegisteredEvent m_renderingEventRegistration;
        RegisteredEvent m_dpiChangedEventRegistration;

        ComPtr<ICanvasDevice> m_canvasDevice;
        ComPtr<IImage> m_imageControl;
        ComPtr<CanvasImageSource> m_canvasImageSource;
        bool m_needToHookCompositionRendering;
        bool m_imageSourceNeedsReset;
        bool m_isLoaded;

        int m_currentWidth;
        int m_currentHeight;
        
    public:
        CanvasControl(
            std::shared_ptr<ICanvasControlAdapter> adapter);

        ~CanvasControl();

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

    private:
        void CreateBaseClass();
        void CreateImageControl();
        void RegisterEventHandlers();

        template<typename T, typename DELEGATE, typename HANDLER>
        void RegisterEventHandlerOnSelf(
            ComPtr<T> const& self, 
            HRESULT (STDMETHODCALLTYPE T::* addMethod)(DELEGATE*, EventRegistrationToken*), 
            HANDLER handler);

        bool IsWindowVisible();

        void PreDraw();
        void EnsureSizeDependentResources();
        void CallDrawHandlers();

        enum class InvalidateReason
        {
            Default,
            SurfaceContentsLost
        };

        void InvalidateImpl(InvalidateReason reason = InvalidateReason::Default);

        void HookCompositionRenderingIfNecessary();

        HRESULT OnApplicationSuspending(IInspectable* sender, ISuspendingEventArgs* args);
        HRESULT OnLoaded(IInspectable* sender, IRoutedEventArgs* args);
        HRESULT OnSizeChanged(IInspectable* sender, ISizeChangedEventArgs* args);
        HRESULT OnCompositionRendering(IInspectable* sender, IInspectable* args);        
        HRESULT OnDpiChanged(IDisplayInformation* sender, IInspectable* args);
        HRESULT OnSurfaceContentsLost(IInspectable* sender, IInspectable* args);
        HRESULT OnWindowVisibilityChanged(IInspectable* sender, IVisibilityChangedEventArgs* args);
    };

}}}}
