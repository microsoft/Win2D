// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

class MockWICImagingFactory : public RuntimeClass<
    RuntimeClassFlags<ClassicCom>,
    ChainInterfaces<IWICImagingFactory2, IWICImagingFactory>>
{
public:
    // IWICImagingFactory
    MOCK_METHOD5(CreateDecoderFromFilename                , HRESULT(LPCWSTR,const GUID *,DWORD,WICDecodeOptions,IWICBitmapDecoder **));
    MOCK_METHOD4(CreateDecoderFromStream                  , HRESULT(IStream *,const GUID *,WICDecodeOptions,IWICBitmapDecoder **));
    MOCK_METHOD4(CreateDecoderFromFileHandle              , HRESULT(ULONG_PTR,const GUID *,WICDecodeOptions,IWICBitmapDecoder **));
    MOCK_METHOD2(CreateComponentInfo                      , HRESULT(const IID &,IWICComponentInfo **));
    MOCK_METHOD3(CreateDecoder                            , HRESULT(const GUID &,const GUID *,IWICBitmapDecoder **));
    MOCK_METHOD3(CreateEncoder                            , HRESULT(const GUID &,const GUID *,IWICBitmapEncoder **));
    MOCK_METHOD1(CreatePalette                            , HRESULT(IWICPalette **));
    MOCK_METHOD1(CreateFormatConverter                    , HRESULT(IWICFormatConverter **));
    MOCK_METHOD1(CreateBitmapScaler                       , HRESULT(IWICBitmapScaler **));
    MOCK_METHOD1(CreateBitmapClipper                      , HRESULT(IWICBitmapClipper **));
    MOCK_METHOD1(CreateBitmapFlipRotator                  , HRESULT(IWICBitmapFlipRotator **));
    MOCK_METHOD1(CreateStream                             , HRESULT(IWICStream **));
    MOCK_METHOD1(CreateColorContext                       , HRESULT(IWICColorContext **));
    MOCK_METHOD1(CreateColorTransformer                   , HRESULT(IWICColorTransform **));
    MOCK_METHOD5(CreateBitmap                             , HRESULT(UINT,UINT,REFWICPixelFormatGUID,WICBitmapCreateCacheOption,IWICBitmap **));
    MOCK_METHOD3(CreateBitmapFromSource                   , HRESULT(IWICBitmapSource *,WICBitmapCreateCacheOption,IWICBitmap **));
    MOCK_METHOD6(CreateBitmapFromSourceRect               , HRESULT(IWICBitmapSource *,UINT,UINT,UINT,UINT,IWICBitmap **));
    MOCK_METHOD7(CreateBitmapFromMemory                   , HRESULT(UINT,UINT,REFWICPixelFormatGUID,UINT,UINT,BYTE *,IWICBitmap **));
    MOCK_METHOD4(CreateBitmapFromHBITMAP                  , HRESULT(HBITMAP,HPALETTE,WICBitmapAlphaChannelOption,IWICBitmap **));
    MOCK_METHOD2(CreateBitmapFromHICON                    , HRESULT(HICON,IWICBitmap **));
    MOCK_METHOD3(CreateComponentEnumerator                , HRESULT(DWORD,DWORD,IEnumUnknown **));
    MOCK_METHOD2(CreateFastMetadataEncoderFromDecoder     , HRESULT(IWICBitmapDecoder *,IWICFastMetadataEncoder **));
    MOCK_METHOD2(CreateFastMetadataEncoderFromFrameDecode , HRESULT(IWICBitmapFrameDecode *,IWICFastMetadataEncoder **));
    MOCK_METHOD3(CreateQueryWriter                        , HRESULT(const GUID &,const GUID *,IWICMetadataQueryWriter **));
    MOCK_METHOD3(CreateQueryWriterFromReader              , HRESULT(IWICMetadataQueryReader *,const GUID *,IWICMetadataQueryWriter **));

    // IWICImagingFactory2
    MOCK_METHOD2(CreateImageEncoder, HRESULT(ID2D1Device *,IWICImageEncoder **));
};
