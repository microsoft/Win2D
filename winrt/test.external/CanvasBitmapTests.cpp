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
            [&]()
        {
            ICanvasBitmap^ bitmapJpeg = WaitExecution(CanvasBitmap::LoadAsync(canvasDevice, nullptr));
        });

        // Test invalid device parameter.
        Assert::ExpectException<Platform::InvalidArgumentException^>(
            [&]()
        {
            ICanvasBitmap^ bitmapJpeg = WaitExecution(CanvasBitmap::LoadAsync(nullptr, realJpegImage.fileName));
        });
    }
};