// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

TEST_CLASS(CanvasImageUnitTests)
{
    struct Fixture
    {
        ComPtr<StubCanvasDevice> m_canvasDevice;

        Fixture()
        {
            m_canvasDevice = Make<StubCanvasDevice>();
            
            m_canvasDevice->CreateRenderTargetBitmapMethod.AllowAnyCall(
                [&](float, float, float, DirectXPixelFormat, CanvasAlphaMode)
                {
                    return Make<StubD2DBitmap>(D2D1_BITMAP_OPTIONS_TARGET);
                });
        }
    };

    TEST_METHOD_EX(CanvasImage_GetBounds_CorrectContext)
    {
        Fixture f;

        ABI::Windows::Foundation::Rect bounds;

        D2D1_MATRIX_3X2_F someTransform = D2D1::Matrix3x2F(1, 2, 3, 4, 5, 6);
        D2D1_MATRIX_3X2_F currentTransform = someTransform;

        auto d2dDeviceContext = Make<MockD2DDeviceContext>();

        d2dDeviceContext->GetDeviceMethod.AllowAnyCallAlwaysCopyValueToParam(Make<StubD2DDevice>());

        d2dDeviceContext->GetTransformMethod.AllowAnyCall(
            [&](D2D1_MATRIX_3X2_F* matrix)
            {
                *matrix = currentTransform;
            });        

        d2dDeviceContext->SetTransformMethod.AllowAnyCall(
            [&](const D2D1_MATRIX_3X2_F* matrix)
            {
                currentTransform = *matrix;
            });

        d2dDeviceContext->GetImageWorldBoundsMethod.SetExpectedCalls(1,
            [&](ID2D1Image* image, D2D1_RECT_F* bounds)
            {
                return S_OK;
            });

        auto drawingSession = Make<CanvasDrawingSession>(d2dDeviceContext.Get());

        auto canvasBitmap = CanvasRenderTarget::CreateNew(
            f.m_canvasDevice.Get(),
            1.0f,
            1.0f,
            DEFAULT_DPI,
            PIXEL_FORMAT(B8G8R8A8UIntNormalized),
            CanvasAlphaMode::Premultiplied);
        canvasBitmap->GetBounds(drawingSession.Get(), &bounds);

        Assert::AreEqual(someTransform, currentTransform); 
    }
};
