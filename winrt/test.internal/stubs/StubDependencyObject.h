// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace canvas
{
    class StubDependencyObject : public RuntimeClass<IDependencyObject>
    {

    public:
        StubDependencyObject()
        {
        }

    public:
        IFACEMETHODIMP GetValue(ABI::Microsoft::UI::Xaml::IDependencyProperty* dp, IInspectable** result)  override { return S_OK; }

        IFACEMETHODIMP SetValue(ABI::Microsoft::UI::Xaml::IDependencyProperty* dp, IInspectable* value)  override { return S_OK; }

        IFACEMETHODIMP ClearValue(ABI::Microsoft::UI::Xaml::IDependencyProperty* dp)  override { return S_OK; }

        IFACEMETHODIMP ReadLocalValue(ABI::Microsoft::UI::Xaml::IDependencyProperty* dp, IInspectable** result)  override { return S_OK; }

        IFACEMETHODIMP GetAnimationBaseValue(ABI::Microsoft::UI::Xaml::IDependencyProperty* dp, IInspectable** result)  override { return S_OK; }

        IFACEMETHODIMP RegisterPropertyChangedCallback(ABI::Microsoft::UI::Xaml::IDependencyProperty* dp, ABI::Microsoft::UI::Xaml::IDependencyPropertyChangedCallback* callback, INT64* result)  override { return S_OK; }

        IFACEMETHODIMP UnregisterPropertyChangedCallback(ABI::Microsoft::UI::Xaml::IDependencyProperty* dp, INT64 token)  override { return S_OK; }

        IFACEMETHODIMP get_Dispatcher(ABI::Windows::UI::Core::ICoreDispatcher** value)  override { return S_OK; }

        IFACEMETHODIMP get_DispatcherQueue(ABI::Microsoft::UI::Dispatching::IDispatcherQueue** value)  override { return S_OK; }

    };
}
