// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

class TestBitmapAdapter : public CanvasBitmapAdapter
{
    ComPtr<IWICBitmapSource> m_converter;

public:
    std::function<void()> MockCreateWICFormatConverter;

    TestBitmapAdapter(ComPtr<IWICFormatConverter> converter)
        : m_converter(converter)
    {
    }

    ComPtr<IWICBitmapSource> CreateWICFormatConverter(HSTRING fileName)
    {
        if (MockCreateWICFormatConverter)
            MockCreateWICFormatConverter();
        return m_converter;
    }

    ComPtr<IWICBitmapSource> CreateWICFormatConverter(IStream* fileStream)
    {
        Assert::Fail(); // Unexpected
        return nullptr;
    }

    virtual void SaveLockedMemoryToFile(
        HSTRING fileName,
        CanvasBitmapFileFormat fileFormat,
        float quality,
        unsigned int width,
        unsigned int height,
        float dpiX,
        float dpiY,
        ScopedBitmapMappedPixelAccess* bitmapLock)
    {
        Assert::Fail(); // Unexpected
    }


    virtual void SaveLockedMemoryToStream(
        IRandomAccessStream* stream,
        CanvasBitmapFileFormat fileFormat,
        float quality,
        unsigned int width,
        unsigned int height,
        float dpiX,
        float dpiY,
        ScopedBitmapMappedPixelAccess* bitmapLock)
    {
        Assert::Fail(); // Unexpected
    }
};


inline ComPtr<CanvasBitmap> CreateStubCanvasBitmap(float dpi = DEFAULT_DPI, ICanvasDevice* device = nullptr)
{
    return Make<CanvasBitmap>(device, Make<StubD2DBitmap>(D2D1_BITMAP_OPTIONS_NONE, dpi).Get());
}


inline ComPtr<ICanvasDrawingSession> CreateStubDrawingSession()
{
    auto d2dDeviceContext = Make<MockD2DDeviceContext>();

    d2dDeviceContext->GetDeviceMethod.AllowAnyCallAlwaysCopyValueToParam(Make<StubD2DDevice>());

    d2dDeviceContext->CreateEffectMethod.AllowAnyCall(
        [&](IID const&, ID2D1Effect** effect)
        {
            auto mockEffect = Make<MockD2DEffect>();
            return mockEffect.CopyTo(effect);
        });

    return Make<CanvasDrawingSession>(d2dDeviceContext.Get());
}
