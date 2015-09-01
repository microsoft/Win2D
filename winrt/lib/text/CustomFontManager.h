// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Text
{
    class DefaultCustomFontManagerAdapter;


    class CustomFontManagerAdapter : public Singleton<CustomFontManagerAdapter, DefaultCustomFontManagerAdapter>
    {
    public:
        virtual ~CustomFontManagerAdapter() = default;

        virtual ComPtr<IDWriteFactory> CreateDWriteFactory(DWRITE_FACTORY_TYPE type) = 0;
        virtual IStorageFileStatics* GetStorageFileStatics() = 0;
    };


    class DefaultCustomFontManagerAdapter : public CustomFontManagerAdapter
    {
        ComPtr<IStorageFileStatics> m_storageFileStatics;

    public:
        virtual ComPtr<IDWriteFactory> CreateDWriteFactory(DWRITE_FACTORY_TYPE type) override;
        virtual IStorageFileStatics* GetStorageFileStatics() override;
    };


    class CustomFontManager : public Singleton<CustomFontManager>
    {
        ComPtr<IDWriteFactory> m_isolatedFactory;
        ComPtr<IDWriteFactory> m_sharedFactory;
        ComPtr<IUriRuntimeClassFactory> m_uriFactory;
        ComPtr<IDWriteFontCollectionLoader> m_customLoader;
        std::shared_ptr<CustomFontManagerAdapter> m_adapter;

    public:
        CustomFontManager();

        ComPtr<IDWriteFontCollection> GetFontCollectionFromUri(WinString const& uri);

        void ValidateUri(WinString const& uriString);

        ComPtr<IDWriteFactory> const& GetSharedFactory();

    private:
        ComPtr<IDWriteFactory> const& GetIsolatedFactory();

        WinString GetAbsolutePathFromUri(WinString const& uriString);

    };
}}}}}
