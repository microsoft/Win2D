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
        std::shared_ptr<CustomFontManagerAdapter> m_adapter;
        ComPtr<IUriRuntimeClassFactory> m_uriFactory;

        std::mutex m_mutex;
        ComPtr<IDWriteFactory> m_isolatedFactory;
        ComPtr<IDWriteFactory> m_sharedFactory;
        ComPtr<IDWriteFontCollectionLoader> m_customLoader;

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
