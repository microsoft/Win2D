// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

template<typename T>
class Nullable : public Microsoft::WRL::RuntimeClass<ABI::Windows::Foundation::IReference<T>>
{
    InspectableClass(IReference<T>::z_get_rc_name_impl(), BaseTrust);

public:
    // T_abi and T are normally the same, but when T=bool, T_abi=boolean.
    typedef typename ABI::Windows::Foundation::Internal::GetAbiType<typename RuntimeClass::IReference::T_complex>::type T_abi;

private:
    T_abi m_value;

    static_assert(std::is_pod<T>::value, "T must be plain-old-data");

public:
    Nullable(T_abi const& value)
        : m_value(value)
    {}

    IFACEMETHODIMP get_Value(T_abi* value)
    {
        if (!value)
            return E_POINTER;

        *value = m_value;
        return S_OK;
    }
};
