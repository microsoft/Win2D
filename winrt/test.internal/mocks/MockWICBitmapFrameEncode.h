// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

class MockWICBitmapFrameEncode : public RuntimeClass<
    RuntimeClassFlags<ClassicCom>,
    IWICBitmapFrameEncode>
{
public:
    MOCK_METHOD1(Initialize             , HRESULT(IPropertyBag2 *));
    MOCK_METHOD2(SetSize                , HRESULT(UINT,UINT));
    MOCK_METHOD2(SetResolution          , HRESULT(double,double));
    MOCK_METHOD1(SetPixelFormat         , HRESULT(WICPixelFormatGUID *));
    MOCK_METHOD2(SetColorContexts       , HRESULT(UINT,IWICColorContext **));
    MOCK_METHOD1(SetPalette             , HRESULT(IWICPalette *));
    MOCK_METHOD1(SetThumbnail           , HRESULT(IWICBitmapSource *));
    MOCK_METHOD4(WritePixels            , HRESULT(UINT,UINT,UINT,BYTE *));
    MOCK_METHOD2(WriteSource            , HRESULT(IWICBitmapSource *,WICRect *));
    MOCK_METHOD0(Commit                 , HRESULT());
    MOCK_METHOD1(GetMetadataQueryWriter , HRESULT(IWICMetadataQueryWriter **));
};