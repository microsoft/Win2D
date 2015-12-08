// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

class MockStream : public RuntimeClass<
    RuntimeClassFlags<ClassicCom>,
    ChainInterfaces<IStream, ISequentialStream>>
{
public:
    // ISequentialStream
    MOCK_METHOD3(Read  , HRESULT(void* ,ULONG,ULONG* ));
    MOCK_METHOD3(Write , HRESULT(const void* ,ULONG,ULONG* ));

    // IStream
    MOCK_METHOD3(Seek         , HRESULT(LARGE_INTEGER,DWORD,ULARGE_INTEGER* ));
    MOCK_METHOD1(SetSize      , HRESULT(ULARGE_INTEGER));
    MOCK_METHOD4(CopyTo       , HRESULT(IStream* ,ULARGE_INTEGER,ULARGE_INTEGER* ,ULARGE_INTEGER* ));
    MOCK_METHOD1(Commit       , HRESULT(DWORD));
    MOCK_METHOD0(Revert       , HRESULT());
    MOCK_METHOD3(LockRegion   , HRESULT(ULARGE_INTEGER,ULARGE_INTEGER,DWORD));
    MOCK_METHOD3(UnlockRegion , HRESULT(ULARGE_INTEGER,ULARGE_INTEGER,DWORD));
    MOCK_METHOD2(Stat         , HRESULT(STATSTG* ,DWORD));
    MOCK_METHOD1(Clone        , HRESULT(IStream** ));
};
