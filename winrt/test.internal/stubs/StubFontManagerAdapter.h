// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#include "StubStorageFileStatics.h"

namespace canvas
{
    class StubFontManagerAdapter : public CustomFontManagerAdapter
    {
    public:
        ComPtr<StubStorageFileStatics> StorageFileStatics;

        StubFontManagerAdapter()
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
