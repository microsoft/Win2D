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

ComPtr<ID2D1DeviceContext1> CreateTestD2DDeviceContext(CanvasDevice^ device)
{
    if (!device)
        device = ref new CanvasDevice();
    auto d2dDevice = GetWrappedResource<ID2D1Device1>(device);

    ComPtr<ID2D1DeviceContext1> context;
    ThrowIfFailed(d2dDevice->CreateDeviceContext(
        D2D1_DEVICE_CONTEXT_OPTIONS_NONE,
        &context));

    return context;
}

ComPtr<ID2D1Bitmap1> CreateTestD2DBitmap(D2D1_BITMAP_OPTIONS options, ComPtr<ID2D1DeviceContext1> context)
{
    if (!context)
        context = CreateTestD2DDeviceContext();

    ComPtr<ID2D1Bitmap1> d2dBitmap;

    D2D1_BITMAP_PROPERTIES1 bitmapProperties = D2D1::BitmapProperties1();
    bitmapProperties.bitmapOptions = options;
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

void VerifyDpiAndAlpha(ComPtr<ID2D1Bitmap1> const& d2dBitmap, float expectedDpi, D2D1_ALPHA_MODE expectedAlphaMode, float dpiTolerance)
{
    float dpiX, dpiY;
    d2dBitmap->GetDpi(&dpiX, &dpiY);

    Assert::AreEqual(expectedDpi, dpiX, dpiTolerance);
    Assert::AreEqual(expectedDpi, dpiY, dpiTolerance);

    Assert::AreEqual(expectedAlphaMode, d2dBitmap->GetPixelFormat().alphaMode);
}
