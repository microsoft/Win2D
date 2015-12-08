// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

class MockWICBitmapEncoder : public RuntimeClass<
    RuntimeClassFlags<ClassicCom>,
    IWICBitmapEncoder>
{
public:
    MOCK_METHOD2(Initialize             , HRESULT(IStream *,WICBitmapEncoderCacheOption));
    MOCK_METHOD1(GetContainerFormat     , HRESULT(GUID *));
    MOCK_METHOD1(GetEncoderInfo         , HRESULT(IWICBitmapEncoderInfo **));
    MOCK_METHOD2(SetColorContexts       , HRESULT(UINT,IWICColorContext **));
    MOCK_METHOD1(SetPalette             , HRESULT(IWICPalette *));
    MOCK_METHOD1(SetThumbnail           , HRESULT(IWICBitmapSource *));
    MOCK_METHOD1(SetPreview             , HRESULT(IWICBitmapSource *));
    MOCK_METHOD2(CreateNewFrame         , HRESULT(IWICBitmapFrameEncode **,IPropertyBag2 **));
    MOCK_METHOD0(Commit                 , HRESULT());
    MOCK_METHOD1(GetMetadataQueryWriter , HRESULT(IWICMetadataQueryWriter **));
};
