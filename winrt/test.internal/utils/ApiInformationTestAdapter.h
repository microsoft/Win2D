// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#if WINVER > _WIN32_WINNT_WINBLUE

#include <lib/utils/ApiInformationAdapter.h>

class ApiInformationTestAdapter : public ApiInformationAdapter
{
    std::vector<std::pair<std::wstring, int>> m_contracts;
    
public:
    static std::shared_ptr<ApiInformationTestAdapter> Create()
    {
        auto adapter = std::make_shared<ApiInformationTestAdapter>();
        ApiInformationAdapter::SetInstance(adapter);
        return adapter;
    }
    
    ApiInformationTestAdapter()
    {
        AddContract(UNIVERSAL_API_CONTRACT, 1);
    }
    
    void AddContract(wchar_t const* contractName, int majorVersion)
    {
        m_contracts.push_back({contractName, majorVersion});
    }

    virtual bool IsApiContractPresent(wchar_t const* contractName, int majorVersion)
    {
        std::pair<std::wstring, int> toFind(contractName, majorVersion);
        return (std::find(m_contracts.begin(), m_contracts.end(), toFind) != m_contracts.end());
    }
};

#endif
