// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

using namespace Microsoft::WRL;

TEST_CLASS(DWriteExperiments)
{
public:
    TEST_METHOD(DWriteCreateFactory_Shared_ReturnsSameObject)
    {
        ComPtr<IDWriteFactory2> factory1;
        ComPtr<IDWriteFactory2> factory2;

        ThrowIfFailed(DWriteCreateFactory(
            DWRITE_FACTORY_TYPE_SHARED,
            __uuidof(&factory1),
            static_cast<IUnknown**>(&factory1)));

        ThrowIfFailed(DWriteCreateFactory(
            DWRITE_FACTORY_TYPE_SHARED,
            __uuidof(&factory2),
            static_cast<IUnknown**>(&factory2)));

        Assert::AreEqual(factory1.Get(), factory2.Get());
    }
};

