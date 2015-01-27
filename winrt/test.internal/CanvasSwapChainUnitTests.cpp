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
#include "CanvasSwapChain.h"
#include "MockDXGIAdapter.h"
#include "MockDXGISwapChain.h"
#include "MockDXGIFactory.h"
#include "TestDeviceResourceCreationAdapter.h"

TEST_CLASS(CanvasSwapChainUnitTests)
{
    struct StubDeviceFixture
    {
        std::shared_ptr<CanvasSwapChainManager> m_swapChainManager;
        ComPtr<StubCanvasDevice> m_canvasDevice;

        StubDeviceFixture()
        {
            m_canvasDevice = Make<StubCanvasDevice>();
            m_swapChainManager = std::make_shared<CanvasSwapChainManager>();
            
            m_canvasDevice->CreateSwapChainMethod.AllowAnyCall([=](int32_t, int32_t, DirectXPixelFormat, int32_t, CanvasAlphaMode)
            {
                auto dxgiSwapChain = Make<MockDxgiSwapChain>();
                dxgiSwapChain->SetMatrixTransformMethod.SetExpectedCalls(1);
                return dxgiSwapChain;
            });
        }

        ComPtr<CanvasSwapChain> CreateTestSwapChain(float dpi = DEFAULT_DPI)
        {
            return m_swapChainManager->Create(
                m_canvasDevice.Get(),
                1.0f,
                1.0f,
                PIXEL_FORMAT(B8G8R8A8UIntNormalized),
                2,
                CanvasAlphaMode::Premultiplied,
                dpi);
        }
    };

    TEST_METHOD_EX(CanvasSwapChain_Creation)
    {
        StubDeviceFixture f;

        const int dpiScale = 2;

        f.m_canvasDevice->CreateSwapChainMethod.SetExpectedCalls(1, 
            [=](int32_t widthInPixels, int32_t heightInPixels, DirectXPixelFormat format, int32_t bufferCount, CanvasAlphaMode alphaMode)
            {
                Assert::AreEqual(23 * dpiScale, widthInPixels);
                Assert::AreEqual(45 * dpiScale, heightInPixels);
                Assert::AreEqual(PIXEL_FORMAT(B8G8R8A8UIntNormalizedSrgb), format);
                Assert::AreEqual(4, bufferCount);
                Assert::AreEqual(CanvasAlphaMode::Ignore, alphaMode);

                auto dxgiSwapChain = Make<MockDxgiSwapChain>();

                dxgiSwapChain->SetMatrixTransformMethod.SetExpectedCalls(1, [=](DXGI_MATRIX_3X2_F const* matrix)
                {
                    Assert::AreEqual(1.0f / dpiScale, matrix->_11);
                    Assert::AreEqual(0.0f, matrix->_12);
                    Assert::AreEqual(0.0f, matrix->_21);
                    Assert::AreEqual(1.0f / dpiScale, matrix->_22);
                    Assert::AreEqual(0.0f, matrix->_31);
                    Assert::AreEqual(0.0f, matrix->_32);

                    return S_OK;
                });

                return dxgiSwapChain;
            });

        auto swapChain = f.m_swapChainManager->Create(
            f.m_canvasDevice.Get(),
            23.0f,
            45.0f,
            PIXEL_FORMAT(B8G8R8A8UIntNormalizedSrgb),
            4,
            CanvasAlphaMode::Ignore,
            DEFAULT_DPI * dpiScale);

        float dpi = 0;
        ThrowIfFailed(swapChain->get_Dpi(&dpi));
        Assert::AreEqual(DEFAULT_DPI * dpiScale, dpi);
    }

    struct FullDeviceFixture
    {
    public:
        ComPtr<CanvasDevice> m_canvasDevice;
        std::shared_ptr<CanvasSwapChainManager> m_swapChainManager;

        FullDeviceFixture()
        {
            // Validating certain parameters requires an actual CanvasDevice, not a 
            // mock object.

            auto mockDxgiDevice = Make<MockDxgiDevice>();
            mockDxgiDevice->MockGetParent = 
                [&](IID const& iid, void** out)
                {
                    Assert::AreEqual(__uuidof(IDXGIAdapter2), iid);
                    auto mockAdapter = Make<MockDxgiAdapter>();

                    mockAdapter->GetParentMethod.SetExpectedCalls(1, 
                    [&](IID const& iid, void** out)
                    {
                        Assert::AreEqual(__uuidof(IDXGIFactory2), iid);
                        auto mockFactory = Make<MockDxgiFactory>();
                        mockFactory.CopyTo(reinterpret_cast<IDXGIFactory2**>(out));

                        return S_OK;

                    });

                    mockAdapter.CopyTo(reinterpret_cast<IDXGIAdapter2**>(out));

                    return S_OK;
                };

            auto d2dDevice = Make<MockD2DDevice>(mockDxgiDevice.Get());

            auto resourceCreationAdapter = std::make_shared<TestDeviceResourceCreationAdapter>();
            auto deviceManager = std::make_shared<CanvasDeviceManager>(resourceCreationAdapter);

            m_canvasDevice = deviceManager->GetOrCreate(d2dDevice.Get());

            m_swapChainManager = std::make_shared<CanvasSwapChainManager>();
        }

        ComPtr<CanvasSwapChain> CreateTestSwapChain(float width, float height, int32_t bufferCount)
        {
            return m_swapChainManager->Create(
                m_canvasDevice.Get(),
                width,
                height,
                PIXEL_FORMAT(B8G8R8A8UIntNormalized),
                bufferCount,
                CanvasAlphaMode::Premultiplied,
                DEFAULT_DPI);
        }
    };

    TEST_METHOD_EX(CanvasSwapChain_Creation_InvalidParameters)
    {
        FullDeviceFixture f0;
        ExpectHResultException(E_INVALIDARG, [&]  {  f0.CreateTestSwapChain(-1, 100, 2); });

        FullDeviceFixture f1;
        ExpectHResultException(E_INVALIDARG, [&]  {  f1.CreateTestSwapChain(123, -3, 2); });

        FullDeviceFixture f2;
        ExpectHResultException(E_INVALIDARG, [&]  {  f2.CreateTestSwapChain(100, 100, -3); });
    }

    TEST_METHOD_EX(CanvasSwapChain_Closed)
    {
        StubDeviceFixture f;

        auto canvasSwapChain = f.CreateTestSwapChain();

        Assert::AreEqual(S_OK, canvasSwapChain->Close());

        ComPtr<ICanvasDrawingSession> drawingSession;
        Assert::AreEqual(RO_E_CLOSED, canvasSwapChain->CreateDrawingSession(Color{}, &drawingSession));

        Size s;
        BitmapSize bs;
        Assert::AreEqual(RO_E_CLOSED, canvasSwapChain->get_Size(&s));
        Assert::AreEqual(RO_E_CLOSED, canvasSwapChain->get_SizeInPixels(&bs));

        DirectXPixelFormat pixelFormat;
        Assert::AreEqual(RO_E_CLOSED, canvasSwapChain->get_Format(&pixelFormat));

        int32_t i;
        Assert::AreEqual(RO_E_CLOSED, canvasSwapChain->get_BufferCount(&i));

        CanvasAlphaMode alphaMode;
        Assert::AreEqual(RO_E_CLOSED, canvasSwapChain->get_AlphaMode(&alphaMode));

        Assert::AreEqual(RO_E_CLOSED, canvasSwapChain->get_SourceSize(&s));
        Assert::AreEqual(RO_E_CLOSED, canvasSwapChain->put_SourceSize(Size{ 1, 1 }));

        Matrix3x2 m;
        Assert::AreEqual(RO_E_CLOSED, canvasSwapChain->get_TransformMatrix(&m));
        Assert::AreEqual(RO_E_CLOSED, canvasSwapChain->put_TransformMatrix(Matrix3x2{}));

        CanvasSwapChainRotation r;
        Assert::AreEqual(RO_E_CLOSED, canvasSwapChain->get_Rotation(&r));
        Assert::AreEqual(RO_E_CLOSED, canvasSwapChain->put_Rotation(CanvasSwapChainRotation::None));

        Assert::AreEqual(RO_E_CLOSED, canvasSwapChain->ResizeBuffersWithSize(2, 2));
        Assert::AreEqual(RO_E_CLOSED, canvasSwapChain->ResizeBuffersWithAllOptions(2, 2, PIXEL_FORMAT(B8G8R8A8UIntNormalized), 2));

        ComPtr<ICanvasDevice> device;
        Assert::AreEqual(RO_E_CLOSED, canvasSwapChain->get_Device(&device));
    }


    TEST_METHOD_EX(CanvasSwapChain_NullArgs)
    {
        StubDeviceFixture f;

        auto canvasSwapChain = f.CreateTestSwapChain();

        Assert::AreEqual(E_INVALIDARG, canvasSwapChain->CreateDrawingSession(Color{}, nullptr));
        Assert::AreEqual(E_INVALIDARG, canvasSwapChain->get_Size(nullptr));
        Assert::AreEqual(E_INVALIDARG, canvasSwapChain->get_SizeInPixels(nullptr));
        Assert::AreEqual(E_INVALIDARG, canvasSwapChain->get_Dpi(nullptr));
        Assert::AreEqual(E_INVALIDARG, canvasSwapChain->get_Format(nullptr));
        Assert::AreEqual(E_INVALIDARG, canvasSwapChain->get_BufferCount(nullptr));
        Assert::AreEqual(E_INVALIDARG, canvasSwapChain->get_AlphaMode(nullptr));
        Assert::AreEqual(E_INVALIDARG, canvasSwapChain->get_Device(nullptr));
    }

    void ResetForPropertyTest(ComPtr<MockDxgiSwapChain>& swapChain)
    {
        swapChain->GetDesc1Method.SetExpectedCalls(1,
            [=](DXGI_SWAP_CHAIN_DESC1* desc)
            {
                desc->Width = 123;
                desc->Height = 456;
                desc->Format = DXGI_FORMAT_R16G16B16A16_UNORM;
                desc->BufferCount = 5;
                desc->AlphaMode = DXGI_ALPHA_MODE_IGNORE;
                return S_OK;
            });
    }

    TEST_METHOD_EX(CanvasSwapChain_Properties)
    {
        StubDeviceFixture f;

        const float testDpi = 144;

        auto swapChain = Make<MockDxgiSwapChain>();

        swapChain->SetMatrixTransformMethod.SetExpectedCalls(1);

        f.m_canvasDevice->CreateSwapChainMethod.AllowAnyCall([=](int32_t, int32_t, DirectXPixelFormat, int32_t, CanvasAlphaMode)
        {
            return swapChain;
        });

        auto canvasSwapChain = f.CreateTestSwapChain(testDpi);

        int32_t i;
        Size size;
        BitmapSize bitmapSize;
        float dpi;
        DirectXPixelFormat pixelFormat;
        CanvasAlphaMode alphaMode;
        ComPtr<ICanvasDevice> device;

        ResetForPropertyTest(swapChain);
        ThrowIfFailed(canvasSwapChain->get_Size(&size));
        Assert::AreEqual(123.0f * DEFAULT_DPI / testDpi, size.Width);
        Assert::AreEqual(456.0f * DEFAULT_DPI / testDpi, size.Height);

        ResetForPropertyTest(swapChain);
        ThrowIfFailed(canvasSwapChain->get_SizeInPixels(&bitmapSize));
        Assert::AreEqual(123u, bitmapSize.Width);
        Assert::AreEqual(456u, bitmapSize.Height);
        
        swapChain->GetDesc1Method.SetExpectedCalls(0);
        ThrowIfFailed(canvasSwapChain->get_Dpi(&dpi));
        Assert::AreEqual(testDpi, dpi);

        ResetForPropertyTest(swapChain);
        ThrowIfFailed(canvasSwapChain->get_Format(&pixelFormat));
        Assert::AreEqual(PIXEL_FORMAT(R16G16B16A16UIntNormalized), pixelFormat);

        ResetForPropertyTest(swapChain);
        ThrowIfFailed(canvasSwapChain->get_BufferCount(&i));
        Assert::AreEqual(5, i);

        ResetForPropertyTest(swapChain);
        ThrowIfFailed(canvasSwapChain->get_AlphaMode(&alphaMode));
        Assert::AreEqual(CanvasAlphaMode::Ignore, alphaMode);

        ThrowIfFailed(canvasSwapChain->get_Device(&device));
        Assert::AreEqual(device.Get(), static_cast<ICanvasDevice*>(f.m_canvasDevice.Get()));
    }

    TEST_METHOD_EX(CanvasSwapChain_Rotation)
    {
        StubDeviceFixture f;

        auto swapChain = Make<MockDxgiSwapChain>();
        swapChain->SetMatrixTransformMethod.SetExpectedCalls(1);

        f.m_canvasDevice->CreateSwapChainMethod.AllowAnyCall([=](int32_t, int32_t, DirectXPixelFormat, int32_t, CanvasAlphaMode)
        {
            return swapChain;
        });

        auto canvasSwapChain = f.CreateTestSwapChain();

        swapChain->GetRotationMethod.SetExpectedCalls(1, [](DXGI_MODE_ROTATION* value)
        {
            *value = DXGI_MODE_ROTATION_ROTATE270;
            return S_OK;
        });

        CanvasSwapChainRotation rotation = CanvasSwapChainRotation::None;
        canvasSwapChain->get_Rotation(&rotation);
        Assert::AreEqual(CanvasSwapChainRotation::Rotate270, rotation);

        swapChain->SetRotationMethod.SetExpectedCalls(1, [](DXGI_MODE_ROTATION value)
        {
            Assert::AreEqual<uint32_t>(DXGI_MODE_ROTATION_ROTATE180, value);
            return S_OK;
        });

        canvasSwapChain->put_Rotation(CanvasSwapChainRotation::Rotate180);
    }

    TEST_METHOD_EX(CanvasSwapChain_TransformMatrix)
    {
        StubDeviceFixture f;

        auto swapChain = Make<MockDxgiSwapChain>();
        swapChain->SetMatrixTransformMethod.SetExpectedCalls(1);

        f.m_canvasDevice->CreateSwapChainMethod.AllowAnyCall([=](int32_t, int32_t, DirectXPixelFormat, int32_t, CanvasAlphaMode)
        {
            return swapChain;
        });

        auto canvasSwapChain = f.CreateTestSwapChain();

        swapChain->GetMatrixTransformMethod.SetExpectedCalls(1, [](DXGI_MATRIX_3X2_F* value)
        {
            *value = DXGI_MATRIX_3X2_F{ 2, 0, 0, 3, 4, 5 };
            return S_OK;
        });

        Matrix3x2 matrix{};
        canvasSwapChain->get_TransformMatrix(&matrix);
        Assert::AreEqual(Matrix3x2{ 2, 0, 0, 3, 4, 5 }, matrix);

        swapChain->SetMatrixTransformMethod.SetExpectedCalls(1, [](DXGI_MATRIX_3X2_F const* value)
        {
            Assert::AreEqual(Matrix3x2{ 6, 0, 0, 7, 8, 9 }, *ReinterpretAs<Matrix3x2 const*>(value));
            return S_OK;
        });

        canvasSwapChain->put_TransformMatrix(Matrix3x2{ 6, 0, 0, 7, 8, 9 });
    }

    TEST_METHOD_EX(CanvasSwapChain_TransformMatrix_AdjustsForDpi)
    {
        const float dpiScale = 1.5f;

        StubDeviceFixture f;

        auto swapChain = Make<MockDxgiSwapChain>();
        swapChain->SetMatrixTransformMethod.SetExpectedCalls(1);

        f.m_canvasDevice->CreateSwapChainMethod.AllowAnyCall([=](int32_t, int32_t, DirectXPixelFormat, int32_t, CanvasAlphaMode)
        {
            return swapChain;
        });

        auto canvasSwapChain = f.CreateTestSwapChain(DEFAULT_DPI * dpiScale);

        swapChain->GetMatrixTransformMethod.SetExpectedCalls(1, [](DXGI_MATRIX_3X2_F* value)
        {
            *value = DXGI_MATRIX_3X2_F{ 2, 0, 0, 3, 4, 5 };
            return S_OK;
        });

        Matrix3x2 matrix{};
        canvasSwapChain->get_TransformMatrix(&matrix);
        Assert::AreEqual(Matrix3x2{ 2 * dpiScale, 0, 0, 3 * dpiScale, 4 * dpiScale, 5 * dpiScale }, matrix);

        swapChain->SetMatrixTransformMethod.SetExpectedCalls(1, [&](DXGI_MATRIX_3X2_F const* value)
        {
            const float epsilon = 0.000001f;

            Assert::AreEqual(6 / dpiScale, value->_11, epsilon);
            Assert::AreEqual(0 / dpiScale, value->_12, epsilon);
            Assert::AreEqual(0 / dpiScale, value->_21, epsilon);
            Assert::AreEqual(7 / dpiScale, value->_22, epsilon);
            Assert::AreEqual(8 / dpiScale, value->_31, epsilon);
            Assert::AreEqual(9 / dpiScale, value->_32, epsilon);

            return S_OK;
        });

        canvasSwapChain->put_TransformMatrix(Matrix3x2{ 6, 0, 0, 7, 8, 9 });
    }

    TEST_METHOD_EX(CanvasSwapChain_SourceSize)
    {
        StubDeviceFixture f;

        auto swapChain = Make<MockDxgiSwapChain>();
        swapChain->SetMatrixTransformMethod.SetExpectedCalls(1);

        f.m_canvasDevice->CreateSwapChainMethod.AllowAnyCall([=](int32_t, int32_t, DirectXPixelFormat, int32_t, CanvasAlphaMode)
        {
            return swapChain;
        });

        auto canvasSwapChain = f.CreateTestSwapChain();

        swapChain->GetSourceSizeMethod.SetExpectedCalls(1, [](uint32_t* width, uint32_t* height)
        {
            *width = 1;
            *height = 2;
            return S_OK;
        });

        Size size;
        canvasSwapChain->get_SourceSize(&size);
        Assert::AreEqual(1.0f, size.Width);
        Assert::AreEqual(2.0f, size.Height);

        swapChain->SetSourceSizeMethod.SetExpectedCalls(1, [](uint32_t width, uint32_t height)
        {
            Assert::AreEqual(23u, width);
            Assert::AreEqual(42u, height);
            return S_OK;
        });

        canvasSwapChain->put_SourceSize(Size{ 23, 42 });
    }

    TEST_METHOD_EX(CanvasSwapChain_SourceSize_AdjustsForDpi)
    {
        const float dpiScale = 1.5f;

        StubDeviceFixture f;

        auto swapChain = Make<MockDxgiSwapChain>();
        swapChain->SetMatrixTransformMethod.SetExpectedCalls(1);

        f.m_canvasDevice->CreateSwapChainMethod.AllowAnyCall([=](int32_t, int32_t, DirectXPixelFormat, int32_t, CanvasAlphaMode)
        {
            return swapChain;
        });

        auto canvasSwapChain = f.CreateTestSwapChain(DEFAULT_DPI * dpiScale);

        swapChain->GetSourceSizeMethod.SetExpectedCalls(1, [](uint32_t* width, uint32_t* height)
        {
            *width = 1;
            *height = 2;
            return S_OK;
        });

        Size size;
        canvasSwapChain->get_SourceSize(&size);
        Assert::AreEqual(1.0f / dpiScale, size.Width);
        Assert::AreEqual(2.0f / dpiScale, size.Height);

        swapChain->SetSourceSizeMethod.SetExpectedCalls(1, [&](uint32_t width, uint32_t height)
        {
            Assert::AreEqual(static_cast<uint32_t>(round(23 * dpiScale)), width);
            Assert::AreEqual(static_cast<uint32_t>(round(42 * dpiScale)), height);
            return S_OK;
        });

        canvasSwapChain->put_SourceSize(Size{ 23, 42 });
    }

    TEST_METHOD_EX(CanvasSwapChain_ResizeBuffers)
    {
        StubDeviceFixture f;

        f.m_canvasDevice->CreateSwapChainMethod.AllowAnyCall([=](int32_t, int32_t, DirectXPixelFormat, int32_t, CanvasAlphaMode)
        {
            auto swapChain = Make<MockDxgiSwapChain>();

            swapChain->SetMatrixTransformMethod.SetExpectedCalls(1);

            swapChain->ResizeBuffersMethod.SetExpectedCalls(1,
                [=](UINT bufferCount,
                    UINT width,
                    UINT height,
                    DXGI_FORMAT newFormat,
                    UINT swapChainFlags)
                {
                    Assert::AreEqual(3u, bufferCount);
                    Assert::AreEqual(555u, width);
                    Assert::AreEqual(666u, height);
                    Assert::AreEqual(DXGI_FORMAT_R8G8B8A8_UNORM, newFormat);
                    Assert::AreEqual(0u, swapChainFlags);
                    return S_OK;
                });

            return swapChain;
        });

        auto canvasSwapChain = f.CreateTestSwapChain();

        ThrowIfFailed(canvasSwapChain->ResizeBuffersWithAllOptions(555, 666, PIXEL_FORMAT(R8G8B8A8UIntNormalized), 3));
    }

    TEST_METHOD_EX(CanvasSwapChain_ResizeBuffersSizeOnly)
    {
        StubDeviceFixture f;

        const DirectXPixelFormat originalPixelFormat = PIXEL_FORMAT(R16G16B16A16Float);
        const int originalBufferCount = 7;

        f.m_canvasDevice->CreateSwapChainMethod.AllowAnyCall([=](int32_t, int32_t, DirectXPixelFormat, int32_t, CanvasAlphaMode)
        {
            auto swapChain = Make<MockDxgiSwapChain>();
            
            swapChain->SetMatrixTransformMethod.SetExpectedCalls(1);

            swapChain->ResizeBuffersMethod.SetExpectedCalls(1,
                [=](UINT bufferCount,
                    UINT width,
                    UINT height,
                    DXGI_FORMAT newFormat,
                    UINT swapChainFlags)
                {
                    Assert::AreEqual(originalBufferCount, (int)bufferCount);
                    Assert::AreEqual(555u, width);
                    Assert::AreEqual(666u, height);
                    Assert::AreEqual(static_cast<DXGI_FORMAT>(originalPixelFormat), newFormat);
                    Assert::AreEqual(0u, swapChainFlags);
                    return S_OK;
                });

            swapChain->GetDesc1Method.SetExpectedCalls(1,
                [=](DXGI_SWAP_CHAIN_DESC1* desc)
                {
                    desc->Width = 1;
                    desc->Height = 1;
                    desc->Format = static_cast<DXGI_FORMAT>(originalPixelFormat);
                    desc->BufferCount = originalBufferCount;
                    desc->AlphaMode = DXGI_ALPHA_MODE_IGNORE;
                    return S_OK;
                });

            return swapChain;
        });

        auto canvasSwapChain = f.m_swapChainManager->Create(f.m_canvasDevice.Get(),
                                                            1.0f, 1.0f,
                                                            originalPixelFormat,
                                                            originalBufferCount,
                                                            CanvasAlphaMode::Premultiplied,
                                                            DEFAULT_DPI);

        ThrowIfFailed(canvasSwapChain->ResizeBuffersWithSize(555, 666));
    }

    TEST_METHOD_EX(CanvasSwapChain_Present)
    {
        StubDeviceFixture f;

        f.m_canvasDevice->CreateSwapChainMethod.AllowAnyCall([=](int32_t, int32_t, DirectXPixelFormat, int32_t, CanvasAlphaMode)
        {
            auto swapChain = Make<MockDxgiSwapChain>();

            swapChain->SetMatrixTransformMethod.SetExpectedCalls(1);

            swapChain->Present1Method.SetExpectedCalls(1,
                [=](
                UINT syncInterval,
                UINT presentFlags,
                const DXGI_PRESENT_PARAMETERS* presentParameters)
                {
                    Assert::AreEqual(1u, syncInterval);
                    Assert::AreEqual(0u, presentFlags);
                    Assert::IsNotNull(presentParameters);
                    Assert::AreEqual(0u, presentParameters->DirtyRectsCount);
                    Assert::IsNull(presentParameters->pDirtyRects);
                    Assert::IsNull(presentParameters->pScrollOffset);
                    Assert::IsNull(presentParameters->pScrollRect);
                    return S_OK;
                });

            return swapChain;
        });

        auto canvasSwapChain = f.CreateTestSwapChain();

        ThrowIfFailed(canvasSwapChain->Present());
    }

    TEST_METHOD_EX(CanvasSwapChain_PresentWithSyncInterval)
    {
        StubDeviceFixture f;

        f.m_canvasDevice->CreateSwapChainMethod.AllowAnyCall([=](int32_t, int32_t, DirectXPixelFormat, int32_t, CanvasAlphaMode)
        {
            auto swapChain = Make<MockDxgiSwapChain>();

            swapChain->SetMatrixTransformMethod.SetExpectedCalls(1);

            swapChain->Present1Method.SetExpectedCalls(1,
                [=](
                UINT syncInterval,
                UINT presentFlags,
                const DXGI_PRESENT_PARAMETERS* presentParameters)
                {
                    Assert::AreEqual(3u, syncInterval);
                    Assert::AreEqual(0u, presentFlags);
                    Assert::IsNotNull(presentParameters);
                    Assert::AreEqual(0u, presentParameters->DirtyRectsCount);
                    Assert::IsNull(presentParameters->pDirtyRects);
                    Assert::IsNull(presentParameters->pScrollOffset);
                    Assert::IsNull(presentParameters->pScrollRect);
                    return S_OK;
                });

            return swapChain;
        });

        auto canvasSwapChain = f.CreateTestSwapChain();

        ThrowIfFailed(canvasSwapChain->PresentWithSyncInterval(3));
    }

    TEST_METHOD_EX(CanvasSwapChain_CreateDrawingSession)
    {
        StubDeviceFixture f;

        float anyDpi = 123.0f;
        auto anyPixelFormat = static_cast<DXGI_FORMAT>(PIXEL_FORMAT(B8G8R8A8UIntNormalized));
        Color expectedClearColor = { 1, 2, 3, 4 };

        auto dxgiSwapChain = Make<MockDxgiSwapChain>();
        auto dxgiSurface = Make<MockDxgiSurface>();
        auto deviceContext = Make<MockD2DDeviceContext>();
        auto d2dBitmap = Make<MockD2DBitmap>();

        auto canvasSwapChain = f.m_swapChainManager->GetOrCreate(
            f.m_canvasDevice.Get(),
            dxgiSwapChain.Get(),
            anyDpi);


        dxgiSwapChain->GetDesc1Method.SetExpectedCalls(1,
            [&] (DXGI_SWAP_CHAIN_DESC1* desc)
            {
                desc->Width = 123;
                desc->Height = 456;
                desc->Format = anyPixelFormat;
                desc->BufferCount = 5;
                desc->AlphaMode = DXGI_ALPHA_MODE_PREMULTIPLIED;
                return S_OK;
            });

        dxgiSwapChain->GetBufferMethod.SetExpectedCalls(1,
            [&] (UINT buffer, REFIID riid, void** surface)
            {
                Assert::AreEqual(0U, buffer);
                return dxgiSurface.CopyTo(riid, surface);
            });


        f.m_canvasDevice->CreateDeviceContextMethod.SetExpectedCalls(1,
            [&] ()
            {
                return deviceContext;
            });


        deviceContext->ClearMethod.SetExpectedCalls(1,
            [&] (D2D1_COLOR_F const* actualClearColor)
            {
                Assert::AreEqual(ToD2DColor(expectedClearColor), *actualClearColor);
            });

        deviceContext->SetDpiMethod.SetExpectedCalls(1,
            [&] (float dpiX, float dpiY)
            {
                Assert::AreEqual(anyDpi, dpiX);
                Assert::AreEqual(anyDpi, dpiY);
            });

        deviceContext->CreateBitmapFromDxgiSurfaceMethod.SetExpectedCalls(1,
            [&] (IDXGISurface* surface, D2D1_BITMAP_PROPERTIES1 const* properties, ID2D1Bitmap1** value)
            {
                Assert::AreEqual<IDXGISurface*>(dxgiSurface.Get(), surface);
                Assert::AreEqual(D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW, properties->bitmapOptions);
                Assert::AreEqual(anyPixelFormat, properties->pixelFormat.format);
                Assert::AreEqual(D2D1_ALPHA_MODE_PREMULTIPLIED, properties->pixelFormat.alphaMode);
                return d2dBitmap.CopyTo(value);
            });

        deviceContext->SetTargetMethod.SetExpectedCalls(1,
            [&] (ID2D1Image* target)
            {
                Assert::AreEqual<ID2D1Image*>(d2dBitmap.Get(), target);
            });

        deviceContext->BeginDrawMethod.SetExpectedCalls(1);

        deviceContext->EndDrawMethod.SetExpectedCalls(1);


        ComPtr<ICanvasDrawingSession> drawingSession;
        ThrowIfFailed(canvasSwapChain->CreateDrawingSession(expectedClearColor, &drawingSession));

        auto retrievedDeviceContext = GetWrappedResource<ID2D1DeviceContext>(drawingSession);
        Assert::IsTrue(IsSameInstance(deviceContext.Get(), retrievedDeviceContext.Get()));
    }

    struct DrawingSessionAdapterFixture
    {
        std::shared_ptr<CanvasSwapChainManager> m_swapChainManager;
        ComPtr<StubCanvasDevice> m_canvasDevice;
        ComPtr<MockD2DDeviceContext> m_deviceContext;

        enum Options
        {
            None,
            FailDuringClear
        };

        DrawingSessionAdapterFixture(Options options = None)
        {
            m_swapChainManager = std::make_shared<CanvasSwapChainManager>();

            m_deviceContext = Make<MockD2DDeviceContext>();

            auto expectedTargetBitmap = Make<StubD2DBitmap>();
            DXGI_FORMAT expectedFormat = DXGI_FORMAT_R16G16B16A16_UNORM;

            auto expectedBackBufferSurface = Make<MockDxgiSurface>();

            auto d2dDevice = Make<MockD2DDevice>();
            d2dDevice->MockCreateDeviceContext =
                [this, expectedBackBufferSurface, expectedFormat, expectedTargetBitmap, options](D2D1_DEVICE_CONTEXT_OPTIONS deviceContextOptions, ID2D1DeviceContext1** value)
                {
                    Assert::AreEqual(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, deviceContextOptions);

                    m_deviceContext->CreateBitmapFromDxgiSurfaceMethod.SetExpectedCalls(1,
                        [expectedBackBufferSurface, expectedFormat, expectedTargetBitmap](IDXGISurface* surface, const D2D1_BITMAP_PROPERTIES1* properties, ID2D1Bitmap1** out)
                        {
                            Assert::AreEqual(static_cast<IDXGISurface*>(expectedBackBufferSurface.Get()), surface);
                            Assert::IsNotNull(properties);
                            Assert::IsNull(properties->colorContext);
                            Assert::AreEqual(DEFAULT_DPI, properties->dpiX);
                            Assert::AreEqual(DEFAULT_DPI, properties->dpiY);
                            Assert::AreEqual(D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW, properties->bitmapOptions);
                            Assert::AreEqual(expectedFormat, properties->pixelFormat.format);
                            Assert::AreEqual(D2D1_ALPHA_MODE_IGNORE, properties->pixelFormat.alphaMode);

                            ThrowIfFailed(expectedTargetBitmap.CopyTo(out));

                            return S_OK;
                        });

                    m_deviceContext->SetTargetMethod.SetExpectedCalls(1,
                        [expectedTargetBitmap](ID2D1Image* target)
                        {
                            Assert::AreEqual(static_cast<ID2D1Image*>(expectedTargetBitmap.Get()), target);
                        });

                    m_deviceContext->BeginDrawMethod.SetExpectedCalls(1,
                        []()
                        {

                        });
                    
                    m_deviceContext->ClearMethod.SetExpectedCalls(1,
                        [options](D2D1_COLOR_F const* color)
                        {
                            if (options == FailDuringClear)
                            {
                                ThrowHR(E_NOTIMPL);
                            }

                            D2D1_COLOR_F d2dBlue = D2D1::ColorF(0, 0, 1);
                            Assert::AreEqual(d2dBlue, *color);
                        });

                    m_deviceContext->EndDrawMethod.SetExpectedCalls(1,
                        [](D2D1_TAG*, D2D1_TAG*)
                        {
                            return S_OK;
                        });

                    ThrowIfFailed(m_deviceContext.CopyTo(value));
                };

            m_canvasDevice = Make<StubCanvasDevice>(d2dDevice);
            
            m_canvasDevice->CreateSwapChainMethod.AllowAnyCall([=](int32_t, int32_t, DirectXPixelFormat, int32_t, CanvasAlphaMode)
            {
                auto swapChain = Make<MockDxgiSwapChain>();

                swapChain->SetMatrixTransformMethod.SetExpectedCalls(1);

                swapChain->GetDesc1Method.SetExpectedCalls(1,
                    [=](DXGI_SWAP_CHAIN_DESC1* desc)
                    {
                        // The only fields that should get used are the format and alpha mode.
                        DXGI_SWAP_CHAIN_DESC1 zeroed = {};
                        *desc = zeroed;

                        desc->Format = expectedFormat;
                        desc->AlphaMode = DXGI_ALPHA_MODE_IGNORE;
                        return S_OK;
                    });

                swapChain->GetBufferMethod.SetExpectedCalls(1,
                    [=](UINT index, const IID& iid, void** out)
                    {
                        Assert::AreEqual(0u, index);
                        Assert::AreEqual(__uuidof(IDXGISurface2), iid);

                        ThrowIfFailed(expectedBackBufferSurface.CopyTo(reinterpret_cast<IDXGISurface2**>(out)));

                        return S_OK;
                    });

                return swapChain;
            });
        }

        ComPtr<CanvasSwapChain> CreateTestSwapChain()
        {
            return m_swapChainManager->Create(
                m_canvasDevice.Get(),
                1.0f,
                1.0f,
                PIXEL_FORMAT(B8G8R8A8UIntNormalized),
                2,
                CanvasAlphaMode::Premultiplied,
                DEFAULT_DPI);
        }
    };

    TEST_METHOD_EX(CanvasSwapChain_DrawingSessionAdapter)
    {
        //
        // This test verifies the interaction between drawing session creation and the underlying
        // D2D/DXGI resources.
        //

        DrawingSessionAdapterFixture f;

        auto canvasSwapChain = f.CreateTestSwapChain();

        f.m_deviceContext->SetDpiMethod.SetExpectedCalls(1);

        ComPtr<ICanvasDrawingSession> drawingSession;
        Color canvasBlue = { 255, 0, 0, 255 };
        ThrowIfFailed(canvasSwapChain->CreateDrawingSession(canvasBlue, &drawingSession));
    }

    TEST_METHOD_EX(CanvasSwapChainDrawingSessionAdapter_EndDraw_Cleanup)
    {
        //
        // This test ensures that even if an exception occurs, EndDraw still gets called on
        // the native device context.
        //
        // This tests injects the failure a bit differently from 
        // CanvasImageSourceDrawingSessionAdapter_When_SisNative_Gives_Unusuable_DeviceContext_Then_EndDraw_Called.
        //
        // The implementation for CanvasImageSource does a QI with a change in type (ID2D1DeviceContext/ID2D1DeviceContext1)
        // so that test injects a failure into CopyTo. CanvasSwapChain's implementation does not have this QI, so
        // the failure is injected into Clear instead.
        // 

        DrawingSessionAdapterFixture f(DrawingSessionAdapterFixture::FailDuringClear);

        auto canvasSwapChain = f.CreateTestSwapChain();

        ComPtr<ICanvasDrawingSession> drawingSession;
        Assert::AreEqual(E_NOTIMPL, canvasSwapChain->CreateDrawingSession(Color{0, 0, 0, 0}, &drawingSession));
    }

    TEST_METHOD_EX(CanvasSwapChain_DpiProperties)
    {
        const float dpi = 144;

        StubDeviceFixture f;

        auto swapChain = f.m_swapChainManager->Create(f.m_canvasDevice.Get(),
                                                      1.0f, 1.0f,
                                                      PIXEL_FORMAT(B8G8R8A8UIntNormalizedSrgb),
                                                      2,
                                                      CanvasAlphaMode::Ignore,
                                                      dpi);

        float actualDpi = 0;
        ThrowIfFailed(swapChain->get_Dpi(&actualDpi));
        Assert::AreEqual(dpi, actualDpi);

        const float testValue = 100;

        int pixels = 0;
        ThrowIfFailed(swapChain->ConvertDipsToPixels(testValue, &pixels));
        Assert::AreEqual((int)(testValue * dpi / DEFAULT_DPI), pixels);

        float dips = 0;
        ThrowIfFailed(swapChain->ConvertPixelsToDips((int)testValue, &dips));
        Assert::AreEqual(testValue * DEFAULT_DPI / dpi, dips);
    }
};
