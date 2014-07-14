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

