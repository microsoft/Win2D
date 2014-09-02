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
    using namespace ABI::Windows::Foundation::Collections;

    class MockCoreApplication : public RuntimeClass<
        ABI::Windows::ApplicationModel::Core::ICoreApplication>
    {
    public:
        std::function<ComPtr<IPropertySet>()> MockGetProperties;

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
            if (!MockGetProperties)
            {
                Assert::Fail(L"Unexpected call to get_Properties");
                return E_NOTIMPL;
            }

            return ExceptionBoundary(
                [&]
                {
                    auto properties = MockGetProperties();
                    ThrowIfFailed(properties.CopyTo(propertySet));
                });
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
