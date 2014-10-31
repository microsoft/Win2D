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

#include "pch.h"

#include "CanvasControl.h"
#include "CanvasDevice.h"
#include "CanvasImageSource.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas
{
    using namespace ::Microsoft::WRL::Wrappers;
    using namespace ABI::Windows::ApplicationModel::Core;
    using namespace ABI::Windows::ApplicationModel;
    using namespace ABI::Windows::Graphics::Display;
    using namespace ABI::Windows::UI::Xaml::Media;
    using namespace ABI::Windows::UI::Xaml;

    IFACEMETHODIMP CanvasDrawEventArgsFactory::Create(
        ICanvasDrawingSession* drawingSession,
        ICanvasDrawEventArgs** drawEventArgs)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckAndClearOutPointer(drawEventArgs);

                auto newCanvasDrawEventArgs = Make<CanvasDrawEventArgs>(drawingSession);
                CheckMakeResult(newCanvasDrawEventArgs);

                ThrowIfFailed(newCanvasDrawEventArgs.CopyTo(drawEventArgs));
            });
    }

    CanvasDrawEventArgs::CanvasDrawEventArgs(ICanvasDrawingSession* drawingSession) 
        : m_drawingSession(drawingSession)
    {}

    IFACEMETHODIMP CanvasDrawEventArgs::get_DrawingSession(ICanvasDrawingSession** value)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckAndClearOutPointer(value);
                ComPtr<ICanvasDrawingSession> drawingSession = m_drawingSession.EnsureNotClosed();
                ThrowIfFailed(drawingSession.CopyTo(value));
            });
    }


    class CanvasControlAdapter : public ICanvasControlAdapter
    {
        ComPtr<IUserControlFactory> m_userControlFactory;
        ComPtr<IActivationFactory> m_canvasDeviceFactory;
        ComPtr<ICompositionTargetStatics> m_compositionTargetStatics;
        ComPtr<ICanvasImageSourceFactory> m_canvasImageSourceFactory;
        ComPtr<IActivationFactory> m_imageControlFactory;
        ComPtr<IDisplayInformationStatics> m_displayInformationStatics;
        ComPtr<IWindowStatics> m_windowStatics;
        ComPtr<ICoreApplication> m_coreApplication;

        bool m_isDesignModeEnabled;

    public:
        CanvasControlAdapter()
            : m_isDesignModeEnabled(QueryIsDesignModeEnabled())
        {
            ComPtr<IDesignModeStatics> designModeStatics;
            ThrowIfFailed(GetActivationFactory(
                HStringReference(RuntimeClass_Windows_ApplicationModel_DesignMode).Get(),
                &designModeStatics));

            auto& module = Module<InProc>::GetModule();

            ThrowIfFailed(GetActivationFactory(
                HStringReference(RuntimeClass_Windows_UI_Xaml_Controls_UserControl).Get(),
                &m_userControlFactory));

            ThrowIfFailed(module.GetActivationFactory(
                HStringReference(RuntimeClass_Microsoft_Graphics_Canvas_CanvasDevice).Get(),
                &m_canvasDeviceFactory));


            ThrowIfFailed(GetActivationFactory(
                HStringReference(RuntimeClass_Windows_UI_Xaml_Media_CompositionTarget).Get(),
                &m_compositionTargetStatics));

            ComPtr<IActivationFactory> imageSourceActivationFactory;
            ThrowIfFailed(module.GetActivationFactory(
                HStringReference(RuntimeClass_Microsoft_Graphics_Canvas_CanvasImageSource).Get(),
                &imageSourceActivationFactory));
            ThrowIfFailed(imageSourceActivationFactory.As(&m_canvasImageSourceFactory));

            ThrowIfFailed(GetActivationFactory(
                HStringReference(RuntimeClass_Windows_UI_Xaml_Controls_Image).Get(),
                &m_imageControlFactory));

            ThrowIfFailed(GetActivationFactory(
                HStringReference(RuntimeClass_Windows_Graphics_Display_DisplayInformation).Get(), 
                &m_displayInformationStatics));

            ThrowIfFailed(GetActivationFactory(
                HStringReference(RuntimeClass_Windows_UI_Xaml_Window).Get(),
                &m_windowStatics));

            ThrowIfFailed(GetActivationFactory(
                HStringReference(RuntimeClass_Windows_ApplicationModel_Core_CoreApplication).Get(),
                &m_coreApplication));
        }

        virtual std::pair<ComPtr<IInspectable>, ComPtr<IUserControl>> CreateUserControl(IInspectable* canvasControl) override 
        {
            ComPtr<IInspectable> userControlInspectable;
            ComPtr<IUserControl> userControl;

            ThrowIfFailed(m_userControlFactory->CreateInstance(
                canvasControl, 
                &userControlInspectable, 
                &userControl));

            return std::make_pair(userControlInspectable, userControl);
        }

        virtual ComPtr<ICanvasDevice> CreateCanvasDevice() override 
        {
            ComPtr<IInspectable> inspectableDevice;
            ThrowIfFailed(m_canvasDeviceFactory->ActivateInstance(&inspectableDevice));

            ComPtr<ICanvasDevice> device;
            ThrowIfFailed(inspectableDevice.As(&device));

            return device;
        }

        virtual RegisteredEvent AddApplicationSuspendingCallback(IEventHandler<SuspendingEventArgs*>* handler) override
        {
            return RegisteredEvent(
                m_coreApplication.Get(),
                &ICoreApplication::add_Suspending,
                &ICoreApplication::remove_Suspending,
                handler);
        }

        virtual RegisteredEvent AddCompositionRenderingCallback(IEventHandler<IInspectable*>* handler) override 
        {
            return RegisteredEvent(
                m_compositionTargetStatics.Get(),
                &ICompositionTargetStatics::add_Rendering,
                &ICompositionTargetStatics::remove_Rendering,
                handler);
        }

        virtual RegisteredEvent AddSurfaceContentsLostCallback(IEventHandler<IInspectable*>* handler) override
        {
            return RegisteredEvent(
                m_compositionTargetStatics.Get(),
                &ICompositionTargetStatics::add_SurfaceContentsLost,
                &ICompositionTargetStatics::remove_SurfaceContentsLost,
                handler);
        }

        virtual RegisteredEvent AddVisibilityChangedCallback(IWindowVisibilityChangedEventHandler* handler, IWindow* window) override
        {
            // Don't register for the visiblity changed event if we're in design
            // mode.  Although we have been given a value IWindow, we'll crash
            // if we attempt to call add_VisibilityChanged on it!
            if (IsDesignModeEnabled())
                return RegisteredEvent();

            return RegisteredEvent(
                window,
                &IWindow::add_VisibilityChanged,
                &IWindow::remove_VisibilityChanged,
                handler);
        }

        virtual ComPtr<CanvasImageSource> CreateCanvasImageSource(ICanvasDevice* device, int width, int height, CanvasBackground backgroundMode) override 
        {
            ComPtr<ICanvasResourceCreator> resourceCreator;
            ThrowIfFailed(device->QueryInterface(resourceCreator.GetAddressOf()));

            ComPtr<ICanvasImageSource> imageSource;
            ThrowIfFailed(m_canvasImageSourceFactory->CreateWithBackground(
                resourceCreator.Get(),
                width, 
                height,
                backgroundMode,
                &imageSource));

            // Since we know that CanvasImageSourceFactory will only ever return
            // CanvasImageSource instances, we can be certain that the
            // ICanvasImageSource we get back is actually a CanvasImageSource.
            return static_cast<CanvasImageSource*>(imageSource.Get());
        }

        virtual ComPtr<IImage> CreateImageControl() override 
        {
            ComPtr<IInspectable> inspectableImage;
            ThrowIfFailed(m_imageControlFactory->ActivateInstance(&inspectableImage));

            ComPtr<IImage> image;
            ThrowIfFailed(inspectableImage.As(&image));

            return image;
        }

        float GetLogicalDpi() override
        {
            ComPtr<IDisplayInformation> displayInformation;
            ThrowIfFailed(m_displayInformationStatics->GetForCurrentView(&displayInformation));

            FLOAT logicalDpi;
            ThrowIfFailed(displayInformation->get_LogicalDpi(&logicalDpi));
            return logicalDpi;
        }

        virtual RegisteredEvent AddDpiChangedCallback(ITypedEventHandler<DisplayInformation*, IInspectable*>* handler) override
        {
            // Don't register for the DPI changed event if we're in design mode
            if (IsDesignModeEnabled())
                return RegisteredEvent();

            ComPtr<IDisplayInformation> displayInformation;
            ThrowIfFailed(m_displayInformationStatics->GetForCurrentView(&displayInformation));

            return RegisteredEvent(
                displayInformation.Get(), 
                &IDisplayInformation::add_DpiChanged, 
                &IDisplayInformation::remove_DpiChanged,
                handler);
        }

        virtual ComPtr<IWindow> GetCurrentWindow() override
        {
            ComPtr<IWindow> currentWindow;
            ThrowIfFailed(m_windowStatics->get_Current(&currentWindow));
            return currentWindow;
        }

    private:
        bool IsDesignModeEnabled() const
        {
            return m_isDesignModeEnabled;
        }

        static bool QueryIsDesignModeEnabled()
        {
            ComPtr<IDesignModeStatics> designModeStatics;
            ThrowIfFailed(GetActivationFactory(
                HStringReference(RuntimeClass_Windows_ApplicationModel_DesignMode).Get(),
                &designModeStatics));

            boolean enabled;
            ThrowIfFailed(designModeStatics->get_DesignModeEnabled(&enabled));
            return !!enabled;
        }
    };


    class CanvasControlFactory : public ActivationFactory<>
    {
        std::shared_ptr<CanvasControlAdapter> m_adapter;

    public:
        CanvasControlFactory()
            : m_adapter(std::make_shared<CanvasControlAdapter>())
        {
        }

        IFACEMETHODIMP ActivateInstance(IInspectable** obj) override
        {
            return ExceptionBoundary(
                [&]
                {
                    CheckAndClearOutPointer(obj);

                    auto control = Make<CanvasControl>(m_adapter);
                    CheckMakeResult(control);

                    ThrowIfFailed(control.CopyTo(obj));
                });
        }
    };

    CanvasControl::GuardedState::GuardedState()
        : m_clearColor{}
        , m_imageSourceNeedsReset{}
        , m_needToHookCompositionRendering{}
    {
    }


    void CanvasControl::GuardedState::TriggerRender(CanvasControl* control, InvalidateReason reason)
    {
        std::unique_lock<std::mutex> lock(m_lock);
        TriggerRenderImpl(control, reason);
    }


    void CanvasControl::GuardedState::SetClearColor(CanvasControl* control, Color const& value)
    {
        std::unique_lock<std::mutex> lock(m_lock);

        if (m_clearColor.A == value.A &&
            m_clearColor.R == value.R && 
            m_clearColor.G == value.G &&
            m_clearColor.B == value.B)
        {
            return;
        }
        
        bool wasOpaque = (m_clearColor.A == 255);
        bool isOpaque = (value.A == 255);
        
        auto invalidateReason = InvalidateReason::Default;
        if (wasOpaque != isOpaque)
            invalidateReason = InvalidateReason::ImageSourceNeedsReset;
        
        m_clearColor = value;
        TriggerRenderImpl(control, invalidateReason);
    }


    void CanvasControl::GuardedState::OnWindowVisibilityChanged(CanvasControl* control, bool isVisible)
    {
        std::unique_lock<std::mutex> lock(m_lock);

        if (isVisible)
        {
            HookCompositionRenderingIfNecessary(control);
        }
        else
        {                    
            if (m_renderingEventRegistration)
            {
                //
                // The window is invisible.  This means that
                // OnCompositionRendering() will not do anything.  However, XAML
                // will run composition if any handlers are registered on the
                // rendering event.  Since we want to avoid the system doing
                // unnecessary work we take steps to unhook the event when we're
                // not making good use of it.
                //
                m_renderingEventRegistration.Release();
                m_needToHookCompositionRendering = true;
            }
        }
    }


    Color CanvasControl::GuardedState::GetClearColor()
    {
        std::unique_lock<std::mutex> lock(m_lock);
        return m_clearColor;
    }


    void CanvasControl::GuardedState::TriggerRenderImpl(CanvasControl* control, InvalidateReason reason)
    {
        if (reason == InvalidateReason::ImageSourceNeedsReset)
            m_imageSourceNeedsReset = true;

        if (m_renderingEventRegistration)
            return;

        m_needToHookCompositionRendering = true;

        if (!control->IsWindowVisible())
            return;

        HookCompositionRenderingIfNecessary(control);
    }


    void CanvasControl::GuardedState::HookCompositionRenderingIfNecessary(CanvasControl* control)
    {
        if (m_renderingEventRegistration)
            return;
        
        if (!m_needToHookCompositionRendering)
            return;
        
        m_renderingEventRegistration = control->m_adapter->AddCompositionRenderingCallback(
            control, 
            &CanvasControl::OnCompositionRendering);
        
        m_needToHookCompositionRendering = false; 
    }
    

    CanvasControl::CanvasControl(std::shared_ptr<ICanvasControlAdapter> adapter)
        : m_adapter(adapter)
        , m_window(m_adapter->GetCurrentWindow())
        , m_canvasDevice(m_adapter->CreateCanvasDevice())
        , m_isLoaded(false)
        , m_currentWidth(0)
        , m_currentHeight(0)
        , m_guardedState(std::make_unique<GuardedState>())
    {
        CreateBaseClass();
        CreateImageControl();
        RegisterEventHandlers();
    }

    void CanvasControl::CreateBaseClass()
    {
        auto base = m_adapter->CreateUserControl(static_cast<ICanvasControl*>(this));
        ThrowIfFailed(SetComposableBasePointers(base.first.Get(), base.second.Get()));
    }

    void CanvasControl::CreateImageControl()
    {
        m_imageControl = m_adapter->CreateImageControl();

        //
        // Set the stretch mode to Fill. This will ensure that on high DPI, the
        // layout will confine the control to the correct area even when the
        // backing image has a different physical size from the control's
        // device-independent size.
        //
        // The logic in EnsureSizeDependentResources ensures that the Source
        // assigned to the Image control matches the CanvasImageSource extents.
        //
        ThrowIfFailed(m_imageControl->put_Stretch(ABI::Windows::UI::Xaml::Media::Stretch_Fill));

        //
        // Set the image control as the content of this control.
        //
        ComPtr<IUIElement> imageAsUIElement;
        ThrowIfFailed(m_imageControl.As(&imageAsUIElement));

        ComPtr<IUserControl> thisAsUserControl;
        ThrowIfFailed(GetComposableBase().As(&thisAsUserControl));
        thisAsUserControl->put_Content(imageAsUIElement.Get());
    }

    void CanvasControl::RegisterEventHandlers()
    {
        using namespace ABI::Windows::UI::Xaml::Controls;
        using namespace Windows::Foundation;

        auto thisAsFrameworkElement = As<IFrameworkElement>(GetComposableBase());

        RegisterEventHandlerOnSelf(thisAsFrameworkElement, &IFrameworkElement::add_Loaded, &CanvasControl::OnLoaded);
        RegisterEventHandlerOnSelf(thisAsFrameworkElement, &IFrameworkElement::add_SizeChanged, &CanvasControl::OnSizeChanged);

        m_applicationSuspendingEventRegistration = m_adapter->AddApplicationSuspendingCallback(this, &CanvasControl::OnApplicationSuspending);
        m_dpiChangedEventRegistration = m_adapter->AddDpiChangedCallback(this, &CanvasControl::OnDpiChanged);
        m_surfaceContentsLostEventRegistration = m_adapter->AddSurfaceContentsLostCallback(this, &CanvasControl::OnSurfaceContentsLost);
        m_windowVisibilityChangedEventRegistration = m_adapter->AddVisibilityChangedCallback(this, &CanvasControl::OnWindowVisibilityChanged, m_window.Get());
    }

    template<typename T, typename DELEGATE, typename HANDLER>
    void CanvasControl::RegisterEventHandlerOnSelf(
        ComPtr<T> const& self,
        HRESULT (STDMETHODCALLTYPE T::* addMethod)(DELEGATE*, EventRegistrationToken*), 
        HANDLER handler)
    {
        // 'self' here is assumed to be something that was obtained by QI'ing
        // ourselves (or our component base).  In this case we don't need to
        // unregister the event on destruction since once we're destroyed these
        // events won't get fired.
        EventRegistrationToken tokenThatIsThrownAway{};

        auto callback = Callback<DELEGATE>(this, handler);
        CheckMakeResult(callback);

        ThrowIfFailed((self.Get()->*addMethod)(callback.Get(), &tokenThatIsThrownAway));
    }

    CanvasControl::~CanvasControl()
    {
    }

    Color CanvasControl::GuardedState::PreDrawAndGetClearColor(CanvasControl* control)
    {
        std::unique_lock<std::mutex> lock(m_lock);

        if (m_imageSourceNeedsReset)
        {
            control->m_canvasImageSource.Reset();
            m_imageSourceNeedsReset = false;
        }

        m_renderingEventRegistration.Release();

        return m_clearColor;
    }

    void CanvasControl::EnsureSizeDependentResources(CanvasBackground backgroundMode)
    {
        // CanvasControl should always have a device
        assert(m_canvasDevice);

        // It is illegal to call get_actualWidth/Height before Loaded.
        assert(m_isLoaded);

        ComPtr<IFrameworkElement> thisAsFrameworkElement;
        ThrowIfFailed(GetComposableBase().As(&thisAsFrameworkElement));

        double actualWidth, actualHeight;
        ThrowIfFailed(thisAsFrameworkElement->get_ActualWidth(&actualWidth));
        ThrowIfFailed(thisAsFrameworkElement->get_ActualHeight(&actualHeight));

        assert(actualWidth <= static_cast<double>(INT_MAX));
        assert(actualHeight <= static_cast<double>(INT_MAX));
        
        const float logicalDpi = m_adapter->GetLogicalDpi();
        const double dpiScalingFactor = logicalDpi / DEFAULT_DPI;

        const double deviceDependentWidth = actualWidth * dpiScalingFactor;
        const double deviceDependentHeight = actualHeight * dpiScalingFactor;

        assert(deviceDependentWidth <= static_cast<double>(INT_MAX));
        assert(deviceDependentWidth >= 0.0);
        assert(deviceDependentHeight <= static_cast<double>(INT_MAX));
        assert(deviceDependentHeight >= 0.0);

        auto width = static_cast<int>(ceil(deviceDependentWidth));
        auto height = static_cast<int>(ceil(deviceDependentHeight));

        if (m_canvasImageSource)
        {
            // If we already have an image source that's the right size we don't
            // need to do anything.
            if (width == m_currentWidth && height == m_currentHeight)
                return;
        }

        if (width <= 0 || height <= 0)
        {
            // Zero-sized controls don't have image sources
            m_canvasImageSource.Reset();
            m_currentWidth = 0;
            m_currentHeight = 0;
            ThrowIfFailed(m_imageControl->put_Source(nullptr));
        }
        else
        {
            m_canvasImageSource = m_adapter->CreateCanvasImageSource(
                m_canvasDevice.Get(),
                width,
                height,
                backgroundMode);
            
            m_currentWidth = width;
            m_currentHeight = height;
            
            //
            // Set this new image source on the image control
            //
            auto baseImageSource = As<IImageSource>(m_canvasImageSource);
            ThrowIfFailed(m_imageControl->put_Source(baseImageSource.Get()));
        }
    }

    void CanvasControl::CallDrawHandlers(Color const& clearColor)
    {
        if (!m_canvasImageSource)
        {
            return;
        }

        auto drawingSession = m_canvasImageSource->CreateDrawingSessionWithDpi(clearColor, m_adapter->GetLogicalDpi());
        ComPtr<CanvasDrawEventArgs> drawEventArgs = Make<CanvasDrawEventArgs>(drawingSession.Get());
        CheckMakeResult(drawEventArgs);

        ThrowIfFailed(m_drawEventList.InvokeAll(this, drawEventArgs.Get()));

        ComPtr<IClosable> drawingSessionClosable;
        ThrowIfFailed(drawingSession.As(&drawingSessionClosable));
        ThrowIfFailed(drawingSessionClosable->Close());
    }
    
    HRESULT CanvasControl::OnApplicationSuspending(IInspectable* sender, ISuspendingEventArgs* args)
    {
        return ExceptionBoundary(
            [&]
            {
                auto direct3DDevice = As<IDirect3DDevice>(m_canvasDevice);
                ThrowIfFailed(direct3DDevice->Trim());
            });
    }

    HRESULT CanvasControl::OnLoaded(IInspectable* sender, IRoutedEventArgs* args)
    {
        return ExceptionBoundary(
            [&]
            {
                m_isLoaded = true;

                // The scheme for the CreateResources event is:
                // - On Loaded, all handlers are invoked.
                // - If any handler is added thereafter, it is invoked as soon as it is added.
                //
                // And so, there isn't a need to keep track of which handlers
                // have been invoked and which have not.
                //

                try
                {
                    InvokeCreateResources();
                    m_guardedState->TriggerRender(this);
                }
                catch (DeviceLostException const&)
                {
                    HandleDeviceLost();
                }
            });
    }

    void CanvasControl::InvokeCreateResources()
    {
        ThrowIfFailed(m_createResourcesEventList.InvokeAll(this, static_cast<IInspectable*>(nullptr)));
    }

    void CanvasControl::HandleDeviceLost()
    {
        // This function must be called from inside a catch block.
        assert(std::current_exception());

        // Was our device really lost?
        auto d3dDevice = GetDXGIInterface<ID3D11Device>(m_canvasDevice.Get());
        
        if (d3dDevice->GetDeviceRemovedReason() == S_OK)
        {
            // The device lost exception wasn't caused by the device managed
            // by this CanvasControl.  Recreating m_canvasDevice therefore
            // won't resolve this.  So instead we propogate the error.
            throw;
        }
        
        m_canvasDevice.Reset();
        m_guardedState->TriggerRender(this, InvalidateReason::ImageSourceNeedsReset);
    }

    HRESULT CanvasControl::OnSizeChanged(IInspectable* sender, ISizeChangedEventArgs* args)
    {
        return ExceptionBoundary(
            [&]
            {
                //
                // If we get a size changed before we've loaded then we don't do
                // anything.
                //
                if (!m_isLoaded)
                    return;

                //
                // OnSizeChanged can get called multiple times.  We only want to
                // invalidate if it represents a size change from what the
                // control was last set to.
                //
                ABI::Windows::Foundation::Size newSize{};                
                ThrowIfFailed(args->get_NewSize(&newSize));

                auto newWidth = static_cast<int>(newSize.Width);
                auto newHeight = static_cast<int>(newSize.Height);

                if (newWidth == m_currentWidth && newHeight == m_currentHeight)
                    return;

                m_guardedState->TriggerRender(this);
            });
    }

    IFACEMETHODIMP CanvasControl::add_CreateResources(
        ITypedEventHandler<CanvasControl*, IInspectable*>* value,
        EventRegistrationToken *token)
    {
        return ExceptionBoundary(
            [&]
            {
                // TODO #1922 Ensure that this operation is threadsafe.  (Check
                // m_window's CoreDispatcher::HasThreadAccess)

                ThrowIfFailed(m_createResourcesEventList.Add(value, token));

                // CreateResources will never be invoked until the control is
                // Loaded.
                if (!m_isLoaded)
                    return;

                try
                {
                    ThrowIfFailed(value->Invoke(this, nullptr));
                }
                catch (DeviceLostException const&)
                {
                    HandleDeviceLost();
                }
            });
    }
    
    IFACEMETHODIMP CanvasControl::remove_CreateResources(
        EventRegistrationToken token)
    {
        return ExceptionBoundary(
            [&]
            {
                ThrowIfFailed(m_createResourcesEventList.Remove(token));
            });
    }

    IFACEMETHODIMP CanvasControl::add_Draw(
        ABI::Windows::Foundation::ITypedEventHandler<ABI::Microsoft::Graphics::Canvas::CanvasControl*, ABI::Microsoft::Graphics::Canvas::CanvasDrawEventArgs*>* value,
        EventRegistrationToken* token)
    {
        return ExceptionBoundary(
            [&]
            {
                ThrowIfFailed(m_drawEventList.Add(value, token));
            });
    }

    IFACEMETHODIMP CanvasControl::remove_Draw(
        EventRegistrationToken token)
    {
        return ExceptionBoundary(
            [&]
            {
                ThrowIfFailed(m_drawEventList.Remove(token));
            });
    }

    IFACEMETHODIMP CanvasControl::put_ClearColor(Color value)
    {
        return ExceptionBoundary(
            [&]
            {
                m_guardedState->SetClearColor(this, value);
            });
    }

    IFACEMETHODIMP CanvasControl::get_ClearColor(Color* value)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(value);
                *value = m_guardedState->GetClearColor();
            });
    }

    HRESULT CanvasControl::OnCompositionRendering(IInspectable*, IInspectable*)
    {
        return ExceptionBoundary(
            [&]
            {
                if (!IsWindowVisible())
                    return;

                try
                {
                    if (!m_canvasDevice)
                    {
                        m_canvasDevice = m_adapter->CreateCanvasDevice();
                        InvokeCreateResources();
                    }
                    
                    auto clearColor = m_guardedState->PreDrawAndGetClearColor(this);

                    if (m_drawEventList.GetSize() == 0)
                    {
                        return;
                    }
                    
                    auto backgroundMode = clearColor.A == 255 ? CanvasBackground::Opaque : CanvasBackground::Transparent;
                    EnsureSizeDependentResources(backgroundMode);
                    CallDrawHandlers(clearColor);
                }
                catch (DeviceLostException const&)
                {
                    HandleDeviceLost();
                }
            });
    }



    bool CanvasControl::IsWindowVisible()
    {
        boolean visible;

        HRESULT hr = m_window->get_Visible(&visible);

        if (hr == E_FAIL)
        {
            // In design mode get_Visible will return E_FAIL.  In this case we
            // just treat the window as visible.
            return true;
        }
        else
        {
            ThrowIfFailed(hr);
            return !!visible;
        }
    }


    IFACEMETHODIMP CanvasControl::Invalidate()
    {
        return ExceptionBoundary(
            [&]
            {
                m_guardedState->TriggerRender(this);
            });
    }


    IFACEMETHODIMP CanvasControl::MeasureOverride(
        ABI::Windows::Foundation::Size availableSize, 
        ABI::Windows::Foundation::Size* returnValue)
    {
        return ExceptionBoundary(
            [&]
            {
                //
                // MeasureOverride must call Measure on its children (in this
                // case this is just the image control).
                //
                ComPtr<IUIElement> imageOverrides;
                ThrowIfFailed(m_imageControl.As(&imageOverrides));
                ThrowIfFailed(imageOverrides->Measure(availableSize));

                //
                // However, we ignore how they respond and reply that we're
                // happy to be sized however the layout engine wants to size us.
                //
                *returnValue = ABI::Windows::Foundation::Size{};
            });
    }
    
    IFACEMETHODIMP CanvasControl::ArrangeOverride(
        ABI::Windows::Foundation::Size finalSize, 
        ABI::Windows::Foundation::Size* returnValue)
    {
        return ExceptionBoundary(
            [&]
            {
                //
                // Allow base class to handle this
                //
                ComPtr<IFrameworkElementOverrides> base;
                ThrowIfFailed(GetComposableBase().As(&base));
                ThrowIfFailed(base->ArrangeOverride(finalSize, returnValue));
            });
    }
    
    IFACEMETHODIMP CanvasControl::OnApplyTemplate()
    {
        return ExceptionBoundary(
            [&]
            {
                //
                // Allow base class to handle this
                //
                ComPtr<IFrameworkElementOverrides> base;
                ThrowIfFailed(GetComposableBase().As(&base));
                ThrowIfFailed(base->OnApplyTemplate());
            });
    }

    IFACEMETHODIMP CanvasControl::get_Device(ICanvasDevice** value)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckAndClearOutPointer(value);
                ThrowIfFailed(m_canvasDevice.CopyTo(value));
            });
    }
    
    HRESULT CanvasControl::OnDpiChanged(IDisplayInformation*, IInspectable*)
    {
        return ExceptionBoundary(
            [&]
            {
                m_guardedState->TriggerRender(this);
            });
    }

    HRESULT CanvasControl::OnSurfaceContentsLost(IInspectable*, IInspectable*)
    {
        return ExceptionBoundary(
            [&]
            {
                m_guardedState->TriggerRender(this, InvalidateReason::ImageSourceNeedsReset);
            });
    }

    HRESULT CanvasControl::OnWindowVisibilityChanged(IInspectable*, IVisibilityChangedEventArgs* args)
    {
        return ExceptionBoundary(
            [&]
            {
                boolean isVisible;
                ThrowIfFailed(args->get_Visible(&isVisible));

                m_guardedState->OnWindowVisibilityChanged(this, !!isVisible);
            });
    }

    ActivatableClassWithFactory(CanvasDrawEventArgs, CanvasDrawEventArgsFactory);
    ActivatableClassWithFactory(CanvasControl, CanvasControlFactory);
}}}}
