// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

class MockPropertyBag : public RuntimeClass<
    RuntimeClassFlags<ClassicCom>,
    IPropertyBag2>
{
public:
    MOCK_METHOD5(Read            , HRESULT(ULONG,PROPBAG2 *,IErrorLog *,VARIANT *,HRESULT *));
    MOCK_METHOD3(Write           , HRESULT(ULONG,PROPBAG2 *,VARIANT *));
    MOCK_METHOD1(CountProperties , HRESULT(ULONG *));
    MOCK_METHOD4(GetPropertyInfo , HRESULT(ULONG,ULONG,PROPBAG2 *,ULONG *));
    MOCK_METHOD4(LoadObject      , HRESULT(LPCOLESTR,DWORD,IUnknown *,IErrorLog *));
};
