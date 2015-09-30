// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

#include <Map.h>

using namespace collections;


// Fake interface specializations to make WRL happy.
// In a real app, MIDL would generate a more complete version of this.
namespace ABI {
    namespace Windows {
        namespace Foundation {
            namespace Collections
            {
                template<> struct __declspec(uuid("957FE485-BEF1-4422-8A6D-0891D57A4653")) IMap         <HSTRING, int>                 : IMap_impl         <HSTRING, int>                 { static const wchar_t* z_get_rc_name_impl() { return L""; } };
                template<> struct __declspec(uuid("96FD0F48-4A8A-4BF3-8291-6D004DA23669")) IMapView     <HSTRING, int>                 : IMapView_impl     <HSTRING, int>                 { static const wchar_t* z_get_rc_name_impl() { return L""; } };
                template<> struct __declspec(uuid("CD061E56-2851-4353-B313-C43858367BE1")) IKeyValuePair<HSTRING, int>                 : IKeyValuePair_impl<HSTRING, int>                 { static const wchar_t* z_get_rc_name_impl() { return L""; } };
                template<> struct __declspec(uuid("2D98F7AC-794D-4466-9B26-CEB4F784A156")) IIterable    <IKeyValuePair<HSTRING, int>*> : IIterable_impl    <IKeyValuePair<HSTRING, int>*> { static const wchar_t* z_get_rc_name_impl() { return L""; } };
                template<> struct __declspec(uuid("2363E52F-36DF-4FF5-818F-F77D8FDA9E14")) IIterator    <IKeyValuePair<HSTRING, int>*> : IIterator_impl    <IKeyValuePair<HSTRING, int>*> { static const wchar_t* z_get_rc_name_impl() { return L""; } };
                template<> struct __declspec(uuid("BF768E5D-8992-441C-8393-3856C4D0FAB0")) IVector      <IKeyValuePair<HSTRING, int>*> : IVector_impl      <IKeyValuePair<HSTRING, int>*> { static const wchar_t* z_get_rc_name_impl() { return L""; } };
                template<> struct __declspec(uuid("E5D0047D-8A18-4493-A845-795252E5E089")) IVectorView  <IKeyValuePair<HSTRING, int>*> : IVectorView_impl  <IKeyValuePair<HSTRING, int>*> { static const wchar_t* z_get_rc_name_impl() { return L""; } };
            }
        }
    }
}


TEST_CLASS(MapTests)
{
    static void AssertMapsEqual(ComPtr<Map<HSTRING, int>> const& actual, std::vector<std::wstring> const& expectedKeys, std::vector<int> const& expectedValues)
    {
        auto& map = actual->InternalMap();

        Assert::AreEqual(map.size(), expectedKeys.size());
        Assert::AreEqual(map.size(), expectedValues.size());

        for (size_t i = 0; i < map.size(); i++)
        {
            auto it = map.find(WinString(expectedKeys[i].c_str()));

            Assert::IsFalse(it == map.end());

            Assert::AreEqual(expectedValues[i], it->second);
        }
    }


    TEST_METHOD_EX(MapMethodsTest)
    {
        auto m = Make<Map<HSTRING, int>>();

        Assert::IsFalse(m->IsFixedSize());

        // Constructor starts the map empty.
        AssertMapsEqual(m, {}, {});

        // Insert.
        boolean replaced;

        ThrowIfFailed(m->Insert(WinString(L"one"), 1, &replaced));
        Assert::IsFalse(!!replaced);
        AssertMapsEqual(m, { L"one" }, { 1 });

        ThrowIfFailed(m->Insert(WinString(L"two"), 2, &replaced));
        Assert::IsFalse(!!replaced);
        AssertMapsEqual(m, { L"one", L"two" }, { 1, 2 });

        ThrowIfFailed(m->Insert(WinString(L"one"), 23, &replaced));
        Assert::IsTrue(!!replaced);
        AssertMapsEqual(m, { L"one", L"two" }, { 23, 2 });

        // get_Size.
        unsigned size;

        ThrowIfFailed(m->get_Size(&size));
        Assert::AreEqual(2u, size);

        // HasKey.
        boolean hasKey;

        ThrowIfFailed(m->HasKey(WinString(L"one"), &hasKey));
        Assert::IsTrue(!!hasKey);

        ThrowIfFailed(m->HasKey(WinString(L"two"), &hasKey));
        Assert::IsTrue(!!hasKey);

        ThrowIfFailed(m->HasKey(WinString(L"three"), &hasKey));
        Assert::IsFalse(!!hasKey);

        ThrowIfFailed(m->HasKey(nullptr, &hasKey));
        Assert::IsFalse(!!hasKey);

        // Lookup.
        int value;

        ThrowIfFailed(m->Lookup(WinString(L"one"), &value));
        Assert::AreEqual(23, value);

        ThrowIfFailed(m->Lookup(WinString(L"two"), &value));
        Assert::AreEqual(2, value);

        Assert::AreEqual(E_BOUNDS, m->Lookup(WinString(L"three"), &value));
        Assert::AreEqual(0, value);

        // Remove.
        Assert::AreEqual(E_BOUNDS, m->Remove(WinString(L"three")));
        AssertMapsEqual(m, { L"one", L"two" }, { 23, 2 });

        ThrowIfFailed(m->Remove(WinString(L"one")));
        AssertMapsEqual(m, { L"two" }, { 2 });

        ThrowIfFailed(m->Remove(WinString(L"two")));
        AssertMapsEqual(m, {}, {});

        // Clear.
        ThrowIfFailed(m->Clear());
        AssertMapsEqual(m, {}, {});

        m->InternalMap()[WinString(L"hello")] = 23;
        m->InternalMap()[WinString(L"world")] = 42;

        AssertMapsEqual(m, { L"hello", L"world" }, { 23, 42 });

        ThrowIfFailed(m->Clear());
        AssertMapsEqual(m, {}, {});
    }


    TEST_METHOD_EX(MapViewTest)
    {
        auto m = Make<Map<HSTRING, int>>();

        m->InternalMap()[WinString(L"hello")] = 23;
        m->InternalMap()[WinString(L"world")] = 42;

        ComPtr<IMapView<HSTRING, int>> view;
        ThrowIfFailed(m->GetView(&view));

        // get_Size.
        unsigned size;

        ThrowIfFailed(view->get_Size(&size));
        Assert::AreEqual(2u, size);

        // HasKey.
        boolean hasKey;

        ThrowIfFailed(view->HasKey(WinString(L"hello"), &hasKey));
        Assert::IsTrue(!!hasKey);

        ThrowIfFailed(view->HasKey(WinString(L"world"), &hasKey));
        Assert::IsTrue(!!hasKey);

        ThrowIfFailed(view->HasKey(WinString(L"HeLLo"), &hasKey));
        Assert::IsFalse(!!hasKey);

        // Lookup.
        int value;

        ThrowIfFailed(view->Lookup(WinString(L"hello"), &value));
        Assert::AreEqual(23, value);

        ThrowIfFailed(view->Lookup(WinString(L"world"), &value));
        Assert::AreEqual(42, value);

        Assert::AreEqual(E_BOUNDS, view->Lookup(WinString(L"narp"), &value));

        // IIterable::First.
        ComPtr<IIterable<IKeyValuePair<HSTRING, int>*>> iterable;
        ComPtr<IIterator<IKeyValuePair<HSTRING, int>*>> iterator;
        ComPtr<IKeyValuePair<HSTRING, int>> keyValuePair;
        boolean hasCurrent;
        WinString key;

        ThrowIfFailed(view.As(&iterable));
        ThrowIfFailed(iterable->First(&iterator));

        ThrowIfFailed(iterator->get_Current(&keyValuePair));
        ThrowIfFailed(keyValuePair->get_Key(key.GetAddressOf()));
        ThrowIfFailed(keyValuePair->get_Value(&value));

        Assert::AreEqual<std::wstring>(L"hello", static_cast<wchar_t const*>(key));
        Assert::AreEqual(23, value);

        ThrowIfFailed(iterator->MoveNext(&hasCurrent));
        Assert::IsTrue(!!hasCurrent);

        ThrowIfFailed(iterator->get_Current(&keyValuePair));
        ThrowIfFailed(keyValuePair->get_Key(key.GetAddressOf()));
        ThrowIfFailed(keyValuePair->get_Value(&value));

        Assert::AreEqual<std::wstring>(L"world", static_cast<wchar_t const*>(key));
        Assert::AreEqual(42, value);

        ThrowIfFailed(iterator->MoveNext(&hasCurrent));
        Assert::IsFalse(!!hasCurrent);
    }


    TEST_METHOD_EX(MapViewTestIteratorTest)
    {
        auto m = Make<Map<HSTRING, int>>();

        m->InternalMap()[WinString(L"hello")] = 23;
        m->InternalMap()[WinString(L"world")] = 42;

        ComPtr<IIterator<IKeyValuePair<HSTRING, int>*>> iterator;
        ComPtr<IKeyValuePair<HSTRING, int>> keyValuePair;
        boolean hasCurrent;
        WinString key;
        int value;

        ThrowIfFailed(m->First(&iterator));

        // First element.
        ThrowIfFailed(iterator->get_HasCurrent(&hasCurrent));
        Assert::IsTrue(!!hasCurrent);

        ThrowIfFailed(iterator->get_Current(&keyValuePair));
        ThrowIfFailed(keyValuePair->get_Key(key.GetAddressOf()));
        ThrowIfFailed(keyValuePair->get_Value(&value));

        Assert::AreEqual<std::wstring>(L"hello", static_cast<wchar_t const*>(key));
        Assert::AreEqual(23, value);

        // Second element.
        ThrowIfFailed(iterator->MoveNext(&hasCurrent));
        Assert::IsTrue(!!hasCurrent);

        ThrowIfFailed(iterator->get_HasCurrent(&hasCurrent));
        Assert::IsTrue(!!hasCurrent);

        ThrowIfFailed(iterator->get_Current(&keyValuePair));
        ThrowIfFailed(keyValuePair->get_Key(key.GetAddressOf()));
        ThrowIfFailed(keyValuePair->get_Value(&value));

        Assert::AreEqual<std::wstring>(L"world", static_cast<wchar_t const*>(key));
        Assert::AreEqual(42, value);

        // At the end.
        ThrowIfFailed(iterator->MoveNext(&hasCurrent));
        Assert::IsFalse(!!hasCurrent);

        ThrowIfFailed(iterator->get_HasCurrent(&hasCurrent));
        Assert::IsFalse(!!hasCurrent);

        Assert::AreEqual(E_BOUNDS, iterator->get_Current(&keyValuePair));
        Assert::IsNull(keyValuePair.Get());

        Assert::AreEqual(E_BOUNDS, iterator->MoveNext(&hasCurrent));
    }


    TEST_METHOD_EX(MapFixedSizeTest)
    {
        std::pair<WinString, int> initialValues[] =
        {
            std::make_pair(WinString(L"hello"), 23),
            std::make_pair(WinString(L"world"), 42),
        };

        auto m = Make<Map<HSTRING, int>>(true, std::begin(initialValues), std::end(initialValues));

        Assert::IsTrue(m->IsFixedSize());

        // Constructor sets all elements to zeros.
        AssertMapsEqual(m, { L"hello", L"world" }, { 23, 42 });

        // Insert works to replace existing values.
        boolean replaced;

        ThrowIfFailed(m->Insert(WinString(L"hello"), 1, &replaced));
        ThrowIfFailed(m->Insert(WinString(L"world"), 2, &replaced));

        AssertMapsEqual(m, { L"hello", L"world" }, { 1, 2 });

        // But we can't change the size of the map.
        Assert::AreEqual(E_NOTIMPL, m->Insert(WinString(L"narp"), 123, &replaced));

        Assert::AreEqual(E_NOTIMPL, m->Remove(WinString(L"hello")));
        Assert::AreEqual(E_NOTIMPL, m->Remove(WinString(L"narp")));

        Assert::AreEqual(E_NOTIMPL, m->Clear());

        AssertMapsEqual(m, { L"hello", L"world" }, { 1, 2 });
    }


    TEST_METHOD_EX(MapChangeTrackingTest)
    {
        auto m = Make<Map<HSTRING, int>>();

        // We can get and set the dirty flag.
        Assert::IsFalse(m->IsChanged());

        m->SetChanged(true);
        Assert::IsTrue(m->IsChanged());

        m->SetChanged(false);
        Assert::IsFalse(m->IsChanged());

        // Insert sets dirty.
        boolean replaced;

        m->SetChanged(false);
        ThrowIfFailed(m->Insert(WinString(L"hello"), 1, &replaced));
        Assert::IsTrue(m->IsChanged());

        m->SetChanged(false);
        ThrowIfFailed(m->Insert(WinString(L"hello"), 2, &replaced));
        Assert::IsTrue(m->IsChanged());

        // Getters do not set the dirty flag.
        m->SetChanged(false);

        unsigned size;
        ThrowIfFailed(m->get_Size(&size));

        boolean hasKey;
        ThrowIfFailed(m->HasKey(WinString(L"hello"), &hasKey));

        int value;
        ThrowIfFailed(m->Lookup(WinString(L"hello"), &value));

        Assert::IsFalse(m->IsChanged());

        // Remove sets dirty.
        m->SetChanged(false);
        ThrowIfFailed(m->Remove(WinString(L"hello")));
        Assert::IsTrue(m->IsChanged());

        // Clear sets dirty.
        m->SetChanged(false);
        ThrowIfFailed(m->Clear());
        Assert::IsTrue(m->IsChanged());
    }
};
