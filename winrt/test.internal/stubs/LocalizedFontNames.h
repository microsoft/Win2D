// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#include "mocks/MockDWriteLocalizedStrings.h"
#include "mocks/MockDWriteStringList.h"

namespace canvas
{
    class LocalizedFontNames : public MockDWriteLocalizedStrings
    {
        struct LocalizedName
        {
            std::wstring Name;
            std::wstring Locale;
        };
        std::vector<LocalizedName> m_names;

    public:

        LocalizedFontNames(std::wstring const& name, std::wstring const& locale)
        {
            m_names.push_back({ name, locale });
        }

        LocalizedFontNames(std::wstring const& n1, std::wstring const& l1, std::wstring const& n2, std::wstring const& l2)
        {
            m_names.push_back({ n1, l1 });
            m_names.push_back({ n2, l2 });
        }

        IFACEMETHODIMP_(uint32_t) GetCount() override
        {
            return static_cast<uint32_t>(m_names.size());
        }

        IFACEMETHODIMP FindLocaleName(
            WCHAR const* localeName,
            uint32_t* index,
            BOOL* exists) override
        {
            for (uint32_t i = 0; i < m_names.size(); ++i)
            {
                if (wcscmp(localeName, m_names[i].Locale.c_str()) == 0)
                {
                    *index = i;
                    *exists = TRUE;
                    return S_OK;
                }
            }
            *exists = FALSE;
            return S_OK;
        }

        IFACEMETHODIMP GetStringLength(
            uint32_t index,
            uint32_t* length) override
        {
            *length = static_cast<uint32_t>(m_names[index].Name.size());
            return S_OK;
        }

        IFACEMETHODIMP GetString(
            uint32_t index,
            WCHAR* stringBuffer,
            uint32_t size) override
        {
            Assert::IsTrue(size == m_names[index].Name.size() + 1); // accounts for null term
            StringCchCopyN(stringBuffer, size, &m_names[index].Name[0], size);
            return S_OK;
        }

        IFACEMETHODIMP GetLocaleNameLength(
            uint32_t index,
            uint32_t* length) override
        {
            *length = static_cast<uint32_t>(m_names[index].Locale.size());
            return S_OK;
        }

        IFACEMETHODIMP GetLocaleName(
            uint32_t index,
            WCHAR* stringBuffer,
            uint32_t size) override
        {
            Assert::IsTrue(size == m_names[index].Locale.size() + 1); // accounts for null term
            StringCchCopyN(stringBuffer, size, &m_names[index].Locale[0], size);
            return S_OK;
        }
    };

#if WINVER > _WIN32_WINNT_WINBLUE
    class StringList : public MockDWriteStringList
    {
        struct LocalizedName
        {
            std::wstring Name;
            std::wstring Locale;
        };
        std::vector<LocalizedName> m_names;

    public:

        StringList(std::wstring const& name, std::wstring const& locale)
        {
            m_names.push_back({ name, locale });
        }

        StringList(std::wstring const& n1, std::wstring const& l1, std::wstring const& n2, std::wstring const& l2)
        {
            m_names.push_back({ n1, l1 });
            m_names.push_back({ n2, l2 });
        }

        IFACEMETHODIMP_(uint32_t) GetCount() override
        {
            return static_cast<uint32_t>(m_names.size());
        }

        IFACEMETHODIMP GetStringLength(
            uint32_t index,
            uint32_t* length) override
        {
            *length = static_cast<uint32_t>(m_names[index].Name.size());
            return S_OK;
        }

        IFACEMETHODIMP GetString(
            uint32_t index,
            WCHAR* stringBuffer,
            uint32_t size) override
        {
            Assert::IsTrue(size == m_names[index].Name.size() + 1); // accounts for null term
            StringCchCopyN(stringBuffer, size, &m_names[index].Name[0], size);
            return S_OK;
        }

        IFACEMETHODIMP GetLocaleNameLength(
            uint32_t index,
            uint32_t* length) override
        {
            *length = static_cast<uint32_t>(m_names[index].Locale.size());
            return S_OK;
        }

        IFACEMETHODIMP GetLocaleName(
            uint32_t index,
            WCHAR* stringBuffer,
            uint32_t size) override
        {
            Assert::IsTrue(size == m_names[index].Locale.size() + 1); // accounts for null term
            StringCchCopyN(stringBuffer, size, &m_names[index].Locale[0], size);
            return S_OK;
        }
    };
#endif

}