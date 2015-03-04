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

#include "pch.h"

class CountWinStringBase
{
    static std::map<HSTRING, int>* s_strings;

public:
    static void Init(std::map<HSTRING, int>* strings)
    {
        s_strings = strings;
    }

    static void Check()
    {
        Assert::IsTrue(s_strings->empty());
    }

    int RefCount()
    {
        auto derivedThis = static_cast<WinStringT<CountWinStringBase>*>(this);
        HSTRING hstring = derivedThis->operator HSTRING();

        auto it = s_strings->find(hstring);
        if (it == s_strings->end())
            return 0;
        else
            return it->second;
    }

    //
    // These static functions will be picked up by WinStringT and so replace the
    // global functions of the same name.
    //

    static HRESULT WindowsCreateString(const wchar_t* str, uint32_t length, HSTRING* value)
    {
        HRESULT hr = ::WindowsCreateString(str, length, value);

        if (SUCCEEDED(hr))
            (*s_strings)[*value] = 1;

        return hr;
    }

    static HRESULT WindowsDuplicateString(HSTRING s, HSTRING* value)
    {
        HRESULT hr = ::WindowsDuplicateString(s, value);

        if (SUCCEEDED(hr))
        {
            Assert::AreEqual<void*>(s, *value);
            Assert::IsTrue(s_strings->find(s) != s_strings->end());        
            (*s_strings)[s]++;
        }

        return hr;
    }

    static HRESULT WindowsDeleteString(HSTRING s)
    {
        HRESULT hr = ::WindowsDeleteString(s);

        if (SUCCEEDED(hr))
        {
            Assert::IsTrue(s_strings->find(s) != s_strings->end());
            (*s_strings)[s]--;

            if ((*s_strings)[s] == 0)
                s_strings->erase(s);
        }

        return hr;
    }
};

typedef WinStringT<CountWinStringBase> CountWinString;

//
// In order to avoid having a global std::map, we arrange it so that the map is
// owned by the test class.  Then we just have a global pointer that we point to
// the map.  This avoids issues with memory deallocation during shutdown.
//
std::map<HSTRING, int>* CountWinStringBase::s_strings = nullptr;

TEST_CLASS(WinStringTests)
{
    std::map<HSTRING, int> m_strings;

    TEST_METHOD_INITIALIZE(Init)
    {
        m_strings.clear();
        CountWinString::Init(&m_strings);
    }

    TEST_METHOD_CLEANUP(Cleanup)
    {
        CountWinString::Check();
    }

    TEST_METHOD_EX(WinString_SimpleCreateDelete)
    {
        CountWinString s(L"hello");
        Assert::AreEqual(1, s.RefCount());
    }

    TEST_METHOD_EX(WinString_CreateFromWstring)
    {
        CountWinString s(std::wstring(L"hello"));
        Assert::AreEqual(1, s.RefCount());
        Assert::AreEqual<std::wstring>(L"hello", static_cast<const wchar_t*>(s));
    }

    TEST_METHOD_EX(WinString_CreateFromHSTRING)
    {
        CountWinString s1(L"hello");
        Assert::AreEqual(1, s1.RefCount());

        CountWinString s2(static_cast<HSTRING>(s1));
        Assert::AreEqual(2, s1.RefCount());
        Assert::AreEqual(2, s2.RefCount());
    }

    TEST_METHOD_EX(WinString_CopyConstructor)
    {
        CountWinString s1(L"hello");
        Assert::AreEqual(1, s1.RefCount());

        CountWinString s2(s1);
        Assert::AreEqual(2, s1.RefCount());
        Assert::AreEqual(2, s2.RefCount());
    }

    TEST_METHOD_EX(WinString_MoveConstructor)
    {
        CountWinString s1(L"hello");
        Assert::AreEqual(1, s1.RefCount());

        CountWinString s2(std::move(s1));
        Assert::AreEqual(0, s1.RefCount());
        Assert::AreEqual(1, s2.RefCount());        
    }

    TEST_METHOD_EX(WinString_Assignment)
    {
        CountWinString s1(L"hello");
        CountWinString s2(L"goodbye");
        CountWinString s3(s1);

        s1 = s2;

        Assert::AreEqual<std::wstring>(L"goodbye", static_cast<const wchar_t*>(s1));
        Assert::AreEqual<std::wstring>(L"goodbye", static_cast<const wchar_t*>(s2));
        Assert::AreEqual<std::wstring>(L"hello",   static_cast<const wchar_t*>(s3));

        Assert::AreEqual(2, s1.RefCount());
        Assert::AreEqual(2, s2.RefCount());
        Assert::AreEqual(1, s3.RefCount());
    }

    TEST_METHOD_EX(WinString_AssignToHSTRING)
    {
        CountWinString s1(L"hello");
        CountWinString s2(s1);
        CountWinString s3(L"goodbye");

        Assert::AreEqual(2, s1.RefCount());
        Assert::AreEqual(2, s2.RefCount());
        Assert::AreEqual(1, s3.RefCount());

        s2 = static_cast<HSTRING>(s3);

        Assert::AreEqual(1, s1.RefCount());
        Assert::AreEqual(2, s2.RefCount());
        Assert::AreEqual(2, s3.RefCount());
    }

    TEST_METHOD_EX(WinString_MoveAssignment)
    {
        CountWinString s1(L"hello");
        CountWinString s2(L"goodbye");
        CountWinString s3(s1);

        // When moving we don't want to make any assertions about what happens
        // to the thing we moved from.  However, we do care that no ref counts
        // have increased.

        int helloRefCount = s1.RefCount();
        int goodbyeRefCount = s2.RefCount();

        s1 = std::move(s2);

        Assert::AreEqual<std::wstring>(L"goodbye", static_cast<const wchar_t*>(s1));
        Assert::AreEqual<std::wstring>(L"hello",   static_cast<const wchar_t*>(s3));

        Assert::IsTrue(s1.RefCount() <= goodbyeRefCount);
        Assert::IsTrue(s3.RefCount() <= helloRefCount);
    }

    TEST_METHOD_EX(WinString_GetAddressOf)
    {
        CountWinString s(L"hello");

        Assert::AreEqual(1, s.RefCount());

        Assert::IsNotNull(s.GetAddressOf());

        Assert::AreEqual(0, s.RefCount());
    }

    TEST_METHOD_EX(WinString_CopyTo)
    {
        CountWinString s1(L"hello");

        CountWinString s2;
        s1.CopyTo(s2.GetAddressOf());

        Assert::AreEqual(2, s1.RefCount());        
        Assert::AreEqual(2, s2.RefCount());        
    }

    TEST_METHOD_EX(WinString_Equals)
    {
        CountWinString s1(L"hello");
        CountWinString s2(L"hello");

        Assert::AreEqual(1, s1.RefCount());
        Assert::AreEqual(1, s2.RefCount());
        Assert::IsTrue(s1.Equals(s2));            
    }

    TEST_METHOD_EX(WinString_EmbeddedNulls)
    {
        CountWinString s1;
        ThrowIfFailed(CountWinString::WindowsCreateString(L"hello\0", 6, s1.GetAddressOf()));

        CountWinString s2(L"hello");

        Assert::IsTrue(s1.HasEmbeddedNull());
        Assert::IsFalse(s2.HasEmbeddedNull());
        Assert::IsFalse(s1.Equals(s2));

        s1 = s1.GetCopyWithoutEmbeddedNull();

        Assert::IsFalse(s1.HasEmbeddedNull());
        Assert::IsTrue(s1.Equals(s2));
    }

    TEST_METHOD_EX(WinString_BeginEnd)
    {
        std::wstring str(L"hello world");
        WinString s(str);

        auto b = begin(s);
        auto e = end(s);

        Assert::AreEqual(std::distance(str.begin(), str.end()), std::distance(b, e));
        Assert::IsTrue(std::equal(b, e, str.begin()));        
    }

    TEST_METHOD_EX(WinString_ConstructFromIterators)
    {
        WinString str(L"hello world");
        auto it = std::find(begin(str), end(str), L' ');

        WinString s1(begin(str), it);
        WinString s2(it, end(str));

        Assert::AreEqual(L"hello", static_cast<wchar_t const*>(s1));
        Assert::AreEqual(L" world", static_cast<wchar_t const*>(s2));
    }
};

