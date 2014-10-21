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
    using namespace ABI::Windows::UI::Xaml;
    using namespace ABI::Windows::UI::Xaml::Media;
    using namespace ABI::Windows::Graphics::Display;
    using namespace ABI::Windows::ApplicationModel;

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
        ComPtr<IDesignModeStatics> m_designModeStatics;

    public:
        CanvasControlAdapter()
        {
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
                HStringReference(RuntimeClass_Windows_ApplicationModel_DesignMode).Get(),
                &m_designModeStatics));
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

        virtual EventRegistrationToken AddCompositionRenderingCallback(IEventHandler<IInspectable*>* handler) override 
        {
            EventRegistrationToken token;
            ThrowIfFailed(m_compositionTargetStatics->add_Rendering(handler, &token));
            return token;
        }

        virtual void RemoveCompositionRenderingCallback(EventRegistrationToken token) override 
        {
            ThrowIfFailed(m_compositionTargetStatics->remove_Rendering(token));
        }

        virtual ComPtr<ICanvasImageSource> CreateCanvasImageSource(ICanvasDevice* device, int width, int height) override 
        {
            ComPtr<ICanvasResourceCreator> resourceCreator;
            ThrowIfFailed(device->QueryInterface(resourceCreator.GetAddressOf()));

            ComPtr<ICanvasImageSource> imageSource;
            ThrowIfFailed(m_canvasImageSourceFactory->Create(
                resourceCreator.Get(),
                width, 
                height, 
                &imageSource));

            return imageSource;
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

        virtual void AddDpiChangedCallback(ITypedEventHandler<DisplayInformation*, IInspectable*>* handler) override
        {
            // Don't register for the DPI changed event if we're in design mode
            if (IsDesignModeEnabled())
                return;

            ComPtr<IDisplayInformation> displayInformation;
            ThrowIfFailed(m_displayInformationStatics->GetForCurrentView(&displayInformation));

            EventRegistrationToken token;
            ThrowIfFailed(displayInformation->add_DpiChanged(handler, &token));
        }

    private:
        bool IsDesignModeEnabled()
        {
            boolean enabled;
            ThrowIfFailed(m_designModeStatics->get_DesignModeEnabled(&enabled));
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


    CanvasControl::CanvasControl(std::shared_ptr<ICanvasControlAdapter> adapter)
        : m_adapter(adapter)
        , m_canvasDevice(m_adapter->CreateCanvasDevice())
        , m_drawNeeded(false)
        , m_isLoaded(false)
        , m_currentWidth(0)
        , m_currentHeight(0)
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

        ComPtr<IFrameworkElement> thisAsFrameworkElement;
        ThrowIfFailed(GetComposableBase().As(&thisAsFrameworkElement));

        // Register for Loaded event
        auto onLoadedFn = Callback<IRoutedEventHandler>(this, &CanvasControl::OnLoaded);
        EventRegistrationToken loadedToken{};

        ThrowIfFailed(thisAsFrameworkElement->add_Loaded(
            onLoadedFn.Get(),
            &loadedToken));

        // TODO #2189 Investigate a potential leak, with events that are registered but not unregistered.

        //  Register for SizeChanged event
        auto onSizeChangedFn = Callback<ISizeChangedEventHandler>(this, &CanvasControl::OnSizeChanged);
        EventRegistrationToken sizeChangedToken{};

        ThrowIfFailed(thisAsFrameworkElement->add_SizeChanged(
            onSizeChangedFn.Get(),
            &sizeChangedToken));

        // Register for DpiChanged event.
        auto dpiChangedEventHandler = Callback<ITypedEventHandler<DisplayInformation*, IInspectable*>, CanvasControl>(this, &CanvasControl::OnDpiChangedCallback);
        m_adapter->AddDpiChangedCallback(dpiChangedEventHandler.Get());
    }

    void CanvasControl::ClearDrawNeeded()
    {
        //
        // Note that this lock is scoped to release before the m_drawEventList.FireAll().
        // This is necessary, since handlers themselves may call Invalidate().
        //
        std::unique_lock<std::mutex> lock(m_drawLock);
        m_drawNeeded = false;

        // TODO #1953 Consider clearing m_renderingEventToken, or coalescing m_renderingEventToken
        // and m_drawNeeded into one variable.
        m_adapter->RemoveCompositionRenderingCallback(m_renderingEventToken);
    }

    void CanvasControl::EnsureSizeDependentResources()
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
                height);
            
            m_currentWidth = width;
            m_currentHeight = height;
            
            //
            // Set this new image source on the image control
            //
            ComPtr<IImageSource> baseImageSource;
            ThrowIfFailed(m_canvasImageSource.As(&baseImageSource));
            ThrowIfFailed(m_imageControl->put_Source(baseImageSource.Get()));
        }
    }

    void CanvasControl::CallDrawHandlers()
    {
        if (m_drawEventList.GetSize() == 0)
        {
            return;
        }

        if (!m_canvasImageSource)
        {
            return;
        }

        ComPtr<ICanvasDrawingSession> drawingSession;
        ComPtr<CanvasImageSource> imageSourceImplementation = 
            static_cast<CanvasImageSource*>(m_canvasImageSource.Get());

        ThrowIfFailed(imageSourceImplementation->CreateDrawingSessionWithDpi(m_adapter->GetLogicalDpi(), &drawingSession));
        ComPtr<CanvasDrawEventArgs> drawEventArgs = Make<CanvasDrawEventArgs>(drawingSession.Get());
        CheckMakeResult(drawEventArgs);

        ThrowIfFailed(m_drawEventList.InvokeAll(this, drawEventArgs.Get()));

        ComPtr<IClosable> drawingSessionClosable;
        ThrowIfFailed(drawingSession.As(&drawingSessionClosable));
        ThrowIfFailed(drawingSessionClosable->Close()); // Device removal should be handled here.
    }

    HRESULT CanvasControl::OnLoaded(IInspectable* sender, IRoutedEventArgs* args)
    {
        return ExceptionBoundary(
            [&]
            {
                m_isLoaded = true;

                // The scheme for the CreateResources event is:
                // - On Loaded, all handlers are fired.
                // - If any handler is added thereafter, it is fired as soon as it is added.
                //
                // And so, there isn't a need to keep track of which handlers have been fired and which have not.
                //

                ThrowIfFailed(m_createResourcesEventList.InvokeAll(this, static_cast<IInspectable*>(nullptr)));

                InvalidateImpl();
            });
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

                InvalidateImpl();
            });
    }

    IFACEMETHODIMP CanvasControl::add_CreateResources(
        ABI::Windows::Foundation::ITypedEventHandler<ABI::Microsoft::Graphics::Canvas::CanvasControl*, IInspectable*>* value,
        EventRegistrationToken *token)
    {
        return ExceptionBoundary(
            [&]
            {
                ThrowIfFailed(m_createResourcesEventList.Add(value, token));

                if (m_isLoaded)
                {
                    // TODO #1922 Ensure that this operation is threadsafe.
                    ThrowIfFailed(value->Invoke(this, nullptr));
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

    HRESULT CanvasControl::OnRenderCallback(IInspectable *pSender, IInspectable *pArgs)
    {
        return ExceptionBoundary(
            [&]
            {
                ClearDrawNeeded();

                EnsureSizeDependentResources();

                CallDrawHandlers();
            });
    }

    IFACEMETHODIMP CanvasControl::Invalidate()
    {
        return ExceptionBoundary(
            [&]
            {
                InvalidateImpl();
            });
    }


    void CanvasControl::InvalidateImpl()
    {
        std::lock_guard<std::mutex> lock(m_drawLock);

        if (m_drawNeeded)
            return;

        m_drawNeeded = true;
        ComPtr<IEventHandler<IInspectable*>> renderingEventHandler = Callback<IEventHandler<IInspectable*>, CanvasControl>(this, &CanvasControl::OnRenderCallback);
        m_renderingEventToken = m_adapter->AddCompositionRenderingCallback(renderingEventHandler.Get());
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
    
    HRESULT CanvasControl::OnDpiChangedCallback(IDisplayInformation* sender, IInspectable* args)
    {
        return ExceptionBoundary(
            [&]
            {
                InvalidateImpl();
            });
    }

    ActivatableClassWithFactory(CanvasDrawEventArgs, CanvasDrawEventArgsFactory);
    ActivatableClassWithFactory(CanvasControl, CanvasControlFactory);
}}}}
