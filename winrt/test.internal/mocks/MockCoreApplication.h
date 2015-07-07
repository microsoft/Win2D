// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace canvas
{
    using namespace ABI::Windows::Foundation::Collections;

    class MockCoreApplication : public RuntimeClass<
        ABI::Windows::ApplicationModel::Core::ICoreApplication>
    {
    public:
        CALL_COUNTER_WITH_MOCK(get_PropertiesMethod, HRESULT(IPropertySet**));

        IFACEMETHODIMP get_Id(HSTRING *)
        {
            Assert::Fail(L"Unexpected call to get_Id");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP add_Suspending(ABI::Windows::Foundation::__FIEventHandler_1_Windows__CApplicationModel__CSuspendingEventArgs_t *,EventRegistrationToken *)
        {
            Assert::Fail(L"Unexpected call to add_Suspending");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP remove_Suspending(EventRegistrationToken)
        {
            Assert::Fail(L"Unexpected call to remove_Suspending");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP add_Resuming(ABI::Windows::Foundation::__FIEventHandler_1_IInspectable_t *,EventRegistrationToken *)
        {
            Assert::Fail(L"Unexpected call to add_Resuming");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP remove_Resuming(EventRegistrationToken)
        {
            Assert::Fail(L"Unexpected call to remove_Resuming");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP get_Properties(IPropertySet** propertySet)
        {
            return get_PropertiesMethod.WasCalled(propertySet);
        }

        IFACEMETHODIMP GetCurrentView(ABI::Windows::ApplicationModel::Core::ICoreApplicationView **)
        {
            Assert::Fail(L"Unexpected call to GetCurrentView");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP Run(ABI::Windows::ApplicationModel::Core::IFrameworkViewSource *)
        {
            Assert::Fail(L"Unexpected call to Run");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP RunWithActivationFactories(ABI::Windows::Foundation::IGetActivationFactory *)
        {
            Assert::Fail(L"Unexpected call to RunWithActivationFactories");
            return E_NOTIMPL;
        }
    };
}
