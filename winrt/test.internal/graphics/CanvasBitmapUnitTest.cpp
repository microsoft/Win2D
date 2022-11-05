// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

TEST_CLASS(CanvasBitmapUnitTest)
{
    struct Fixture
    {
        ComPtr<MockWICFormatConverter> m_converter;
        std::shared_ptr<TestBitmapAdapter> m_adapter;
        
        WinString m_testFileName;
        uint32_t m_testImageWidth;
        uint32_t m_testImageHeight;
        float m_testImageWidthDip;
        float m_testImageHeightDip;
        ComPtr<StubCanvasDevice> m_canvasDevice;

        Fixture()
        {
            m_converter = Make<MockWICFormatConverter>();

            m_adapter = std::make_shared<TestBitmapAdapter>(m_converter);
            CanvasBitmapAdapter::SetInstance(m_adapter);

            m_testImageWidth = 16;
            m_testImageHeight = 32;

            m_testImageWidthDip = 4.0f;
            m_testImageHeightDip = 8.0f;

            m_canvasDevice = Make<StubCanvasDevice>();
            m_canvasDevice->MockCreateBitmapFromWicResource =
                [&](IWICBitmapSource* converter, CanvasAlphaMode alpha, float dpi) -> ComPtr<ID2D1Bitmap1>
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

        auto canvasBitmap = CanvasBitmap::CreateNew(f.m_canvasDevice.Get(), f.m_testFileName, DEFAULT_DPI, CanvasAlphaMode::Premultiplied);
        
        ASSERT_IMPLEMENTS_INTERFACE(canvasBitmap, ICanvasBitmap);
        ASSERT_IMPLEMENTS_INTERFACE(canvasBitmap, ICanvasImage);
        ASSERT_IMPLEMENTS_INTERFACE(canvasBitmap, IGraphicsEffectSource);
        ASSERT_IMPLEMENTS_INTERFACE(canvasBitmap, IDirect3DSurface);
        ASSERT_IMPLEMENTS_INTERFACE(canvasBitmap, ABI::Windows::Foundation::IClosable);
        ASSERT_IMPLEMENTS_INTERFACE(canvasBitmap, IDirect3DDxgiInterfaceAccess);
        ASSERT_IMPLEMENTS_INTERFACE(canvasBitmap, ICanvasImageInterop);
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

        auto canvasBitmap = CanvasBitmap::CreateNew(f.m_canvasDevice.Get(), f.m_testFileName, DEFAULT_DPI, CanvasAlphaMode::Premultiplied);

        Assert::IsNotNull(canvasBitmap.Get());

        Assert::AreEqual(S_OK, canvasBitmap->Close());

        Assert::AreEqual(RO_E_CLOSED, canvasBitmap->get_SizeInPixels(&bitmapSize));
        Assert::AreEqual(RO_E_CLOSED, canvasBitmap->get_Size(&size));
        Assert::AreEqual(RO_E_CLOSED, canvasBitmap->get_Bounds(&bounds));
        Assert::AreEqual(RO_E_CLOSED, canvasBitmap->get_Description(&surfaceDescription));
        Assert::AreEqual(RO_E_CLOSED, canvasBitmap->GetInterface(IID_PPV_ARGS(&dxgiSurface)));

        Assert::AreEqual(RO_E_CLOSED, canvasBitmap->GetBounds(f.m_canvasDevice.Get(), &bounds));
        Assert::AreEqual(RO_E_CLOSED, canvasBitmap->GetBoundsWithTransform(f.m_canvasDevice.Get(), matrix, &bounds));

        auto otherBitmap = CanvasBitmap::CreateNew(f.m_canvasDevice.Get(), f.m_testFileName, DEFAULT_DPI, CanvasAlphaMode::Premultiplied);
        Assert::AreEqual(RO_E_CLOSED, canvasBitmap->CopyPixelsFromBitmap(otherBitmap.Get()));
        Assert::AreEqual(RO_E_CLOSED, canvasBitmap->CopyPixelsFromBitmapWithDestPoint(otherBitmap.Get(), 0, 0));
        Assert::AreEqual(RO_E_CLOSED, canvasBitmap->CopyPixelsFromBitmapWithDestPointAndSourceRect(otherBitmap.Get(), 0, 0, 0, 0, 0, 0));
    }

    TEST_METHOD_EX(CanvasBitmap_GetDevice)
    {
        Fixture f;
        auto bitmap = CanvasBitmap::CreateNew(f.m_canvasDevice.Get(), f.m_testFileName, DEFAULT_DPI, CanvasAlphaMode::Premultiplied);

        ComPtr<ICanvasDevice> actualDevice;
        bitmap->get_Device(&actualDevice);
        Assert::AreEqual<ICanvasDevice*>(f.m_canvasDevice.Get(), actualDevice.Get());
    }

    TEST_METHOD_EX(CanvasBitmap_GetAlphaMode)
    {
        Fixture f;

        auto canvasDevice = Make<StubCanvasDevice>();
        auto d2dBitmap = Make<StubD2DBitmap>();
        
        canvasDevice->MockCreateBitmapFromWicResource = [&](IWICBitmapSource*, CanvasAlphaMode, float) -> ComPtr<ID2D1Bitmap1>
        {
            return d2dBitmap;
        };

        auto bitmap = CanvasBitmap::CreateNew(canvasDevice.Get(), f.m_testFileName, DEFAULT_DPI, CanvasAlphaMode::Premultiplied);

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
        
        canvasDevice->MockCreateBitmapFromWicResource = [&](IWICBitmapSource*, CanvasAlphaMode, float) -> ComPtr<ID2D1Bitmap1>
        {
            return d2dBitmap;
        };

        auto bitmap = CanvasBitmap::CreateNew(canvasDevice.Get(), f.m_testFileName, DEFAULT_DPI, CanvasAlphaMode::Premultiplied);

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
        f.m_adapter->MockCreateWicBitmapSource =
            [&]
            {
                Assert::AreEqual(isConverterCreated, false);
                isConverterCreated = true;
            };

        auto canvasBitmap = CanvasBitmap::CreateNew(f.m_canvasDevice.Get(), f.m_testFileName, DEFAULT_DPI, CanvasAlphaMode::Premultiplied);

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
        f.m_adapter->MockCreateWicBitmapSource =
            [&]
            {
                Assert::AreEqual(isConverterCreated, false);
                isConverterCreated = true;
            };

        auto canvasBitmap = CanvasBitmap::CreateNew(f.m_canvasDevice.Get(), f.m_testFileName, DEFAULT_DPI, CanvasAlphaMode::Premultiplied);

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

        auto canvasBitmap = CanvasBitmap::CreateNew(f.m_canvasDevice.Get(), f.m_testFileName, DEFAULT_DPI, CanvasAlphaMode::Premultiplied);

        Assert::AreEqual(E_INVALIDARG, canvasBitmap->GetBounds(nullptr, &bounds));
        Assert::AreEqual(E_INVALIDARG, canvasBitmap->GetBounds(f.m_canvasDevice.Get(), nullptr));
        Assert::AreEqual(E_INVALIDARG, canvasBitmap->GetBoundsWithTransform(nullptr, matrix, &bounds));
        Assert::AreEqual(E_INVALIDARG, canvasBitmap->GetBoundsWithTransform(f.m_canvasDevice.Get(), matrix, nullptr));
    }

    TEST_METHOD_EX(CanvasBitmap_DpiProperties)
    {
        const float dpi = 144;

        Fixture f;
        auto canvasBitmap = CanvasBitmap::CreateNew(f.m_canvasDevice.Get(), f.m_testFileName, dpi, CanvasAlphaMode::Premultiplied);

        float actualDpi = 0;
        ThrowIfFailed(canvasBitmap->get_Dpi(&actualDpi));
        Assert::AreEqual(dpi, actualDpi);

        VerifyConvertDipsToPixels(dpi, canvasBitmap);

        const float testValue = 100;
        float dips = 0;
        ThrowIfFailed(canvasBitmap->ConvertPixelsToDips((int)testValue, &dips));
        Assert::AreEqual(testValue * DEFAULT_DPI / dpi, dips);
    }

    TEST_METHOD_EX(CanvasBitmap_CopyPixelsFromBitmap_NullArg)
    {
        Fixture f;

        auto canvasBitmap = CanvasBitmap::CreateNew(f.m_canvasDevice.Get(), f.m_testFileName, DEFAULT_DPI, CanvasAlphaMode::Premultiplied);

        Assert::AreEqual(E_INVALIDARG, canvasBitmap->CopyPixelsFromBitmap(nullptr));
        Assert::AreEqual(E_INVALIDARG, canvasBitmap->CopyPixelsFromBitmapWithDestPoint(nullptr, 0, 0));
        Assert::AreEqual(E_INVALIDARG, canvasBitmap->CopyPixelsFromBitmapWithDestPointAndSourceRect(nullptr, 0, 0, 0, 0, 0, 0));
    }

    struct CopyFromBitmapFixture : public Fixture
    {
        ComPtr<CanvasBitmap> DestBitmap;
        ComPtr<CanvasBitmap> SourceBitmap;

        CopyFromBitmapFixture(D2D1_POINT_2U expectedDestPoint, D2D1_RECT_U expectedSourceRect)
        {
            auto canvasDevice = Make<StubCanvasDevice>();

            auto sourceD2DBitmap = Make<StubD2DBitmap>();
            sourceD2DBitmap->GetPixelFormatMethod.AllowAnyCall([] { return D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_TYPELESS); });
            sourceD2DBitmap->GetPixelSizeMethod.AllowAnyCall([] { return D2D1_SIZE_U{ 1024, 1024 }; });

            canvasDevice->MockCreateBitmapFromWicResource =
                [&](IWICBitmapSource*, CanvasAlphaMode, float)
                {
                    return sourceD2DBitmap;
                };
            SourceBitmap = CanvasBitmap::CreateNew(canvasDevice.Get(), m_testFileName, DEFAULT_DPI, CanvasAlphaMode::Premultiplied);

            auto destD2DBitmap = Make<StubD2DBitmap>();
            destD2DBitmap->GetPixelFormatMethod.AllowAnyCall([] { return D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_TYPELESS); });
            destD2DBitmap->GetPixelSizeMethod.AllowAnyCall([] { return D2D1_SIZE_U{ 2028, 2028 }; });

            destD2DBitmap->CopyFromBitmapMethod.SetExpectedCalls(1,
                [=](D2D1_POINT_2U const* destinationPoint, ID2D1Bitmap *bitmap, D2D1_RECT_U const* sourceRectangle)
                {
                    Assert::IsNotNull(destinationPoint);
                    Assert::AreEqual(expectedDestPoint, *destinationPoint);

                    Assert::IsNotNull(bitmap);
                    Assert::AreEqual(static_cast<ID2D1Bitmap*>(sourceD2DBitmap.Get()), bitmap);

                    Assert::IsNotNull(sourceRectangle);
                    Assert::AreEqual(expectedSourceRect, *sourceRectangle);

                    return S_OK;
                });

            canvasDevice->MockCreateBitmapFromWicResource =
                [&](IWICBitmapSource*, CanvasAlphaMode, float)
                {
                    return destD2DBitmap;
                };
            DestBitmap = CanvasBitmap::CreateNew(canvasDevice.Get(), m_testFileName, DEFAULT_DPI, CanvasAlphaMode::Premultiplied);
        }
    };

    TEST_METHOD_EX(CanvasBitmap_CopyPixelsFromBitmap)
    {
        D2D1_POINT_2U expectedDestPoint{ 0, 0 };
        D2D1_RECT_U expectedSourceRect{ 0, 0, 1024, 1024 };
        
        CopyFromBitmapFixture f(expectedDestPoint, expectedSourceRect);
        ThrowIfFailed(f.DestBitmap->CopyPixelsFromBitmap(f.SourceBitmap.Get()));
    }

    TEST_METHOD_EX(CanvasBitmap_CopyPixelsFromBitmapWithDestPoint)
    {
        int32_t destPoint[] = { 234, 2 };

        D2D1_POINT_2U expectedDestPoint{ 234, 2 };
        D2D1_RECT_U expectedSourceRect{ 0, 0, 1024, 1024 };

        CopyFromBitmapFixture f(expectedDestPoint, expectedSourceRect);
        ThrowIfFailed(f.DestBitmap->CopyPixelsFromBitmapWithDestPoint(f.SourceBitmap.Get(), destPoint[0], destPoint[1]));
    }

    TEST_METHOD_EX(CanvasBitmap_CopyPixelsFromBitmapWithDestPointAndSourceRect)
    {
        int32_t destPoint[] = { 234, 2 };
        int32_t sourceRect[] = { 111, 222, 333, 445 };

        D2D1_POINT_2U expectedDestPoint{ 234, 2 };
        D2D1_RECT_U expectedSourceRect{ 111, 222, 111 + 333, 222 + 445 };

        CopyFromBitmapFixture f(expectedDestPoint, expectedSourceRect);

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
        sourceD2DBitmap->GetPixelFormatMethod.AllowAnyCall([] { return D2D1::PixelFormat(); });
        canvasDevice->MockCreateBitmapFromWicResource = [&](IWICBitmapSource*, CanvasAlphaMode, float) -> ComPtr<ID2D1Bitmap1> { return sourceD2DBitmap; };
        auto sourceBitmap = CanvasBitmap::CreateNew(canvasDevice.Get(), f.m_testFileName, DEFAULT_DPI, CanvasAlphaMode::Premultiplied);

        auto destD2DBitmap = Make<StubD2DBitmap>();
        destD2DBitmap->GetPixelFormatMethod.AllowAnyCall([] { return D2D1::PixelFormat(); });
        canvasDevice->MockCreateBitmapFromWicResource = [&](IWICBitmapSource*, CanvasAlphaMode, float) -> ComPtr<ID2D1Bitmap1> { return destD2DBitmap; };
        auto destBitmap = CanvasBitmap::CreateNew(canvasDevice.Get(), f.m_testFileName, DEFAULT_DPI, CanvasAlphaMode::Premultiplied);

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
