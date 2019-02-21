// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#include "AnimatedControlAsyncAction.h"
#include "BaseControlAdapter.h"
#include "CanvasGameLoop.h"
#include "CanvasSwapChainPanel.h"
#include "StepTimer.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace UI { namespace Xaml
{
    using namespace ABI::Windows::ApplicationModel;
    using namespace ABI::Windows::UI::Core;
    using namespace ABI::Windows::UI::Xaml::Controls;
    using namespace ABI::Windows::UI::Xaml::Shapes;
    using namespace ABI::Windows::UI::Xaml;
    using namespace ABI::Windows::Foundation;
    using namespace ABI::Windows::System::Threading;

    class CanvasAnimatedUpdateEventArgs : public RuntimeClass<ICanvasAnimatedUpdateEventArgs>,
                                          private LifespanTracker<CanvasAnimatedUpdateEventArgs>
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_UI_Xaml_CanvasAnimatedUpdateEventArgs, BaseTrust);
        
        CanvasTimingInformation m_timingInformation;

    public:
        CanvasAnimatedUpdateEventArgs(CanvasTimingInformation timing);

        IFACEMETHODIMP get_Timing(CanvasTimingInformation* value);
    };

    class CanvasAnimatedUpdateEventArgsFactory : public AgileActivationFactory<ICanvasAnimatedUpdateEventArgsFactory>,
                                                 private LifespanTracker<CanvasAnimatedUpdateEventArgsFactory>
    {
        InspectableClassStatic(RuntimeClass_Microsoft_Graphics_Canvas_UI_Xaml_CanvasAnimatedUpdateEventArgs, BaseTrust);

    public:
        IFACEMETHOD(Create)(
            CanvasTimingInformation timingInformation,
            ICanvasAnimatedUpdateEventArgs** updateEventArgs);
    };

    class CanvasAnimatedDrawEventArgsFactory : public AgileActivationFactory<ICanvasAnimatedDrawEventArgsFactory>,
                                               private LifespanTracker<CanvasAnimatedDrawEventArgsFactory>
    {
        InspectableClassStatic(RuntimeClass_Microsoft_Graphics_Canvas_UI_Xaml_CanvasAnimatedDrawEventArgs, BaseTrust);

    public:
        IFACEMETHOD(Create)(
            ICanvasDrawingSession* drawingSession,
            CanvasTimingInformation timingInformation,
            ICanvasAnimatedDrawEventArgs** drawEventArgs) override;
    };

    class CanvasAnimatedDrawEventArgs : public RuntimeClass<ICanvasAnimatedDrawEventArgs>,
                                        private LifespanTracker<CanvasAnimatedDrawEventArgs>
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_UI_Xaml_CanvasAnimatedDrawEventArgs, BaseTrust);

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

        virtual ComPtr<IShape> CreateDesignModeShape() = 0;

        virtual std::unique_ptr<CanvasGameLoop> CreateAndStartGameLoop(
            CanvasAnimatedControl* control,
            ISwapChainPanel* swapChainPanel) = 0;

        virtual void Sleep(DWORD timeInMs) = 0;
    };

    std::shared_ptr<ICanvasAnimatedControlAdapter> CreateCanvasAnimatedControlAdapter();

    class CanvasAnimatedControl : public RuntimeClass<
        MixIn<CanvasAnimatedControl, BaseControlWithDrawHandler<CanvasAnimatedControlTraits>>,
        ComposableBase<>>,
        public BaseControlWithDrawHandler<CanvasAnimatedControlTraits>,
        public ICanvasGameLoopClient
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_UI_Xaml_CanvasAnimatedControl, BaseTrust);

        EventSource<Animated_UpdateEventHandler, InvokeModeOptions<StopOnFirstError>> m_updateEventList;
        EventSource<ITypedEventHandler<ICanvasAnimatedControl*, IInspectable*>, InvokeModeOptions<StopOnFirstError>> m_gameLoopStartingEventList;
        EventSource<ITypedEventHandler<ICanvasAnimatedControl*, IInspectable*>, InvokeModeOptions<StopOnFirstError>> m_gameLoopStoppedEventList;

        ComPtr<ICanvasSwapChainPanel> m_canvasSwapChainPanel;
        ComPtr<IShape> m_designModeShape; // in design mode we use a shape rather than a swap chain panel

        std::unique_ptr<CanvasGameLoop> m_gameLoop;
        ComPtr<IAsyncAction> m_renderLoopAction;
        
        ComPtr<ISuspendingDeferral> m_suspendingDeferral;

        StepTimer m_stepTimer;
        bool m_hasUpdated;

        //
        // State shared between the UI thread and the update/render thread.
        // Access to this must be guarded using m_sharedStateMutex
        //
        struct SharedState
        {
            SharedState()
                : IsPaused(false)
                , TimeWhenPausedWasSet{}
                , TimeSpentPaused{}
                , IsStepTimerFixedStep(false)
                , TargetElapsedTime(0)
                , ShouldResetElapsedTime(false)
                , NeedsDraw(true)
                , Invalidated(false)
                , DeviceNeedsReCreationWithNewOptions(false)
                , SizeSeenByGameLoop{}
                , IsInTick(false)
            {}

            bool IsPaused;
            int64_t TimeWhenPausedWasSet;
            int64_t TimeSpentPaused;
            bool IsStepTimerFixedStep;
            uint64_t TargetElapsedTime;
            bool ShouldResetElapsedTime;
            bool NeedsDraw;
            bool Invalidated;
            bool DeviceNeedsReCreationWithNewOptions;
            Size SizeSeenByGameLoop;
            bool IsInTick;
            std::vector<ComPtr<AnimatedControlAsyncAction>> PendingAsyncActions;
        };

        std::mutex m_sharedStateMutex;
        SharedState m_sharedState;

    public:
        CanvasAnimatedControl(
            std::shared_ptr<ICanvasAnimatedControlAdapter> adapter);

        virtual ~CanvasAnimatedControl();

        //
        // ICanvasAnimatedControl
        //

        IFACEMETHODIMP SetVisible(boolean isVisible) override;

        IFACEMETHODIMP get_ClearColor(Color* value) override;
        IFACEMETHODIMP put_ClearColor(Color value) override;

        IFACEMETHODIMP add_Update(
            Animated_UpdateEventHandler* value,
            EventRegistrationToken *token) override;

        IFACEMETHODIMP remove_Update(
            EventRegistrationToken token) override;

        IFACEMETHODIMP add_GameLoopStarting(
            ITypedEventHandler<ICanvasAnimatedControl*, IInspectable*>* value,
            EventRegistrationToken* token) override;

        IFACEMETHODIMP remove_GameLoopStarting(
            EventRegistrationToken token) override;

        IFACEMETHODIMP add_GameLoopStopped(
            ITypedEventHandler<ICanvasAnimatedControl*, IInspectable*>* value,
            EventRegistrationToken* token) override;

        IFACEMETHODIMP remove_GameLoopStopped(
            EventRegistrationToken token) override;

        IFACEMETHODIMP put_IsFixedTimeStep(boolean value) override;

        IFACEMETHODIMP get_IsFixedTimeStep(boolean* value) override;

        IFACEMETHODIMP put_TargetElapsedTime(TimeSpan value) override;

        IFACEMETHODIMP get_TargetElapsedTime(TimeSpan* value) override;

        IFACEMETHODIMP put_Paused(boolean value) override;

        IFACEMETHODIMP get_Paused(boolean* value) override;
        
        IFACEMETHODIMP get_Size(Size* value) override;

        IFACEMETHODIMP Invalidate() override;
        
        IFACEMETHODIMP ResetElapsedTime() override;

        IFACEMETHODIMP CreateCoreIndependentInputSource(
            CoreInputDeviceTypes deviceType,
            ICoreInputSourceBase** returnValue) override;

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

        virtual void Changed(ChangeReason reason) override final;
        virtual void Loaded() override final;
        virtual void Unloaded() override final;
        virtual void ApplicationSuspending(ISuspendingEventArgs* args) override final;
        virtual void ApplicationResuming() override final;
        virtual void WindowVisibilityChanged() override final;

    private:
        void CreateContentControl();

        // ICanvasGameLoopClient methods

        virtual void OnGameLoopStarting() override;

        virtual void OnGameLoopStopped() override;

        virtual bool Tick(
            CanvasSwapChain* target, 
            bool areResourcesCreated) override;

        virtual void OnTickLoopEnded() override;

        struct UpdateResult
        {
            bool Updated;
            bool IsRunningSlowly;
        };

        UpdateResult Update(bool forceUpdate, int64_t timeSpentPaused);

        void ChangedImpl();

        CanvasTimingInformation GetTimingInformationFromTimer();

        void IssueAsyncActions(
            std::vector<ComPtr<AnimatedControlAsyncAction>> const& pendingActions);

        void CancelAsyncActions();
    };

}}}}}}
