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

#include "WinStringWrapper.h"

//
// WinStringBuilder helps building up an HSTRING that will be populated by an
// API that expects to fill a sized buffer.
//
// Example:
//
//   WinStringBuilder stringBuilder;
//   auto length = m_format->GetFontFamilyNameLength() + 1;
//   auto buffer = stringBuilder.Allocate(length);
//   ThrowIfFailed(m_format->GetFontFamilyName(buffer, length));
//   WinString theWinString = stringBuilder.Get();
//
class WinStringBuilder
{
    HSTRING_BUFFER m_hstringBuffer;

public:
    WinStringBuilder()
        : m_hstringBuffer(nullptr)
    {
    }

    ~WinStringBuilder()
    {
        if (m_hstringBuffer)
            (void)WindowsDeleteStringBuffer(m_hstringBuffer);
    }

    wchar_t* Allocate(uint32_t length)
    {
        if (m_hstringBuffer)
            ThrowHR(E_UNEXPECTED);

        wchar_t* buffer = nullptr;
        ThrowIfFailed(WindowsPreallocateStringBuffer(length, &buffer, &m_hstringBuffer));
        return buffer;
    }

    void Format(wchar_t const* format, ...)
    {
        // Disable _CRT_SECURE_NO_WARNINGS. We need to use _vsnwprintf,
        // because _vsnwprintf_s does not return the number of characters needed.
        #pragma warning(push)
        #pragma warning(disable: 4996)
        
        // Measure how much room we need.
        va_list args;
        va_start(args, format);
        auto length = _vsnwprintf(nullptr, 0, format, args);
        va_end(args);

        // Allocate the buffer.
        auto buffer = Allocate(length);

        // Format the string.
        va_start(args, format);
        _vsnwprintf(buffer, length, format, args);
        va_end(args);

        #pragma warning(pop)
    }

    WinString Get()
    {
        if (!m_hstringBuffer)
            ThrowHR(E_UNEXPECTED);

        WinString value;
        ThrowIfFailed(WindowsPromoteStringBuffer(m_hstringBuffer, value.GetAddressOf()));

        //
        // If the string has embedded nulls then we almost certainly don't
        // want to keep them around.  This can happen when, eg,
        // IDWriteTextFormat::GetFontFamilyName has to supply the
        // null-terminator on a string, while WindowsPreallocateStringBuffer
        // will include the null-terminator as part of the string.
        //

        if (value.HasEmbeddedNull())
            value = value.GetCopyWithoutEmbeddedNull();

        m_hstringBuffer = nullptr;

        return value;
    }


    WinStringBuilder(WinStringBuilder const&) = delete;
    WinStringBuilder& operator=(WinStringBuilder const&) = delete;
    WinStringBuilder(WinStringBuilder&&) = delete;
    WinStringBuilder& operator=(WinStringBuilder&&) = delete;        
};
