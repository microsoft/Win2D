// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

class MockWICImageEncoder : public RuntimeClass<
    RuntimeClassFlags<ClassicCom>,
    IWICImageEncoder>
{
public:
    MOCK_METHOD3(WriteFrame          , HRESULT(ID2D1Image *,IWICBitmapFrameEncode *,const WICImageParameters *));
    MOCK_METHOD3(WriteFrameThumbnail , HRESULT(ID2D1Image *,IWICBitmapFrameEncode *,const WICImageParameters *));
    MOCK_METHOD3(WriteThumbnail      , HRESULT(ID2D1Image *,IWICBitmapEncoder *,const WICImageParameters *));
};
