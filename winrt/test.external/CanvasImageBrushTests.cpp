// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

using namespace Microsoft::Graphics::Canvas;
using namespace Microsoft::Graphics::Canvas::Effects;
using namespace WinRTDirectX;
using namespace Windows::UI;

static CanvasBitmap^ CreateArbitraryCanvasBitmap(CanvasDevice^ device)
{ 
    return CanvasBitmap::CreateFromColors(
        device,
        ref new Platform::Array<Color>(0),
        0,
        0,
        DEFAULT_DPI,
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

    TEST_METHOD(CanvasImageBrush_Construct_WithBitmap)
    {
        auto anyBitmap = CreateArbitraryCanvasBitmap(m_device);

        auto brush = ref new CanvasImageBrush(m_device, anyBitmap);

        Assert::AreEqual<ICanvasImage>(anyBitmap, brush->Image);
    }

    TEST_METHOD(CanvasImageBrush_Construct_WithImage)
    {
        auto anyImage = ref new CanvasCommandList(m_device);

        auto brush = ref new CanvasImageBrush(m_device, anyImage);

        Assert::AreEqual<ICanvasImage>(anyImage, brush->Image);
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

    TEST_METHOD(CanvasImageBrush_ImageProperty_WithCommandList)
    {
        auto anyCommandList = ref new CanvasCommandList(m_device);

        auto brush = ref new CanvasImageBrush(m_device);
        brush->Image = anyCommandList;

        Assert::AreEqual<ICanvasImage^>(anyCommandList, brush->Image);
    }

    TEST_METHOD(CanvasImageBrush_ImageProperty_WithEffect)
    {
        auto anyEffect = CreateArbitraryEffect();

        auto brush = ref new CanvasImageBrush(m_device);
        brush->Image = anyEffect;

        Assert::AreEqual<ICanvasImage^>(anyEffect, brush->Image);
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

    TEST_METHOD(CanvasImageBrush_AccessBaseBrushPropertiesBeforeSettingSourceRect)
    {
        auto brush = ref new CanvasImageBrush(m_device, CreateArbitraryEffect());

        float opacity = 23;
        float3x2 transform{ 1, 2, 3, 4, 5, 6 };

        brush->Opacity = opacity;
        brush->Transform = transform;

        Assert::AreEqual(opacity, brush->Opacity);
        Assert::AreEqual<float3x2>(transform, brush->Transform);
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

        Assert::AreEqual(m_imageThatIsNotABitmap, GetOrCreate<ICanvasImage>(d2dEffect.Get()));
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

    TEST_METHOD(CanvasImageBrush_GetOrCreate_IsIdempotent)
    {
        auto d2dImageBrush = CreateD2DImageBrush();

        auto brush1 = GetOrCreate<CanvasImageBrush>(m_device, d2dImageBrush.Get());
        auto brush2 = GetOrCreate<CanvasImageBrush>(m_device, d2dImageBrush.Get());

        Assert::AreEqual(brush1, brush2);
    }

    TEST_METHOD(CanvasImageBrush_GetResource_WrongDevice)
    {
        auto brush = ref new CanvasImageBrush(m_device);

        auto otherDevice = ref new CanvasDevice();

        ExpectCOMException(E_INVALIDARG, L"Existing resource wrapper is associated with a different device.",
            [&]
            {
                GetWrappedResource<ID2D1Brush>(otherDevice, brush);
            });
    }

    TEST_METHOD(CanvasImageBrush_GetResource_WhenSourceIsEffect_AppliesDpiCompensation)
    {
        // Create an image brush.
        auto brush = ref new CanvasImageBrush(m_device);
        brush->SourceRectangle = Rect{ 0, 0, 1, 1 };
        auto d2dBrush = GetWrappedResource<ID2D1ImageBrush>(brush);

        // Create a bitmap image.
        auto image = ref new CanvasRenderTarget(m_device, 1, 1, DEFAULT_DPI);
        auto d2dImage = GetWrappedResource<ID2D1Image>(image);

        // Create an effect, with the bitmap as its source.
        auto effect = ref new GaussianBlurEffect();
        effect->Source = image;
        auto d2dEffect = GetWrappedResource<ID2D1Effect>(m_device, effect);

        // Set the effect as the brush image, purely at the D2D level (not using Win2D APIs).
        d2dBrush->SetImage(As<ID2D1Image>(d2dEffect).Get());

        // Looking up the ID2D1ImageBrush a second time should return the same instance as before.
        auto d2dBrush2 = GetWrappedResource<ID2D1ImageBrush>(brush);
        Assert::IsTrue(IsSameInstance(d2dBrush.Get(), d2dBrush2.Get()));

        // DPI compensation fixups should have been applied to the effect.
        ComPtr<ID2D1Image> effectInput;
        d2dEffect->GetInput(0, &effectInput);
        Assert::IsFalse(IsSameInstance(effectInput.Get(), d2dImage.Get()));

        auto dpiEffect = As<ID2D1Effect>(effectInput);
        Assert::IsTrue(dpiEffect->GetValue<IID>(D2D1_PROPERTY_CLSID) == CLSID_D2D1DpiCompensation);

        dpiEffect->GetInput(0, &effectInput);
        Assert::IsTrue(IsSameInstance(effectInput.Get(), d2dImage.Get()));

        // Win2D property getter should notice that we changed the brush source.
        Assert::IsTrue(brush->Image == effect);

        // DPI compensation should not be visible in the Win2D view of the effect graph.
        Assert::IsTrue(effect->Source == image);
    }

    TEST_METHOD(CanvasImageBrush_GetResource_WhenSourceIsEffectOnWrongDevice_RerealizesAccordingly)
    {
        // Create an image brush.
        auto brush = ref new CanvasImageBrush(m_device);
        brush->SourceRectangle = Rect{ 0, 0, 1, 1 };

        // Create an effect, and set it as the brush image.
        auto effect = ref new ColorSourceEffect();
        brush->Image = effect;

        // Validate that the proper D2D resources were set.
        auto d2dBrush = GetWrappedResource<ID2D1ImageBrush>(brush);
        auto d2dEffect1 = GetWrappedResource<ID2D1Effect>(m_device, effect);

        ComPtr<ID2D1Image> brushImage;
        d2dBrush->GetImage(&brushImage);
        Assert::IsTrue(IsSameInstance(d2dEffect1.Get(), brushImage.Get()));

        // Realize the effect onto a second device.
        auto device2 = ref new CanvasDevice();
        auto d2dEffect2 = GetWrappedResource<ID2D1Effect>(device2, effect);

        // D2D brush state should still be pointing at the now orphaned realized effect,
        // but the Win2D CanvasImageBrush.Image property getter still knows what effect this is.
        d2dBrush->GetImage(&brushImage);
        Assert::IsTrue(IsSameInstance(d2dEffect1.Get(), brushImage.Get()));

        Assert::AreEqual<ICanvasImage>(effect, brush->Image);

        // Setting a property on the effect should be reflected on its second D2D instance, but not the first.
        effect->Color = Colors::Red;

        Assert::AreEqual(float4(0, 0, 0, 1), d2dEffect1->GetValue<float4>(D2D1_FLOOD_PROP_COLOR));
        Assert::AreEqual(float4(1, 0, 0, 1), d2dEffect2->GetValue<float4>(D2D1_FLOOD_PROP_COLOR));

        // Now use the brush. This should realize the effect back over to the original device.
        auto d2dBrush2 = GetWrappedResource<ID2D1ImageBrush>(brush);
     
        Assert::IsTrue(IsSameInstance(d2dBrush.Get(), d2dBrush2.Get()));

        Assert::AreEqual<ICanvasImage>(effect, brush->Image);

        d2dBrush->GetImage(&brushImage);

        Assert::AreEqual(effect, GetOrCreate<ColorSourceEffect>(m_device, brushImage.Get()));

        Assert::IsFalse(IsSameInstance(d2dEffect1.Get(), brushImage.Get()));
        Assert::IsFalse(IsSameInstance(d2dEffect2.Get(), brushImage.Get()));

        effect->Color = Colors::Blue;
        Assert::AreEqual(float4(0, 0, 1, 1), As<ID2D1Effect>(brushImage)->GetValue<float4>(D2D1_FLOOD_PROP_COLOR));
    }

    ComPtr<ID2D1ImageBrush> CreateD2DImageBrush(D2D1_RECT_F rect = D2D1_RECT_F{})
    {
        ComPtr<ID2D1ImageBrush> d2dImageBrush;
        ThrowIfFailed(m_deviceContext->CreateImageBrush(m_d2dBitmap.Get(), D2D1_IMAGE_BRUSH_PROPERTIES{rect}, &d2dImageBrush));
        return d2dImageBrush;
    }
};

