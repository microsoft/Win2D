// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

template<typename TEXT_OBJECT>
void TrimmingDelimiterValidationTest(TEXT_OBJECT textObject)
{
    //
    // The characters passed as trimming delimiters to CanvasTextFormat and
    // CanvasTextLayout are encoded as UTF-16. DWrite deals in UTF-32 values.
    //
    // If the delimiter character passed to Win2D is a one- or two- byte
    // UTF-16 value, then it can be simply reinterpreted. If the 
    // code point falls above U+FFFF, however, then it comprises two
    // UTF-16 code units and needs a proper UTF-32 conversion. This test 
    // validates that reinterpreting/conversion.
    //

    wchar_t const* validTestCases[] = {
        L"", // Zero length

        L"A", // ASCII
        L"\\",

        L"♬", // One code unit, non-ASCII
        L"文",

        L"🌆", // Characters that comprise two UTF-16 code units
        L"🎁",
        L"🆗"
    };

    for (auto testCase : validTestCases)
    {
        Assert::AreEqual(S_OK, textObject->put_TrimmingDelimiter(WinString(testCase)));

        HSTRING actual;
        Assert::AreEqual(S_OK, textObject->get_TrimmingDelimiter(&actual));

        Assert::AreEqual(testCase, WindowsGetStringRawBuffer(actual, nullptr));
    }

    //
    // These stored this way rather than an escaped string literal 
    // because the compiler validates invalid Unicode characters 
    // within string literals.
    //
    wchar_t invalidTestCases[][3] = {

        0xD800, 0xDBFF, 0x0, // Lower surrogate is not in range
        0xD800, 0xE000, 0x0,
        0xDBFF, 0xDBFF, 0x0,
        0xDBFF, 0xE000, 0x0,

        0xD7FF, 0xDC00, 0x0, // Upper surrogate is not in range
        0xDC00, 0xDC00, 0x0,
        0xD7FF, 0xDFFF, 0x0,
        0xDC00, 0xDFFF, 0x0,

        0xDC0D, 0xE000, 0x0, // Neither are in range
    };

    for (auto testCase : invalidTestCases)
    {
        Assert::AreEqual(E_INVALIDARG, textObject->put_TrimmingDelimiter(WinString(testCase)));
    }

    // 
    // Seemingly-well-formed strings that don't map to two code units should fail.
    //
    Assert::AreEqual(E_INVALIDARG, textObject->put_TrimmingDelimiter(WinString(L"01")));
    Assert::AreEqual(E_INVALIDARG, textObject->put_TrimmingDelimiter(WinString(L"123")));
    Assert::AreEqual(E_INVALIDARG, textObject->put_TrimmingDelimiter(WinString(L"1234")));
}