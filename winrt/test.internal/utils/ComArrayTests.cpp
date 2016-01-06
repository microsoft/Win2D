// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

static int stringCreateCount;
static int stringDuplicateCount;
static int stringDeleteCount;

TEST_CLASS(ComArrayTests)
{
    class Tracker
    {
    public:
        static void* CoTaskMemAlloc(size_t bytes)
        {
            auto p = ::CoTaskMemAlloc(bytes);
            GetAllocations()->insert(p);
            return p;
        }
        
        static void CoTaskMemFree(void* p)
        {
            if (p)
            {
                Assert::AreEqual<size_t>(1U, GetAllocations()->count(p));
                GetAllocations()->erase(p);
            }
            ::CoTaskMemFree(p);
        }

        static void Reset()
        {
            GetAllocations()->clear();
        }

        static void Verify()
        {
            Assert::IsTrue(GetAllocations()->empty(), L"CoTaskMem leak detected");
        }

    private:
        static std::set<void*>* GetAllocations()
        {
            static std::set<void*> allocations;
            return &allocations;
        }
    };

    TEST_METHOD_INITIALIZE(Initialize)
    {
        Tracker::Reset();
    }

    TEST_METHOD_CLEANUP(Cleanup)
    {
        Tracker::Verify();
    }

    TEST_METHOD_EX(ComArray_ZeroSizedArray_HasNonNullData)
    {
        ComArray<float, Tracker> array(0);

        Assert::AreEqual(0U, array.GetSize());
        Assert::IsNotNull(array.GetData());
    }

    TEST_METHOD_EX(ComArray_SizedArray)
    {
        ComArray<float, Tracker> array(100);
        Assert::AreEqual(100U, array.GetSize());
        Assert::IsNotNull(array.GetData());
    }

    TEST_METHOD_EX(ComArray_ArraySubscriptOperator)
    {
        ComArray<float, Tracker> array(100);

        Assert::AreEqual(array.GetData(), &array[0]);
        Assert::AreEqual(array.GetData() + 1, &array[1]);
    }

    TEST_METHOD_EX(ComArray_MoveConstructor)
    {
        ComArray<float, Tracker> sourceArray(100);
        auto originalData = sourceArray.GetData();

        ComArray<float, Tracker> destArray(std::move(sourceArray));

        Assert::AreEqual(0U, sourceArray.GetSize());
        Assert::IsNull(sourceArray.GetData());

        Assert::AreEqual(100U, destArray.GetSize());
        Assert::AreEqual(originalData, destArray.GetData());
    }

    TEST_METHOD_EX(ComArray_MoveAssignment)
    {
        ComArray<float, Tracker> sourceArray(100);
        auto originalData = sourceArray.GetData();

        ComArray<float, Tracker> destArray;
        destArray = std::move(sourceArray);

        Assert::AreEqual(0U, sourceArray.GetSize());
        Assert::IsNull(sourceArray.GetData());

        Assert::AreEqual(100U, destArray.GetSize());
        Assert::AreEqual(originalData, destArray.GetData());
    }

    TEST_METHOD_EX(ComArray_Detach)
    {
        ComArray<float, Tracker> array(100);

        uint32_t size;
        float* data;
        array.Detach(&size, &data);

        Assert::AreEqual(100U, size);
        Tracker::CoTaskMemFree(data);
    }

    TEST_METHOD_EX(ComArray_GetAddressOfData_ReleasesAndTracksNewValue)
    {
        ComArray<float, Tracker> array(100);
        *array.GetAddressOfData() = static_cast<float*>(Tracker::CoTaskMemAlloc(1));

        // For this test to pass, the memory allocated by the constructor must
        // have been freed and the memory assigned in the above line must also
        // have been freed.
    }

    TEST_METHOD_EX(ComArray_ConstructFromIterators_Empty)
    {
        std::vector<float> src;
        ComArray<float, Tracker> array(src.begin(), src.end());
        Assert::AreEqual(0U, array.GetSize());
        Assert::IsNotNull(array.GetData());
    }

    TEST_METHOD_EX(CopyToComArray_WithData)
    {
        std::vector<float> src{ 1, 2, 3 };

        ComArray<float, Tracker> array(src.begin(), src.end());

        Assert::AreEqual(3U, array.GetSize());
        auto d = array.GetData();
        Assert::AreEqual(1.0f, d[0]);
        Assert::AreEqual(2.0f, d[1]);
        Assert::AreEqual(3.0f, d[2]);
    }

    TEST_METHOD_EX(TransformToComArray_Empty)
    {
        auto transformFn = [](int v) { Assert::Fail(L"Don't expect transformFn to get called"); return 0.0; };
        std::vector<int> src;

        auto array = TransformToComArray<double, decltype(src.begin()), decltype(transformFn), Tracker>(src.begin(), src.end(), transformFn);
        
        Assert::AreEqual(0U, array.GetSize());
        Assert::IsNotNull(array.GetData());
    }

    TEST_METHOD_EX(TransformToComArray_WithData)
    {
        auto transformFn = [](int v) { return static_cast<double>(v+1.0); };
        std::vector<int> src{ 1, 2, 3 };

        auto array = TransformToComArray<double, decltype(src.begin()), decltype(transformFn), Tracker>(src.begin(), src.end(), transformFn);
        
        Assert::AreEqual(3U, array.GetSize());
        auto d = array.GetData();
        Assert::AreEqual(2.0, d[0]);
        Assert::AreEqual(3.0, d[1]);
        Assert::AreEqual(4.0, d[2]);
    }

    TEST_METHOD_EX(ComArray_ReleasesInterfacePointers)
    {
        struct MockInterface
        {
            MockInterface()
                : refCount(1)
            { }

            DWORD AddRef() { return ++refCount; }
            DWORD Release() { return --refCount; };

            int refCount;
        };

        MockInterface a, b;
        MockInterface* values[3] = { &a, &b, nullptr };

        {
            // Constructor addrefs.
            ComArray<ComPtr<MockInterface>> array(std::begin(values), std::end(values));

            Assert::AreEqual(2, a.refCount);
            Assert::AreEqual(2, b.refCount);
        }

        // Destructor releases.
        Assert::AreEqual(1, a.refCount);
        Assert::AreEqual(1, b.refCount);

        {
            // Size based ctor doesn't try to addref nulls.
            ComArray<ComPtr<MockInterface>> array(2);

            array[0] = &a;
            Assert::AreEqual(2, a.refCount);

            // GetAddressOfData releases.
            auto address = array.GetAddressOfData();
            auto size = array.GetAddressOfSize();

            Assert::AreEqual(1, a.refCount);

            // Emulate a COM API that returns an array of interface pointers.
            auto mem = reinterpret_cast<MockInterface**>(::CoTaskMemAlloc(sizeof(MockInterface*) * 2));

            mem[0] = &a;
            mem[1] = &b;

            *address = mem;
            *size = 2;

            Assert::AreEqual(1, a.refCount);
            Assert::AreEqual(1, b.refCount);

            // Detach does not release the interfaces.
            uint32_t sizeOut;
            MockInterface** memOut;

            array.Detach(&sizeOut, &memOut);

            Assert::AreEqual<void*>(mem, memOut);
            ::CoTaskMemFree(mem);
        }

        Assert::AreEqual(1, a.refCount);
        Assert::AreEqual(1, b.refCount);
    }

    TEST_METHOD_EX(ComArray_ReleasesStrings)
    {
        struct CountStrings
        {
            static HRESULT WindowsCreateString(const wchar_t* str, uint32_t length, HSTRING* value)
            {
                Assert::IsTrue(str && *str);
                stringCreateCount++;
                return ::WindowsCreateString(str, length, value);
            }

            static HRESULT WindowsDuplicateString(HSTRING s, HSTRING* value)
            {
                if (s)
                    stringDuplicateCount++;

                return ::WindowsDuplicateString(s, value);
            }

            static HRESULT WindowsDeleteString(HSTRING s)
            {
                stringDeleteCount++;
                return ::WindowsDeleteString(s);
            }
        };

        stringCreateCount = 0;
        stringDuplicateCount = 0;
        stringDeleteCount = 0;

        typedef WinStringT<CountStrings> CountedString;

        {
            CountedString values[3] = { CountedString(L"hello"), CountedString(L"world"), CountedString() };

            Assert::AreEqual(2, stringCreateCount);
            Assert::AreEqual(0, stringDuplicateCount);
            Assert::AreEqual(0, stringDeleteCount);

            {
                // Constructor duplicates.
                ComArray<CountedString> array(std::begin(values), std::end(values));

                Assert::AreEqual(2, stringCreateCount);
                Assert::AreEqual(2, stringDuplicateCount);
                Assert::AreEqual(0, stringDeleteCount);
            }

            // Destructor deletes.
            Assert::AreEqual(2, stringCreateCount);
            Assert::AreEqual(2, stringDuplicateCount);
            Assert::AreEqual(2, stringDeleteCount);

            HSTRING* mem;
            HSTRING* memOut;
            uint32_t sizeOut;

            {
                // Size based ctor doesn't try to duplicate nulls.
                ComArray<CountedString> array(2);

                array[0] = CountedString(L"hello");

                Assert::AreEqual(3, stringCreateCount);
                Assert::AreEqual(2, stringDuplicateCount);
                Assert::AreEqual(2, stringDeleteCount);

                // GetAddressOfData deletes.
                auto address = array.GetAddressOfData();
                auto size = array.GetAddressOfSize();

                Assert::AreEqual(3, stringCreateCount);
                Assert::AreEqual(2, stringDuplicateCount);
                Assert::AreEqual(3, stringDeleteCount);

                // Emulate a COM API that returns an array of interface pointers.
                mem = reinterpret_cast<HSTRING*>(::CoTaskMemAlloc(sizeof(HSTRING) * 2));

                CountedString(L"hello").CopyTo(&mem[0]);
                CountedString(L"world").CopyTo(&mem[1]);

                *address = mem;
                *size = 2;

                Assert::AreEqual(5, stringCreateCount);
                Assert::AreEqual(4, stringDuplicateCount);
                Assert::AreEqual(5, stringDeleteCount);

                // Detach does not release the interfaces.
                array.Detach(&sizeOut, &memOut);
            }

            Assert::AreEqual(5, stringCreateCount);
            Assert::AreEqual(4, stringDuplicateCount);
            Assert::AreEqual(5, stringDeleteCount);

            Assert::AreEqual<void*>(mem, memOut);
            CountStrings::WindowsDeleteString(mem[0]);
            CountStrings::WindowsDeleteString(mem[1]);
            ::CoTaskMemFree(mem);
        }

        Assert::AreEqual(5, stringCreateCount);
        Assert::AreEqual(4, stringDuplicateCount);
        Assert::AreEqual(9, stringDeleteCount);
    }
};
