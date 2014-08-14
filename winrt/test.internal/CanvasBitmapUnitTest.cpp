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
#include "CanvasBitmap.h"
#include "TestBitmapResourceCreationAdapter.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

TEST_CLASS(CanvasBitmapUnitTest)
{
public:

    ComPtr<MockWICFormatConverter> m_converter;
    std::shared_ptr<TestBitmapResourceCreationAdapter> m_adapter;

    WinString m_testFileName;
    int m_testImageWidth;
    int m_testImageHeight;
    float m_testImageWidthDip;
    float m_testImageHeightDip;
    ComPtr<StubCanvasDevice> m_canvasDevice;
    ComPtr<MockD2DBitmap> m_bitmap;

    TEST_METHOD_INITIALIZE(Reset)
    {
        m_testImageWidth = 16;
        m_testImageHeight = 32;

        m_testImageWidthDip = 4.0f;
        m_testImageHeightDip = 8.0f;

        m_bitmap = Make<MockD2DBitmap>();
        m_canvasDevice = Make<StubCanvasDevice>();
        m_canvasDevice->MockCreateBitmap =
            [&]() -> ComPtr<ID2D1Bitmap1>
            {
                return m_bitmap;
            };

        m_testFileName = WinString(L"fakeImage.jpg");

        m_converter = Make<MockWICFormatConverter>();
        m_adapter = std::make_shared<TestBitmapResourceCreationAdapter>(m_converter);
    }

    TEST_METHOD(CanvasBitmap_Implements_Expected_Interfaces)
	{
        auto canvasBitmap = Make<canvas::CanvasBitmap>(m_canvasDevice.Get(), m_testFileName, m_adapter);
        
        ASSERT_IMPLEMENTS_INTERFACE(canvasBitmap, ICanvasBitmap);
        ASSERT_IMPLEMENTS_INTERFACE(canvasBitmap, ICanvasImage);
        ASSERT_IMPLEMENTS_INTERFACE(canvasBitmap, ABI::Windows::Foundation::IClosable);
        ASSERT_IMPLEMENTS_INTERFACE(canvasBitmap, ICanvasImageInternal);
	}

    TEST_METHOD(CanvasBitmap_Closed)
    {
        ABI::Windows::Foundation::Size size;
        auto canvasBitmap = Make<canvas::CanvasBitmap>(m_canvasDevice.Get(), m_testFileName, m_adapter);

        Assert::IsNotNull(canvasBitmap.Get());

        Assert::AreEqual(S_OK, canvasBitmap->Close());

        Assert::AreEqual(RO_E_CLOSED, canvasBitmap->get_SizeInPixels(&size));
        Assert::AreEqual(RO_E_CLOSED, canvasBitmap->get_SizeInDips(&size));
    }

    TEST_METHOD(CanvasBitmap_Get_Size)
    {
        ABI::Windows::Foundation::Size size;

        bool isConverterCreated = false;
        m_adapter->MockCreateWICFormatConverter =
            [&]()
            {
                Assert::AreEqual(isConverterCreated, false);
                isConverterCreated = true;
            };

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

        auto canvasBitmap = Make<canvas::CanvasBitmap>(m_canvasDevice.Get(), m_testFileName, m_adapter);
        Assert::AreEqual(true, isConverterCreated);

        HRESULT result = canvasBitmap->get_SizeInPixels(&size);
        Assert::AreEqual(S_OK, result);
        Assert::AreEqual(m_testImageWidth, (int)size.Width);
        Assert::AreEqual(m_testImageHeight, (int)size.Height);

        result = canvasBitmap->get_SizeInDips(&size);
        Assert::AreEqual(S_OK, result);
        Assert::AreEqual(m_testImageWidthDip, size.Width);
        Assert::AreEqual(m_testImageHeightDip, size.Height);
    }
};
