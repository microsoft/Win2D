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

#include "CanvasRenderTarget.h"
#include "TestBitmapResourceCreationAdapter.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace ABI::Windows::Foundation;

TEST_CLASS(CanvasRenderTargetTests)
{
    std::shared_ptr<CanvasRenderTargetDrawingSessionFactory> m_drawingSessionFactory;

public:

    TEST_METHOD_INITIALIZE(Reset)
    {
        m_drawingSessionFactory = std::make_shared<CanvasRenderTargetDrawingSessionFactory>();
    }

    TEST_METHOD(CanvasRenderTarget_Implements_Expected_Interfaces)
    {
        auto canvasDevice = Make<StubCanvasDevice>();
        canvasDevice->MockCreateBitmap =
            [&](Size size) -> ComPtr<ID2D1Bitmap1>
            {
                return nullptr;
            };

        Size size = { 1, 1 };
        auto renderTarget = Make<CanvasRenderTarget>(canvasDevice.Get(), size, m_drawingSessionFactory);

        ASSERT_IMPLEMENTS_INTERFACE(renderTarget, ICanvasRenderTarget);
        ASSERT_IMPLEMENTS_INTERFACE(renderTarget, ICanvasBitmap);
        ASSERT_IMPLEMENTS_INTERFACE(renderTarget, ICanvasImage);
        ASSERT_IMPLEMENTS_INTERFACE(renderTarget, ABI::Windows::Foundation::IClosable);
        ASSERT_IMPLEMENTS_INTERFACE(renderTarget, ICanvasImageInternal);
    }

    TEST_METHOD(CanvasRenderTarget_Closure)
    {
        auto canvasDevice = Make<StubCanvasDevice>();
        canvasDevice->MockCreateBitmap =
            [&](Size size) -> ComPtr<ID2D1Bitmap1>
            {
                return Make<StubD2DBitmap>();
            };

        Size size = { 1, 1 };
        auto renderTarget = Make<CanvasRenderTarget>(canvasDevice.Get(), size, m_drawingSessionFactory);

        ComPtr<IClosable> renderTargetClosable;
        ThrowIfFailed(renderTarget.As(&renderTargetClosable));

        Assert::AreEqual(S_OK, renderTargetClosable->Close());

        ComPtr<ICanvasDrawingSession> drawingSession;
        Assert::AreEqual(RO_E_CLOSED, renderTarget->CreateDrawingSession(&drawingSession));
    }


    TEST_METHOD(CanvasRenderTarget_DrawingSession)
    {
        auto canvasDevice = Make<StubCanvasDevice>();
        auto expectedD2DBitmap = Make<StubD2DBitmap>();
        canvasDevice->MockCreateBitmap =
            [&](Size size) -> ComPtr<ID2D1Bitmap1>
            {
                return expectedD2DBitmap;
            };

        auto drawingSessionFactory = std::make_shared<MockCanvasRenderTargetDrawingSessionFactory>();
        bool createCalled = false;
        drawingSessionFactory->MockCreate =
            [&](ICanvasDevice* device, ID2D1Bitmap1* target)
            {
                Assert::IsFalse(createCalled);
                createCalled = true;
                Assert::AreEqual(static_cast<ICanvasDevice*>(canvasDevice.Get()), device);
                Assert::AreEqual(static_cast<ID2D1Bitmap1*>(expectedD2DBitmap.Get()), target);
                return Make<MockCanvasDrawingSession>();
            };

        Size size = { 1, 1 };
        auto renderTarget = Make<CanvasRenderTarget>(canvasDevice.Get(), size, drawingSessionFactory);

        ComPtr<ICanvasDrawingSession> drawingSession;
        ThrowIfFailed(renderTarget->CreateDrawingSession(&drawingSession));

        Assert::IsTrue(createCalled);
    }

    TEST_METHOD(CanvasRenderTarget_InheritanceFromBitmap)
    {
        // This exercises some of the Bitmap methods on CanvasRenderTarget.

        Size expectedSize = { 33, 44 };

        auto canvasDevice = Make<StubCanvasDevice>();
        canvasDevice->MockCreateBitmap =
            [&](Size size) -> ComPtr<ID2D1Bitmap1>
            {
                auto bitmap = Make<StubD2DBitmap>();
                bitmap->MockGetSize =
                    [&](float* width, float* height)
                    {
                        *width = expectedSize.Width;
                        *height = expectedSize.Height;
                    };                    
                bitmap->MockGetPixelSize =
                    [&](unsigned int* width, unsigned int* height)
                    {
                        *width = static_cast<UINT>(expectedSize.Width);
                        *height = static_cast<UINT>(expectedSize.Height);
                    };

                return bitmap;
            };


        auto renderTarget = Make<CanvasRenderTarget>(canvasDevice.Get(), expectedSize, m_drawingSessionFactory);

        ComPtr<ICanvasBitmap> renderTargetAsBitmap;
        ThrowIfFailed(renderTarget.As(&renderTargetAsBitmap));

        Size retrievedSize;

        ThrowIfFailed(renderTargetAsBitmap->get_Size(&retrievedSize));
        Assert::AreEqual(expectedSize.Width, retrievedSize.Width);
        Assert::AreEqual(expectedSize.Height, retrievedSize.Height);

        // Bitmaps are constructed against default DPI, currently, so the pixel 
        // size and dips size shouldbe equal.
        ThrowIfFailed(renderTargetAsBitmap->get_SizeInPixels(&retrievedSize));
        Assert::AreEqual(expectedSize.Width, retrievedSize.Width);
        Assert::AreEqual(expectedSize.Height, retrievedSize.Height);
    }
};