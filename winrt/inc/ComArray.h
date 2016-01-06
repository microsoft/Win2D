// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#include <algorithm>
#include "Utilities.h"
#include "WinStringWrapper.h"

//
// RAII type for managing COM arrays.  These are arrays that are passed to/from
// WinRT interfaces.  The memory backing them must be managed using
// CoTaskMemAlloc and CoTaskMemFree.
//
// There are two main scenarios for using this:
//
// 1. Retrieving an array from a WinRT API such as IPropertyValue.
//
//    ComArray<float> array;
//    ThrowIfFailed(propertyValue->GetSingleArray(array.GetAddressOfSize(), array.GetAddressOfData()));
//    if (array.GetSize() != 16)
//        ThowHR(E_BOUNDS);
//    *output = *(Numerics::Matrix4x4*)array.GetData();
//
//
// 2. Returning an array from a WinRT API:
//
//    ComArray<CanvasGradientStop> array;
//    // populate the array
//    array.Detach(valueCount, valueElements);
// 
//
// The array can be populated using the begin() iterator.  However, the
// CopyToComArray and TransformToComArray helpers capture common usage:
//
// std::vector<float> sourceFloats;
// ComArray<float> array(sourceFloats.begin(), sourceFloats.end());
//
// std::vector<int> sourceInts;
// ComArray<double> array = TransformToComArray<double>(sourceInts.begin(), sourceInts.end(), 
//     [](int v) { return static_cast<double>(v); });
//
//
// For testing purposes, ComArray is templated on a base class.  This
// CoTaskMemFree/CoTaskMemAlloc to be replaced with test doubles.
//
struct EmptyComArrayBase {};

// Traits describing how to implement a ComArray of plain-old-data.
template<typename T>
struct ComArrayTraits
{
    static_assert(std::is_pod<T>::value && !std::is_pointer<T>::value, "T must be plain-old non-pointer data");

    typedef T* data_ptr_t;

    static void InitializeElements(T*, uint32_t) { }
    static void ReleaseElements(T*, uint32_t) { }
};

// Traits describing how to implement a ComArray of ComPtr<T>.
template<typename T>
struct ComArrayTraits<Microsoft::WRL::ComPtr<T>>
{
    static_assert(sizeof(Microsoft::WRL::ComPtr<T>) == sizeof(T*), "ComPtr must be reinterpretable as T*");

    typedef T** data_ptr_t;

    static void InitializeElements(Microsoft::WRL::ComPtr<T>* data, uint32_t size)
    {
        std::uninitialized_fill_n(data->GetAddressOf(), size, nullptr);
    }

    static void ReleaseElements(Microsoft::WRL::ComPtr<T>* data, uint32_t size)
    {
        std::for_each(data, data + size, std::mem_fn(&Microsoft::WRL::ComPtr<T>::Reset));
    }
};

// Traits describing how to implement a ComArray of WinString.
template<typename Base>
struct ComArrayTraits<WinStringT<Base>>
{
    static_assert(sizeof(WinStringT<Base>) == sizeof(HSTRING), "WinString must be reinterpretable as HSTRING");

    typedef HSTRING* data_ptr_t;

    static void InitializeElements(WinStringT<Base>* data, uint32_t size)
    {
        std::uninitialized_fill_n(data, size, nullptr);
    }

    static void ReleaseElements(WinStringT<Base>* data, uint32_t size)
    {
        std::for_each(data, data + size, std::mem_fn(&WinStringT<Base>::Release));
    }
};

template<typename T, typename Base=EmptyComArrayBase>
class ComArray : public Base
{
    T* m_data;
    uint32_t m_size;

    typedef ComArrayTraits<T> Traits;

public:
    ComArray(ComArray const&) = delete;
    ComArray& operator=(ComArray const&) = delete;

    ComArray()
        : m_data(nullptr)
        , m_size(0)
    {
    }

    ComArray(size_t size)
        : m_data(static_cast<T*>(CoTaskMemAlloc(size * sizeof(T))))
        , m_size(static_cast<uint32_t>(size))
    {
        assert(size <= UINT_MAX);
        if (!m_data)
            ThrowHR(E_OUTOFMEMORY);
        Traits::InitializeElements(m_data, m_size);
    }

    template<typename ITERATOR>
    ComArray(ITERATOR first, ITERATOR last)
        : ComArray(std::distance(first, last))
    {
        std::copy(first, last, begin(*this));
    }

    ComArray(ComArray&& other)
        : m_data(nullptr)
        , m_size(0)
    {
        *this = std::move(other);
    }

    ComArray& operator=(ComArray&& other)
    {
        std::swap(m_data, other.m_data);
        std::swap(m_size, other.m_size);
        return *this;
    }

    ~ComArray()
    {
        Release();
    }

    uint32_t GetSize() const
    {
        return m_size;
    }

    T* GetData()
    {
        return m_data;
    }

    T const* GetData() const
    {
        return m_data;
    }

    T& operator[](uint32_t index)
    {
        assert(index < m_size);
        return m_data[index];
    }

    uint32_t* GetAddressOfSize()
    {
        Release();
        return &m_size;
    }

    typename Traits::data_ptr_t* GetAddressOfData()
    {
        Release();
        return reinterpret_cast<Traits::data_ptr_t*>(&m_data);
    }

    void Detach(uint32_t* size, typename Traits::data_ptr_t* data)
    {
        *size = m_size;
        *data = reinterpret_cast<Traits::data_ptr_t>(m_data);
        
        m_size = 0;
        m_data = nullptr;
    }

private:
    void Release()
    {
        Traits::ReleaseElements(m_data, m_size);
        CoTaskMemFree(m_data);
        m_data = nullptr;
        m_size = 0;
    }
};

template<typename T, typename Base>
inline stdext::checked_array_iterator<T const*> begin(ComArray<T, Base> const& a)
{
    return stdext::checked_array_iterator<T const*>(a.GetData(), a.GetSize());
}

template<typename T, typename Base>
inline stdext::checked_array_iterator<T*> begin(ComArray<T, Base>& a)
{
    return stdext::checked_array_iterator<T*>(a.GetData(), a.GetSize());
}

template<typename T, typename Base>
inline stdext::checked_array_iterator<T const*> end(ComArray<T, Base> const& a)
{
    return stdext::checked_array_iterator<T const*>(a.GetData(), a.GetSize(), a.GetSize());
}

template<typename T, typename Base>
inline stdext::checked_array_iterator<T*> end(ComArray<T, Base>& a)
{
    return stdext::checked_array_iterator<T*>(a.GetData(), a.GetSize(), a.GetSize());
}

//
// Creates a new array where the values are generated by passing each element
// from the source range through func.
//
template<typename T, typename ITERATOR, typename FN, typename Base=EmptyComArrayBase>
inline ComArray<T, Base> TransformToComArray(ITERATOR first, ITERATOR last, FN func)
{
    auto size = std::distance(first, last);

    ComArray<T, Base> array(size);
    std::transform(first, last, begin(array), func);

    return array;
}
