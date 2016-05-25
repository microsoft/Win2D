// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

using namespace Microsoft::WRL;
using namespace ABI::Windows::Foundation;

namespace NativeComponent
{
    public ref class ReflectionHelper sealed
    {
    public:
        static bool IsFactoryAgile(Platform::String^ className)
        {
            Wrappers::HStringReference classNameReference(className->Begin());

            ComPtr<IUnknown> activationFactory;
            ThrowIfFailed(GetActivationFactory(classNameReference.Get(), &activationFactory));

            return MaybeAs<IMarshal>(activationFactory) != nullptr;
        }
    };
}
