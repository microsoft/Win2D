// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"
#include "../lib/utils/HashUtilities.h"

using namespace ABI::Microsoft::Graphics::Canvas;

TEST_CLASS(HashUtilitiesTests)
{
    TEST_METHOD_EX(Version5UuidTest)
    {
        const BYTE name1[] = { 'H', 'e', 'l', 'l', 'o' };
        const BYTE name2[] = { 'W', 'o', 'r', 'l', 'd' };

        static const IID salt1{ 0xA911588C, 0xDB0A, 0x41D2, 0xAF, 0x64, 0xEB, 0xEC, 0x03, 0x72, 0x94, 0xD0 };
        static const IID salt2{ 0x88B56025, 0x60BC, 0x4FE9, 0xAA, 0x3F, 0x11, 0x48, 0xBD, 0x0C, 0x65, 0x3E };

        IID result11 = GetVersion5Uuid(salt1, name1, sizeof(name1));
        IID result12 = GetVersion5Uuid(salt1, name2, sizeof(name2));
        IID result21 = GetVersion5Uuid(salt2, name1, sizeof(name1));
        IID result22 = GetVersion5Uuid(salt2, name2, sizeof(name2));

        // Validate against a hard-coded expected result.
        static const IID expected{ 0xC3091B3B, 0x5718, 0x5928, 0x8C, 0xBF, 0x41, 0x29, 0x8E, 0xF7, 0xC6, 0x14 };

        Assert::AreEqual(expected, result11);

        // All results should be different (changing either name or salt alters the UUID).
        Assert::AreNotEqual(result11, result12);
        Assert::AreNotEqual(result11, result21);
        Assert::AreNotEqual(result11, result22);
        Assert::AreNotEqual(result12, result21);
        Assert::AreNotEqual(result12, result22);
        Assert::AreNotEqual(result21, result22);

        // Hashing the same inputs a second time should give the same results as before.
        Assert::AreEqual(result11, GetVersion5Uuid(salt1, name1, sizeof(name1)));
        Assert::AreEqual(result12, GetVersion5Uuid(salt1, name2, sizeof(name2)));
        Assert::AreEqual(result21, GetVersion5Uuid(salt2, name1, sizeof(name1)));
        Assert::AreEqual(result22, GetVersion5Uuid(salt2, name2, sizeof(name2)));

        // All should be marked as standard RFC 4122 UUID variant.
        Assert::AreEqual(2, GetUuidVariant(result11));
        Assert::AreEqual(2, GetUuidVariant(result12));
        Assert::AreEqual(2, GetUuidVariant(result21));
        Assert::AreEqual(2, GetUuidVariant(result22));

        // All should be marked as version 5 (SHA-1 name-based) UUID.
        Assert::AreEqual(5, GetUuidVersion(result11));
        Assert::AreEqual(5, GetUuidVersion(result12));
        Assert::AreEqual(5, GetUuidVersion(result21));
        Assert::AreEqual(5, GetUuidVersion(result22));

        // Sanity check our GetUuidVariant and GetUUidVersion methods against standard (version 4) UUIDs.
        Assert::AreEqual(2, GetUuidVariant(salt1));
        Assert::AreEqual(2, GetUuidVariant(salt2));

        Assert::AreEqual(4, GetUuidVersion(salt1));
        Assert::AreEqual(4, GetUuidVersion(salt2));
    }


    static int GetUuidVariant(IID const& uuid)
    {
        return (uuid.Data4[0] & 0xC0) >> 6;
    }


    static int GetUuidVersion(IID const& uuid)
    {
        return (uuid.Data3 & 0xF000) >> 12;
    }
};
