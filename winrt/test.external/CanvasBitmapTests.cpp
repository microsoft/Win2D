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
using namespace Windows::Storage::Streams;
using Platform::String;

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
        WIN32_FILE_ATTRIBUTE_DATA fileInfo;
        Assert::AreNotEqual(GetFileAttributesEx(testImageFileName->Data(), GetFileExInfoStandard, &fileInfo), 0);
        
        CanvasDevice^ canvasDevice = ref new CanvasDevice();

        auto bitmapJpeg = WaitExecution(CanvasBitmap::LoadAsync(canvasDevice, testImageFileName));
        Assert::AreEqual((uint32_t)testImageWidth, bitmapJpeg->SizeInPixels.Width);
        Assert::AreEqual((uint32_t)testImageHeight, bitmapJpeg->SizeInPixels.Height);
        Assert::AreEqual((float)testImageWidth, bitmapJpeg->Size.Width);
        Assert::AreEqual((float)testImageHeight, bitmapJpeg->Size.Height);
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
                bitmapAsync = CanvasBitmap::LoadAsync(canvasDevice, uri, CanvasAlphaMode::Ignore);
                highDpiAsync = CanvasBitmap::LoadAsync(canvasDevice, uri, CanvasAlphaMode::Ignore, highDpi);
            }
            else
            {
                auto storageFile = WaitExecution(Windows::Storage::StorageFile::GetFileFromApplicationUriAsync(uri));
                auto stream = WaitExecution(storageFile->OpenReadAsync());

                bitmapAsync = CanvasBitmap::LoadAsync(canvasDevice, stream, CanvasAlphaMode::Ignore);

                storageFile = WaitExecution(Windows::Storage::StorageFile::GetFileFromApplicationUriAsync(uri));
                stream = WaitExecution(storageFile->OpenReadAsync());

                highDpiAsync = CanvasBitmap::LoadAsync(canvasDevice, stream, CanvasAlphaMode::Ignore, highDpi);
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
                CanvasAlphaMode::Straight,
                99.0f);
            auto d2dBitmap = GetWrappedResource<ID2D1Bitmap1>(bitmap);
            auto size = bitmap->SizeInPixels;
            Assert::AreEqual(static_cast<uint32_t>(testCase.width), size.Width);
            Assert::AreEqual(static_cast<uint32_t>(testCase.height), size.Height);
            VerifyDpiAndAlpha(d2dBitmap, 99.0f, D2D1_ALPHA_MODE_STRAIGHT);

            bitmap = CanvasBitmap::CreateFromColors(
                canvasDevice,
                colorArray1x1,
                testCase.width,
                testCase.height,
                CanvasAlphaMode::Ignore,
                25.0f);
            d2dBitmap = GetWrappedResource<ID2D1Bitmap1>(bitmap);
            size = bitmap->SizeInPixels;
            Assert::AreEqual(static_cast<uint32_t>(testCase.width), size.Width);
            Assert::AreEqual(static_cast<uint32_t>(testCase.height), size.Height);
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
                        CanvasAlphaMode::Premultiplied,
                        DEFAULT_DPI);

                    bitmap = CanvasBitmap::CreateFromColors(
                        canvasDevice,
                        colorArray1x1,
                        testCase.width,
                        testCase.height,
                        CanvasAlphaMode::Premultiplied,
                        DEFAULT_DPI);
                });
        }
    }

    TEST_METHOD(CanvasBitmap_CreateFromBytes_AcceptsZeroSizedArray)
    {
        auto device = ref new CanvasDevice();
        auto anyFormat = DirectXPixelFormat::B8G8R8A8UIntNormalized;
        auto anyAlphaMode = CanvasAlphaMode::Premultiplied;

        auto zeroSizedArray = ref new Platform::Array<BYTE>(0);
        auto bitmap = CanvasBitmap::CreateFromBytes(device, zeroSizedArray, 0, 0, anyFormat, anyAlphaMode);
        
        auto size = bitmap->SizeInPixels;
        Assert::AreEqual(0u, size.Width);
        Assert::AreEqual(0u, size.Height);
    }

    TEST_METHOD(CanvasBitmap_CreateFromColors_AcceptsZeroSizedArray)
    {
        auto device = ref new CanvasDevice();
        auto anyAlphaMode = CanvasAlphaMode::Premultiplied;

        auto zeroSizedArray = ref new Platform::Array<Color>(0);
        auto bitmap = CanvasBitmap::CreateFromColors(device, zeroSizedArray, 0, 0, anyAlphaMode);
        
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
                CanvasBitmap::CreateFromBytes(device, oneElementArray, 256, 256, anyFormat, anyAlphaMode);
            });
    }

    TEST_METHOD(CanvasBitmap_CreateFromColors_FailsIfArrayTooSmall)
    {
        auto device = ref new CanvasDevice();
        auto anyAlphaMode = CanvasAlphaMode::Premultiplied;

        auto oneElementArray = ref new Platform::Array<Color>(1);

        Assert::ExpectException<Platform::InvalidArgumentException^>(
            [&]
            {
                CanvasBitmap::CreateFromColors(device, oneElementArray, 256, 256, anyAlphaMode);
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
        auto canvasBitmap = WaitExecution(CanvasBitmap::LoadAsync(m_sharedDevice, testImageFileName));

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
        auto canvasBitmap = WaitExecution(CanvasBitmap::LoadAsync(m_sharedDevice, testImageFileName));

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
        auto canvasBitmap = WaitExecution(CanvasBitmap::LoadAsync(m_sharedDevice, testImageFileName));

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

        Color GetExpectedColor(int sliceIndex)
        {
            assert(sliceIndex >= 0 && sliceIndex < c_subresourceSliceCount);
            static const Color colors[c_subresourceSliceCount] = { Colors::Red, Colors::Green, Colors::Blue };
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
        SubresourceTestFixture f(m_sharedDevice);

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

            Platform::Array<Color>^ colorData = f.GetRenderTarget(i)->GetPixelColors();
            Assert::AreEqual(sliceDimension * sliceDimension, colorData->Length);
            for (Color c : colorData)
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
    Color ReferenceColorFromIndex(int i)
    {
        Color c;
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
    Platform::Array<Color>^ GetDataFunction(CanvasBitmap^ canvasBitmap)
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
    Platform::Array<Color>^ GetDataFunction(CanvasBitmap^ canvasBitmap, int left, int top, int width, int height)
    {
        return canvasBitmap->GetPixelColors(left, top, width, height);
    }

    void SetBitmapData(CanvasBitmap^ canvasBitmap, Platform::Array<byte>^ dataArray)
    {
        return canvasBitmap->SetPixelBytes(dataArray);
    }

    void SetBitmapData(CanvasBitmap^ canvasBitmap, Platform::Array<Color>^ dataArray)
    {
        return canvasBitmap->SetPixelColors(dataArray);
    }

    void SetBitmapData(CanvasBitmap^ canvasBitmap, Platform::Array<byte>^ dataArray, int left, int top, int width, int height)
    {
        return canvasBitmap->SetPixelBytes(dataArray, left, top, width, height);
    }

    void SetBitmapData(CanvasBitmap^ canvasBitmap, Platform::Array<Color>^ dataArray, int left, int top, int width, int height)
    {
        return canvasBitmap->SetPixelColors(dataArray, left, top, width, height);
    }

    void IncrementColor(byte& b)
    {
        b++;
    }

    void IncrementColor(Color& c)
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
        Platform::Array<Color>^ imageData = ref new Platform::Array<Color>(totalSize);
        for (int i = 0; i < totalSize; i++)
        {
            imageData[i] = ReferenceColorFromIndex<Color>(i);
        }

        auto canvasBitmap = CanvasBitmap::CreateFromColors(
            m_sharedDevice,
            imageData,
            width,
            height,
            CanvasAlphaMode::Premultiplied);

        VerifyBitmapGetData<Color>(canvasBitmap, width, imageData, 1);

        VerifyBitmapSetData<Color>(canvasBitmap, width, imageData, 1);
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
        Platform::Array<Color>^ colorArray = ref new Platform::Array<Color>(1);

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
        Platform::Array<Color>^ colors = ref new Platform::Array<Color>(1);

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
        auto rt = ref new CanvasRenderTarget(m_sharedDevice, 1, 1, DirectXPixelFormat::R8G8B8A8UIntNormalized, CanvasAlphaMode::Premultiplied, DEFAULT_DPI);
        Platform::Array<Color>^ colors = ref new Platform::Array<Color>(1);

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

        Platform::Array<Color>^ zeroSizedColorArray = ref new Platform::Array<Color>(0);
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

        int testCases[] = {-1, +1 }; // Tests array being too small and too large.

        for (int testCase : testCases)
        {
            Assert::ExpectException<Platform::InvalidArgumentException^>(
                [&]
                {
                    Platform::Array<byte>^ array = ref new Platform::Array<byte>(expectedByteSizeOfBitmap + testCase);
                    canvasBitmap->SetPixelBytes(array);
                });

            Assert::ExpectException<Platform::InvalidArgumentException^>(
                [&]
                {
                    Platform::Array<Color>^ array = ref new Platform::Array<Color>(expectedColorCountOfBitmap + testCase);
                    canvasBitmap->SetPixelColors(array);
                });
            Assert::ExpectException<Platform::InvalidArgumentException^>(
                [&]
                {
                    Platform::Array<byte>^ array = ref new Platform::Array<byte>(expectedByteSizeOfSubrectangle + testCase);
                    canvasBitmap->SetPixelBytes(array, subrectangle.Left, subrectangle.Top, subrectangle.Width, subrectangle.Height);
                });
            Assert::ExpectException<Platform::InvalidArgumentException^>(
                [&]
                {
                    Platform::Array<Color>^ array = ref new Platform::Array<Color>(expectedColorCountOfSubrectangle + testCase);
                    canvasBitmap->SetPixelColors(array, subrectangle.Left, subrectangle.Top, subrectangle.Width, subrectangle.Height);
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

            VerifySubresourceSlice<Color>(f.GetRenderTarget(i), sliceDimension, 1);
        }
    }

    TEST_METHOD(CanavasBitmap_CopyPixelsFromBitmap_MistmachingDevices)
    {
        //
        // This verifies that bitmaps used with CopyPixelsFromBitmap must belong to
        // the same device. This is validated by D2D. Still, Win2D should
        // not do anything to interfere with this behavior or cause
        // ungraceful errors.
        //
        auto canvasDevice0 = ref new CanvasDevice();
        auto bitmap0 = ref new CanvasRenderTarget(canvasDevice0, 1, 1, DEFAULT_DPI);

        auto canvasDevice1 = ref new CanvasDevice();
        auto bitmap1 = ref new CanvasRenderTarget(canvasDevice1, 1, 1, DEFAULT_DPI);

        Assert::ExpectException<Platform::COMException^>([&] { bitmap0->CopyPixelsFromBitmap(bitmap1); });        
    }

    TEST_METHOD(CanavasBitmap_CopyPixelsFromBitmap_SameBitmap)
    {
        //
        // This is validated by D2D. Ensure the expected error occurs.
        //
        auto bitmap = ref new CanvasRenderTarget(m_sharedDevice, 1, 1, DEFAULT_DPI);

        Assert::ExpectException<Platform::COMException^>([&] { bitmap->CopyPixelsFromBitmap(bitmap); });
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
            return nullptr;
        }
    };

    TEST_METHOD(CanvasBitmap_InvalidStreams)
    {
        // Ensure an unreadable stream causes LoadAsync to fail.
        StreamWithRestrictions^ streamWithNoRead = ref new StreamWithRestrictions(false, true); 
               
        auto asyncLoad = CanvasBitmap::LoadAsync(m_sharedDevice, streamWithNoRead);
        Assert::ExpectException<Platform::NotImplementedException^>(
            [&]
            {
                WaitExecution(asyncLoad);
            });

        asyncLoad = CanvasBitmap::LoadAsync(m_sharedDevice, streamWithNoRead, CanvasAlphaMode::Ignore);
        Assert::ExpectException<Platform::NotImplementedException^>(
            [&]
            {
                WaitExecution(asyncLoad);
            });

        // Ensure an unwritable stream causes SaveToStreamAsync to fail.
        StreamWithRestrictions^ streamWithNoWrite = ref new StreamWithRestrictions(true, false);
        auto bitmap = ref new CanvasRenderTarget(m_sharedDevice, 1, 1, DEFAULT_DPI);
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
            return nullptr;
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

        asyncLoad = CanvasBitmap::LoadAsync(m_sharedDevice, unreliableStream, CanvasAlphaMode::Ignore);
        ExpectCOMException(INET_E_DOWNLOAD_FAILURE, 
            [&]
            {
                WaitExecution(asyncLoad);
            });

        auto bitmap = ref new CanvasRenderTarget(m_sharedDevice, 1, 1, DEFAULT_DPI);

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
};
