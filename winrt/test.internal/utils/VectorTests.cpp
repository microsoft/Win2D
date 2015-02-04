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

using namespace collections;


namespace
{
    // Fake WinRT interface, used as a collection element type to make sure it gets refcounted properly.
    class MockInterface
    {
    public:
        MockInterface()
            : refCount(1)
        { }

        DWORD AddRef()  { return ++refCount; }
        DWORD Release() { return --refCount; };

        int refCount;
    };


    // Fake WinRT runtime class implementation.
    class MockRuntimeClass : public MockInterface
    {
    };
}


namespace Microsoft {
    namespace VisualStudio {
        namespace CppUnitTestFramework
        {
            template<> inline std::wstring ToString<MockInterface>(MockInterface* value)
            {
                return PointerToString(L"MockInterface", value);
            }
        }
    }
}


// Fake interface specializations to make WRL happy.
// In a real app, MIDL would generate a more complete version of this.
namespace ABI {
    namespace Windows {
        namespace Foundation {
            namespace Collections
            {
#if (WINVER <= 0x0603)
                template<> struct __declspec(uuid("b939af5b-b45d-5489-9149-61442c1905fe")) IVector    <int> : IVector_impl    <int> { static const wchar_t* z_get_rc_name_impl() { return L""; } };
                template<> struct __declspec(uuid("8d720cdf-3934-5d3f-9a55-40e8063b086a")) IVectorView<int> : IVectorView_impl<int> { static const wchar_t* z_get_rc_name_impl() { return L""; } };
                template<> struct __declspec(uuid("81a643fb-f51c-5565-83c4-f96425777b66")) IIterable  <int> : IIterable_impl  <int> { static const wchar_t* z_get_rc_name_impl() { return L""; } };
                template<> struct __declspec(uuid("bfea7f78-50c2-5f1d-a6ea-9e978d2699ff")) IIterator  <int> : IIterator_impl  <int> { static const wchar_t* z_get_rc_name_impl() { return L""; } };
#endif

                template<> struct __declspec(uuid("b6a5841a-795a-4579-bd7c-94f160faf3fd")) IVector    <MockInterface*> : IVector_impl    <MockInterface*> { static const wchar_t* z_get_rc_name_impl() { return L""; } };
                template<> struct __declspec(uuid("85f9e708-40a5-452e-949a-0caab9bf2e7c")) IVectorView<MockInterface*> : IVectorView_impl<MockInterface*> { static const wchar_t* z_get_rc_name_impl() { return L""; } };
                template<> struct __declspec(uuid("e84e0e0c-a8f7-4fbe-ad49-d7dbd4e5cf49")) IIterable  <MockInterface*> : IIterable_impl  <MockInterface*> { static const wchar_t* z_get_rc_name_impl() { return L""; } };
                template<> struct __declspec(uuid("a4af48ab-a5c0-49e4-9d92-704471459f93")) IIterator  <MockInterface*> : IIterator_impl  <MockInterface*> { static const wchar_t* z_get_rc_name_impl() { return L""; } };

                template<> struct __declspec(uuid("662ce162-4e9b-489f-9920-21cdacf81244")) IVector    <MockRuntimeClass*> : IVector_impl    <ABI::Windows::Foundation::Internal::AggregateType<MockRuntimeClass*, MockInterface*>> { static const wchar_t* z_get_rc_name_impl() { return L""; } };
                template<> struct __declspec(uuid("5164aa0c-ade9-42af-a620-5628369e26d1")) IVectorView<MockRuntimeClass*> : IVectorView_impl<ABI::Windows::Foundation::Internal::AggregateType<MockRuntimeClass*, MockInterface*>> { static const wchar_t* z_get_rc_name_impl() { return L""; } };
                template<> struct __declspec(uuid("b59125a3-9520-444d-9982-8d26cb01ef12")) IIterable  <MockRuntimeClass*> : IIterable_impl  <ABI::Windows::Foundation::Internal::AggregateType<MockRuntimeClass*, MockInterface*>> { static const wchar_t* z_get_rc_name_impl() { return L""; } };
                template<> struct __declspec(uuid("96f9fd14-f634-419e-bf26-5771b1848de7")) IIterator  <MockRuntimeClass*> : IIterator_impl  <ABI::Windows::Foundation::Internal::AggregateType<MockRuntimeClass*, MockInterface*>> { static const wchar_t* z_get_rc_name_impl() { return L""; } };
            }
        }
    }
}


TEST_CLASS(VectorTests)
{
    template<typename T>
    static void AssertVectorsEqual(std::vector<T> const& actual, std::vector<T> const& expected)
    {
        Assert::AreEqual(expected.size(), actual.size());

        for (size_t i = 0; i < actual.size(); i++)
        {
            Assert::AreEqual(expected[i], actual[i]);
        }
    }


    template<typename T>
    static void AssertVectorsEqual(std::vector<ComPtr<typename std::remove_pointer<T>::type>> const& actual, std::vector<T> const& expected)
    {
        Assert::AreEqual(expected.size(), actual.size());

        for (size_t i = 0; i < actual.size(); i++)
        {
            Assert::AreEqual(expected[i], actual[i].Get());
        }
    }


    template<typename T>
    static void AssertVectorsEqual(ComPtr<Vector<T>> const& actual, std::vector<T> const& expected)
    {
        AssertVectorsEqual(actual->InternalVector(), expected);
    }


    TEST_METHOD(VectorMethodsTest)
    {
        auto v = Make<Vector<int>>();

        Assert::IsFalse(v->IsFixedSize());

        // Constructor starts the vector empty.
        AssertVectorsEqual<int>(v, {});
        
        // Append.
        ThrowIfFailed(v->Append(1));
        AssertVectorsEqual<int>(v, { 1 });

        ThrowIfFailed(v->Append(23));
        AssertVectorsEqual<int>(v, { 1, 23 });

        ThrowIfFailed(v->Append(42));
        AssertVectorsEqual<int>(v, { 1, 23, 42 });

        // get_Size.
        unsigned size;

        ThrowIfFailed(v->get_Size(&size));
        Assert::AreEqual(3u, size);

        // GetAt.
        int value;

        ThrowIfFailed(v->GetAt(0, &value));
        Assert::AreEqual(1, value);

        ThrowIfFailed(v->GetAt(1, &value));
        Assert::AreEqual(23, value);

        ThrowIfFailed(v->GetAt(2, &value));
        Assert::AreEqual(42, value);

        Assert::AreEqual(E_BOUNDS, v->GetAt(3, &value));
        Assert::AreEqual(0, value);
        
        Assert::AreEqual(E_BOUNDS, v->GetAt((DWORD)-1, &value));
        Assert::AreEqual(0, value);

        // IndexOf.
        unsigned index;
        boolean found;

        ThrowIfFailed(v->IndexOf(1, &index, &found));
        Assert::IsTrue(found != 0);
        Assert::AreEqual(0u, index);

        ThrowIfFailed(v->IndexOf(23, &index, &found));
        Assert::IsTrue(found != 0);
        Assert::AreEqual(1u, index);

        ThrowIfFailed(v->IndexOf(42, &index, &found));
        Assert::IsTrue(found != 0);
        Assert::AreEqual(2u, index);

        ThrowIfFailed(v->IndexOf(123, &index, &found));
        Assert::IsFalse(found != 0);
        Assert::AreEqual(0u, index);

        // SetAt.
        ThrowIfFailed(v->SetAt(0, 100));
        AssertVectorsEqual<int>(v, { 100, 23, 42 });

        ThrowIfFailed(v->SetAt(1, 123));
        AssertVectorsEqual<int>(v, { 100, 123, 42 });

        ThrowIfFailed(v->SetAt(2, 142));
        AssertVectorsEqual<int>(v, { 100, 123, 142 });

        Assert::AreEqual(E_BOUNDS, v->SetAt(3, 0));
        Assert::AreEqual(E_BOUNDS, v->SetAt((DWORD)-1, 0));
        
        // InsertAt.
        ThrowIfFailed(v->InsertAt(0, 1));
        AssertVectorsEqual<int>(v, { 1, 100, 123, 142 });

        ThrowIfFailed(v->InsertAt(2, 2));
        AssertVectorsEqual<int>(v, { 1, 100, 2, 123, 142 });

        ThrowIfFailed(v->InsertAt(5, 3));
        AssertVectorsEqual<int>(v, { 1, 100, 2, 123, 142, 3 });

        Assert::AreEqual(E_BOUNDS, v->InsertAt(7, 0));
        Assert::AreEqual(E_BOUNDS, v->InsertAt((DWORD)-1, 0));

        // RemoveAt.
        ThrowIfFailed(v->RemoveAt(0));
        AssertVectorsEqual<int>(v, { 100, 2, 123, 142, 3 });

        ThrowIfFailed(v->RemoveAt(2));
        AssertVectorsEqual<int>(v, { 100, 2, 142, 3 });

        ThrowIfFailed(v->RemoveAt(3));
        AssertVectorsEqual<int>(v, { 100, 2, 142 });

        Assert::AreEqual(E_BOUNDS, v->RemoveAt(3));
        Assert::AreEqual(E_BOUNDS, v->RemoveAt((DWORD)-1));

        // RemoveAtEnd.
        ThrowIfFailed(v->RemoveAtEnd());
        AssertVectorsEqual<int>(v, { 100, 2 });

        ThrowIfFailed(v->RemoveAtEnd());
        AssertVectorsEqual<int>(v, { 100 });

        ThrowIfFailed(v->RemoveAtEnd());
        AssertVectorsEqual<int>(v, {});

        Assert::AreEqual(E_BOUNDS, v->RemoveAtEnd());

        // Sanity check some other failure cases with an empty collection.
        Assert::AreEqual(E_BOUNDS, v->GetAt(0, &value));
        Assert::AreEqual(E_BOUNDS, v->SetAt(0, 0));
        Assert::AreEqual(E_BOUNDS, v->RemoveAt(0));

        // Clear.
        ThrowIfFailed(v->Clear());
        AssertVectorsEqual<int>(v, {});

        v->InternalVector().push_back(1);
        v->InternalVector().push_back(2);
        AssertVectorsEqual<int>(v, { 1, 2 });

        ThrowIfFailed(v->Clear());
        AssertVectorsEqual<int>(v, {});

        // ReplaceAll.
        int values1[] = { 1, 2, 3 };
        ThrowIfFailed(v->ReplaceAll(3, values1));
        AssertVectorsEqual<int>(v, { 1, 2, 3 });

        int values2[] = { 23, 42 };
        ThrowIfFailed(v->ReplaceAll(2, values2));
        AssertVectorsEqual<int>(v, { 23, 42 });
            
        // GetMany.
        std::vector<int> output(3);
        unsigned actual;

        ThrowIfFailed(v->GetMany(0, (DWORD)output.size(), &output.front(), &actual));
        Assert::AreEqual(2u, actual);
        AssertVectorsEqual<int>(output, { 23, 42, 0 });

        output = std::vector<int>(3);
        ThrowIfFailed(v->GetMany(1, (DWORD)output.size(), &output.front(), &actual));
        Assert::AreEqual(1u, actual);
        AssertVectorsEqual<int>(output, { 42, 0, 0 });

        output = std::vector<int>(3);
        ThrowIfFailed(v->GetMany(2, (DWORD)output.size(), &output.front(), &actual));
        Assert::AreEqual(0u, actual);
        AssertVectorsEqual<int>(output, { 0, 0, 0 });

        output = std::vector<int>(3);
        ThrowIfFailed(v->GetMany(0, 1, &output.front(), &actual));
        Assert::AreEqual(1u, actual);
        AssertVectorsEqual<int>(output, { 23, 0, 0 });

        Assert::AreEqual(E_BOUNDS, v->GetMany((DWORD)-1, 1, &output.front(), &actual));
        Assert::AreEqual(E_BOUNDS, v->GetMany(3, 1, &output.front(), &actual));
    }


    TEST_METHOD(VectorViewTest)
    {
        auto v = Make<Vector<int>>();
        
        v->InternalVector().push_back(1);
        v->InternalVector().push_back(23);
        v->InternalVector().push_back(42);

        ComPtr<IVectorView<int>> view;
        ThrowIfFailed(v->GetView(&view));

        // get_Size.
        unsigned size;

        ThrowIfFailed(view->get_Size(&size));
        Assert::AreEqual(3u, size);

        // GetAt.
        int value;

        ThrowIfFailed(view->GetAt(0, &value));
        Assert::AreEqual(1, value);

        ThrowIfFailed(view->GetAt(1, &value));
        Assert::AreEqual(23, value);

        ThrowIfFailed(view->GetAt(2, &value));
        Assert::AreEqual(42, value);

        Assert::AreEqual(E_BOUNDS, view->GetAt(3, &value));
        Assert::AreEqual(0, value);

        Assert::AreEqual(E_BOUNDS, view->GetAt((DWORD)-1, &value));
        Assert::AreEqual(0, value);

        // IndexOf.
        unsigned index;
        boolean found;

        ThrowIfFailed(view->IndexOf(1, &index, &found));
        Assert::IsTrue(found != 0);
        Assert::AreEqual(0u, index);

        ThrowIfFailed(view->IndexOf(23, &index, &found));
        Assert::IsTrue(found != 0);
        Assert::AreEqual(1u, index);

        ThrowIfFailed(view->IndexOf(42, &index, &found));
        Assert::IsTrue(found != 0);
        Assert::AreEqual(2u, index);

        ThrowIfFailed(view->IndexOf(123, &index, &found));
        Assert::IsFalse(found != 0);
        Assert::AreEqual(0u, index);

        // GetMany.
        std::vector<int> output(3);
        unsigned actual;

        ThrowIfFailed(view->GetMany(0, (DWORD)output.size(), &output.front(), &actual));
        Assert::AreEqual(3u, actual);
        AssertVectorsEqual<int>(output, { 1, 23, 42 });

        output = std::vector<int>(3);
        ThrowIfFailed(view->GetMany(1, (DWORD)output.size(), &output.front(), &actual));
        Assert::AreEqual(2u, actual);
        AssertVectorsEqual<int>(output, { 23, 42, 0 });

        output = std::vector<int>(3);
        ThrowIfFailed(view->GetMany(0, 1, &output.front(), &actual));
        Assert::AreEqual(1u, actual);
        AssertVectorsEqual<int>(output, { 1, 0, 0 });

        Assert::AreEqual(E_BOUNDS, view->GetMany((DWORD)-1, 1, &output.front(), &actual));
        Assert::AreEqual(E_BOUNDS, view->GetMany(4, 1, &output.front(), &actual));

        // IIterable::First.
        ComPtr<IIterable<int>> iterable;
        ComPtr<IIterator<int>> iterator;

        ThrowIfFailed(view.As(&iterable));
        ThrowIfFailed(iterable->First(&iterator));
        ThrowIfFailed(iterator->get_Current(&value));
        Assert::AreEqual(1, value);
    }


    TEST_METHOD(VectorIteratorTest)
    {
        auto v = Make<Vector<int>>();

        v->InternalVector().push_back(23);
        v->InternalVector().push_back(42);

        ComPtr<IIterator<int>> iterator;
        ThrowIfFailed(v->First(&iterator));

        // First element.
        boolean hasCurrent;
        int value;

        ThrowIfFailed(iterator->get_HasCurrent(&hasCurrent));
        Assert::IsTrue(hasCurrent != 0);

        ThrowIfFailed(iterator->get_Current(&value));
        Assert::AreEqual(23, value);

        ThrowIfFailed(iterator->MoveNext(&hasCurrent));
        Assert::IsTrue(hasCurrent != 0);

        // Second element.
        ThrowIfFailed(iterator->get_HasCurrent(&hasCurrent));
        Assert::IsTrue(hasCurrent != 0);

        ThrowIfFailed(iterator->get_Current(&value));
        Assert::AreEqual(42, value);

        ThrowIfFailed(iterator->MoveNext(&hasCurrent));
        Assert::IsFalse(hasCurrent != 0);

        // At the end.
        ThrowIfFailed(iterator->get_HasCurrent(&hasCurrent));
        Assert::IsFalse(hasCurrent != 0);

        Assert::AreEqual(E_BOUNDS, iterator->get_Current(&value));
        Assert::AreEqual(0, value);

        Assert::AreEqual(E_BOUNDS, iterator->MoveNext(&hasCurrent));

        // GetMany.
        std::vector<int> output(3);
        unsigned actual;

        ThrowIfFailed(v->First(&iterator));
        ThrowIfFailed(iterator->GetMany((DWORD)output.size(), &output.front(), &actual));
        Assert::AreEqual(2u, actual);
        AssertVectorsEqual<int>(output, { 23, 42, 0 });

        ThrowIfFailed(v->First(&iterator));
        ThrowIfFailed(iterator->MoveNext(&hasCurrent));
        ThrowIfFailed(iterator->GetMany((DWORD)output.size(), &output.front(), &actual));
        Assert::AreEqual(1u, actual);
        AssertVectorsEqual<int>(output, { 42, 0, 0 });

        ThrowIfFailed(v->First(&iterator));
        ThrowIfFailed(iterator->MoveNext(&hasCurrent));
        ThrowIfFailed(iterator->MoveNext(&hasCurrent));
        ThrowIfFailed(iterator->GetMany((DWORD)output.size(), &output.front(), &actual));
        Assert::AreEqual(0u, actual);
        AssertVectorsEqual<int>(output, { 0, 0, 0 });

        ThrowIfFailed(v->First(&iterator));
        ThrowIfFailed(iterator->GetMany(1, &output.front(), &actual));
        Assert::AreEqual(1u, actual);
        AssertVectorsEqual<int>(output, { 23, 0, 0 });
        ThrowIfFailed(iterator->GetMany(1, &output.front(), &actual));
        Assert::AreEqual(1u, actual);
        AssertVectorsEqual<int>(output, { 42, 0, 0 });
        ThrowIfFailed(iterator->GetMany(1, &output.front(), &actual));
        Assert::AreEqual(0u, actual);
        AssertVectorsEqual<int>(output, { 0, 0, 0 });
    }


    TEST_METHOD(VectorFixedSizeTest)
    {
        auto v = Make<Vector<int>>(3, true);

        Assert::IsTrue(v->IsFixedSize());
        
        // Constructor sets all elements to zeros.
        AssertVectorsEqual<int>(v, { 0, 0, 0 });

        // SetAt works normally.
        ThrowIfFailed(v->SetAt(0, 1));
        ThrowIfFailed(v->SetAt(1, 23));
        ThrowIfFailed(v->SetAt(2, 42));

        AssertVectorsEqual<int>(v, { 1, 23, 42 });

        // But we can't change the size of the vector.
        Assert::AreEqual(E_NOTIMPL, v->Append(1));
        Assert::AreEqual(E_NOTIMPL, v->InsertAt(0, 1));
        Assert::AreEqual(E_NOTIMPL, v->RemoveAt(0));
        Assert::AreEqual(E_NOTIMPL, v->RemoveAtEnd());
        Assert::AreEqual(E_NOTIMPL, v->Clear());

        AssertVectorsEqual<int>(v, { 1, 23, 42 });

        // ReplaceAll only works if we have the same number of elements as before.
        int values[] = { 1, 2, 3, 4 };
        ThrowIfFailed(v->ReplaceAll(3, values));
        AssertVectorsEqual<int>(v, { 1, 2, 3 });

        Assert::AreEqual(E_NOTIMPL, v->ReplaceAll(0, values));
        Assert::AreEqual(E_NOTIMPL, v->ReplaceAll(2, values));
        Assert::AreEqual(E_NOTIMPL, v->ReplaceAll(4, values));
    }


    TEST_METHOD(VectorChangeTrackingTest)
    {
        auto v = Make<Vector<int>>();

        // We can get and set the dirty flag.
        Assert::IsFalse(v->IsChanged());

        v->SetChanged(true);
        Assert::IsTrue(v->IsChanged());

        v->SetChanged(false);
        Assert::IsFalse(v->IsChanged());

        // Append sets dirty.
        v->SetChanged(false);
        ThrowIfFailed(v->Append(1));
        Assert::IsTrue(v->IsChanged());

        // SetAt sets dirty.
        v->SetChanged(false);
        ThrowIfFailed(v->SetAt(0, 100));
        Assert::IsTrue(v->IsChanged());

        // InsertAt sets dirty.
        v->SetChanged(false);
        ThrowIfFailed(v->InsertAt(0, 1));
        Assert::IsTrue(v->IsChanged());

        // RemoveAt sets dirty.
        v->SetChanged(false);
        ThrowIfFailed(v->RemoveAt(0));
        Assert::IsTrue(v->IsChanged());

        // RemoveAtEnd sets dirty.
        v->SetChanged(false);
        ThrowIfFailed(v->RemoveAtEnd());
        Assert::IsTrue(v->IsChanged());

        // Clear sets dirty.
        v->SetChanged(false);
        ThrowIfFailed(v->Clear());
        Assert::IsTrue(v->IsChanged());

        // ReplaceAll sets dirty.
        v->SetChanged(false);
        int values[] = { 1, 2, 3 };
        ThrowIfFailed(v->ReplaceAll(3, values));
        Assert::IsTrue(v->IsChanged());

        // Getters do not set the dirty flag.
        v->SetChanged(false);
     
        unsigned size;
        ThrowIfFailed(v->get_Size(&size));

        int value;
        ThrowIfFailed(v->GetAt(0, &value));
        
        unsigned index;
        boolean found;
        ThrowIfFailed(v->IndexOf(1, &index, &found));

        Assert::IsFalse(v->IsChanged());
    }


    TEST_METHOD(VectorOfInterfacesTest)
    {
        MockInterface a, b, c;

        Assert::AreEqual(1, a.refCount);
        Assert::AreEqual(1, b.refCount);
        Assert::AreEqual(1, c.refCount);

        auto v = Make<Vector<MockInterface*>>();

        // Append should increase refcounts.
        ThrowIfFailed(v->Append(&a));
        ThrowIfFailed(v->Append(&b));

        AssertVectorsEqual<MockInterface*>(v, { &a, &b });

        Assert::AreEqual(2, a.refCount);
        Assert::AreEqual(2, b.refCount);
        Assert::AreEqual(1, c.refCount);

        // GetAt should increase refcounts.
        MockInterface* value;

        ThrowIfFailed(v->GetAt(0, &value));
        Assert::AreEqual(&a, value);

        Assert::AreEqual(3, a.refCount);
        Assert::AreEqual(2, b.refCount);
        Assert::AreEqual(1, c.refCount);

        value->Release();

        // IndexOf should work with ComPtr types, but not mess with refcounts.
        unsigned index;
        boolean found;

        ThrowIfFailed(v->IndexOf(&a, &index, &found));
        Assert::IsTrue(found != 0);
        Assert::AreEqual(0u, index);

        ThrowIfFailed(v->IndexOf(&b, &index, &found));
        Assert::IsTrue(found != 0);
        Assert::AreEqual(1u, index);

        ThrowIfFailed(v->IndexOf(&c, &index, &found));
        Assert::IsFalse(found != 0);
        Assert::AreEqual(0u, index);

        Assert::AreEqual(2, a.refCount);
        Assert::AreEqual(2, b.refCount);
        Assert::AreEqual(1, c.refCount);

        // SetAt should update refcounts.
        ThrowIfFailed(v->SetAt(0, &c));

        AssertVectorsEqual<MockInterface*>(v, { &c, &b });

        Assert::AreEqual(1, a.refCount);
        Assert::AreEqual(2, b.refCount);
        Assert::AreEqual(2, c.refCount);

        // GetMany should increase refcounts.
        std::vector<MockInterface*> output(3);
        unsigned actual;

        ThrowIfFailed(v->GetMany(0, (DWORD)output.size(), &output.front(), &actual));
        Assert::AreEqual(2u, actual);
        AssertVectorsEqual<MockInterface*>(output, { &c, &b, nullptr });

        Assert::AreEqual(1, a.refCount);
        Assert::AreEqual(3, b.refCount);
        Assert::AreEqual(3, c.refCount);

        output[0]->Release();
        output[1]->Release();

        // Clear should release refcounts.
        ThrowIfFailed(v->Clear());
        AssertVectorsEqual<MockInterface*>(v, {});

        Assert::AreEqual(1, a.refCount);
        Assert::AreEqual(1, b.refCount);
        Assert::AreEqual(1, c.refCount);
    }


    TEST_METHOD(VectorOfRuntimeClassesTest)
    {
        auto v = Make<Vector<MockRuntimeClass*>>();

        // Setter methods should use the ABI type.
        MockInterface a;
        ThrowIfFailed(v->Append(&a));
        Assert::AreEqual(2, a.refCount);

        // Getter methods should use the ABI type.
        MockInterface *value;
        ThrowIfFailed(v->GetAt(0, &value));
        Assert::AreEqual(&a, value);
        Assert::AreEqual(3, a.refCount);

        // View should use the logical type.
        ComPtr<IVectorView<MockRuntimeClass*>> view;
        ThrowIfFailed(v->GetView(&view));

        // Iterator should use the logical type.
        ComPtr<IIterator<MockRuntimeClass*>> iterator;
        ThrowIfFailed(v->First(&iterator));
    }
};
