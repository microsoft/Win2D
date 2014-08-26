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
#include <Vector.h>

namespace ABI {
    namespace Windows {
        namespace Foundation {
            namespace Collections
            {
                template<> struct __declspec(uuid("b939af5b-b45d-5489-9149-61442c1905fe")) IVector<int>     : IVector_impl    <int> { };
                template<> struct __declspec(uuid("8d720cdf-3934-5d3f-9a55-40e8063b086a")) IVectorView<int> : IVectorView_impl<int> { };
                template<> struct __declspec(uuid("81a643fb-f51c-5565-83c4-f96425777b66")) IIterable<int>   : IIterable_impl  <int> { };
                template<> struct __declspec(uuid("bfea7f78-50c2-5f1d-a6ea-9e978d2699ff")) IIterator<int>   : IIterator_impl  <int> { };
            }
        }
    }
}

namespace NativeComponent
{
    using namespace Windows::Foundation::Collections;
    using namespace Microsoft::WRL;
    using namespace collections;

    public ref class VectorCreator sealed
    {
    public:
        static IVector<int>^ CreateVectorOfInts(int initialSize, bool fixedSize)
        {
            auto v = Make<Vector<int>>(initialSize, fixedSize);
            CheckMakeResult(v);

            return reinterpret_cast<IVector<int>^>(v.Get());
        }
    };
}
