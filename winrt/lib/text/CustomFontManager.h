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

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas
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
}}}}