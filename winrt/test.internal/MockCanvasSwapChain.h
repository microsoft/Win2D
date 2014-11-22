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

        IFACEMETHOD(CreateDrawingSession)(
            Color clearColor,
            ICanvasDrawingSession** drawingSession) override
        {
            return E_NOTIMPL;
        }

        IFACEMETHOD(get_Width)(int32_t* value) override
        {
            return E_NOTIMPL;
        }

        IFACEMETHOD(get_Height)(int32_t* value) override
        {
            return E_NOTIMPL;
        }

        IFACEMETHOD(get_Format)(DirectXPixelFormat* value) override
        {
            return E_NOTIMPL;
        }

        IFACEMETHOD(get_BufferCount)(int32_t* value) override
        {
            return E_NOTIMPL;
        }

        IFACEMETHOD(get_AlphaMode)(CanvasAlphaBehavior* value) override
        {
            return E_NOTIMPL;
        }

        IFACEMETHOD(Present)() override
        {
            return E_NOTIMPL;
        }

        IFACEMETHOD(ResizeBuffers)(
            int32_t bufferCount,
            int32_t newWidth,
            int32_t newHeight,
            DirectXPixelFormat newFormat) override
        {
            return E_NOTIMPL;
        }

        // IClosable
        IFACEMETHOD(Close)() override
        {
            return E_NOTIMPL;
        }

        // ICanvasResourceCreator
        IFACEMETHOD(get_Device)(ICanvasDevice** value) override
        {
            return E_NOTIMPL;
        }

        // ICanvasResourceWrapperNative
        IFACEMETHOD(GetResource)(const IID& iid, void** out) override
        {
            return GetResourceMethod.WasCalled(iid, out);
        }

    };
}
