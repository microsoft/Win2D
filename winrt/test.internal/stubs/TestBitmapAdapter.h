// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

class TestBitmapAdapter : public CanvasBitmapAdapter
{
    ComPtr<IWICBitmapSource> m_converter;

public:
    std::function<void()> MockCreateWicBitmapSource;

    TestBitmapAdapter(ComPtr<IWICFormatConverter> converter)
        : m_converter(converter)
    {
    }

    virtual WicBitmapSource CreateWicBitmapSource(ICanvasDevice* device, HSTRING fileName, bool tryEnableIndexing) override
    {
        if (MockCreateWicBitmapSource)
            MockCreateWicBitmapSource();
        return WicBitmapSource{ m_converter, WICBitmapTransformRotate0 };
    }

    virtual WicBitmapSource CreateWicBitmapSource(ICanvasDevice* device, IStream* fileStream, bool tryEnableIndexing) override
    {
        Assert::Fail(); // Unexpected
        return WicBitmapSource{ m_converter, WICBitmapTransformRotate0 };
    }

    virtual ComPtr<IWICBitmapSource> CreateFlipRotator(
            ComPtr<IWICBitmapSource> const& source,
            WICBitmapTransformOptions transformOptions) override
    {
        return source;
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
