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
#include "TestBitmapResourceCreationAdapter.h"
#include "TestEffect.h"

TEST_CLASS(CanvasImageBrushTests)
{
public:
    class StubRectReference : public RuntimeClass<
        IReference<Rect> >
    {
    public:
        Rect m_rect;

        StubRectReference()
        {
            m_rect = Rect{ 0, 0, 10, 10 };
        }

        StubRectReference(float x, float y, float w, float h)
        {
            m_rect = Rect{ x, y, w, h };
        }

        STDMETHOD(get_Value)(ABI::Windows::Foundation::Rect* value) override
        {
            *value = m_rect;
            return S_OK;
        }
    };

    bool AreReferencedRectsEqual(IReference<Rect>* ref0, IReference<Rect>* ref1)
    {
        // Precondition: Both args non-null.
        assert(ref0 && ref1);

        Rect rect0;
        ThrowIfFailed(ref0->get_Value(&rect0));

        Rect rect1;
        ThrowIfFailed(ref1->get_Value(&rect1));

        return  rect0.X == rect1.X &&
                rect0.Y == rect1.Y &&
                rect0.Width == rect1.Width &&
                rect0.Height == rect1.Height;
    }

    class TestCanvasImageBrushAdapter : public ICanvasImageBrushAdapter
    {
    public:
        virtual ComPtr<IReference<Rect>> CreateRectReference(D2D1_RECT_F const& d2dRect) override
        {
            ComPtr<StubRectReference> ret = Make<StubRectReference>();
            ret->m_rect = FromD2DRect(d2dRect);
            return ret;
        }
    };

    ComPtr<ICanvasDevice> CreateTestDevice()
    {
        auto mockCanvasDevice = Make<MockCanvasDevice>();

        mockCanvasDevice->MockCreateImageBrush =
            [&](ID2D1Image* image)
            {
                return Make<MockD2DImageBrush>();
            };

        mockCanvasDevice->MockCreateBitmapBrush =
            [&](ID2D1Bitmap1* bitmap)
            {
                ComPtr<MockD2DBitmapBrush> bitmapBrush = Make<MockD2DBitmapBrush>();

                bitmapBrush->MockSetBitmap =
                    [&](ID2D1Bitmap* bitmap)
                    {
                    };

                return bitmapBrush;
            };

        return mockCanvasDevice;
    }

    ComPtr<CanvasImageBrush> CreateMinimalTestBrush()
    {
        auto canvasDevice = CreateTestDevice();
        auto adapter = std::make_shared<TestCanvasImageBrushAdapter>();
        return Make<CanvasImageBrush>(canvasDevice.Get(), nullptr, adapter);
    }

    TEST_METHOD(CanvasImageBrush_Implements_Expected_Interfaces)
    {
        auto brush = CreateMinimalTestBrush();

        ASSERT_IMPLEMENTS_INTERFACE(brush, ICanvasImageBrush);
        ASSERT_IMPLEMENTS_INTERFACE(brush, ICanvasBrush);
        ASSERT_IMPLEMENTS_INTERFACE(brush, ICanvasBrushInternal);
        ASSERT_IMPLEMENTS_INTERFACE(brush, ABI::Windows::Foundation::IClosable);
    }

    TEST_METHOD(CanvasImageBrush_Closure)
    {
        auto brush = CreateMinimalTestBrush();
        
        Assert::AreEqual(S_OK, brush->Close());

        ComPtr<ICanvasImage> image;
        Assert::AreEqual(RO_E_CLOSED, brush->get_Image(&image));
        Assert::AreEqual(RO_E_CLOSED, brush->put_Image(image.Get()));

        CanvasEdgeBehavior extend;
        Assert::AreEqual(RO_E_CLOSED, brush->get_ExtendX(&extend));
        Assert::AreEqual(RO_E_CLOSED, brush->put_ExtendX(extend));
        Assert::AreEqual(RO_E_CLOSED, brush->get_ExtendY(&extend));
        Assert::AreEqual(RO_E_CLOSED, brush->put_ExtendY(extend));

        ComPtr<IReference<Rect>> sourceRect;
        Assert::AreEqual(RO_E_CLOSED, brush->get_SourceRectangle(&sourceRect));
        sourceRect = Make<StubRectReference>();
        Assert::AreEqual(RO_E_CLOSED, brush->put_SourceRectangle(sourceRect.Get()));

        CanvasImageInterpolation interpolation;
        Assert::AreEqual(RO_E_CLOSED, brush->get_Interpolation(&interpolation));
        Assert::AreEqual(RO_E_CLOSED, brush->put_Interpolation(interpolation));

        float opacity;
        Assert::AreEqual(RO_E_CLOSED, brush->get_Opacity(&opacity));
        Assert::AreEqual(RO_E_CLOSED, brush->put_Opacity(1));

        Numerics::Matrix3x2 transform;
        Assert::AreEqual(RO_E_CLOSED, brush->get_Transform(&transform));
        Assert::AreEqual(RO_E_CLOSED, brush->put_Transform(transform));
    }

    TEST_METHOD(CanvasImageBrush_Properties_NullArgs)
    {
        auto brush = CreateMinimalTestBrush();

        Assert::AreEqual(E_INVALIDARG, brush->get_Image(nullptr));
        Assert::AreEqual(E_INVALIDARG, brush->get_ExtendX(nullptr));
        Assert::AreEqual(E_INVALIDARG, brush->get_ExtendY(nullptr));
        Assert::AreEqual(E_INVALIDARG, brush->get_SourceRectangle(nullptr));
        Assert::AreEqual(E_INVALIDARG, brush->get_Interpolation(nullptr));
        Assert::AreEqual(E_INVALIDARG, brush->get_Opacity(nullptr));
        Assert::AreEqual(E_INVALIDARG, brush->get_Transform(nullptr));
    }

    template<class BackingBrushType>
    void VerifyCommonBrushProperties(
        ComPtr<CanvasImageBrush> const& brush,
        ComPtr<BackingBrushType> const& backingBrush)
    {
        {
            bool getExtendXCalled = false;
            backingBrush->MockGetExtendModeX =
                [&]()
                {
                    Assert::IsFalse(getExtendXCalled);
                    getExtendXCalled = true;
                    return D2D1_EXTEND_MODE_MIRROR;
                };
            CanvasEdgeBehavior edgeBehavior;
            ThrowIfFailed(brush->get_ExtendX(&edgeBehavior));
            Assert::IsTrue(getExtendXCalled);
            Assert::AreEqual(CanvasEdgeBehavior::Mirror, edgeBehavior);
        }
        {
            bool setExtendXCalled = false;
            backingBrush->MockSetExtendModeX =
                [&](D2D1_EXTEND_MODE extendMode)
                {
                    Assert::IsFalse(setExtendXCalled);
                    setExtendXCalled = true;
                    Assert::AreEqual(D2D1_EXTEND_MODE_MIRROR, extendMode);
                };
            ThrowIfFailed(brush->put_ExtendX(CanvasEdgeBehavior::Mirror));
            Assert::IsTrue(setExtendXCalled);
        }
        {
            bool getExtendYCalled = false;
            backingBrush->MockGetExtendModeY =
                [&]()
                {
                    Assert::IsFalse(getExtendYCalled);
                    getExtendYCalled = true;
                    return D2D1_EXTEND_MODE_WRAP;
                };
            CanvasEdgeBehavior edgeBehavior;
            ThrowIfFailed(brush->get_ExtendY(&edgeBehavior));
            Assert::IsTrue(getExtendYCalled);
            Assert::AreEqual(CanvasEdgeBehavior::Wrap, edgeBehavior);
        }
        {
            bool setExtendYCalled = false;
            backingBrush->MockSetExtendModeY =
                [&](D2D1_EXTEND_MODE extendMode)
                {
                    Assert::IsFalse(setExtendYCalled);
                    setExtendYCalled = true;
                    Assert::AreEqual(D2D1_EXTEND_MODE_WRAP, extendMode);
                };
            ThrowIfFailed(brush->put_ExtendY(CanvasEdgeBehavior::Wrap));
            Assert::IsTrue(setExtendYCalled);
        }
        {
            bool getOpacityCalled = false;
            backingBrush->MockGetOpacity =
                [&]()
                {
                    Assert::IsFalse(getOpacityCalled);
                    getOpacityCalled = true;
                    return 0.6f;
                };
            float opacity;
            ThrowIfFailed(brush->get_Opacity(&opacity));
            Assert::IsTrue(getOpacityCalled);
            Assert::AreEqual(0.6f, opacity);
        }
        {
            bool setOpacityCalled = false;
            backingBrush->MockSetOpacity =
                [&](float opacity)
                {
                    Assert::IsFalse(setOpacityCalled);
                    setOpacityCalled = true;
                    // No sanitization done is supposed to occur.
                    Assert::AreEqual(500.0f, opacity);
                };
            ThrowIfFailed(brush->put_Opacity(500));
            Assert::IsTrue(setOpacityCalled);
        }
        {
            bool getTransformCalled = false;
            D2D1_MATRIX_3X2_F d2dTransform = D2D1::Matrix3x2F(1, 2, 3, 4, 5, 6);
            backingBrush->MockGetTransform =
                [&](D2D1_MATRIX_3X2_F* transform)
                {
                    Assert::IsFalse(getTransformCalled);
                    getTransformCalled = true;
                    *transform = d2dTransform;
                };
            Numerics::Matrix3x2 transform;
            ThrowIfFailed(brush->get_Transform(&transform));
            Assert::IsTrue(getTransformCalled);
            Assert::AreEqual(*(reinterpret_cast<Numerics::Matrix3x2*>(&d2dTransform)), transform);
        }
        {
            bool setTransformCalled = false;
            D2D1_MATRIX_3X2_F d2dTransform = D2D1::Matrix3x2F(1, 2, 3, 4, 5, 6);
            backingBrush->MockSetTransform =
                [&](const D2D1_MATRIX_3X2_F* transform)
            {
                Assert::IsFalse(setTransformCalled);
                setTransformCalled = true;
                Assert::AreEqual(d2dTransform, *transform);
            };
            Numerics::Matrix3x2 transform = *(reinterpret_cast<Numerics::Matrix3x2*>(&d2dTransform));
            ThrowIfFailed(brush->put_Transform(transform));
            Assert::IsTrue(setTransformCalled);
        }

    }

    TEST_METHOD(CanvasImageBrush_DisallowInteropBehaviors)
    {
        auto brush = CreateMinimalTestBrush();

        ComPtr<ICanvasImage> image;
        Assert::AreEqual(E_NOTIMPL, brush->get_Image(&image));

    }

    TEST_METHOD(CanvasImageBrush_BitmapBrushProperties)
    {
        auto canvasDevice = Make<MockCanvasDevice>();
        auto bitmapBrush = Make<MockD2DBitmapBrush>();
        auto adapter = std::make_shared<TestCanvasImageBrushAdapter>();

        canvasDevice->MockCreateBitmapBrush = [&](ID2D1Bitmap1* bitmap) { return bitmapBrush; };

        // Returns a non-functional brush.
        canvasDevice->MockCreateImageBrush = [&](ID2D1Image* image) { return Make<MockD2DImageBrush>(); };
        
        bitmapBrush->MockSetBitmap = [&](ID2D1Bitmap* bitmap) {};

        auto brush = Make<CanvasImageBrush>(canvasDevice.Get(), nullptr, adapter);

        VerifyCommonBrushProperties(brush, bitmapBrush);

        bool setBitmapCalled = false;
        bitmapBrush->MockSetBitmap =
            [&](ID2D1Bitmap* bitmap)
            {
                Assert::IsFalse(setBitmapCalled);
                setBitmapCalled = true;
                Assert::IsNull(bitmap);
            };
        // Bitmaps are not fully supported yet, so this
        // uses a NULL image.
        ThrowIfFailed(brush->put_Image(nullptr));
        Assert::IsTrue(setBitmapCalled);

        bool getInterpolationModeCalled = false;
        bitmapBrush->MockGetInterpolationMode1 =
            [&]()
        {
            Assert::IsFalse(getInterpolationModeCalled);
            getInterpolationModeCalled = true;
            return D2D1_INTERPOLATION_MODE_CUBIC;
        };
        CanvasImageInterpolation interpolation;
        ThrowIfFailed(brush->get_Interpolation(&interpolation));
        Assert::IsTrue(getInterpolationModeCalled);
        Assert::AreEqual(CanvasImageInterpolation::Cubic, interpolation);

        bool setInterpolationModeCalled = false;
        bitmapBrush->MockSetInterpolationMode1 =
            [&](D2D1_INTERPOLATION_MODE interpolationMode)
        {
            Assert::IsFalse(setInterpolationModeCalled);
            setInterpolationModeCalled = true;
            Assert::AreEqual(D2D1_INTERPOLATION_MODE_ANISOTROPIC, interpolationMode);
        };
        ThrowIfFailed(brush->put_Interpolation(CanvasImageInterpolation::Anisotropic));
        Assert::IsTrue(setInterpolationModeCalled);
    }

    class SwitchableTestBrushFixture
    {
    public:
        ComPtr<MockD2DBitmapBrush> m_bitmapBrush;
        ComPtr<MockD2DImageBrush> m_imageBrush;
        ComPtr<ID2D1Image> m_targetImage;
        ComPtr<CanvasImageBrush> m_canvasImageBrush;
        ComPtr<ICanvasImageBrushInternal> m_canvasBrushInternal;
        ComPtr<MockCanvasDevice> m_canvasDevice;

        D2D1_MATRIX_3X2_F m_transform;

        SwitchableTestBrushFixture(bool initializeWithBitmap = false)
            : m_transform(D2D1::Matrix3x2F(1, 2, 3, 4, 5, 6))
        {
            m_canvasDevice = Make<MockCanvasDevice>();
            auto adapter = std::make_shared<TestCanvasImageBrushAdapter>();
            m_bitmapBrush = Make<MockD2DBitmapBrush>();
            m_imageBrush = Make<MockD2DImageBrush>();

            m_canvasDevice->MockGetD2DImage =
                [&](ICanvasImage* canvasImage) -> ComPtr<ID2D1Image>
                {
                    ComPtr<IEffect> effect;
                    ComPtr<ICanvasBitmap> bitmap;
                    if (SUCCEEDED(canvasImage->QueryInterface(IID_PPV_ARGS(&effect))))
                    {
                        return Make<MockD2DEffect>();
                    }
                    else if (SUCCEEDED(canvasImage->QueryInterface(IID_PPV_ARGS(&bitmap))))
                    {
                        return Make<MockD2DBitmap>();
                    }
                    else
                    {
                        Assert::Fail(); // notimpl
                        return nullptr;
                    }
                };
            
            m_canvasDevice->MockCreateBitmapBrush =
                [&](ID2D1Bitmap1* bitmap)
                {
                    m_bitmapBrush->MockGetBitmap = [&](ID2D1Bitmap** bitmap) 
                    { 
                        if (m_targetImage) m_targetImage.CopyTo(bitmap);
                        else *bitmap = nullptr;
                    };
                    m_bitmapBrush->MockSetBitmap = [&](ID2D1Bitmap* bitmap)
                    { 
                        m_targetImage = bitmap;
                    };

                    m_bitmapBrush->MockGetExtendModeX = [&]() { return D2D1_EXTEND_MODE_MIRROR; };
                    m_bitmapBrush->MockGetExtendModeY = [&]() { return D2D1_EXTEND_MODE_WRAP; };
                    m_bitmapBrush->MockGetInterpolationMode1 = [&]() { return D2D1_INTERPOLATION_MODE_ANISOTROPIC; };
                    m_bitmapBrush->MockGetOpacity = [&]() { return 0.1f; };
                    m_bitmapBrush->MockGetTransform = [&](D2D1_MATRIX_3X2_F* transform) { *transform = m_transform; };

                    m_bitmapBrush->MockSetExtendModeX = [&](D2D1_EXTEND_MODE extend) { Assert::AreEqual(D2D1_EXTEND_MODE_MIRROR, extend); };
                    m_bitmapBrush->MockSetExtendModeY = [&](D2D1_EXTEND_MODE extend) { Assert::AreEqual(D2D1_EXTEND_MODE_WRAP, extend); };
                    m_bitmapBrush->MockSetInterpolationMode1 = [&](D2D1_INTERPOLATION_MODE mode) { Assert::AreEqual(D2D1_INTERPOLATION_MODE_ANISOTROPIC, mode); };
                    m_bitmapBrush->MockSetOpacity = [&](float opacity) { Assert::AreEqual(0.1f, opacity); };
                    m_bitmapBrush->MockSetTransform = [&](const D2D1_MATRIX_3X2_F* transform) { m_transform = *transform; };

                    return m_bitmapBrush;
                };
            
            m_canvasDevice->MockCreateImageBrush =
                [&](ID2D1Image* image)
                {
                    m_imageBrush->MockGetImage = [&](ID2D1Image** image) 
                    {
                        if (m_targetImage) m_targetImage.CopyTo(image);
                        else *image = nullptr;
                    };
                    m_imageBrush->MockSetImage = [&](ID2D1Image* image)
                    {
                        m_targetImage = image;
                    };

                    m_imageBrush->MockGetExtendModeX = [&]() { return D2D1_EXTEND_MODE_MIRROR; };
                    m_imageBrush->MockGetExtendModeY = [&]() { return D2D1_EXTEND_MODE_WRAP; };
                    m_imageBrush->MockGetInterpolationMode = [&]() { return D2D1_INTERPOLATION_MODE_ANISOTROPIC; };
                    m_imageBrush->MockGetOpacity = [&]() { return 0.1f; };
                    m_imageBrush->MockGetTransform = [&](D2D1_MATRIX_3X2_F* transform) { *transform = m_transform; };
                    m_imageBrush->MockGetSourceRectangle = [&](D2D1_RECT_F* rect) { *rect = D2D1::RectF(0, 0, 10, 10); };

                    m_imageBrush->MockSetExtendModeX = [&](D2D1_EXTEND_MODE extend) { Assert::AreEqual(D2D1_EXTEND_MODE_MIRROR, extend); };
                    m_imageBrush->MockSetExtendModeY = [&](D2D1_EXTEND_MODE extend) { Assert::AreEqual(D2D1_EXTEND_MODE_WRAP, extend); };
                    m_imageBrush->MockSetInterpolationMode = [&](D2D1_INTERPOLATION_MODE mode) { Assert::AreEqual(D2D1_INTERPOLATION_MODE_ANISOTROPIC, mode); };
                    m_imageBrush->MockSetOpacity = [&](float opacity) { Assert::AreEqual(0.1f, opacity); };
                    m_imageBrush->MockSetTransform = [&](const D2D1_MATRIX_3X2_F* transform) { m_transform = *transform; };
                    m_imageBrush->MockSetSourceRectangle = [&](const D2D1_RECT_F* rect) { Assert::AreEqual(D2D1::RectF(0, 0, 10, 10), *rect); };

                    return m_imageBrush;
                };

            ComPtr<ICanvasImage> canvasBitmap;
            if (initializeWithBitmap) 
            {
                canvasBitmap = CreateStubCanvasBitmap();
            }

            m_canvasImageBrush = Make<CanvasImageBrush>(m_canvasDevice.Get(), canvasBitmap.Get(), adapter);
            ThrowIfFailed(m_canvasImageBrush.As(&m_canvasBrushInternal));
        }

    };

    void VerifyBackedByBitmapBrush(
        ComPtr<ICanvasBrushInternal> const& brushInternal,
        ComPtr<ID2D1Image>* outTarget = nullptr) // Optionally retrieve the target bitmap
    {
        auto d2dBrush = brushInternal->GetD2DBrush();
        ComPtr<ID2D1BitmapBrush1> bitmapBrush;
        ThrowIfFailed(d2dBrush.As(&bitmapBrush));
        if (outTarget)
        {
            ComPtr<ID2D1Bitmap> target;
            bitmapBrush->GetBitmap(&target);
            *outTarget = target;
        }
    }

    void VerifyBackedByImageBrush(
        ComPtr<ICanvasImageBrushInternal> const& brushInternal,
        ComPtr<ID2D1Image>* outTarget = nullptr) // Optionally retrieve the target image
    {
        auto d2dBrush = brushInternal->GetD2DBrushNoValidation();
        ComPtr<ID2D1ImageBrush> imageBrush;
        ThrowIfFailed(d2dBrush.As(&imageBrush));
        if (outTarget)
        {
            ComPtr<ID2D1Image> target;
            imageBrush->GetImage(&target);
            *outTarget = target;
        }
    }
    TEST_METHOD(CanvasImageBrush_Switching)
    {
        //
        // This test iterates once with an image brush initialized to a null
        // bitmap, and once initialized to an actual bitmap.
        //
        for (int i = 0; i < 2; ++i)
        {
            SwitchableTestBrushFixture f(i == 0);
            ComPtr<ID2D1Image> target0, target1, target2;

            ComPtr<ICanvasImageBrushInternal> brushInternal;
            ThrowIfFailed(f.m_canvasImageBrush.As(&brushInternal));

            // Initially assigned to bitmap brush.
            VerifyBackedByBitmapBrush(brushInternal, &target0);

            // Assigning a null source rect doesn't change this.
            f.m_canvasImageBrush->put_SourceRectangle(nullptr);
            VerifyBackedByBitmapBrush(brushInternal);

            // Assigning a non-null source rect switches it to image brush.
            ThrowIfFailed(f.m_canvasImageBrush->put_SourceRectangle(Make<StubRectReference>().Get()));
            VerifyBackedByImageBrush(brushInternal, &target1);

            // Assigning a different source rect doesn't change this.
            ThrowIfFailed(f.m_canvasImageBrush->put_SourceRectangle(Make<StubRectReference>().Get()));
            VerifyBackedByImageBrush(brushInternal);

            // Removing the source rect switches back to bitmap brush.
            f.m_canvasImageBrush->put_SourceRectangle(nullptr);
            VerifyBackedByBitmapBrush(brushInternal, &target2);

            // Verify the target stayed the same.
            Assert::AreEqual(target0.Get(), target1.Get());
            Assert::AreEqual(target1.Get(), target2.Get());
        }
    }
    
    TEST_METHOD(CanvasImageBrush_ImageBrushProperties)
    {
        SwitchableTestBrushFixture f;

        // Force the canvas brush into using an D2D image backing brush by setting
        // a non-NULL source rectangle.
        ThrowIfFailed(f.m_canvasImageBrush->put_SourceRectangle(Make<StubRectReference>().Get()));

        VerifyCommonBrushProperties(f.m_canvasImageBrush.Get(), f.m_imageBrush);

        bool setImageCalled = false;
        f.m_imageBrush->MockSetImage =
            [&](ID2D1Image* image)
            {
                Assert::IsFalse(setImageCalled);
                setImageCalled = true;
                Assert::IsNull(image);
            };
        // Bitmaps and images are not fully supported yet, so this
        // uses a NULL image.
        ThrowIfFailed(f.m_canvasImageBrush->put_Image(nullptr));
        Assert::IsTrue(setImageCalled);
        
        bool getInterpolationModeCalled = false;
        f.m_imageBrush->MockGetInterpolationMode =
            [&]()
            {
                Assert::IsFalse(getInterpolationModeCalled);
                getInterpolationModeCalled = true;
                return D2D1_INTERPOLATION_MODE_CUBIC;
            };
        CanvasImageInterpolation interpolation;
        ThrowIfFailed(f.m_canvasImageBrush->get_Interpolation(&interpolation));
        Assert::IsTrue(getInterpolationModeCalled);
        Assert::AreEqual(CanvasImageInterpolation::Cubic, interpolation);

        bool setInterpolationModeCalled = false;
        f.m_imageBrush->MockSetInterpolationMode =
            [&](D2D1_INTERPOLATION_MODE interpolationMode)
            {
                Assert::IsFalse(setInterpolationModeCalled);
                setInterpolationModeCalled = true;
                Assert::AreEqual(D2D1_INTERPOLATION_MODE_ANISOTROPIC, interpolationMode);
            };
        ThrowIfFailed(f.m_canvasImageBrush->put_Interpolation(CanvasImageInterpolation::Anisotropic));
        Assert::IsTrue(setInterpolationModeCalled);

        {
            bool getSourceRectangleCalled = false;
            f.m_imageBrush->MockGetSourceRectangle =
                [&](D2D1_RECT_F* sourceRectangle)
                {
                    Assert::IsFalse(getSourceRectangleCalled);
                    getSourceRectangleCalled = true;
                    *sourceRectangle = D2D1::RectF(1, 2, 9, 10);
                };
            ComPtr<IReference<Rect>> sourceRectangle;
            ThrowIfFailed(f.m_canvasImageBrush->get_SourceRectangle(&sourceRectangle));
            Assert::IsTrue(getSourceRectangleCalled);
            Rect rectValue;
            ThrowIfFailed(sourceRectangle->get_Value(&rectValue));
            Rect expected = { 1, 2, 8, 8 };
            Assert::AreEqual(expected, rectValue);
        }

    }

    TEST_METHOD(CanvasImageBrush_BackedByEffect_SourceRectangle)
    {
        // Create an image brush backed by an effect.
        SwitchableTestBrushFixture f;
        auto effect0 = Make<TestEffect>();
        ThrowIfFailed(f.m_canvasImageBrush->put_Image(effect0.Get()));

        // Ensure it's backed by an image brush.
        VerifyBackedByImageBrush(f.m_canvasBrushInternal);

        // Ensure it has a null source rect.
        ComPtr<IReference<Rect>> retrievedSourceRect;
        ThrowIfFailed(f.m_canvasImageBrush->get_SourceRectangle(&retrievedSourceRect));
        Assert::IsNull(retrievedSourceRect.Get());

        // Switch it to a different effect.
        auto effect1 = Make<TestEffect>();
        ThrowIfFailed(f.m_canvasImageBrush->put_Image(effect1.Get()));
        
        // Ensure the source rect is still null.
        ThrowIfFailed(f.m_canvasImageBrush->get_SourceRectangle(&retrievedSourceRect));
        Assert::IsNull(retrievedSourceRect.Get());

        // Set the source rect to something.
        ComPtr<StubRectReference> testSourceRect = Make<StubRectReference>();
        ThrowIfFailed(f.m_canvasImageBrush->put_SourceRectangle(testSourceRect.Get()));
        
        // Ensure source rect is the correct value.
        ThrowIfFailed(f.m_canvasImageBrush->get_SourceRectangle(&retrievedSourceRect));
        Assert::IsTrue(AreReferencedRectsEqual(testSourceRect.Get(), retrievedSourceRect.Get()));

        // Set the backing storage to a bitmap.
        auto bitmap = CreateStubCanvasBitmap();
        ThrowIfFailed(f.m_canvasImageBrush->put_Image(bitmap.Get()));

        // Source rect should still be set.
        ThrowIfFailed(f.m_canvasImageBrush->get_SourceRectangle(&retrievedSourceRect));
        Assert::IsTrue(AreReferencedRectsEqual(testSourceRect.Get(), retrievedSourceRect.Get()));

        // Should be backed by image brush still.
        VerifyBackedByImageBrush(f.m_canvasBrushInternal);

        // Set the backing storage to effect again.
        ThrowIfFailed(f.m_canvasImageBrush->put_Image(effect1.Get()));

        // Ensure source rect has the correct value.
        ThrowIfFailed(f.m_canvasImageBrush->get_SourceRectangle(&retrievedSourceRect));
        Assert::IsTrue(AreReferencedRectsEqual(testSourceRect.Get(), retrievedSourceRect.Get()));

        // Make a drawing session.
        auto manager = std::make_shared<CanvasDrawingSessionManager>();
        ComPtr<StubD2DDeviceContextWithGetFactory> d2dDeviceContext =
            Make<StubD2DDeviceContextWithGetFactory>();
        d2dDeviceContext->MockFillRectangle = [&](const D2D1_RECT_F* rect, ID2D1Brush* brush) {};

        ComPtr<CanvasDrawingSession> drawingSession = manager->Create(
            f.m_canvasDevice.Get(),
            d2dDeviceContext.Get(),
            std::make_shared<StubCanvasDrawingSessionAdapter>());

        ThrowIfFailed(drawingSession->FillRectangleAtCoordsWithBrush(0, 0, 0, 0, f.m_canvasImageBrush.Get())); // Should not throw

        // Set the source rect to null.
        ThrowIfFailed(f.m_canvasImageBrush->put_SourceRectangle(nullptr));

        // Try to draw the effect with the drawing session. Should throw because
        // a null source rect is set.
        HRESULT hrFillRect = drawingSession->FillRectangleAtCoordsWithBrush(0, 0, 0, 0, f.m_canvasImageBrush.Get());
        Assert::AreEqual(E_INVALIDARG, hrFillRect);

        // Set the source rect to a valid one, and issue the call again. Should succeed.
        ThrowIfFailed(f.m_canvasImageBrush->put_SourceRectangle(testSourceRect.Get()));
        ThrowIfFailed(drawingSession->FillRectangleAtCoordsWithBrush(0, 0, 0, 0, f.m_canvasImageBrush.Get()));
    }

};
