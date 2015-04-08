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
#include "AnimatedControlInput.h"
#include "CanvasSwapChainPanel.h"
#include "StepTimer.h"
#include "AnimatedControlAsyncAction.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas
{
    using namespace ABI::Windows::ApplicationModel;
    using namespace ABI::Windows::UI::Xaml::Controls;
    using namespace ABI::Windows::UI::Xaml;
    using namespace ABI::Windows::Foundation;
    using namespace ABI::Windows::System::Threading;

    class CanvasAnimatedUpdateEventArgs : public RuntimeClass<ICanvasAnimatedUpdateEventArgs>,
                                          private LifespanTracker<CanvasAnimatedUpdateEventArgs>
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_CanvasAnimatedUpdateEventArgs, BaseTrust);
        
        CanvasTimingInformation m_timingInformation;

    public:
        CanvasAnimatedUpdateEventArgs(CanvasTimingInformation timing);

        IFACEMETHODIMP get_Timing(CanvasTimingInformation* value);
    };

    class CanvasAnimatedUpdateEventArgsFactory : public ActivationFactory<ICanvasAnimatedUpdateEventArgsFactory>,
                                                 private LifespanTracker<CanvasAnimatedUpdateEventArgsFactory>
    {
        InspectableClassStatic(RuntimeClass_Microsoft_Graphics_Canvas_CanvasAnimatedUpdateEventArgs, BaseTrust);

    public:
        IFACEMETHOD(Create)(
            CanvasTimingInformation timingInformation,
            ICanvasAnimatedUpdateEventArgs** updateEventArgs);
    };

    class CanvasAnimatedDrawEventArgsFactory : public ActivationFactory<ICanvasAnimatedDrawEventArgsFactory>,
                                               private LifespanTracker<CanvasAnimatedDrawEventArgsFactory>
    {
        InspectableClassStatic(RuntimeClass_Microsoft_Graphics_Canvas_CanvasAnimatedDrawEventArgs, BaseTrust);

    public:
        IFACEMETHOD(Create)(
            ICanvasDrawingSession* drawingSession,
            CanvasTimingInformation timingInformation,
            ICanvasAnimatedDrawEventArgs** drawEventArgs) override;
    };

    class CanvasAnimatedDrawEventArgs : public RuntimeClass<ICanvasAnimatedDrawEventArgs>,
                                        private LifespanTracker<CanvasAnimatedDrawEventArgs>
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

        virtual ComPtr<IAsyncAction> StartUpdateRenderLoop(
            std::function<void()> const& beforeLoopFn,
            std::function<bool()> const& tickFn,
            std::function<void()> const& afterLoopFn) = 0;

        virtual void StartChangedAction(ComPtr<IWindow> const& window, std::function<void()> changedFn) = 0;
    };

    class CanvasRenderLoop
    {
        ComPtr<IThreadPoolStatics> m_threadPoolStatics;

    public:
        CanvasRenderLoop(IThreadPoolStatics* threadPoolStatics)
            : m_threadPoolStatics(threadPoolStatics)
        { }

        ComPtr<IAsyncAction> StartUpdateRenderLoop(
            std::function<void()> const& beforeLoopFn,
            std::function<bool()> const& tickFn,
            std::function<void()> const& afterLoopFn);

        void StartChangedAction(ComPtr<IWindow> const& window, std::function<void()> changedFn);
    };

    class CanvasAnimatedControl : public RuntimeClass<
        MixIn<CanvasAnimatedControl, BaseControl<CanvasAnimatedControlTraits>>,
        ComposableBase<>>,
        public BaseControl<CanvasAnimatedControlTraits>
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_CanvasAnimatedControl, BaseTrust);

        EventSource<Animated_UpdateEventHandler, InvokeModeOptions<StopOnFirstError>> m_updateEventList;

        ComPtr<ICanvasSwapChainPanel> m_canvasSwapChainPanel;

        ComPtr<AnimatedControlInput> m_input;

        ComPtr<IAsyncAction> m_renderLoopAction;
        
        ComPtr<ISuspendingDeferral> m_suspendingDeferral;

        StepTimer m_stepTimer;
        bool m_hasUpdated;

        //
        // State shared between the UI thread and the update/render thread.
        // Access to this must be guarded using BaseControl's mutex.
        //
        struct SharedState
        {
            SharedState()
                : IsPaused(false)
                , FirstTickAfterWasPaused(true)
                , IsStepTimerFixedStep(false)
                , TargetElapsedTime(0)
                , ShouldResetElapsedTime(false)
                , NeedsDraw(true)
            {}

            bool IsPaused;
            bool FirstTickAfterWasPaused;
            bool IsStepTimerFixedStep;
            uint64_t TargetElapsedTime;
            bool ShouldResetElapsedTime;
            bool NeedsDraw;
            std::vector<ComPtr<AnimatedControlAsyncAction>> PendingAsyncActions;
        };

        SharedState m_sharedState;

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

        IFACEMETHODIMP ResetElapsedTime() override;

        IFACEMETHODIMP get_Input(ICorePointerInputSource** value) override;

        IFACEMETHODIMP RemoveFromVisualTree() override;

        IFACEMETHODIMP get_HasGameLoopThreadAccess(boolean* value) override;

        IFACEMETHODIMP RunOnGameLoopThreadAsync(
            IDispatchedHandler* callback,
            IAsyncAction** asyncAction) override;

        //
        // BaseControl
        //

        virtual void CreateOrUpdateRenderTarget(
            ICanvasDevice* device,
            CanvasAlphaMode newAlphaMode,
            float newDpi,
            Size newSize,
            RenderTarget* renderTarget) override final;

        virtual ComPtr<CanvasAnimatedDrawEventArgs> CreateDrawEventArgs(
            ICanvasDrawingSession* drawingSession,
            bool isRunningSlowly) override final;

        virtual void Changed(Lock const& lock, ChangeReason reason = ChangeReason::Other) override final;
        virtual void Loaded() override final;
        virtual void Unloaded() override final;
        virtual void ApplicationSuspending(ISuspendingEventArgs* args) override final;
        virtual void ApplicationResuming() override final;

    private:
        void CreateSwapChainPanel();

        bool Tick(
            CanvasSwapChain* target, 
            bool areResourcesCreated);

        struct UpdateResult
        {
            bool Updated;
            bool IsRunningSlowly;
        };

        UpdateResult Update(bool forceUpdate);

        void ChangedImpl();

        CanvasTimingInformation GetTimingInformationFromTimer();

        void CanvasAnimatedControl::IssueAsyncActions(
            std::vector<ComPtr<AnimatedControlAsyncAction>> const& pendingActions);
    };

}}}}
