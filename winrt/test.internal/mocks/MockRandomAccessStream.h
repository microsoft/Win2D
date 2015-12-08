// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

class MockRandomAccessStream : public RuntimeClass<
    IRandomAccessStream, IClosable, IInputStream, IOutputStream>
{
public:
    // IRandomAccessStream
    MOCK_METHOD1(get_Size          , HRESULT(UINT64 *));
    MOCK_METHOD1(put_Size          , HRESULT(UINT64));
    MOCK_METHOD2(GetInputStreamAt  , HRESULT(UINT64,IInputStream **));
    MOCK_METHOD2(GetOutputStreamAt , HRESULT(UINT64,ABI::Windows::Storage::Streams::IOutputStream **));
    MOCK_METHOD1(get_Position      , HRESULT(UINT64 *));
    MOCK_METHOD1(Seek              , HRESULT(UINT64));
    MOCK_METHOD1(CloneStream       , HRESULT(ABI::Windows::Storage::Streams::IRandomAccessStream **));
    MOCK_METHOD1(get_CanRead       , HRESULT(boolean *));
    MOCK_METHOD1(get_CanWrite      , HRESULT(boolean *));

    // IClosable
    MOCK_METHOD0(Close, HRESULT());

    // IInputStream
    MOCK_METHOD4(ReadAsync, HRESULT(IBuffer *,UINT32,InputStreamOptions,__FIAsyncOperationWithProgress_2_Windows__CStorage__CStreams__CIBuffer_UINT32_t **));

    // IOutputStream
    MOCK_METHOD2(WriteAsync, HRESULT(IBuffer *,__FIAsyncOperationWithProgress_2_UINT32_UINT32_t **));
    MOCK_METHOD1(FlushAsync, HRESULT(__FIAsyncOperation_1_boolean_t **));
};
