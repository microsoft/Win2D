// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may
// not use these files except in compliance with the License. You may obtain
// a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
// License for the specific language governing permissions and limitations
// under the License.

#pragma once

namespace canvas
{
    class MockDxgiAdapter : public RuntimeClass<
        RuntimeClassFlags<ClassicCom>,
        ChainInterfaces<IDXGIAdapter2, IDXGIAdapter1, IDXGIAdapter, IDXGIObject >>
    {
    public:

        CALL_COUNTER_WITH_MOCK(GetDesc2Method, HRESULT(DXGI_ADAPTER_DESC2*));
        CALL_COUNTER_WITH_MOCK(EnumOutputsMethod, HRESULT(UINT, IDXGIOutput**));
        CALL_COUNTER_WITH_MOCK(CheckInterfaceSupportMethod, HRESULT(REFGUID, LARGE_INTEGER*));
        CALL_COUNTER_WITH_MOCK(SetPrivateDataMethod, HRESULT(REFGUID, UINT, const void*));
        CALL_COUNTER_WITH_MOCK(SetPrivateDataInterfaceMethod, HRESULT(REFGUID, const IUnknown*));
        CALL_COUNTER_WITH_MOCK(GetPrivateDataMethod, HRESULT(REFIID, UINT*, void*));
        CALL_COUNTER_WITH_MOCK(GetParentMethod, HRESULT(REFIID, void**));

        // IDXGIAdapter2

        virtual HRESULT STDMETHODCALLTYPE GetDesc2(
            DXGI_ADAPTER_DESC2* desc) override
        {
            return GetDesc2Method.WasCalled(desc);
        }

        // IDXGIAdapter1

        virtual HRESULT STDMETHODCALLTYPE GetDesc1(
            DXGI_ADAPTER_DESC1* desc) override
        {
            Assert::Fail(); // Expected to call GetDesc2 instead
            return E_NOTIMPL;
        }

        // IDXGIAdapter

        virtual HRESULT STDMETHODCALLTYPE EnumOutputs(
            UINT index,
            IDXGIOutput** output) override
        {
            return EnumOutputsMethod.WasCalled(index, output);
        }

        virtual HRESULT STDMETHODCALLTYPE GetDesc(
            DXGI_ADAPTER_DESC* desc) override
        {
            Assert::Fail(); // Expected to call GetDesc2 instead
            return E_NOTIMPL;
        }

        virtual HRESULT STDMETHODCALLTYPE CheckInterfaceSupport(
            REFGUID interfaceName,
            LARGE_INTEGER* umdVersion) override
        {
            return CheckInterfaceSupportMethod.WasCalled(interfaceName, umdVersion);
        }

        // IDXGIObject

        virtual HRESULT STDMETHODCALLTYPE SetPrivateData(
            REFGUID name,
            UINT dataSize,
            const void* data) override
        {
            return SetPrivateDataMethod.WasCalled(name, dataSize, data);
        }

        virtual HRESULT STDMETHODCALLTYPE SetPrivateDataInterface(
            REFGUID name,
            const IUnknown* unknown) override
        {
            return SetPrivateDataInterfaceMethod.WasCalled(name, unknown);
        }

        virtual HRESULT STDMETHODCALLTYPE GetPrivateData(
            REFGUID name,
            UINT* dataSize,
            void* data) override
        {
            return GetPrivateDataMethod.WasCalled(name, dataSize, data);
        }

        virtual HRESULT STDMETHODCALLTYPE GetParent(
            REFIID riid,
            void** parent) override
        {
            return GetParentMethod.WasCalled(riid, parent);
        }
    };
}
