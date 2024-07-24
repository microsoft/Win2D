// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

using Platform::String;
using namespace Microsoft::Graphics::Canvas;
using namespace Microsoft::WRL::Wrappers;
using namespace WinRTDirectX;
using namespace Windows::Devices::Enumeration;
using namespace Windows::Foundation;
using namespace Windows::Graphics::Imaging;
using namespace Windows::Storage::Streams;
using namespace Microsoft::UI;


auto gMustBeMultipleOf4ErrorText = L"Block compressed image width & height must be a multiple of 4 pixels.";            
auto gSubRectangleMustBeBlockAligned = L"Subrectangles from block compressed images must be aligned to a multiple of 4 pixels.";


const int c_subresourceSliceCount = 3;

struct SignedRect
{
    SignedRect(int l, int t, int w, int h)
        :Left(l), Top(t), Width(w), Height(h) {}
    int Left;
    int Top;
    int Width;
    int Height;
};

TEST_CLASS(CanvasBitmapTests)
{
    CanvasDevice^ m_sharedDevice;

    static const int testImageWidth = 196;
    static const int testImageHeight = 147;

    Platform::String^ testImageFileName;

public:
    CanvasBitmapTests()
    {
        testImageFileName = L"Assets/imageTiger.jpg";

        m_sharedDevice = ref new CanvasDevice();
    }

    TEST_METHOD(CanvasBitmap_PropertiesAndClose)
    {
        //
        // This exercises a typical test image, as well as the 
        // ability two load two important edge-case formats
        // (bmp and ico).
        //

        struct TestCase
        {
            Platform::String^ FileName;
            int ImageWidth;
            int ImageHeight;
        } testCases[] =
        {
            L"Assets/imageTiger.jpg", testImageWidth, testImageHeight,
            L"Images/x.bmp", 16, 16,
            L"Images/x.ico", 16, 16,
            L"Images/x.tif", 16, 16 // Metadata-less tif
        };

        for (auto testCase : testCases)
        {
            WIN32_FILE_ATTRIBUTE_DATA fileInfo;
            Assert::AreNotEqual(GetFileAttributesEx(testCase.FileName->Data(), GetFileExInfoStandard, &fileInfo), 0);

            CanvasDevice^ canvasDevice = ref new CanvasDevice();

            auto bitmap = WaitExecution(CanvasBitmap::LoadAsync(canvasDevice, testCase.FileName));
            Assert::AreEqual((uint32_t)testCase.ImageWidth, bitmap->SizeInPixels.Width);
            Assert::AreEqual((uint32_t)testCase.ImageHeight, bitmap->SizeInPixels.Height);
            Assert::AreEqual((float)testCase.ImageWidth, bitmap->Size.Width);
            Assert::AreEqual((float)testCase.ImageHeight, bitmap->Size.Height);
        }
    }

    TEST_METHOD(CanvasBitmap_LoadFromInvalidParameters)
    {
        CanvasDevice^ canvasDevice = ref new CanvasDevice();

        auto async = CanvasBitmap::LoadAsync(canvasDevice, "ThisImageFileDoesNotExist.jpg");

        ExpectCOMException(HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND),
            [&]
            {
                WaitExecution(async);
            });            
            
        Uri^ uri = ref new Uri("ms-appx:///nope");

        async = CanvasBitmap::LoadAsync(canvasDevice, uri);

        ExpectCOMException(HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND),
            [&]
            {
                WaitExecution(async);
            });
    }

    TEST_METHOD(CanvasBitmap_LoadStreamAndUri)
    {
        CanvasDevice^ canvasDevice = ref new CanvasDevice();

        for (int i = 0; i < 2; ++i)
        {
            IAsyncOperation<CanvasBitmap^>^ bitmapAsync;
            IAsyncOperation<CanvasBitmap^>^ highDpiAsync;

            const float highDpi = 150;

            Uri^ uri = ref new Uri("ms-appx:///Assets/HighDpiGrid.png");
            if (i == 0)
            {
                bitmapAsync = CanvasBitmap::LoadAsync(canvasDevice, uri, DEFAULT_DPI, CanvasAlphaMode::Ignore);
                highDpiAsync = CanvasBitmap::LoadAsync(canvasDevice, uri, highDpi, CanvasAlphaMode::Ignore);
            }
            else
            {
                auto storageFile = WaitExecution(Windows::Storage::StorageFile::GetFileFromApplicationUriAsync(uri));
                auto stream = WaitExecution(storageFile->OpenReadAsync());

                bitmapAsync = CanvasBitmap::LoadAsync(canvasDevice, stream, DEFAULT_DPI, CanvasAlphaMode::Ignore);

                storageFile = WaitExecution(Windows::Storage::StorageFile::GetFileFromApplicationUriAsync(uri));
                stream = WaitExecution(storageFile->OpenReadAsync());

                highDpiAsync = CanvasBitmap::LoadAsync(canvasDevice, stream, highDpi, CanvasAlphaMode::Ignore);
            }

            // Verify loading at default DPI.
            auto bitmap = WaitExecution(bitmapAsync);

            Assert::AreEqual(4.0f, bitmap->Size.Width);
            Assert::AreEqual(4.0f, bitmap->Size.Height);
            Assert::AreEqual(4u, bitmap->SizeInPixels.Width);
            Assert::AreEqual(4u, bitmap->SizeInPixels.Height);

            auto d2dBitmap = GetWrappedResource<ID2D1Bitmap1>(bitmap);

            VerifyDpiAndAlpha(d2dBitmap, 96, D2D1_ALPHA_MODE_IGNORE);

            // Verify loading at high DPI.
            bitmap = WaitExecution(highDpiAsync);

            Assert::AreEqual(4.0f * 96 / highDpi, bitmap->Size.Width);
            Assert::AreEqual(4.0f * 96 / highDpi, bitmap->Size.Height);
            Assert::AreEqual(4u, bitmap->SizeInPixels.Width);
            Assert::AreEqual(4u, bitmap->SizeInPixels.Height);

            d2dBitmap = GetWrappedResource<ID2D1Bitmap1>(bitmap);

            VerifyDpiAndAlpha(d2dBitmap, highDpi, D2D1_ALPHA_MODE_IGNORE);
        }
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

            Assert::AreEqual(196u, bitmaps[i]->SizeInPixels.Width);
            Assert::AreEqual(147u, bitmaps[i]->SizeInPixels.Height);
        }
    }

    TEST_METHOD(CanvasBitmap_NativeInterop)
    {
        auto canvasDevice = ref new CanvasDevice();

        auto originalBitmap = WaitExecution(CanvasBitmap::LoadAsync(canvasDevice, testImageFileName));
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

        struct SourceArrayTestCase
        {
            Platform::Array<BYTE>^ byteArray;
            Platform::Array<Windows::UI::Color>^ colorArray;
        } sourceArrayTestCases[] =
        {
            { ref new Platform::Array<BYTE>(1), ref new Platform::Array<Windows::UI::Color>(1) },
            { ref new Platform::Array<BYTE>(0), ref new Platform::Array<Windows::UI::Color>(0) },
            { nullptr, nullptr }
        };

        for (auto& testCase : legalSizeTestCases)
        {
            for (auto& sourceArrays : sourceArrayTestCases)
            {
                auto bitmap = CanvasBitmap::CreateFromBytes(
                    canvasDevice,
                    sourceArrays.byteArray,
                    testCase.width,
                    testCase.height,
                    DirectXPixelFormat::A8UIntNormalized, // One of the few formats that is actually supported with straight alpha
                    99.0f,
                    CanvasAlphaMode::Straight);
                auto d2dBitmap = GetWrappedResource<ID2D1Bitmap1>(bitmap);
                auto size = bitmap->SizeInPixels;
                Assert::AreEqual(static_cast<uint32_t>(testCase.width), size.Width);
                Assert::AreEqual(static_cast<uint32_t>(testCase.height), size.Height);
                VerifyDpiAndAlpha(d2dBitmap, 99.0f, D2D1_ALPHA_MODE_STRAIGHT);

                bitmap = CanvasBitmap::CreateFromColors(
                    canvasDevice,
                    sourceArrays.colorArray,
                    testCase.width,
                    testCase.height,
                    25.0f,
                    CanvasAlphaMode::Ignore);
                d2dBitmap = GetWrappedResource<ID2D1Bitmap1>(bitmap);
                size = bitmap->SizeInPixels;
                Assert::AreEqual(static_cast<uint32_t>(testCase.width), size.Width);
                Assert::AreEqual(static_cast<uint32_t>(testCase.height), size.Height);
                VerifyDpiAndAlpha(d2dBitmap, 25.0f, D2D1_ALPHA_MODE_IGNORE);

                // Skip the zero or null source array test cases if the bitmap size is non-zero.
                if (testCase.width > 0 && testCase.height > 0)
                    break;
            }
        }

        SizeTestCase illegalSizeTestCases[] =
        {
            { -1, 1 },
            { 1, -1 },
            { -234, -50 },
        };

        for (auto& testCase : illegalSizeTestCases)
        {
            for (auto& sourceArrays : sourceArrayTestCases)
            {
                Assert::ExpectException<Platform::InvalidArgumentException^>(
                    [&]
                    {
                        auto bitmap = CanvasBitmap::CreateFromBytes(
                            canvasDevice,
                            sourceArrays.byteArray,
                            testCase.width,
                            testCase.height,
                            DirectXPixelFormat::B8G8R8A8UIntNormalized,
                            DEFAULT_DPI,
                            CanvasAlphaMode::Premultiplied);

                        bitmap = CanvasBitmap::CreateFromColors(
                            canvasDevice,
                            sourceArrays.colorArray,
                            testCase.width,
                            testCase.height,
                            DEFAULT_DPI,
                            CanvasAlphaMode::Premultiplied);
                    });
            }
        }
    }

    TEST_METHOD(CanvasBitmap_CreateFromBytes_AcceptsZeroSizedArray)
    {
        auto device = ref new CanvasDevice();
        auto anyFormat = DirectXPixelFormat::B8G8R8A8UIntNormalized;
        auto anyAlphaMode = CanvasAlphaMode::Premultiplied;

        auto zeroSizedArray = ref new Platform::Array<BYTE>(0);
        auto bitmap = CanvasBitmap::CreateFromBytes(device, zeroSizedArray, 0, 0, anyFormat, DEFAULT_DPI, anyAlphaMode);
        
        auto size = bitmap->SizeInPixels;
        Assert::AreEqual(0u, size.Width);
        Assert::AreEqual(0u, size.Height);
    }

    TEST_METHOD(CanvasBitmap_CreateFromColors_AcceptsZeroSizedArray)
    {
        auto device = ref new CanvasDevice();
        auto anyAlphaMode = CanvasAlphaMode::Premultiplied;

        auto zeroSizedArray = ref new Platform::Array<Windows::UI::Color>(0);
        auto bitmap = CanvasBitmap::CreateFromColors(device, zeroSizedArray, 0, 0, DEFAULT_DPI, anyAlphaMode);
        
        auto size = bitmap->SizeInPixels;
        Assert::AreEqual(0u, size.Width);
        Assert::AreEqual(0u, size.Height);
    }

    TEST_METHOD(CanvasBitmap_CreateFromBytes_FailsIfArrayTooSmall)
    {
        auto device = ref new CanvasDevice();
        auto anyFormat = DirectXPixelFormat::B8G8R8A8UIntNormalized;
        auto anyAlphaMode = CanvasAlphaMode::Premultiplied;

        auto oneElementArray = ref new Platform::Array<BYTE>(1);

        Assert::ExpectException<Platform::InvalidArgumentException^>(
            [&]
            {
                CanvasBitmap::CreateFromBytes(device, oneElementArray, 256, 256, anyFormat, DEFAULT_DPI, anyAlphaMode);
            });
    }

    TEST_METHOD(CanvasBitmap_CreateFromColors_FailsIfArrayTooSmall)
    {
        auto device = ref new CanvasDevice();
        auto anyAlphaMode = CanvasAlphaMode::Premultiplied;

        auto oneElementArray = ref new Platform::Array<Windows::UI::Color>(1);

        Assert::ExpectException<Platform::InvalidArgumentException^>(
            [&]
            {
                CanvasBitmap::CreateFromColors(device, oneElementArray, 256, 256, DEFAULT_DPI, anyAlphaMode);
            });
    }

    TEST_METHOD(CanvasBitmap_LoadAyncFromFile_NonDefaultDpi)
    {
        auto canvasDevice = ref new CanvasDevice();

        // Image chosen to be 200% of default
        auto canvasBitmap = WaitExecution(CanvasBitmap::LoadAsync(canvasDevice, L"Assets/HighDpiGrid.png"));

        auto d2dBitmap = GetWrappedResource<ID2D1Bitmap1>(canvasBitmap);

        // We should ignore the DPI metadata saved in the file, and return a bitmap with default 96 DPI.
        VerifyDpiAndAlpha(d2dBitmap, DEFAULT_DPI, D2D1_ALPHA_MODE_PREMULTIPLIED);
    }

    void VerifyBitmapDecoderDimensionsMatchTestImage(BitmapDecoder^ bitmapDecoder)
    {
        Assert::AreEqual(testImageWidth, (int)bitmapDecoder->PixelWidth);
        Assert::AreEqual(testImageHeight, (int)bitmapDecoder->PixelHeight);
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
        DisableDebugLayer disableDebug; // 6184116 causes the debug layer to fail when CanvasBitmap::SaveAsync is called
        auto device = ref new CanvasDevice();
        auto canvasBitmap = WaitExecution(CanvasBitmap::LoadAsync(device, testImageFileName));

        String^ tempFolder = Windows::Storage::ApplicationData::Current->TemporaryFolder->Path;
        String^ pathPrefix = String::Concat(tempFolder, L"\\test.");

        struct TestCase
        {
            std::wstring Extension;
            Platform::Guid ImagingFormat;
        } testCases[] = {
                { L"bmp",   BitmapDecoder::BmpDecoderId    },
                { L"dib",   BitmapDecoder::BmpDecoderId    },
                { L"rle",   BitmapDecoder::BmpDecoderId    },
                { L"png",   BitmapDecoder::PngDecoderId    },
                { L"jpg",   BitmapDecoder::JpegDecoderId   },
                { L"JPE",   BitmapDecoder::JpegDecoderId   },
                { L"jpeg",  BitmapDecoder::JpegDecoderId   },
                { L"jfif",  BitmapDecoder::JpegDecoderId   },
                { L"EXIF",  BitmapDecoder::JpegDecoderId   },
                { L"tif",   BitmapDecoder::TiffDecoderId   },
                { L"tiff",  BitmapDecoder::TiffDecoderId   },
                { L"gif",   BitmapDecoder::GifDecoderId    },
                { L"wdp",   BitmapDecoder::JpegXRDecoderId },
                { L"jxr",   BitmapDecoder::JpegXRDecoderId },
        };

        for (auto testCase : testCases)
        {
            String^ targetPath = String::Concat(pathPrefix, ref new String(testCase.Extension.c_str()));

            WaitExecution(canvasBitmap->SaveAsync(targetPath));

            auto bitmapDecoder = LoadBitmapDecoderFromPath(targetPath);

            Assert::AreEqual(testCase.ImagingFormat, bitmapDecoder->DecoderInformation->CodecId);

            VerifyBitmapDecoderDimensionsMatchTestImage(bitmapDecoder);
        }
    }

    TEST_METHOD(CanvasBitmap_SaveToFileAndStreamAsync_UseSpecifiedEncoder)
    {
        DisableDebugLayer disableDebug; // 6184116 causes the debug layer to fail when CanvasBitmap::SaveAsync is called
        auto device = ref new CanvasDevice();
        auto canvasBitmap = WaitExecution(CanvasBitmap::LoadAsync(device, testImageFileName));

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
        } testCases[] = {
            { CanvasBitmapFileFormat::Jpeg,     BitmapDecoder::JpegDecoderId   },
            { CanvasBitmapFileFormat::Bmp,      BitmapDecoder::BmpDecoderId    },
            { CanvasBitmapFileFormat::Png,      BitmapDecoder::PngDecoderId    },
            { CanvasBitmapFileFormat::Tiff,     BitmapDecoder::TiffDecoderId   },
            { CanvasBitmapFileFormat::Gif,      BitmapDecoder::GifDecoderId    },
            { CanvasBitmapFileFormat::JpegXR,   BitmapDecoder::JpegXRDecoderId },
        };

        for (auto testCase : testCases)
        {
            WaitExecution(canvasBitmap->SaveAsync(targetPath, testCase.CanvasFormat));

            auto bitmapDecoder = LoadBitmapDecoderFromPath(targetPath);

            Assert::AreEqual(testCase.ImagingFormat, bitmapDecoder->DecoderInformation->CodecId);

            VerifyBitmapDecoderDimensionsMatchTestImage(bitmapDecoder);
        }

        for (auto testCase : testCases)
        {
            InMemoryRandomAccessStream^ memoryStream = ref new InMemoryRandomAccessStream();

            WaitExecution(canvasBitmap->SaveAsync(memoryStream, testCase.CanvasFormat));

            auto bitmapDecoder = WaitExecution_RequiresWorkerThread(BitmapDecoder::CreateAsync(memoryStream));

            Assert::AreEqual(testCase.ImagingFormat, bitmapDecoder->DecoderInformation->CodecId);

            VerifyBitmapDecoderDimensionsMatchTestImage(bitmapDecoder);
        }
    }

    TEST_METHOD(CanvasBitmap_SaveToBitmapAsync_ImageQuality)
    {
        DisableDebugLayer disableDebug; // 6184116 causes the debug layer to fail when CanvasBitmap::SaveAsync is called
        auto device = ref new CanvasDevice();
        auto canvasBitmap = WaitExecution(CanvasBitmap::LoadAsync(device, testImageFileName));

        String^ tempFolder = Windows::Storage::ApplicationData::Current->TemporaryFolder->Path;

        struct TestCase
        {
            CanvasBitmapFileFormat FileFormat;
            bool SupportsQuality;
        } testCases[] {
                { CanvasBitmapFileFormat::Bmp,    false },
                { CanvasBitmapFileFormat::Gif,    false },
                { CanvasBitmapFileFormat::Jpeg,   true  },
                { CanvasBitmapFileFormat::JpegXR, true  },
                { CanvasBitmapFileFormat::Png,    false },
                { CanvasBitmapFileFormat::Tiff,   false },
        };

        for (auto testCase : testCases)
        {
            String^ lowQPath = String::Concat(tempFolder, L"\\low.bin");
            WaitExecution(canvasBitmap->SaveAsync(lowQPath, testCase.FileFormat, 0.1f));

            String^ highQPath = String::Concat(tempFolder, L"\\high.bin");
            WaitExecution(canvasBitmap->SaveAsync(highQPath, testCase.FileFormat, 1.0f));

            ULONGLONG lowQSize;
            auto lowQDecoder = LoadBitmapDecoderFromPath(lowQPath, &lowQSize);
            VerifyBitmapDecoderDimensionsMatchTestImage(lowQDecoder);

            ULONGLONG highQSize;
            auto highQDecoder = LoadBitmapDecoderFromPath(highQPath, &highQSize);
            VerifyBitmapDecoderDimensionsMatchTestImage(highQDecoder);

            if (testCase.SupportsQuality) Assert::IsTrue(lowQSize < highQSize);
            else Assert::AreEqual(lowQSize, highQSize);
        }
    }

    TEST_METHOD(CanvasBitmap_SaveToFileAsync_PixelFormats)
    {
        DisableDebugLayer disableDebug; // 6184116 causes the debug layer to fail when CanvasBitmap::SaveAsync is called
        auto device = ref new CanvasDevice(true);

        // This checks that we can at least save all supported bitmap formats,
        // but doesn't validate what actually gets saved.

        struct TestCase
        {
            DirectXPixelFormat Format;
            CanvasAlphaMode AlphaMode;
            HRESULT ExpectedHResult;

            TestCase(DirectXPixelFormat format, HRESULT hr = S_OK)
                : Format(format)
                , AlphaMode(CanvasAlphaMode::Ignore)
                , ExpectedHResult(hr)
            {}

            TestCase(DirectXPixelFormat format, CanvasAlphaMode alphaMode, HRESULT hr = S_OK)
                : Format(format)
                , AlphaMode(alphaMode)
                , ExpectedHResult(hr)
            {}
        };

        TestCase testCases[] =
            {
                { DirectXPixelFormat::B8G8R8A8UIntNormalized },
                { DirectXPixelFormat::B8G8R8A8UIntNormalizedSrgb },
                { DirectXPixelFormat::B8G8R8X8UIntNormalized },
                { DirectXPixelFormat::R8G8B8A8UIntNormalized },
                { DirectXPixelFormat::R8G8B8A8UIntNormalizedSrgb },
                { DirectXPixelFormat::R16G16B16A16Float },
                { DirectXPixelFormat::R32G32B32A32Float },
                { DirectXPixelFormat::R10G10B10A2UIntNormalized },
                { DirectXPixelFormat::R16G16B16A16UIntNormalized },
                { DirectXPixelFormat::BC1UIntNormalized },
                { DirectXPixelFormat::BC2UIntNormalized },
                { DirectXPixelFormat::BC3UIntNormalized },
                { DirectXPixelFormat::A8UIntNormalized,   CanvasAlphaMode::Straight },
                { DirectXPixelFormat::R8G8UIntNormalized, WINCODEC_ERR_UNSUPPORTEDPIXELFORMAT },
                { DirectXPixelFormat::R8UIntNormalized,   WINCODEC_ERR_UNSUPPORTEDPIXELFORMAT },
            };

        for (auto testCase : testCases)
        {
            if (!device->IsPixelFormatSupported(testCase.Format))
                continue;
            
            auto data = ref new Platform::Array<uint8_t>(1024);
            auto width = 4;
            auto height = 4;
            auto bitmap = CanvasBitmap::CreateFromBytes(device, data, width, height, testCase.Format, DEFAULT_DPI, testCase.AlphaMode);

            auto stream = ref new InMemoryRandomAccessStream();

            auto operation = bitmap->SaveAsync(stream, CanvasBitmapFileFormat::Bmp);

            if (SUCCEEDED(testCase.ExpectedHResult))
            {
                WaitExecution(operation);
            }
            else
            {
                ExpectCOMException(testCase.ExpectedHResult, [&] { WaitExecution(operation); });
            }
        }
    }

    TEST_METHOD(CanvasBitmap_SaveToFileAsync_InvalidArguments)
    {
        auto canvasBitmap = ref new CanvasRenderTarget(m_sharedDevice, 1, 1, DEFAULT_DPI);

        String^ validPath = String::Concat(Windows::Storage::ApplicationData::Current->TemporaryFolder->Path, L"\\test.bin");

        Assert::ExpectException<Platform::InvalidArgumentException^>(
            [&]
            {
                WaitExecution(canvasBitmap->SaveAsync(L"an invalid path"));
            });

        Assert::ExpectException<Platform::InvalidArgumentException^>(
            [&]
            {
                WaitExecution(canvasBitmap->SaveAsync(L"fileName.jpg."));
            });

        Assert::ExpectException<Platform::InvalidArgumentException^>(
            [&]
            {
                WaitExecution(canvasBitmap->SaveAsync(L""));
            });

        Assert::ExpectException<Platform::InvalidArgumentException^>(
            [&]
            {
                WaitExecution(canvasBitmap->SaveAsync(validPath, static_cast<CanvasBitmapFileFormat>(999)));
            });

        Assert::ExpectException<Platform::InvalidArgumentException^>(
            [&]
            {
                WaitExecution(canvasBitmap->SaveAsync(validPath, CanvasBitmapFileFormat::Jpeg, -FLT_EPSILON));
            });

        // We do not support saving DDS files
        Assert::ExpectException<Platform::InvalidArgumentException^>(
            [&]
            {
                WaitExecution(canvasBitmap->SaveAsync(L"foo.dds"));
            });
    }

    struct SubresourceTestFixture
    {
        CanvasRenderTarget^ m_renderTargets[c_subresourceSliceCount];

    public:
        SubresourceTestFixture(CanvasDevice^ device) 
            : m_renderTargets{}
        {
            ComPtr<ID3D11Device> d3dDevice;
            ThrowIfFailed(GetDXGIInterface<ID3D11Device>(device, &d3dDevice));

            const unsigned int bindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
            CD3D11_TEXTURE2D_DESC textureDescription(DXGI_FORMAT_B8G8R8A8_UNORM, 8, 8, 1, c_subresourceSliceCount, bindFlags);

            ComPtr<ID3D11Texture2D> texture2D;
            ThrowIfFailed(d3dDevice->CreateTexture2D(&textureDescription, nullptr, &texture2D));

            ComPtr<IDXGIResource1> resource;
            ThrowIfFailed(texture2D.As(&resource));

            //
            // This clears each slice to a different color. If Win2D
            // wrapped the wrong slice, tests will read the wrong color
            // and size.
            //
            for (unsigned int i = 0; i < c_subresourceSliceCount; i++)
            {
                ComPtr<IDXGISurface2> slice;

                ThrowIfFailed(resource->CreateSubresourceSurface(i, &slice));

                auto wrappedSurface = CreateDirect3DSurface(slice.Get());

                m_renderTargets[i] = CanvasRenderTarget::CreateFromDirect3D11Surface(device, wrappedSurface);

                CanvasDrawingSession^ drawingSession = m_renderTargets[i]->CreateDrawingSession();
                drawingSession->Clear(GetExpectedColor(i));
                delete drawingSession;
            }
        }

        Windows::UI::Color GetExpectedColor(int sliceIndex)
        {
            assert(sliceIndex >= 0 && sliceIndex < c_subresourceSliceCount);
            static const  Windows::UI::Color colors[c_subresourceSliceCount] = { Colors::Red, Colors::Green, Colors::Blue };
            return colors[sliceIndex];
        }

        byte GetExpectedByte(int sliceIndex, int byteNumber)
        {
            assert(sliceIndex >= 0 && sliceIndex < c_subresourceSliceCount);
            assert(byteNumber >= 0 && byteNumber < 4);
            static const byte bytes[c_subresourceSliceCount][4] {
                    { 0, 0, 0xFF, 0xFF },
                    { 0, 0x80, 0, 0xFF },
                    { 0xFF, 0, 0, 0xFF },
            };
            return bytes[sliceIndex][byteNumber];
        }

        CanvasRenderTarget^ GetRenderTarget(int sliceIndex)
        {
            assert(sliceIndex >= 0 && sliceIndex < c_subresourceSliceCount);
            return m_renderTargets[sliceIndex];
        }
    };

    TEST_METHOD(CanvasBitmap_SaveToFile_Subresource)
    {
        DisableDebugLayer disableDebug; // 6184116 causes the debug layer to fail when CanvasBitmap::SaveAsync is called
        auto device = ref new CanvasDevice();
        SubresourceTestFixture f(device);

        String^ savePath = String::Concat(Windows::Storage::ApplicationData::Current->TemporaryFolder->Path, L"\\test.bin");

        for (unsigned int i = 0; i < c_subresourceSliceCount; i++)
        {
            WaitExecution(f.GetRenderTarget(i)->SaveAsync(savePath, CanvasBitmapFileFormat::Jpeg));

            unsigned int sliceDimension = 8 >> i;

            auto decoder = LoadBitmapDecoderFromPath(savePath);

            Assert::AreEqual(sliceDimension, (unsigned int)decoder->PixelWidth);
            Assert::AreEqual(sliceDimension, (unsigned int)decoder->PixelHeight);
        }
    }

    TEST_METHOD(CanvasBitmap_GetData_Subresource)
    {
        // Skip this test on hardware known to fail it due to a driver bug.
        // Normally we wouldn't bother with checks like this, but Shawn has
        // become fed up with the test constantly showing red on his laptop :-(
        if (GpuMatchesDescription(m_sharedDevice, L"Intel(R) HD Graphics 3000"))
            return;

        SubresourceTestFixture f(m_sharedDevice);

        for (unsigned int i = 0; i < c_subresourceSliceCount; i++)
        {
            unsigned int sliceDimension = 8 >> i;

            Platform::Array<byte>^ byteData = f.GetRenderTarget(i)->GetPixelBytes();
            Assert::AreEqual(sliceDimension * sliceDimension * 4, byteData->Length);
            for (unsigned int j = 0; j < byteData->Length; j += 4)
            {
                Assert::AreEqual(f.GetExpectedByte(i, 0), byteData[j + 0]);
                Assert::AreEqual(f.GetExpectedByte(i, 1), byteData[j + 1]);
                Assert::AreEqual(f.GetExpectedByte(i, 2), byteData[j + 2]);
                Assert::AreEqual(f.GetExpectedByte(i, 3), byteData[j + 3]);
            }

            Platform::Array<Windows::UI::Color>^ colorData = f.GetRenderTarget(i)->GetPixelColors();
            Assert::AreEqual(sliceDimension * sliceDimension, colorData->Length);
            for (Windows::UI::Color c : colorData)
            {
                Assert::AreEqual(f.GetExpectedColor(i), c);
            }
        }
    }

    template<typename TYPE>
    TYPE ReferenceColorFromIndex(int i)
    {
        Assert::Fail();
        TYPE t;
        return t;
    }

    template<>
    byte ReferenceColorFromIndex(int i)
    {
        return i % UCHAR_MAX;
    }

    template<typename TYPE>
    void WriteReferenceDataToArray(Platform::Array<TYPE>^ t)
    {
        for (unsigned int i = 0; i < t->Length; i++)
        {
            t[i] = ReferenceColorFromIndex<TYPE>(i);
        }
    }

    template<typename TYPE>
    void VerifyArraysEqual(Platform::Array<TYPE>^ arr1, Platform::Array<TYPE>^ arr2)
    {
        Assert::AreEqual(arr1->Length, arr2->Length);

        for (unsigned int i = 0; i < arr1->Length; i++)
        {
            Assert::AreEqual(arr1[i], arr2[i]);
        }
    }

    template<>
    Windows::UI::Color ReferenceColorFromIndex(int i)
    {
        Windows::UI::Color c;
        c.R = (i * 4 + 0) % UCHAR_MAX;
        c.G = (i * 4 + 1) % UCHAR_MAX;
        c.B = (i * 4 + 2) % UCHAR_MAX;
        c.A = (i * 4 + 3) % UCHAR_MAX;
        return c;
    }

    template<typename TYPE>
    Platform::Array<TYPE>^ GetDataFunction(CanvasBitmap^ canvasBitmap)
    {
        Assert::Fail();
        return nullptr; // Unexpected
    }

    template<>
    Platform::Array<byte>^ GetDataFunction(CanvasBitmap^ canvasBitmap)
    {
        return canvasBitmap->GetPixelBytes();
    }

    template<>
    Platform::Array<Windows::UI::Color>^ GetDataFunction(CanvasBitmap^ canvasBitmap)
    {
        return canvasBitmap->GetPixelColors();
    }

    template<typename TYPE>
    Platform::Array<TYPE>^ GetDataFunction(CanvasBitmap^ canvasBitmap, int left, int top, int width, int height)
    {
        Assert::Fail();
        return nullptr; // Unexpected
    }

    template<>
    Platform::Array<byte>^ GetDataFunction(CanvasBitmap^ canvasBitmap, int left, int top, int width, int height)
    {
        return canvasBitmap->GetPixelBytes(left, top, width, height);
    }

    template<>
    Platform::Array<Windows::UI::Color>^ GetDataFunction(CanvasBitmap^ canvasBitmap, int left, int top, int width, int height)
    {
        return canvasBitmap->GetPixelColors(left, top, width, height);
    }

    void SetBitmapData(CanvasBitmap^ canvasBitmap, Platform::Array<byte>^ dataArray)
    {
        return canvasBitmap->SetPixelBytes(dataArray);
    }

    void SetBitmapData(CanvasBitmap^ canvasBitmap, Platform::Array<Windows::UI::Color>^ dataArray)
    {
        return canvasBitmap->SetPixelColors(dataArray);
    }

    void SetBitmapData(CanvasBitmap^ canvasBitmap, Platform::Array<byte>^ dataArray, int left, int top, int width, int height)
    {
        return canvasBitmap->SetPixelBytes(dataArray, left, top, width, height);
    }

    void SetBitmapData(CanvasBitmap^ canvasBitmap, Platform::Array<Windows::UI::Color>^ dataArray, int left, int top, int width, int height)
    {
        return canvasBitmap->SetPixelColors(dataArray, left, top, width, height);
    }

    void IncrementColor(byte& b)
    {
        b++;
    }

    void IncrementColor(Windows::UI::Color& c)
    {
        c.R++;
        c.B++;
        c.G++;
        c.A++;
    }

    template<typename TYPE>
    void VerifyGetWholeBitmapData(
        CanvasBitmap^ canvasBitmap,
        Platform::Array<TYPE>^ imageData)
    {
        Platform::Array<TYPE>^ retrievedData = GetDataFunction<TYPE>(canvasBitmap);

        Assert::AreEqual(imageData->Length, retrievedData->Length);
        for (unsigned int i = 0; i < imageData->Length; i++)
        {
            Assert::AreEqual(imageData[i], retrievedData[i]);
        }
    }

    template<typename TYPE>
    void VerifyGetSubrectangleData(
        CanvasBitmap^ canvasBitmap,
        int bitmapWidth,
        Platform::Array<TYPE>^ imageData,
        unsigned int elementsPerPixel,
        SignedRect subrectangle)
    {
        Platform::Array<TYPE>^ retrievedData = GetDataFunction<TYPE>(
            canvasBitmap, 
            subrectangle.Left, 
            subrectangle.Top, 
            subrectangle.Width, 
            subrectangle.Height);

        Assert::AreEqual(subrectangle.Width * subrectangle.Height * elementsPerPixel, retrievedData->Length);
        
        for (int y = 0; y < subrectangle.Height; y++)
        {
            for (int x = 0; x < subrectangle.Width; x++)
            {
                const unsigned int sourcePixelIndex = ((y + subrectangle.Top) * bitmapWidth) + (x + subrectangle.Left);
                const unsigned int sourceElementIndex = sourcePixelIndex * elementsPerPixel;

                const unsigned int destPixelIndex = (y * subrectangle.Width) + x;
                const unsigned int destElementIndex = destPixelIndex * elementsPerPixel;

                for (unsigned int i = 0; i < elementsPerPixel; i++)
                {
                    Assert::AreEqual(imageData[sourceElementIndex + i], retrievedData[destElementIndex + i]);
                }
            }
        }
    }

    template<typename TYPE>
    void VerifySetWholeBitmapData(
        CanvasBitmap^ canvasBitmap,
        Platform::Array<TYPE>^ imageData)
    {
        for (unsigned int i = 0; i < imageData->Length; i++)
        {
            IncrementColor(imageData[i]);
        }

        SetBitmapData(canvasBitmap, imageData);

        VerifyGetWholeBitmapData(canvasBitmap, imageData);
    }

    template<typename TYPE>
    void VerifySetSubrectangleData(
        CanvasBitmap^ canvasBitmap,
        int bitmapWidth,
        Platform::Array<TYPE>^ imageData,
        unsigned int elementsPerPixel,
        SignedRect subrectangle)
    {
        const unsigned int arraySize = subrectangle.Width * subrectangle.Height * elementsPerPixel;
        Platform::Array<TYPE>^ dataArray = ref new Platform::Array<TYPE>(arraySize);

        for (unsigned int i = 0; i < arraySize; i++)
        {
            dataArray[i] = ReferenceColorFromIndex<TYPE>(i);

            // Locate this element in imageData and update it accordingly.
            const unsigned int pixelIndex = i / elementsPerPixel;
            const unsigned int xWithinSubrect = pixelIndex % subrectangle.Width;
            const unsigned int yWithinSubrect = pixelIndex / subrectangle.Width;
            const unsigned int bitmapX = subrectangle.Left + xWithinSubrect;
            const unsigned int bitmapY = subrectangle.Top + yWithinSubrect;
            const unsigned int bitmapIndex = (bitmapY * bitmapWidth) + bitmapX;
            const unsigned int subElement = (i % elementsPerPixel);
            const unsigned int elementIndex = bitmapIndex * elementsPerPixel + subElement;
            imageData[elementIndex] = dataArray[i];
        }

        SetBitmapData(canvasBitmap, dataArray, subrectangle.Left, subrectangle.Top, subrectangle.Width, subrectangle.Height);

        VerifyGetWholeBitmapData(canvasBitmap, imageData);
    }

    template<typename TYPE>
    void VerifyBitmapGetData(
        CanvasBitmap^ canvasBitmap,
        int bitmapWidth,
        Platform::Array<TYPE>^ imageData,
        unsigned int elementsPerPixel)
    {
        // Whole bitmap
        VerifyGetWholeBitmapData<TYPE>(canvasBitmap, imageData);

        // 1x1 subrectangle
        VerifyGetSubrectangleData<TYPE>(canvasBitmap, bitmapWidth, imageData, elementsPerPixel, SignedRect(1, 0, 1, 1));

        // Typical subrectangle
        VerifyGetSubrectangleData<TYPE>(canvasBitmap, bitmapWidth, imageData, elementsPerPixel, SignedRect(2, 2, 3, 4));

        // Subrectangle which happens to be the whole bitmap
        SignedRect wholeBounds(0, 0, static_cast<int>(canvasBitmap->Bounds.Width), static_cast<int>(canvasBitmap->Bounds.Height));
        VerifyGetSubrectangleData<TYPE>(canvasBitmap, bitmapWidth, imageData, elementsPerPixel, wholeBounds);
    }

    template<typename TYPE>
    void VerifyBitmapSetData(
        CanvasBitmap^ canvasBitmap,
        int bitmapWidth,
        Platform::Array<TYPE>^ imageData,
        unsigned int elementsPerPixel)
    {
        // Whole bitmap
        VerifySetWholeBitmapData<TYPE>(canvasBitmap, imageData);

        // 1x1 subrectangle
        VerifySetSubrectangleData<TYPE>(canvasBitmap, bitmapWidth, imageData, elementsPerPixel, SignedRect(1, 0, 1, 1));

        // Typical subrectangle
        VerifySetSubrectangleData<TYPE>(canvasBitmap, bitmapWidth, imageData, elementsPerPixel, SignedRect(2, 2, 3, 4));

        // Subrectangle which happens to be the whole bitmap
        SignedRect wholeBounds(0, 0, static_cast<int>(canvasBitmap->Bounds.Width), static_cast<int>(canvasBitmap->Bounds.Height));
        VerifySetSubrectangleData<TYPE>(canvasBitmap, bitmapWidth, imageData, elementsPerPixel, wholeBounds);
    }

    TEST_METHOD(CanvasBitmap_GetPixelBytesAndSetPixelBytes)
    {
        // Test that a byte pattern roundtrips.
        const int width = 8;
        const int height = 9;
        const int totalSize = width * height;
        Platform::Array<byte>^ imageData = ref new Platform::Array<byte>(totalSize * 4);
        for (int i = 0; i < totalSize * 4; i++)
        {
            imageData[i] = ReferenceColorFromIndex<byte>(i);
        }

        auto canvasBitmap = CanvasBitmap::CreateFromBytes(
            m_sharedDevice,
            imageData,
            width,
            height,
            DirectXPixelFormat::B8G8R8A8UIntNormalized,
            DEFAULT_DPI,
            CanvasAlphaMode::Premultiplied);
            
        VerifyBitmapGetData<byte>(canvasBitmap, width, imageData, 4);

        VerifyBitmapSetData<byte>(canvasBitmap, width, imageData, 4);
    }

    TEST_METHOD(CanvasBitmap_GetPixelColorsAndSetPixelColors)
    {
        // Test that a color pattern roundtrips.
        const int width = 8;
        const int height = 9;
        const int totalSize = width * height;
        Platform::Array<Windows::UI::Color>^ imageData = ref new Platform::Array<Windows::UI::Color>(totalSize);
        for (int i = 0; i < totalSize; i++)
        {
            imageData[i] = ReferenceColorFromIndex<Windows::UI::Color>(i);
        }

        auto canvasBitmap = CanvasBitmap::CreateFromColors(
            m_sharedDevice,
            imageData,
            width,
            height,
            DEFAULT_DPI,
            CanvasAlphaMode::Premultiplied);

        VerifyBitmapGetData<Windows::UI::Color>(canvasBitmap, width, imageData, 1);

        VerifyBitmapSetData<Windows::UI::Color>(canvasBitmap, width, imageData, 1);
    }

    TEST_METHOD(CanvasBitmap_GetAndSetPixelBytesAndColors_InvalidArguments)
    {
        auto canvasBitmap = ref new CanvasRenderTarget(m_sharedDevice, 1, 1, DEFAULT_DPI);

        SignedRect testCases[] = {
            SignedRect(0, 0, 0, 0), // Retrieval of a zero-sized subregion should fail.
            SignedRect(0, 0, 2, 2), // Subregion which exceeds bitmap size should fail.
            SignedRect(-2, 3, 5, 4), // Negative extents should fail.
            SignedRect(2, -3, 5, 4), 
            SignedRect(0, 0, -1, 3),
            SignedRect(0, 0, 1, -3),
        };

        Platform::Array<byte>^ byteArray = ref new Platform::Array<byte>(1);
        Platform::Array<Windows::UI::Color>^ colorArray = ref new Platform::Array<Windows::UI::Color>(1);

        for (SignedRect testCase : testCases)
        {
            Assert::ExpectException<Platform::InvalidArgumentException^>(
                [&]
                {
                canvasBitmap->GetPixelBytes(testCase.Left, testCase.Top, testCase.Width, testCase.Height);
                });

            Assert::ExpectException<Platform::InvalidArgumentException^>(
                [&]
                {
                canvasBitmap->GetPixelColors(testCase.Left, testCase.Top, testCase.Width, testCase.Height);
                });

            Assert::ExpectException<Platform::InvalidArgumentException^>(
                [&]
                {
                canvasBitmap->SetPixelBytes(byteArray, testCase.Left, testCase.Top, testCase.Width, testCase.Height);
                });

            Assert::ExpectException<Platform::InvalidArgumentException^>(
                [&]
                {
                canvasBitmap->SetPixelColors(colorArray, testCase.Left, testCase.Top, testCase.Width, testCase.Height);
                });

        }
    }

    TEST_METHOD(CanvasRenderTarget_SetPixelBytes_InvalidArraySize_ThrowsDescriptiveException)
    {
        auto rt = ref new CanvasRenderTarget(m_sharedDevice, 2, 2, DEFAULT_DPI);
        Platform::Array<byte>^ bytes = ref new Platform::Array<byte>(1);

        try
        {
            rt->SetPixelBytes(bytes, 0, 0, 2, 2);
            Assert::Fail(L"Expected exception not thrown");
        }
        catch (Platform::InvalidArgumentException^ e)
        {
            std::wstring msg(e->Message->Data());
            Assert::IsTrue(msg.find(L"The array was expected to be of size 16; actual array was of size 1.") != std::wstring::npos);
        }
    }

    TEST_METHOD(CanvasRenderTarget_SetPixelColors_InvalidArraySize_ThrowsDescriptiveException)
    {
        auto rt = ref new CanvasRenderTarget(m_sharedDevice, 2, 2, DEFAULT_DPI);
        Platform::Array<Windows::UI::Color>^ colors = ref new Platform::Array<Windows::UI::Color>(1);

        ExpectCOMException(
            E_INVALIDARG,
            L"The array was expected to be of size 4; actual array was of size 1.", 
            [&]
            {
            rt->SetPixelColors(colors, 0, 0, 2, 2);
            });   
    }

    TEST_METHOD(CanvasRenderTarget_PixelColors_InvalidPixelFormat_ThrowsDescriptiveException)
    {
        auto rt = ref new CanvasRenderTarget(m_sharedDevice, 1, 1, DEFAULT_DPI, DirectXPixelFormat::R8G8B8A8UIntNormalized, CanvasAlphaMode::Premultiplied);
        Platform::Array<Windows::UI::Color>^ colors = ref new Platform::Array<Windows::UI::Color>(1);

        const wchar_t* expectedMessage = L"This method only supports resources with pixel format DirectXPixelFormat.B8G8R8A8UIntNormalized.";

        ExpectCOMException(E_INVALIDARG, expectedMessage,
            [&]
            {
                rt->SetPixelColors(colors);
            });    
            
        ExpectCOMException(E_INVALIDARG, expectedMessage,
            [&]
            {
                rt->GetPixelColors();
            });    
    }

    TEST_METHOD(CanvasBitmap_SetPixelBytesAndSetPixelColors_ZeroArrayOnZeroSizedBitmap)
    {
        //
        // Because we have the convention that you cannot get a zero-sized array
        // as data, we also have the convention that you cannot set a zero array. 
        // Verify this corner case on a zero-sized bitmap.
        //
        auto canvasBitmap = ref new CanvasRenderTarget(m_sharedDevice, 0, 0, DEFAULT_DPI);

        Platform::Array<byte>^ zeroSizedByteArray = ref new Platform::Array<byte>(0);
        Assert::ExpectException<Platform::InvalidArgumentException^>(
            [&]
            {
                canvasBitmap->SetPixelBytes(zeroSizedByteArray);
            });
        Assert::ExpectException<Platform::InvalidArgumentException^>(
            [&]
            {
                canvasBitmap->SetPixelBytes(zeroSizedByteArray, 0, 0, 0, 0);
            });

        Platform::Array<Windows::UI::Color>^ zeroSizedColorArray = ref new Platform::Array<Windows::UI::Color>(0);
        Assert::ExpectException<Platform::InvalidArgumentException^>(
            [&]
            {
                canvasBitmap->SetPixelColors(zeroSizedColorArray);
            });
        Assert::ExpectException<Platform::InvalidArgumentException^>(
            [&]
            {
                canvasBitmap->SetPixelColors(zeroSizedColorArray, 0, 0, 0, 0);
            });
    }

    TEST_METHOD(CanvasBitmap_SetPixelBytesAndSetPixelColors_InvalidArguments)
    {
        const int bitmapWidth = 2;
        const int bitmapHeight = 3;
        const int expectedByteSizeOfBitmap = bitmapWidth * bitmapHeight * 4;
        const int expectedColorCountOfBitmap = bitmapWidth * bitmapHeight;

        auto canvasBitmap = ref new CanvasRenderTarget(m_sharedDevice, static_cast<float>(bitmapWidth), static_cast<float>(bitmapHeight), DEFAULT_DPI);
        
        SignedRect subrectangle(0, 0, 1, 2);
        const int expectedByteSizeOfSubrectangle = subrectangle.Width * subrectangle.Height * 4;
        const int expectedColorCountOfSubrectangle = subrectangle.Width * subrectangle.Height;

        Assert::ExpectException<Platform::InvalidArgumentException^>(
            [&]
            {
                Platform::Array<byte>^ array = ref new Platform::Array<byte>(expectedByteSizeOfBitmap - 1);
                canvasBitmap->SetPixelBytes(array);
            });
        
        Assert::ExpectException<Platform::InvalidArgumentException^>(
            [&]
            {
                Platform::Array<Windows::UI::Color>^ array = ref new Platform::Array<Windows::UI::Color>(expectedColorCountOfBitmap - 1);
                canvasBitmap->SetPixelColors(array);
            });
        Assert::ExpectException<Platform::InvalidArgumentException^>(
            [&]
            {
                Platform::Array<byte>^ array = ref new Platform::Array<byte>(expectedByteSizeOfSubrectangle - 1);
                canvasBitmap->SetPixelBytes(array, subrectangle.Left, subrectangle.Top, subrectangle.Width, subrectangle.Height);
            });
        Assert::ExpectException<Platform::InvalidArgumentException^>(
            [&]
            {
                Platform::Array<Windows::UI::Color>^ array = ref new Platform::Array<Windows::UI::Color>(expectedColorCountOfSubrectangle - 1);
                canvasBitmap->SetPixelColors(array, subrectangle.Left, subrectangle.Top, subrectangle.Width, subrectangle.Height);
            });
    }

    TEST_METHOD(CanvasBitmap_SetPixelBytes_AcceptsArraysLargerThanRequired)
    {
        auto width = 256;
        auto height = 256;
        auto bytesPerPixel = 4;

        auto requiredSize = width * height * bytesPerPixel;

        auto data = ref new Platform::Array<uint8_t>(requiredSize + 1);

        auto bitmap = CanvasBitmap::CreateFromBytes(
            m_sharedDevice,
            data,
            width,
            height,
            DirectXPixelFormat::B8G8R8A8UIntNormalized);

        bitmap->SetPixelBytes(data);
    }

    TEST_METHOD(CanvasBitmap_SetPixelColors_AcceptsArraysLargerThanRequired)
    {
        auto width = 256;
        auto height = 256;

        auto requiredSize = width * height;

        auto data = ref new Platform::Array<Windows::UI::Color>(requiredSize + 1);

        auto bitmap = CanvasBitmap::CreateFromColors(
            m_sharedDevice,
            data,
            width,
            height);

        bitmap->SetPixelColors(data);
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


    template<typename TYPE>
    void VerifySubresourceSlice(CanvasBitmap^ bitmap, int sliceDimension, int elementsPerPixel)
    {
        Platform::Array<TYPE>^ sourceData = ref new Platform::Array<TYPE>(sliceDimension * sliceDimension * elementsPerPixel);
        WriteReferenceDataToArray<TYPE>(sourceData);
        SetBitmapData(bitmap, sourceData);
           
        Platform::Array<TYPE>^ retrievedBytes = GetDataFunction<TYPE>(bitmap);
        VerifyArraysEqual<TYPE>(sourceData, retrievedBytes);
    }

    TEST_METHOD(CanvasBitmap_SetData_Subresource)
    {
        SubresourceTestFixture f(m_sharedDevice);

        for (unsigned int i = 0; i < c_subresourceSliceCount; i++)
        {
            unsigned int sliceDimension = 8 >> i;

            VerifySubresourceSlice<byte>(f.GetRenderTarget(i), sliceDimension, 4);

            VerifySubresourceSlice<Windows::UI::Color>(f.GetRenderTarget(i), sliceDimension, 1);
        }
    }

    TEST_METHOD(CanavasBitmap_CopyPixelsFromBitmap_SameBitmap)
    {
        // We expect this test to hit debug layer validation failures, so must run it without the debug layer.
        DisableDebugLayer disableDebug;

        //
        // This is validated by D2D. Ensure the expected error occurs.
        //
        auto device = ref new CanvasDevice();
        auto bitmap = ref new CanvasRenderTarget(device, 1, 1, DEFAULT_DPI);

        Assert::ExpectException<Platform::InvalidArgumentException^>([&] { bitmap->CopyPixelsFromBitmap(bitmap); });

        Assert::ExpectException<Platform::InvalidArgumentException^>([&] { bitmap->CopyPixelsFromBitmap(bitmap, 0, 0); });

        Assert::ExpectException<Platform::InvalidArgumentException^>([&] { bitmap->CopyPixelsFromBitmap(bitmap, 0, 0, 0, 0, 1, 1); });
    }

    TEST_METHOD(CanvasBitmap_SaveToStreamAsync_InvalidArguments)
    {
        auto canvasBitmap = ref new CanvasRenderTarget(m_sharedDevice, 1, 1, DEFAULT_DPI);

        auto validStream = ref new InMemoryRandomAccessStream();

        Assert::ExpectException<Platform::InvalidArgumentException^>(
            [&]
            {
                IRandomAccessStream^ nullStream = nullptr;
                canvasBitmap->SaveAsync(nullStream, CanvasBitmapFileFormat::Bmp);
            });

        Assert::ExpectException<Platform::InvalidArgumentException^>(
            [&]
            {
                WaitExecution(canvasBitmap->SaveAsync(validStream, CanvasBitmapFileFormat::Auto));
            });

        Assert::ExpectException<Platform::InvalidArgumentException^>(
            [&]
            {
                WaitExecution(canvasBitmap->SaveAsync(validStream, static_cast<CanvasBitmapFileFormat>(999)));
            });

        Assert::ExpectException<Platform::InvalidArgumentException^>(
            [&]
            {
                WaitExecution(canvasBitmap->SaveAsync(validStream, CanvasBitmapFileFormat::Jpeg, -FLT_EPSILON));
            });
    }

    ref class StreamWithRestrictions sealed : public IRandomAccessStream
    {
        InMemoryRandomAccessStream^ m_wrappedStream;
        bool m_canRead;
        bool m_canWrite;

    public:
        StreamWithRestrictions(bool canRead, bool canWrite)
            : m_canRead(canRead)
            , m_canWrite(canWrite)
        {
            m_wrappedStream = ref new InMemoryRandomAccessStream();
        }

        virtual ~StreamWithRestrictions() // Implements Dispose which is required
        {
        }

        virtual IAsyncOperationWithProgress<IBuffer ^, unsigned int>^ ReadAsync(IBuffer^ buffer, unsigned int offset, InputStreamOptions options)
        {
            if (m_canRead)
            {
                return m_wrappedStream->ReadAsync(buffer, offset, options);
            }
            else
            {
                throw ref new Platform::NotImplementedException();
            }
        }

        virtual IAsyncOperationWithProgress<unsigned int, unsigned int>^ WriteAsync(IBuffer^ buffer)
        {
            if (m_canWrite)
            {
                return m_wrappedStream->WriteAsync(buffer);
            }
            else
            {
                throw ref new Platform::NotImplementedException();
            }
        }

        virtual IAsyncOperation<bool>^ FlushAsync()
        {
            return m_wrappedStream->FlushAsync();
        }

        virtual property bool CanRead { bool get() { return m_canRead; } }

        virtual property bool CanWrite { bool get() { return m_canWrite; } }

        virtual property unsigned __int64 Position { unsigned __int64 get() { return m_wrappedStream->Position; } }

        virtual property unsigned __int64 Size 
        { 
            unsigned __int64 get() { return m_wrappedStream->Size; }
            void set(unsigned __int64 value) { m_wrappedStream->Size = value; }
        } 

        virtual IInputStream^ GetInputStreamAt(unsigned __int64 position)
        {
            return m_wrappedStream->GetInputStreamAt(position);
        }

        virtual IOutputStream^ GetOutputStreamAt(unsigned __int64 position)
        {
            return m_wrappedStream->GetOutputStreamAt(position);
        }

        virtual void Seek(unsigned __int64 position)
        {
            m_wrappedStream->Seek(position);
        }

        virtual IRandomAccessStream^ CloneStream()
        {
            Assert::Fail(); // Not impl
        }
    };

    TEST_METHOD(CanvasBitmap_InvalidStreams)
    {
        DisableDebugLayer disableDebug; // 6184116 causes the debug layer to fail when CanvasBitmap::SaveAsync is called
        auto device = ref new CanvasDevice();
        
        // Ensure an unreadable stream causes LoadAsync to fail.
        StreamWithRestrictions^ streamWithNoRead = ref new StreamWithRestrictions(false, true); 
               
        auto asyncLoad = CanvasBitmap::LoadAsync(device, streamWithNoRead);
        Assert::ExpectException<Platform::NotImplementedException^>(
            [&]
            {
                WaitExecution(asyncLoad);
            });

        asyncLoad = CanvasBitmap::LoadAsync(device, streamWithNoRead);
        Assert::ExpectException<Platform::NotImplementedException^>(
            [&]
            {
                WaitExecution(asyncLoad);
            });

        // Ensure an unwritable stream causes SaveToStreamAsync to fail.
        StreamWithRestrictions^ streamWithNoWrite = ref new StreamWithRestrictions(true, false);
        auto bitmap = ref new CanvasRenderTarget(device, 1, 1, DEFAULT_DPI);
        auto asyncSave = bitmap->SaveAsync(streamWithNoWrite, CanvasBitmapFileFormat::Bmp);
        Assert::ExpectException<Platform::NotImplementedException^>(
            [&]
            {
                WaitExecution(asyncSave);
            });

        asyncSave = bitmap->SaveAsync(streamWithNoWrite, CanvasBitmapFileFormat::Bmp, 0.6f);        
        Assert::ExpectException<Platform::NotImplementedException^>(
            [&]
            {
                WaitExecution(asyncSave);
            });
    }

    ref class StreamWithNetworkProblems sealed : public IRandomAccessStream
    {
        InMemoryRandomAccessStream^ m_wrappedStream;

    public:
        StreamWithNetworkProblems()
        {
            m_wrappedStream = ref new InMemoryRandomAccessStream();
        }

        virtual ~StreamWithNetworkProblems() // Implements Dispose which is required
        {
        }

        virtual IAsyncOperationWithProgress<IBuffer ^, unsigned int>^ ReadAsync(IBuffer^ buffer, unsigned int offset, InputStreamOptions options)
        {
            return create_async([=](progress_reporter<unsigned int> reporter) -> IBuffer^
            {
                //
                // This conditional is necessary because of the behavior of the ARM compiler.
                // It checks control flow more aggressively; a lambda which simply throws
                // will cause an 'unreachable code' warning with its caller.
                //
                if (buffer != nullptr) 
                {
                    throw ref new Platform::COMException(INET_E_DOWNLOAD_FAILURE);
                }
                return nullptr;
            });

        }

        virtual IAsyncOperationWithProgress<unsigned int, unsigned int>^ WriteAsync(IBuffer^ buffer)
        {
            return create_async([=](progress_reporter<unsigned int> reporter) -> unsigned int
            {
                throw ref new Platform::COMException(INET_E_CONNECTION_TIMEOUT);
            });
        }

        virtual IAsyncOperation<bool>^ FlushAsync()
        {
            return m_wrappedStream->FlushAsync();
        }

        virtual property bool CanRead { bool get() { return true; } }

        virtual property bool CanWrite { bool get() { return true; } }

        virtual property unsigned __int64 Position { unsigned __int64 get() { return m_wrappedStream->Position; } }

        virtual property unsigned __int64 Size
        {
            unsigned __int64 get() { return m_wrappedStream->Size; }
            void set(unsigned __int64 value) { m_wrappedStream->Size = value; }
        }

        virtual IInputStream^ GetInputStreamAt(unsigned __int64 position)
        {
            return m_wrappedStream->GetInputStreamAt(position);
        }

        virtual IOutputStream^ GetOutputStreamAt(unsigned __int64 position)
        {
            return m_wrappedStream->GetOutputStreamAt(position);
        }

        virtual void Seek(unsigned __int64 position)
        {
            m_wrappedStream->Seek(position);
        }

        virtual IRandomAccessStream^ CloneStream()
        {
            Assert::Fail(); // Not impl
        }
    };

    TEST_METHOD(CanvasBitmap_UnreliableStreams)
    {
        // Ensure an unreliable stream causes LoadAsync and SaveToStreamAsync to fail,
        // and doesn't cause AVs or ungraceful failures.
        StreamWithNetworkProblems^ unreliableStream = ref new StreamWithNetworkProblems();

        auto asyncLoad = CanvasBitmap::LoadAsync(m_sharedDevice, unreliableStream);
        ExpectCOMException(INET_E_DOWNLOAD_FAILURE,
            [&]
            {
                WaitExecution(asyncLoad);
            });

        asyncLoad = CanvasBitmap::LoadAsync(m_sharedDevice, unreliableStream);
        ExpectCOMException(INET_E_DOWNLOAD_FAILURE, 
            [&]
            {
                WaitExecution(asyncLoad);
            });

        DisableDebugLayer disableDebug; // 6184116 causes the debug layer to fail when CanvasBitmap::SaveAsync is called
        auto device = ref new CanvasDevice();

        auto bitmap = ref new CanvasRenderTarget(device, 1, 1, DEFAULT_DPI);

        auto asyncSave = bitmap->SaveAsync(unreliableStream, CanvasBitmapFileFormat::Bmp);
        ExpectCOMException(INET_E_CONNECTION_TIMEOUT,
            [&]
            {
                WaitExecution(asyncSave);
            });
        
        asyncSave = bitmap->SaveAsync(unreliableStream, CanvasBitmapFileFormat::Bmp, 0.5f);
        ExpectCOMException(INET_E_CONNECTION_TIMEOUT, 
            [&]
            {
                WaitExecution(asyncSave);
            });

    }

    //
    // These DDS tests are against real-life DDS files.  The
    // "Images/build_dds_test.cmd" script generates the source images.
    //
    
    TEST_METHOD(CanvasBitmap_DDS)
    {
        struct TestCase
        {
            std::wstring Name;
            HRESULT ExpectedLoadResult;
            DirectXPixelFormat ExpectedPixelFormat;
            wchar_t const* ExpectedExceptionMessage;

            TestCase(wchar_t const* name, DirectXPixelFormat format)
                : Name(name)
                , ExpectedLoadResult(S_OK)
                , ExpectedPixelFormat(format)
                , ExpectedExceptionMessage(nullptr)
            {}

            TestCase(wchar_t const* name, HRESULT hr, wchar_t const* msg = nullptr)
                : Name(name)
                , ExpectedLoadResult(hr)
                , ExpectedPixelFormat{}
                , ExpectedExceptionMessage(msg)
            {}
        };

        TestCase testCases[]
        {
            { L"DXT1",            DirectXPixelFormat::BC1UIntNormalized },
            { L"DXT2",            DirectXPixelFormat::BC2UIntNormalized },
            { L"DXT3",            DirectXPixelFormat::BC2UIntNormalized },
            { L"DXT4",            DirectXPixelFormat::BC3UIntNormalized },
            { L"DXT5",            DirectXPixelFormat::BC3UIntNormalized },
            { L"BC1_UNORM",       DirectXPixelFormat::BC1UIntNormalized },
            { L"BC2_UNORM",       DirectXPixelFormat::BC2UIntNormalized },
            { L"BC3_UNORM",       DirectXPixelFormat::BC3UIntNormalized },

            // WIC only loads BC[123]_UNORM block compressed formats.  Any other
            // formats (including non-block compressed formats) fail to load.
                
            { L"BC1_UNORM_SRGB",  WINCODEC_ERR_BADHEADER },
            { L"BC2_UNORM_SRGB",  WINCODEC_ERR_BADHEADER },
            { L"BC3_UNORM_SRGB",  WINCODEC_ERR_BADHEADER },
            { L"D3DFMT_ARGB",     WINCODEC_ERR_BADHEADER },
            { L"R8G8B8A8_UNORM",  WINCODEC_ERR_BADHEADER },
            { L"B8G8R8A8_UNORM",  WINCODEC_ERR_BADHEADER },

            // Image dimensions must be multiples of 4
            { L"1x1", E_FAIL, gMustBeMultipleOf4ErrorText },
            { L"2x2", E_FAIL, gMustBeMultipleOf4ErrorText },
            { L"3x3", E_FAIL, gMustBeMultipleOf4ErrorText },
            { L"4x4", DirectXPixelFormat::BC1UIntNormalized },
            { L"5x4", E_FAIL, gMustBeMultipleOf4ErrorText },
            { L"4x5", E_FAIL, gMustBeMultipleOf4ErrorText },
        };

        auto device = ref new CanvasDevice();

        for (auto testCase : testCases)
        {
            auto fileName = std::wstring(L"Images/x_") + testCase.Name + L".DDS";

            auto loadOperation = CanvasBitmap::LoadAsync(device, ref new Platform::String(fileName.c_str()));

            if (FAILED(testCase.ExpectedLoadResult))
            {
                ExpectCOMException(
                    testCase.ExpectedLoadResult,
                    testCase.ExpectedExceptionMessage,
                    [&] { WaitExecution(loadOperation); });
            }
            else
            {
                auto bitmap = WaitExecution(loadOperation);
                Assert::AreEqual(CanvasAlphaMode::Premultiplied, bitmap->AlphaMode);
                Assert::AreEqual(testCase.ExpectedPixelFormat, bitmap->Format, fileName.c_str());
            }
        }
    }

    TEST_METHOD(CanvasBitmap_DDS_ExplicitAlphaMode)
    {
        auto device = ref new CanvasDevice();

        struct TestCase
        {
            CanvasAlphaMode AlphaMode;
            HRESULT ExpectedLoadResult;
        };

        TestCase testCases[]
        {
            { CanvasAlphaMode::Premultiplied, S_OK },
            { CanvasAlphaMode::Straight,      E_INVALIDARG },
            { CanvasAlphaMode::Ignore,        E_INVALIDARG }
        };

        for (auto testCase : testCases)
        {
            auto loadOperation = CanvasBitmap::LoadAsync(device, "Images/x_DXT1.DDS", 96, testCase.AlphaMode);

            if (FAILED(testCase.ExpectedLoadResult))
            {
                ExpectCOMException(testCase.ExpectedLoadResult, [&] { WaitExecution(loadOperation); });
            }
            else
            {
                WaitExecution(loadOperation);
            }
        }
    }

    TEST_METHOD(CanvasBitmap_DDS_Dpi)
    {
        auto device = ref new CanvasDevice();

        float testCases[] = { 12, 34, 56, 78, 89 };

        for (auto testCase : testCases)
        {
            auto bitmap = WaitExecution(CanvasBitmap::LoadAsync(device, "Images/x_DXT1.DDS", testCase));
                        
            Assert::AreEqual(testCase, bitmap->Dpi);
        }
    }

    static void ForAllBlockCompressedFormats(std::function<void(DirectXPixelFormat)> testFn)
    {
        DirectXPixelFormat blockCompressedFormats[]
        {
            DirectXPixelFormat::BC1UIntNormalized,
            DirectXPixelFormat::BC2UIntNormalized,
            DirectXPixelFormat::BC3UIntNormalized
        };

        for (auto bcFormat : blockCompressedFormats)
        {
            testFn(bcFormat);
        }
    }

    TEST_METHOD(CanvasBitmap_CreateFromBytes_FailWhenNotMultipleOf4)
    {
        ForAllBlockCompressedFormats(
            [] (DirectXPixelFormat format)
            {
                auto device = ref new CanvasDevice();
            
                BitmapSize invalidSizes[]
                {
                    { 1, 1 },
                    { 4, 5 },
                    { 5, 4 }
                };
            
                auto data = ref new Platform::Array<uint8_t>(100);
            
                for (BitmapSize invalidSize : invalidSizes)
                {
                    auto width = invalidSize.Width;
                    auto height = invalidSize.Height;
                    
                    ExpectCOMException(
                        E_INVALIDARG,
                        gMustBeMultipleOf4ErrorText,
                        [&] { CanvasBitmap::CreateFromBytes(device, data, width, height, format); });
                }
            });
    }

    static unsigned GetBytesPerBlock(DirectXPixelFormat format)
    {
        switch (format)
        {
        case DirectXPixelFormat::BC1UIntNormalized: return 8;
        case DirectXPixelFormat::BC2UIntNormalized: return 16;
        case DirectXPixelFormat::BC3UIntNormalized: return 16;
        default: ThrowHR(E_UNEXPECTED);
        }
    }

    struct BcFixture
    {
        DirectXPixelFormat Format;
        CanvasDevice^ Device;

        unsigned Width;
        unsigned Height;
        
        unsigned BlocksWide;
        unsigned BlocksHigh;

        Platform::Array<uint8_t>^ Data;

        BcFixture(DirectXPixelFormat format)
            : Format(format)
            , Device(ref new CanvasDevice())
            , Width(16)
            , Height(16)
            , BlocksWide(Width / 4)
            , BlocksHigh(Height / 4)
            , Data(ref new Platform::Array<uint8_t>(BlocksWide * BlocksHigh * GetBytesPerBlock(Format)))
        {
            std::fill(begin(Data), end(Data), static_cast<uint8_t>(0));
        }

        CanvasBitmap^ CreateBitmap()
        {
            return CanvasBitmap::CreateFromBytes(Device, Data, Width, Height, Format);
        }
    };

    TEST_METHOD(CanvasBitmap_GetPixelBytes)
    {
        ForAllBlockCompressedFormats(
            [] (DirectXPixelFormat format)
            {
                BcFixture f(format);
                
                for (auto i = 0u; i < f.Data->Length; ++i)
                {
                    f.Data[i] = static_cast<uint8_t>(i);
                }

                auto bitmap = f.CreateBitmap();
                auto pixelBytes = bitmap->GetPixelBytes();

                Assert::AreEqual(f.Data->Length, pixelBytes->Length);
        
                for (auto i = 0u; i < f.Data->Length; ++i)
                {
                    Assert::AreEqual(f.Data[i], pixelBytes[i]);
                }
            });
    }

    TEST_METHOD(CanvasBitmap_GetPixelBytes_SubRectangle)
    {
        ForAllBlockCompressedFormats(
            [] (DirectXPixelFormat format)
            {
                BcFixture f(format);

                // This test will just extract the bytes for the region (4,4) -
                // (8,8) (which is a single block).  So we'll write 0's for
                // everything outside that region.

                uint8_t insideValue = 0;
                
                for (auto i = 0u; i < f.Data->Length; ++i)
                {
                    auto blockI = (i / GetBytesPerBlock(format));
                    auto blockX = (blockI % f.BlocksWide);
                    auto blockY = (blockI / f.BlocksWide);

                    if (blockX == 1 && blockY == 1)
                        f.Data[i] = insideValue++;
                    else
                        f.Data[i] = 0;
                }
                
                auto bitmap = f.CreateBitmap();
                auto pixelBytes = bitmap->GetPixelBytes(4, 4, 4, 4);

                Assert::AreEqual<uint32_t>(insideValue, pixelBytes->Length);
                
                for (auto i = 0u; i < pixelBytes->Length; ++i)
                {
                    Assert::AreEqual(static_cast<uint8_t>(i), pixelBytes[i]);
                }
            });
    }

    TEST_METHOD(CanvasBitmap_SetPixelBytes)
    {
        ForAllBlockCompressedFormats(
            [] (DirectXPixelFormat format)
            {
                BcFixture f(format);
                auto bitmap = f.CreateBitmap();

                for (auto i = 0u; i < f.Data->Length; ++i)
                {
                    f.Data[i] = static_cast<uint8_t>(i);
                }

                bitmap->SetPixelBytes(f.Data);

                auto pixelBytes = bitmap->GetPixelBytes();

                Assert::AreEqual(f.Data->Length, pixelBytes->Length);
                for (auto i = 0u; i < f.Data->Length; ++i)
                {
                    Assert::AreEqual(f.Data[i], pixelBytes[i]);
                }
            });
    }

    TEST_METHOD(CanvasBitmap_SetPixelBytes_SubRectangle)
    {
        ForAllBlockCompressedFormats(
            [] (DirectXPixelFormat format)
            {
                BcFixture f(format);
                auto bitmap = f.CreateBitmap();

                auto subRectData = ref new Platform::Array<uint8_t>(GetBytesPerBlock(format));

                for (auto i = 0u; i < subRectData->Length; ++i)
                {
                    subRectData[i] = static_cast<uint8_t>(i);
                }

                bitmap->SetPixelBytes(subRectData, 4, 4, 4, 4);

                auto pixelBytes = bitmap->GetPixelBytes(4, 4, 4, 4);

                Assert::AreEqual(subRectData->Length, pixelBytes->Length);
                for (auto i = 0u; i < subRectData->Length; ++i)
                {
                    Assert::AreEqual(subRectData[i], pixelBytes[i]);
                }
            });
    }

    static void ForAllInvalidBcSubRectangles(std::function<void(int, int, int, int)> testFn)
    {
        struct R
        {
            int X;
            int Y;
            int Width;
            int Height;
        };

        R invalidSubRectangles[]
        {
            { 1, 4, 4, 4 },
            { 4, 1, 4, 4 },
            { 4, 4, 5, 4 },
            { 4, 4, 4, 5 }
        };

        for (auto r : invalidSubRectangles)
        {
            testFn(r.X, r.Y, r.Width, r.Height);
        }
    }

    TEST_METHOD(CanvasBitmap_GetPixelBytes_SubRectangle_FailsWhenUnaligned)
    {
        ForAllBlockCompressedFormats(
            [] (DirectXPixelFormat format)
            {
                BcFixture f(format);

                auto bitmap = f.CreateBitmap();

                ForAllInvalidBcSubRectangles(
                    [&] (uint32_t x, uint32_t y, uint32_t w, uint32_t h)
                    {
                        ExpectCOMException(
                            E_INVALIDARG,
                            gSubRectangleMustBeBlockAligned,
                            [&] { bitmap->GetPixelBytes(x, y, w, h); });
                    });
            });
    }

    TEST_METHOD(CanvasBitmap_SetPixelBytes_SubRectangle_FailsWhenUnaligned)
    {
        ForAllBlockCompressedFormats(
            [] (DirectXPixelFormat format)
            {
                BcFixture f(format);

                auto bitmap = f.CreateBitmap();

                ForAllInvalidBcSubRectangles(
                    [&] (uint32_t x, uint32_t y, uint32_t w, uint32_t h)
                    {
                        auto data = ref new Platform::Array<uint8_t>(100);
                        ExpectCOMException(
                            E_INVALIDARG,
                            gSubRectangleMustBeBlockAligned,
                            [&] { bitmap->SetPixelBytes(data, x, y, w, h); });
                    });
            });
    }

    TEST_METHOD(CanvasBitmap_CopyPixelsFromBitmap_BlockCompressed)
    {
        ForAllBlockCompressedFormats(
            [] (DirectXPixelFormat format)
            {
                BcFixture f(format);

                auto srcBitmap = f.CreateBitmap();
                auto dstBitmap = f.CreateBitmap();

                dstBitmap->CopyPixelsFromBitmap(srcBitmap);
                dstBitmap->CopyPixelsFromBitmap(srcBitmap, 4, 4, 4, 4, 8, 8);
            });
    }

    TEST_METHOD(CanvasBitmap_CopyPixelsFromBitmap_BlockCompressed_FailsWhenUnaligned)
    {
        ForAllBlockCompressedFormats(
            [] (DirectXPixelFormat format)
            {
                BcFixture f(format);

                auto srcBitmap = f.CreateBitmap();
                auto dstBitmap = f.CreateBitmap();

                ExpectCOMException(E_INVALIDARG, gSubRectangleMustBeBlockAligned, [&] { dstBitmap->CopyPixelsFromBitmap(srcBitmap, 1, 0, 0, 0, 4, 4); });
                ExpectCOMException(E_INVALIDARG, gSubRectangleMustBeBlockAligned, [&] { dstBitmap->CopyPixelsFromBitmap(srcBitmap, 0, 1, 0, 0, 4, 4); });

                ForAllInvalidBcSubRectangles(
                    [&] (uint32_t x, uint32_t y, uint32_t w, uint32_t h)
                    {
                        ExpectCOMException(E_INVALIDARG, gSubRectangleMustBeBlockAligned, [&] { dstBitmap->CopyPixelsFromBitmap(srcBitmap, 0, 0, x, y, w, h); });
                    });
            });
    }

    template<typename T, size_t N>
    static Platform::ArrayReference<BYTE> AsArrayReference(T(&array)[N])
    {
        return Platform::ArrayReference<BYTE>(reinterpret_cast<BYTE*>(array), sizeof(T) * N);
    }

    static void AssertPixelValues(Platform::Array<BYTE>^ expected, Platform::Array<BYTE>^ actual)
    {
        Assert::AreEqual(expected->Length, actual->Length);

        for (auto i = 0u; i < expected->Length; i++)
        {
            Assert::AreEqual(expected->get(i), actual->get(i));
        }
    }

    template<typename TValue>
    static void TestCopyPixelsFromBitmap(CanvasDevice^ device1, CanvasDevice^ device2, DirectXPixelFormat format, DirectXPixelFormat otherFormat, int blockSize = 1)
    {
        TValue initialData1[] =
        {
            1,  2,  3,  4,
            5,  6,  7,  8,
            9,  10, 11, 12,
            13, 14, 15, 16
        };

        TValue initialData2[] =
        {
            17, 18, 
            19, 20
        };

        auto bitmap1 = CanvasBitmap::CreateFromBytes(device1, AsArrayReference(initialData1), 4 * blockSize, 4 * blockSize, format);
        auto bitmap2 = CanvasBitmap::CreateFromBytes(device2, AsArrayReference(initialData2), 2 * blockSize, 2 * blockSize, format);

        // Overload #1
        bitmap1->CopyPixelsFromBitmap(bitmap2);

        TValue expected1[] = 
        { 
            17, 18, 3,  4, 
            19, 20, 7,  8, 
            9,  10, 11, 12, 
            13, 14, 15, 16 
        };

        AssertPixelValues(AsArrayReference(expected1), bitmap1->GetPixelBytes());

        // Overload #2
        bitmap1->CopyPixelsFromBitmap(bitmap2, 2 * blockSize, 1 * blockSize);

        TValue expected2[] =
        {
            17, 18, 3,  4,
            19, 20, 17, 18,
            9,  10, 19, 20,
            13, 14, 15, 16
        };

        AssertPixelValues(AsArrayReference(expected2), bitmap1->GetPixelBytes());

        // Overload #3
        bitmap1->CopyPixelsFromBitmap(bitmap2, 1 * blockSize, 3 * blockSize, 0 * blockSize, 1 * blockSize, 2 * blockSize, 1 * blockSize);

        TValue expected3[] =
        {
            17, 18, 3,  4,
            19, 20, 17, 18,
            9,  10, 19, 20,
            13, 19, 20, 16
        };

        AssertPixelValues(AsArrayReference(expected3), bitmap1->GetPixelBytes());

        // Copying between bitmaps of different formats fails.
        auto otherFormatBitmap = CanvasBitmap::CreateFromBytes(device2, AsArrayReference(initialData1), 4, 4, otherFormat);

        ExpectCOMException(E_INVALIDARG, L"Bitmaps are not the same pixel format.", [&] { bitmap1->CopyPixelsFromBitmap(otherFormatBitmap); });

        // Out-of-bounds copies fail.
        ExpectCOMException(E_INVALIDARG, [&] { bitmap1->CopyPixelsFromBitmap(bitmap2, -blockSize,    0,             0, 0, blockSize, blockSize); });
        ExpectCOMException(E_INVALIDARG, [&] { bitmap1->CopyPixelsFromBitmap(bitmap2, 0,             -blockSize,    0, 0, blockSize, blockSize); });
        ExpectCOMException(E_INVALIDARG, [&] { bitmap1->CopyPixelsFromBitmap(bitmap2, 4 * blockSize, 0,             0, 0, blockSize, blockSize); });
        ExpectCOMException(E_INVALIDARG, [&] { bitmap1->CopyPixelsFromBitmap(bitmap2, 0,             4 * blockSize, 0, 0, blockSize, blockSize); });

        ExpectCOMException(E_INVALIDARG, [&] { bitmap1->CopyPixelsFromBitmap(bitmap2, 0, 0, -blockSize,    0,             blockSize, blockSize); });
        ExpectCOMException(E_INVALIDARG, [&] { bitmap1->CopyPixelsFromBitmap(bitmap2, 0, 0, 0,             -blockSize,    blockSize, blockSize); });
        ExpectCOMException(E_INVALIDARG, [&] { bitmap1->CopyPixelsFromBitmap(bitmap2, 0, 0, 2 * blockSize, 0,             blockSize, blockSize); });
        ExpectCOMException(E_INVALIDARG, [&] { bitmap1->CopyPixelsFromBitmap(bitmap2, 0, 0, 0,             2 * blockSize, blockSize, blockSize); });
    }

    TEST_METHOD(CanvasBitmap_CopyPixelsFromBitmap_SameDevice_B8G8R8A8)
    {
        auto device = ref new CanvasDevice();

        TestCopyPixelsFromBitmap<uint32_t>(device, device, DirectXPixelFormat::B8G8R8A8UIntNormalized, DirectXPixelFormat::R8G8B8A8UIntNormalized);
    }

    TEST_METHOD(CanvasBitmap_CopyPixelsFromBitmap_SameDevice_BC1)
    {
        auto device = ref new CanvasDevice();

        TestCopyPixelsFromBitmap<uint64_t>(device, device, DirectXPixelFormat::BC1UIntNormalized, DirectXPixelFormat::R8G8B8A8UIntNormalized, 4);
    }

    TEST_METHOD(CanvasBitmap_CopyPixelsFromBitmap_DifferentDevices_B8G8R8A8)
    {
        auto device1 = ref new CanvasDevice();
        auto device2 = ref new CanvasDevice();

        TestCopyPixelsFromBitmap<uint32_t>(device1, device2, DirectXPixelFormat::B8G8R8A8UIntNormalized, DirectXPixelFormat::R8G8B8A8UIntNormalized);
    }

    TEST_METHOD(CanvasBitmap_CopyPixelsFromBitmap_DifferentDevices_BC1)
    {
        auto device1 = ref new CanvasDevice();
        auto device2 = ref new CanvasDevice();

        TestCopyPixelsFromBitmap<uint64_t>(device1, device2, DirectXPixelFormat::BC1UIntNormalized, DirectXPixelFormat::R8G8B8A8UIntNormalized, 4);
    }

    TEST_METHOD(CanvasBitmap_CopyPixelsFromBitmap_DifferentD2DDevices_ButSameD3DDevice)
    {
        auto device1 = ref new CanvasDevice();
        auto device2 = CanvasDevice::CreateFromDirect3D11Device(device1);

        TestCopyPixelsFromBitmap<uint32_t>(device1, device2, DirectXPixelFormat::B8G8R8A8UIntNormalized, DirectXPixelFormat::R8G8B8A8UIntNormalized);
    }

    TEST_METHOD(CanvasBitmap_MaxSizeError)
    {
        auto device = ref new CanvasDevice();
        auto maxSize = device->MaximumBitmapSizeInPixels;
        auto tooBig = maxSize + 1;
        auto colors = ref new Platform::Array<Windows::UI::Color>(tooBig);
        wchar_t msg[256];

        swprintf_s(msg, L"Cannot create CanvasBitmap sized %d x 1; MaximumBitmapSizeInPixels for this device is %d.", tooBig, maxSize);
        ExpectCOMException(E_INVALIDARG, msg, [&] { CanvasBitmap::CreateFromColors(device, colors, tooBig, 1); });

        swprintf_s(msg, L"Cannot create CanvasBitmap sized 1 x %d; MaximumBitmapSizeInPixels for this device is %d.", tooBig, maxSize);
        ExpectCOMException(E_INVALIDARG, msg, [&] { CanvasBitmap::CreateFromColors(device, colors, 1, tooBig); });
    }
};
