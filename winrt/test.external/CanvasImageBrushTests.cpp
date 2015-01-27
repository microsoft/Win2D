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
using namespace Windows::UI;

static CanvasBitmap^ CreateArbitraryCanvasBitmap(CanvasDevice^ device)
{ 
    return CanvasBitmap::CreateFromColors(
        device,
        ref new Platform::Array<Color>(0),
        0,
        0,
        CanvasAlphaMode::Premultiplied);
}

static ICanvasImage^ CreateArbitraryEffect()
{
    return ref new Effects::ColorSourceEffect();
}


TEST_CLASS(CanvasImageBrushTests)
{
    CanvasDevice^ m_device;
    ICanvasImage^ m_anyImage;

public:
    CanvasImageBrushTests()
        : m_device(ref new CanvasDevice())
        , m_anyImage(CreateArbitraryCanvasBitmap(m_device))
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
        auto anyBitmap = CreateArbitraryCanvasBitmap(m_device);

        auto brush = ref new CanvasImageBrush(m_device);
        brush->Image = anyBitmap;

        Assert::AreEqual<ICanvasImage^>(anyBitmap, brush->Image);
    }

    TEST_METHOD(CanvasImageBrush_ImageProperty_WithRenderTarget)
    {
        auto anyRenderTarget = ref new CanvasRenderTarget(m_device, 1, 1, DEFAULT_DPI);

        auto brush = ref new CanvasImageBrush(m_device);
        brush->Image = anyRenderTarget;

        Assert::AreEqual<ICanvasImage^>(anyRenderTarget, brush->Image);
    }

    TEST_METHOD(CanvasImageBrush_ImageProperty_WithEffect)
    {
        auto anyEffect = CreateArbitraryEffect();

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

        Assert::AreEqual(anyRect, brush->SourceRectangle->Value);
    }

    TEST_METHOD(CanvasImageBrush_DefaultSourceRectangle_IsNull)
    {
        auto brush = ref new CanvasImageBrush(m_device);
        Assert::IsNull(brush->SourceRectangle);
    }

    TEST_METHOD(CanvasImageBrush_DefaultSourceRectangle_ForBitmap_IsNull)
    {
        auto brush = ref new CanvasImageBrush(m_device, CreateArbitraryCanvasBitmap(m_device));
        Assert::IsNull(brush->SourceRectangle);
    }

    TEST_METHOD(CanvasImageBrush_DefaultSourceRectangle_ForNonBitmap_IsNull)
    {
        auto brush = ref new CanvasImageBrush(m_device, CreateArbitraryEffect());
        Assert::IsNull(brush->SourceRectangle);
    }
};


TEST_CLASS(CanvasImageBrushBackedByBitmapInteropTests)
{
    CanvasDevice^ m_device;
    CanvasBitmap^ m_bitmap;
    CanvasImageBrush^ m_brush;

public:
    CanvasImageBrushBackedByBitmapInteropTests()
        : m_device(ref new CanvasDevice())
        , m_bitmap(CreateArbitraryCanvasBitmap(m_device))
        , m_brush(ref new CanvasImageBrush(m_device, m_bitmap))
    {
    }

    TEST_METHOD(CanvasImageBrush_BackedByBitmap_GetWrappedResource_GetD2DBitmapBrush)
    {
        auto d2dBitmapBrush = GetWrappedResource<ID2D1BitmapBrush1>(m_brush);

        ValidateBitmapBrush(d2dBitmapBrush);
    }

    TEST_METHOD(CanvasImageBrush_BackedByBitmap_GetWrappedResource_GetD2DBrush)
    {
        auto d2dBrush = GetWrappedResource<ID2D1Brush>(m_brush);
        auto d2dBitmapBrush = As<ID2D1BitmapBrush1>(d2dBrush);

        ValidateBitmapBrush(d2dBitmapBrush);
    }

    TEST_METHOD(CanvasImageBrush_BackedByImage_GetWrappedResource_GetD2DBitmapBrush)
    {
        SetBrushToBeBackedByImageBrush();

        // When the CanvasImageBrush is backed by a D2DImageBrush then it is not
        // possible to get a D2DBitmapBrush out of it.
        Assert::ExpectException<Platform::InvalidCastException^>(
            [&] { GetWrappedResource<ID2D1BitmapBrush1>(m_brush); });
    }

    TEST_METHOD(CanvasImageBrush_BackedByImage_GetWrappedResource_GetD2DBrush)
    {
        SetBrushToBeBackedByImageBrush();

        auto d2dBrush = GetWrappedResource<ID2D1Brush>(m_brush);
        auto d2dImageBrush = As<ID2D1ImageBrush>(d2dBrush);

        auto expectedD2DImage = As<ID2D1Image>(GetWrappedResource<ID2D1Bitmap>(m_bitmap));

        ComPtr<ID2D1Image> actualD2DImage;
        d2dImageBrush->GetImage(&actualD2DImage);

        Assert::AreEqual(expectedD2DImage.Get(), actualD2DImage.Get());
    }

    void ValidateBitmapBrush(ComPtr<ID2D1BitmapBrush1> const& d2dBitmapBrush)
    {
        auto expectedD2DBitmap = GetWrappedResource<ID2D1Bitmap>(m_bitmap);

        ComPtr<ID2D1Bitmap> actualD2DBitmap;
        d2dBitmapBrush->GetBitmap(&actualD2DBitmap);

        Assert::AreEqual(expectedD2DBitmap.Get(), actualD2DBitmap.Get());
    }

    void SetBrushToBeBackedByImageBrush()
    {
        m_brush->SourceRectangle = Rect{0,0,1,1};
    }
};


TEST_CLASS(CanvasImageBrushBackedByNonBitmapImageInteropTests)
{
    CanvasDevice^ m_device;
    ICanvasImage^ m_imageThatIsNotABitmap;
    CanvasImageBrush^ m_brush;

public:
    CanvasImageBrushBackedByNonBitmapImageInteropTests()
        : m_device(ref new CanvasDevice())
        , m_imageThatIsNotABitmap(CreateArbitraryEffect())
        , m_brush(ref new CanvasImageBrush(m_device, m_imageThatIsNotABitmap))
    {
    }

    TEST_METHOD(CanvasImageBrush_BackedByNonBitmap_GetWrappedResource_GetD2DBitmapBrush)
    {
        // When the CanvasImageBrush is backed by a D2DImageBrush then it is not
        // possible to get a D2DBitmapBrush out of it.
        Assert::ExpectException<Platform::InvalidCastException^>(
            [&] { GetWrappedResource<ID2D1BitmapBrush1>(m_brush); });
    }

    TEST_METHOD(CanvasImageBrush_BackedByNonBitmap_GetWrappedResource_GetD2DBrush)
    {
        auto d2dBrush = GetWrappedResource<ID2D1Brush>(m_brush);
        auto d2dImageBrush = As<ID2D1ImageBrush>(d2dBrush);

        ValidateImageBrush(d2dImageBrush);
    }

    TEST_METHOD(CanvasImageBrush_BackedByNonBitmap_GetWrappedResource_GetD2DImageBrush)
    {
        auto d2dImageBrush = GetWrappedResource<ID2D1ImageBrush>(m_brush);

        ValidateImageBrush(d2dImageBrush);
    }

    TEST_METHOD(CanvasImageBrush_BackedByNonBitmap_WithNoSourceRectangleSet_WrappedImageBrush_HasAllZerosSourceRectangle)
    {
        m_brush->SourceRectangle = Rect{1,2,3,4};
        m_brush->SourceRectangle = nullptr;
        auto d2dImageBrush = GetWrappedResource<ID2D1ImageBrush>(m_brush);

        D2D1_RECT_F sourceRectangle;
        d2dImageBrush->GetSourceRectangle(&sourceRectangle);

        //
        // There was some discussion about what the appropriate value to use
        // here.  An all zero rectangle was picked since this most aligns to the
        // idea of a "null" rectangle.  It's what you get if you construct the
        // rectangle using D2D1_RECT_F{}.
        //
        // Generally though, if you set SourceRectangle to null something
        // strange is going to happen.  What is certain though, in this case, is
        // that it shouldn't keep the old Rect{1,2,3,4} value.
        //
        Assert::AreEqual(D2D1_RECT_F{}, sourceRectangle);
    }

    void ValidateImageBrush(ComPtr<ID2D1ImageBrush> const& brush)
    {
        ComPtr<ID2D1Image> image;
        brush->GetImage(&image);

        auto d2dEffect = As<ID2D1Effect>(image);

        // TODO #1697: Without effect interop there's not much more we can check
        // here.  Once we have effect interop we should check that
        // GetOrCreate<>(d2dEffect) matches m_imageThatIsNotABitmap.
    }
};


TEST_CLASS(CanvasImageBrush_GetOrCreateTests)
{
    CanvasDevice^ m_device;
    ComPtr<ID2D1DeviceContext1> m_deviceContext;
    ComPtr<ID2D1Bitmap1> m_d2dBitmap;

public:
    CanvasImageBrush_GetOrCreateTests()
        : m_device(ref new CanvasDevice())
        , m_deviceContext(CreateTestD2DDeviceContext(m_device))
        , m_d2dBitmap(CreateTestD2DBitmap(D2D1_BITMAP_OPTIONS_NONE, m_deviceContext))
    {
    }

    TEST_METHOD(CanvasImageBrush_GetOrCreate_BitmapBrush)
    {
        ComPtr<ID2D1BitmapBrush> d2dBitmapBrush;
        ThrowIfFailed(m_deviceContext->CreateBitmapBrush(m_d2dBitmap.Get(), &d2dBitmapBrush));

        auto brush = GetOrCreate<CanvasImageBrush>(m_device, d2dBitmapBrush.Get());

        auto wrappedD2DBitmapBrush = GetWrappedResource<ID2D1BitmapBrush>(brush);
        Assert::AreEqual(d2dBitmapBrush.Get(), wrappedD2DBitmapBrush.Get());

        auto canvasImage = brush->Image;
        auto canvasBitmap = safe_cast<CanvasBitmap^>(canvasImage);

        auto wrappedD2DBitmap = GetWrappedResource<ID2D1Bitmap1>(canvasBitmap);
        Assert::AreEqual(m_d2dBitmap.Get(), wrappedD2DBitmap.Get());
    }

    TEST_METHOD(CanvasImageBrush_GetOrCreate_ImageBrush)
    {
        auto d2dImageBrush = CreateD2DImageBrush();

        auto brush = GetOrCreate<CanvasImageBrush>(m_device, d2dImageBrush.Get());

        auto wrappedD2DImageBrush = GetWrappedResource<ID2D1ImageBrush>(brush);
        Assert::AreEqual(d2dImageBrush.Get(), wrappedD2DImageBrush.Get());

        auto canvasImage = brush->Image;
        auto canvasBitmap = safe_cast<CanvasBitmap^>(canvasImage);

        auto wrappedD2DBitmap = GetWrappedResource<ID2D1Bitmap1>(canvasBitmap);
        Assert::AreEqual(m_d2dBitmap.Get(), wrappedD2DBitmap.Get());
    }

    TEST_METHOD(CanvasImageBrush_GetOrCreate_ImageBrush_PicksUpSourceRect)
    {
        auto anyD2DRect = D2D1_RECT_F{1,2,3,4};
        auto anyRect = Rect{1,2,2,2};

        auto d2dImageBrush = CreateD2DImageBrush(anyD2DRect);

        auto brush = GetOrCreate<CanvasImageBrush>(m_device, d2dImageBrush.Get());

        Assert::IsNotNull(brush->SourceRectangle);
        Assert::AreEqual(anyRect, brush->SourceRectangle->Value);
    }

    TEST_METHOD(CanvasImageBrush_GetOrCreate_ImageBrushWithAllZeroSourceRect_RemainsImageBrush)
    {
        auto allZeroRect = D2D1_RECT_F{0,0,0,0};
        auto d2dImageBrush = CreateD2DImageBrush(allZeroRect);

        auto brush = GetOrCreate<CanvasImageBrush>(m_device, d2dImageBrush.Get());

        auto wrappedD2DImageBrush = GetWrappedResource<ID2D1ImageBrush>(brush);

        Assert::IsTrue(IsSameInstance(d2dImageBrush.Get(), wrappedD2DImageBrush.Get()));
    }

    TEST_METHOD(CanvasImageBrush_GetOrCreate_IsNotIdempotent)
    {
        //
        // Since CanvasImageBrush wraps multiple D2D resources and tracks state
        // not necessarily stored in the underlying resource, GetOrCreate does
        // not attempt to be idempotent.
        //

        auto d2dImageBrush = CreateD2DImageBrush();

        auto brush1 = GetOrCreate<CanvasImageBrush>(m_device, d2dImageBrush.Get());
        auto brush2 = GetOrCreate<CanvasImageBrush>(m_device, d2dImageBrush.Get());

        Assert::IsFalse(IsSameInstance(reinterpret_cast<IInspectable*>(brush1), reinterpret_cast<IInspectable*>(brush2)));
    }

    ComPtr<ID2D1ImageBrush> CreateD2DImageBrush(D2D1_RECT_F rect = D2D1_RECT_F{})
    {
        ComPtr<ID2D1ImageBrush> d2dImageBrush;
        ThrowIfFailed(m_deviceContext->CreateImageBrush(m_d2dBitmap.Get(), D2D1_IMAGE_BRUSH_PROPERTIES{rect}, &d2dImageBrush));
        return d2dImageBrush;
    }
};

