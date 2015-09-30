// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

// Reuse the ElementTraits<> helper from Vector.h.
#include "Vector.h"

namespace collections
{
    // Less-than operator for comparing map keys - default version uses std::less<>.
    template<typename T>
    struct MapKeyComparison : public std::less<T>
    { };


    // Less-than operator for comparing map keys - specialized for COM pointer types.
    template<typename T>
    struct MapKeyComparison<Microsoft::WRL::ComPtr<T>>
    {
        bool operator() (Microsoft::WRL::ComPtr<T> const& value1, Microsoft::WRL::ComPtr<T> const& value2)
        {
            Microsoft::WRL::ComPtr<IUnknown> value1Identity;
            Microsoft::WRL::ComPtr<IUnknown> value2Identity;

            if (value1)
                ThrowIfFailed(value1.As(&value1Identity));

            if (value2)
                ThrowIfFailed(value2.As(&value2Identity));

            return value1Identity < value2Identity;
        }
    };


    // Less-than operator for comparing map keys - specialized for strings.
    template<>
    struct MapKeyComparison<WinString>
    {
        bool operator() (WinString const& value1, WinString const& value2)
        {
            int compareResult;
            ThrowIfFailed(WindowsCompareStringOrdinal(value1, value2, &compareResult));
            return compareResult < 0;
        }
    };


    // Map traits describe how the collection itself is implemented.
    // This default version just uses an STL map.
    template<typename TKey, typename TValue>
    struct DefaultMapTraits
    {
        typedef ElementTraits<TKey> KeyTraits;
        typedef ElementTraits<TValue> ValueTraits;

        typedef std::map<typename KeyTraits::ElementType, typename ValueTraits::ElementType, MapKeyComparison<typename KeyTraits::ElementType>> InternalMapType;


        static unsigned GetSize(InternalMapType const& map)
        {
            return (unsigned)map.size();
        };


        static bool HasKey(InternalMapType const& map, TKey const& key)
        {
            return map.find(KeyTraits::Wrap(key)) != map.end();
        }


        static typename ValueTraits::ElementType Lookup(InternalMapType const& map, TKey const& key)
        {
            auto it = map.find(KeyTraits::Wrap(key));

            if (it == map.end())
                ThrowHR(E_BOUNDS);

            return it->second;
        }


        static bool Insert(InternalMapType& map, TKey const& key, TValue const& value, bool isFixedSize)
        {
            auto it = map.find(KeyTraits::Wrap(key));

            if (it != map.end())
            {
                // Replace existing item.
                it->second = ValueTraits::Wrap(value);
                return true;
            }
            else
            {
                // Insert new item.
                if (isFixedSize)
                    ThrowHR(E_NOTIMPL);

                map.insert(it, std::make_pair(KeyTraits::Wrap(key), ValueTraits::Wrap(value)));
                return false;
            }
        }


        static void Remove(InternalMapType& map, TKey const& key)
        {
            auto count = map.erase(KeyTraits::Wrap(key));
            
            if (count != 1)
                ThrowHR(E_BOUNDS);
        }


        static void Clear(InternalMapType& map)
        {
            map.clear();
        }


        static InternalMapType const& GetKeyValuePairs(InternalMapType const& map)
        {
            return map;
        }
    };


    // Implements the WinRT IMap interface.
    template<typename TKey, typename TValue, template<typename TKey_abi, typename TValue_abi> class Traits = DefaultMapTraits>
    class Map : public Microsoft::WRL::RuntimeClass<ABI::Windows::Foundation::Collections::IMap<TKey, TValue>,
                                                    ABI::Windows::Foundation::Collections::IIterable<ABI::Windows::Foundation::Collections::IKeyValuePair<TKey, TValue>*>>,
                private LifespanTracker<Map<TKey, TValue, Traits>>
    {
        InspectableClass(IMap::z_get_rc_name_impl(), BaseTrust);

    public:
        // T_abi is often the same as T, but if T is a runtime class, T_abi will be the corresponding interface.
        typedef typename ABI::Windows::Foundation::Internal::GetAbiType<typename RuntimeClass::IMap::K_complex>::type TKey_abi;
        typedef typename ABI::Windows::Foundation::Internal::GetAbiType<typename RuntimeClass::IMap::V_complex>::type TValue_abi;

        // Specialize our traits class to use the ABI version of the type.
        typedef Traits<TKey_abi, TValue_abi> Traits;

    private:
        // Fields.
        typename Traits::InternalMapType mMap;

        bool isFixedSize;
        bool isChanged;


    public:
        // Constructs an empty map.
        Map()
            : isFixedSize(false),
              isChanged(false)
        { }


        // Constructs a map of the specified size.
        template<typename... Args>
        Map(bool isFixedSize, Args&&... args)
            : mMap(std::forward<Args>(args)...),
              isFixedSize(isFixedSize),
              isChanged(false)
        { }


        // Checks whether this map is fixed or resizable.
        bool IsFixedSize() const
        {
            return isFixedSize;
        }


        // Checks whether the contents of the map have changed since the last call to SetChanged(false).
        bool IsChanged() const
        {
            return isChanged;
        }


        // Sets or clears the IsChanged flag.
        void SetChanged(bool changed)
        {
            isChanged = changed;
        }


        // Expose direct access to the internal STL collection. This lets C++ owners
        // bypass the ExceptionBoundary overhead of the public WinRT API surface.
        typename Traits::InternalMapType& InternalMap()
        {
            return mMap;
        }


        virtual HRESULT STDMETHODCALLTYPE get_Size(_Out_ unsigned* size)
        {
            return ExceptionBoundary([&]
            {
                CheckInPointer(size);

                *size = Traits::GetSize(mMap);
            });
        };


        virtual HRESULT STDMETHODCALLTYPE HasKey(_In_ TKey_abi key, _Out_ boolean* hasKey)
        {
            return ExceptionBoundary([&]
            {
                CheckInPointer(hasKey);

                *hasKey = Traits::HasKey(mMap, key);
            });
        }


        virtual HRESULT STDMETHODCALLTYPE Lookup(_In_ TKey_abi key, _Out_ TValue_abi* item)
        {
            return ExceptionBoundary([&]
            {
                CheckInPointer(item);
                ZeroMemory(item, sizeof(*item));

                Traits::ValueTraits::Unwrap(Traits::Lookup(mMap, key), item);
            });
        }


        virtual HRESULT STDMETHODCALLTYPE Insert(_In_ TKey_abi key, _In_ TValue_abi value, _Out_ boolean* replaced)
        {
            return ExceptionBoundary([&]
            {
                CheckInPointer(replaced);

                *replaced = Traits::Insert(mMap, key, value, isFixedSize);
                isChanged = true;
            });
        }


        virtual HRESULT STDMETHODCALLTYPE Remove(_In_ TKey_abi key)
        {
            return ExceptionBoundary([&]
            {
                if (isFixedSize)
                    ThrowHR(E_NOTIMPL);

                Traits::Remove(mMap, key);
                isChanged = true;
            });
        }


        virtual HRESULT STDMETHODCALLTYPE Clear()
        {
            return ExceptionBoundary([&]
            {
                if (isFixedSize)
                    ThrowHR(E_NOTIMPL);

                Traits::Clear(mMap);
                isChanged = true;
            });
        }


        virtual HRESULT STDMETHODCALLTYPE GetView(_Outptr_result_maybenull_ ABI::Windows::Foundation::Collections::IMapView<TKey, TValue>** view)
        {
            return ExceptionBoundary([&]
            {
                CheckAndClearOutPointer(view);

                auto mapView = Microsoft::WRL::Make<MapView<TKey, TValue, Map>>(this);
                CheckMakeResult(mapView);

                *view = mapView.Detach();
            });
        }


        virtual HRESULT STDMETHODCALLTYPE First(_Outptr_result_maybenull_ ABI::Windows::Foundation::Collections::IIterator<ABI::Windows::Foundation::Collections::IKeyValuePair<TKey, TValue>*>** first)
        {
            return ExceptionBoundary([&]
            {
                // Flatten our map into a Vector<KeyValuePair>.
                auto vector = Microsoft::WRL::Make<Vector<ABI::Windows::Foundation::Collections::IKeyValuePair<TKey, TValue>*>>();
                CheckMakeResult(vector);

                for (auto& entry : Traits::GetKeyValuePairs(mMap))
                {
                    auto keyValuePair = Microsoft::WRL::Make<KeyValuePair<TKey, TValue, Traits>>(entry.first, entry.second);
                    CheckMakeResult(keyValuePair);

                    ThrowIfFailed(vector->Append(keyValuePair.Get()));
                }

                // Iterate over the vector.
                ThrowIfFailed(vector->First(first));
            });
        }
    };


    // Implements the WinRT IMapView interface.
    template<typename TKey, typename TValue, typename TMap>
    class MapView : public Microsoft::WRL::RuntimeClass<ABI::Windows::Foundation::Collections::IMapView<TKey, TValue>,
                                                        ABI::Windows::Foundation::Collections::IIterable<ABI::Windows::Foundation::Collections::IKeyValuePair<TKey, TValue>*>>,
                    private LifespanTracker<MapView<TKey, TValue, TMap>>
    {
        InspectableClass(IMapView::z_get_rc_name_impl(), BaseTrust);

        // Fields.
        Microsoft::WRL::ComPtr<TMap> mMap;


    public:
        // Constructor wraps around an existing Map<T>.
        MapView(TMap* map)
            : mMap(map)
        { }


        virtual HRESULT STDMETHODCALLTYPE get_Size(_Out_ unsigned *size)
        {
            return mMap->get_Size(size);
        };


        virtual HRESULT STDMETHODCALLTYPE HasKey(_In_ typename TMap::TKey_abi key, _Out_ boolean* hasKey)
        {
            return mMap->HasKey(key, hasKey);
        }


        virtual HRESULT STDMETHODCALLTYPE Lookup(_In_ typename TMap::TKey_abi key, _Out_ typename TMap::TValue_abi* item)
        {
            return mMap->Lookup(key, item);
        }


        virtual HRESULT STDMETHODCALLTYPE Split(_Out_ IMapView** first, _Out_ IMapView** second)
        {
            return ExceptionBoundary([&]
            {
                CheckAndClearOutPointer(first);
                CheckAndClearOutPointer(second);
            });
        }


        virtual HRESULT STDMETHODCALLTYPE First(_Outptr_result_maybenull_ ABI::Windows::Foundation::Collections::IIterator<ABI::Windows::Foundation::Collections::IKeyValuePair<TKey, TValue>*>** first)
        {
            return mMap->First(first);
        }
    };


    // Implements the WinRT KeyValuePair interface.
    template<typename TKey, typename TValue, typename Traits>
    class KeyValuePair : public Microsoft::WRL::RuntimeClass<ABI::Windows::Foundation::Collections::IKeyValuePair<TKey, TValue>>,
                         private LifespanTracker<KeyValuePair<TKey, TValue, Traits>>
    {
        InspectableClass(KeyValuePair::z_get_rc_name_impl(), BaseTrust);

        typedef typename ABI::Windows::Foundation::Internal::GetAbiType<typename RuntimeClass::IKeyValuePair::K_complex>::type TKey_abi;
        typedef typename ABI::Windows::Foundation::Internal::GetAbiType<typename RuntimeClass::IKeyValuePair::V_complex>::type TValue_abi;

        // Fields.
        typename Traits::KeyTraits::ElementType mKey;
        typename Traits::ValueTraits::ElementType mValue;


    public:
        KeyValuePair(typename Traits::KeyTraits::ElementType const& key, typename Traits::ValueTraits::ElementType const& value)
            : mKey(key),
              mValue(value)
        { }


        virtual HRESULT STDMETHODCALLTYPE get_Key(_Out_ typename TKey_abi* key)
        {
            return ExceptionBoundary([&]
            {
                CheckInPointer(key);
                ZeroMemory(key, sizeof(*key));

                Traits::KeyTraits::Unwrap(mKey, key);
            });
        }


        virtual HRESULT STDMETHODCALLTYPE get_Value(_Out_ typename TValue_abi* value)
        {
            return ExceptionBoundary([&]
            {
                CheckInPointer(value);
                ZeroMemory(value, sizeof(*value));

                Traits::ValueTraits::Unwrap(mValue, value);
            });
        }
    };
}
