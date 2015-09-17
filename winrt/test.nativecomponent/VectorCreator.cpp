// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"
#include <Vector.h>

#ifndef DEF___FIVector_1_int_USE
namespace ABI {
    namespace Windows {
        namespace Foundation {
            namespace Collections
            {
                template<> struct __declspec(uuid("b939af5b-b45d-5489-9149-61442c1905fe")) IVector    <int> : IVector_impl    <int> { static const wchar_t* z_get_rc_name_impl() { return L""; } };
                template<> struct __declspec(uuid("8d720cdf-3934-5d3f-9a55-40e8063b086a")) IVectorView<int> : IVectorView_impl<int> { static const wchar_t* z_get_rc_name_impl() { return L""; } };
                template<> struct __declspec(uuid("81a643fb-f51c-5565-83c4-f96425777b66")) IIterable  <int> : IIterable_impl  <int> { static const wchar_t* z_get_rc_name_impl() { return L""; } };
                template<> struct __declspec(uuid("bfea7f78-50c2-5f1d-a6ea-9e978d2699ff")) IIterator  <int> : IIterator_impl  <int> { static const wchar_t* z_get_rc_name_impl() { return L""; } };
            }
        }
    }
}
#endif

namespace NativeComponent
{
    using namespace Windows::Foundation::Collections;
    using namespace Microsoft::WRL;
    using namespace collections;

    public ref class VectorCreator sealed
    {
    public:
        static IVector<int>^ CreateVectorOfInts(bool fixedSize, int initialSize)
        {
            auto v = Make<Vector<int>>(fixedSize, initialSize);
            CheckMakeResult(v);

            return reinterpret_cast<IVector<int>^>(v.Get());
        }
    };
}
