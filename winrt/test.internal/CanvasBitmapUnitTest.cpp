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
    struct Fixture
    {
        ComPtr<MockWICFormatConverter> m_converter;
        std::shared_ptr<TestBitmapResourceCreationAdapter> m_adapter;
        std::shared_ptr<CanvasBitmapManager> m_bitmapManager;
        
        WinString m_testFileName;
        uint32_t m_testImageWidth;
        uint32_t m_testImageHeight;
        float m_testImageWidthDip;
        float m_testImageHeightDip;
        ComPtr<StubCanvasDevice> m_canvasDevice;

        Fixture()
        {
            m_converter = Make<MockWICFormatConverter>();
            m_adapter = std::make_shared<TestBitmapResourceCreationAdapter>(m_converter);
            m_bitmapManager = std::make_shared<CanvasBitmapManager>(m_adapter);

            m_testImageWidth = 16;
            m_testImageHeight = 32;

            m_testImageWidthDip = 4.0f;
            m_testImageHeightDip = 8.0f;

            m_canvasDevice = Make<StubCanvasDevice>();
            m_canvasDevice->MockCreateBitmapFromWicResource =
                [&](IWICFormatConverter* converter, CanvasAlphaMode alpha, float dpi) -> ComPtr<ID2D1Bitmap1>
                {
                    auto bitmap = Make<StubD2DBitmap>(D2D1_BITMAP_OPTIONS_NONE, dpi);

                    bitmap->GetPixelSizeMethod.AllowAnyCall(
                        [&] { return D2D1_SIZE_U{m_testImageWidth, m_testImageHeight}; });

                    bitmap->GetSizeMethod.AllowAnyCall(
                        [&] { return D2D1_SIZE_F{m_testImageWidthDip, m_testImageHeightDip}; });

                    return bitmap;
                };

            m_testFileName = WinString(L"fakeImage.jpg");
        }
    };

    TEST_METHOD_EX(CanvasBitmap_Implements_Expected_Interfaces)
	{
        Fixture f;

        auto canvasBitmap = f.m_bitmapManager->Create(f.m_canvasDevice.Get(), f.m_testFileName, CanvasAlphaMode::Premultiplied, DEFAULT_DPI);
        
        ASSERT_IMPLEMENTS_INTERFACE(canvasBitmap, ICanvasBitmap);
        ASSERT_IMPLEMENTS_INTERFACE(canvasBitmap, ICanvasImage);
        ASSERT_IMPLEMENTS_INTERFACE(canvasBitmap, IEffectInput);
        ASSERT_IMPLEMENTS_INTERFACE(canvasBitmap, IDirect3DSurface);
        ASSERT_IMPLEMENTS_INTERFACE(canvasBitmap, ABI::Windows::Foundation::IClosable);
        ASSERT_IMPLEMENTS_INTERFACE(canvasBitmap, IDirect3DDxgiInterfaceAccess);
        ASSERT_IMPLEMENTS_INTERFACE(canvasBitmap, ICanvasImageInternal);
        ASSERT_IMPLEMENTS_INTERFACE(canvasBitmap, ICanvasBitmapInternal);
    }

    TEST_METHOD_EX(CanvasBitmap_Closed)
    {
        Fixture f;

        ABI::Windows::Foundation::Size size;
        BitmapSize bitmapSize;
        ABI::Windows::Foundation::Rect bounds;
        Numerics::Matrix3x2 matrix = {0};
        Direct3DSurfaceDescription surfaceDescription;
        ComPtr<IDXGISurface> dxgiSurface;

        auto canvasBitmap = f.m_bitmapManager->Create(f.m_canvasDevice.Get(), f.m_testFileName, CanvasAlphaMode::Premultiplied, DEFAULT_DPI);

        Assert::IsNotNull(canvasBitmap.Get());

        Assert::AreEqual(S_OK, canvasBitmap->Close());

        Assert::AreEqual(RO_E_CLOSED, canvasBitmap->get_SizeInPixels(&bitmapSize));
        Assert::AreEqual(RO_E_CLOSED, canvasBitmap->get_Size(&size));
        Assert::AreEqual(RO_E_CLOSED, canvasBitmap->get_Bounds(&bounds));
        Assert::AreEqual(RO_E_CLOSED, canvasBitmap->get_Description(&surfaceDescription));
        Assert::AreEqual(RO_E_CLOSED, canvasBitmap->GetInterface(IID_PPV_ARGS(&dxgiSurface)));

        auto drawingSession = CreateStubDrawingSession();
        Assert::AreEqual(RO_E_CLOSED, canvasBitmap->GetBounds(drawingSession.Get(), &bounds));
        Assert::AreEqual(RO_E_CLOSED, canvasBitmap->GetBoundsWithTransform(drawingSession.Get(), matrix, &bounds));

        auto otherBitmap = f.m_bitmapManager->Create(f.m_canvasDevice.Get(), f.m_testFileName, CanvasAlphaMode::Premultiplied, DEFAULT_DPI);
        Assert::AreEqual(RO_E_CLOSED, canvasBitmap->CopyPixelsFromBitmap(otherBitmap.Get()));
        Assert::AreEqual(RO_E_CLOSED, canvasBitmap->CopyPixelsFromBitmapWithDestPoint(otherBitmap.Get(), 0, 0));
        Assert::AreEqual(RO_E_CLOSED, canvasBitmap->CopyPixelsFromBitmapWithDestPointAndSourceRect(otherBitmap.Get(), 0, 0, 0, 0, 0, 0));
    }

    TEST_METHOD_EX(CanvasBitmap_GetDevice)
    {
        Fixture f;
        auto bitmap = f.m_bitmapManager->Create(f.m_canvasDevice.Get(), f.m_testFileName, CanvasAlphaMode::Premultiplied, DEFAULT_DPI);

        ComPtr<ICanvasDevice> actualDevice;
        bitmap->get_Device(&actualDevice);
        Assert::AreEqual<ICanvasDevice*>(f.m_canvasDevice.Get(), actualDevice.Get());
    }

    TEST_METHOD_EX(CanvasBitmap_GetAlphaMode)
    {
        Fixture f;

        auto canvasDevice = Make<StubCanvasDevice>();
        auto d2dBitmap = Make<StubD2DBitmap>();
        
        canvasDevice->MockCreateBitmapFromWicResource = [&](IWICFormatConverter*, CanvasAlphaMode, float) -> ComPtr<ID2D1Bitmap1>
        {
            return d2dBitmap;
        };

        auto bitmap = f.m_bitmapManager->Create(canvasDevice.Get(), f.m_testFileName, CanvasAlphaMode::Premultiplied, DEFAULT_DPI);

        d2dBitmap->GetPixelFormatMethod.SetExpectedCalls(1, []
        {
            return D2D1_PIXEL_FORMAT{ DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_STRAIGHT };
        });

        CanvasAlphaMode alphaMode = CanvasAlphaMode::Ignore;
        bitmap->get_AlphaMode(&alphaMode);
        Assert::AreEqual(CanvasAlphaMode::Straight, alphaMode);
    }

    TEST_METHOD_EX(CanvasBitmap_GetFormat)
    {
        Fixture f;

        auto canvasDevice = Make<StubCanvasDevice>();
        auto d2dBitmap = Make<StubD2DBitmap>();
        
        canvasDevice->MockCreateBitmapFromWicResource = [&](IWICFormatConverter*, CanvasAlphaMode, float) -> ComPtr<ID2D1Bitmap1>
        {
            return d2dBitmap;
        };

        auto bitmap = f.m_bitmapManager->Create(canvasDevice.Get(), f.m_testFileName, CanvasAlphaMode::Premultiplied, DEFAULT_DPI);

        d2dBitmap->GetPixelFormatMethod.SetExpectedCalls(1, []
        {
            return D2D1_PIXEL_FORMAT{ DXGI_FORMAT_BC3_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED };
        });

        DirectXPixelFormat format = PIXEL_FORMAT(Unknown);
        bitmap->get_Format(&format);
        Assert::AreEqual(PIXEL_FORMAT(BC3UIntNormalized), format);
    }

    TEST_METHOD_EX(CanvasBitmap_Get_Size)
    {
        Fixture f;

        bool isConverterCreated = false;
        f.m_adapter->MockCreateWICFormatConverter =
            [&]
            {
                Assert::AreEqual(isConverterCreated, false);
                isConverterCreated = true;
            };

        auto canvasBitmap = f.m_bitmapManager->Create(f.m_canvasDevice.Get(), f.m_testFileName, CanvasAlphaMode::Premultiplied, DEFAULT_DPI);

        Assert::AreEqual(true, isConverterCreated);

        BitmapSize bitmapSize;
        HRESULT result = canvasBitmap->get_SizeInPixels(&bitmapSize);
        Assert::AreEqual(S_OK, result);
        Assert::AreEqual(f.m_testImageWidth, static_cast<uint32_t>(bitmapSize.Width));
        Assert::AreEqual(f.m_testImageHeight, static_cast<uint32_t>(bitmapSize.Height));

        ABI::Windows::Foundation::Size size;
        result = canvasBitmap->get_Size(&size);
        Assert::AreEqual(S_OK, result);
        Assert::AreEqual(f.m_testImageWidthDip, size.Width);
        Assert::AreEqual(f.m_testImageHeightDip, size.Height);
    }

    TEST_METHOD_EX(CanvasBitmap_Get_Bounds)
    {
        Fixture f;

        ABI::Windows::Foundation::Rect bounds;

        bool isConverterCreated = false;
        f.m_adapter->MockCreateWICFormatConverter =
            [&]
            {
                Assert::AreEqual(isConverterCreated, false);
                isConverterCreated = true;
            };

        auto canvasBitmap = f.m_bitmapManager->Create(f.m_canvasDevice.Get(), f.m_testFileName, CanvasAlphaMode::Premultiplied, DEFAULT_DPI);

        Assert::AreEqual(true, isConverterCreated);

        HRESULT result = canvasBitmap->get_Bounds(&bounds);
        Assert::AreEqual(S_OK, result);
        Assert::AreEqual(0.0f, bounds.X);
        Assert::AreEqual(0.0f, bounds.Y);
        Assert::AreEqual(f.m_testImageWidthDip, bounds.Width);
        Assert::AreEqual(f.m_testImageHeightDip, bounds.Height);
    }

    TEST_METHOD_EX(CanvasBitmap_GetBounds_NullArg)
    {
        Fixture f;

        ABI::Windows::Foundation::Rect bounds;
        Numerics::Matrix3x2 matrix = { 0 };

        auto canvasBitmap = f.m_bitmapManager->Create(f.m_canvasDevice.Get(), f.m_testFileName, CanvasAlphaMode::Premultiplied, DEFAULT_DPI);

        auto drawingSession = CreateStubDrawingSession();
        Assert::AreEqual(E_INVALIDARG, canvasBitmap->GetBounds(nullptr, &bounds));
        Assert::AreEqual(E_INVALIDARG, canvasBitmap->GetBounds(drawingSession.Get(), nullptr));
        Assert::AreEqual(E_INVALIDARG, canvasBitmap->GetBoundsWithTransform(nullptr, matrix, &bounds));
        Assert::AreEqual(E_INVALIDARG, canvasBitmap->GetBoundsWithTransform(drawingSession.Get(), matrix, nullptr));
    }

    TEST_METHOD_EX(CanvasBitmap_DpiProperties)
    {
        const float dpi = 144;

        Fixture f;
        auto canvasBitmap = f.m_bitmapManager->Create(f.m_canvasDevice.Get(), f.m_testFileName, CanvasAlphaMode::Premultiplied, dpi);

        float actualDpi = 0;
        ThrowIfFailed(canvasBitmap->get_Dpi(&actualDpi));
        Assert::AreEqual(dpi, actualDpi);

        const float testValue = 100;

        int pixels = 0;
        ThrowIfFailed(canvasBitmap->ConvertDipsToPixels(testValue, &pixels));
        Assert::AreEqual((int)(testValue * dpi / DEFAULT_DPI), pixels);

        float dips = 0;
        ThrowIfFailed(canvasBitmap->ConvertPixelsToDips((int)testValue, &dips));
        Assert::AreEqual(testValue * DEFAULT_DPI / dpi, dips);
    }

    TEST_METHOD_EX(CanvasBitmap_CopyPixelsFromBitmap_NullArg)
    {
        Fixture f;

        auto canvasBitmap = f.m_bitmapManager->Create(f.m_canvasDevice.Get(), f.m_testFileName, CanvasAlphaMode::Premultiplied, DEFAULT_DPI);

        Assert::AreEqual(E_INVALIDARG, canvasBitmap->CopyPixelsFromBitmap(nullptr));
        Assert::AreEqual(E_INVALIDARG, canvasBitmap->CopyPixelsFromBitmapWithDestPoint(nullptr, 0, 0));
        Assert::AreEqual(E_INVALIDARG, canvasBitmap->CopyPixelsFromBitmapWithDestPointAndSourceRect(nullptr, 0, 0, 0, 0, 0, 0));
    }

    struct CopyFromBitmapFixture : public Fixture
    {
        ComPtr<CanvasBitmap> DestBitmap;
        ComPtr<CanvasBitmap> SourceBitmap;

        CopyFromBitmapFixture(
            int32_t* destX = nullptr,
            int32_t* destY = nullptr,
            int32_t* sourceRectLeft = nullptr,
            int32_t* sourceRectTop = nullptr,
            int32_t* sourceRectWidth = nullptr,
            int32_t* sourceRectHeight = nullptr)
        {
            auto canvasDevice = Make<StubCanvasDevice>();

            auto sourceD2DBitmap = Make<StubD2DBitmap>();
            canvasDevice->MockCreateBitmapFromWicResource =
                [&](IWICFormatConverter*, CanvasAlphaMode, float) -> ComPtr<ID2D1Bitmap1>
                {
                    return sourceD2DBitmap;
                };
            SourceBitmap = m_bitmapManager->Create(canvasDevice.Get(), m_testFileName, CanvasAlphaMode::Premultiplied, DEFAULT_DPI);

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
            destD2DBitmap->CopyFromBitmapMethod.SetExpectedCalls(1,
                [=](D2D1_POINT_2U const* destinationPoint, ID2D1Bitmap *bitmap, D2D1_RECT_U const* sourceRectangle)
                {
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
                });

            canvasDevice->MockCreateBitmapFromWicResource =
                [&](IWICFormatConverter*, CanvasAlphaMode, float) -> ComPtr<ID2D1Bitmap1>
                {
                    return destD2DBitmap;
                };
            DestBitmap = m_bitmapManager->Create(canvasDevice.Get(), m_testFileName, CanvasAlphaMode::Premultiplied, DEFAULT_DPI);
        }
    };

    TEST_METHOD_EX(CanvasBitmap_CopyPixelsFromBitmap)
    {
        CopyFromBitmapFixture f;
        ThrowIfFailed(f.DestBitmap->CopyPixelsFromBitmap(f.SourceBitmap.Get()));
    }

    TEST_METHOD_EX(CanvasBitmap_CopyPixelsFromBitmapWithDestPoint)
    {
        int32_t destPoint[] = { 234, 2 };

        CopyFromBitmapFixture f(&destPoint[0], &destPoint[1]);
        ThrowIfFailed(f.DestBitmap->CopyPixelsFromBitmapWithDestPoint(f.SourceBitmap.Get(), destPoint[0], destPoint[1]));
    }

    TEST_METHOD_EX(CanvasBitmap_CopyPixelsFromBitmapWithDestPointAndSourceRect)
    {
        int32_t destPoint[] = { 234, 2 };
        int32_t sourceRect[] = { 444, 555, 666, 772 };

        CopyFromBitmapFixture f(
            &destPoint[0], 
            &destPoint[1],
            &sourceRect[0],
            &sourceRect[1],
            &sourceRect[2],
            &sourceRect[3]);

        ThrowIfFailed(f.DestBitmap->CopyPixelsFromBitmapWithDestPointAndSourceRect(
            f.SourceBitmap.Get(), 
            destPoint[0], 
            destPoint[1],
            sourceRect[0],
            sourceRect[1],
            sourceRect[2],
            sourceRect[3]));
    }

    TEST_METHOD_EX(CanvasBitmap_CopyPixelsFromBitmap_InvalidCoordinates)
    {
        Fixture f;

        auto canvasDevice = Make<StubCanvasDevice>();

        auto sourceD2DBitmap = Make<StubD2DBitmap>();
        canvasDevice->MockCreateBitmapFromWicResource = [&](IWICFormatConverter*, CanvasAlphaMode, float) -> ComPtr<ID2D1Bitmap1> { return sourceD2DBitmap; };
        auto sourceBitmap = f.m_bitmapManager->Create(canvasDevice.Get(), f.m_testFileName, CanvasAlphaMode::Premultiplied, DEFAULT_DPI);

        auto destD2DBitmap = Make<StubD2DBitmap>();
        canvasDevice->MockCreateBitmapFromWicResource = [&](IWICFormatConverter*, CanvasAlphaMode, float) -> ComPtr<ID2D1Bitmap1> { return destD2DBitmap; };
        auto destBitmap = f.m_bitmapManager->Create(canvasDevice.Get(), f.m_testFileName, CanvasAlphaMode::Premultiplied, DEFAULT_DPI);

        // Negative coordinates
        Assert::AreEqual(E_INVALIDARG, destBitmap->CopyPixelsFromBitmapWithDestPoint(sourceBitmap.Get(), -1, 100));
        Assert::AreEqual(E_INVALIDARG, destBitmap->CopyPixelsFromBitmapWithDestPoint(sourceBitmap.Get(), 99, -2));

        // Negative source rect extents
        Assert::AreEqual(E_INVALIDARG, destBitmap->CopyPixelsFromBitmapWithDestPointAndSourceRect(sourceBitmap.Get(), 0, 0, -1, 1, 5, 5));
        Assert::AreEqual(E_INVALIDARG, destBitmap->CopyPixelsFromBitmapWithDestPointAndSourceRect(sourceBitmap.Get(), 0, 0, 1, -1, 5, 5));
        Assert::AreEqual(E_INVALIDARG, destBitmap->CopyPixelsFromBitmapWithDestPointAndSourceRect(sourceBitmap.Get(), 0, 0, 1, 1, -5, 5));
        Assert::AreEqual(E_INVALIDARG, destBitmap->CopyPixelsFromBitmapWithDestPointAndSourceRect(sourceBitmap.Get(), 0, 0, 1, 1, 5, -5));
    }
};
