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

#pragma once

#include "StubStorageFileStatics.h"

namespace canvas
{
    class StubCanvasTextFormatAdapter : public CanvasTextFormatAdapter
    {
    public:
        ComPtr<StubStorageFileStatics> StorageFileStatics;

        StubCanvasTextFormatAdapter()
            : StorageFileStatics(Make<StubStorageFileStatics>())
        {
        }

        virtual ComPtr<IDWriteFactory> CreateDWriteFactory(DWRITE_FACTORY_TYPE type) override
        {
            ComPtr<IDWriteFactory> factory;
            ThrowIfFailed(DWriteCreateFactory(type, __uuidof(factory), &factory));
            return factory;
        }

        virtual IStorageFileStatics* GetStorageFileStatics() override
        {
            return StorageFileStatics.Get();
        }
    };
}
