// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

class MockD2DImageSource
    : public RuntimeClass<RuntimeClassFlags<ClassicCom>
    , ChainInterfaces<ID2D1ImageSource, ID2D1Resource>>
{
public:
    // ID2D1Resource
    MOCK_METHOD1_CONST(GetFactory, void(ID2D1Factory**));

    // ID2D1ImageSource
    MOCK_METHOD0(OfferResources      , HRESULT());
    MOCK_METHOD1(TryReclaimResources , HRESULT(BOOL*));
};