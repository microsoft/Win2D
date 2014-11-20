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

#include "pch.h"

using namespace Microsoft::Graphics::Canvas;
using namespace Microsoft::WRL::Wrappers;
using namespace Windows::Foundation;
using namespace Windows::Devices::Enumeration;
using namespace Microsoft::Graphics::Canvas::DirectX;
using namespace Windows::Graphics::Imaging;
using namespace Windows::UI;
using namespace Windows::Storage::Streams;
using Platform::String;

TEST_CLASS(CanvasSwapChainTests)
{
    TEST_METHOD(CanvasSwapChain_NativeInterop)
    {
        auto canvasDevice = ref new CanvasDevice();

        auto canvasSwapChain = ref new CanvasSwapChain(canvasDevice, 1, 2);

        auto dxgiSwapChain = GetWrappedResource<IDXGISwapChain2>(canvasSwapChain);

        DXGI_SWAP_CHAIN_DESC1 swapChainDescription;
        ThrowIfFailed(dxgiSwapChain->GetDesc1(&swapChainDescription));
        Assert::AreEqual(1u, swapChainDescription.Width);
        Assert::AreEqual(2u, swapChainDescription.Height);

        auto newCanvasSwapChain = GetOrCreate<CanvasSwapChain>(canvasDevice, dxgiSwapChain.Get());
        auto newDxgiSwapChain = GetWrappedResource<IDXGISwapChain2>(newCanvasSwapChain);

        Assert::AreEqual(canvasSwapChain, newCanvasSwapChain);
        Assert::AreEqual(dxgiSwapChain.Get(), newDxgiSwapChain.Get());
    }

    TEST_METHOD(CanvasSwapChain_DrawOperation)
    {
        auto canvasDevice = ref new CanvasDevice(CanvasDebugLevel::Information);

        auto canvasSwapChain = ref new CanvasSwapChain(canvasDevice, 1, 1);

        Color referenceColor = Colors::Red;

        auto drawingSession = canvasSwapChain->CreateDrawingSession(referenceColor);
        delete drawingSession;

        auto dxgiSwapChain = GetWrappedResource<IDXGISwapChain2>(canvasSwapChain);

        ComPtr<IDXGISurface> dxgiSwapChainSurface;
        ThrowIfFailed(dxgiSwapChain->GetBuffer(0, IID_PPV_ARGS(&dxgiSwapChainSurface)));

        auto wrappedSwapChainSurface = CreateDirect3DSurface(dxgiSwapChainSurface.Get());

        auto swapChainRenderTarget = CanvasRenderTarget::CreateFromDirect3D11Surface(canvasDevice, wrappedSwapChainSurface);

        auto color = swapChainRenderTarget->GetPixelColors();

        Assert::AreEqual(1u, color->Length);
        Assert::AreEqual(referenceColor, color[0]);
    }
};
