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
        CanvasRenderTarget^ renderTarget = ref new CanvasRenderTarget(canvasDevice, Size(0, 0));
        Assert::AreEqual(0.0f, renderTarget->Size.Width);
        Assert::AreEqual(0.0f, renderTarget->Size.Height);
        Assert::AreEqual(0.0f, renderTarget->SizeInPixels.Width);
        Assert::AreEqual(0.0f, renderTarget->SizeInPixels.Height);

        CanvasDrawingSession^ drawingSession = renderTarget->CreateDrawingSession();
        drawingSession->Clear(Colors::White);
        delete drawingSession;
    }


    TEST_METHOD(CanvasRenderTarget_NativeInterop)
    {
        auto canvasDevice = ref new CanvasDevice();

        auto originalRenderTarget = ref new CanvasRenderTarget(canvasDevice, Size(1, 1));
        auto originalD2DBitmap = GetWrappedResource<ID2D1Bitmap1>(originalRenderTarget);
        auto newRenderTarget = GetOrCreate<CanvasRenderTarget>(originalD2DBitmap.Get());
        auto newD2DBitmap = GetWrappedResource<ID2D1Bitmap1>(newRenderTarget);

        Assert::AreEqual(originalRenderTarget, newRenderTarget);
        Assert::AreEqual(originalD2DBitmap.Get(), newD2DBitmap.Get());
    }

    ComPtr<ID2D1Bitmap1> CreateTestD2DBitmap()
    {
        ComPtr<ID2D1DeviceContext1> context = CreateTestD2DDeviceContext();
        ComPtr<ID2D1Bitmap1> d2dBitmap;

        D2D1_BITMAP_PROPERTIES1 bitmapProperties = D2D1::BitmapProperties1();
        bitmapProperties.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET;
        bitmapProperties.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
        bitmapProperties.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;

        ThrowIfFailed(context->CreateBitmap(
            D2D1::SizeU(1, 1),
            nullptr, // data 
            0,  // data pitch
            &bitmapProperties,
            &d2dBitmap));

        return d2dBitmap;
    }
};
