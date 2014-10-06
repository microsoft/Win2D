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

    TEST_METHOD(CanvasImageBrush_SourceRectangle)
    {
        auto brush = ref new CanvasImageBrush(m_device);

        Rect anyRect{1,2,3,4};

        brush->SourceRectangle = anyRect;

        Rect actualRect = brush->SourceRectangle->Value;

        Assert::AreEqual(anyRect, actualRect);
    }
};

