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


// Compares two interface pointers, querying to IUnknown to follow COM identity rules.
template<typename T, typename U>
inline bool IsSameInstance(T* value1, U* value2)
{
    ComPtr<IUnknown> identity1;
    ComPtr<IUnknown> identity2;

    ThrowIfFailed(value1->QueryInterface(IID_PPV_ARGS(identity1.GetAddressOf())));
    ThrowIfFailed(value2->QueryInterface(IID_PPV_ARGS(identity2.GetAddressOf())));

    return identity1 == identity2;
}

// Shortcut QueryInterface
template<typename T, typename U>
inline Microsoft::WRL::ComPtr<T> As(Microsoft::WRL::ComPtr<U> const& u)
{
    ComPtr<T> t;
    ThrowIfFailed(u.As(&t));
    return t;
}

template<typename T, typename U>
inline Microsoft::WRL::ComPtr<T> As(U* u)
{
    ComPtr<T> t;
    ThrowIfFailed(u->QueryInterface(IID_PPV_ARGS(t.ReleaseAndGetAddressOf())));
    return t;
}
