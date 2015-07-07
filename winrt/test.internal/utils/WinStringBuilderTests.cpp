// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

TEST_CLASS(WinStringBuilderTests)
{
    TEST_METHOD_EX(WinStringBuilder_Allocate)
    {
        WinStringBuilder builder;

        // Build a string.
        auto buffer = builder.Allocate(4);
        Assert::IsNotNull(buffer);

        buffer[0] = 't';
        buffer[1] = 'e';
        buffer[2] = 's';
        buffer[3] = 't';

        // Can't build other strings while this one is pending.
        ExpectHResultException(E_UNEXPECTED, [&] { builder.Allocate(4); });
        ExpectHResultException(E_UNEXPECTED, [&] { builder.Format(L"Test"); });

        // Read back the string.
        auto result = builder.Get();

        Assert::IsFalse(result.HasEmbeddedNull());
        Assert::AreEqual(0, wcscmp(L"test", (wchar_t const*)result));

        // Can't read it back twice.
        ExpectHResultException(E_UNEXPECTED, [&] { builder.Get(); });

        // After we read back the first string, can now build a new one. Let's try containing an embedded null.
        buffer = builder.Allocate(5);
        Assert::IsNotNull(buffer);

        buffer[0] = 't';
        buffer[1] = 'e';
        buffer[2] = 0;
        buffer[3] = 's';
        buffer[4] = 't';

        result = builder.Get();

        Assert::IsFalse(result.HasEmbeddedNull());
        Assert::AreEqual(0, wcscmp(L"te", static_cast<wchar_t const*>(result)));
    }


    TEST_METHOD_EX(WinStringBuilder_Format)
    {
        WinStringBuilder builder;

        // Format a string.
        builder.Format(L"There are %d %s in the %s.", 23, L"cats", L"refrigerator");

        // Can't build other strings while this one is pending.
        ExpectHResultException(E_UNEXPECTED, [&] { builder.Allocate(4); });
        ExpectHResultException(E_UNEXPECTED, [&] { builder.Format(L"Test"); });

        // Read back the string.
        auto result = builder.Get();

        Assert::IsFalse(result.HasEmbeddedNull());
        Assert::AreEqual(0, wcscmp(L"There are 23 cats in the refrigerator.", static_cast<wchar_t const*>(result)));
    }
};
