// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

template<typename T>
class ReferenceArray : public Microsoft::WRL::RuntimeClass<ABI::Windows::Foundation::IReferenceArray<T>>
                     , private LifespanTracker<ReferenceArray<T>>
{
    InspectableClass(IReferenceArray<T>::z_get_rc_name_impl(), BaseTrust);

public:
    // T_abi and T are normally the same, but when T=bool, T_abi=boolean.
    typedef typename ABI::Windows::Foundation::Internal::GetAbiType<typename RuntimeClass::IReferenceArray::T_complex>::type T_abi;

private:
    std::vector<T_abi> m_array;

    static_assert(std::is_pod<T>::value, "T must be plain-old-data");

public:
    template<typename... Args>
    ReferenceArray(Args&&... args)
        : m_array(std::forward<Args>(args)...)
    { }

    IFACEMETHODIMP get_Value(UINT32* length, T_abi** value)
    {
        return ExceptionBoundary([&]
        {
            CheckInPointer(length);
            CheckAndClearOutPointer(value);

            ComArray<T_abi> arrayCopy(m_array.begin(), m_array.end());

            arrayCopy.Detach(length, value);
        });
    }
};
