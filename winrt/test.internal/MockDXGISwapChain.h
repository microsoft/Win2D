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
    class MockDxgiSwapChain : public RuntimeClass<
        RuntimeClassFlags<ClassicCom>,
        ChainInterfaces<IDXGISwapChain2, IDXGISwapChain1, IDXGISwapChain, IDXGIDeviceSubObject, IDXGIObject>>
    {
    public:
        CALL_COUNTER_WITH_MOCK(SetSourceSizeMethod, HRESULT(UINT, UINT));
        CALL_COUNTER_WITH_MOCK(GetSourceSizeMethod, HRESULT(UINT*, UINT*));
        CALL_COUNTER_WITH_MOCK(SetMaximumFrameLatencyMethod, HRESULT(UINT));
        CALL_COUNTER_WITH_MOCK(GetMaximumFrameLatencyMethod, HRESULT(UINT*));
        CALL_COUNTER_WITH_MOCK(GetFrameLatencyWaitableObjectMethod, HANDLE());
        CALL_COUNTER_WITH_MOCK(SetMatrixTransformMethod, HRESULT(const DXGI_MATRIX_3X2_F*));
        CALL_COUNTER_WITH_MOCK(GetMatrixTransformMethod, HRESULT(DXGI_MATRIX_3X2_F*));
        CALL_COUNTER_WITH_MOCK(GetDesc1Method, HRESULT(DXGI_SWAP_CHAIN_DESC1*));
        CALL_COUNTER_WITH_MOCK(GetFullscreenDescMethod, HRESULT(DXGI_SWAP_CHAIN_FULLSCREEN_DESC*));
        CALL_COUNTER_WITH_MOCK(GetHwndMethod, HRESULT(HWND*));
        CALL_COUNTER_WITH_MOCK(GetCoreWindowMethod, HRESULT(REFIID, void**));
        CALL_COUNTER_WITH_MOCK(Present1Method, HRESULT(UINT, UINT, const DXGI_PRESENT_PARAMETERS*));
        CALL_COUNTER_WITH_MOCK(IsTemporaryMonoSupportedMethod, BOOL());
        CALL_COUNTER_WITH_MOCK(GetRestrictToOutputMethod, HRESULT(IDXGIOutput**));
        CALL_COUNTER_WITH_MOCK(SetBackgroundColorMethod, HRESULT(const DXGI_RGBA*));
        CALL_COUNTER_WITH_MOCK(GetBackgroundColorMethod, HRESULT(const DXGI_RGBA*));
        CALL_COUNTER_WITH_MOCK(SetRotationMethod, HRESULT(DXGI_MODE_ROTATION));
        CALL_COUNTER_WITH_MOCK(GetRotationMethod, HRESULT(DXGI_MODE_ROTATION*));
        CALL_COUNTER_WITH_MOCK(GetBufferMethod, HRESULT(UINT, REFIID, void**));
        CALL_COUNTER_WITH_MOCK(SetFullscreenStateMethod, HRESULT(BOOL, IDXGIOutput*));
        CALL_COUNTER_WITH_MOCK(GetFullscreenStateMethod, HRESULT(BOOL*, IDXGIOutput**));
        CALL_COUNTER_WITH_MOCK(ResizeBuffersMethod, HRESULT(UINT, UINT, UINT, DXGI_FORMAT, UINT));
        CALL_COUNTER_WITH_MOCK(ResizeTargetMethod, HRESULT(const DXGI_MODE_DESC*));
        CALL_COUNTER_WITH_MOCK(GetContainingOutputMethod, HRESULT(IDXGIOutput**));
        CALL_COUNTER_WITH_MOCK(GetFrameStatisticsMethod, HRESULT(DXGI_FRAME_STATISTICS*));
        CALL_COUNTER_WITH_MOCK(GetLastPresentCountMethod, HRESULT(UINT*));
        CALL_COUNTER_WITH_MOCK(GetDeviceMethod, HRESULT(REFIID, void**));
        CALL_COUNTER_WITH_MOCK(SetPrivateDataMethod, HRESULT(REFGUID, UINT, const void*));
        CALL_COUNTER_WITH_MOCK(SetPrivateDataInterfaceMethod, HRESULT(REFGUID, const IUnknown*));
        CALL_COUNTER_WITH_MOCK(GetPrivateDataMethod, HRESULT(REFIID, UINT*, void*));
        CALL_COUNTER_WITH_MOCK(GetParentMethod, HRESULT(REFIID, void**));

        // IDXGISwapChain2

        virtual HRESULT STDMETHODCALLTYPE SetSourceSize(
            UINT width,
            UINT height) override
        {
            return SetSourceSizeMethod.WasCalled(width, height);
        }

        virtual HRESULT STDMETHODCALLTYPE GetSourceSize(
            UINT* width,
            UINT* height) override
        {
            return GetSourceSizeMethod.WasCalled(width, height);
        }

        virtual HRESULT STDMETHODCALLTYPE SetMaximumFrameLatency(
            UINT maxLatency) override
        {
            return SetMaximumFrameLatencyMethod.WasCalled(maxLatency);
        }

        virtual HRESULT STDMETHODCALLTYPE GetMaximumFrameLatency(
             UINT* maxLatency) override
        {
            return GetMaximumFrameLatencyMethod.WasCalled(maxLatency);
        }

        virtual HANDLE STDMETHODCALLTYPE GetFrameLatencyWaitableObject(void) override
        {
            return GetFrameLatencyWaitableObjectMethod.WasCalled();
        }

        virtual HRESULT STDMETHODCALLTYPE SetMatrixTransform(
            const DXGI_MATRIX_3X2_F* matrix) override
        {
            return SetMatrixTransformMethod.WasCalled(matrix);
        }

        virtual HRESULT STDMETHODCALLTYPE GetMatrixTransform(
            DXGI_MATRIX_3X2_F* matrix) override
        {
            return GetMatrixTransformMethod.WasCalled(matrix);
        }

        // IDXGISwapChain1

        virtual HRESULT STDMETHODCALLTYPE GetDesc1(
            DXGI_SWAP_CHAIN_DESC1* desc) override
        {
            return GetDesc1Method.WasCalled(desc);
        }

        virtual HRESULT STDMETHODCALLTYPE GetFullscreenDesc(
            DXGI_SWAP_CHAIN_FULLSCREEN_DESC* desc) override
        {
            return GetFullscreenDescMethod.WasCalled(desc);
        }

        virtual HRESULT STDMETHODCALLTYPE GetHwnd(
            HWND* hwnd) override
        {
            return GetHwndMethod.WasCalled(hwnd);
        }

        virtual HRESULT STDMETHODCALLTYPE GetCoreWindow(
            REFIID refiid,
            void** unk) override
        {
            return GetCoreWindowMethod.WasCalled(refiid, unk);
        }

        virtual HRESULT STDMETHODCALLTYPE Present1(
            UINT syncInterval,
            UINT presentFlags,
            const DXGI_PRESENT_PARAMETERS* presentParameters) override
        {
            return Present1Method.WasCalled(syncInterval, presentFlags, presentParameters);
        }

        virtual BOOL STDMETHODCALLTYPE IsTemporaryMonoSupported(void) override
        {
            return IsTemporaryMonoSupportedMethod.WasCalled();
        }

        virtual HRESULT STDMETHODCALLTYPE GetRestrictToOutput(
            IDXGIOutput** restrictToOutput) override
        {
            return GetRestrictToOutputMethod.WasCalled(restrictToOutput);
        }

        virtual HRESULT STDMETHODCALLTYPE SetBackgroundColor(
            const DXGI_RGBA* color) override
        {
            return SetBackgroundColorMethod.WasCalled(color);
        }

        virtual HRESULT STDMETHODCALLTYPE GetBackgroundColor(
            DXGI_RGBA* color) override
        {
            return GetBackgroundColorMethod.WasCalled(color);
        }

        virtual HRESULT STDMETHODCALLTYPE SetRotation(
            DXGI_MODE_ROTATION rotation) override
        {
            return SetRotationMethod.WasCalled(rotation);
        }

        virtual HRESULT STDMETHODCALLTYPE GetRotation(
            DXGI_MODE_ROTATION* rotation) override
        {
            return GetRotationMethod.WasCalled(rotation);
        }

        // IDXGISwapChain

        virtual HRESULT STDMETHODCALLTYPE Present(
            UINT SyncInterval,
            UINT Flags) override
        {
            Assert::Fail(); // Present1 should be used instead.
            return E_NOTIMPL;
        }

        virtual HRESULT STDMETHODCALLTYPE GetBuffer(
            UINT buffer,
            REFIID riid,
            void** surface) override
        {
            return GetBufferMethod.WasCalled(buffer, riid, surface);
        }

        virtual HRESULT STDMETHODCALLTYPE SetFullscreenState(
            BOOL fullscreen,
            IDXGIOutput* target) override
        {
            return SetFullscreenStateMethod.WasCalled(fullscreen, target);
        }

        virtual HRESULT STDMETHODCALLTYPE GetFullscreenState(
            BOOL* fullscreen,
            IDXGIOutput** target) override
        {
            return GetFullscreenStateMethod.WasCalled(fullscreen, target);
        }

        virtual HRESULT STDMETHODCALLTYPE GetDesc(
            DXGI_SWAP_CHAIN_DESC* desc) override
        {
            Assert::Fail(); // GetDesc1 should be used instead.
            return E_NOTIMPL;
        }

        virtual HRESULT STDMETHODCALLTYPE ResizeBuffers(
            UINT bufferCount,
            UINT width,
            UINT height,
            DXGI_FORMAT newFormat,
            UINT swapChainFlags) override
        {
            return ResizeBuffersMethod.WasCalled(bufferCount, width, height, newFormat, swapChainFlags);
        }

        virtual HRESULT STDMETHODCALLTYPE ResizeTarget(
            const DXGI_MODE_DESC* newTargetParameters) override
        {
            return ResizeTargetMethod.WasCalled(newTargetParameters);
        }

        virtual HRESULT STDMETHODCALLTYPE GetContainingOutput(
            IDXGIOutput** output) override
        {
            return GetContainingOutputMethod.WasCalled(output);
        }

        virtual HRESULT STDMETHODCALLTYPE GetFrameStatistics(
            DXGI_FRAME_STATISTICS* stats) override
        {
            return GetFrameStatisticsMethod.WasCalled(stats);
        }

        virtual HRESULT STDMETHODCALLTYPE GetLastPresentCount(
            UINT* lastPresentCount) override
        {
            return GetLastPresentCountMethod.WasCalled(lastPresentCount);
        }

        // IDXGISubObject

        virtual HRESULT STDMETHODCALLTYPE GetDevice(
            REFIID riid,
            void** device) override
        {
            return GetDeviceMethod.WasCalled(riid, device);
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
