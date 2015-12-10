// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

class Fixture
{
public:
    std::shared_ptr<TestDeviceAdapter> DeviceAdapter;
    std::shared_ptr<SharedDeviceState> SharedDeviceState;

    Fixture()
    {
        DeviceAdapter = std::make_shared<TestDeviceAdapter>();
        CanvasDeviceAdapter::SetInstance(DeviceAdapter);

        SharedDeviceState = SharedDeviceState::GetInstance();
    }

    void AssertDeviceManagerRoundtrip(ICanvasDevice* expectedCanvasDevice)
    {
        ComPtr<ICanvasDeviceInternal> canvasDeviceInternal;
        ThrowIfFailed(expectedCanvasDevice->QueryInterface(canvasDeviceInternal.GetAddressOf()));

        auto d2dDevice = canvasDeviceInternal->GetD2DDevice();
        auto actualCanvasDevice = ResourceManager::GetOrCreate<ICanvasDevice>(d2dDevice.Get());

        Assert::AreEqual<ICanvasDevice*>(expectedCanvasDevice, actualCanvasDevice.Get());
    }
};

TEST_CLASS(CanvasDeviceTests)
{
public:

    TEST_METHOD_EX(CanvasDevice_Implements_Expected_Interfaces)
    {
        Fixture f;
        auto canvasDevice = CanvasDevice::CreateNew(false);

        ASSERT_IMPLEMENTS_INTERFACE(canvasDevice, ICanvasDevice);
        ASSERT_IMPLEMENTS_INTERFACE(canvasDevice, ABI::Windows::Foundation::IClosable);
        ASSERT_IMPLEMENTS_INTERFACE(canvasDevice, ICanvasResourceWrapperNative);
        ASSERT_IMPLEMENTS_INTERFACE(canvasDevice, ICanvasDeviceInternal);
    }

    TEST_METHOD_EX(CanvasDevice_ForceSoftwareRenderer)
    {
        for (int i = 0; i < 2; i++)
        {
            boolean expectedForceSoftwareRenderer = i == 1;

            Fixture f;
            auto canvasDevice = CanvasDevice::CreateNew(
                !!expectedForceSoftwareRenderer);

            Assert::IsNotNull(canvasDevice.Get());

            // Verify ForceSoftwareRenderer property getter returns the right thing
            boolean forceSoftwareRendererActual;
            canvasDevice->get_ForceSoftwareRenderer(&forceSoftwareRendererActual);
            Assert::AreEqual(expectedForceSoftwareRenderer, forceSoftwareRendererActual);

            // Ensure that the getter returns E_INVALIDARG with null ptr.
            Assert::AreEqual(E_INVALIDARG, canvasDevice->get_ForceSoftwareRenderer(nullptr));

            Assert::AreEqual(1, f.DeviceAdapter->m_numD2DFactoryCreationCalls);

            f.AssertDeviceManagerRoundtrip(canvasDevice.Get());
        }
    }

    TEST_METHOD_EX(CanvasDevice_Create_With_Specific_Direct3DDevice)
    {
        ComPtr<StubD3D11Device> stubD3D11Device = Make<StubD3D11Device>();

        ComPtr<IDirect3DDevice> stubDirect3DDevice;
        ThrowIfFailed(CreateDirect3D11DeviceFromDXGIDevice(stubD3D11Device.Get(), &stubDirect3DDevice));

        Fixture f;
        auto canvasDevice = CanvasDevice::CreateNew(
            stubDirect3DDevice.Get());
        Assert::IsNotNull(canvasDevice.Get());

        // A D2D device should still have been created
        Assert::AreEqual(1, f.DeviceAdapter->m_numD2DFactoryCreationCalls);

        // But not a D3D device.
        Assert::AreEqual(0, f.DeviceAdapter->m_numD3dDeviceCreationCalls);

        f.AssertDeviceManagerRoundtrip(canvasDevice.Get());

        //
        // CanvasDevices created using native interop should always return 
        // ForceSoftwareRenderer==false.
        //
        boolean forceSoftwareRenderer;
        ThrowIfFailed(canvasDevice->get_ForceSoftwareRenderer(&forceSoftwareRenderer));
        Assert::IsFalse(!!forceSoftwareRenderer);

        // Try a NULL Direct3DDevice. 
        ExpectHResultException(E_INVALIDARG,
            [&] { CanvasDevice::CreateNew(nullptr); });
    }

    TEST_METHOD_EX(CanvasDevice_Create_From_D2DDevice)
    {
        auto d2dDevice = Make<MockD2DDevice>(Make<MockD2DFactory>().Get());

        Fixture f;
        auto canvasDevice = Make<CanvasDevice>(d2dDevice.Get());
        Assert::IsNotNull(canvasDevice.Get());

        // Nothing should have been created
        Assert::AreEqual(0, f.DeviceAdapter->m_numD2DFactoryCreationCalls);
        Assert::AreEqual(0, f.DeviceAdapter->m_numD3dDeviceCreationCalls);

        f.AssertDeviceManagerRoundtrip(canvasDevice.Get());

        //
        // CanvasDevices created using native interop should always return 
        // ForceSoftwareRenderer==false.
        //
        boolean forceSoftwareRenderer;
        ThrowIfFailed(canvasDevice->get_ForceSoftwareRenderer(&forceSoftwareRenderer));
        Assert::IsFalse(!!forceSoftwareRenderer);
    }

    TEST_METHOD_EX(CanvasDevice_Closed)
    {
        Fixture f;
        auto canvasDevice = CanvasDevice::CreateNew(false);
        Assert::IsNotNull(canvasDevice.Get());

        Assert::AreEqual(S_OK, canvasDevice->Close());

        ComPtr<IDXGIDevice> dxgiDevice;
        Assert::AreEqual(RO_E_CLOSED, canvasDevice->GetInterface(IID_PPV_ARGS(&dxgiDevice)));

        boolean forceSoftwareRenderer;
        Assert::AreEqual(RO_E_CLOSED, canvasDevice->get_ForceSoftwareRenderer(&forceSoftwareRenderer));

        ComPtr<ICanvasLock> lock;
        Assert::AreEqual(RO_E_CLOSED, canvasDevice->Lock(&lock));

        boolean priority;
        Assert::AreEqual(RO_E_CLOSED, canvasDevice->get_LowPriority(&priority));
        Assert::AreEqual(RO_E_CLOSED, canvasDevice->put_LowPriority(false));

        uint64_t cacheSize;
        Assert::AreEqual(RO_E_CLOSED, canvasDevice->get_MaximumCacheSize(&cacheSize));
        Assert::AreEqual(RO_E_CLOSED, canvasDevice->put_MaximumCacheSize(0));
    }

    ComPtr<ID2D1Device1> GetD2DDevice(ComPtr<ICanvasDevice> const& canvasDevice)
    {
        ComPtr<ICanvasDeviceInternal> canvasDeviceInternal;
        Assert::AreEqual(S_OK, canvasDevice.As(&canvasDeviceInternal));
        return canvasDeviceInternal->GetD2DDevice();
    }

    TEST_METHOD_EX(CanvasDevice_HwSwFallback)
    {
        Fixture f;

        int d3dDeviceCreationCount = 0;

        // Default canvas device should be hardware.
        auto canvasDevice = CanvasDevice::CreateNew(
            false);
        d3dDeviceCreationCount++;

        Assert::IsFalse(f.DeviceAdapter->m_retrievableForceSoftwareRenderer);
        Assert::AreEqual(d3dDeviceCreationCount, f.DeviceAdapter->m_numD3dDeviceCreationCalls);

        // Now disable the hardware path.
        f.DeviceAdapter->SetHardwareEnabled(false);

        {
            // Ensure the fallback works.
            canvasDevice = CanvasDevice::CreateNew(
                false);
            d3dDeviceCreationCount++;

            // Ensure the software path was used.
            Assert::IsTrue(f.DeviceAdapter->m_retrievableForceSoftwareRenderer);
            Assert::AreEqual(d3dDeviceCreationCount, f.DeviceAdapter->m_numD3dDeviceCreationCalls);

            // Ensure the ForceSoftwareRenderer property getter returns the right thing.
            boolean forceSoftwareRenderer;
            canvasDevice->get_ForceSoftwareRenderer(&forceSoftwareRenderer);
            Assert::IsFalse(!!forceSoftwareRenderer);
        }
        {
            // Re-create another whole device with the hardware path on, ensuring there isn't some weird statefulness problem.
            f.DeviceAdapter->SetHardwareEnabled(true);
            canvasDevice = CanvasDevice::CreateNew(
                false);
            d3dDeviceCreationCount++;

            // Ensure the hardware path was enabled.
            Assert::IsFalse(f.DeviceAdapter->m_retrievableForceSoftwareRenderer);
            Assert::AreEqual(d3dDeviceCreationCount, f.DeviceAdapter->m_numD3dDeviceCreationCalls);

            // Ensure the ForceSoftwareRenderer property is still correct.
            boolean forceSoftwareRenderer;
            canvasDevice->get_ForceSoftwareRenderer(&forceSoftwareRenderer);
            Assert::IsFalse(!!forceSoftwareRenderer);
        }
    }

    TEST_METHOD_EX(CanvasDevice_DeviceProperty)
    {
        Fixture f;
        auto device = CanvasDevice::CreateNew(false);

        Assert::AreEqual(E_INVALIDARG, device->get_Device(nullptr));

        ComPtr<ICanvasDevice> deviceVerify;
        ThrowIfFailed(device->get_Device(&deviceVerify));
        Assert::AreEqual(static_cast<ICanvasDevice*>(device.Get()), deviceVerify.Get());
    }
    
    TEST_METHOD_EX(CanvasDevice_MaximumBitmapSize_NullArg)
    {
        Fixture f;
        auto canvasDevice = CanvasDevice::CreateNew(false);

        Assert::AreEqual(E_INVALIDARG, canvasDevice->get_MaximumBitmapSizeInPixels(nullptr));
    }

    TEST_METHOD_EX(CanvasDevice_MaximumBitmapSize_Property)
    {
        auto d2dDevice = Make<MockD2DDevice>();

        const int32_t someSize = 1234567;

        d2dDevice->MockCreateDeviceContext =
            [&](D2D1_DEVICE_CONTEXT_OPTIONS, ID2D1DeviceContext1** value)
            {
                auto deviceContext = Make<StubD2DDeviceContext>(d2dDevice.Get());

                deviceContext->GetMaximumBitmapSizeMethod.SetExpectedCalls(1, [&]() { return someSize; });

                ThrowIfFailed(deviceContext.CopyTo(value));
            };

        Fixture f;
        auto canvasDevice = Make<CanvasDevice>(d2dDevice.Get());

        int32_t maximumBitmapSize;
        ThrowIfFailed(canvasDevice->get_MaximumBitmapSizeInPixels(&maximumBitmapSize));

        Assert::AreEqual(someSize, maximumBitmapSize);
    }

    TEST_METHOD_EX(CanvasDevice_IsPixelFormatSupported_NullArg)
    {
        Fixture f;
        auto canvasDevice = CanvasDevice::CreateNew(false);

        Assert::AreEqual(E_INVALIDARG, canvasDevice->IsPixelFormatSupported(PIXEL_FORMAT(B8G8R8A8UIntNormalized), nullptr));
    }

    TEST_METHOD_EX(CanvasDevice_IsPixelFormatSupported)
    {
        auto d2dDevice = Make<MockD2DDevice>();

        d2dDevice->MockCreateDeviceContext =
            [&](D2D1_DEVICE_CONTEXT_OPTIONS, ID2D1DeviceContext1** value)
            {
                auto deviceContext = Make<StubD2DDeviceContext>(d2dDevice.Get());

                deviceContext->IsDxgiFormatSupportedMethod.SetExpectedCalls(2, [](DXGI_FORMAT format)
                {
                    switch (format)
                    {
                        case DXGI_FORMAT_B8G8R8A8_UNORM:
                            return true;

                        case DXGI_FORMAT_BC3_UNORM:
                            return false;

                        default:
                            Assert::Fail();
                            return false;
                    }
                });

                ThrowIfFailed(deviceContext.CopyTo(value));
            };

        Fixture f;
        auto canvasDevice = Make<CanvasDevice>(d2dDevice.Get());

        boolean isSupported;

        ThrowIfFailed(canvasDevice->IsPixelFormatSupported(PIXEL_FORMAT(B8G8R8A8UIntNormalized), &isSupported));
        Assert::IsTrue(!!isSupported);

        ThrowIfFailed(canvasDevice->IsPixelFormatSupported(PIXEL_FORMAT(BC3UIntNormalized), &isSupported));
        Assert::IsFalse(!!isSupported);
    }

    TEST_METHOD_EX(CanvasDevice_IsBufferPrecisionSupported_NullArg)
    {
        Fixture f;
        auto canvasDevice = CanvasDevice::CreateNew(false);

        Assert::AreEqual(E_INVALIDARG, canvasDevice->IsBufferPrecisionSupported(CanvasBufferPrecision::Precision8UIntNormalized, nullptr));
    }

    TEST_METHOD_EX(CanvasDevice_IsBufferPrecisionSupported)
    {
        auto d2dDevice = Make<MockD2DDevice>();

        d2dDevice->MockCreateDeviceContext =
            [&](D2D1_DEVICE_CONTEXT_OPTIONS, ID2D1DeviceContext1** value)
            {
                auto deviceContext = Make<StubD2DDeviceContext>(d2dDevice.Get());

                deviceContext->IsBufferPrecisionSupportedMethod.SetExpectedCalls(2, [](D2D1_BUFFER_PRECISION precision)
                {
                    switch (precision)
                    {
                        case D2D1_BUFFER_PRECISION_8BPC_UNORM:
                            return true;

                        case D2D1_BUFFER_PRECISION_32BPC_FLOAT:
                            return false;

                        default:
                            Assert::Fail();
                            return false;
                    }
                });

                ThrowIfFailed(deviceContext.CopyTo(value));
            };

        Fixture f;
        auto canvasDevice = Make<CanvasDevice>(d2dDevice.Get());

        boolean isSupported;

        ThrowIfFailed(canvasDevice->IsBufferPrecisionSupported(CanvasBufferPrecision::Precision8UIntNormalized, &isSupported));
        Assert::IsTrue(!!isSupported);

        ThrowIfFailed(canvasDevice->IsBufferPrecisionSupported(CanvasBufferPrecision::Precision32Float, &isSupported));
        Assert::IsFalse(!!isSupported);
    }

    TEST_METHOD_EX(CanvasDevice_LowPriority)
    {
        Fixture f;
        
        auto d2dDevice = Make<MockD2DDevice>();
        auto canvasDevice = Make<CanvasDevice>(d2dDevice.Get());

        boolean value;

        // Get null arg.
        Assert::AreEqual(E_INVALIDARG, canvasDevice->get_LowPriority(nullptr));

        // Get normal priority.
        d2dDevice->GetRenderingPriorityMethod.SetExpectedCalls(1, [] { return D2D1_RENDERING_PRIORITY_NORMAL; });
        ThrowIfFailed(canvasDevice->get_LowPriority(&value));
        Assert::IsFalse(!!value);

        // Get low priority.
        d2dDevice->GetRenderingPriorityMethod.SetExpectedCalls(1, [] { return D2D1_RENDERING_PRIORITY_LOW; });
        ThrowIfFailed(canvasDevice->get_LowPriority(&value));
        Assert::IsTrue(!!value);

        // Set normal priority.
        d2dDevice->SetRenderingPriorityMethod.SetExpectedCalls(1, [](D2D1_RENDERING_PRIORITY priority)
        {
            Assert::AreEqual(D2D1_RENDERING_PRIORITY_NORMAL, priority);
        });

        ThrowIfFailed(canvasDevice->put_LowPriority(false));

        // Set low priority.
        d2dDevice->SetRenderingPriorityMethod.SetExpectedCalls(1, [](D2D1_RENDERING_PRIORITY priority)
        {
            Assert::AreEqual(D2D1_RENDERING_PRIORITY_LOW, priority);
        });

        ThrowIfFailed(canvasDevice->put_LowPriority(true));
    }

    TEST_METHOD_EX(CanvasDevice_MaximumCacheSize)
    {
        Fixture f;

        auto d2dDevice = Make<MockD2DDevice>();
        auto canvasDevice = Make<CanvasDevice>(d2dDevice.Get());

        // Get null arg.
        Assert::AreEqual(E_INVALIDARG, canvasDevice->get_MaximumCacheSize(nullptr));

        // Get.
        const uint64_t someValue = 12345;
        uint64_t value;

        d2dDevice->GetMaximumTextureMemoryMethod.SetExpectedCalls(1, [&] { return someValue; });
        ThrowIfFailed(canvasDevice->get_MaximumCacheSize(&value));
        Assert::AreEqual(someValue, value);

        // Set.
        const uint64_t someOtherValue = 6789;

        d2dDevice->SetMaximumTextureMemoryMethod.SetExpectedCalls(1, [&](uint64_t maximumTextureMemory)
        {
            Assert::AreEqual(someOtherValue, maximumTextureMemory);
        });

        ThrowIfFailed(canvasDevice->put_MaximumCacheSize(someOtherValue));
    }

    TEST_METHOD_EX(CanvasDevice_CreateCommandList_ReturnsCommandListFromDeviceContext)
    {
        auto d2dDevice = Make<MockD2DDevice>();

        auto d2dCommandList = Make<MockD2DCommandList>();

        auto deviceContext = Make<StubD2DDeviceContext>(d2dDevice.Get());
        deviceContext->CreateCommandListMethod.SetExpectedCalls(1,
            [&](ID2D1CommandList** value)
            {
                return d2dCommandList.CopyTo(value);
            });

        d2dDevice->MockCreateDeviceContext =
            [&](D2D1_DEVICE_CONTEXT_OPTIONS, ID2D1DeviceContext1** value)
            {
                ThrowIfFailed(deviceContext.CopyTo(value));
            };

        Fixture f;
        auto canvasDevice = Make<CanvasDevice>(d2dDevice.Get());
        auto actualD2DCommandList = canvasDevice->CreateCommandList();

        Assert::IsTrue(IsSameInstance(d2dCommandList.Get(), actualD2DCommandList.Get()));
    }

    TEST_METHOD_EX(CanvasDevice_CreateRenderTarget_ReturnsBitmapCreatedWithCorrectProperties)
    {
        Fixture f;

        auto d2dDevice = Make<MockD2DDevice>();
        auto d2dBitmap = Make<MockD2DBitmap>();

        float anyWidth = 1.0f;
        float anyHeight = 2.0f;
        auto anyFormat = PIXEL_FORMAT(R16G16B16A16UIntNormalized);
        auto anyAlphaMode = CanvasAlphaMode::Ignore;
        float anyDpi = 3.0f;

        auto deviceContext = Make<StubD2DDeviceContext>(d2dDevice.Get());
        deviceContext->CreateBitmapMethod.SetExpectedCalls(1,
            [&] (D2D1_SIZE_U size, void const* sourceData, UINT32 pitch, D2D1_BITMAP_PROPERTIES1 const* bitmapProperties, ID2D1Bitmap1** bitmap)
            {
                Assert::AreEqual<int>(SizeDipsToPixels(anyWidth, anyDpi), size.width);
                Assert::AreEqual<int>(SizeDipsToPixels(anyHeight, anyDpi), size.height);
                Assert::IsNull(sourceData);
                Assert::AreEqual(0U, pitch);
                Assert::AreEqual(D2D1_BITMAP_OPTIONS_TARGET, bitmapProperties->bitmapOptions);
                Assert::AreEqual(anyDpi, bitmapProperties->dpiX);
                Assert::AreEqual(anyDpi, bitmapProperties->dpiY);
                Assert::AreEqual(static_cast<DXGI_FORMAT>(anyFormat), bitmapProperties->pixelFormat.format);
                Assert::AreEqual(ToD2DAlphaMode(anyAlphaMode), bitmapProperties->pixelFormat.alphaMode);
                return d2dBitmap.CopyTo(bitmap);
            });

        d2dDevice->MockCreateDeviceContext =
            [&](D2D1_DEVICE_CONTEXT_OPTIONS, ID2D1DeviceContext1** value)
            {
                ThrowIfFailed(deviceContext.CopyTo(value));
            };

        auto canvasDevice = Make<CanvasDevice>(d2dDevice.Get());
        auto actualBitmap = canvasDevice->CreateRenderTargetBitmap(anyWidth, anyHeight, anyDpi, anyFormat, anyAlphaMode);

        Assert::IsTrue(IsSameInstance(d2dBitmap.Get(), actualBitmap.Get()));
    }
};

TEST_CLASS(DefaultDeviceAdapterTests)
{
    //
    // This tests GetDxgiDevice against real-live D3D/D2D instances since it
    // relies on non-trivial interaction with these to behave as we expect.
    //
    TEST_METHOD_EX(GetDxgiDevice)
    {
        //
        // Set up
        //
        DefaultDeviceAdapter adapter;

        ComPtr<ID3D11Device> d3dDevice;
        if (!adapter.TryCreateD3DDevice(
            true, // Forces software rendering
            false, // No debug layer
            &d3dDevice))
        {
            Assert::Fail(L"Failed to create d3d device");
        }

        ComPtr<IDXGIDevice3> dxgiDevice;
        ThrowIfFailed(d3dDevice.As(&dxgiDevice));

        auto factory = adapter.CreateD2DFactory(CanvasDebugLevel::None);
        ComPtr<ID2D1Device1> d2dDevice;
        ThrowIfFailed(factory->CreateDevice(dxgiDevice.Get(), &d2dDevice));

        //
        // Test
        //
        auto actualDxgiDevice = adapter.GetDxgiDevice(d2dDevice.Get());

        Assert::AreEqual(dxgiDevice.Get(), actualDxgiDevice.Get());
    }
};

static const HRESULT deviceRemovedHResults[] = {
    DXGI_ERROR_DEVICE_HUNG,
    DXGI_ERROR_DEVICE_REMOVED,
    DXGI_ERROR_DEVICE_RESET,
    DXGI_ERROR_DRIVER_INTERNAL_ERROR,
    DXGI_ERROR_INVALID_CALL,
    D2DERR_RECREATE_TARGET
};

TEST_CLASS(CanvasDeviceLostTests)
{
public:

    TEST_METHOD_EX(CanvasDeviceLostTests_Closed)
    {
        Fixture f;
        auto canvasDevice = CanvasDevice::CreateNew(false);

        Assert::AreEqual(S_OK, canvasDevice->Close());

        EventRegistrationToken token{};
        MockEventHandler<DeviceLostHandlerType> dummyDeviceLostHandler(L"DeviceLost");
        Assert::AreEqual(RO_E_CLOSED, canvasDevice->add_DeviceLost(dummyDeviceLostHandler.Get(), &token));

        // remove_DeviceLost is intended to not check if the object is closed, and like all EventSource
        // events it returns success if you try and remove an unregistered token.
        Assert::AreEqual(S_OK, canvasDevice->remove_DeviceLost(token));

        boolean b;
        Assert::AreEqual(RO_E_CLOSED, canvasDevice->IsDeviceLost(0, &b));

        Assert::AreEqual(RO_E_CLOSED, canvasDevice->RaiseDeviceLost());

    }

    TEST_METHOD_EX(CanvasDeviceLostTests_NullArgs)
    {
        Fixture f;
        auto canvasDevice = CanvasDevice::CreateNew(false);

        EventRegistrationToken token{};
        MockEventHandler<DeviceLostHandlerType> dummyDeviceLostHandler(L"DeviceLost");
        Assert::AreEqual(E_INVALIDARG, canvasDevice->add_DeviceLost(nullptr, &token));
        Assert::AreEqual(E_INVALIDARG, canvasDevice->add_DeviceLost(dummyDeviceLostHandler.Get(), nullptr));
        Assert::AreEqual(E_INVALIDARG, canvasDevice->IsDeviceLost(0, nullptr));
    }

    class DeviceLostAdapter : public TestDeviceAdapter
    {
        virtual ComPtr<StubD3D11Device> CreateStubD3D11Device() override
        {
            auto stubD3DDevice = Make<StubD3D11Device>();

            stubD3DDevice->GetDeviceRemovedReasonMethod.AllowAnyCall(
                [] { return DXGI_ERROR_DEVICE_REMOVED; });

            return stubD3DDevice;
        }
    };

    class DeviceLostFixture
    {
        std::shared_ptr<DeviceLostAdapter> m_deviceAdapter;

    public:
        DeviceLostFixture()
        {
            m_deviceAdapter = std::make_shared<DeviceLostAdapter>();
            CanvasDeviceAdapter::SetInstance(m_deviceAdapter);
        }
    };

    TEST_METHOD_EX(CanvasDeviceLostTests_IsDeviceLost_DeviceRemovedHr_DeviceIsLost_ReturnsTrue)
    {
        DeviceLostFixture f;
        auto canvasDevice = CanvasDevice::CreateNew(false);

        for (HRESULT hr : deviceRemovedHResults)
        {
            boolean isDeviceLost;
            Assert::AreEqual(S_OK, canvasDevice->IsDeviceLost(hr, &isDeviceLost));
            Assert::IsTrue(!!isDeviceLost);
        }
    }

    TEST_METHOD_EX(CanvasDeviceLostTests_IsDeviceLost_SomeArbitraryHr_DeviceIsLost_ReturnsFalse)
    {
        DeviceLostFixture f;
        auto canvasDevice = CanvasDevice::CreateNew(false);

        boolean isDeviceLost;
        Assert::AreEqual(S_OK, canvasDevice->IsDeviceLost(E_INVALIDARG, &isDeviceLost));
        Assert::IsFalse(!!isDeviceLost);
    }

    TEST_METHOD_EX(CanvasDeviceLostTests_IsDeviceLost_DeviceRemovedHr_DeviceNotActuallyLost_ReturnsFalse)
    {
        Fixture f;
        auto canvasDevice = CanvasDevice::CreateNew(false);

        for (HRESULT hr : deviceRemovedHResults)
        {
            boolean isDeviceLost;
            Assert::AreEqual(S_OK, canvasDevice->IsDeviceLost(hr, &isDeviceLost));
            Assert::IsFalse(!!isDeviceLost);
        }
    }

    TEST_METHOD_EX(CanvasDeviceLostTests_IsDeviceLost_SomeArbitraryHr_DeviceNotActuallyLost_ReturnsFalse)
    {
        DeviceLostFixture f;
        auto canvasDevice = CanvasDevice::CreateNew(false);

        boolean isDeviceLost;
        Assert::AreEqual(S_OK, canvasDevice->IsDeviceLost(E_INVALIDARG, &isDeviceLost));
        Assert::IsFalse(!!isDeviceLost);
    }

    TEST_METHOD_EX(CanvasDeviceLostTests_RaiseDeviceLost_RaisesSubscribedHandlers_DeviceActuallyLost)
    {
        DeviceLostFixture f;
        auto canvasDevice = CanvasDevice::CreateNew(false);

        MockEventHandler<DeviceLostHandlerType> deviceLostHandler(L"DeviceLost");
        deviceLostHandler.SetExpectedCalls(1);

        EventRegistrationToken token{};
        Assert::AreEqual(S_OK, canvasDevice->add_DeviceLost(deviceLostHandler.Get(), &token));

        Assert::AreEqual(S_OK, canvasDevice->RaiseDeviceLost());
    }

    TEST_METHOD_EX(CanvasDeviceLostTests_RemoveEventThen_RaiseDeviceLost_DoesNotInvokeHandler)
    {
        DeviceLostFixture f;
        auto canvasDevice = CanvasDevice::CreateNew(false);

        MockEventHandler<DeviceLostHandlerType> deviceLostHandler(L"DeviceLost");
        deviceLostHandler.SetExpectedCalls(0);

        EventRegistrationToken token{};
        Assert::AreEqual(S_OK, canvasDevice->add_DeviceLost(deviceLostHandler.Get(), &token));
        Assert::AreEqual(S_OK, canvasDevice->remove_DeviceLost(token));

        Assert::AreEqual(S_OK, canvasDevice->RaiseDeviceLost());
    }

    TEST_METHOD_EX(CanvasDeviceLostTests_RaiseDeviceLost_HasCorrectSenderAndArgs)
    {
        DeviceLostFixture f;
        auto canvasDevice = CanvasDevice::CreateNew(false);

        MockEventHandler<DeviceLostHandlerType> deviceLostHandler(L"DeviceLost");
        deviceLostHandler.SetExpectedCalls(1, 
            [&](ICanvasDevice* sender, IInspectable* args)
            {
                Assert::AreEqual(static_cast<ICanvasDevice*>(canvasDevice.Get()), sender);
                Assert::IsNull(args);
                return S_OK;
            });

        EventRegistrationToken token{};
        Assert::AreEqual(S_OK, canvasDevice->add_DeviceLost(deviceLostHandler.Get(), &token));

        Assert::AreEqual(S_OK, canvasDevice->RaiseDeviceLost());
    }

    TEST_METHOD_EX(CanvasDeviceLostTests_RaiseDeviceLost_ExceptionFromHandlerIsPropagated)
    {
        DeviceLostFixture f;
        auto canvasDevice = CanvasDevice::CreateNew(false);

        MockEventHandler<DeviceLostHandlerType> deviceLostHandler(L"DeviceLost");
        deviceLostHandler.SetExpectedCalls(1, 
            [&](ICanvasDevice* sender, IInspectable* args)
            {
                return E_UNEXPECTED;
            });

        EventRegistrationToken token{};
        Assert::AreEqual(S_OK, canvasDevice->add_DeviceLost(deviceLostHandler.Get(), &token));

        Assert::AreEqual(E_UNEXPECTED, canvasDevice->RaiseDeviceLost());
    }

    class D3DDeviceAccessAdapter : public TestDeviceAdapter
    {
    public:
        ComPtr<StubD3D11Device> D3DDevice;

        D3DDeviceAccessAdapter()
            : D3DDevice(Make<StubD3D11Device>())
        {
        }

        ComPtr<StubD3D11Device> CreateStubD3D11Device() override
        {
            return D3DDevice;
        }
    };

    TEST_METHOD_EX(CanvasDeviceLostTests_RaiseDeviceLost_IfDeviceIsNotActuallyLost_ErrorIsReturned)
    {
        auto adapter = std::make_shared<D3DDeviceAccessAdapter>();
        CanvasDeviceAdapter::SetInstance(adapter);

        auto sharedDeviceState = SharedDeviceState::GetInstance();

        //
        // The default mock for GetDeviceRemovedReason returns S_OK.
        //
        adapter->D3DDevice->GetDeviceRemovedReasonMethod.ExpectAtLeastOneCall();

        auto device = sharedDeviceState->GetSharedDevice(false);

        Assert::AreEqual(E_INVALIDARG, device->RaiseDeviceLost());
        ValidateStoredErrorState(E_INVALIDARG, Strings::DeviceExpectedToBeLost);
    }
};

CanvasDebugLevel allDebugLevels[] = {
    CanvasDebugLevel::Error,
    CanvasDebugLevel::Warning,
    CanvasDebugLevel::Information,
    CanvasDebugLevel::None };

TEST_CLASS(CanvasDebugLevelTests)
{
    class DebugLevelFixture : public Fixture
    {
    public:
        void ExpectOneQueryDebugLevel(CanvasDebugLevel debugLevel)
        {
            DeviceAdapter->GetCoreApplicationMethod.SetExpectedCalls(1,
                [debugLevel]
                {
                    auto coreApplication = Make<MockCoreApplication>();
                    coreApplication->get_PropertiesMethod.SetExpectedCalls(1, 
                        [debugLevel](IPropertySet** out)
                        {
                            auto mockProperties = Make<MockProperties>();
                            mockProperties->LookupMethod.SetExpectedCalls(1, 
                                [debugLevel](HSTRING, IInspectable** out)
                                {
                                    auto debugPropertyHolder = Make<StubDebugProperty>(debugLevel);
                                    debugPropertyHolder.CopyTo(out);
                                    return S_OK;
                                });
                            mockProperties->InsertMethod.AllowAnyCall();
                            return mockProperties.CopyTo(out);
                        });

                    return coreApplication;
                });
        }

        void AssertDebugLevel(CanvasDebugLevel expected)
        {
            Assert::AreEqual(expected, DeviceAdapter->m_debugLevel);
        }

        void AssertD3DDebugLevel(CanvasDebugLevel expected)
        {
            const bool expectD3DDebugLevel = expected != CanvasDebugLevel::None;

            Assert::AreEqual(expectD3DDebugLevel, DeviceAdapter->m_retrievableUseDebugD3DDevice);
        }

        void AssertNoD3DDebugLevel()
        {
            Assert::IsFalse(DeviceAdapter->m_retrievableUseDebugD3DDevice);
        }
    };

    TEST_METHOD_EX(CreateWithForceSoftwareRendererOption_HasCorrectDebugLevel)
    {
        for (auto debugLevel : allDebugLevels)
        {
            DebugLevelFixture f;

            f.SharedDeviceState->SetDebugLevel(debugLevel);

            f.ExpectOneQueryDebugLevel(debugLevel);
            auto canvasDevice = CanvasDevice::CreateNew(false);

            f.AssertD3DDebugLevel(debugLevel);
        }
    }

    TEST_METHOD_EX(CreateFromDirect3D11Device_HasCorrectDebugLevel)
    {
        for (auto debugLevel : allDebugLevels)
        {
            DebugLevelFixture f;

            ComPtr<StubD3D11Device> stubD3D11Device = Make<StubD3D11Device>();

            ComPtr<IDirect3DDevice> stubDirect3DDevice;
            ThrowIfFailed(CreateDirect3D11DeviceFromDXGIDevice(stubD3D11Device.Get(), &stubDirect3DDevice));

            f.SharedDeviceState->SetDebugLevel(debugLevel);

            f.ExpectOneQueryDebugLevel(debugLevel);
            auto canvasDevice = CanvasDevice::CreateNew(stubDirect3DDevice.Get());
            
            f.AssertDebugLevel(debugLevel);
            f.AssertNoD3DDebugLevel(); // We don't own creating a D3D device in this path
        }
    }

    TEST_METHOD_EX(GetSharedDevice_HasCorrectDebugLevel)
    {
        for (auto debugLevel : allDebugLevels)
        {
            DebugLevelFixture f;

            f.SharedDeviceState->SetDebugLevel(debugLevel);

            f.ExpectOneQueryDebugLevel(debugLevel);

            auto canvasDevice = f.SharedDeviceState->GetSharedDevice(false);

            f.AssertDebugLevel(debugLevel);
            f.AssertD3DDebugLevel(debugLevel);
        }
    }

    TEST_METHOD_EX(GetSharedDevice_ReturnsExistingDevice_EvenIfDebugLevelDoesntMatch)
    {
        DebugLevelFixture f;

        f.SharedDeviceState->SetDebugLevel(CanvasDebugLevel::Information);
        auto canvasDevice1 = f.SharedDeviceState->GetSharedDevice(false);

        f.SharedDeviceState->SetDebugLevel(CanvasDebugLevel::Warning);
        auto canvasDevice2 = f.SharedDeviceState->GetSharedDevice(false);

        Assert::IsTrue(IsSameInstance(canvasDevice1.Get(), canvasDevice2.Get()));
    }
};


bool allForceSoftwareRendererOptions[] = {
    true,
    false
};

TEST_CLASS(CanvasGetSharedDeviceTests)
{
public:

    class GetSharedDevice_Adapter : public TestDeviceAdapter
    {
        int m_deviceLostCounter;
        bool m_canCreateDevices;

    public:
        CALL_COUNTER_WITH_MOCK(CreateStubD3D11DeviceMethod, ComPtr<StubD3D11Device>());

        GetSharedDevice_Adapter()
            : m_canCreateDevices(true)
        {
            CreateStubD3D11DeviceMethod.AllowAnyCall([]{return Make<StubD3D11Device>(); });
        }

        ComPtr<StubD3D11Device> CreateStubD3D11Device() override
        {
            return CreateStubD3D11DeviceMethod.WasCalled();
        }

        virtual bool TryCreateD3DDevice(
            bool forceSoftwareRenderer,
            bool useDebugD3DDevice,
            ComPtr<ID3D11Device>* device) override
        {
            if (m_canCreateDevices)
                return __super::TryCreateD3DDevice(forceSoftwareRenderer, useDebugD3DDevice, device);
            else
                return false;
        }

        void SetCreatingDevicesEnabled(bool value)
        {
            m_canCreateDevices = value;
        }

    };

    class Fixture
    {
    public:
        std::shared_ptr<GetSharedDevice_Adapter> Adapter;
        std::shared_ptr<SharedDeviceState> SharedDeviceState;

        Fixture()
        {
            Adapter = std::make_shared<GetSharedDevice_Adapter>();
            CanvasDeviceAdapter::SetInstance(Adapter);

            SharedDeviceState = SharedDeviceState::GetInstance();
        }
    };

    TEST_METHOD_EX(CanvasGetSharedDeviceTests_NullArg)
    {
        auto canvasDeviceFactory = Make<CanvasDeviceFactory>();

        Assert::AreEqual(E_INVALIDARG, canvasDeviceFactory->GetSharedDevice(nullptr));
    }

    ComPtr<ICanvasDevice> GetSharedDevice_ExpectForceSoftwareRenderer(
        Fixture& f,
        bool passedInAndExpectedBack)
    {
        ComPtr<ICanvasDevice> device = f.SharedDeviceState->GetSharedDevice(passedInAndExpectedBack);

        boolean retrievedValue;
        Assert::AreEqual(S_OK, device->get_ForceSoftwareRenderer(&retrievedValue));
        Assert::AreEqual(passedInAndExpectedBack, !!retrievedValue);

        return device;
    }

    TEST_METHOD_EX(CanvasGetSharedDeviceTests_CreateNewDevice)
    {
        Fixture f;

        GetSharedDevice_ExpectForceSoftwareRenderer(f, true);
        GetSharedDevice_ExpectForceSoftwareRenderer(f, false);
    }

    TEST_METHOD_EX(CanvasGetSharedDeviceTests_GetExistingDevice)
    {
        Fixture f; 

        // Set up this way to validate against cache entries overwriting the wrong spot.
        ComPtr<ICanvasDevice> devices[_countof(allForceSoftwareRendererOptions) * 2];

        devices[0] = GetSharedDevice_ExpectForceSoftwareRenderer(f, false);
        devices[1] = GetSharedDevice_ExpectForceSoftwareRenderer(f, true);

        devices[2] = GetSharedDevice_ExpectForceSoftwareRenderer(f, false);
        devices[3] = GetSharedDevice_ExpectForceSoftwareRenderer(f, true);

        Assert::AreEqual(devices[0].Get(), devices[2].Get());
        Assert::AreEqual(devices[1].Get(), devices[3].Get());
    }

    TEST_METHOD_EX(CanvasGetSharedDeviceTests_CreateNewDevice_HasSoftwareFallback)
    {
        Fixture f;
        f.Adapter->SetHardwareEnabled(false);

        GetSharedDevice_ExpectForceSoftwareRenderer(f, false);
    }

    TEST_METHOD_EX(CanvasGetSharedDeviceTests_NoDeviceAvailable)
    {
        Fixture f;
        f.Adapter->SetCreatingDevicesEnabled(false);

        ExpectHResultException(E_FAIL, [&]{ f.SharedDeviceState->GetSharedDevice(false); });
    }

    TEST_METHOD_EX(CanvasGetSharedDeviceTests_ExistingDevice_Lost_RaisesEvent)
    {
        Fixture f;

        auto d3dDevice = Make<StubD3D11Device>();
        f.Adapter->CreateStubD3D11DeviceMethod.AllowAnyCall([&](){ return d3dDevice; });

        auto device = f.SharedDeviceState->GetSharedDevice(false);

        // 
        //Expect the DeviceLost event to get raised.
        //
        MockEventHandler<DeviceLostHandlerType> deviceLostHandler(L"DeviceLost");
        deviceLostHandler.SetExpectedCalls(1);
        EventRegistrationToken token{};
        Assert::AreEqual(S_OK, device->add_DeviceLost(deviceLostHandler.Get(), &token));

        // Lose the device
        d3dDevice->GetDeviceRemovedReasonMethod.AllowAnyCall(
            [&] 
            { 
                return DXGI_ERROR_DEVICE_REMOVED;
            });

        // Try and get the cached device again
        f.SharedDeviceState->GetSharedDevice(false);
    }

    TEST_METHOD_EX(CanvasGetSharedDeviceTests_ExistingDevice_LastDeviceReferenceWasReleased)
    {
        Fixture f;

        auto device = f.SharedDeviceState->GetSharedDevice(false);
        Assert::IsNotNull(device.Get());
        device.Reset();

        auto device2 = f.SharedDeviceState->GetSharedDevice(false);
        Assert::IsNotNull(device2.Get());
    }

    TEST_METHOD_EX(CanvasGetSharedDeviceTests_ManagerReleasesAllReferences)
    {
        WeakRef weakDevices[2];
        {
            Fixture f;

            auto d1 = f.SharedDeviceState->GetSharedDevice(false);
            ThrowIfFailed(AsWeak(d1.Get(), &weakDevices[0]));

            auto d2 = f.SharedDeviceState->GetSharedDevice(true);
            ThrowIfFailed(AsWeak(d2.Get(), &weakDevices[1]));

            Assert::IsTrue(IsWeakRefValid(weakDevices[0]));
            Assert::IsTrue(IsWeakRefValid(weakDevices[1]));
        }
        Assert::IsFalse(IsWeakRefValid(weakDevices[0]));
        Assert::IsFalse(IsWeakRefValid(weakDevices[1]));
    }

    TEST_METHOD_EX(CanvasDevice_EntersLockWhenCallingDxgiMethods)
    {
        Fixture f;
        auto device = CanvasDevice::CreateNew(false);

        auto d2dDevice = As<ICanvasDeviceInternal>(device)->GetD2DDevice();

        ComPtr<ID2D1Factory> d2dFactory;
        d2dDevice->GetFactory(&d2dFactory);
        auto mockFactory = static_cast<MockD2DFactory*>(d2dFactory.Get());

        // Trim should also call the D2D ClearResources method.
        static_cast<MockD2DDevice*>(d2dDevice.Get())->ClearResourcesMethod.SetExpectedCalls(1);

        // Creating the device should have taken the lock once, while enumerating DXGI outputs.
        Assert::AreEqual(1, mockFactory->GetEnterCount());
        Assert::AreEqual(1, mockFactory->GetLeaveCount());

        // Calling the DXGI Trim() method should lock a second time.
        device->Trim();

        Assert::AreEqual(2, mockFactory->GetEnterCount());
        Assert::AreEqual(2, mockFactory->GetLeaveCount());
    }

    struct LockFixture : public Fixture
    {
        ComPtr<CanvasDevice> Device;
        ComPtr<MockD2DFactory> D2DFactory;

        int InitialEnterCount;
        int InitialLeaveCount;
        
        LockFixture()
        {
            Device = CanvasDevice::CreateNew(false);

            ComPtr<ID2D1Factory> d2dFactory;
            As<ICanvasDeviceInternal>(Device)->GetD2DDevice()->GetFactory(&d2dFactory);

            D2DFactory = static_cast<MockD2DFactory*>(d2dFactory.Get());
            
            InitialEnterCount = D2DFactory->GetEnterCount();
            InitialLeaveCount = D2DFactory->GetLeaveCount();
        }

        void ValidateEnterLeaveCount(int enter, int leave)
        {
            Assert::AreEqual(InitialEnterCount + enter, D2DFactory->GetEnterCount());
            Assert::AreEqual(InitialLeaveCount + leave, D2DFactory->GetLeaveCount());
        }
    };

    TEST_METHOD_EX(CanvasDevice_Lock_Closed_CallsEnterAndLeave)
    {
        LockFixture f;
        
        ComPtr<ICanvasLock> canvasLock;
        ThrowIfFailed(f.Device->Lock(&canvasLock));

        f.ValidateEnterLeaveCount(1, 0);

        // any number of Closes is fine
        ThrowIfFailed(As<IClosable>(canvasLock)->Close());
        ThrowIfFailed(As<IClosable>(canvasLock)->Close());
        ThrowIfFailed(As<IClosable>(canvasLock)->Close());
        ThrowIfFailed(As<IClosable>(canvasLock)->Close());

        f.ValidateEnterLeaveCount(1, 1);
    }

    TEST_METHOD_EX(CanvasDevice_Lock_Released_CallsEnterAndLeave)
    {
        LockFixture f;
        
        ComPtr<ICanvasLock> canvasLock;
        ThrowIfFailed(f.Device->Lock(&canvasLock));

        f.ValidateEnterLeaveCount(1, 0);

        canvasLock.Reset();

        f.ValidateEnterLeaveCount(1, 1);
    }
};
