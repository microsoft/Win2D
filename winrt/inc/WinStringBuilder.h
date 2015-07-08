// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

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
