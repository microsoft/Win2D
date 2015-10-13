// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

#if WINVER > _WIN32_WINNT_WINBLUE

#include "ApiInformationAdapter.h"

using namespace ABI::Windows::Foundation::Metadata;

bool DefaultApiInformationAdapter::IsApiContractPresent(wchar_t const* contractName, int majorVersion)
{
    assert(majorVersion >= 0 && majorVersion <= 0xFF);
    
    ComPtr<IApiInformationStatics> apiInformation;
    ThrowIfFailed(GetActivationFactory(
        HStringReference(RuntimeClass_Windows_Foundation_Metadata_ApiInformation).Get(),
        &apiInformation));

    boolean isPresent;
    ThrowIfFailed(apiInformation->IsApiContractPresentByMajor(
        HStringReference(contractName).Get(),
        static_cast<uint16_t>(majorVersion),
        &isPresent));

    return !!isPresent;
}

#endif