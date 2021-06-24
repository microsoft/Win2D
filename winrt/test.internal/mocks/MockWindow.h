// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

class MockWindow : public RuntimeClass<IWindow>
{
    bool m_visible;

public:
    ComPtr<MockEventSource<WindowVisibilityChangedEventHandler>> VisibilityChangedEventSource;

    CALL_COUNTER_WITH_MOCK(get_DispatcherQueueMethod, HRESULT(ABI::Microsoft::UI::Dispatching::IDispatcherQueue**));

    MockWindow()
        : m_visible(true)
        , VisibilityChangedEventSource(Make<MockEventSource<WindowVisibilityChangedEventHandler>>(L"VisibilityChanged"))
    {
    }

    void SetVisible(bool visible)
    {
        m_visible = visible;

        class VisibilityChangedEventArgs : public RuntimeClass<ABI::Microsoft::UI::Xaml::IWindowVisibilityChangedEventArgs>
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

            IFACEMETHODIMP get_Handled(boolean* value) override
            {
                Assert::Fail(L"Unexpected call to IWindowVisibilityChangedEventArgs::get_Handled");
                return E_UNEXPECTED;
            }

            IFACEMETHODIMP put_Handled(boolean value) override
            {
                Assert::Fail(L"Unexpected call to IWindowVisibilityChangedEventArgs::put_Handled");
                return E_UNEXPECTED;
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
        ABI::Microsoft::UI::Xaml::IUIElement **value)
    {
        Assert::Fail(L"Unexpected call to IWindow::get_Content");
        return E_UNEXPECTED;
    }
                       
    IFACEMETHODIMP put_Content( 
        ABI::Microsoft::UI::Xaml::IUIElement *value)
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
              
#ifdef WINUI3
    IFACEMETHODIMP get_Compositor(
        ABI::Microsoft::UI::Composition::ICompositor** value
    )
    {
        Assert::Fail(L"Unexpected call to IWindow::get_Compositor");
        return E_UNEXPECTED;
    }

    IFACEMETHODIMP STDMETHODCALLTYPE get_DispatcherQueue(
        ABI::Microsoft::UI::Dispatching::IDispatcherQueue** value
    )
    {
        return get_DispatcherQueueMethod.WasCalled(value);
    }

    virtual HRESULT STDMETHODCALLTYPE get_Title(
        HSTRING* value
    )
    {
        Assert::Fail(L"Unexpected call to IWindow::get_Title");
        return E_UNEXPECTED;
    }

    virtual HRESULT STDMETHODCALLTYPE put_Title(
        HSTRING value
    )
    {
        Assert::Fail(L"Unexpected call to IWindow::put_Title");
        return E_UNEXPECTED;
    }

    virtual HRESULT STDMETHODCALLTYPE get_ExtendsContentIntoTitleBar(
        boolean* value)
    {
        Assert::Fail(L"Unexpected call to IWindow::get_ExtendsContentIntoTitleBar");
        return E_UNEXPECTED;
    }

    virtual HRESULT STDMETHODCALLTYPE put_ExtendsContentIntoTitleBar(
        boolean value
    )
    {
        Assert::Fail(L"Unexpected call to IWindow::put_ExtendsContentIntoTitleBar");
        return E_UNEXPECTED;
    }
#endif

    IFACEMETHODIMP get_Dispatcher( 
        ABI::Windows::UI::Core::ICoreDispatcher **value)
    {
#ifndef WINUI3
        return get_DispatcherMethod.WasCalled(value);
#else
        Assert::Fail(L"Unexpected call to IWindow::get_Dispatcher");
#endif
    }

    IFACEMETHODIMP add_Activated( 
#ifdef WINUI3
        __FITypedEventHandler_2_IInspectable_Microsoft__CUI__CXaml__CWindowActivatedEventArgs* handler,
#else
        ABI::Windows::UI::Xaml::IWindowActivatedEventHandler* value,
#endif
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
#ifdef WINUI3
        __FITypedEventHandler_2_IInspectable_Microsoft__CUI__CXaml__CWindowEventArgs* handler,
#else
        ABI::Windows::UI::Xaml::IWindowClosedEventHandler* value,
#endif
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
#ifdef WINUI3
        __FITypedEventHandler_2_IInspectable_Microsoft__CUI__CXaml__CWindowSizeChangedEventArgs* handler,
#else
        ABI::Windows::UI::Xaml::IWindowSizeChangedEventHandler* value,
#endif
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
#ifdef WINUI3
        __FITypedEventHandler_2_IInspectable_Microsoft__CUI__CXaml__CWindowVisibilityChangedEventArgs* handler,
#else
        ABI::Windows::UI::Xaml::WindowVisibilityChangedEventHandler* value,
#endif
        EventRegistrationToken *token)
    {
#ifdef WINUI3
        Assert::Fail(L"Unexpected call to IWindow::add_VisibilityChanged");
        return E_UNEXPECTED;
#else
        return VisibilityChangedEventSource->add_Event(value, token);
#endif
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

#ifdef WINUI3
    virtual HRESULT STDMETHODCALLTYPE SetTitleBar(
        ABI::Microsoft::UI::Xaml::IUIElement* titleBar
    )
    {
        Assert::Fail(L"Unexpected call to IWindow::SetTitleBar");
        return E_UNEXPECTED;
    }
#endif
};
