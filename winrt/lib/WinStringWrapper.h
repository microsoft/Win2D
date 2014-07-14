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

#include <winstring.h>

#include <ErrorHandling.h>

namespace canvas
{
    //
    // Wrapper around HSTRING that uses exceptions for error handling so that it
    // can nicely support copying and assignment.
    //
    // Note: we can't call this file "WinString.h" because that will conflict
    // with the system header of the same name.
    //
    // TODO #1675: tests for this to convince ourselves that we can't leak
    // strings.
    //
    class WinString
    {
        HSTRING m_value;

    public:
        WinString()
            : m_value(nullptr)
        {
        }

        explicit WinString(const wchar_t* str)
        {
            auto length = wcslen(str);
            ThrowIfFailed(WindowsCreateString(str, static_cast<uint32_t>(length), &m_value));
        }

        explicit WinString(const std::wstring& str)
            : WinString(str.c_str())
        {}

        WinString(const WinString& other)
            : m_value(nullptr)
        {
            other.CopyTo(GetAddressOf());
        }

        WinString(WinString&& other)
            : m_value(nullptr)
        {
            std::swap(m_value, other.m_value);
        }

        ~WinString()
        {
            Release();
        }

        WinString& operator=(const wchar_t* str)
        {
            return operator=(WinString(str));
        }

        WinString& operator=(HSTRING str)
        {
            Release();
            ThrowIfFailed(WindowsDuplicateString(str, &m_value));
            return *this;
        }

        WinString& operator=(const WinString& other)
        {
            return operator=(other.m_value);
        }

        WinString& operator=(WinString&& other)
        {
            std::swap(m_value, other.m_value);
            return *this;
        }

        void Release()
        {
            if (m_value)
            {
                WindowsDeleteString(m_value);
                m_value = nullptr;
            }
        }

        HSTRING* GetAddressOf()
        {
            Release();
            return &m_value;
        }

        void CopyTo(HSTRING* dest) const
        {
            ThrowIfFailed(WindowsDuplicateString(m_value, dest));
        }


        explicit operator const wchar_t*() const
        {
            return WindowsGetStringRawBuffer(m_value, nullptr);
        }

        operator HSTRING() const
        {
            return m_value;
        }

        bool Equals(HSTRING other) const
        {
            int32_t result{};
            ThrowIfFailed(WindowsCompareStringOrdinal(m_value, other, &result));
            return (result == 0);
        }
    };
}

