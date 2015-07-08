// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

template<typename T>
class Nullable : public Microsoft::WRL::RuntimeClass<ABI::Windows::Foundation::IReference<T>>
{
    InspectableClass(IReference<T>::z_get_rc_name_impl(), BaseTrust);

    T m_value;

    static_assert(std::is_pod<T>::value, "T must be plain-old-data");

public:
    Nullable(T const& value)
        : m_value(value)
    {}

    IFACEMETHODIMP get_Value(T* value)
    {
        if (!value)
            return E_POINTER;

        *value = m_value;
        return S_OK;
    }
};
