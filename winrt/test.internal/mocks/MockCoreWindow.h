// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace canvas
{
    class MockCoreWindow : public RuntimeClass<
        ABI::Windows::UI::Core::ICoreWindow>
    {
    public:
        CALL_COUNTER_WITH_MOCK(get_BoundsMethod, HRESULT(ABI::Windows::Foundation::Rect*));

        IFACEMETHODIMP get_AutomationHostProvider( 
            IInspectable**) override
        {
            Assert::Fail(L"Unexpected call to get_AutomationHostProvider");
            return E_NOTIMPL;
        }
                        
        IFACEMETHODIMP get_Bounds( 
            ABI::Windows::Foundation::Rect* bounds) override
        {
            return get_BoundsMethod.WasCalled(bounds);
        }
                        
        IFACEMETHODIMP get_CustomProperties( 
            ABI::Windows::Foundation::Collections::IPropertySet**) override
        {
            Assert::Fail(L"Unexpected call to get_CustomProperties");
            return E_NOTIMPL;
        }
                        
        IFACEMETHODIMP get_Dispatcher( 
            ABI::Windows::UI::Core::ICoreDispatcher**) override
        {
            Assert::Fail(L"Unexpected call to get_Dispatcher");
            return E_NOTIMPL;
        }
                        
        IFACEMETHODIMP get_FlowDirection( 
            ABI::Windows::UI::Core::CoreWindowFlowDirection*) override
        {
            Assert::Fail(L"Unexpected call to get_FlowDirection");
            return E_NOTIMPL;
        }
                        
        IFACEMETHODIMP put_FlowDirection( 
            ABI::Windows::UI::Core::CoreWindowFlowDirection) override
        {
            Assert::Fail(L"Unexpected call to put_FlowDirection");
            return E_NOTIMPL;
        }
                        
        IFACEMETHODIMP get_IsInputEnabled( 
            boolean*) override
        {
            Assert::Fail(L"Unexpected call to get_IsInputEnabled");
            return E_NOTIMPL;
        }
                        
        IFACEMETHODIMP put_IsInputEnabled( 
            boolean) override
        {
            Assert::Fail(L"Unexpected call to put_IsInputEnabled");
            return E_NOTIMPL;
        }
                        
        IFACEMETHODIMP get_PointerCursor( 
            ABI::Windows::UI::Core::ICoreCursor**) override
        {
            Assert::Fail(L"Unexpected call to get_PointerCursor");
            return E_NOTIMPL;
        }
                        
        IFACEMETHODIMP put_PointerCursor( 
            ABI::Windows::UI::Core::ICoreCursor*) override
        {
            Assert::Fail(L"Unexpected call to put_PointerCursor");
            return E_NOTIMPL;
        }
                        
        IFACEMETHODIMP get_PointerPosition( 
            ABI::Windows::Foundation::Point*) override
        {
            Assert::Fail(L"Unexpected call to get_PointerPosition");
            return E_NOTIMPL;
        }
                        
        IFACEMETHODIMP get_Visible( 
            boolean*) override
        {
            Assert::Fail(L"Unexpected call to get_Visible");
            return E_NOTIMPL;
        }
                        
        IFACEMETHODIMP Activate() override
        {
            Assert::Fail(L"Unexpected call to Activate");
            return E_NOTIMPL;
        }
                        
        IFACEMETHODIMP Close() override
        {
            Assert::Fail(L"Unexpected call to Close");
            return E_NOTIMPL;
        }
                        
        IFACEMETHODIMP GetAsyncKeyState( 
            ABI::Windows::System::VirtualKey,
            ABI::Windows::UI::Core::CoreVirtualKeyStates*) override
        {
            Assert::Fail(L"Unexpected call to GetAsyncKeyState");
            return E_NOTIMPL;
        }
                        
        IFACEMETHODIMP GetKeyState( 
            ABI::Windows::System::VirtualKey,
            ABI::Windows::UI::Core::CoreVirtualKeyStates*) override
        {
            Assert::Fail(L"Unexpected call to GetKeyState");
            return E_NOTIMPL;
        }
                        
        IFACEMETHODIMP ReleasePointerCapture() override
        {
            Assert::Fail(L"Unexpected call to ReleasePointerCapture");
            return E_NOTIMPL;
        }
                        
        IFACEMETHODIMP SetPointerCapture() override
        {
            Assert::Fail(L"Unexpected call to SetPointerCapture");
            return E_NOTIMPL;
        }
                        
        IFACEMETHODIMP add_Activated( 
            __FITypedEventHandler_2_Windows__CUI__CCore__CCoreWindow_Windows__CUI__CCore__CWindowActivatedEventArgs*,
            EventRegistrationToken*) override
        {
            Assert::Fail(L"Unexpected call to add_Activated");
            return E_NOTIMPL;
        }
                        
        IFACEMETHODIMP remove_Activated( 
            EventRegistrationToken) override
        {
            Assert::Fail(L"Unexpected call to remove_Activated");
            return E_NOTIMPL;
        }
                        
        IFACEMETHODIMP add_AutomationProviderRequested( 
            __FITypedEventHandler_2_Windows__CUI__CCore__CCoreWindow_Windows__CUI__CCore__CAutomationProviderRequestedEventArgs*,
            EventRegistrationToken*) override
        {
            Assert::Fail(L"Unexpected call to add_AutomationProviderRequested");
            return E_NOTIMPL;
        }
                        
        IFACEMETHODIMP remove_AutomationProviderRequested( 
            EventRegistrationToken) override
        {
            Assert::Fail(L"Unexpected call to remove_AutomationProviderRequested");
            return E_NOTIMPL;
        }
                        
        IFACEMETHODIMP add_CharacterReceived( 
            __FITypedEventHandler_2_Windows__CUI__CCore__CCoreWindow_Windows__CUI__CCore__CCharacterReceivedEventArgs*,
            EventRegistrationToken*) override
        {
            Assert::Fail(L"Unexpected call to add_CharacterReceived");
            return E_NOTIMPL;
        }
                        
        IFACEMETHODIMP remove_CharacterReceived( 
            EventRegistrationToken) override
        {
            Assert::Fail(L"Unexpected call to remove_CharacterReceived");
            return E_NOTIMPL;
        }
                        
        IFACEMETHODIMP add_Closed( 
            __FITypedEventHandler_2_Windows__CUI__CCore__CCoreWindow_Windows__CUI__CCore__CCoreWindowEventArgs*,
            EventRegistrationToken*) override
        {
            Assert::Fail(L"Unexpected call to add_Closed");
            return E_NOTIMPL;
        }
                        
        IFACEMETHODIMP remove_Closed( 
            EventRegistrationToken) override
        {
            Assert::Fail(L"Unexpected call to remove_Closed");
            return E_NOTIMPL;
        }
                        
        IFACEMETHODIMP add_InputEnabled( 
            __FITypedEventHandler_2_Windows__CUI__CCore__CCoreWindow_Windows__CUI__CCore__CInputEnabledEventArgs*,
            EventRegistrationToken*) override
        {
            Assert::Fail(L"Unexpected call to add_InputEnabled");
            return E_NOTIMPL;
        }
                        
        IFACEMETHODIMP remove_InputEnabled( 
            EventRegistrationToken) override
        {
            Assert::Fail(L"Unexpected call to remove_InputEnabled");
            return E_NOTIMPL;
        }
                        
        IFACEMETHODIMP add_KeyDown( 
            __FITypedEventHandler_2_Windows__CUI__CCore__CCoreWindow_Windows__CUI__CCore__CKeyEventArgs*,
            EventRegistrationToken*) override
        {
            Assert::Fail(L"Unexpected call to add_KeyDown");
            return E_NOTIMPL;
        }
                        
        IFACEMETHODIMP remove_KeyDown( 
            EventRegistrationToken) override
        {
            Assert::Fail(L"Unexpected call to remove_KeyDown");
            return E_NOTIMPL;
        }
                        
        IFACEMETHODIMP add_KeyUp( 
            __FITypedEventHandler_2_Windows__CUI__CCore__CCoreWindow_Windows__CUI__CCore__CKeyEventArgs*,
            EventRegistrationToken*) override
        {
            Assert::Fail(L"Unexpected call to add_KeyUp");
            return E_NOTIMPL;
        }
                        
        IFACEMETHODIMP remove_KeyUp( 
            EventRegistrationToken) override
        {
            Assert::Fail(L"Unexpected call to remove_KeyUp");
            return E_NOTIMPL;
        }
                        
        IFACEMETHODIMP add_PointerCaptureLost( 
            __FITypedEventHandler_2_Windows__CUI__CCore__CCoreWindow_Windows__CUI__CCore__CPointerEventArgs*,
            EventRegistrationToken*) override
        {
            Assert::Fail(L"Unexpected call to add_PointerCaptureLost");
            return E_NOTIMPL;
        }
                        
        IFACEMETHODIMP remove_PointerCaptureLost( 
            EventRegistrationToken) override
        {
            Assert::Fail(L"Unexpected call to remove_PointerCaptureLost");
            return E_NOTIMPL;
        }
                        
        IFACEMETHODIMP add_PointerEntered( 
            __FITypedEventHandler_2_Windows__CUI__CCore__CCoreWindow_Windows__CUI__CCore__CPointerEventArgs*,
            EventRegistrationToken*) override
        {
            Assert::Fail(L"Unexpected call to add_PointerEntered");
            return E_NOTIMPL;
        }
                        
        IFACEMETHODIMP remove_PointerEntered( 
            EventRegistrationToken) override
        {
            Assert::Fail(L"Unexpected call to remove_PointerEntered");
            return E_NOTIMPL;
        }
                        
        IFACEMETHODIMP add_PointerExited( 
            __FITypedEventHandler_2_Windows__CUI__CCore__CCoreWindow_Windows__CUI__CCore__CPointerEventArgs*,
            EventRegistrationToken*) override
        {
            Assert::Fail(L"Unexpected call to add_PointerExited");
            return E_NOTIMPL;
        }
                        
        IFACEMETHODIMP remove_PointerExited( 
            EventRegistrationToken) override
        {
            Assert::Fail(L"Unexpected call to remove_PointerExited");
            return E_NOTIMPL;
        }
                        
        IFACEMETHODIMP add_PointerMoved( 
            __FITypedEventHandler_2_Windows__CUI__CCore__CCoreWindow_Windows__CUI__CCore__CPointerEventArgs*,
            EventRegistrationToken*) override
        {
            Assert::Fail(L"Unexpected call to add_PointerMoved");
            return E_NOTIMPL;
        }
                        
        IFACEMETHODIMP remove_PointerMoved( 
            EventRegistrationToken) override
        {
            Assert::Fail(L"Unexpected call to remove_PointerMoved");
            return E_NOTIMPL;
        }
                        
        IFACEMETHODIMP add_PointerPressed( 
            __FITypedEventHandler_2_Windows__CUI__CCore__CCoreWindow_Windows__CUI__CCore__CPointerEventArgs*,
            EventRegistrationToken*) override
        {
            Assert::Fail(L"Unexpected call to add_PointerPressed");
            return E_NOTIMPL;
        }
                        
        IFACEMETHODIMP remove_PointerPressed( 
            EventRegistrationToken) override
        {
            Assert::Fail(L"Unexpected call to remove_PointerPressed");
            return E_NOTIMPL;
        }
                        
        IFACEMETHODIMP add_PointerReleased( 
            __FITypedEventHandler_2_Windows__CUI__CCore__CCoreWindow_Windows__CUI__CCore__CPointerEventArgs*,
            EventRegistrationToken*) override
        {
            Assert::Fail(L"Unexpected call to add_PointerReleased");
            return E_NOTIMPL;
        }
                        
        IFACEMETHODIMP remove_PointerReleased( 
            EventRegistrationToken) override
        {
            Assert::Fail(L"Unexpected call to remove_PointerReleased");
            return E_NOTIMPL;
        }
                        
        IFACEMETHODIMP add_TouchHitTesting( 
            __FITypedEventHandler_2_Windows__CUI__CCore__CCoreWindow_Windows__CUI__CCore__CTouchHitTestingEventArgs*,
            EventRegistrationToken*) override
        {
            Assert::Fail(L"Unexpected call to add_TouchHitTesting");
            return E_NOTIMPL;
        }
                        
        IFACEMETHODIMP remove_TouchHitTesting( 
            EventRegistrationToken) override
        {
            Assert::Fail(L"Unexpected call to remove_TouchHitTesting");
            return E_NOTIMPL;
        }
                        
        IFACEMETHODIMP add_PointerWheelChanged( 
            __FITypedEventHandler_2_Windows__CUI__CCore__CCoreWindow_Windows__CUI__CCore__CPointerEventArgs*,
            EventRegistrationToken*) override
        {
            Assert::Fail(L"Unexpected call to add_PointerWheelChanged");
            return E_NOTIMPL;
        }
                        
        IFACEMETHODIMP remove_PointerWheelChanged( 
            EventRegistrationToken) override
        {
            Assert::Fail(L"Unexpected call to remove_PointerWheelChanged");
            return E_NOTIMPL;
        }
                        
        IFACEMETHODIMP add_SizeChanged( 
            __FITypedEventHandler_2_Windows__CUI__CCore__CCoreWindow_Windows__CUI__CCore__CWindowSizeChangedEventArgs*,
            EventRegistrationToken*) override
        {
            Assert::Fail(L"Unexpected call to add_SizeChanged");
            return E_NOTIMPL;
        }
                        
        IFACEMETHODIMP remove_SizeChanged( 
            EventRegistrationToken) override
        {
            Assert::Fail(L"Unexpected call to remove_SizeChanged");
            return E_NOTIMPL;
        }
                        
        IFACEMETHODIMP add_VisibilityChanged( 
            __FITypedEventHandler_2_Windows__CUI__CCore__CCoreWindow_Windows__CUI__CCore__CVisibilityChangedEventArgs*,
            EventRegistrationToken*) override
        {
            Assert::Fail(L"Unexpected call to add_VisibilityChanged");
            return E_NOTIMPL;
        }
                        
        IFACEMETHODIMP remove_VisibilityChanged( 
            EventRegistrationToken) override
        {
            Assert::Fail(L"Unexpected call to remove_VisibilityChanged");
            return E_NOTIMPL;
        }
                        
    };
}
