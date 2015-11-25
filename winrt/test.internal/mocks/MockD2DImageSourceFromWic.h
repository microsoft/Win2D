// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#if WINVER > _WIN32_WINNT_WINBLUE

class MockD2DImageSourceFromWic
    : public RuntimeClass<RuntimeClassFlags<ClassicCom>
    , ChainInterfaces<ID2D1ImageSourceFromWic, ID2D1ImageSource, ID2D1Resource>>
{
public:
    // ID2D1Resource
    MOCK_METHOD1_CONST(GetFactory, void(ID2D1Factory**));

    // ID2D1ImageSource
    MOCK_METHOD0(OfferResources      , HRESULT());
    MOCK_METHOD1(TryReclaimResources , HRESULT(BOOL*));

    // ID2D1ImageSourceFromWic
    MOCK_METHOD1(EnsureCached    , HRESULT(const D2D1_RECT_U*));
    MOCK_METHOD1(TrimCache       , HRESULT(const D2D1_RECT_U*));
    MOCK_METHOD1_CONST(GetSource , void(IWICBitmapSource**));
};

#endif