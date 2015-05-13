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

#include "BaseControl.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace UI { namespace Xaml
{
    using namespace ABI::Windows::ApplicationModel;
    using namespace ABI::Windows::Foundation;
    using namespace ABI::Windows::Graphics::Display;
    using namespace ABI::Windows::UI::Core;
    using namespace ABI::Windows::UI::Xaml::Controls;
    using namespace ABI::Windows::UI::Xaml::Media;
    using namespace ABI::Windows::UI::Xaml;

    class CanvasDrawEventArgsFactory : public ActivationFactory<ICanvasDrawEventArgsFactory>,
                                       private LifespanTracker<CanvasDrawEventArgsFactory>
    {
        InspectableClassStatic(RuntimeClass_Microsoft_Graphics_Canvas_UI_Xaml_CanvasDrawEventArgs, BaseTrust);

    public:
        IFACEMETHOD(Create)(
            ICanvasDrawingSession* drawingSession,
            ICanvasDrawEventArgs** drawEventArgs) override;
    };

    class CanvasDrawEventArgs : public RuntimeClass<ICanvasDrawEventArgs>,
                                private LifespanTracker<CanvasDrawEventArgs>
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_UI_Xaml_CanvasDrawEventArgs, BaseTrust);

        ClosablePtr<ICanvasDrawingSession> m_drawingSession;

     public:
         CanvasDrawEventArgs(ICanvasDrawingSession* drawingSession);

         IFACEMETHODIMP get_DrawingSession(ICanvasDrawingSession** value);
    };

    typedef ITypedEventHandler<CanvasControl*, CanvasDrawEventArgs*> Static_DrawEventHandler;
    typedef ITypedEventHandler<CanvasControl*, CanvasCreateResourcesEventArgs*> Static_CreateResourcesEventHandler;


    class ICanvasControlAdapter;
    class CanvasControl;

    struct CanvasControlTraits
    {
        typedef ICanvasControlAdapter adapter_t;
        typedef Static_CreateResourcesEventHandler createResourcesEventHandler_t;
        typedef Static_DrawEventHandler drawEventHandler_t;
        typedef CanvasDrawEventArgs drawEventArgs_t;
        typedef CanvasControl control_t;
        typedef ICanvasControl controlInterface_t;
        typedef CanvasImageSource renderTarget_t;
    };

    class ICanvasControlAdapter : public IBaseControlAdapter<CanvasControlTraits>
    {
    public:
        virtual RegisteredEvent AddCompositionRenderingCallback(IEventHandler<IInspectable*>*) = 0;
        virtual RegisteredEvent AddSurfaceContentsLostCallback(IEventHandler<IInspectable*>*) = 0;
        virtual ComPtr<CanvasImageSource> CreateCanvasImageSource(ICanvasDevice* device, float width, float height, float dpi, CanvasAlphaMode alphaMode) = 0;
        virtual ComPtr<IImage> CreateImageControl() = 0;
        
#define CB_HELPER(NAME, DELEGATE)                                       \
        template<typename T, typename METHOD, typename... EXTRA_ARGS>   \
        RegisteredEvent NAME(T* obj, METHOD method, EXTRA_ARGS... extraArgs) \
        {                                                               \
            return AddCallback<DELEGATE>(&ICanvasControlAdapter::NAME, obj, method, extraArgs...); \
        }

        CB_HELPER(AddCompositionRenderingCallback, IEventHandler<IInspectable*>);
        CB_HELPER(AddSurfaceContentsLostCallback, IEventHandler<IInspectable*>);

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
        ABI::Windows::UI::Xaml::IFrameworkElementOverrides,
        MixIn<CanvasControl, BaseControl<CanvasControlTraits>>,
        ComposableBase<>>,
        public BaseControl<CanvasControlTraits>
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_UI_Xaml_CanvasControl, BaseTrust);

        RegisteredEvent m_renderingEventRegistration; // protected by BaseControl's mutex
        bool m_needToHookCompositionRendering;        // protected by BaseControl's mutex

        RegisteredEvent m_surfaceContentsLostEventRegistration;

        ComPtr<IImage> m_imageControl;
        
    public:
        CanvasControl(std::shared_ptr<ICanvasControlAdapter> adapter);

        ~CanvasControl();

        //
        // ICanvasControl
        //

        IFACEMETHODIMP Invalidate() override
        {
            return ExceptionBoundary(
                [&]
                {
                    Changed(GetLock());
                });
        }

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

        //
        // BaseControl
        //

        virtual void CreateOrUpdateRenderTarget(
            ICanvasDevice* device,
            CanvasAlphaMode newAlphaMode,
            float newDpi,
            Size newSize,
            RenderTarget* renderTarget) override final;

        virtual ComPtr<CanvasDrawEventArgs> CreateDrawEventArgs(
            ICanvasDrawingSession* drawingSession,
            bool isRunningSlowly) override final;

        virtual void Changed(Lock const& lock, ChangeReason reason = ChangeReason::Other) override final;
        virtual void Loaded() override final;
        virtual void Unloaded() override final;
        virtual void ApplicationSuspending(ISuspendingEventArgs* args) override final;
        virtual void ApplicationResuming() override final;
        virtual void WindowVisibilityChanged() override final;

    private:
        void HookCompositionRenderingIfNecessary();

        void CreateImageControl();
        void RegisterEventHandlers();
        void UnregisterEventHandlers();

        HRESULT OnCompositionRendering(IInspectable* sender, IInspectable* args);
        HRESULT OnSurfaceContentsLost(IInspectable* sender, IInspectable* args);

        void ChangedImpl();
    };

}}}}}}
