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

const float c_dpiTolerance = 0.1f;
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

        auto async = CanvasBitmap::LoadAsync(canvasDevice, "ThisImageFileDoesNotExist.jpg");

        ExpectCOMExceptionWithHresult(HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND),
            [&]
            {
                WaitExecution(async);
            });            
            
        Uri^ uri = ref new Uri("ms-appx:///nope");

        async = CanvasBitmap::LoadAsync(canvasDevice, uri);

        ExpectCOMExceptionWithHresult(HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND),
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

            Uri^ uri = ref new Uri("ms-appx:///Assets/HighDpiGrid.png");
            if (i == 0)
            {
                bitmapAsync = CanvasBitmap::LoadAsync(canvasDevice, uri, CanvasAlphaBehavior::Ignore);
            }
            else
            {
                auto storageFile = WaitExecution(Windows::Storage::StorageFile::GetFileFromApplicationUriAsync(uri));
                auto stream = WaitExecution(storageFile->OpenReadAsync());

                bitmapAsync = CanvasBitmap::LoadAsync(canvasDevice, stream, CanvasAlphaBehavior::Ignore);
            }

            auto bitmap = WaitExecution(bitmapAsync);

            Assert::AreEqual(2.00001454f, bitmap->Size.Width);
            Assert::AreEqual(2.00001454f, bitmap->Size.Height);
            Assert::AreEqual(4.0f, bitmap->SizeInPixels.Width);
            Assert::AreEqual(4.0f, bitmap->SizeInPixels.Height);

            auto d2dBitmap = GetWrappedResource<ID2D1Bitmap1>(bitmap);

            VerifyDpiAndAlpha(d2dBitmap, 192, D2D1_ALPHA_MODE_IGNORE, c_dpiTolerance);
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
        VerifyDpiAndAlpha(d2dBitmap, 192, D2D1_ALPHA_MODE_PREMULTIPLIED, c_dpiTolerance);
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
            Assert::AreEqual(m_testImage.m_dpi, static_cast<float>(bitmapDecoder->DpiX), c_dpiTolerance);
            Assert::AreEqual(m_testImage.m_dpi, static_cast<float>(bitmapDecoder->DpiY), c_dpiTolerance);
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

    TEST_METHOD(CanvasBitmap_SaveToFileAndStreamAsync_UseSpecifiedEncoder)
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

        for (auto testCase : testCases)
        {
            InMemoryRandomAccessStream^ memoryStream = ref new InMemoryRandomAccessStream();

            WaitExecution(canvasBitmap->SaveToStreamAsync(memoryStream, testCase.CanvasFormat));

            auto bitmapDecoder = WaitExecution_RequiresWorkerThread(BitmapDecoder::CreateAsync(memoryStream));

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

    TEST_METHOD(CanvasBitmap_SaveToFileAsync_InvalidArguments)
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
            WaitExecution(f.GetRenderTarget(i)->SaveToFileAsync(savePath, CanvasBitmapFileFormat::Jpeg));

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

            Platform::Array<byte>^ byteData = f.GetRenderTarget(i)->GetBytes();
            Assert::AreEqual(sliceDimension * sliceDimension * 4, byteData->Length);
            for (unsigned int j = 0; j < byteData->Length; j += 4)
            {
                Assert::AreEqual(f.GetExpectedByte(i, 0), byteData[j + 0]);
                Assert::AreEqual(f.GetExpectedByte(i, 1), byteData[j + 1]);
                Assert::AreEqual(f.GetExpectedByte(i, 2), byteData[j + 2]);
                Assert::AreEqual(f.GetExpectedByte(i, 3), byteData[j + 3]);
            }

            Platform::Array<Color>^ colorData = f.GetRenderTarget(i)->GetColors();
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
        return canvasBitmap->GetBytes();
    }

    template<>
    Platform::Array<Color>^ GetDataFunction(CanvasBitmap^ canvasBitmap)
    {
        return canvasBitmap->GetColors();
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
        return canvasBitmap->GetBytes(left, top, width, height);
    }

    template<>
    Platform::Array<Color>^ GetDataFunction(CanvasBitmap^ canvasBitmap, int left, int top, int width, int height)
    {
        return canvasBitmap->GetColors(left, top, width, height);
    }

    void SetBitmapData(CanvasBitmap^ canvasBitmap, Platform::Array<byte>^ dataArray)
    {
        return canvasBitmap->SetBytes(dataArray);
    }

    void SetBitmapData(CanvasBitmap^ canvasBitmap, Platform::Array<Color>^ dataArray)
    {
        return canvasBitmap->SetColors(dataArray);
    }

    void SetBitmapData(CanvasBitmap^ canvasBitmap, Platform::Array<byte>^ dataArray, int left, int top, int width, int height)
    {
        return canvasBitmap->SetBytes(dataArray, left, top, width, height);
    }

    void SetBitmapData(CanvasBitmap^ canvasBitmap, Platform::Array<Color>^ dataArray, int left, int top, int width, int height)
    {
        return canvasBitmap->SetColors(dataArray, left, top, width, height);
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

    TEST_METHOD(CanvasBitmap_GetBytesAndSetBytes)
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
            CanvasAlphaBehavior::Premultiplied);
            
        VerifyBitmapGetData<byte>(canvasBitmap, width, imageData, 4);

        VerifyBitmapSetData<byte>(canvasBitmap, width, imageData, 4);
    }

    TEST_METHOD(CanvasBitmap_GetColorsAndSetColors)
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
            CanvasAlphaBehavior::Premultiplied);

        VerifyBitmapGetData<Color>(canvasBitmap, width, imageData, 1);

        VerifyBitmapSetData<Color>(canvasBitmap, width, imageData, 1);
    }

    TEST_METHOD(CanvasBitmap_GetAndSetBytesAndColors_InvalidArguments)
    {
        auto canvasBitmap = ref new CanvasRenderTarget(m_sharedDevice, 1, 1);

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
                    canvasBitmap->GetBytes(testCase.Left, testCase.Top, testCase.Width, testCase.Height);
                });

            Assert::ExpectException<Platform::InvalidArgumentException^>(
                [&]
                {
                    canvasBitmap->GetColors(testCase.Left, testCase.Top, testCase.Width, testCase.Height);
                });

            Assert::ExpectException<Platform::InvalidArgumentException^>(
                [&]
                {
                    canvasBitmap->SetBytes(byteArray, testCase.Left, testCase.Top, testCase.Width, testCase.Height);
                });

            Assert::ExpectException<Platform::InvalidArgumentException^>(
                [&]
                {
                    canvasBitmap->SetColors(colorArray, testCase.Left, testCase.Top, testCase.Width, testCase.Height);
                });

        }
    }

    TEST_METHOD(CanvasBitmap_SetBytesAndSetColors_ZeroArrayOnZeroSizedBitmap)
    {
        //
        // Because we have the convention that you cannot get a zero-sized array
        // as data, we also have the convention that you cannot set a zero array. 
        // Verify this corner case on a zero-sized bitmap.
        //
        auto canvasBitmap = ref new CanvasRenderTarget(m_sharedDevice, 0, 0);

        Platform::Array<byte>^ zeroSizedByteArray = ref new Platform::Array<byte>(0);
        Assert::ExpectException<Platform::InvalidArgumentException^>(
            [&]
            {
                canvasBitmap->SetBytes(zeroSizedByteArray);
            });
        Assert::ExpectException<Platform::InvalidArgumentException^>(
            [&]
            {
                canvasBitmap->SetBytes(zeroSizedByteArray, 0, 0, 0, 0);
            });

        Platform::Array<Color>^ zeroSizedColorArray = ref new Platform::Array<Color>(0);
        Assert::ExpectException<Platform::InvalidArgumentException^>(
            [&]
            {
                canvasBitmap->SetColors(zeroSizedColorArray);
            });
        Assert::ExpectException<Platform::InvalidArgumentException^>(
            [&]
            {
                canvasBitmap->SetColors(zeroSizedColorArray, 0, 0, 0, 0);
            });
    }

    TEST_METHOD(CanvasBitmap_SetBytesAndSetColors_InvalidArguments)
    {
        const int bitmapWidth = 2;
        const int bitmapHeight = 3;
        const int expectedByteSizeOfBitmap = bitmapWidth * bitmapHeight * 4;
        const int expectedColorCountOfBitmap = bitmapWidth * bitmapHeight;

        auto canvasBitmap = ref new CanvasRenderTarget(m_sharedDevice, static_cast<float>(bitmapWidth), static_cast<float>(bitmapHeight));
        
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
                    canvasBitmap->SetBytes(array);
                });

            Assert::ExpectException<Platform::InvalidArgumentException^>(
                [&]
                {
                    Platform::Array<Color>^ array = ref new Platform::Array<Color>(expectedColorCountOfBitmap + testCase);
                    canvasBitmap->SetColors(array);
                });
            Assert::ExpectException<Platform::InvalidArgumentException^>(
                [&]
                {
                    Platform::Array<byte>^ array = ref new Platform::Array<byte>(expectedByteSizeOfSubrectangle + testCase);
                    canvasBitmap->SetBytes(array, subrectangle.Left, subrectangle.Top, subrectangle.Width, subrectangle.Height);
                });
            Assert::ExpectException<Platform::InvalidArgumentException^>(
                [&]
                {
                    Platform::Array<Color>^ array = ref new Platform::Array<Color>(expectedColorCountOfSubrectangle + testCase);
                    canvasBitmap->SetColors(array, subrectangle.Left, subrectangle.Top, subrectangle.Width, subrectangle.Height);
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

    TEST_METHOD(CanavasBitmap_CopyFromBitmap_MistmachingDevices)
    {
        //
        // This verifies that bitmaps used with CopyFromBitmap must belong to
        // the same device. This is validated by D2D. Still, Win2D should
        // not do anything to interfere with this behavior or cause
        // ungraceful errors.
        //
        auto canvasDevice0 = ref new CanvasDevice();
        auto bitmap0 = ref new CanvasRenderTarget(canvasDevice0, 1, 1);

        auto canvasDevice1 = ref new CanvasDevice();
        auto bitmap1 = ref new CanvasRenderTarget(canvasDevice1, 1, 1);

        Assert::ExpectException<Platform::COMException^>([&] { bitmap0->CopyFromBitmap(bitmap1); });        
    }

    TEST_METHOD(CanavasBitmap_CopyFromBitmap_SameBitmap)
    {
        //
        // This is validated by D2D. Ensure the expected error occurs.
        //
        auto bitmap = ref new CanvasRenderTarget(m_sharedDevice, 1, 1);

        Assert::ExpectException<Platform::COMException^>([&] { bitmap->CopyFromBitmap(bitmap); });
    }

    TEST_METHOD(CanvasBitmap_SaveToStreamAsync_InvalidArguments)
    {
        auto canvasBitmap = ref new CanvasRenderTarget(m_sharedDevice, 1, 1);

        auto validStream = ref new InMemoryRandomAccessStream();

        Assert::ExpectException<Platform::InvalidArgumentException^>(
            [&]
            {
                canvasBitmap->SaveToStreamAsync(nullptr, CanvasBitmapFileFormat::Bmp);
            });

        Assert::ExpectException<Platform::InvalidArgumentException^>(
            [&]
            {
                WaitExecution(canvasBitmap->SaveToStreamAsync(validStream, CanvasBitmapFileFormat::Auto));
            });

        Assert::ExpectException<Platform::InvalidArgumentException^>(
            [&]
            {
                WaitExecution(canvasBitmap->SaveToStreamAsync(validStream, static_cast<CanvasBitmapFileFormat>(999)));
            });

        Assert::ExpectException<Platform::InvalidArgumentException^>(
            [&]
            {
                WaitExecution(canvasBitmap->SaveToStreamAsync(validStream, CanvasBitmapFileFormat::Jpeg, -FLT_EPSILON));
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

        asyncLoad = CanvasBitmap::LoadAsync(m_sharedDevice, streamWithNoRead, CanvasAlphaBehavior::Ignore);
        Assert::ExpectException<Platform::NotImplementedException^>(
            [&]
            {
                WaitExecution(asyncLoad);
            });

        // Ensure an unwritable stream causes SaveToStreamAsync to fail.
        StreamWithRestrictions^ streamWithNoWrite = ref new StreamWithRestrictions(true, false);
        auto bitmap = ref new CanvasRenderTarget(m_sharedDevice, 1, 1);
        auto asyncSave = bitmap->SaveToStreamAsync(streamWithNoWrite, CanvasBitmapFileFormat::Bmp);        
        Assert::ExpectException<Platform::NotImplementedException^>(
            [&]
            {
                WaitExecution(asyncSave);
            });

        asyncSave = bitmap->SaveToStreamAsync(streamWithNoWrite, CanvasBitmapFileFormat::Bmp, 0.6f);        
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
                throw ref new Platform::COMException(INET_E_DOWNLOAD_FAILURE);
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
        ExpectCOMExceptionWithHresult(INET_E_DOWNLOAD_FAILURE,
            [&]
            {
                WaitExecution(asyncLoad);
            });

        asyncLoad = CanvasBitmap::LoadAsync(m_sharedDevice, unreliableStream, CanvasAlphaBehavior::Ignore);
        ExpectCOMExceptionWithHresult(INET_E_DOWNLOAD_FAILURE, 
            [&]
            {
                WaitExecution(asyncLoad);
            });

        auto bitmap = ref new CanvasRenderTarget(m_sharedDevice, 1, 1);

        auto asyncSave = bitmap->SaveToStreamAsync(unreliableStream, CanvasBitmapFileFormat::Bmp);
        ExpectCOMExceptionWithHresult(INET_E_CONNECTION_TIMEOUT,
            [&]
            {
                WaitExecution(asyncSave);
            });
        
        asyncSave = bitmap->SaveToStreamAsync(unreliableStream, CanvasBitmapFileFormat::Bmp, 0.5f);
        ExpectCOMExceptionWithHresult(INET_E_CONNECTION_TIMEOUT, 
            [&]
            {
                WaitExecution(asyncSave);
            });

    }
};
