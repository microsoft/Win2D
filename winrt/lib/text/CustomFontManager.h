// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Text
{
    class ICanvasTextFormatAdapter;

    class CustomFontManager
        : private LifespanTracker<CustomFontManager>
    {
        ComPtr<IDWriteFactory> m_isolatedFactory;
        ComPtr<IDWriteFactory> m_sharedFactory;
        ComPtr<IUriRuntimeClassFactory> m_uriFactory;
        ComPtr<IDWriteFontCollectionLoader> m_customLoader;
        std::shared_ptr<ICanvasTextFormatAdapter> m_adapter;

    public:
        CustomFontManager(std::shared_ptr<ICanvasTextFormatAdapter> const& adapter);

        ComPtr<IDWriteFontCollection> GetFontCollectionFromUri(WinString const& uri);

        void ValidateUri(WinString const& uriString);

    protected:
        ComPtr<IDWriteFactory> const& GetSharedFactory();

    private:
        ComPtr<IDWriteFactory> const& GetIsolatedFactory();

        WinString GetAbsolutePathFromUri(WinString const& uriString);

    };
}}}}}
