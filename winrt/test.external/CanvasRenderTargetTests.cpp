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
#if (WINVER > 0x0603)
using namespace Windows::Graphics::DirectX;
#else
using namespace Microsoft::Graphics::Canvas::DirectX;
#endif
using namespace Microsoft::WRL::Wrappers;
using namespace Windows::Foundation;
using namespace Windows::UI;

TEST_CLASS(CanvasRenderTargetTests)
{
    TEST_METHOD(CanvasRenderTarget_ZeroSized)
    {
        CanvasDevice^ canvasDevice = ref new CanvasDevice();

        // 
        // While this scenario is impractical and uncommon, verify it does not 
        // throw any errors. 
        //
        CanvasRenderTarget^ renderTarget = ref new CanvasRenderTarget(canvasDevice, 0, 0, DEFAULT_DPI);
        Assert::AreEqual(0.0f, renderTarget->Size.Width);
        Assert::AreEqual(0.0f, renderTarget->Size.Height);
        Assert::AreEqual(0u, renderTarget->SizeInPixels.Width);
        Assert::AreEqual(0u, renderTarget->SizeInPixels.Height);

        CanvasDrawingSession^ drawingSession = renderTarget->CreateDrawingSession();
        drawingSession->Clear(Colors::White);
        delete drawingSession;
    }


    TEST_METHOD(CanvasRenderTarget_NativeInterop)
    {
        auto canvasDevice = ref new CanvasDevice();
        auto originalRenderTarget = ref new CanvasRenderTarget(canvasDevice, 1, 1, DEFAULT_DPI);
        auto originalD2DBitmap = GetWrappedResource<ID2D1Bitmap1>(originalRenderTarget);

        //
        // GetOrCreate via CanvasDevice
        //
        auto newRenderTarget = GetOrCreate<CanvasRenderTarget>(canvasDevice, originalD2DBitmap.Get());
        auto newD2DBitmap = GetWrappedResource<ID2D1Bitmap1>(newRenderTarget);

        Assert::AreEqual(originalRenderTarget, newRenderTarget);
        Assert::AreEqual(originalD2DBitmap.Get(), newD2DBitmap.Get());

        //
        // GetOrCreate via ID2D1Device
        //
        auto d2dDevice = GetWrappedResource<ID2D1Device1>(canvasDevice);
        newRenderTarget = GetOrCreate<CanvasRenderTarget>(d2dDevice.Get(), originalD2DBitmap.Get());
        newD2DBitmap = GetWrappedResource<ID2D1Bitmap1>(newRenderTarget);

        Assert::AreEqual(originalRenderTarget, newRenderTarget);
        Assert::AreEqual(originalD2DBitmap.Get(), newD2DBitmap.Get());
    }
};
