// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

class MockWindow : public RuntimeClass<IWindow>
{
    bool m_visible;

public:
    ComPtr<MockEventSource<IWindowVisibilityChangedEventHandler>> VisibilityChangedEventSource;
    CALL_COUNTER_WITH_MOCK(get_DispatcherMethod, HRESULT(ICoreDispatcher**));

    MockWindow()
        : m_visible(true)
        , VisibilityChangedEventSource(Make<MockEventSource<IWindowVisibilityChangedEventHandler>>(L"VisibilityChanged"))
    {
    }

    void SetVisible(bool visible)
    {
        m_visible = visible;

        class VisibilityChangedEventArgs : public RuntimeClass<ABI::Windows::UI::Core::IVisibilityChangedEventArgs>
        {
            bool m_isVisible;
        public:
            VisibilityChangedEventArgs(bool isVisible)
                : m_isVisible(isVisible)
            {}

            IFACEMETHODIMP get_Visible(boolean* value) override
            {
                *value = m_isVisible;
                return S_OK;
            }
        };

        auto eventArgs = Make<VisibilityChangedEventArgs>(m_visible);
        IInspectable* sender = nullptr;
        ThrowIfFailed(VisibilityChangedEventSource->InvokeAll(sender, eventArgs.Get()));
    }

    IFACEMETHODIMP get_Bounds( 
        ABI::Windows::Foundation::Rect *value)
    {
        Assert::Fail(L"Unexpected call to IWindow::get_Bounds");
        return E_UNEXPECTED;
    }
                        
    IFACEMETHODIMP get_Visible( 
        boolean *value)
    {
        *value = m_visible;
        return S_OK;
    }
                        
    IFACEMETHODIMP get_Content( 
        ABI::Windows::UI::Xaml::IUIElement **value)
    {
        Assert::Fail(L"Unexpected call to IWindow::get_Content");
        return E_UNEXPECTED;
    }
                       
    IFACEMETHODIMP put_Content( 
        ABI::Windows::UI::Xaml::IUIElement *value)
    {
        Assert::Fail(L"Unexpected call to IWindow::put_Content");
        return E_UNEXPECTED;
    }
                        
    IFACEMETHODIMP get_CoreWindow( 
        ABI::Windows::UI::Core::ICoreWindow **value)
    {
        Assert::Fail(L"Unexpected call to IWindow::get_CoreWindow");
        return E_UNEXPECTED;
    }
                        
    IFACEMETHODIMP get_Dispatcher( 
        ABI::Windows::UI::Core::ICoreDispatcher **value)
    {
        return get_DispatcherMethod.WasCalled(value);
    }

    IFACEMETHODIMP add_Activated( 
        ABI::Windows::UI::Xaml::IWindowActivatedEventHandler *value,
        EventRegistrationToken *token)
    {
        Assert::Fail(L"Unexpected call to IWindow::add_Activated");
        return E_UNEXPECTED;
    }

    IFACEMETHODIMP remove_Activated( 
        EventRegistrationToken token)
    {
        Assert::Fail(L"Unexpected call to IWindow::remove_Activated");
        return E_UNEXPECTED;
    }
                        
    IFACEMETHODIMP add_Closed( 
        ABI::Windows::UI::Xaml::IWindowClosedEventHandler *value,
        EventRegistrationToken *token)
    {
        Assert::Fail(L"Unexpected call to IWindow::add_Closed");
        return E_UNEXPECTED;
    }
                        
    IFACEMETHODIMP remove_Closed( 
        EventRegistrationToken token)
    {
        Assert::Fail(L"Unexpected call to IWindow::remove_Closed");
        return E_UNEXPECTED;
    }
                        
    IFACEMETHODIMP add_SizeChanged( 
        ABI::Windows::UI::Xaml::IWindowSizeChangedEventHandler *value,
        EventRegistrationToken *token)
    {
        Assert::Fail(L"Unexpected call to IWindow::add_SizeChanged");
        return E_UNEXPECTED;
    }
                        
    IFACEMETHODIMP remove_SizeChanged( 
        EventRegistrationToken token)
    {
        Assert::Fail(L"Unexpected call to IWindow::remove_SizeChanged");
        return E_UNEXPECTED;
    }
                        
    IFACEMETHODIMP add_VisibilityChanged( 
        ABI::Windows::UI::Xaml::IWindowVisibilityChangedEventHandler *value,
        EventRegistrationToken *token)
    {
        return VisibilityChangedEventSource->add_Event(value, token);
    }
                        
    IFACEMETHODIMP remove_VisibilityChanged( 
        EventRegistrationToken token)
    {
        return VisibilityChangedEventSource->remove_Event(token);
    }
                        
    IFACEMETHODIMP Activate()
    {
        Assert::Fail(L"Unexpected call to IWindow::Activate");
        return E_UNEXPECTED;
    }
                        
    IFACEMETHODIMP Close()
    {
        Assert::Fail(L"Unexpected call to IWindow::Close");
        return E_UNEXPECTED;
    }
};
