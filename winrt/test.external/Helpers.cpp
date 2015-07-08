// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

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

void VerifyDpiAndAlpha(ComPtr<ID2D1Bitmap1> const& d2dBitmap, float expectedDpi, D2D1_ALPHA_MODE expectedAlphaMode)
{
    float dpiX, dpiY;
    d2dBitmap->GetDpi(&dpiX, &dpiY);

    Assert::AreEqual(expectedDpi, dpiX);
    Assert::AreEqual(expectedDpi, dpiY);

    Assert::AreEqual(expectedAlphaMode, d2dBitmap->GetPixelFormat().alphaMode);
}

WicBitmapTestFixture CreateWicBitmapTestFixture()
{
    WicBitmapTestFixture result;

    ComPtr<IWICImagingFactory2> wicImagingFactory;
    ThrowIfFailed(CoCreateInstance(
        CLSID_WICImagingFactory,
        nullptr,
        CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(&wicImagingFactory)));

    ComPtr<IWICBitmap> wicBitmap;
    ThrowIfFailed(wicImagingFactory->CreateBitmap(1, 1, GUID_WICPixelFormat32bppPBGRA, WICBitmapCacheOnDemand, &wicBitmap));

    ComPtr<ID2D1Factory2> d2dFactory;
    ThrowIfFailed(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, d2dFactory.GetAddressOf()));

    ComPtr<ID2D1RenderTarget> d2dWicBitmapRenderTarget;
    ThrowIfFailed(d2dFactory->CreateWicBitmapRenderTarget(
        wicBitmap.Get(),
        D2D1::RenderTargetProperties(
        D2D1_RENDER_TARGET_TYPE_DEFAULT,
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)),
        d2dWicBitmapRenderTarget.GetAddressOf()));

    result.RenderTarget = As<ID2D1DeviceContext1>(d2dWicBitmapRenderTarget);

    //
    // Interesting note- D2D doesn't try and prevent you from using the same WIC bitmap as both
    // a target and a potential source.
    //
    ThrowIfFailed(result.RenderTarget->CreateBitmapFromWicBitmap(wicBitmap.Get(), &result.Bitmap));

    return result;
}
