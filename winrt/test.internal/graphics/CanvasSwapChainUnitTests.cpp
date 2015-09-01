// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"
#include "mocks/MockCoreWindow.h"
#include "mocks/MockDXGIAdapter.h"
#include "mocks/MockDXGIFactory.h"
#include "stubs/StubDxgiDevice.h"
#include "stubs/StubDxgiSwapChain.h"

TEST_CLASS(CanvasSwapChainUnitTests)
{
    struct StubDeviceFixture
    {
        ComPtr<StubCanvasDevice> m_canvasDevice;

        StubDeviceFixture()
        {
            m_canvasDevice = Make<StubCanvasDevice>();
            
            m_canvasDevice->CreateSwapChainForCompositionMethod.AllowAnyCall([=](int32_t, int32_t, DirectXPixelFormat, int32_t, CanvasAlphaMode)
            {
                auto dxgiSwapChain = Make<MockDxgiSwapChain>();
                dxgiSwapChain->SetMatrixTransformMethod.SetExpectedCalls(1);
                return dxgiSwapChain;
            });
        }

        ComPtr<CanvasSwapChain> CreateTestSwapChain(float dpi = DEFAULT_DPI)
        {
            return CanvasSwapChain::CreateNew(
                m_canvasDevice.Get(),
                1.0f,
                1.0f,
                dpi,
                CanvasSwapChain::DefaultPixelFormat,
                CanvasSwapChain::DefaultBufferCount,
                CanvasSwapChain::DefaultCompositionAlphaMode);
        }
    };

    TEST_METHOD_EX(CanvasSwapChain_Creation)
    {
        StubDeviceFixture f;

        const int dpiScale = 2;

        f.m_canvasDevice->CreateSwapChainForCompositionMethod.SetExpectedCalls(1, 
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

        auto swapChain = CanvasSwapChain::CreateNew(
            f.m_canvasDevice.Get(),
            23.0f,
            45.0f,
            DEFAULT_DPI * dpiScale,
            PIXEL_FORMAT(B8G8R8A8UIntNormalizedSrgb),
            4,
            CanvasAlphaMode::Ignore);

        float dpi = 0;
        ThrowIfFailed(swapChain->get_Dpi(&dpi));
        Assert::AreEqual(DEFAULT_DPI * dpiScale, dpi);
    }

    struct MockDxgiFixture
    {
        ComPtr<MockDxgiFactory> DxgiFactory;
        ComPtr<StubDxgiDevice> DxgiDevice;
        ComPtr<CanvasDevice> Device;

        MockDxgiFixture()
            : DxgiFactory(Make<MockDxgiFactory>())
            , DxgiDevice(Make<StubDxgiDevice>())
        {
            auto dxgiAdapter = Make<MockDxgiAdapter>();
            dxgiAdapter->GetParentMethod.AllowAnyCall(
                [=] (IID const& iid, void** out)
                {
                    Assert::AreEqual(__uuidof(IDXGIFactory2), iid);
                    return DxgiFactory.CopyTo(reinterpret_cast<IDXGIFactory2**>(out));
                });

            DxgiDevice->MockGetParent =
                [=] (IID const& iid, void** out)
                {
                    Assert::AreEqual(__uuidof(IDXGIAdapter2), iid);
                    return dxgiAdapter.CopyTo(reinterpret_cast<IDXGIAdapter2**>(out));
                };

            auto d2dDevice = Make<MockD2DDevice>(DxgiDevice.Get());

            auto resourceCreationAdapter = std::make_shared<TestDeviceResourceCreationAdapter>();
            auto deviceManager = std::make_shared<CanvasDeviceManager>(resourceCreationAdapter);

            Device = deviceManager->CreateWrapper(d2dDevice.Get());
        }
    };

    struct CoreWindowFixture : public MockDxgiFixture
    {
        ComPtr<MockCoreWindow> Window;
        
        CoreWindowFixture()
            : Window(Make<MockCoreWindow>())
        {
        }
    };

    TEST_METHOD_EX(CanvasSwapChain_CreateForCoreWindowWithDpi)
    {
        CoreWindowFixture f;

        auto anyWidthInDips = 100.0f;
        auto anyHeightInDips = 200.0f;
        auto anyDpi = 50.0f;

        auto dxgiSwapChain = Make<MockDxgiSwapChain>();

        f.Window->get_BoundsMethod.SetExpectedCalls(1,
            [&] (Rect* bounds)
            {
                bounds->X = 123;
                bounds->Y = 456;
                bounds->Width = anyWidthInDips;
                bounds->Height = anyHeightInDips;
                return S_OK;
            });

        f.DxgiFactory->CreateSwapChainForCoreWindowMethod.SetExpectedCalls(1,
            [&] (IUnknown* device, IUnknown* window, const DXGI_SWAP_CHAIN_DESC1* desc, IDXGIOutput* restrictToOutput, IDXGISwapChain1** swapChain)
            {
                Assert::IsTrue(IsSameInstance(f.DxgiDevice.Get(), device));
                Assert::IsTrue(IsSameInstance(f.Window.Get(), window));
                Assert::IsNull(restrictToOutput);
                Assert::IsNotNull(swapChain);

                Assert::AreEqual<int>(SizeDipsToPixels(anyWidthInDips, anyDpi), desc->Width);
                Assert::AreEqual<int>(SizeDipsToPixels(anyHeightInDips, anyDpi), desc->Height);
                Assert::AreEqual(static_cast<DXGI_FORMAT>(CanvasSwapChain::DefaultPixelFormat), desc->Format);
                Assert::AreEqual(false, !!desc->Stereo);
                Assert::AreEqual(1U, desc->SampleDesc.Count);
                Assert::AreEqual(0U, desc->SampleDesc.Quality);
                Assert::IsTrue(DXGI_USAGE_RENDER_TARGET_OUTPUT == desc->BufferUsage);
                Assert::AreEqual<int>(CanvasSwapChain::DefaultBufferCount, desc->BufferCount);
                Assert::IsTrue(DXGI_SCALING_STRETCH == desc->Scaling);
                Assert::IsTrue(DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL == desc->SwapEffect);
                Assert::IsTrue(ToDxgiAlphaMode(CanvasSwapChain::DefaultCoreWindowAlphaMode) == desc->AlphaMode);
                Assert::AreEqual(0U, desc->Flags);
                
                return dxgiSwapChain.CopyTo(swapChain);
            });

        auto sc = CanvasSwapChain::CreateNew(
            f.Device.Get(),
            f.Window.Get(),
            anyDpi);

        auto wrappedSc = GetWrappedResource<IDXGISwapChain1>(sc);
        Assert::IsTrue(IsSameInstance(dxgiSwapChain.Get(), wrappedSc.Get()));
    }

    TEST_METHOD_EX(CanvasSwapChain_CreateForCoreWindowWithDpi_ErrorCases)
    {
        CoreWindowFixture f;

        auto anyDpi = 123.0f;

        f.Window->get_BoundsMethod.AllowAnyCall(
            [&] (Rect* bounds)
            {
                bounds->X = 123;
                bounds->Y = 456;
                bounds->Width = 789;
                bounds->Height = 101112;
                return S_OK;
            });

        ExpectHResultException(E_INVALIDARG, [&] { CanvasSwapChain::CreateNew(nullptr, f.Window.Get(), anyDpi); });
        ExpectHResultException(E_INVALIDARG, [&] { CanvasSwapChain::CreateNew(f.Device.Get(), nullptr, anyDpi); });
        ExpectHResultException(E_INVALIDARG, [&] { CanvasSwapChain::CreateNew(f.Device.Get(), f.Window.Get(), -1.0f); });
    }

    TEST_METHOD_EX(CanvasSwapChain_CreateForCoreWindowWithAllOptionsAndDpi)
    {
        CoreWindowFixture f;

        auto anyWidthInDips = 100.0f;
        auto anyHeightInDips = 200.0f;
        auto anyPixelFormat = PIXEL_FORMAT(R8G8B8A8UIntNormalized);
        auto anyBufferCount = 3;
        auto anyDpi = 50.0f;

        auto dxgiSwapChain = Make<MockDxgiSwapChain>();

        f.DxgiFactory->CreateSwapChainForCoreWindowMethod.SetExpectedCalls(1,
            [&] (IUnknown* device, IUnknown* window, const DXGI_SWAP_CHAIN_DESC1* desc, IDXGIOutput* restrictToOutput, IDXGISwapChain1** swapChain)
            {
                Assert::IsTrue(IsSameInstance(f.DxgiDevice.Get(), device));
                Assert::IsTrue(IsSameInstance(f.Window.Get(), window));
                Assert::IsNull(restrictToOutput);
                Assert::IsNotNull(swapChain);

                Assert::AreEqual<int>(SizeDipsToPixels(anyWidthInDips, anyDpi), desc->Width);
                Assert::AreEqual<int>(SizeDipsToPixels(anyHeightInDips, anyDpi), desc->Height);
                Assert::AreEqual(static_cast<DXGI_FORMAT>(anyPixelFormat), desc->Format);
                Assert::AreEqual(false, !!desc->Stereo);
                Assert::AreEqual(1U, desc->SampleDesc.Count);
                Assert::AreEqual(0U, desc->SampleDesc.Quality);
                Assert::IsTrue(DXGI_USAGE_RENDER_TARGET_OUTPUT == desc->BufferUsage);
                Assert::AreEqual<int>(anyBufferCount, desc->BufferCount);
                Assert::IsTrue(DXGI_SCALING_STRETCH == desc->Scaling);
                Assert::IsTrue(DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL == desc->SwapEffect);
                Assert::AreEqual(0U, desc->Flags);
                
                return dxgiSwapChain.CopyTo(swapChain);
            });

        auto sc = CanvasSwapChain::CreateNew(
            f.Device.Get(),
            f.Window.Get(),
            anyWidthInDips,
            anyHeightInDips,
            anyDpi,
            anyPixelFormat,
            anyBufferCount);

        auto wrappedSc = GetWrappedResource<IDXGISwapChain1>(sc);
        Assert::IsTrue(IsSameInstance(dxgiSwapChain.Get(), wrappedSc.Get()));
    }


    TEST_METHOD_EX(CanvasSwapChain_CreateForCoreWindowWithAllOptionsAndDpi_ErrorCases)
    {
        CoreWindowFixture f;

        auto anyWidthInDips = 100.0f;
        auto anyHeightInDips = 200.0f;
        auto anyPixelFormat = PIXEL_FORMAT(R8G8B8A8UIntNormalized);
        auto anyBufferCount = 3;
        auto anyDpi = 50.0f;

        ExpectHResultException(E_INVALIDARG, [&] { CanvasSwapChain::CreateNew((ICanvasDevice*)nullptr, f.Window.Get(), anyWidthInDips, anyHeightInDips, anyDpi, anyPixelFormat, anyBufferCount); });
        ExpectHResultException(E_INVALIDARG, [&] { CanvasSwapChain::CreateNew(f.Device.Get(), (ICoreWindow*)nullptr, anyWidthInDips, anyHeightInDips, anyDpi, anyPixelFormat, anyBufferCount); });
        ExpectHResultException(E_INVALIDARG, [&] { CanvasSwapChain::CreateNew(f.Device.Get(), f.Window.Get(), -1.0f, anyHeightInDips, anyDpi, anyPixelFormat, anyBufferCount); });
        ExpectHResultException(E_INVALIDARG, [&] { CanvasSwapChain::CreateNew(f.Device.Get(), f.Window.Get(), anyWidthInDips, -1.0f, anyDpi, anyPixelFormat, anyBufferCount); });
        ExpectHResultException(E_INVALIDARG, [&] { CanvasSwapChain::CreateNew(f.Device.Get(), f.Window.Get(), anyWidthInDips, anyHeightInDips, -1.0f, anyPixelFormat, anyBufferCount); });
        ExpectHResultException(E_INVALIDARG, [&] { CanvasSwapChain::CreateNew(f.Device.Get(), f.Window.Get(), anyWidthInDips, anyHeightInDips, anyDpi, anyPixelFormat, -1); });
    }


    struct FullDeviceFixture : public MockDxgiFixture
    {
    public:
        ComPtr<CanvasSwapChain> CreateTestSwapChain(float width, float height, int32_t bufferCount)
        {
            return CanvasSwapChain::CreateNew(
                Device.Get(),
                width,
                height,
                DEFAULT_DPI,
                CanvasSwapChain::DefaultPixelFormat,
                bufferCount,
                CanvasSwapChain::DefaultCompositionAlphaMode);
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

        Assert::AreEqual(RO_E_CLOSED, canvasSwapChain->ResizeBuffersWithSize(Size{ 2, 2 }));
        Assert::AreEqual(RO_E_CLOSED, canvasSwapChain->ResizeBuffersWithWidthAndHeight(2, 2));
        Assert::AreEqual(RO_E_CLOSED, canvasSwapChain->ResizeBuffersWithAllOptions(2, 2, DEFAULT_DPI, PIXEL_FORMAT(B8G8R8A8UIntNormalized), 2));

        ComPtr<ICanvasDevice> device;
        Assert::AreEqual(RO_E_CLOSED, canvasSwapChain->get_Device(&device));

        Assert::AreEqual(RO_E_CLOSED, canvasSwapChain->WaitForVerticalBlank());
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

        f.m_canvasDevice->CreateSwapChainForCompositionMethod.AllowAnyCall([=](int32_t, int32_t, DirectXPixelFormat, int32_t, CanvasAlphaMode)
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

        f.m_canvasDevice->CreateSwapChainForCompositionMethod.AllowAnyCall([=](int32_t, int32_t, DirectXPixelFormat, int32_t, CanvasAlphaMode)
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

        f.m_canvasDevice->CreateSwapChainForCompositionMethod.AllowAnyCall([=](int32_t, int32_t, DirectXPixelFormat, int32_t, CanvasAlphaMode)
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

        f.m_canvasDevice->CreateSwapChainForCompositionMethod.AllowAnyCall([=](int32_t, int32_t, DirectXPixelFormat, int32_t, CanvasAlphaMode)
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

        f.m_canvasDevice->CreateSwapChainForCompositionMethod.AllowAnyCall([=](int32_t, int32_t, DirectXPixelFormat, int32_t, CanvasAlphaMode)
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

        f.m_canvasDevice->CreateSwapChainForCompositionMethod.AllowAnyCall([=](int32_t, int32_t, DirectXPixelFormat, int32_t, CanvasAlphaMode)
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

        f.m_canvasDevice->CreateSwapChainForCompositionMethod.AllowAnyCall([=](int32_t, int32_t, DirectXPixelFormat, int32_t, CanvasAlphaMode)
        {
            auto swapChain = Make<StubDxgiSwapChain>();

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

        ThrowIfFailed(canvasSwapChain->ResizeBuffersWithAllOptions(555, 666, DEFAULT_DPI, PIXEL_FORMAT(R8G8B8A8UIntNormalized), 3));
    }

    void VerifyResizeBuffersDpiTestCase(int overloadIndex, float dpiScaling)
    {
        StubDeviceFixture f;

        const float newDpi = DEFAULT_DPI * dpiScaling;

        f.m_canvasDevice->CreateSwapChainForCompositionMethod.AllowAnyCall([&](int32_t, int32_t, DirectXPixelFormat, int32_t, CanvasAlphaMode)
        {
            auto swapChain = Make<StubDxgiSwapChain>();

            swapChain->ResizeBuffersMethod.SetExpectedCalls(1,
                [&](UINT bufferCount,
                UINT width,
                UINT height,
                DXGI_FORMAT newFormat,
                UINT swapChainFlags)
                {
                    Assert::AreEqual(100 * dpiScaling, static_cast<float>(width));
                    Assert::AreEqual(200 * dpiScaling, static_cast<float>(height));
                    return S_OK;
                });

            if (overloadIndex == 0)
            {
                swapChain->GetDesc1Method.SetExpectedCalls(1,
                    [=](DXGI_SWAP_CHAIN_DESC1* desc)
                    {
                        desc->Width = 1;
                        desc->Height = 1;
                        desc->Format = static_cast<DXGI_FORMAT>(CanvasSwapChain::DefaultPixelFormat);
                        desc->BufferCount = CanvasSwapChain::DefaultBufferCount;
                        desc->AlphaMode = DXGI_ALPHA_MODE_IGNORE;
                        return S_OK;
                    });
            }

            return swapChain;
        });

        auto canvasSwapChain = f.CreateTestSwapChain();

        if (overloadIndex == 0)
            ThrowIfFailed(canvasSwapChain->ResizeBuffersWithWidthAndHeightAndDpi(100, 200, newDpi));
        else
            ThrowIfFailed(canvasSwapChain->ResizeBuffersWithAllOptions(100, 200, newDpi, CanvasSwapChain::DefaultPixelFormat, CanvasSwapChain::DefaultBufferCount));

        float retrievedDpi;
        ThrowIfFailed(canvasSwapChain->get_Dpi(&retrievedDpi));
        Assert::AreEqual(newDpi, retrievedDpi);
    }

    TEST_METHOD_EX(CanvasSwapChain_ResizeBuffers_ChangesDpi)
    {
        VerifyResizeBuffersDpiTestCase(0, 0.5f);
        VerifyResizeBuffersDpiTestCase(0, 2.0f);

        VerifyResizeBuffersDpiTestCase(1, 0.5f);
        VerifyResizeBuffersDpiTestCase(1, 2.0f);
    }

    TEST_METHOD_EX(CanvasSwapChain_ResizeBuffersSizeOnly)
    {
        for (int whichOverload = 0; whichOverload < 2; whichOverload++)
        {
            StubDeviceFixture f;

            const DirectXPixelFormat originalPixelFormat = PIXEL_FORMAT(R16G16B16A16Float);
            const int originalBufferCount = 7;

            f.m_canvasDevice->CreateSwapChainForCompositionMethod.AllowAnyCall([=](int32_t, int32_t, DirectXPixelFormat, int32_t, CanvasAlphaMode)
            {
                auto swapChain = Make<StubDxgiSwapChain>();

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

            auto canvasSwapChain = CanvasSwapChain::CreateNew(f.m_canvasDevice.Get(),
                                                              1.0f, 1.0f,
                                                              DEFAULT_DPI,
                                                              originalPixelFormat,
                                                              originalBufferCount,
                                                              CanvasAlphaMode::Premultiplied);

            switch (whichOverload)
            {
            case 0:
                ThrowIfFailed(canvasSwapChain->ResizeBuffersWithSize(Size{ 555, 666 }));
                break;

            case 1:
                ThrowIfFailed(canvasSwapChain->ResizeBuffersWithWidthAndHeight(555, 666));
                break;

            default:
                Assert::Fail();
            }
        }
    }

    TEST_METHOD_EX(CanvasSwapChain_Present)
    {
        StubDeviceFixture f;

        f.m_canvasDevice->CreateSwapChainForCompositionMethod.AllowAnyCall([=](int32_t, int32_t, DirectXPixelFormat, int32_t, CanvasAlphaMode)
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

        f.m_canvasDevice->CreateSwapChainForCompositionMethod.AllowAnyCall([=](int32_t, int32_t, DirectXPixelFormat, int32_t, CanvasAlphaMode)
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

        auto canvasSwapChain = Make<CanvasSwapChain>(
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

        f.m_canvasDevice->CreateDeviceContextForDrawingSessionMethod.SetExpectedCalls(1,
            [&] ()
            {
                return deviceContext;
            });

        deviceContext->SetTextAntialiasModeMethod.SetExpectedCalls(1,
            [](D2D1_TEXT_ANTIALIAS_MODE mode)
            {
                Assert::AreEqual(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE, mode);
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
        ComPtr<StubCanvasDevice> m_canvasDevice;
        ComPtr<MockD2DDeviceContext> m_deviceContext;

        enum Options
        {
            None,
            FailDuringClear
        };

        DrawingSessionAdapterFixture(Options options = None)
        {
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
            
            m_canvasDevice->CreateSwapChainForCompositionMethod.AllowAnyCall([=](int32_t, int32_t, DirectXPixelFormat, int32_t, CanvasAlphaMode)
            {
                auto swapChain = Make<StubDxgiSwapChain>(expectedBackBufferSurface);

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

                return swapChain;
            });
        }

        ComPtr<CanvasSwapChain> CreateTestSwapChain()
        {
            return CanvasSwapChain::CreateNew(
                m_canvasDevice.Get(),
                1.0f,
                1.0f,
                DEFAULT_DPI,
                CanvasSwapChain::DefaultPixelFormat,
                CanvasSwapChain::DefaultBufferCount,
                CanvasSwapChain::DefaultCompositionAlphaMode);
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
        f.m_deviceContext->SetTextAntialiasModeMethod.AllowAnyCall();

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

    TEST_METHOD_EX(CanvasSwapChain_ResizeBuffers_DoesNotMessUpTransform)
    {
        struct TestCase
        {
            Matrix3x2 Transform;
            float Dpi;
        } testCases[] {
            { { 2.0f, 0.0f, 0.0f, 2.0f, 1.0f, 1.0f }, DEFAULT_DPI / 2 },
            { { -1.0f, 1.0f, 2.0f, 3.0f, 1.0f, 1.0f }, DEFAULT_DPI },
            { { 4.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f }, DEFAULT_DPI * 2 },
            { { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f }, DEFAULT_DPI },
        };

        for (auto testCase : testCases)
        {
            DrawingSessionAdapterFixture f;

            // Create a swap chain at default DPI
            auto swapChain = f.CreateTestSwapChain();

            Matrix3x2 transform1 = testCase.Transform;
            ThrowIfFailed(swapChain->put_TransformMatrix(transform1));

            // Resize, changing only the DPI
            ThrowIfFailed(swapChain->ResizeBuffersWithWidthAndHeightAndDpi(1.0f, 1.0f, testCase.Dpi));

            // Get the transform back
            Matrix3x2 transform2;
            ThrowIfFailed(swapChain->get_TransformMatrix(&transform2));

            // Should be equal
            Assert::AreEqual(transform1, transform2);
        }
    }

    TEST_METHOD_EX(CanvasSwapChain_DpiProperties)
    {
        const float dpi = 144;

        StubDeviceFixture f;

        auto swapChain = CanvasSwapChain::CreateNew(f.m_canvasDevice.Get(),
                                                    1.0f, 1.0f,
                                                    dpi,
                                                    PIXEL_FORMAT(B8G8R8A8UIntNormalizedSrgb),
                                                    2,
                                                    CanvasAlphaMode::Ignore);

        float actualDpi = 0;
        ThrowIfFailed(swapChain->get_Dpi(&actualDpi));
        Assert::AreEqual(dpi, actualDpi);

        VerifyConvertDipsToPixels(dpi, swapChain);

        const float testValue = 100;
        float dips = 0;
        ThrowIfFailed(swapChain->ConvertPixelsToDips((int)testValue, &dips));
        Assert::AreEqual(testValue * DEFAULT_DPI / dpi, dips);
    }

    TEST_METHOD_EX(CanvasSwapChain_WaitForVerticalBlank)
    {
        StubDeviceFixture f;

        f.m_canvasDevice->CreateSwapChainForCompositionMethod.AllowAnyCall(
            [=](int32_t, int32_t, DirectXPixelFormat, int32_t, CanvasAlphaMode)
            {
                auto dxgiSwapChain = Make<MockDxgiSwapChain>();
                dxgiSwapChain->SetMatrixTransformMethod.SetExpectedCalls(1);
                return dxgiSwapChain;
            });

        auto mockDxgiOutput = Make<MockDxgiOutput>();

        f.m_canvasDevice->GetPrimaryDisplayOutputMethod.SetExpectedCalls(1, 
            [&]()
            {
                return mockDxgiOutput;
            });

        mockDxgiOutput->WaitForVBlankMethod.SetExpectedCalls(1);

        auto canvasSwapChain = f.CreateTestSwapChain();

        ThrowIfFailed(canvasSwapChain->WaitForVerticalBlank());
    }

    TEST_METHOD_EX(CanvasSwapChain_WaitForVerticalBlank_SleepsIfNoPrimaryOutput)
    {
        StubDeviceFixture f;

        f.m_canvasDevice->CreateSwapChainForCompositionMethod.AllowAnyCall(
            [=](int32_t, int32_t, DirectXPixelFormat, int32_t, CanvasAlphaMode)
        {
            auto dxgiSwapChain = Make<MockDxgiSwapChain>();
            dxgiSwapChain->SetMatrixTransformMethod.SetExpectedCalls(1);
            return dxgiSwapChain;
        });

        f.m_canvasDevice->GetPrimaryDisplayOutputMethod.SetExpectedCalls(1,
            [&]()
            {
                return nullptr;
            });

        auto swapChainAdapter = std::make_shared<CanvasSwapChainTestAdapter>();
        CanvasSwapChainAdapter::SetInstance(swapChainAdapter);

        auto canvasSwapChain = CanvasSwapChain::CreateNew(
            f.m_canvasDevice.Get(),
            1.0f,
            1.0f,
            DEFAULT_DPI,
            CanvasSwapChain::DefaultPixelFormat,
            CanvasSwapChain::DefaultBufferCount,
            CanvasSwapChain::DefaultCompositionAlphaMode);

        bool sleepCalled = false;
        swapChainAdapter->m_sleepFn =
            [&](DWORD timeInMs)
            {
                Assert::IsFalse(sleepCalled);
                sleepCalled = true;
                Assert::AreEqual(static_cast<DWORD>(0), timeInMs);
            };

        ThrowIfFailed(canvasSwapChain->WaitForVerticalBlank());

        Assert::IsTrue(sleepCalled);
    }

    static void AssertLockCount(int expectedLockCount, MockD2DFactory* factory)
    {
        Assert::AreEqual(expectedLockCount, factory->GetEnterCount());
        Assert::AreEqual(expectedLockCount, factory->GetLeaveCount());
    }

    TEST_METHOD_EX(CanvasSwapChain_EntersLockWhenCallingDxgiMethods)
    {
        StubDeviceFixture f;

        auto dxgiSwapChain = Make<MockDxgiSwapChain>();

        dxgiSwapChain->Present1Method.AllowAnyCall();
        dxgiSwapChain->ResizeBuffersMethod.AllowAnyCall();

        dxgiSwapChain->GetMatrixTransformMethod.AllowAnyCall([](DXGI_MATRIX_3X2_F* m) { *m = DXGI_MATRIX_3X2_F{}; return S_OK; });
        dxgiSwapChain->SetMatrixTransformMethod.AllowAnyCall();

        dxgiSwapChain->GetRotationMethod.AllowAnyCall([](DXGI_MODE_ROTATION* r) { *r = DXGI_MODE_ROTATION_IDENTITY; return S_OK; });
        dxgiSwapChain->SetRotationMethod.AllowAnyCall();

        dxgiSwapChain->GetSourceSizeMethod.AllowAnyCall([](UINT* w, UINT* h) { *w = 0; *h = 0; return S_OK; });
        dxgiSwapChain->SetSourceSizeMethod.AllowAnyCall();

        dxgiSwapChain->GetDesc1Method.AllowAnyCall([](DXGI_SWAP_CHAIN_DESC1* desc)
        {
            *desc = DXGI_SWAP_CHAIN_DESC1{}; 
            desc->AlphaMode = DXGI_ALPHA_MODE_PREMULTIPLIED;
            return S_OK; 
        });

        f.m_canvasDevice->CreateSwapChainForCompositionMethod.AllowAnyCall([=](int32_t, int32_t, DirectXPixelFormat, int32_t, CanvasAlphaMode)
        {
            return dxgiSwapChain;
        });

        ComPtr<ID2D1Factory> d2dFactory;
        As<ICanvasDeviceInternal>(f.m_canvasDevice)->GetD2DDevice()->GetFactory(&d2dFactory);
        auto mockFactory = static_cast<MockD2DFactory*>(d2dFactory.Get());

        int expectedLockCount = 0;
        AssertLockCount(0, mockFactory);

        // Creating a swapchain should lock the DXGI device.
        auto swapChain = f.CreateTestSwapChain();

        AssertLockCount(++expectedLockCount, mockFactory);

        // get_Size should lock.
        Size size;
        ThrowIfFailed(swapChain->get_Size(&size));

        AssertLockCount(++expectedLockCount, mockFactory);

        // get_SizeInPixels should lock.
        BitmapSize sizeInPixels;
        ThrowIfFailed(swapChain->get_SizeInPixels(&sizeInPixels));

        AssertLockCount(++expectedLockCount, mockFactory);

        // get_Format should lock.
        DirectXPixelFormat pixelFormat;
        ThrowIfFailed(swapChain->get_Format(&pixelFormat));

        AssertLockCount(++expectedLockCount, mockFactory);

        // get_BufferCount should lock.
        int bufferCount;
        ThrowIfFailed(swapChain->get_BufferCount(&bufferCount));

        AssertLockCount(++expectedLockCount, mockFactory);

        // get_AlphaMode should lock.
        CanvasAlphaMode alphaMode;
        ThrowIfFailed(swapChain->get_AlphaMode(&alphaMode));

        AssertLockCount(++expectedLockCount, mockFactory);

        // get_Rotation should lock.
        CanvasSwapChainRotation rotation;
        ThrowIfFailed(swapChain->get_Rotation(&rotation));

        AssertLockCount(++expectedLockCount, mockFactory);

        // put_Rotation should lock.
        ThrowIfFailed(swapChain->put_Rotation(CanvasSwapChainRotation::None));

        AssertLockCount(++expectedLockCount, mockFactory);

        // get_SourceSize should lock.
        ThrowIfFailed(swapChain->get_SourceSize(&size));

        AssertLockCount(++expectedLockCount, mockFactory);

        // put_SourceSize should lock.
        ThrowIfFailed(swapChain->put_SourceSize(Size{}));

        AssertLockCount(++expectedLockCount, mockFactory);

        // get_TransformMatrix should lock.
        Matrix3x2 matrix;
        ThrowIfFailed(swapChain->get_TransformMatrix(&matrix));

        AssertLockCount(++expectedLockCount, mockFactory);

        // put_TransformMatrix should lock.
        ThrowIfFailed(swapChain->put_TransformMatrix(Matrix3x2{}));

        AssertLockCount(++expectedLockCount, mockFactory);

        // Present should lock.
        ThrowIfFailed(swapChain->Present());

        AssertLockCount(++expectedLockCount, mockFactory);

        // ResizeBuffers should lock.
        ThrowIfFailed(swapChain->ResizeBuffersWithAllOptions(1, 1, 1, PIXEL_FORMAT(B8G8R8A8UIntNormalizedSrgb), 2));

        AssertLockCount(++expectedLockCount, mockFactory);
    }
};
