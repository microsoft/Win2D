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

using namespace Microsoft::Graphics::Canvas;
using namespace Microsoft::WRL::Wrappers;
using namespace Windows::Foundation;
using namespace Windows::Devices::Enumeration;
using namespace Microsoft::Graphics::Canvas::DirectX;
using namespace Windows::UI;


TEST_CLASS(CanvasBitmapTests)
{
    struct TestImage
    {
        Platform::String^ fileName;
        int widthInPixels;
        int heightInPixels;

        float widthInDip;
        float heightInDip;
    };

    TestImage m_testImage;
    float m_pngDpiTolerance;

public:
    CanvasBitmapTests()
    {
        m_testImage.fileName = L"Assets/imageTiger.jpg";
        m_testImage.widthInPixels = 196;
        m_testImage.heightInPixels = 147;

        const float imageDpi = 72.0f;
        const float dpiScaling = DEFAULT_DPI / imageDpi;
        m_testImage.widthInDip = 196.0f * dpiScaling;
        m_testImage.heightInDip = 147.0f * dpiScaling;

        m_pngDpiTolerance = 1e-2f;
    }

    TEST_METHOD(CanvasBitmap_PropertiesAndClose)
    {

        WIN32_FILE_ATTRIBUTE_DATA fileInfo;
        Assert::AreNotEqual(GetFileAttributesEx(m_testImage.fileName->Data(), GetFileExInfoStandard, &fileInfo), 0);
        
        CanvasDevice^ canvasDevice = ref new CanvasDevice();

        auto bitmapJpeg = WaitExecution(CanvasBitmap::LoadAsync(canvasDevice, m_testImage.fileName));
        Assert::AreEqual(m_testImage.widthInPixels, (int)bitmapJpeg->SizeInPixels.Width);
        Assert::AreEqual(m_testImage.heightInPixels, (int)bitmapJpeg->SizeInPixels.Height);
        Assert::AreEqual(m_testImage.widthInDip, bitmapJpeg->Size.Width);
        Assert::AreEqual(m_testImage.heightInDip, bitmapJpeg->Size.Height);

        // Test invalid bitmap parameter.
        Assert::ExpectException<Platform::InvalidArgumentException^>(
            [&]
            {
                auto bitmap = WaitExecution(CanvasBitmap::LoadAsync(canvasDevice, static_cast<Platform::String^>(nullptr)));
            });

        Assert::ExpectException<Platform::InvalidArgumentException^>(
            [&]
            {
                auto bitmap = WaitExecution(CanvasBitmap::LoadAsync(canvasDevice, static_cast<Uri^>(nullptr)));
            });

        // Test invalid device parameter.
        Assert::ExpectException<Platform::InvalidArgumentException^>(
            [&]
            {
                auto bitmap = WaitExecution(CanvasBitmap::LoadAsync(nullptr, m_testImage.fileName));
            });
    }


    TEST_METHOD(CanvasBitmap_LoadFromInvalidParameters)
    {
        CanvasDevice^ canvasDevice = ref new CanvasDevice();

        try
        {
            auto async = CanvasBitmap::LoadAsync(canvasDevice, "ThisImageFileDoesNotExist.jpg");

            WaitExecution(async);

            Assert::Fail(L"Trying to load a missing file should throw!");
        }
        catch (Platform::COMException^ e)
        {
            Assert::AreEqual<unsigned>(HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND), e->HResult);
        }

        try
        {
            Uri^ uri = ref new Uri("ms-appx:///nope");

            auto async = CanvasBitmap::LoadAsync(canvasDevice, uri);

            WaitExecution(async);

            Assert::Fail(L"Trying to load an invalid URI should throw!");
        }
        catch (Platform::COMException^ e)
        {
            Assert::AreEqual<unsigned>(HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND), e->HResult);
        }        
    }

    TEST_METHOD(CanvasBitmap_LoadUri)
    {
        CanvasDevice^ canvasDevice = ref new CanvasDevice();

        Uri^ uri = ref new Uri("ms-appx:///Assets/HighDpiGrid.png");

        auto async = CanvasBitmap::LoadAsync(canvasDevice, uri, CanvasAlphaBehavior::Ignore);

        auto bitmap = WaitExecution(async);

        Assert::AreEqual(2.00001454f, bitmap->Size.Width);
        Assert::AreEqual(2.00001454f, bitmap->Size.Height);
        Assert::AreEqual(4.0f, bitmap->SizeInPixels.Width);
        Assert::AreEqual(4.0f, bitmap->SizeInPixels.Height);

        auto d2dBitmap = GetWrappedResource<ID2D1Bitmap1>(bitmap);

        VerifyDpiAndAlpha(d2dBitmap, 192, D2D1_ALPHA_MODE_IGNORE, m_pngDpiTolerance);
    }


    TEST_METHOD(CanvasBitmap_LoadMany)
    {
        const int bitmapCount = 8;

        CanvasDevice^ canvasDevice = ref new CanvasDevice();

        CanvasBitmap^ bitmaps[bitmapCount] = {};

        // Events to notify us when each texture has finished loading.
        Event loadedEvents[bitmapCount];

        for (int i = 0; i < bitmapCount; i++)
        {
            loadedEvents[i] = Event(CreateEventEx(NULL, NULL, CREATE_EVENT_MANUAL_RESET, EVENT_ALL_ACCESS));
        }

        // Kick off several simultaneous asynchronous loads.
        for (int i = 0; i < bitmapCount; i++)
        {
            IAsyncOperation<CanvasBitmap^>^ asyncOperation = CanvasBitmap::LoadAsync(canvasDevice, L"Assets/imageTiger.jpg");

            // Use a completion event handler to store the loaded bitmap data, 
            // but do not otherwise hold onto any references to the active async operation.
            asyncOperation->Completed = ref new AsyncOperationCompletedHandler<CanvasBitmap^>(
                [&bitmaps, &loadedEvents, i](IAsyncOperation<CanvasBitmap^>^ asyncInfo, Windows::Foundation::AsyncStatus asyncStatus)
            {
                Assert::AreEqual((int)Windows::Foundation::AsyncStatus::Completed, (int)asyncStatus);

                bitmaps[i] = asyncInfo->GetResults();

                SetEvent(loadedEvents[i].Get());
            });
        }

        // Wait for loading to complete.
        const int timeout = 5000;

        HANDLE eventHandles[bitmapCount];
        std::transform(std::begin(loadedEvents), std::end(loadedEvents), eventHandles, [](Event const& e) { return e.Get(); });

        auto waitResult = WaitForMultipleObjectsEx(bitmapCount, eventHandles, true, timeout, false);

        Assert::IsTrue(waitResult >= WAIT_OBJECT_0 && 
                       waitResult < WAIT_OBJECT_0 + bitmapCount);

        // Make sure we got sensible results back.
        for (int i = 0; i < bitmapCount; i++)
        {
            Assert::IsNotNull(bitmaps[i]);

            Assert::AreEqual(196.0f, bitmaps[i]->SizeInPixels.Width);
            Assert::AreEqual(147.0f, bitmaps[i]->SizeInPixels.Height);
        }
    }


    TEST_METHOD(CanvasBitmap_NativeInterop)
    {
        auto canvasDevice = ref new CanvasDevice();

        auto originalBitmap = WaitExecution(CanvasBitmap::LoadAsync(canvasDevice, m_testImage.fileName));
        auto originalD2DBitmap = GetWrappedResource<ID2D1Bitmap1>(originalBitmap);
        auto newBitmap = GetOrCreate<CanvasBitmap>(canvasDevice, originalD2DBitmap.Get());
        auto newD2DBitmap = GetWrappedResource<ID2D1Bitmap1>(newBitmap);

        Assert::AreEqual(originalBitmap, newBitmap);
        Assert::AreEqual(originalD2DBitmap.Get(), newD2DBitmap.Get());
    }

    TEST_METHOD(CanvasBitmap_CreateBitmap_Sizes)
    {
        auto canvasDevice = ref new CanvasDevice();

        struct SizeTestCase
        {
            int32_t width;
            int32_t height;
        } legalSizeTestCases[] =
        {
            { 0, 0 },
            { 1, 0 },
            { 0, 1 },
            { 1, 1 },
        };

        Platform::Array<BYTE>^ byteArray1x1 = ref new Platform::Array<BYTE>(1);
        Platform::Array<Color>^ colorArray1x1 = ref new Platform::Array<Color>(1);

        for (auto& testCase : legalSizeTestCases)
        {
            auto bitmap = CanvasBitmap::CreateFromBytes(
                canvasDevice,
                byteArray1x1,
                testCase.width,
                testCase.height,
                DirectXPixelFormat::A8UIntNormalized, // One of the few formats that is actually supported with straight alpha
                CanvasAlphaBehavior::Straight,
                99.0f);
            auto d2dBitmap = GetWrappedResource<ID2D1Bitmap1>(bitmap);
            auto size = bitmap->SizeInPixels;
            Assert::AreEqual(static_cast<float>(testCase.width), size.Width);
            Assert::AreEqual(static_cast<float>(testCase.height), size.Height);
            VerifyDpiAndAlpha(d2dBitmap, 99.0f, D2D1_ALPHA_MODE_STRAIGHT);

            bitmap = CanvasBitmap::CreateFromColors(
                canvasDevice,
                colorArray1x1,
                testCase.width,
                testCase.height,
                CanvasAlphaBehavior::Ignore,
                25.0f);
            d2dBitmap = GetWrappedResource<ID2D1Bitmap1>(bitmap);
            size = bitmap->SizeInPixels;
            Assert::AreEqual(static_cast<float>(testCase.width), size.Width);
            Assert::AreEqual(static_cast<float>(testCase.height), size.Height);
            VerifyDpiAndAlpha(d2dBitmap, 25.0f, D2D1_ALPHA_MODE_IGNORE);
        }

        SizeTestCase illegalSizeTestCases[] =
        {
            { -1, 1 },
            { 1, -1 },
            { -234, -50 },
        };

        for (auto& testCase : illegalSizeTestCases)
        {                
            Assert::ExpectException<Platform::InvalidArgumentException^>(
                [&]
                {
                    auto bitmap = CanvasBitmap::CreateFromBytes(
                        canvasDevice,
                        byteArray1x1,
                        testCase.width,
                        testCase.height,
                        DirectXPixelFormat::B8G8R8A8UIntNormalized,
                        CanvasAlphaBehavior::Premultiplied,
                        DEFAULT_DPI);

                    bitmap = CanvasBitmap::CreateFromColors(
                        canvasDevice,
                        colorArray1x1,
                        testCase.width,
                        testCase.height,
                        CanvasAlphaBehavior::Premultiplied,
                        DEFAULT_DPI);
                });
        }
    }

    TEST_METHOD(CanvasBitmap_CreateFromBytes_AcceptsZeroSizedArray)
    {
        auto device = ref new CanvasDevice();
        auto anyFormat = DirectXPixelFormat::B8G8R8A8UIntNormalized;
        auto anyAlphaBehavior = CanvasAlphaBehavior::Premultiplied;

        auto zeroSizedArray = ref new Platform::Array<BYTE>(0);
        auto bitmap = CanvasBitmap::CreateFromBytes(device, zeroSizedArray, 0, 0, anyFormat, anyAlphaBehavior);
        
        auto size = bitmap->SizeInPixels;
        Assert::AreEqual(0.0f, size.Width);
        Assert::AreEqual(0.0f, size.Height);
    }

    TEST_METHOD(CanvasBitmap_CreateFromColors_AcceptsZeroSizedArray)
    {
        auto device = ref new CanvasDevice();
        auto anyAlphaBehavior = CanvasAlphaBehavior::Premultiplied;

        auto zeroSizedArray = ref new Platform::Array<Color>(0);
        auto bitmap = CanvasBitmap::CreateFromColors(device, zeroSizedArray, 0, 0, anyAlphaBehavior);
        
        auto size = bitmap->SizeInPixels;
        Assert::AreEqual(0.0f, size.Width);
        Assert::AreEqual(0.0f, size.Height);
    }

    TEST_METHOD(CanvasBitmap_CreateFromBytes_FailsIfArrayTooSmall)
    {
        auto device = ref new CanvasDevice();
        auto anyFormat = DirectXPixelFormat::B8G8R8A8UIntNormalized;
        auto anyAlphaBehavior = CanvasAlphaBehavior::Premultiplied;

        auto oneElementArray = ref new Platform::Array<BYTE>(1);

        Assert::ExpectException<Platform::InvalidArgumentException^>(
            [&]
            {
                CanvasBitmap::CreateFromBytes(device, oneElementArray, 256, 256, anyFormat, anyAlphaBehavior);
            });
    }

    TEST_METHOD(CanvasBitmap_CreateFromColors_FailsIfArrayTooSmall)
    {
        auto device = ref new CanvasDevice();
        auto anyAlphaBehavior = CanvasAlphaBehavior::Premultiplied;

        auto oneElementArray = ref new Platform::Array<Color>(1);

        Assert::ExpectException<Platform::InvalidArgumentException^>(
            [&]
            {
                CanvasBitmap::CreateFromColors(device, oneElementArray, 256, 256, anyAlphaBehavior);
            });
    }

    TEST_METHOD(CanvasBitmap_LoadAyncFromFile_NonDefaultDpi)
    {
        auto canvasDevice = ref new CanvasDevice();

        // Image chosen to be 200% of default
        auto canvasBitmap = WaitExecution(CanvasBitmap::LoadAsync(canvasDevice, L"Assets/HighDpiGrid.png"));

        auto d2dBitmap = GetWrappedResource<ID2D1Bitmap1>(canvasBitmap);
        VerifyDpiAndAlpha(d2dBitmap, 192, D2D1_ALPHA_MODE_PREMULTIPLIED, m_pngDpiTolerance);
    }
};
