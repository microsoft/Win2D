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

TEST_CLASS(CanvasImageUnitTests)
{
    struct Fixture
    {
        std::shared_ptr<CanvasRenderTargetManager> m_manager;
        ComPtr<StubCanvasDevice> m_canvasDevice;

        Fixture()
        {
            auto converter = Make<MockWICFormatConverter>();
            auto adapter = std::make_shared<TestBitmapResourceCreationAdapter>(converter);
            m_manager = std::make_shared<CanvasRenderTargetManager>(adapter);
            m_canvasDevice = Make<StubCanvasDevice>();
            
            m_canvasDevice->MockCreateRenderTargetBitmap =
                [&](float, float, DirectXPixelFormat, CanvasAlphaBehavior, float)
                {
                    return Make<StubD2DBitmap>(D2D1_BITMAP_OPTIONS_TARGET);
                };
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

        auto manager = std::make_shared<CanvasDrawingSessionManager>();

        auto drawingSession = manager->GetOrCreate(d2dDeviceContext.Get());

        auto canvasBitmap = f.m_manager->Create(
            f.m_canvasDevice.Get(),
            1.0f,
            1.0f,
            DirectXPixelFormat::B8G8R8A8UIntNormalized,
            CanvasAlphaBehavior::Premultiplied,
            DEFAULT_DPI);
        canvasBitmap->GetBounds(drawingSession.Get(), &bounds);

        Assert::AreEqual(someTransform, currentTransform); 
    }
};
