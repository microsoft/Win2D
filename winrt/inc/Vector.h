// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#include <wrl.h>
#include <vector>
#include "ErrorHandling.h"
#include "LifespanTracker.h"

namespace collections
{
    // Element traits describe how to store and manipulate the values inside a collection.
    // This default implementation is for value types. The same template is specialized with
    // more interesting versions for reference counted pointer types and maybe someday strings.
    template<typename T>
    struct ElementTraits
    {
        typedef T ElementType;

        static ElementType Wrap(T const& value)
        {
            return value;
        }

        static void Unwrap(ElementType const& value, _Out_ T* result)
        {
            *result = value;
        }

        static bool Equals(T const& value1, T const& value2)
        {
            return value1 == value2;
        }
    };


    // Specialized element traits for reference counted pointer types.
    template<typename T>
    struct ElementTraits<T*>
    {
        typedef Microsoft::WRL::ComPtr<T> ElementType;

        static ElementType Wrap(T* value)
        {
            return Microsoft::WRL::ComPtr<T>(value);
        }

        static void Unwrap(ElementType const& value, _Out_ T** result)
        {
            ThrowIfFailed(value.CopyTo(result));
        }

        static bool Equals(Microsoft::WRL::ComPtr<T> const& value1, T* value2)
        {
            return value1.Get() == value2;
        }
    };


    // Vector traits describe how the collection itself is implemented.
    // This default version just uses an STL vector.
    template<typename T>
    struct DefaultVectorTraits : public ElementTraits<T>
    {
        typedef std::vector<ElementType> InternalVectorType;
    };


    // Implements the WinRT IVector interface.
    template<typename T, template<typename T_abi> class Traits = DefaultVectorTraits>
    class Vector : public Microsoft::WRL::RuntimeClass<ABI::Windows::Foundation::Collections::IVector<T>,
                                                       ABI::Windows::Foundation::Collections::IIterable<T>>,
                   private LifespanTracker<Vector<T, Traits>>
    {
        InspectableClass(IVector<T>::z_get_rc_name_impl(), BaseTrust);

    public:
        // T_abi is often the same as T, but if T is a runtime class, T_abi will be the corresponding interface.
        typedef typename ABI::Windows::Foundation::Internal::GetAbiType<typename RuntimeClass::IVector::T_complex>::type T_abi;

        // Specialize our traits class to use the ABI version of the type.
        typedef Traits<T_abi> Traits;

    private:
        // Fields.
        typename Traits::InternalVectorType mVector;

        bool isFixedSize;
        bool isChanged;


    public:
        // Constructs an empty vector.
        Vector()
            : isFixedSize(false),
              isChanged(false)
        { }


        // Constructs a vector of the specified size.
        Vector(unsigned initialSize, bool isFixedSize)
            : isFixedSize(isFixedSize),
              isChanged(false)
        {
            mVector.resize(initialSize);
        }


        // Checks whether this vector is fixed or resizable.
        bool IsFixedSize() const
        {
            return isFixedSize;
        }


        // Checks whether the contents of the vector have changed since the last call to SetChanged(false).
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
        typename Traits::InternalVectorType& InternalVector()
        {
            return mVector;
        }


        virtual HRESULT STDMETHODCALLTYPE get_Size(_Out_ unsigned *size)
        {
            return ExceptionBoundary([&]
            {
                CheckInPointer(size);

                *size = (unsigned)mVector.size();
            });
        };


        virtual HRESULT STDMETHODCALLTYPE GetAt(_In_opt_ unsigned index, _Out_ T_abi *item)
        {
            return ExceptionBoundary([&]
            {
                CheckInPointer(item);
                ZeroMemory(item, sizeof(*item));

                if (index >= mVector.size())
                    ThrowHR(E_BOUNDS);

                Traits::Unwrap(mVector[index], item);
            });
        }


        virtual HRESULT STDMETHODCALLTYPE IndexOf(_In_opt_ T_abi value, _Out_ unsigned *index, _Out_ boolean *found)
        {
            return ExceptionBoundary([&]
            {
                CheckInPointer(index);
                CheckInPointer(found);

                *index = 0;
                *found = false;

                for (unsigned i = 0; i < mVector.size(); i++)
                {
                    if (Traits::Equals(mVector[i], value))
                    {
                        *index = i;
                        *found = true;
                        break;
                    }
                }
            });
        }


        virtual HRESULT STDMETHODCALLTYPE SetAt(_In_ unsigned index, _In_opt_ T_abi item)
        {
            return ExceptionBoundary([&]
            {
                if (index >= mVector.size())
                    ThrowHR(E_BOUNDS);

                mVector[index] = Traits::Wrap(item);
                isChanged = true;
            });
        }


        virtual HRESULT STDMETHODCALLTYPE InsertAt(_In_ unsigned index, _In_opt_ T_abi item)
        {
            return ExceptionBoundary([&]
            {
                if (isFixedSize)
                    ThrowHR(E_NOTIMPL);

                if (index > mVector.size())
                    ThrowHR(E_BOUNDS);

                mVector.insert(mVector.begin() + index, Traits::Wrap(item));
                isChanged = true;
            });
        }


        virtual HRESULT STDMETHODCALLTYPE RemoveAt(_In_ unsigned index)
        {
            return ExceptionBoundary([&]
            {
                if (isFixedSize)
                    ThrowHR(E_NOTIMPL);

                if (index >= mVector.size())
                    ThrowHR(E_BOUNDS);

                mVector.erase(mVector.begin() + index);
                isChanged = true;
            });
        }


        virtual HRESULT STDMETHODCALLTYPE Append(_In_opt_ T_abi item)
        {
            return ExceptionBoundary([&]
            {
                if (isFixedSize)
                    ThrowHR(E_NOTIMPL);

                mVector.emplace_back(Traits::Wrap(item));
                isChanged = true;
            });
        }


        virtual HRESULT STDMETHODCALLTYPE RemoveAtEnd()
        {
            return ExceptionBoundary([&]
            {
                if (isFixedSize)
                    ThrowHR(E_NOTIMPL);

                if (mVector.empty())
                    ThrowHR(E_BOUNDS);

                mVector.pop_back();
                isChanged = true;
            });
        }


        virtual HRESULT STDMETHODCALLTYPE Clear()
        {
            return ExceptionBoundary([&]
            {
                if (isFixedSize)
                    ThrowHR(E_NOTIMPL);

                mVector.clear();
                isChanged = true;
            });
        }


        virtual HRESULT STDMETHODCALLTYPE ReplaceAll(_In_ unsigned count, _In_reads_(count) T_abi *value)
        {
            return ExceptionBoundary([&]
            {
                if (isFixedSize && count != mVector.size())
                    ThrowHR(E_NOTIMPL);

                CheckInPointer(value);

                mVector.resize(count);

                for (unsigned i = 0; i < count; i++)
                {
                    mVector[i] = Traits::Wrap(value[i]);
                }

                isChanged = true;
            });
        }


        virtual HRESULT STDMETHODCALLTYPE GetView(_Outptr_result_maybenull_ ABI::Windows::Foundation::Collections::IVectorView<T> **view)
        {
            return ExceptionBoundary([&]
            {
                CheckAndClearOutPointer(view);

                auto vectorView = Microsoft::WRL::Make<VectorView<T, Vector>>(this);
                CheckMakeResult(vectorView);

                *view = vectorView.Detach();
            });
        }


        virtual HRESULT STDMETHODCALLTYPE First(_Outptr_result_maybenull_ ABI::Windows::Foundation::Collections::IIterator<T> **first)
        {
            return ExceptionBoundary([&]
            {
                CheckAndClearOutPointer(first);

                auto iterator = Microsoft::WRL::Make<VectorIterator<T, Vector>>(this);
                CheckMakeResult(iterator);

                *first = iterator.Detach();
            });
        }
    };


    // Implements the WinRT IVectorView interface.
    template<typename T, typename TVector>
    class VectorView : public Microsoft::WRL::RuntimeClass<ABI::Windows::Foundation::Collections::IVectorView<T>,
                                                           ABI::Windows::Foundation::Collections::IIterable<T>>,
                       private LifespanTracker<VectorView<T, TVector>>
    {
        InspectableClass(IVectorView<T>::z_get_rc_name_impl(), BaseTrust);

        // Fields.
        Microsoft::WRL::ComPtr<TVector> mVector;


    public:
        // Constructor wraps around an existing Vector<T>.
        VectorView(TVector* vector)
            : mVector(vector)
        { }


        virtual HRESULT STDMETHODCALLTYPE get_Size(_Out_ unsigned *size)
        {
            return mVector->get_Size(size);
        };


        virtual HRESULT STDMETHODCALLTYPE GetAt(_In_opt_ unsigned index, _Out_ typename TVector::T_abi *item)
        {
            return mVector->GetAt(index, item);
        }

        
        virtual HRESULT STDMETHODCALLTYPE IndexOf(_In_opt_ typename TVector::T_abi value, _Out_ unsigned *index, _Out_ boolean *found)
        {
            return mVector->IndexOf(value, index, found);
        }


        virtual HRESULT STDMETHODCALLTYPE First(_Outptr_result_maybenull_ ABI::Windows::Foundation::Collections::IIterator<T> **first)
        {
            return mVector->First(first);
        }
    };


    // Implements the WinRT IIterator interface.
    template<typename T, typename TVector>
    class VectorIterator : public Microsoft::WRL::RuntimeClass<ABI::Windows::Foundation::Collections::IIterator<T>>,
                           private LifespanTracker<VectorIterator<T, TVector>>
    {
        InspectableClass(IIterator<T>::z_get_rc_name_impl(), BaseTrust);

        // Fields.
        Microsoft::WRL::ComPtr<TVector> mVector;
        unsigned mPosition;


    public:
        // Constructor wraps around an existing Vector<T>.
        VectorIterator(TVector* vector)
            : mVector(vector),
              mPosition(0)
        { }


        virtual HRESULT STDMETHODCALLTYPE get_Current(_Out_ typename TVector::T_abi *current)
        {
            return mVector->GetAt(mPosition, current);
        }


        virtual HRESULT STDMETHODCALLTYPE get_HasCurrent(_Out_ boolean *hasCurrent)
        {
            return ExceptionBoundary([&]
            {
                CheckInPointer(hasCurrent);

                *hasCurrent = (mPosition < mVector->InternalVector().size());
            });
        }


        virtual HRESULT STDMETHODCALLTYPE MoveNext(_Out_ boolean *hasCurrent)
        {
            return ExceptionBoundary([&]
            {
                CheckInPointer(hasCurrent);

                auto size = mVector->InternalVector().size();

                if (mPosition >= size)
                {
                    ThrowHR(E_BOUNDS);
                }

                mPosition++;
                *hasCurrent = (mPosition < size);
            });
        }
    };
}
