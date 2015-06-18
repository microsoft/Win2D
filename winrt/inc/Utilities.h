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

#include <windows.foundation.h>


// Compares two interface pointers, querying to IUnknown to follow COM identity rules.
template<typename T, typename U>
inline bool IsSameInstance(T* value1, U* value2)
{
    if (reinterpret_cast<void*>(value1) == reinterpret_cast<void*>(value2))
        return true; //Easy, early out, and covers null==null case

    if ((value1 == nullptr) || (value2 == nullptr))
        return false;

    Microsoft::WRL::ComPtr<IUnknown> identity1;
    Microsoft::WRL::ComPtr<IUnknown> identity2;

    ThrowIfFailed(value1->QueryInterface(IID_PPV_ARGS(identity1.GetAddressOf())));
    ThrowIfFailed(value2->QueryInterface(IID_PPV_ARGS(identity2.GetAddressOf())));

    return identity1 == identity2;
}

// Shortcut QueryInterface
template<typename T, typename U>
inline Microsoft::WRL::ComPtr<T> As(Microsoft::WRL::ComPtr<U> const& u)
{
    static_assert(!std::is_same<T, U>::value, "types should differ");

    Microsoft::WRL::ComPtr<T> t;
    ThrowIfFailed(u.As(&t));
    return t;
}

template<typename T, typename U>
inline Microsoft::WRL::ComPtr<T> As(U* u)
{
    static_assert(!std::is_same<T, U>::value, "types should differ");

    Microsoft::WRL::ComPtr<T> t;
    ThrowIfFailed(u->QueryInterface(IID_PPV_ARGS(t.ReleaseAndGetAddressOf())));
    return t;
}

#ifdef __cplusplus_winrt

template<typename T, typename U>
inline Microsoft::WRL::ComPtr<T> As(U^ u)
{
    return As<T>(reinterpret_cast<IInspectable*>(u));
}

#endif

template<typename T, typename U>
inline Microsoft::WRL::ComPtr<T> MaybeAs(Microsoft::WRL::ComPtr<U> const& u)
{
    static_assert(!std::is_same<T, U>::value, "types should differ");

    Microsoft::WRL::ComPtr<T> t;
    if (u && SUCCEEDED(u.As(&t)))
        return t;
    else
        return nullptr;
}

template<typename T, typename U>
inline Microsoft::WRL::ComPtr<T> MaybeAs(U* u)
{
    static_assert(!std::is_same<T, U>::value, "types should differ");

    Microsoft::WRL::ComPtr<T> t;
    if (u && SUCCEEDED(u->QueryInterface(IID_PPV_ARGS(t.ReleaseAndGetAddressOf()))))
        return t;
    else
        return nullptr;
}

template<typename T>
inline Microsoft::WRL::WeakRef AsWeak(T* t)
{
    Microsoft::WRL::WeakRef weakRef;
    ThrowIfFailed(AsWeak(t, &weakRef));
    return weakRef;
}

template<typename T>
inline Microsoft::WRL::ComPtr<T> LockWeakRef(Microsoft::WRL::WeakRef& weakRef)
{
    Microsoft::WRL::ComPtr<T> t;
    ThrowIfFailed(weakRef.As(&t));
    return t;
}

inline bool operator!=(ABI::Windows::Foundation::Size const& left, ABI::Windows::Foundation::Size const& right)
{
    return (left.Width != right.Width) || (left.Height != right.Height);
}

template<typename T, typename U>
inline Microsoft::WRL::ComPtr<T> GetWrappedResource(U&& wrapper)
{
    Microsoft::WRL::ComPtr<T> resource;
    ThrowIfFailed(As<ABI::Microsoft::Graphics::Canvas::ICanvasResourceWrapperNative>(wrapper)->GetResource(IID_PPV_ARGS(&resource)));
    return resource;
}
