// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#include "BaseControl.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace UI { namespace Xaml
{

    class ICanvasVirtualControlAdapter;
    class CanvasVirtualControl;

    struct CanvasVirtualControlTraits
    {
        typedef ICanvasVirtualControlAdapter adapter_t;
        typedef ITypedEventHandler<CanvasVirtualControl*, CanvasCreateResourcesEventArgs*> createResourcesEventHandler_t;
        typedef CanvasVirtualControl control_t;
        typedef ICanvasVirtualControl controlInterface_t;
        typedef ICanvasVirtualImageSource renderTarget_t;
    };

    
    class ICanvasVirtualControlAdapter : public IBaseControlAdapter<CanvasVirtualControlTraits>
                                       , public virtual IImageControlMixInAdapter
    {
    public:
        virtual ComPtr<ICanvasVirtualImageSource> CreateCanvasVirtualImageSource(
            ICanvasDevice* device,
            float width,
            float height,
            float dpi,
            CanvasAlphaMode alphaMode) = 0;
    };


    typedef ITypedEventHandler<CanvasVirtualControl*, CanvasRegionsInvalidatedEventArgs*> ControlRegionsInvalidatedHandler;

    
    class CanvasVirtualControl
        : public RuntimeClass<
            RuntimeClassFlags<WinRtClassicComMix>,
            MixIn<CanvasVirtualControl, BaseControl<CanvasVirtualControlTraits>>,
            MixIn<CanvasVirtualControl, ImageControlMixIn>,
            ComposableBase<>>
        , public BaseControl<CanvasVirtualControlTraits>
        , public ImageControlMixIn
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_UI_Xaml_CanvasVirtualControl, BaseTrust);

        RegisteredEvent m_regionsInvalidatedEventRegistration;
        EventSource<ControlRegionsInvalidatedHandler, InvokeModeOptions<StopOnFirstError>> m_regionsInvalidatedEventSource;

        ComPtr<ICanvasVirtualImageSource> m_lastImageSourceThatHasBeenDrawn;
        
    public:
        CanvasVirtualControl(std::shared_ptr<ICanvasVirtualControlAdapter> adapter);

        //
        // ICanvasVirtualControl
        //

        IFACEMETHODIMP add_RegionsInvalidated(ITypedEventHandler<CanvasVirtualControl*, CanvasRegionsInvalidatedEventArgs*>*, EventRegistrationToken*) override;
        IFACEMETHODIMP remove_RegionsInvalidated(EventRegistrationToken) override;
        IFACEMETHODIMP CreateDrawingSession(Rect, ICanvasDrawingSession**) override;
        IFACEMETHODIMP SuspendDrawingSession(ICanvasDrawingSession*) override;
        IFACEMETHODIMP ResumeDrawingSession(ICanvasDrawingSession*) override;
        IFACEMETHODIMP Invalidate() override;
        IFACEMETHODIMP InvalidateRegion(Rect) override;

        //
        // BaseControl
        //

        virtual void CreateOrUpdateRenderTarget(
            ICanvasDevice* device,
            CanvasAlphaMode newAlphaMode,
            float newDpi,
            Size newSize,
            RenderTarget* renderTarget) override final;

        virtual void Changed(ChangeReason reason) override final;
        virtual void Loaded() override final;
        virtual void Unloaded() override final;
        virtual void ApplicationSuspending(ISuspendingEventArgs* args) override final;
        virtual void ApplicationResuming() override final;
        virtual void WindowVisibilityChanged() override final;

    private:
        void ChangedImpl(ChangeReason reason);

        HRESULT OnRegionsInvalidated(ICanvasVirtualImageSource* sender, ICanvasRegionsInvalidatedEventArgs* args);
        void OnRegionsInvalidatedImpl(ICanvasVirtualImageSource* sender, ICanvasRegionsInvalidatedEventArgs* args);

        static void ClearAllRegions(
            ICanvasVirtualImageSource* imageSource,
            Color const& clearColor,
            ICanvasRegionsInvalidatedEventArgs* args);
};
    
}}}}}}
