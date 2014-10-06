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
