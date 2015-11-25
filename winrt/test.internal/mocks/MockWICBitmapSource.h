// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

class MockWICBitmapSource : public RuntimeClass<RuntimeClassFlags<ClassicCom>, IWICBitmapSource>
{
public:
    MOCK_METHOD2(GetSize        , HRESULT(UINT*, UINT*));
    MOCK_METHOD1(GetPixelFormat , HRESULT(WICPixelFormatGUID*));
    MOCK_METHOD2(GetResolution  , HRESULT(double*, double*));
    MOCK_METHOD1(CopyPalette    , HRESULT(IWICPalette*));
    MOCK_METHOD4(CopyPixels     , HRESULT(const WICRect*, UINT, UINT, BYTE*));
};


