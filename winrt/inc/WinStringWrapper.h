// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#include <winstring.h>

#include "ErrorHandling.h"

//
// Wrapper around HSTRING that uses exceptions for error handling so that it
// can nicely support copying and assignment.
//
// Note: we can't call this file "WinString.h" because that will conflict
// with the system header of the same name.
//


// For testing purposes, WinStringT is templated on a base class.  This
// allows tests to replace the various Windows APIs (eg WindowsCreateString)
// with test doubles.
struct EmptyWinStringBase {};

template<typename Base=EmptyWinStringBase>
class WinStringT : public Base
{
    HSTRING m_value;

public:
    WinStringT()
        : m_value(nullptr)
    {
    }

    WinStringT(std::nullptr_t)
        : m_value(nullptr)
    {
    }

    explicit WinStringT(HSTRING str)
    {
        ThrowIfFailed(WindowsDuplicateString(str, &m_value));
    }

    explicit WinStringT(wchar_t const* str)
    {
        auto length = wcslen(str);
        ThrowIfFailed(WindowsCreateString(str, static_cast<uint32_t>(length), &m_value));
    }

    explicit WinStringT(std::wstring const& str)
        : WinStringT(str.c_str())
    {}

    WinStringT(wchar_t const* begin, wchar_t const* end)
    {
        auto length = std::distance(begin, end);
        ThrowIfFailed(WindowsCreateString(begin, static_cast<uint32_t>(length), &m_value));
    }

    WinStringT(WinStringT const& other)
        : m_value(nullptr)
    {
        other.CopyTo(GetAddressOf());
    }

    WinStringT(WinStringT&& other)
        : m_value(nullptr)
    {
        std::swap(m_value, other.m_value);
    }

    ~WinStringT()
    {
        Release();
    }

    WinStringT& operator=(wchar_t const* str)
    {
        return operator=(WinString(str));
    }

    WinStringT& operator=(HSTRING str)
    {
        Release();
        ThrowIfFailed(WindowsDuplicateString(str, &m_value));
        return *this;
    }

    WinStringT& operator=(WinStringT const& other)
    {
        return operator=(other.m_value);
    }

    WinStringT& operator=(WinStringT&& other)
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

    explicit operator wchar_t const*() const
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

    bool HasEmbeddedNull() const
    {
        BOOL hasEmbeddedNull = FALSE;
        ThrowIfFailed(WindowsStringHasEmbeddedNull(m_value, &hasEmbeddedNull));

        return !!hasEmbeddedNull;
    }

    WinStringT GetCopyWithoutEmbeddedNull() const
    {
        // Recreate the string from the c-style string.  This will only copy
        // up to the first null character.
        return WinStringT(static_cast<wchar_t const*>(*this));
    }

    friend wchar_t const* begin(WinStringT const& s)
    {
        return static_cast<wchar_t const*>(s);
    }

    friend wchar_t const* end(WinStringT const& s)
    {
        uint32_t length = 0;
        auto buf = WindowsGetStringRawBuffer(s.m_value, &length);
        return buf + length;
    }
};

typedef WinStringT<> WinString;

// Helper method for quickly getting a string buffer and validating it's not zero-length
inline wchar_t const* GetStringBuffer(HSTRING string, uint32_t* outputLength = nullptr)
{
    uint32_t textLength;
    auto textBuffer = WindowsGetStringRawBuffer(string, &textLength);
    ThrowIfNullPointer(textBuffer, E_INVALIDARG);

    if (outputLength)
        *outputLength = textLength;

    return textBuffer;
}

