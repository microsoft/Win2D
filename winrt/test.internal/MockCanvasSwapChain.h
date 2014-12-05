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
    class MockCanvasSwapChain : public RuntimeClass<
        RuntimeClassFlags<WinRtClassicComMix>,
        ICanvasSwapChain,
        ICanvasResourceCreator,
        IClosable,
        CloakedIid<ICanvasResourceWrapperNative>>
    {
    public:
        CALL_COUNTER_WITH_MOCK(GetResourceMethod, HRESULT(const IID&, void**));
        CALL_COUNTER_WITH_MOCK(GetDpiMethod, HRESULT(float*));

        IFACEMETHOD(CreateDrawingSession)(
            Color clearColor,
            ICanvasDrawingSession** drawingSession) override
        {
            Assert::Fail(L"Unexpected call to CreateDrawingSession");
            return E_NOTIMPL;
        }

        IFACEMETHOD(get_Size)(Size* value) override
        {
            Assert::Fail(L"Unexpected call to get_Size");
            return E_NOTIMPL;
        }

        IFACEMETHOD(get_SizeInPixels)(Size* value) override
        {
            Assert::Fail(L"Unexpected call to get_SizeInPixels");
            return E_NOTIMPL;
        }

        IFACEMETHOD(get_Dpi)(float* value) override
        {
            return GetDpiMethod.WasCalled(value);
        }

        IFACEMETHOD(get_Format)(DirectXPixelFormat* value) override
        {
            Assert::Fail(L"Unexpected call to get_Format");
            return E_NOTIMPL;
        }

        IFACEMETHOD(get_BufferCount)(int32_t* value) override
        {
            Assert::Fail(L"Unexpected call to get_BufferCount");
            return E_NOTIMPL;
        }

        IFACEMETHOD(get_AlphaMode)(CanvasAlphaBehavior* value) override
        {
            Assert::Fail(L"Unexpected call to get_AlphaMode");
            return E_NOTIMPL;
        }

        IFACEMETHOD(get_Rotation)(CanvasSwapChainRotation* value) override
        {
            Assert::Fail(L"Unexpected call to get_Rotation");
            return E_NOTIMPL;
        }

        IFACEMETHOD(put_Rotation)(CanvasSwapChainRotation value) override
        {
            Assert::Fail(L"Unexpected call to put_Rotation");
            return E_NOTIMPL;
        }

        IFACEMETHOD(get_SourceSize)(Size* value) override
        {
            Assert::Fail(L"Unexpected call to get_SourceSize");
            return E_NOTIMPL;
        }

        IFACEMETHOD(put_SourceSize)(Size value) override
        {
            Assert::Fail(L"Unexpected call to put_SourceSize");
            return E_NOTIMPL;
        }

        IFACEMETHOD(get_TransformMatrix)(Matrix3x2* value) override
        {
            Assert::Fail(L"Unexpected call to get_TransformMatrix");
            return E_NOTIMPL;
        }

        IFACEMETHOD(put_TransformMatrix)(Matrix3x2 value) override
        {
            Assert::Fail(L"Unexpected call to put_TransformMatrix");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP ConvertPixelsToDips(int pixels, float* dips) override
        {
            Assert::Fail(L"Unexpected call to ConvertPixelsToDips");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP ConvertDipsToPixels(float dips, int* pixels) override
        {
            Assert::Fail(L"Unexpected call to ConvertDipsToPixels");
            return E_NOTIMPL;
        }

        IFACEMETHOD(Present)() override
        {
            Assert::Fail(L"Unexpected call to Present");
            return E_NOTIMPL;
        }

        IFACEMETHOD(PresentWithSyncInterval)(int32_t syncInterval) override
        {
            Assert::Fail(L"Unexpected call to PresentWithSyncInterval");
            return E_NOTIMPL;
        }

        IFACEMETHOD(ResizeBuffersWithSize)(
            float newWidth,
            float newHeight) override
        {
            return E_NOTIMPL;
        }

        IFACEMETHOD(ResizeBuffersWithAllOptions)(
            float newWidth,
            float newHeight,
            DirectXPixelFormat newFormat,
            int32_t bufferCount) override
        {
            Assert::Fail(L"Unexpected call to ResizeBuffers");
            return E_NOTIMPL;
        }

        // IClosable
        IFACEMETHOD(Close)() override
        {
            Assert::Fail(L"Unexpected call to Close");
            return E_NOTIMPL;
        }

        // ICanvasResourceCreator
        IFACEMETHOD(get_Device)(ICanvasDevice** value) override
        {
            Assert::Fail(L"Unexpected call to get_Device");
            return E_NOTIMPL;
        }

        // ICanvasResourceWrapperNative
        IFACEMETHOD(GetResource)(const IID& iid, void** out) override
        {
            return GetResourceMethod.WasCalled(iid, out);
        }

    };
}
