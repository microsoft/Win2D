// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace canvas
{
    class MockDxgiAdapter : public RuntimeClass<
        RuntimeClassFlags<ClassicCom>,
        ChainInterfaces<IDXGIAdapter2, IDXGIAdapter1, IDXGIAdapter, IDXGIObject >>
    {
    public:
        // IDXGIObject
        MOCK_METHOD3(SetPrivateData, HRESULT(REFGUID, UINT, void const*));
        MOCK_METHOD2(SetPrivateDataInterface, HRESULT(REFGUID, IUnknown const*));
        MOCK_METHOD3(GetPrivateData, HRESULT(REFGUID, UINT*, void*));
        MOCK_METHOD2(GetParent, HRESULT(REFIID, void**));

        // IDXGIAdapter
        MOCK_METHOD2(EnumOutputs, HRESULT(UINT, IDXGIOutput**));
        MOCK_METHOD1(GetDesc, HRESULT(DXGI_ADAPTER_DESC*));
        MOCK_METHOD2(CheckInterfaceSupport, HRESULT(REFGUID, LARGE_INTEGER*));

        // IDXGIAdapter1
        MOCK_METHOD1(GetDesc1, HRESULT(DXGI_ADAPTER_DESC1*));

        // IDXGIAdapter2
        MOCK_METHOD1(GetDesc2, HRESULT(DXGI_ADAPTER_DESC2*));
    };
}
