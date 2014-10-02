// Copyright (c) Microsoft Corporation.  All rights reserved

#include "pch.h"

using namespace Microsoft::Graphics::Canvas;
using namespace Windows::UI;

TEST_CLASS(CanvasImageBrushTests)
{
    CanvasDevice^ m_device;
    ICanvasImage^ m_anyImage;

public:
    CanvasImageBrushTests()
        : m_device(ref new CanvasDevice())
        , m_anyImage(ref new CanvasRenderTarget(m_device, Size{1,1}))
    {        
    }

    TEST_METHOD(CanvasImageBrush_Construct)
    {
        auto brush = ref new CanvasImageBrush(m_device);

        Assert::AreEqual<ICanvasImage^>(nullptr, brush->Image);
    }

    TEST_METHOD(CanvasImageBrush_Construct_WithImage)
    {
        auto brush = ref new CanvasImageBrush(m_device, m_anyImage);

        Assert::AreEqual(m_anyImage, brush->Image);
    }

    TEST_METHOD(CanvasImageBrush_ImageProperty_WithBitmap)
    {
        auto anyBitmap = CanvasBitmap::CreateFromColors(
            m_device,
            ref new Platform::Array<Color>(0),
            0,
            0,
            CanvasAlphaBehavior::Premultiplied);

        auto brush = ref new CanvasImageBrush(m_device);
        brush->Image = anyBitmap;

        Assert::AreEqual<ICanvasImage^>(anyBitmap, brush->Image);
    }

    TEST_METHOD(CanvasImageBrush_ImageProperty_WithRenderTarget)
    {
        auto anyRenderTarget = ref new CanvasRenderTarget(m_device, Size{1,1});

        auto brush = ref new CanvasImageBrush(m_device);
        brush->Image = anyRenderTarget;

        Assert::AreEqual<ICanvasImage^>(anyRenderTarget, brush->Image);
    }

    TEST_METHOD(CanvasImageBrush_ImageProperty_WithEffect)
    {
        auto anyEffect = ref new Effects::GaussianBlurEffect();
        anyEffect->Source = m_anyImage;

        auto brush = ref new CanvasImageBrush(m_device);
        brush->Image = anyEffect;

        // TODO #2630: get_Image needs to support effects
        Assert::ExpectException<Platform::NotImplementedException^>(
            [&] { Assert::AreEqual<ICanvasImage^>(anyEffect, brush->Image); });
    }
};

