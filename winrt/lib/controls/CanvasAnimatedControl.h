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

#include "BaseControl.h"
#include "CanvasSwapChainPanel.h"
#include "CanvasSwapChain.h"
#include "StepTimer.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas
{
    using namespace ABI::Windows::ApplicationModel;
    using namespace ABI::Windows::UI::Xaml::Controls;
    using namespace ABI::Windows::UI::Xaml;
    using namespace ABI::Windows::Foundation;
    using namespace ABI::Windows::System::Threading;

    class CanvasAnimatedUpdateEventArgs : public RuntimeClass<
        ICanvasAnimatedUpdateEventArgs>
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_CanvasAnimatedUpdateEventArgs, BaseTrust);
        
        CanvasTimingInformation m_timingInformation;

    public:
        CanvasAnimatedUpdateEventArgs(CanvasTimingInformation timing);

        IFACEMETHODIMP get_Timing(CanvasTimingInformation* value);
    };

    class CanvasAnimatedUpdateEventArgsFactory : public ActivationFactory<ICanvasAnimatedUpdateEventArgsFactory>
    {
        InspectableClassStatic(RuntimeClass_Microsoft_Graphics_Canvas_CanvasAnimatedUpdateEventArgs, BaseTrust);

    public:
        IFACEMETHOD(Create)(
            CanvasTimingInformation timingInformation,
            ICanvasAnimatedUpdateEventArgs** updateEventArgs);
    };

    class CanvasAnimatedDrawEventArgsFactory : public ActivationFactory<ICanvasAnimatedDrawEventArgsFactory>
    {
        InspectableClassStatic(RuntimeClass_Microsoft_Graphics_Canvas_CanvasAnimatedDrawEventArgs, BaseTrust);

    public:
        IFACEMETHOD(Create)(
            ICanvasDrawingSession* drawingSession,
            CanvasTimingInformation timingInformation,
            ICanvasAnimatedDrawEventArgs** drawEventArgs) override;
    };

    class CanvasAnimatedDrawEventArgs : public RuntimeClass<
        ICanvasAnimatedDrawEventArgs>
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_CanvasAnimatedDrawEventArgs, BaseTrust);

        ClosablePtr<ICanvasDrawingSession> m_drawingSession;

        CanvasTimingInformation m_timingInformation;

     public:
         CanvasAnimatedDrawEventArgs(ICanvasDrawingSession* drawingSession, CanvasTimingInformation timingInformation);

         IFACEMETHODIMP get_DrawingSession(ICanvasDrawingSession** value);

         IFACEMETHODIMP get_Timing(CanvasTimingInformation* value);
    };

    typedef ITypedEventHandler<CanvasAnimatedControl*, CanvasCreateResourcesEventArgs*> Animated_CreateResourcesEventHandler;
    typedef ITypedEventHandler<ICanvasAnimatedControl*, CanvasAnimatedUpdateEventArgs*> Animated_UpdateEventHandler;
    typedef ITypedEventHandler<ICanvasAnimatedControl*, CanvasAnimatedDrawEventArgs*> Animated_DrawEventHandler;

    class CanvasAnimatedControl;
    class ICanvasAnimatedControlAdapter;

    struct CanvasAnimatedControlTraits
    {
        typedef ICanvasAnimatedControlAdapter adapter_t;
        typedef Animated_CreateResourcesEventHandler createResourcesEventHandler_t;
        typedef Animated_DrawEventHandler drawEventHandler_t;
        typedef CanvasAnimatedDrawEventArgs drawEventArgs_t;
        typedef CanvasAnimatedControl control_t;
        typedef ICanvasAnimatedControl controlInterface_t;
        typedef CanvasSwapChain renderTarget_t;
    };

    class ICanvasAnimatedControlAdapter : 
        public IBaseControlAdapter<CanvasAnimatedControlTraits>,
        public ICanvasSwapChainPanelAdapter,
        public ICanvasTimingAdapter
    {
    public:
        virtual ComPtr<CanvasSwapChain> CreateCanvasSwapChain(
            ICanvasDevice* device,
            float width, 
            float height, 
            float dpi,
            CanvasAlphaMode alphaMode) = 0;

        virtual ComPtr<CanvasSwapChainPanel> CreateCanvasSwapChainPanel() = 0;

        virtual ComPtr<IAsyncAction> StartUpdateRenderLoop(std::function<bool()> tickFn) = 0;

        virtual ComPtr<IAsyncAction> StartChangedAction(ComPtr<IWindow> const& window, std::function<void()> changedFn) = 0;
    };


    std::shared_ptr<ICanvasAnimatedControlAdapter> CreateCanvasAnimatedControlAdapter(
        IThreadPoolStatics* threadPoolStatics);

    class CanvasAnimatedControl : public RuntimeClass<
        MixIn<CanvasAnimatedControl, BaseControl<CanvasAnimatedControlTraits>>,
        ComposableBase<IUserControl>>,
        public BaseControl<CanvasAnimatedControlTraits>
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_CanvasAnimatedControl, BaseTrust);

        EventSource<Animated_UpdateEventHandler, InvokeModeOptions<StopOnFirstError>> m_updateEventList;

        ComPtr<ICanvasSwapChainPanel> m_canvasSwapChainPanel;

        ComPtr<IAsyncAction> m_renderLoopAction;

        StepTimer m_stepTimer;

        std::mutex m_mutex;

        //
        // The variables below are protected by m_mutex:
        //
        ComPtr<IAsyncAction> m_changedAction;       

        bool m_isPaused;

        Size m_currentSize; // We need to store this so we can access it from the update/render loop     

        bool m_needToRestartRenderThread;

        bool m_forceUpdate;

        // These are duplicated from the step timer, to avoid putting locks around StepTimer as a whole.
        bool m_isStepTimerFixedStep;
        uint64_t m_targetElapsedTime;
        bool m_shouldResetElapsedTime;

    public:
        CanvasAnimatedControl(
            std::shared_ptr<ICanvasAnimatedControlAdapter> adapter);

        ~CanvasAnimatedControl();

        //
        // ICanvasControl
        //

        IFACEMETHODIMP add_Update(
            Animated_UpdateEventHandler* value,
            EventRegistrationToken *token) override;

        IFACEMETHODIMP remove_Update(
            EventRegistrationToken token) override;

        IFACEMETHODIMP put_IsFixedTimeStep(boolean value) override;

        IFACEMETHODIMP get_IsFixedTimeStep(boolean* value) override;

        IFACEMETHODIMP put_TargetElapsedTime(TimeSpan value) override;

        IFACEMETHODIMP get_TargetElapsedTime(TimeSpan* value) override;

        IFACEMETHODIMP put_Paused(boolean value) override;

        IFACEMETHODIMP get_Paused(boolean* value) override;

        IFACEMETHODIMP get_Size(Size* value) override;

        IFACEMETHODIMP ResetElapsedTime() override;

        //
        // BaseControl
        //

        virtual void CreateOrUpdateRenderTarget(
            ICanvasDevice* device,
            CanvasBackground newBackgroundMode,
            float newDpi,
            Size newSize,
            RenderTarget* renderTarget) override final;

        virtual ComPtr<CanvasAnimatedDrawEventArgs> CreateDrawEventArgs(
            ICanvasDrawingSession* drawingSession) override final;

        virtual void Changed() override final;

        virtual void ChangedClearColor(bool differentAlphaMode) override final;

        virtual void ChangedSize() override final;

        virtual void Unloaded() override final;

        virtual void CheckThreadRestrictionIfNecessary() override final;

    private:
        std::unique_lock<std::mutex> GetLock()
        { return std::unique_lock<std::mutex>(m_mutex); }

        void CreateSwapChainPanel();

        bool Tick(
            CanvasSwapChain* target, 
            Color const& clearColor,
            bool areResourcesCreated);

        bool Update();

        HRESULT OnRenderLoopCompleted(IAsyncAction*, AsyncStatus);

        void ChangedImpl();

        CanvasTimingInformation GetTimingInformationFromTimer();

        void CheckUIThreadAccess();
    };

}}}}
