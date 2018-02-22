// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

using namespace Microsoft::Graphics::Canvas;
using namespace Microsoft::WRL::Wrappers;
using namespace WinRTDirectX;
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


    TEST_METHOD(CanvasRenderTarget_Constructors)
    {
        auto creator = ref new StubResourceCreatorWithDpi(ref new CanvasDevice());

        auto renderTarget1 = ref new CanvasRenderTarget(creator, 23, 42);
        auto renderTarget2 = ref new CanvasRenderTarget(creator, Size{ 7, 21 });

        Assert::AreEqual(renderTarget1->Size, Size{ 23, 42 });
        Assert::AreEqual(renderTarget2->Size, Size{ 7, 21 });
    }


    TEST_METHOD(CanvasRenderTarget_MaxSizeError)
    {
        auto device = ref new CanvasDevice();
        auto maxSize = device->MaximumBitmapSizeInPixels;
        auto tooBig = NextValueRepresentableAsFloat(maxSize);
        wchar_t msg[256];

        swprintf_s(msg, L"Cannot create CanvasRenderTarget sized %d x 1; MaximumBitmapSizeInPixels for this device is %d.", tooBig, maxSize);
        ExpectCOMException(E_INVALIDARG, msg, [&] { ref new CanvasRenderTarget(device, static_cast<float>(tooBig), 1, 96); });

        swprintf_s(msg, L"Cannot create CanvasRenderTarget sized 1 x %d; MaximumBitmapSizeInPixels for this device is %d.", tooBig, maxSize);
        ExpectCOMException(E_INVALIDARG, msg, [&] { ref new CanvasRenderTarget(device, 1, static_cast<float>(tooBig), 96); });

        swprintf_s(msg, L"Cannot create CanvasRenderTarget sized %d x 2; MaximumBitmapSizeInPixels for this device is %d.", tooBig, maxSize);
        ExpectCOMException(E_INVALIDARG, msg, [&] { ref new CanvasRenderTarget(device, static_cast<float>(tooBig) / 2, 1, 192); });
    }


    TEST_METHOD(CanvasRenderTarget_NestedBeginDraw)
    {
        auto device = ref new CanvasDevice();
        auto renderTarget = ref new CanvasRenderTarget(device, 1, 1, 96);
        auto drawingSession = renderTarget->CreateDrawingSession();

        ExpectCOMException(
            E_FAIL, 
            L"The last drawing session returned by CreateDrawingSession must be disposed before a new one can be created.",
            [&]
            {
                renderTarget->CreateDrawingSession();
            });
    }
};
