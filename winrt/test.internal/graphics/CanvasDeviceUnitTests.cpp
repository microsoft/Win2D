// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

class Fixture
{
public:
    std::shared_ptr<TestDeviceResourceCreationAdapter> ResourceCreationAdapter;
    std::shared_ptr<CanvasDeviceManager> Manager;

    Fixture()
        : ResourceCreationAdapter(std::make_shared<TestDeviceResourceCreationAdapter>())
        , Manager(std::make_shared<CanvasDeviceManager>(ResourceCreationAdapter))
    {
    }

    void AssertDeviceManagerRoundtrip(ICanvasDevice* expectedCanvasDevice)
    {
        ComPtr<ICanvasDeviceInternal> canvasDeviceInternal;
        ThrowIfFailed(expectedCanvasDevice->QueryInterface(canvasDeviceInternal.GetAddressOf()));

        auto d2dDevice = canvasDeviceInternal->GetD2DDevice();
        auto actualCanvasDevice = Manager->GetOrCreate(d2dDevice.Get());

        Assert::AreEqual<ICanvasDevice*>(expectedCanvasDevice, actualCanvasDevice.Get());
    }
};

TEST_CLASS(CanvasDeviceTests)
{
public:

    TEST_METHOD_EX(CanvasDevice_Implements_Expected_Interfaces)
    {
        Fixture f;
        auto canvasDevice = f.Manager->Create(false);

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
            auto canvasDevice = f.Manager->Create(
                !!expectedForceSoftwareRenderer);

            Assert::IsNotNull(canvasDevice.Get());

            // Verify ForceSoftwareRenderer property getter returns the right thing
            boolean forceSoftwareRendererActual;
            canvasDevice->get_ForceSoftwareRenderer(&forceSoftwareRendererActual);
            Assert::AreEqual(expectedForceSoftwareRenderer, forceSoftwareRendererActual);

            // Ensure that the getter returns E_INVALIDARG with null ptr.
            Assert::AreEqual(E_INVALIDARG, canvasDevice->get_ForceSoftwareRenderer(nullptr));

            Assert::AreEqual(1, f.ResourceCreationAdapter->m_numD2DFactoryCreationCalls);

            f.AssertDeviceManagerRoundtrip(canvasDevice.Get());
        }
    }

    TEST_METHOD_EX(CanvasDevice_Create_With_Specific_Direct3DDevice)
    {
        ComPtr<StubD3D11Device> stubD3D11Device = Make<StubD3D11Device>();

        ComPtr<IDirect3DDevice> stubDirect3DDevice;
        ThrowIfFailed(CreateDirect3D11DeviceFromDXGIDevice(stubD3D11Device.Get(), &stubDirect3DDevice));

        Fixture f;
        auto canvasDevice = f.Manager->Create(
            stubDirect3DDevice.Get());
        Assert::IsNotNull(canvasDevice.Get());

        // A D2D device should still have been created
        Assert::AreEqual(1, f.ResourceCreationAdapter->m_numD2DFactoryCreationCalls);

        // But not a D3D device.
        Assert::AreEqual(0, f.ResourceCreationAdapter->m_numD3dDeviceCreationCalls);

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
            [&] { f.Manager->Create(nullptr); });
    }

    TEST_METHOD_EX(CanvasDevice_Create_From_D2DDevice)
    {
        auto d2dDevice = Make<MockD2DDevice>(Make<MockD2DFactory>().Get());

        Fixture f;
        auto canvasDevice = f.Manager->GetOrCreate(d2dDevice.Get());
        Assert::IsNotNull(canvasDevice.Get());

        // Nothing should have been created
        Assert::AreEqual(0, f.ResourceCreationAdapter->m_numD2DFactoryCreationCalls);
        Assert::AreEqual(0, f.ResourceCreationAdapter->m_numD3dDeviceCreationCalls);

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
        auto canvasDevice = f.Manager->Create(false);
        Assert::IsNotNull(canvasDevice.Get());

        Assert::AreEqual(S_OK, canvasDevice->Close());

        ComPtr<IDXGIDevice> dxgiDevice;
        Assert::AreEqual(RO_E_CLOSED, canvasDevice->GetInterface(IID_PPV_ARGS(&dxgiDevice)));

        boolean forceSoftwareRenderer;
        Assert::AreEqual(RO_E_CLOSED, canvasDevice->get_ForceSoftwareRenderer(&forceSoftwareRenderer));
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
        auto canvasDevice = f.Manager->Create(
            false);
        d3dDeviceCreationCount++;

        Assert::IsFalse(f.ResourceCreationAdapter->m_retrievableForceSoftwareRenderer);
        Assert::AreEqual(d3dDeviceCreationCount, f.ResourceCreationAdapter->m_numD3dDeviceCreationCalls);

        // Now disable the hardware path.
        f.ResourceCreationAdapter->SetHardwareEnabled(false);

        {
            // Ensure the fallback works.
            canvasDevice = f.Manager->Create(
                false);
            d3dDeviceCreationCount++;

            // Ensure the software path was used.
            Assert::IsTrue(f.ResourceCreationAdapter->m_retrievableForceSoftwareRenderer);
            Assert::AreEqual(d3dDeviceCreationCount, f.ResourceCreationAdapter->m_numD3dDeviceCreationCalls);

            // Ensure the ForceSoftwareRenderer property getter returns the right thing.
            boolean forceSoftwareRenderer;
            canvasDevice->get_ForceSoftwareRenderer(&forceSoftwareRenderer);
            Assert::IsFalse(!!forceSoftwareRenderer);
        }
        {
            // Re-create another whole device with the hardware path on, ensuring there isn't some weird statefulness problem.
            f.ResourceCreationAdapter->SetHardwareEnabled(true);
            canvasDevice = f.Manager->Create(
                false);
            d3dDeviceCreationCount++;

            // Ensure the hardware path was enabled.
            Assert::IsFalse(f.ResourceCreationAdapter->m_retrievableForceSoftwareRenderer);
            Assert::AreEqual(d3dDeviceCreationCount, f.ResourceCreationAdapter->m_numD3dDeviceCreationCalls);

            // Ensure the ForceSoftwareRenderer property is still correct.
            boolean forceSoftwareRenderer;
            canvasDevice->get_ForceSoftwareRenderer(&forceSoftwareRenderer);
            Assert::IsFalse(!!forceSoftwareRenderer);
        }
    }

    TEST_METHOD_EX(CanvasDeviceManager_Create_GetOrCreate_Returns_Same_Instance)
    {
        Fixture f;

        auto expectedCanvasDevice = f.Manager->Create(false);

        //
        // Create, followed by GetOrCreate on the same d2d device should give
        // back the same CanvasDevice.
        //

        auto d2dDevice = expectedCanvasDevice->GetD2DDevice();

        auto actualCanvasDevice = f.Manager->GetOrCreate(d2dDevice.Get());

        Assert::AreEqual<ICanvasDevice*>(expectedCanvasDevice.Get(), actualCanvasDevice.Get());

        //
        // Destroying these original CanvasDevices, and the GetOrCreate using
        // the same d2d device should give back a new, different, CanvasDevice.
        //

        WeakRef weakExpectedCanvasDevice;
        ThrowIfFailed(AsWeak(expectedCanvasDevice.Get(), &weakExpectedCanvasDevice));
        expectedCanvasDevice.Reset();
        actualCanvasDevice.Reset();

        actualCanvasDevice = f.Manager->GetOrCreate(d2dDevice.Get());

        ComPtr<ICanvasDevice> unexpectedCanvasDevice;
        weakExpectedCanvasDevice.As(&unexpectedCanvasDevice);

        Assert::AreNotEqual<ICanvasDevice*>(unexpectedCanvasDevice.Get(), actualCanvasDevice.Get());
    }

    TEST_METHOD_EX(CanvasDevice_DeviceProperty)
    {
        Fixture f;
        auto device = f.Manager->Create(false);

        Assert::AreEqual(E_INVALIDARG, device->get_Device(nullptr));

        ComPtr<ICanvasDevice> deviceVerify;
        ThrowIfFailed(device->get_Device(&deviceVerify));
        Assert::AreEqual(static_cast<ICanvasDevice*>(device.Get()), deviceVerify.Get());
    }
    
    TEST_METHOD_EX(CanvasDevice_MaximumBitmapSize_NullArg)
    {
        Fixture f;
        auto canvasDevice = f.Manager->Create(false);

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
        auto canvasDevice = f.Manager->GetOrCreate(d2dDevice.Get());

        int32_t maximumBitmapSize;
        ThrowIfFailed(canvasDevice->get_MaximumBitmapSizeInPixels(&maximumBitmapSize));

        Assert::AreEqual(someSize, maximumBitmapSize);
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
        auto canvasDevice = f.Manager->GetOrCreate(d2dDevice.Get());
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

        auto canvasDevice = f.Manager->GetOrCreate(d2dDevice.Get());
        auto actualBitmap = canvasDevice->CreateRenderTargetBitmap(anyWidth, anyHeight, anyDpi, anyFormat, anyAlphaMode);

        Assert::IsTrue(IsSameInstance(d2dBitmap.Get(), actualBitmap.Get()));
    }
};

TEST_CLASS(DefaultDeviceResourceCreationAdapterTests)
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
        DefaultDeviceResourceCreationAdapter adapter;

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
        auto canvasDevice = f.Manager->Create(false);

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
        auto canvasDevice = f.Manager->Create(false);

        EventRegistrationToken token{};
        MockEventHandler<DeviceLostHandlerType> dummyDeviceLostHandler(L"DeviceLost");
        Assert::AreEqual(E_INVALIDARG, canvasDevice->add_DeviceLost(nullptr, &token));
        Assert::AreEqual(E_INVALIDARG, canvasDevice->add_DeviceLost(dummyDeviceLostHandler.Get(), nullptr));
        Assert::AreEqual(E_INVALIDARG, canvasDevice->IsDeviceLost(0, nullptr));
    }

    class DeviceLostResourceCreationAdapter : public TestDeviceResourceCreationAdapter
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
        std::shared_ptr<DeviceLostResourceCreationAdapter> m_resourceCreationAdapter;

    public:
        std::shared_ptr<CanvasDeviceManager> DeviceManager;

        DeviceLostFixture()
            : m_resourceCreationAdapter(std::make_shared<DeviceLostResourceCreationAdapter>())
            , DeviceManager(std::make_shared<CanvasDeviceManager>(m_resourceCreationAdapter))
        {
        }
    };

    TEST_METHOD_EX(CanvasDeviceLostTests_IsDeviceLost_DeviceRemovedHr_DeviceIsLost_ReturnsTrue)
    {
        DeviceLostFixture f;
        auto canvasDevice = f.DeviceManager->Create(false);

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
        auto canvasDevice = f.DeviceManager->Create(false);

        boolean isDeviceLost;
        Assert::AreEqual(S_OK, canvasDevice->IsDeviceLost(E_INVALIDARG, &isDeviceLost));
        Assert::IsFalse(!!isDeviceLost);
    }

    TEST_METHOD_EX(CanvasDeviceLostTests_IsDeviceLost_DeviceRemovedHr_DeviceNotActuallyLost_ReturnsFalse)
    {
        Fixture f;
        auto canvasDevice = f.Manager->Create(false);

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
        auto canvasDevice = f.DeviceManager->Create(false);

        boolean isDeviceLost;
        Assert::AreEqual(S_OK, canvasDevice->IsDeviceLost(E_INVALIDARG, &isDeviceLost));
        Assert::IsFalse(!!isDeviceLost);
    }

    TEST_METHOD_EX(CanvasDeviceLostTests_RaiseDeviceLost_RaisesSubscribedHandlers_DeviceActuallyLost)
    {
        DeviceLostFixture f;
        auto canvasDevice = f.DeviceManager->Create(false);

        MockEventHandler<DeviceLostHandlerType> deviceLostHandler(L"DeviceLost");
        deviceLostHandler.SetExpectedCalls(1);

        EventRegistrationToken token{};
        Assert::AreEqual(S_OK, canvasDevice->add_DeviceLost(deviceLostHandler.Get(), &token));

        Assert::AreEqual(S_OK, canvasDevice->RaiseDeviceLost());
    }

    TEST_METHOD_EX(CanvasDeviceLostTests_RemoveEventThen_RaiseDeviceLost_DoesNotInvokeHandler)
    {
        DeviceLostFixture f;
        auto canvasDevice = f.DeviceManager->Create(false);

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
        auto canvasDevice = f.DeviceManager->Create(false);

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
        auto canvasDevice = f.DeviceManager->Create(false);

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

    class D3DDeviceAccessAdapter : public TestDeviceResourceCreationAdapter
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
        auto manager = std::make_shared<CanvasDeviceManager>(adapter);

        //
        // The default mock for GetDeviceRemovedReason returns S_OK.
        //
        adapter->D3DDevice->GetDeviceRemovedReasonMethod.ExpectAtLeastOneCall();

        auto device = manager->GetSharedDevice(false);

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
            ResourceCreationAdapter->GetCoreApplicationMethod.SetExpectedCalls(1,
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
            Assert::AreEqual(expected, ResourceCreationAdapter->m_debugLevel);
        }

        void AssertD3DDebugLevel(CanvasDebugLevel expected)
        {
            const bool expectD3DDebugLevel = expected != CanvasDebugLevel::None;

            Assert::AreEqual(expectD3DDebugLevel, ResourceCreationAdapter->m_retrievableUseDebugD3DDevice);
        }

        void AssertNoD3DDebugLevel()
        {
            Assert::IsFalse(ResourceCreationAdapter->m_retrievableUseDebugD3DDevice);
        }
    };

    TEST_METHOD_EX(CreateWithForceSoftwareRendererOption_HasCorrectDebugLevel)
    {
        for (auto debugLevel : allDebugLevels)
        {
            DebugLevelFixture f;

            f.Manager->SetDebugLevel(debugLevel);

            f.ExpectOneQueryDebugLevel(debugLevel);
            auto canvasDevice = f.Manager->Create(false);

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

            f.Manager->SetDebugLevel(debugLevel);

            f.ExpectOneQueryDebugLevel(debugLevel);
            auto canvasDevice = f.Manager->Create(stubDirect3DDevice.Get());
            
            f.AssertDebugLevel(debugLevel);
            f.AssertNoD3DDebugLevel(); // We don't own creating a D3D device in this path
        }
    }

    TEST_METHOD_EX(GetSharedDevice_HasCorrectDebugLevel)
    {
        for (auto debugLevel : allDebugLevels)
        {
            DebugLevelFixture f;

            f.Manager->SetDebugLevel(debugLevel);

            f.ExpectOneQueryDebugLevel(debugLevel);

            auto canvasDevice = f.Manager->GetSharedDevice(false);

            f.AssertDebugLevel(debugLevel);
            f.AssertD3DDebugLevel(debugLevel);
        }
    }

    TEST_METHOD_EX(GetSharedDevice_ReturnsExistingDevice_EvenIfDebugLevelDoesntMatch)
    {
        DebugLevelFixture f;

        f.Manager->SetDebugLevel(CanvasDebugLevel::Information);
        auto canvasDevice1 = f.Manager->GetSharedDevice(false);

        f.Manager->SetDebugLevel(CanvasDebugLevel::Warning);
        auto canvasDevice2 = f.Manager->GetSharedDevice(false);

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

    class GetSharedDevice_Adapter : public TestDeviceResourceCreationAdapter
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
        std::shared_ptr<CanvasDeviceManager> Manager;

        Fixture()
            : Adapter(std::make_shared<GetSharedDevice_Adapter>())
            , Manager(std::make_shared<CanvasDeviceManager>(Adapter))
        {
        }
    };

    TEST_METHOD_EX(CanvasGetSharedDeviceTests_NullArg)
    {
        auto canvasDeviceFactory = Make<CanvasDeviceFactory>();

        Assert::AreEqual(E_INVALIDARG, canvasDeviceFactory->GetSharedDevice(false, nullptr));
    }

    ComPtr<ICanvasDevice> GetSharedDevice_ExpectForceSoftwareRenderer(
        Fixture& f,
        bool passedInAndExpectedBack)
    {
        ComPtr<ICanvasDevice> device = f.Manager->GetSharedDevice(passedInAndExpectedBack);

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

        ExpectHResultException(E_FAIL, [&]{ f.Manager->GetSharedDevice(false); });
    }

    TEST_METHOD_EX(CanvasGetSharedDeviceTests_ExistingDevice_Lost_RaisesEvent)
    {
        Fixture f;

        auto d3dDevice = Make<StubD3D11Device>();
        f.Adapter->CreateStubD3D11DeviceMethod.AllowAnyCall([&](){ return d3dDevice; });

        auto device = f.Manager->GetSharedDevice(false);

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
        f.Manager->GetSharedDevice(false);
    }

    TEST_METHOD_EX(CanvasGetSharedDeviceTests_ExistingDevice_LastDeviceReferenceWasReleased)
    {
        Fixture f;

        auto device = f.Manager->GetSharedDevice(false);
        Assert::IsNotNull(device.Get());
        device.Reset();

        auto device2 = f.Manager->GetSharedDevice(false);
        Assert::IsNotNull(device2.Get());
    }

    TEST_METHOD_EX(CanvasGetSharedDeviceTests_ManagerReleasesAllReferences)
    {
        WeakRef weakDevices[2];
        {
            Fixture f;

            auto d1 = f.Manager->GetSharedDevice(false);
            ThrowIfFailed(AsWeak(d1.Get(), &weakDevices[0]));

            auto d2 = f.Manager->GetSharedDevice(true);
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
        auto device = f.Manager->Create(false);

        ComPtr<ID2D1Factory> d2dFactory;
        As<ICanvasDeviceInternal>(device)->GetD2DDevice()->GetFactory(&d2dFactory);
        auto mockFactory = static_cast<MockD2DFactory*>(d2dFactory.Get());

        // Creating the device should have taken the lock once, while enumerating DXGI outputs.
        Assert::AreEqual(1, mockFactory->GetEnterCount());
        Assert::AreEqual(1, mockFactory->GetLeaveCount());

        // Calling the DXGI Trim() method should lock a second time.
        device->Trim();

        Assert::AreEqual(2, mockFactory->GetEnterCount());
        Assert::AreEqual(2, mockFactory->GetLeaveCount());
    }
};
