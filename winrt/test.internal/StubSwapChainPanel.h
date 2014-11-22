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

namespace canvas
{
    using namespace ABI::Windows::UI::Xaml::Controls;
    using namespace ABI::Windows::UI::Xaml;

    class StubSwapChainPanel : public RuntimeClass<
        RuntimeClassFlags<WinRtClassicComMix>, 
        ISwapChainPanel, 
        IFrameworkElement, 
        ISwapChainPanelNative>
    {
    public:
        CALL_COUNTER_WITH_MOCK(SetSwapChainMethod, HRESULT(IDXGISwapChain*));

        // ISwapChainPanelNative
        IFACEMETHODIMP SetSwapChain(IDXGISwapChain* swapChain) override
        {
            return SetSwapChainMethod.WasCalled(swapChain);
        }

        // ISwapChainPanel

        IFACEMETHODIMP get_CompositionScaleX(FLOAT *) override
        {
            Assert::Fail(L"Unexpected call to get_CompositionScaleX");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP get_CompositionScaleY(FLOAT *) override
        {
            Assert::Fail(L"Unexpected call to get_CompositionScaleY");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP add_CompositionScaleChanged(ITypedEventHandler<SwapChainPanel*, IInspectable*>*, EventRegistrationToken*) override
        {
            Assert::Fail(L"Unexpected call to add_CompositionScaleChanged");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP CreateCoreIndependentInputSource(CoreInputDeviceTypes, ICoreInputSourceBase**) override
        {
            Assert::Fail(L"Unexpected call to CreateCoreIndependentInputSource");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP remove_CompositionScaleChanged(EventRegistrationToken) override
        {
            Assert::Fail(L"Unexpected call to remove_CompositionScaleChanged");
            return E_NOTIMPL;
        }

        // IFrameworkElement

        IFACEMETHODIMP get_Triggers(ABI::Windows::Foundation::Collections::__FIVector_1_Windows__CUI__CXaml__CTriggerBase_t **) override 
        {
            Assert::Fail(L"Unexpected call to get_Triggers");
            return E_NOTIMPL; 
        }

        IFACEMETHODIMP get_Resources(IResourceDictionary **) override 
        {
            Assert::Fail(L"Unexpected call to get_Resources");
            return E_NOTIMPL; 
        }

        IFACEMETHODIMP put_Resources(IResourceDictionary *) override 
        {
            Assert::Fail(L"Unexpected call to put_Resources");
            return E_NOTIMPL; 
        }

        IFACEMETHODIMP get_Tag(IInspectable **) override 
        {
            Assert::Fail(L"Unexpected call to get_Tag");
            return E_NOTIMPL; 
        }

        IFACEMETHODIMP put_Tag(IInspectable *) override 
        {
            Assert::Fail(L"Unexpected call to put_Tag");
            return E_NOTIMPL; 
        }

        IFACEMETHODIMP get_Language(HSTRING *) override 
        {
            Assert::Fail(L"Unexpected call to get_Language");
            return E_NOTIMPL; 
        }

        IFACEMETHODIMP put_Language(HSTRING) override 
        {
            Assert::Fail(L"Unexpected call to put_Language");
            return E_NOTIMPL; 
        }

        IFACEMETHODIMP get_ActualWidth(double* value) override
        {
            Assert::Fail(L"Unexpected call to get_ActualWidth");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP get_ActualHeight(double* value) override 
        {
            Assert::Fail(L"Unexpected call to get_ActualHeight");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP get_Width(DOUBLE *) override 
        {
            Assert::Fail(L"Unexpected call to get_Width");
            return E_NOTIMPL; 
        }

        IFACEMETHODIMP put_Width(DOUBLE) override 
        {
            Assert::Fail(L"Unexpected call to put_Width");
            return E_NOTIMPL; 
        }

        IFACEMETHODIMP get_Height(DOUBLE *) override 
        {
            Assert::Fail(L"Unexpected call to get_Height");
            return E_NOTIMPL; 
        }

        IFACEMETHODIMP put_Height(DOUBLE) override 
        {
            Assert::Fail(L"Unexpected call to put_Height");
            return E_NOTIMPL; 
        }

        IFACEMETHODIMP get_MinWidth(DOUBLE *) override 
        {
            Assert::Fail(L"Unexpected call to get_MinWidth");
            return E_NOTIMPL; 
        }

        IFACEMETHODIMP put_MinWidth(DOUBLE) override 
        {
            Assert::Fail(L"Unexpected call to put_MinWidth");
            return E_NOTIMPL; 
        }

        IFACEMETHODIMP get_MaxWidth(DOUBLE *) override 
        {
            Assert::Fail(L"Unexpected call to get_MaxWidth");
            return E_NOTIMPL; 
        }

        IFACEMETHODIMP put_MaxWidth(DOUBLE) override 
        {
            Assert::Fail(L"Unexpected call to put_MaxWidth");
            return E_NOTIMPL; 
        }

        IFACEMETHODIMP get_MinHeight(DOUBLE *) override 
        {
            Assert::Fail(L"Unexpected call to get_MinHeight");
            return E_NOTIMPL; 
        }

        IFACEMETHODIMP put_MinHeight(DOUBLE) override 
        {
            Assert::Fail(L"Unexpected call to put_MinHeight");
            return E_NOTIMPL; 
        }

        IFACEMETHODIMP get_MaxHeight(DOUBLE *) override 
        {
            Assert::Fail(L"Unexpected call to get_MaxHeight");
            return E_NOTIMPL; 
        }

        IFACEMETHODIMP put_MaxHeight(DOUBLE) override 
        {
            Assert::Fail(L"Unexpected call to put_MaxHeight");
            return E_NOTIMPL; 
        }

        IFACEMETHODIMP get_HorizontalAlignment(HorizontalAlignment *) override 
        {
            Assert::Fail(L"Unexpected call to get_HorizontalAlignment");
            return E_NOTIMPL; 
        }

        IFACEMETHODIMP put_HorizontalAlignment(HorizontalAlignment) override 
        {
            Assert::Fail(L"Unexpected call to put_HorizontalAlignment");
            return E_NOTIMPL; 
        }

        IFACEMETHODIMP get_VerticalAlignment(VerticalAlignment *) override 
        {
            Assert::Fail(L"Unexpected call to get_VerticalAlignment");
            return E_NOTIMPL; 
        }

        IFACEMETHODIMP put_VerticalAlignment(VerticalAlignment) override 
        {
            Assert::Fail(L"Unexpected call to put_VerticalAlignment");
            return E_NOTIMPL; 
        }

        IFACEMETHODIMP get_Margin(Thickness *) override 
        {
            Assert::Fail(L"Unexpected call to get_Margin");
            return E_NOTIMPL; 
        }

        IFACEMETHODIMP put_Margin(Thickness) override 
        {
            Assert::Fail(L"Unexpected call to put_Margin");
            return E_NOTIMPL; 
        }

        IFACEMETHODIMP get_Name(HSTRING *) override 
        {
            Assert::Fail(L"Unexpected call to get_Name");
            return E_NOTIMPL; 
        }

        IFACEMETHODIMP put_Name(HSTRING) override 
        {
            Assert::Fail(L"Unexpected call to put_Name");
            return E_NOTIMPL; 
        }

        IFACEMETHODIMP get_BaseUri(ABI::Windows::Foundation::IUriRuntimeClass **) override 
        {
            Assert::Fail(L"Unexpected call to get_BaseUri");
            return E_NOTIMPL; 
        }

        IFACEMETHODIMP get_DataContext(IInspectable **) override 
        {
            Assert::Fail(L"Unexpected call to get_DataContext");
            return E_NOTIMPL; 
        }

        IFACEMETHODIMP put_DataContext(IInspectable *) override 
        {
            Assert::Fail(L"Unexpected call to put_DataContext");
            return E_NOTIMPL; 
        }

        IFACEMETHODIMP get_Style(IStyle **) override 
        {
            Assert::Fail(L"Unexpected call to get_Style");
            return E_NOTIMPL; 
        }

        IFACEMETHODIMP put_Style(IStyle *) override 
        {
            Assert::Fail(L"Unexpected call to put_Style");
            return E_NOTIMPL; 
        }

        IFACEMETHODIMP get_Parent(IDependencyObject **) override 
        {
            Assert::Fail(L"Unexpected call to get_Parent");
            return E_NOTIMPL; 
        }

        IFACEMETHODIMP get_FlowDirection(FlowDirection *) override 
        {
            Assert::Fail(L"Unexpected call to get_FlowDirection");
            return E_NOTIMPL; 
        }

        IFACEMETHODIMP put_FlowDirection(FlowDirection) override 
        {
            Assert::Fail(L"Unexpected call to put_FlowDirection");
            return E_NOTIMPL; 
        }

        IFACEMETHODIMP add_Loaded(IRoutedEventHandler* handler, EventRegistrationToken* token) override 
        {
            Assert::Fail(L"Unexpected call to add_Loaded");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP remove_Loaded(EventRegistrationToken token) override 
        {
            Assert::Fail(L"Unexpected call to remove_Loaded");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP add_Unloaded(IRoutedEventHandler *,EventRegistrationToken *) override 
        {
            Assert::Fail(L"Unexpected call to add_Unloaded");
            return E_NOTIMPL; 
        }

        IFACEMETHODIMP remove_Unloaded(EventRegistrationToken) override 
        {
            Assert::Fail(L"Unexpected call to remove_Unloaded");
            return E_NOTIMPL; 
        }

        IFACEMETHODIMP add_SizeChanged(ISizeChangedEventHandler* handler, EventRegistrationToken* token) override 
        {
            Assert::Fail(L"Unexpected call to add_SizeChanged");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP remove_SizeChanged(EventRegistrationToken token) override 
        {
            Assert::Fail(L"Unexpected call to remove_SizeChanged");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP add_LayoutUpdated(ABI::Windows::Foundation::__FIEventHandler_1_IInspectable_t *,EventRegistrationToken *) override 
        {
            Assert::Fail(L"Unexpected call to add_LayoutUpdated");
            return E_NOTIMPL; 
        }

        IFACEMETHODIMP remove_LayoutUpdated(EventRegistrationToken) override 
        {
            Assert::Fail(L"Unexpected call to remove_LayoutUpdated");
            return E_NOTIMPL; 
        }

        IFACEMETHODIMP FindName(HSTRING,IInspectable **) override 
        {
            Assert::Fail(L"Unexpected call to FindName");
            return E_NOTIMPL; 
        }

        IFACEMETHODIMP SetBinding(IDependencyProperty *,Data::IBindingBase *) override 
        {
            Assert::Fail(L"Unexpected call to SetBinding");
            return E_NOTIMPL; 
        }
    };
}

