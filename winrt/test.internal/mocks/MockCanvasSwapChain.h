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
    class MockCanvasSwapChain : public CanvasSwapChain
    {
    public:
        CALL_COUNTER_WITH_MOCK(PresentMethod, HRESULT());
        CALL_COUNTER_WITH_MOCK(CreateDrawingSessionMethod, HRESULT(Color, ICanvasDrawingSession**));
        CALL_COUNTER_WITH_MOCK(put_TransformMethod, HRESULT(Matrix3x2));

        MockCanvasSwapChain(
            ICanvasDevice* device,
            std::shared_ptr<CanvasSwapChainManager> manager,
            IDXGISwapChain2* dxgiSwapChain,
            float dpi)
            : CanvasSwapChain(device, manager, dxgiSwapChain, dpi)
        {
            CreateDrawingSessionMethod.AllowAnyCall();
        }

        IFACEMETHOD(CreateDrawingSession)(
            Color clearColor,
            ICanvasDrawingSession** drawingSession) override
        {
            auto hadMock = CreateDrawingSessionMethod.HasMock();

            auto result = CreateDrawingSessionMethod.WasCalled(clearColor, drawingSession);

            if (hadMock)
                return result;
            else
                return CanvasSwapChain::CreateDrawingSession(clearColor, drawingSession);
        }

        IFACEMETHOD(get_Size)(Size* value) override
        {
            Assert::Fail(L"Unexpected call to get_Size");
            return E_NOTIMPL;
        }

        IFACEMETHOD(get_SizeInPixels)(BitmapSize* value) override
        {
            Assert::Fail(L"Unexpected call to get_SizeInPixels");
            return E_NOTIMPL;
        }

        IFACEMETHOD(get_Dpi)(float* value) override
        {
            return CanvasSwapChain::get_Dpi(value);
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

        IFACEMETHOD(get_AlphaMode)(CanvasAlphaMode* value) override
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
            return put_TransformMethod.WasCalled(value);
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
            return PresentMethod.WasCalled();
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
            return CanvasSwapChain::get_Device(value);
        }

        // ICanvasResourceWrapperNative
        IFACEMETHOD(GetResource)(const IID& iid, void** out) override
        {
            return CanvasSwapChain::GetResource(iid, out);
        }

    };

    class MockCanvasSwapChainManager : public CanvasSwapChainManager
    {
    public:
        ComPtr<MockCanvasSwapChain> CreateMock(
            ICanvasDevice* device = Make<StubCanvasDevice>().Get(),
            IDXGISwapChain2* dxgiSwapChain = MakeMockSwapChain().Get(),
            float dpi = DEFAULT_DPI)
        {
            auto swapChain = GetOrCreate(device, dxgiSwapChain, dpi);
            ComPtr<MockCanvasSwapChain> mockSwapChain(dynamic_cast<MockCanvasSwapChain*>(swapChain.Get()));
            Assert::IsNotNull(mockSwapChain.Get());
            return mockSwapChain;
        }

        virtual ComPtr<CanvasSwapChain> CreateWrapper(
            ICanvasDevice* device,
            IDXGISwapChain2* resource,
            float dpi) override
        {
            return Make<MockCanvasSwapChain>(device, shared_from_this(), resource, dpi);
        }

    private:
        static ComPtr<IDXGISwapChain2> MakeMockSwapChain()
        {
            auto dxgiSwapChain = Make<MockDxgiSwapChain>();

            dxgiSwapChain->SetMatrixTransformMethod.AllowAnyCall(
                [] (const DXGI_MATRIX_3X2_F* value)
                {
                    return S_OK;
                });
            
            dxgiSwapChain->GetDesc1Method.AllowAnyCall(
                [] (DXGI_SWAP_CHAIN_DESC1* desc)
                {
                    desc->Width = 1;
                    desc->Height = 1;
                    desc->Format = DXGI_FORMAT_B8G8R8A8_UNORM;
                    desc->BufferCount = 2;
                    desc->AlphaMode = DXGI_ALPHA_MODE_IGNORE;
                    return S_OK;
                });

            dxgiSwapChain->GetBufferMethod.AllowAnyCall(
                [] (UINT index, const IID& iid, void** out)
                {
                    Assert::AreEqual(__uuidof(IDXGISurface2), iid);
                    auto surface = Make<MockDxgiSurface>();

                    return surface.CopyTo(reinterpret_cast<IDXGISurface2**>(out));
                });

            return dxgiSwapChain;
        }
    };
}
