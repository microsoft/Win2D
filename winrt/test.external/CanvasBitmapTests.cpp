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

    TEST_METHOD(CanvasBitmap_PropertiesAndClose)
    {
        TestImage realJpegImage;
        realJpegImage.fileName = L"Assets/imageTiger.jpg";
        realJpegImage.widthInPixels = 196;
        realJpegImage.heightInPixels = 147;
        realJpegImage.widthInDip = 196.0f;
        realJpegImage.heightInDip = 147.0f;

        WIN32_FILE_ATTRIBUTE_DATA fileInfo;
        Assert::AreNotEqual(GetFileAttributesEx(realJpegImage.fileName->Data(), GetFileExInfoStandard, &fileInfo), 0);
        
        CanvasDevice^ canvasDevice = ref new CanvasDevice();

        ICanvasBitmap^ bitmapJpeg = WaitExecution(CanvasBitmap::LoadAsync(canvasDevice, realJpegImage.fileName));
        Assert::AreEqual(realJpegImage.widthInPixels, (int)bitmapJpeg->SizeInPixels.Width);
        Assert::AreEqual(realJpegImage.heightInPixels, (int)bitmapJpeg->SizeInPixels.Height);
        Assert::AreEqual(realJpegImage.widthInDip, bitmapJpeg->SizeInDips.Width);
        Assert::AreEqual(realJpegImage.heightInDip, bitmapJpeg->SizeInDips.Height);

        // Test invalid bitmap parameter.
        Assert::ExpectException<Platform::InvalidArgumentException^>(
            [&]
            {
                ICanvasBitmap^ bitmapJpeg = WaitExecution(CanvasBitmap::LoadAsync(canvasDevice, nullptr));
            });

        // Test invalid device parameter.
        Assert::ExpectException<Platform::InvalidArgumentException^>(
            [&]
            {
                ICanvasBitmap^ bitmapJpeg = WaitExecution(CanvasBitmap::LoadAsync(nullptr, realJpegImage.fileName));
            });
    }


    TEST_METHOD(CanvasBitmap_LoadFromInvalidFile)
    {
        try
        {
            CanvasDevice^ canvasDevice = ref new CanvasDevice();

            auto async = CanvasBitmap::LoadAsync(canvasDevice, "ThisImageFileDoesNotExist.jpg");

            WaitExecution(async);

            Assert::Fail(L"Trying to load a missing file should throw!");
        }
        catch (Platform::COMException^ e)
        {
            Assert::AreEqual<unsigned>(HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND), e->HResult);
        }
    }


    TEST_METHOD(CanvasBitmap_LoadMany)
    {
        const int bitmapCount = 8;

        CanvasDevice^ canvasDevice = ref new CanvasDevice();

        ICanvasBitmap^ bitmaps[bitmapCount] = {};

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
};
