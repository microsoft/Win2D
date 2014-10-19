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
using namespace Windows::Graphics::Imaging;
using namespace Windows::UI;
using Platform::String;

#define DPI_TOLERANCE 0.1f

TEST_CLASS(CanvasBitmapTests)
{
    struct TestImage
    {
        Platform::String^ fileName;
        int widthInPixels;
        int heightInPixels;

        float m_dpi;
        float widthInDip;
        float heightInDip;
    };

    TestImage m_testImage;
    CanvasDevice^ m_sharedDevice;

public:
    CanvasBitmapTests()
    {
        m_testImage.fileName = L"Assets/imageTiger.jpg";
        m_testImage.widthInPixels = 196;
        m_testImage.heightInPixels = 147;

        m_testImage.m_dpi = 72.0f;
        const float dpiScaling = DEFAULT_DPI / m_testImage.m_dpi;
        m_testImage.widthInDip = 196.0f * dpiScaling;
        m_testImage.heightInDip = 147.0f * dpiScaling;
        
        m_sharedDevice = ref new CanvasDevice();
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

        VerifyDpiAndAlpha(d2dBitmap, 192, D2D1_ALPHA_MODE_IGNORE, DPI_TOLERANCE);
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
        VerifyDpiAndAlpha(d2dBitmap, 192, D2D1_ALPHA_MODE_PREMULTIPLIED, DPI_TOLERANCE);
    }

    enum DpiVerifyType
    {
        Exact,
        Fuzzy,
        NotSupported
    };

    void VerifyBitmapDecoderDimensionsMatchTestImage(BitmapDecoder^ bitmapDecoder, DpiVerifyType dpiVerifyType)
    {
        Assert::AreEqual(m_testImage.widthInPixels, (int)bitmapDecoder->PixelWidth);
        Assert::AreEqual(m_testImage.heightInPixels, (int)bitmapDecoder->PixelHeight);

        if (dpiVerifyType == Exact)
        {
            Assert::AreEqual(m_testImage.m_dpi, static_cast<float>(bitmapDecoder->DpiX));
            Assert::AreEqual(m_testImage.m_dpi, static_cast<float>(bitmapDecoder->DpiY));
        }
        else if (dpiVerifyType == Fuzzy)
        {
            Assert::AreEqual(m_testImage.m_dpi, static_cast<float>(bitmapDecoder->DpiX), DPI_TOLERANCE);
            Assert::AreEqual(m_testImage.m_dpi, static_cast<float>(bitmapDecoder->DpiY), DPI_TOLERANCE);
        }
        else
        {
            assert(dpiVerifyType == NotSupported);
            Assert::AreEqual(DEFAULT_DPI, static_cast<float>(bitmapDecoder->DpiX));
            Assert::AreEqual(DEFAULT_DPI, static_cast<float>(bitmapDecoder->DpiY));
        }
    }

    BitmapDecoder^ LoadBitmapDecoderFromPath(String^ path, ULONGLONG* optionalSizeOut = nullptr)
    {
        auto storageFile = WaitExecution(Windows::Storage::StorageFile::GetFileFromPathAsync(path));

        if (optionalSizeOut)
        {
            auto basicProperties = WaitExecution(storageFile->GetBasicPropertiesAsync());
            *optionalSizeOut = basicProperties->Size;
        }

        auto stream = WaitExecution(storageFile->OpenReadAsync());
        Windows::Storage::Streams::IRandomAccessStream^ streamBase = stream;
         
        auto bitmapDecoder = WaitExecution_RequiresWorkerThread(BitmapDecoder::CreateAsync(streamBase));

        return bitmapDecoder;
    }

    //
    // BitmapDecoder makes threading assumptions which do not work
    // straightforwardly in this test environment. The default 
    // apartment and properties of the test environment threads
    // do not work with these assumptions. Because of this, it's
    // necessary to fire this worker thread and block on it.
    //
    template<typename T>
    T WaitExecution_RequiresWorkerThread(IAsyncOperation<T>^ asyncOperation)
    {
        T taskResult;

        Event workerThreadFinishedEvent(CreateEventEx(NULL, NULL, CREATE_EVENT_MANUAL_RESET, EVENT_ALL_ACCESS));
        if (!workerThreadFinishedEvent.IsValid())
            ThrowHR(E_OUTOFMEMORY);

        auto workItem = ref new Windows::System::Threading::WorkItemHandler(
            [asyncOperation, &workerThreadFinishedEvent, &taskResult](IAsyncAction^ workItem)
            {
                auto callback = ref new AsyncOperationCompletedHandler<T>(
                    [&workerThreadFinishedEvent, &taskResult](IAsyncOperation<T>^ asyncInfo, Windows::Foundation::AsyncStatus status)
                    {
                        taskResult = asyncInfo->GetResults();
                        SetEvent(workerThreadFinishedEvent.Get());
                    }, Platform::CallbackContext::Any);

                asyncOperation->Completed = callback;
            });

        Windows::System::Threading::ThreadPool::RunAsync(workItem);

        auto waitResult = WaitForSingleObjectEx(workerThreadFinishedEvent.Get(), 1000 * 5, true);
        if (waitResult != WAIT_OBJECT_0)ThrowHR(E_INVALIDARG);

        return taskResult;

    };

    TEST_METHOD(CanvasBitmap_SaveToFileAsync_DetermineEncoderFromFileExtension)
    {
        auto canvasBitmap = WaitExecution(CanvasBitmap::LoadAsync(m_sharedDevice, m_testImage.fileName));

        String^ tempFolder = Windows::Storage::ApplicationData::Current->TemporaryFolder->Path;
        String^ pathPrefix = String::Concat(tempFolder, L"\\test.");

        struct TestCase
        {
            std::wstring Extension;
            Platform::Guid ImagingFormat;
            DpiVerifyType DpiVerify;
        } testCases[] = {
                { L"bmp",   BitmapDecoder::BmpDecoderId,    DpiVerifyType::Fuzzy },
                { L"dib",   BitmapDecoder::BmpDecoderId,    DpiVerifyType::Fuzzy },
                { L"rle",   BitmapDecoder::BmpDecoderId,    DpiVerifyType::Fuzzy },
                { L"png",   BitmapDecoder::PngDecoderId,    DpiVerifyType::Fuzzy },
                { L"jpg",   BitmapDecoder::JpegDecoderId,   DpiVerifyType::Exact },
                { L"JPE",   BitmapDecoder::JpegDecoderId,   DpiVerifyType::Exact },
                { L"jpeg",  BitmapDecoder::JpegDecoderId,   DpiVerifyType::Exact },
                { L"jfif",  BitmapDecoder::JpegDecoderId,   DpiVerifyType::Exact },
                { L"EXIF",  BitmapDecoder::JpegDecoderId,   DpiVerifyType::Exact },
                { L"tif",   BitmapDecoder::TiffDecoderId,   DpiVerifyType::Exact },
                { L"tiff",  BitmapDecoder::TiffDecoderId,   DpiVerifyType::Exact },
                { L"gif",   BitmapDecoder::GifDecoderId,    DpiVerifyType::NotSupported },
                { L"wdp",   BitmapDecoder::JpegXRDecoderId, DpiVerifyType::Exact },
                { L"jxr",   BitmapDecoder::JpegXRDecoderId, DpiVerifyType::Exact },
        };

        for (auto testCase : testCases)
        {
            String^ targetPath = String::Concat(pathPrefix, ref new String(testCase.Extension.c_str()));

            WaitExecution(canvasBitmap->SaveToFileAsync(targetPath));

            auto bitmapDecoder = LoadBitmapDecoderFromPath(targetPath);

            Assert::AreEqual(testCase.ImagingFormat, bitmapDecoder->DecoderInformation->CodecId);

            VerifyBitmapDecoderDimensionsMatchTestImage(bitmapDecoder, testCase.DpiVerify);
        }
    }

    TEST_METHOD(CanvasBitmap_SaveToFileAsync_UseSpecifiedEncoder)
    {
        auto canvasBitmap = WaitExecution(CanvasBitmap::LoadAsync(m_sharedDevice, m_testImage.fileName));

        String^ tempFolder = Windows::Storage::ApplicationData::Current->TemporaryFolder->Path;
        String^ targetPath = String::Concat(tempFolder, L"\\test.bin");

        //
        // BMP and PNG store their DPI as pixels per meter, so DPI is reported
        // as the result of a calculation and is not exact.
        // GIF doesn't support DPI at all, and always returns default.
        //
        struct TestCase
        {
            CanvasBitmapFileFormat CanvasFormat;
            Platform::Guid ImagingFormat;
            DpiVerifyType DpiVerify;
        } testCases[] = {
            { CanvasBitmapFileFormat::Jpeg,     BitmapDecoder::JpegDecoderId,   DpiVerifyType::Exact },
            { CanvasBitmapFileFormat::Bmp,      BitmapDecoder::BmpDecoderId,    DpiVerifyType::Fuzzy },
            { CanvasBitmapFileFormat::Png,      BitmapDecoder::PngDecoderId,    DpiVerifyType::Fuzzy },
            { CanvasBitmapFileFormat::Tiff,     BitmapDecoder::TiffDecoderId,   DpiVerifyType::Exact },
            { CanvasBitmapFileFormat::Gif,      BitmapDecoder::GifDecoderId,    DpiVerifyType::NotSupported },
            { CanvasBitmapFileFormat::JpegXR,   BitmapDecoder::JpegXRDecoderId, DpiVerifyType::Exact },
        };

        for (auto testCase : testCases)
        {
            WaitExecution(canvasBitmap->SaveToFileAsync(targetPath, testCase.CanvasFormat));

            auto bitmapDecoder = LoadBitmapDecoderFromPath(targetPath);

            Assert::AreEqual(testCase.ImagingFormat, bitmapDecoder->DecoderInformation->CodecId);

            VerifyBitmapDecoderDimensionsMatchTestImage(bitmapDecoder, testCase.DpiVerify);
        }
    }

    TEST_METHOD(CanvasBitmap_SaveToBitmapAsync_ImageQuality)
    {
        auto canvasBitmap = WaitExecution(CanvasBitmap::LoadAsync(m_sharedDevice, m_testImage.fileName));

        String^ tempFolder = Windows::Storage::ApplicationData::Current->TemporaryFolder->Path;

        struct TestCase
        {
            CanvasBitmapFileFormat FileFormat;
            bool SupportsQuality;
            DpiVerifyType DpiVerify;
        } testCases[] {
                { CanvasBitmapFileFormat::Bmp, false, DpiVerifyType::Fuzzy },
                { CanvasBitmapFileFormat::Gif, false, DpiVerifyType::NotSupported },
                { CanvasBitmapFileFormat::Jpeg, true, DpiVerifyType::Exact },
                { CanvasBitmapFileFormat::JpegXR, true, DpiVerifyType::Exact },
                { CanvasBitmapFileFormat::Png, false, DpiVerifyType::Fuzzy },
                { CanvasBitmapFileFormat::Tiff, false, DpiVerifyType::Exact },
        };

        for (auto testCase : testCases)
        {
            String^ lowQPath = String::Concat(tempFolder, L"\\low.bin");
            WaitExecution(canvasBitmap->SaveToFileAsync(lowQPath, testCase.FileFormat, 0.1f));

            String^ highQPath = String::Concat(tempFolder, L"\\high.bin");
            WaitExecution(canvasBitmap->SaveToFileAsync(highQPath, testCase.FileFormat, 1.0f));

            ULONGLONG lowQSize;
            auto lowQDecoder = LoadBitmapDecoderFromPath(lowQPath, &lowQSize);
            VerifyBitmapDecoderDimensionsMatchTestImage(lowQDecoder, testCase.DpiVerify);

            ULONGLONG highQSize;
            auto highQDecoder = LoadBitmapDecoderFromPath(highQPath, &highQSize);
            VerifyBitmapDecoderDimensionsMatchTestImage(highQDecoder, testCase.DpiVerify);

            if (testCase.SupportsQuality) Assert::IsTrue(lowQSize < highQSize);
            else Assert::AreEqual(lowQSize, highQSize);
        }
    }

    TEST_METHOD(CanvasBitmap_SaveToBitmapAsync_InvalidArguments)
    {
        auto canvasBitmap = ref new CanvasRenderTarget(m_sharedDevice, 1, 1);

        String^ validPath = String::Concat(Windows::Storage::ApplicationData::Current->TemporaryFolder->Path, L"\\test.bin");

        Assert::ExpectException<Platform::InvalidArgumentException^>(
            [&]
            {
                WaitExecution(canvasBitmap->SaveToFileAsync(L"an invalid path"));
            });

        Assert::ExpectException<Platform::InvalidArgumentException^>(
            [&]
            {
                WaitExecution(canvasBitmap->SaveToFileAsync(L"fileName.jpg."));
            });

        Assert::ExpectException<Platform::InvalidArgumentException^>(
            [&]
            {
                WaitExecution(canvasBitmap->SaveToFileAsync(L""));
            });

        Assert::ExpectException<Platform::InvalidArgumentException^>(
            [&]
            {
                WaitExecution(canvasBitmap->SaveToFileAsync(validPath, static_cast<CanvasBitmapFileFormat>(999)));
            });

        Assert::ExpectException<Platform::InvalidArgumentException^>(
            [&]
            {
                WaitExecution(canvasBitmap->SaveToFileAsync(validPath, CanvasBitmapFileFormat::Jpeg, -FLT_EPSILON));
            });
    }

    TEST_METHOD(CanvasBitmap_SaveAndGetData_Subresource)
    {
        ComPtr<ID3D11Device> d3dDevice;
        ThrowIfFailed(GetDXGIInterface<ID3D11Device>(m_sharedDevice, &d3dDevice));

        const unsigned int sliceCount = 3;
        const unsigned int bindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
        CD3D11_TEXTURE2D_DESC textureDescription(DXGI_FORMAT_B8G8R8A8_UNORM, 8, 8, 1, sliceCount, bindFlags);

        ComPtr<ID3D11Texture2D> texture2D;
        ThrowIfFailed(d3dDevice->CreateTexture2D(&textureDescription, nullptr, &texture2D));

        ComPtr<IDXGIResource1> resource;
        ThrowIfFailed(texture2D.As(&resource));

        ComPtr<IDXGISurface2> slices[sliceCount];

        //
        // This clears each slice to a different color, and verifies the
        // dimensions of the saved file reflect the correct slice.
        // For CPU read, it verifies the output array dimensions and
        // reads back the correct color.
        //
        Color testColors[] = { Colors::Red, Colors::Green, Colors::Blue };
        byte testColorsRawData[3][4] {
                { 0, 0, 0xFF, 0xFF },
                { 0, 0x80, 0, 0xFF },
                { 0xFF, 0, 0, 0xFF },
        };

        String^ savePath = String::Concat(Windows::Storage::ApplicationData::Current->TemporaryFolder->Path, L"\\test.bin");

        for (unsigned int i = 0; i < sliceCount; i++)
        {
            ThrowIfFailed(resource->CreateSubresourceSurface(i, &slices[i]));

            auto wrappedSurface = CreateDirect3DSurface(slices[i].Get());

            auto renderTarget = CanvasRenderTarget::CreateFromDirect3D11Surface(m_sharedDevice, wrappedSurface);

            CanvasDrawingSession^ drawingSession = renderTarget->CreateDrawingSession();
            drawingSession->Clear(testColors[i]);
            delete drawingSession;

            WaitExecution(renderTarget->SaveToFileAsync(savePath, CanvasBitmapFileFormat::Jpeg));

            unsigned int sliceDimension = 8 >> i;

            auto decoder = LoadBitmapDecoderFromPath(savePath);

            Assert::AreEqual(sliceDimension, (unsigned int)decoder->PixelWidth);
            Assert::AreEqual(sliceDimension, (unsigned int)decoder->PixelHeight);

            Platform::Array<byte>^ byteData = renderTarget->GetBytes();
            Assert::AreEqual(sliceDimension * sliceDimension * 4, byteData->Length);
            for (unsigned int j = 0; j < byteData->Length; j += 4)
            {
                Assert::AreEqual(testColorsRawData[i][0], byteData[j + 0]);
                Assert::AreEqual(testColorsRawData[i][1], byteData[j + 1]);
                Assert::AreEqual(testColorsRawData[i][2], byteData[j + 2]);
                Assert::AreEqual(testColorsRawData[i][3], byteData[j + 3]);
            }

            Platform::Array<Color>^ colorData = renderTarget->GetColors();
            Assert::AreEqual(sliceDimension * sliceDimension, colorData->Length);
            for (Color c : colorData)
            {
                Assert::AreEqual(testColors[i], c);
            }
        }


    }

    TEST_METHOD(CanvasBitmap_GetBytes)
    {
        // Test that a byte pattern roundtrips.
        const int width = 8;
        const int height = 9;
        const int totalSize = width * height;
        Platform::Array<byte>^ imageData = ref new Platform::Array<byte>(totalSize * 4);
        for (int i = 0; i < totalSize * 4; i++)
        {
            imageData[i] = i % UCHAR_MAX;
        }

        auto canvasBitmap = CanvasBitmap::CreateFromBytes(
            m_sharedDevice,
            imageData,
            width,
            height,
            DirectXPixelFormat::B8G8R8A8UIntNormalized,
            CanvasAlphaBehavior::Premultiplied);

        // Retrieval of whole bitmap
        Platform::Array<byte>^ retrievedBytes = canvasBitmap->GetBytes();

        Assert::AreEqual(imageData->Length, retrievedBytes->Length);
        for (unsigned int i = 0; i < imageData->Length; i++)
        {
            Assert::AreEqual(imageData[i], retrievedBytes[i]);
        }

        // Retrieval of a small subregion
        retrievedBytes = canvasBitmap->GetBytes(Rect(1, 0, 1, 1));
        Assert::AreEqual(4u, retrievedBytes->Length);
        for (unsigned int i = 4; i < 8; i++)
        {
            Assert::AreEqual(imageData[i], retrievedBytes[i-4]);
        }

        // Retrieval of a typical subregion
        const unsigned int regionWidth = 3;
        const unsigned int regionHeight = 4;
        Rect sourceRect(2, 2, static_cast<float>(regionWidth), static_cast<float>(regionHeight));

        const unsigned int bytesPerPixel = 4;

        retrievedBytes = canvasBitmap->GetBytes(sourceRect);
        Assert::AreEqual(regionWidth * regionHeight * bytesPerPixel, retrievedBytes->Length);


        for (unsigned int y = 0; y < regionHeight; y++)
        {
            for (unsigned int x = 0; x < regionWidth; x++)
            {
                const unsigned int sourcePixelIndex = ((y + static_cast<int>(sourceRect.Top)) * width) + (x + static_cast<int>(sourceRect.Left));
                const unsigned int sourceByteIndex = sourcePixelIndex * bytesPerPixel;
                const unsigned int destPixelIndex = (y * regionWidth) + x;
                const unsigned int destByteIndex = destPixelIndex * bytesPerPixel;
                for (unsigned int k = 0; k < bytesPerPixel; k++)
                {
                    Assert::AreEqual(imageData[sourceByteIndex + k], retrievedBytes[destByteIndex + k]);
                }
            }
        }

        // Retrieval of a subregion the size of the whole bitmap
        retrievedBytes = canvasBitmap->GetBytes(canvasBitmap->Bounds);

        Assert::AreEqual(imageData->Length, retrievedBytes->Length);
        for (unsigned int i = 0; i < imageData->Length; i++)
        {
            Assert::AreEqual(imageData[i], retrievedBytes[i]);
        }
    }

    TEST_METHOD(CanvasBitmap_GetColors)
    {
        // Test that a color pattern roundtrips.
        const int width = 8;
        const int height = 9;
        const int totalSize = width * height;
        Platform::Array<Color>^ imageData = ref new Platform::Array<Color>(totalSize);
        for (int i = 0; i < totalSize; i++)
        {
            imageData[i].R = (i * 4 + 0) % UCHAR_MAX;
            imageData[i].G = (i * 4 + 1) % UCHAR_MAX;
            imageData[i].B = (i * 4 + 2) % UCHAR_MAX;
            imageData[i].A = (i * 4 + 3) % UCHAR_MAX;
        }

        auto canvasBitmap = CanvasBitmap::CreateFromColors(
            m_sharedDevice,
            imageData,
            width,
            height,
            CanvasAlphaBehavior::Premultiplied);

        // Retrieval of whole bitmap
        Platform::Array<Color>^ retrievedColors = canvasBitmap->GetColors();

        Assert::AreEqual(imageData->Length, retrievedColors->Length);
        for (unsigned int i = 0; i < imageData->Length; i++)
        {
            Assert::AreEqual(imageData[i], retrievedColors[i]);
        }

        // Retrieval of a small subregion
        retrievedColors = canvasBitmap->GetColors(Rect(1, 0, 1, 1));
        Assert::AreEqual(1u, retrievedColors->Length);
        Assert::AreEqual(imageData[1], retrievedColors[0]);

        // Retrieval of a typical subregion
        const unsigned int regionWidth = 3;
        const unsigned int regionHeight = 4;
        Rect sourceRect(2, 2, static_cast<float>(regionWidth), static_cast<float>(regionHeight));
        retrievedColors = canvasBitmap->GetColors(sourceRect);
        Assert::AreEqual(regionWidth * regionHeight, retrievedColors->Length);
        for (unsigned int y = 0; y < regionHeight; y++)
        {
            for (unsigned int x = 0; x < regionWidth; x++)
            {
                const unsigned int sourcePixelIndex = ((y + static_cast<int>(sourceRect.Top)) * width) + (x + static_cast<int>(sourceRect.Left));
                const unsigned int destPixelIndex = (y * regionWidth) + x;
                Assert::AreEqual(imageData[sourcePixelIndex], retrievedColors[destPixelIndex]);
            }
        }

        // Retrieval of a subregion the size of the whole bitmap
        retrievedColors = canvasBitmap->GetColors(canvasBitmap->Bounds);

        Assert::AreEqual(imageData->Length, retrievedColors->Length);
        for (unsigned int i = 0; i < imageData->Length; i++)
        {
            Assert::AreEqual(imageData[i], retrievedColors[i]);
        }
    }

    TEST_METHOD(CanvasBitmap_GetBytesAndGetColors_InvalidArguments)
    {
        auto canvasBitmap = ref new CanvasRenderTarget(m_sharedDevice, 1, 1);

        Rect testCases[] = {
            Rect(0, 0, 0, 0), // Retrieval of a zero-sized subregion should fail.
            Rect(0, 0, 2, 2), // Subregion which exceeds bitmap size should fail.
        };

        for (Rect testCase : testCases)
        {
            Assert::ExpectException<Platform::InvalidArgumentException^>(
                [&]
                {
                    canvasBitmap->GetBytes(testCase);
                });

            Assert::ExpectException<Platform::InvalidArgumentException^>(
                [&]
                {
                    canvasBitmap->GetColors(testCase);
                });
        }
    }

    TEST_METHOD(CanvasBitmap_WicBitmapCannotRetrieveD3DProperties)
    {
        // 
        // This test verifies the expected points of failure while interopping native WIC bitmaps
        // through Win2D objects, and attempting to access Direct3D properties of them.
        //
        auto canvasDevice = ref new CanvasDevice();

        WicBitmapTestFixture f = CreateWicBitmapTestFixture();

        auto wicBitmapBasedDrawingSession = GetOrCreate<CanvasDrawingSession>(f.RenderTarget.Get());

        Assert::ExpectException<Platform::COMException^>([&] { wicBitmapBasedDrawingSession->Device; });

        auto wicBitmapBasedCanvasBitmap = GetOrCreate<CanvasBitmap>(canvasDevice, f.Bitmap.Get());

        Assert::ExpectException<Platform::COMException^>([&] { wicBitmapBasedCanvasBitmap->Description; });

    }
};
