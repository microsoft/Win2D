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

#include "MockWICFormatConverter.h"
#include "TestBitmapResourceCreationAdapter.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

TEST_CLASS(CanvasBitmapUnitTest)
{
public:

    ComPtr<MockWICFormatConverter> m_converter;
    std::shared_ptr<TestBitmapResourceCreationAdapter> m_adapter;
    std::shared_ptr<CanvasBitmapManager> m_bitmapManager;

    WinString m_testFileName;
    int m_testImageWidth;
    int m_testImageHeight;
    float m_testImageWidthDip;
    float m_testImageHeightDip;
    ComPtr<StubCanvasDevice> m_canvasDevice;
    ComPtr<StubD2DBitmap> m_bitmap;

    TEST_METHOD_INITIALIZE(Reset)
    {
        m_converter = Make<MockWICFormatConverter>();
        m_adapter = std::make_shared<TestBitmapResourceCreationAdapter>(m_converter);
        m_bitmapManager = std::make_shared<CanvasBitmapManager>(m_adapter);

        m_testImageWidth = 16;
        m_testImageHeight = 32;

        m_testImageWidthDip = 4.0f;
        m_testImageHeightDip = 8.0f;

        m_bitmap = Make<StubD2DBitmap>();
        m_bitmap->MockGetPixelSize =
            [&](unsigned int* width, unsigned int* height)
            {
                *width = m_testImageWidth;
                *height = m_testImageHeight;
            };

        m_bitmap->MockGetSize =
            [&](float* width, float* height)
            {
                *width = m_testImageWidthDip;
                *height = m_testImageHeightDip;
            };

        m_canvasDevice = Make<StubCanvasDevice>();
        m_canvasDevice->MockCreateBitmapFromWicResource =
            [&]() -> ComPtr<ID2D1Bitmap1>
            {
                return m_bitmap;
            };

        m_testFileName = WinString(L"fakeImage.jpg");

    }

    TEST_METHOD(CanvasBitmap_Implements_Expected_Interfaces)
	{
        auto canvasBitmap = m_bitmapManager->Create(m_canvasDevice.Get(), m_testFileName, CanvasAlphaBehavior::Premultiplied);
        
        ASSERT_IMPLEMENTS_INTERFACE(canvasBitmap, ICanvasBitmap);
        ASSERT_IMPLEMENTS_INTERFACE(canvasBitmap, ICanvasImage);
        ASSERT_IMPLEMENTS_INTERFACE(canvasBitmap, IEffectInput);
        ASSERT_IMPLEMENTS_INTERFACE(canvasBitmap, IDirect3DSurface);
        ASSERT_IMPLEMENTS_INTERFACE(canvasBitmap, ABI::Windows::Foundation::IClosable);
        ASSERT_IMPLEMENTS_INTERFACE(canvasBitmap, IDXGIInterfaceAccess);
        ASSERT_IMPLEMENTS_INTERFACE(canvasBitmap, ICanvasImageInternal);
        ASSERT_IMPLEMENTS_INTERFACE(canvasBitmap, ICanvasBitmapInternal);
    }

    TEST_METHOD(CanvasBitmap_Closed)
    {
        ABI::Windows::Foundation::Size size;
        ABI::Windows::Foundation::Rect bounds;
        Numerics::Matrix3x2 matrix = {0};
        Direct3DSurfaceDescription surfaceDescription;
        ComPtr<IDXGISurface> dxgiSurface;

        auto canvasBitmap = m_bitmapManager->Create(m_canvasDevice.Get(), m_testFileName, CanvasAlphaBehavior::Premultiplied);

        Assert::IsNotNull(canvasBitmap.Get());

        Assert::AreEqual(S_OK, canvasBitmap->Close());

        Assert::AreEqual(RO_E_CLOSED, canvasBitmap->get_SizeInPixels(&size));
        Assert::AreEqual(RO_E_CLOSED, canvasBitmap->get_Size(&size));
        Assert::AreEqual(RO_E_CLOSED, canvasBitmap->get_Bounds(&bounds));
        Assert::AreEqual(RO_E_CLOSED, canvasBitmap->get_Description(&surfaceDescription));
        Assert::AreEqual(RO_E_CLOSED, canvasBitmap->GetDXGIInterface(IID_PPV_ARGS(&dxgiSurface)));

        auto drawingSession = CreateStubDrawingSession();
        Assert::AreEqual(RO_E_CLOSED, canvasBitmap->GetBounds(drawingSession.Get(), &bounds));
        Assert::AreEqual(RO_E_CLOSED, canvasBitmap->GetBoundsWithTransform(drawingSession.Get(), matrix, &bounds));

        auto otherBitmap = m_bitmapManager->Create(m_canvasDevice.Get(), m_testFileName, CanvasAlphaBehavior::Premultiplied);
        Assert::AreEqual(RO_E_CLOSED, canvasBitmap->CopyFromBitmap(otherBitmap.Get()));
        Assert::AreEqual(RO_E_CLOSED, canvasBitmap->CopyFromBitmapWithDestPoint(otherBitmap.Get(), 0, 0));
        Assert::AreEqual(RO_E_CLOSED, canvasBitmap->CopyFromBitmapWithDestPointAndSourceRect(otherBitmap.Get(), 0, 0, 0, 0, 0, 0));
    }

    TEST_METHOD(CanvasBitmap_Get_Size)
    {
        ABI::Windows::Foundation::Size size;

        bool isConverterCreated = false;
        m_adapter->MockCreateWICFormatConverter =
            [&]
            {
                Assert::AreEqual(isConverterCreated, false);
                isConverterCreated = true;
            };

        auto canvasBitmap = m_bitmapManager->Create(m_canvasDevice.Get(), m_testFileName, CanvasAlphaBehavior::Premultiplied);

        Assert::AreEqual(true, isConverterCreated);

        HRESULT result = canvasBitmap->get_SizeInPixels(&size);
        Assert::AreEqual(S_OK, result);
        Assert::AreEqual(m_testImageWidth, (int)size.Width);
        Assert::AreEqual(m_testImageHeight, (int)size.Height);

        result = canvasBitmap->get_Size(&size);
        Assert::AreEqual(S_OK, result);
        Assert::AreEqual(m_testImageWidthDip, size.Width);
        Assert::AreEqual(m_testImageHeightDip, size.Height);
    }

    TEST_METHOD(CanvasBitmap_Get_Bounds)
    {
        ABI::Windows::Foundation::Rect bounds;

        bool isConverterCreated = false;
        m_adapter->MockCreateWICFormatConverter =
            [&]
            {
                Assert::AreEqual(isConverterCreated, false);
                isConverterCreated = true;
            };

        auto canvasBitmap = m_bitmapManager->Create(m_canvasDevice.Get(), m_testFileName, CanvasAlphaBehavior::Premultiplied);

        Assert::AreEqual(true, isConverterCreated);

        HRESULT result = canvasBitmap->get_Bounds(&bounds);
        Assert::AreEqual(S_OK, result);
        Assert::AreEqual(0.0f, bounds.X);
        Assert::AreEqual(0.0f, bounds.Y);
        Assert::AreEqual(m_testImageWidthDip, bounds.Width);
        Assert::AreEqual(m_testImageHeightDip, bounds.Height);
    }

    TEST_METHOD(CanvasBitmap_GetBounds_NullArg)
    {
        ABI::Windows::Foundation::Rect bounds;
        Numerics::Matrix3x2 matrix = { 0 };

        auto canvasBitmap = m_bitmapManager->Create(m_canvasDevice.Get(), m_testFileName, CanvasAlphaBehavior::Premultiplied);

        auto drawingSession = CreateStubDrawingSession();
        Assert::AreEqual(E_INVALIDARG, canvasBitmap->GetBounds(nullptr, &bounds));
        Assert::AreEqual(E_INVALIDARG, canvasBitmap->GetBounds(drawingSession.Get(), nullptr));
        Assert::AreEqual(E_INVALIDARG, canvasBitmap->GetBoundsWithTransform(nullptr, matrix, &bounds));
        Assert::AreEqual(E_INVALIDARG, canvasBitmap->GetBoundsWithTransform(drawingSession.Get(), matrix, nullptr));
    }

    TEST_METHOD(CanvasBitmap_CopyFromBitmap_NullArg)
    {
        auto canvasBitmap = m_bitmapManager->Create(m_canvasDevice.Get(), m_testFileName, CanvasAlphaBehavior::Premultiplied);

        Assert::AreEqual(E_INVALIDARG, canvasBitmap->CopyFromBitmap(nullptr));
        Assert::AreEqual(E_INVALIDARG, canvasBitmap->CopyFromBitmapWithDestPoint(nullptr, 0, 0));
        Assert::AreEqual(E_INVALIDARG, canvasBitmap->CopyFromBitmapWithDestPointAndSourceRect(nullptr, 0, 0, 0, 0, 0, 0));
    }

    class CopyFromBitmapFixture
    {
        CALL_COUNTER(CopyFromBitmapMethod);

    public:
        ComPtr<CanvasBitmap> DestBitmap;
        ComPtr<CanvasBitmap> SourceBitmap;

        CopyFromBitmapFixture(
            std::shared_ptr<CanvasBitmapManager> const& bitmapManager,
            WinString const& testFileName,
            int32_t* destX = nullptr,
            int32_t* destY = nullptr,
            int32_t* sourceRectLeft = nullptr,
            int32_t* sourceRectTop = nullptr,
            int32_t* sourceRectWidth = nullptr,
            int32_t* sourceRectHeight = nullptr)
        {
            auto canvasDevice = Make<StubCanvasDevice>();

            CopyFromBitmapMethod.SetExpectedCalls(1);

            auto sourceD2DBitmap = Make<StubD2DBitmap>();
            canvasDevice->MockCreateBitmapFromWicResource =
                [&]() -> ComPtr<ID2D1Bitmap1>
                {
                    return sourceD2DBitmap;
                };
            SourceBitmap = bitmapManager->Create(canvasDevice.Get(), testFileName, CanvasAlphaBehavior::Premultiplied);

            bool expectDestPoint = false;
            D2D1_POINT_2U expectedDestPoint;
            if (destX)
            {
                assert(destY);
                expectDestPoint = true;
                expectedDestPoint = D2D1::Point2U(static_cast<UINT32>(*destX), static_cast<UINT32>(*destY));
            }

            bool expectSourceRect = false;
            D2D1_RECT_U expectedSourceRect;
            if (sourceRectLeft)
            {
                assert(sourceRectTop && sourceRectWidth && sourceRectHeight);
                expectSourceRect = true;
                expectedSourceRect = D2D1::RectU(
                    static_cast<UINT32>(*sourceRectLeft),
                    static_cast<UINT32>(*sourceRectTop),
                    static_cast<UINT32>(*sourceRectLeft + *sourceRectWidth),
                    static_cast<UINT32>(*sourceRectTop + *sourceRectHeight));
            }

            auto destD2DBitmap = Make<StubD2DBitmap>();
            destD2DBitmap->MockCopyFromBitmap =
                [this, expectDestPoint, expectedDestPoint, sourceD2DBitmap, expectSourceRect, expectedSourceRect](CONST D2D1_POINT_2U* destinationPoint, ID2D1Bitmap *bitmap, CONST D2D1_RECT_U* sourceRectangle)
                {
                    CopyFromBitmapMethod.WasCalled();

                    if (expectDestPoint)
                    {
                        Assert::IsNotNull(destinationPoint);
                        Assert::AreEqual(expectedDestPoint, *destinationPoint);

                    }
                    else
                    {
                        Assert::IsNull(destinationPoint);
                    }

                    Assert::IsNotNull(bitmap);
                    Assert::AreEqual(static_cast<ID2D1Bitmap*>(sourceD2DBitmap.Get()), bitmap);

                    if (expectSourceRect)
                    {
                        Assert::IsNotNull(sourceRectangle);
                        Assert::AreEqual(expectedSourceRect, *sourceRectangle);

                    }
                    else
                    {
                        Assert::IsNull(sourceRectangle);
                    }

                    return S_OK;
                };

            canvasDevice->MockCreateBitmapFromWicResource =
                [&]() -> ComPtr<ID2D1Bitmap1>
                {
                    return destD2DBitmap;
                };
            DestBitmap = bitmapManager->Create(canvasDevice.Get(), testFileName, CanvasAlphaBehavior::Premultiplied);

        }

    };


    TEST_METHOD(CanvasBitmap_CopyFromBitmap)
    {
        int32_t destPoint[] = { 234, 2 };
        int32_t sourceRect[] = { 444, 555, 666, 772 };
        {
            CopyFromBitmapFixture fixture(m_bitmapManager, m_testFileName);
            ThrowIfFailed(fixture.DestBitmap->CopyFromBitmap(fixture.SourceBitmap.Get()));
            Expectations::Instance()->Validate();
        }
        {
            CopyFromBitmapFixture fixture(m_bitmapManager, m_testFileName, &destPoint[0], &destPoint[1]);
            ThrowIfFailed(fixture.DestBitmap->CopyFromBitmapWithDestPoint(fixture.SourceBitmap.Get(), destPoint[0], destPoint[1]));
            Expectations::Instance()->Validate();
        }
        {
            CopyFromBitmapFixture fixture(
                m_bitmapManager, 
                m_testFileName, 
                &destPoint[0], 
                &destPoint[1],
                &sourceRect[0],
                &sourceRect[1],
                &sourceRect[2],
                &sourceRect[3]);

            ThrowIfFailed(fixture.DestBitmap->CopyFromBitmapWithDestPointAndSourceRect(
                fixture.SourceBitmap.Get(), 
                destPoint[0], 
                destPoint[1],
                sourceRect[0],
                sourceRect[1],
                sourceRect[2],
                sourceRect[3]));
            Expectations::Instance()->Validate();
        }
    }

    TEST_METHOD(CanvasBitmap_CopyFromBitmap_InvalidCoordinates)
    {
        auto canvasDevice = Make<StubCanvasDevice>();

        auto sourceD2DBitmap = Make<StubD2DBitmap>();
        canvasDevice->MockCreateBitmapFromWicResource = [&]() -> ComPtr<ID2D1Bitmap1> { return sourceD2DBitmap; };
        auto sourceBitmap = m_bitmapManager->Create(canvasDevice.Get(), m_testFileName, CanvasAlphaBehavior::Premultiplied);

        auto destD2DBitmap = Make<StubD2DBitmap>();
        canvasDevice->MockCreateBitmapFromWicResource = [&]() -> ComPtr<ID2D1Bitmap1> { return destD2DBitmap; };
        auto destBitmap = m_bitmapManager->Create(canvasDevice.Get(), m_testFileName, CanvasAlphaBehavior::Premultiplied);

        // Negative coordinates
        Assert::AreEqual(E_INVALIDARG, destBitmap->CopyFromBitmapWithDestPoint(sourceBitmap.Get(), -1, 100));
        Assert::AreEqual(E_INVALIDARG, destBitmap->CopyFromBitmapWithDestPoint(sourceBitmap.Get(), 99, -2));

        // Negative source rect extents
        Assert::AreEqual(E_INVALIDARG, destBitmap->CopyFromBitmapWithDestPointAndSourceRect(sourceBitmap.Get(), 0, 0, -1, 1, 5, 5));
        Assert::AreEqual(E_INVALIDARG, destBitmap->CopyFromBitmapWithDestPointAndSourceRect(sourceBitmap.Get(), 0, 0, 1, -1, 5, 5));
        Assert::AreEqual(E_INVALIDARG, destBitmap->CopyFromBitmapWithDestPointAndSourceRect(sourceBitmap.Get(), 0, 0, 1, 1, -5, 5));
        Assert::AreEqual(E_INVALIDARG, destBitmap->CopyFromBitmapWithDestPointAndSourceRect(sourceBitmap.Get(), 0, 0, 1, 1, 5, -5));
    }
};
