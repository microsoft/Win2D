// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#include "RecreatableDeviceManager.impl.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace UI { namespace Xaml
{
    using namespace ABI::Windows::ApplicationModel::Core;
    using namespace ABI::Microsoft::UI::Xaml::Controls;
    using namespace ABI::Microsoft::UI::Xaml::Media;
    using namespace ABI::Microsoft::UI::Xaml;

    template<typename TRAITS>
    class BaseControlAdapter : public TRAITS::adapter_t,
                               private LifespanTracker<typename TRAITS::adapter_t>
    {
        ComPtr<IActivationFactory> m_canvasDeviceFactory;
        ComPtr<IUserControlFactory> m_userControlFactory;
        ComPtr<IDisplayInformationStatics> m_displayInformationStatics;
        ComPtr<IWindowStatics> m_windowStatics;
        ComPtr<ICoreApplication> m_coreApplication;

        bool m_isDesignModeEnabled;

    public:

        BaseControlAdapter()
            : m_isDesignModeEnabled(QueryIsDesignModeEnabled())
        {
            ComPtr<IDesignModeStatics> designModeStatics;
            ThrowIfFailed(GetActivationFactory(
                HStringReference(RuntimeClass_Windows_ApplicationModel_DesignMode).Get(),
                &designModeStatics));

            auto& module = Module<InProc>::GetModule();

            ThrowIfFailed(GetActivationFactory(
                HStringReference(RuntimeClass_Microsoft_UI_Xaml_Controls_UserControl).Get(),
                &m_userControlFactory));

            ThrowIfFailed(module.GetActivationFactory(
                HStringReference(RuntimeClass_Microsoft_Graphics_Canvas_CanvasDevice).Get(),
                &m_canvasDeviceFactory));

            ThrowIfFailed(GetActivationFactory(
                HStringReference(RuntimeClass_Windows_Graphics_Display_DisplayInformation).Get(),
                &m_displayInformationStatics));

            ThrowIfFailed(GetActivationFactory(
                HStringReference(RuntimeClass_Microsoft_UI_Xaml_Window).Get(),
                &m_windowStatics));

            ThrowIfFailed(GetActivationFactory(
                HStringReference(RuntimeClass_Windows_ApplicationModel_Core_CoreApplication).Get(),
                &m_coreApplication));
        }

        virtual ~BaseControlAdapter() = default;

        virtual bool IsDesignModeEnabled() override
        {
            return m_isDesignModeEnabled;
        }

        virtual ComPtr<IInspectable> CreateUserControl(IInspectable* canvasControl) override
        {
            ComPtr<IInspectable> userControlInspectable;
            ComPtr<IUserControl> userControl;

            ThrowIfFailed(m_userControlFactory->CreateInstance(
                canvasControl,
                &userControlInspectable,
                &userControl));

            return userControlInspectable;
        }

        virtual RegisteredEvent AddApplicationSuspendingCallback(IEventHandler<SuspendingEventArgs*>* handler) override
        {
            return RegisteredEvent(
                m_coreApplication.Get(),
                &ICoreApplication::add_Suspending,
                &ICoreApplication::remove_Suspending,
                handler);
        }

        virtual RegisteredEvent AddApplicationResumingCallback(IEventHandler<IInspectable*>* handler) override
        {
            return RegisteredEvent(
                m_coreApplication.Get(),
                &ICoreApplication::add_Resuming,
                &ICoreApplication::remove_Resuming,
                handler);
        }

#ifndef WINUI3
        virtual float GetLogicalDpi() override
        {
            // Don't try to look up display information if we're in design mode
            if (IsDesignModeEnabled())
                return DEFAULT_DPI;

            ComPtr<IDisplayInformation> displayInformation;
            ThrowIfFailed(m_displayInformationStatics->GetForCurrentView(&displayInformation));

            FLOAT logicalDpi;
            ThrowIfFailed(displayInformation->get_LogicalDpi(&logicalDpi));
            return logicalDpi;
        }

        virtual RegisteredEvent AddDpiChangedCallback(DpiChangedEventHandler* handler) override
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
#endif
        
        virtual RegisteredEvent AddVisibilityChangedCallback(WindowVisibilityChangedEventHandler* handler, IWindow* window)
        {
            // Don't register for the visiblity changed event if we're in design
            // mode.  Although we have been given a valid IWindow, we'll crash
            // if we attempt to call add_VisibilityChanged on it!
            if (IsDesignModeEnabled())
                return RegisteredEvent();

            return RegisteredEvent(
                window,
                &IWindow::add_VisibilityChanged,
                &IWindow::remove_VisibilityChanged,
                handler);
        }

        virtual RegisteredEvent AddXamlRootChangedCallback(XamlRootChangedEventHandler* handler, IXamlRoot* xamlRoot) override
        {
            // Don't register for the XamlRoot changed event if we're in design
            // mode.
            if (IsDesignModeEnabled())
                return RegisteredEvent();

            return RegisteredEvent(
                xamlRoot,
                &IXamlRoot::add_Changed,
                &IXamlRoot::remove_Changed,
                handler);
        }

        virtual ComPtr<IWindow> GetWindowOfCurrentThread() override
        {
            ComPtr<IWindow> currentWindow;
            ThrowIfFailed(m_windowStatics->get_Current(&currentWindow));
            return currentWindow;
        }

        virtual std::unique_ptr<IRecreatableDeviceManager<TRAITS>> CreateRecreatableDeviceManager(IInspectable* parentControl) override
        {
            return std::make_unique<RecreatableDeviceManager<TRAITS>>(m_canvasDeviceFactory.Get(), parentControl);
        }

    private:
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

}}}}}}
