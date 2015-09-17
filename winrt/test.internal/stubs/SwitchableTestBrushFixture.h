// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#include "stubs/StubD2DEffect.h"

namespace canvas
{
    class SwitchableTestBrushFixture
    {
    public:
        ComPtr<MockD2DBitmapBrush> m_bitmapBrush;
        ComPtr<MockD2DImageBrush> m_imageBrush;
        ComPtr<ID2D1Image> m_targetImage;
        ComPtr<CanvasImageBrush> m_canvasImageBrush;
        ComPtr<ICanvasBrushInternal> m_canvasBrushInternal;
        ComPtr<MockCanvasDevice> m_canvasDevice;
        ComPtr<StubD2DDevice> m_d2dDevice;
        ComPtr<MockD2DDeviceContext> m_d2dDeviceContext;

        D2D1_MATRIX_3X2_F m_transform;

        SwitchableTestBrushFixture(bool initializeWithBitmap = false)
            : m_transform(D2D1::Matrix3x2F(1, 2, 3, 4, 5, 6))
        {
            m_canvasDevice = Make<MockCanvasDevice>();
            m_bitmapBrush = Make<MockD2DBitmapBrush>();
            m_imageBrush = Make<MockD2DImageBrush>();

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
                [&](ID2D1Image* initialImage)
                {
                    m_targetImage = initialImage;

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

                    m_imageBrush->MockSetSourceRectangle = [&](const D2D1_RECT_F* rect) { 
                        // this was brittle; SourceRectangle has good coverage
                        // in CanvasImageBrushTests.cpp
                    };

                    return m_imageBrush;
                };

            ComPtr<ICanvasImage> canvasBitmap;
            if (initializeWithBitmap) 
            {
                canvasBitmap = CreateStubCanvasBitmap();
            }

            m_canvasImageBrush = Make<CanvasImageBrush>(m_canvasDevice.Get(), canvasBitmap.Get());
            ThrowIfFailed(m_canvasImageBrush.As(&m_canvasBrushInternal));

            m_d2dDevice = Make<StubD2DDevice>();
            m_canvasDevice->MockGetD2DDevice = [&] { return m_d2dDevice; };

            m_d2dDeviceContext = Make<StubD2DDeviceContextWithGetFactory>();

            m_canvasDevice->GetResourceCreationDeviceContextMethod.AllowAnyCall(
                [&]
                {
                    return DeviceContextLease(As<ID2D1DeviceContext1>(m_d2dDeviceContext));
                });

            m_d2dDeviceContext->CreateEffectMethod.AllowAnyCall(
                [](IID const& iid, ID2D1Effect** effect)
                {
                    return Make<StubD2DEffect>(iid).CopyTo(effect);
                });
        }
    };
}
