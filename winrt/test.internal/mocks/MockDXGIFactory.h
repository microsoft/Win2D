// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace canvas
{
    class MockDxgiFactory : public RuntimeClass<
        RuntimeClassFlags<ClassicCom>,
        ChainInterfaces<IDXGIFactory2, IDXGIFactory1, IDXGIFactory, IDXGIObject >>
    {
    public:
        CALL_COUNTER_WITH_MOCK(IsWindowedStereoEnabledMethod, BOOL());
        CALL_COUNTER_WITH_MOCK(CreateSwapChainForCoreWindowMethod, HRESULT(IUnknown*, IUnknown*, const DXGI_SWAP_CHAIN_DESC1*, IDXGIOutput*, IDXGISwapChain1**));
        CALL_COUNTER_WITH_MOCK(CreateSwapChainForCompositionMethod, HRESULT(IUnknown*, const DXGI_SWAP_CHAIN_DESC1*, IDXGIOutput*, IDXGISwapChain1**));
        CALL_COUNTER_WITH_MOCK(SetPrivateDataMethod, HRESULT(REFGUID, UINT, const void*));
        CALL_COUNTER_WITH_MOCK(SetPrivateDataInterfaceMethod, HRESULT(REFGUID, const IUnknown*));
        CALL_COUNTER_WITH_MOCK(GetPrivateDataMethod, HRESULT(REFIID, UINT*, void*));
        CALL_COUNTER_WITH_MOCK(GetParentMethod, HRESULT(REFIID, void**));

        // IDXGIFactory2

        virtual BOOL STDMETHODCALLTYPE IsWindowedStereoEnabled(void) override
        {
            return IsWindowedStereoEnabledMethod.WasCalled();
        }

        virtual HRESULT STDMETHODCALLTYPE CreateSwapChainForHwnd(
            IUnknown* device,
            HWND hWnd,
            const DXGI_SWAP_CHAIN_DESC1* desc,
            const DXGI_SWAP_CHAIN_FULLSCREEN_DESC* fullscreenDesc,
            IDXGIOutput* restrictToOutput,
            IDXGISwapChain1** swapChain) override
        {
            Assert::Fail(); // Unexpected
            return E_NOTIMPL;
        }

        virtual HRESULT STDMETHODCALLTYPE CreateSwapChainForCoreWindow(
            IUnknown* device,
            IUnknown* window,
            const DXGI_SWAP_CHAIN_DESC1* desc,
            IDXGIOutput* restrictToOutput,
            IDXGISwapChain1** swapChain) override
        {
            return CreateSwapChainForCoreWindowMethod.WasCalled(device, window, desc, restrictToOutput, swapChain);
        }

        virtual HRESULT STDMETHODCALLTYPE GetSharedResourceAdapterLuid(
            HANDLE resource,
            LUID* luid) override
        {
            Assert::Fail(); // Unexpected
            return E_NOTIMPL;
        }

        virtual HRESULT STDMETHODCALLTYPE RegisterStereoStatusWindow(
            HWND windowHandle,
            UINT msg,
            DWORD* cookie) override
        {
            Assert::Fail(); // Unexpected
            return E_NOTIMPL;
        }

        virtual HRESULT STDMETHODCALLTYPE RegisterStereoStatusEvent(
            HANDLE event,
            DWORD* cookie) override
        {
            Assert::Fail(); // Unexpected
            return E_NOTIMPL;
        }

        virtual void STDMETHODCALLTYPE UnregisterStereoStatus(
            DWORD cookie) override
        {
            Assert::Fail(); // Unexpected
        }

        virtual HRESULT STDMETHODCALLTYPE RegisterOcclusionStatusWindow(
            HWND windowHandle,
            UINT msg,
            DWORD* cookie) override
        {
            Assert::Fail(); // Unexpected
            return E_NOTIMPL;
        }

        virtual HRESULT STDMETHODCALLTYPE RegisterOcclusionStatusEvent(
            HANDLE event,
            DWORD* cookie) override
        {
            Assert::Fail(); // Unexpected
            return E_NOTIMPL;
        }

        virtual void STDMETHODCALLTYPE UnregisterOcclusionStatus(
            DWORD cookie) override
        {
            Assert::Fail(); // Unexpected
        }

        virtual HRESULT STDMETHODCALLTYPE CreateSwapChainForComposition(
            IUnknown* device,
            const DXGI_SWAP_CHAIN_DESC1* desc,
            IDXGIOutput* restrictToOutput,
            IDXGISwapChain1** swapChain) override
        {
            return CreateSwapChainForCompositionMethod.WasCalled(device, desc, restrictToOutput, swapChain);
        }

        // IDXGIFactory1

        virtual HRESULT STDMETHODCALLTYPE EnumAdapters1(
            UINT index,
            IDXGIAdapter1** adapter) override
        {
            Assert::Fail(); // Unexpected
            return E_NOTIMPL;
        }

        virtual BOOL STDMETHODCALLTYPE IsCurrent(void) override
        {
            Assert::Fail(); // Unexpected
            return E_NOTIMPL;
        }

        // IDXGIFactory

        virtual HRESULT STDMETHODCALLTYPE EnumAdapters(
            UINT index,
            IDXGIAdapter** adapter) override
        {
            Assert::Fail(); // Unexpected
            return E_NOTIMPL;
        }

        virtual HRESULT STDMETHODCALLTYPE MakeWindowAssociation(
            HWND windowHandle,
            UINT flags) override
        {
            Assert::Fail(); // Unexpected
            return E_NOTIMPL;
        }

        virtual HRESULT STDMETHODCALLTYPE GetWindowAssociation(
            HWND* windowHandle) override
        {
            Assert::Fail(); // Unexpected
            return E_NOTIMPL;
        }

        virtual HRESULT STDMETHODCALLTYPE CreateSwapChain(
            IUnknown* device,
            DXGI_SWAP_CHAIN_DESC* desc,
            IDXGISwapChain** swapChain) override
        {
            Assert::Fail(); // Unexpected
            return E_NOTIMPL;
        }

        virtual HRESULT STDMETHODCALLTYPE CreateSoftwareAdapter(
            HMODULE module,
            IDXGIAdapter** adapter) override
        {
            Assert::Fail(); // Unexpected
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
