// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

class DefaultApiInformationAdapter;

static wchar_t const* UNIVERSAL_API_CONTRACT = L"Windows.Foundation.UniversalApiContract";

//
// Adapter for accessing information about which APIs are present on the system
// via Windows.Foundation.Metadata.ApiInformation.
//
class ApiInformationAdapter : public Singleton<ApiInformationAdapter, DefaultApiInformationAdapter>
{
public:
    virtual ~ApiInformationAdapter() = default;

    virtual bool IsApiContractPresent(wchar_t const* contractName, int majorVersion) = 0;
};


//
// Default implementation of ApiInformationAdapter calls through to
// Windows.Foundation.Metadata.ApiInformation.
//
class DefaultApiInformationAdapter : public ApiInformationAdapter
{
public:
    virtual bool IsApiContractPresent(wchar_t const* contractName, int majorVersion) override;
};
