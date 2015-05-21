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
    class MockDxgiOutput : public RuntimeClass<
        RuntimeClassFlags<ClassicCom>,
        ChainInterfaces<IDXGIOutput, IDXGIObject >>
    {
    public:
        CALL_COUNTER_WITH_MOCK(WaitForVBlankMethod, HRESULT());
        CALL_COUNTER_WITH_MOCK(SetPrivateDataMethod, HRESULT(REFGUID, UINT, const void*));
        CALL_COUNTER_WITH_MOCK(SetPrivateDataInterfaceMethod, HRESULT(REFGUID, const IUnknown*));
        CALL_COUNTER_WITH_MOCK(GetPrivateDataMethod, HRESULT(REFIID, UINT*, void*));
        CALL_COUNTER_WITH_MOCK(GetParentMethod, HRESULT(REFIID, void**));

        // IDXGIOutput

        virtual HRESULT STDMETHODCALLTYPE GetDesc(
            DXGI_OUTPUT_DESC *pDesc) override
        {
            Assert::Fail();
            return E_NOTIMPL;
        }

        virtual HRESULT STDMETHODCALLTYPE GetDisplayModeList(
            DXGI_FORMAT EnumFormat,
            UINT Flags,
            _Inout_  UINT *pNumModes,
            _Out_writes_to_opt_(*pNumModes, *pNumModes)  DXGI_MODE_DESC *pDesc)
        {
            Assert::Fail();
            return E_NOTIMPL;
        }

        virtual HRESULT STDMETHODCALLTYPE FindClosestMatchingMode(
            const DXGI_MODE_DESC *pModeToMatch,
            DXGI_MODE_DESC *pClosestMatch,
            _In_opt_  IUnknown *pConcernedDevice)
        {
            Assert::Fail();
            return E_NOTIMPL;
        }

        virtual HRESULT STDMETHODCALLTYPE WaitForVBlank(void)
        {
            return WaitForVBlankMethod.WasCalled();
        }

        virtual HRESULT STDMETHODCALLTYPE TakeOwnership(
            IUnknown *pDevice,
            BOOL Exclusive)
        {
            Assert::Fail();
            return E_NOTIMPL;
        }

        virtual void STDMETHODCALLTYPE ReleaseOwnership(void)
        {
            Assert::Fail();
        }

        virtual HRESULT STDMETHODCALLTYPE GetGammaControlCapabilities(
            DXGI_GAMMA_CONTROL_CAPABILITIES *pGammaCaps)
        {
            Assert::Fail();
            return E_NOTIMPL;
        }

        virtual HRESULT STDMETHODCALLTYPE SetGammaControl(
            const DXGI_GAMMA_CONTROL *pArray)
        {
            Assert::Fail();
            return E_NOTIMPL;
        }

        virtual HRESULT STDMETHODCALLTYPE GetGammaControl(
            DXGI_GAMMA_CONTROL *pArray)
        {
            Assert::Fail();
            return E_NOTIMPL;
        }

        virtual HRESULT STDMETHODCALLTYPE SetDisplaySurface(
            IDXGISurface *pScanoutSurface)
        {
            Assert::Fail();
            return E_NOTIMPL;
        }

        virtual HRESULT STDMETHODCALLTYPE GetDisplaySurfaceData(
            IDXGISurface *pDestination)
        {
            Assert::Fail();
            return E_NOTIMPL;
        }

        virtual HRESULT STDMETHODCALLTYPE GetFrameStatistics(
            DXGI_FRAME_STATISTICS *pStats)
        {
            Assert::Fail();
            return E_NOTIMPL;
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
