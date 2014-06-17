// Copyright (c) Microsoft Corporation.  All rights reserved

#include "pch.h"

#include "TestDeviceResourceCreationAdapter.h"

TEST_CLASS(CanvasDeviceTests)
{
public:
    std::shared_ptr<TestDeviceResourceCreationAdapter> m_resourceCreationAdapter;
    std::shared_ptr<CanvasDeviceManager> m_deviceManager;

    TEST_METHOD_INITIALIZE(Reset)
    {
        m_resourceCreationAdapter = std::make_shared<TestDeviceResourceCreationAdapter>();
        m_deviceManager = std::make_shared<CanvasDeviceManager>(m_resourceCreationAdapter);
    }


    void AssertDeviceManagerRoundtrip(ICanvasDevice* expectedCanvasDevice)
    {
        ComPtr<ICanvasDeviceInternal> canvasDeviceInternal;
        ThrowIfFailed(expectedCanvasDevice->QueryInterface(canvasDeviceInternal.GetAddressOf()));

        auto d2dDevice = canvasDeviceInternal->GetD2DDevice();
        auto actualCanvasDevice = m_deviceManager->GetOrCreate(d2dDevice.Get());
        
        Assert::AreEqual<ICanvasDevice*>(expectedCanvasDevice, actualCanvasDevice.Get());
    }


    CanvasHardwareAcceleration GetRequestedHardwareAcceleration(const ComPtr<ICanvasDevice>& canvasDevice)
    {
        ComPtr<ICanvasDeviceInternal> canvasDeviceInternal;
        Assert::AreEqual(S_OK, canvasDevice.As(&canvasDeviceInternal));
        return canvasDeviceInternal->GetRequestedHardwareAcceleration();
    }

    TEST_METHOD(CanvasDevice_Implements_Expected_Interfaces)
    {
        auto canvasDevice = m_deviceManager->Create(CanvasDebugLevel::None, CanvasHardwareAcceleration::Auto);

        ASSERT_IMPLEMENTS_INTERFACE(canvasDevice, ICanvasDevice);
        ASSERT_IMPLEMENTS_INTERFACE(canvasDevice, ABI::Windows::Foundation::IClosable);
        ASSERT_IMPLEMENTS_INTERFACE(canvasDevice, ICanvasResourceWrapperNative);
        ASSERT_IMPLEMENTS_INTERFACE(canvasDevice, ICanvasDeviceInternal);
    }

    TEST_METHOD(CanvasDevice_Defaults_Roundtrip)
    {
        auto canvasDevice = m_deviceManager->Create(CanvasDebugLevel::None, CanvasHardwareAcceleration::Auto);
        Assert::IsNotNull(canvasDevice.Get());

        Assert::AreEqual(1, m_resourceCreationAdapter->m_numD2DFactoryCreationCalls);
        Assert::AreEqual(CanvasDebugLevel::None, m_resourceCreationAdapter->m_debugLevel);
        Assert::AreEqual(1, m_resourceCreationAdapter->m_numD3dDeviceCreationCalls);
        Assert::AreEqual(CanvasHardwareAcceleration::On, m_resourceCreationAdapter->m_retrievableHarwareAcceleration); // Hardware is the default, and should be used here.
        Assert::AreEqual(CanvasHardwareAcceleration::Auto, GetRequestedHardwareAcceleration(canvasDevice));

        AssertDeviceManagerRoundtrip(canvasDevice.Get());
    }

    TEST_METHOD(CanvasDevice_DebugLevels)
    {
        CanvasDebugLevel cases[] = { CanvasDebugLevel::None, CanvasDebugLevel::Error, CanvasDebugLevel::Warning, CanvasDebugLevel::Information };
        for (auto expectedDebugLevel : cases)
        {
            Reset();

            auto canvasDevice = m_deviceManager->Create(expectedDebugLevel, CanvasHardwareAcceleration::Auto);
            Assert::IsNotNull(canvasDevice.Get());

            Assert::AreEqual(1, m_resourceCreationAdapter->m_numD3dDeviceCreationCalls);
            Assert::AreEqual(expectedDebugLevel, m_resourceCreationAdapter->m_debugLevel);
            AssertDeviceManagerRoundtrip(canvasDevice.Get());
        }

        // Try an invalid debug level
        Reset();
        Assert::ExpectException<InvalidArgException>(
            [&]()
            {
                m_deviceManager->Create(
                    static_cast<CanvasDebugLevel>(1234),
                    CanvasHardwareAcceleration::Auto);
            });
    }

    TEST_METHOD(CanvasDevice_HardwareAcceleration)
    {
        CanvasHardwareAcceleration cases[] = { CanvasHardwareAcceleration::On, CanvasHardwareAcceleration::Off };
        for (auto expectedHardwareAcceleration : cases)
        {
            Reset();
            
            auto canvasDevice = m_deviceManager->Create(
                CanvasDebugLevel::Information, 
                expectedHardwareAcceleration);

            Assert::IsNotNull(canvasDevice.Get());

            // Verify HW acceleration property getter returns the right thing
            CanvasHardwareAcceleration hardwareAccelerationActual;
            canvasDevice->get_HardwareAcceleration(&hardwareAccelerationActual);
            Assert::AreEqual(expectedHardwareAcceleration, hardwareAccelerationActual);

            // Verify the roundtrip field was stored correctly
            Assert::AreEqual(expectedHardwareAcceleration, GetRequestedHardwareAcceleration(canvasDevice));

            // Ensure that the getter returns E_INVALIDARG with null ptr.
            Assert::AreEqual(E_INVALIDARG, canvasDevice->get_HardwareAcceleration(NULL));

            Assert::AreEqual(1, m_resourceCreationAdapter->m_numD2DFactoryCreationCalls);
            Assert::AreEqual(CanvasDebugLevel::Information, m_resourceCreationAdapter->m_debugLevel);
            AssertDeviceManagerRoundtrip(canvasDevice.Get());
        }

        // Try some invalid options
        Reset();

        CanvasHardwareAcceleration invalidCases[] = 
            {
                CanvasHardwareAcceleration::Unknown,
                static_cast<CanvasHardwareAcceleration>(0x5678)
            };

        for (auto invalidCase : invalidCases)
        {
            Assert::ExpectException<InvalidArgException>(
                [&]() 
                { 
                    m_deviceManager->Create(
                        CanvasDebugLevel::None,
                        invalidCase);
                });
        }
    }

    TEST_METHOD(CanvasDevice_Create_With_Specific_DirectX11Device)
    {
        using canvas::DirectX11Device;

        ComPtr<MockD3D11Device> mockD3D11Device = Make<MockD3D11Device>();
        ComPtr<IDirectX11Device> stubDirectX11Device = Make<DirectX11Device>(mockD3D11Device.Get());

        auto canvasDevice = m_deviceManager->Create(
            CanvasDebugLevel::None, 
            stubDirectX11Device.Get());
        Assert::IsNotNull(canvasDevice.Get());

        // A D2D device should still have been created
        Assert::AreEqual(1, m_resourceCreationAdapter->m_numD2DFactoryCreationCalls);
        Assert::AreEqual(CanvasDebugLevel::None, m_resourceCreationAdapter->m_debugLevel);

        // But not a D3D device.
        Assert::AreEqual(0, m_resourceCreationAdapter->m_numD3dDeviceCreationCalls);

        AssertDeviceManagerRoundtrip(canvasDevice.Get());

        CanvasHardwareAcceleration actualHardwareAcceleration{};
        ThrowIfFailed(canvasDevice->get_HardwareAcceleration(&actualHardwareAcceleration));
        Assert::AreEqual(CanvasHardwareAcceleration::Unknown, actualHardwareAcceleration);

        // RecoverLostDevice can't succeed because CanvasDevice doesn't know how
        // to recreate the underlying DXGI device
        ComPtr<ICanvasDevice> recoveredDevice;
        Assert::AreEqual(E_INVALIDARG, canvasDevice->RecoverLostDevice(&recoveredDevice));

        // Try a NULL DirectX11Device. 
        Assert::ExpectException<InvalidArgException>(
            [&]() { m_deviceManager->Create(CanvasDebugLevel::None, nullptr); });
    }

    TEST_METHOD(CanvasDevice_Create_From_D2DDevice)
    {
        auto d2dDevice = Make<MockD2DDevice>(Make<MockD2DFactory>().Get());

        auto canvasDevice = m_deviceManager->GetOrCreate(d2dDevice.Get());
        Assert::IsNotNull(canvasDevice.Get());

        // Nothing should have been created
        Assert::AreEqual(0, m_resourceCreationAdapter->m_numD2DFactoryCreationCalls);
        Assert::AreEqual(0, m_resourceCreationAdapter->m_numD3dDeviceCreationCalls);

        AssertDeviceManagerRoundtrip(canvasDevice.Get());

        CanvasHardwareAcceleration actualHardwareAcceleration{};
        ThrowIfFailed(canvasDevice->get_HardwareAcceleration(&actualHardwareAcceleration));
        Assert::AreEqual(CanvasHardwareAcceleration::Unknown, actualHardwareAcceleration);

        // RecoverLostDevice can't succeed because CanvasDevice doesn't know how
        // to recreate the underlying DXGI device
        ComPtr<ICanvasDevice> recoveredDevice;
        Assert::AreEqual(E_INVALIDARG, canvasDevice->RecoverLostDevice(&recoveredDevice));
    }

    TEST_METHOD(CanvasDevice_Closed)
    {
        using canvas::CanvasDevice;
        using canvas::DirectX11Device;

        auto canvasDevice = m_deviceManager->Create(CanvasDebugLevel::None, CanvasHardwareAcceleration::On);
        Assert::IsNotNull(canvasDevice.Get());

        Assert::AreEqual(S_OK, canvasDevice->Close());

        IDirectX11Device* deviceActual = reinterpret_cast<IDirectX11Device*>(0x1234);
        Assert::AreEqual(RO_E_CLOSED, canvasDevice->get_DirectX11Device(&deviceActual));
        Assert::IsNull(deviceActual);

        CanvasHardwareAcceleration hardwareAccelerationActual = static_cast<CanvasHardwareAcceleration>(1);
        Assert::AreEqual(RO_E_CLOSED, canvasDevice->get_HardwareAcceleration(&hardwareAccelerationActual));

        {
            ComPtr<ICanvasDevice> recoveredDevice;
            Assert::AreEqual(RO_E_CLOSED, canvasDevice->RecoverLostDevice(&recoveredDevice));
            Assert::IsNull(recoveredDevice.Get());
        }

        ComPtr<MockD3D11Device> mockD3D11Device = Make<MockD3D11Device>();
        ComPtr<IDirectX11Device> stubDirectX11Device = Make<canvas::DirectX11Device>(mockD3D11Device.Get());
        {
            ComPtr<ICanvasDevice> compatibleDevice;
            Assert::AreEqual(RO_E_CLOSED, canvasDevice->CreateCompatibleDevice(stubDirectX11Device.Get(), &compatibleDevice));
            Assert::IsNull(compatibleDevice.Get());
        }
    }

    ComPtr<ID2D1Device1> GetD2DDevice(const ComPtr<ICanvasDevice>& canvasDevice)
    {
        ComPtr<ICanvasDeviceInternal> canvasDeviceInternal;
        Assert::AreEqual(S_OK, canvasDevice.As(&canvasDeviceInternal));
        return canvasDeviceInternal->GetD2DDevice();
    }

    void VerifyCompatibleDevices(const ComPtr<ICanvasDevice>& canvasDevice, const ComPtr<ICanvasDevice>& compatibleDevice)
    {
        ComPtr<ID2D1Device1> canvasD2DDevice = GetD2DDevice(canvasDevice);
        ComPtr<ID2D1Device1> recoveredD2DDevice = GetD2DDevice(compatibleDevice);

        // Ensure that the original device and the recovered device have different D2D devices.
        Assert::AreNotEqual(canvasD2DDevice.Get(), recoveredD2DDevice.Get());

        // Verify the higher-level DirectX11Devices are different, as well.
        ComPtr<IDirectX11Device> canvasDirectX11Device;
        ComPtr<IDirectX11Device> compatibleDirectX11Device;
        canvasDevice->get_DirectX11Device(&canvasDirectX11Device);
        compatibleDevice->get_DirectX11Device(&compatibleDirectX11Device);
        Assert::AreNotEqual(canvasDirectX11Device.Get(), compatibleDirectX11Device.Get());

        // Ensure the original device and recovered device have the same D2D factory.   
        ComPtr<ID2D1Factory> canvasD2DFactory;
        ComPtr<ID2D1Factory> recoveredD2DFactory;
        canvasD2DDevice->GetFactory(&canvasD2DFactory);
        recoveredD2DDevice->GetFactory(&recoveredD2DFactory);
        Assert::AreEqual(canvasD2DFactory.Get(), recoveredD2DFactory.Get());
    }

    TEST_METHOD(CanvasDevice_RecoverLostDevice)
    {
        auto canvasDevice = m_deviceManager->Create(CanvasDebugLevel::None, CanvasHardwareAcceleration::On);

        ComPtr<ICanvasDevice> recoveredDevice;
        Assert::AreEqual(S_OK, canvasDevice->RecoverLostDevice(&recoveredDevice));

        VerifyCompatibleDevices(canvasDevice, recoveredDevice);
        AssertDeviceManagerRoundtrip(recoveredDevice.Get());
    }

    TEST_METHOD(CanvasDevice_CreateCompatibleDevice)
    {
        using canvas::DirectX11Device;

        auto canvasDevice = m_deviceManager->Create(CanvasDebugLevel::None, CanvasHardwareAcceleration::On);
        ComPtr<ICanvasDevice> compatibleDevice;
            
        ComPtr<MockD3D11Device> mockD3D11Device = Make<MockD3D11Device>();
        ComPtr<IDirectX11Device> stubDirectX11Device = Make<DirectX11Device>(mockD3D11Device.Get());

        Assert::AreEqual(S_OK, canvasDevice->CreateCompatibleDevice(stubDirectX11Device.Get(), &compatibleDevice));

        VerifyCompatibleDevices(canvasDevice, compatibleDevice);
        AssertDeviceManagerRoundtrip(compatibleDevice.Get());

        // Additionally, verify that the compatible device has the correct original DirectX11Device.
        ComPtr<IDirectX11Device> deviceActual;
        Assert::AreEqual(S_OK, compatibleDevice->get_DirectX11Device(&deviceActual));
        Assert::AreEqual(static_cast<IDirectX11Device*>(stubDirectX11Device.Get()), deviceActual.Get());
    }

    TEST_METHOD(CanvasDevice_HwSwFallback)
    {
        Reset();

        using canvas::CanvasDevice;

        int d3dDeviceCreationCount = 0;

        // Default canvas device should be hardware.
        auto canvasDevice = m_deviceManager->Create(
            CanvasDebugLevel::None, 
            CanvasHardwareAcceleration::Auto);
        d3dDeviceCreationCount++;

        Assert::AreEqual(CanvasHardwareAcceleration::On, m_resourceCreationAdapter->m_retrievableHarwareAcceleration);
        Assert::AreEqual(CanvasHardwareAcceleration::Auto, GetRequestedHardwareAcceleration(canvasDevice));
        Assert::AreEqual(d3dDeviceCreationCount, m_resourceCreationAdapter->m_numD3dDeviceCreationCalls);

        // Now disable the hardware path.
        m_resourceCreationAdapter->SetHardwareEnabled(false);

        {
            // Ensure the fallback works.
            auto canvasDevice = m_deviceManager->Create(
                CanvasDebugLevel::None,
                CanvasHardwareAcceleration::Auto);
            d3dDeviceCreationCount++;

            // Ensure the software path was used.
            Assert::AreEqual(CanvasHardwareAcceleration::Off, m_resourceCreationAdapter->m_retrievableHarwareAcceleration);
            Assert::AreEqual(CanvasHardwareAcceleration::Auto, GetRequestedHardwareAcceleration(canvasDevice));
            Assert::AreEqual(d3dDeviceCreationCount, m_resourceCreationAdapter->m_numD3dDeviceCreationCalls);

            // Ensure the HardwareAcceleration property getter returns the right thing.
            CanvasHardwareAcceleration actualHardwareAcceleration;
            canvasDevice->get_HardwareAcceleration(&actualHardwareAcceleration);
            Assert::AreEqual(CanvasHardwareAcceleration::Off, actualHardwareAcceleration);
        }

        // Ensure RecoverLostDevice will yield another software device, and that it has the correct properties.
        {
            ComPtr<ICanvasDevice> recoveredSoftwareDevice;
            ThrowIfFailed(canvasDevice->RecoverLostDevice(&recoveredSoftwareDevice));
            d3dDeviceCreationCount++;
            Assert::AreEqual(d3dDeviceCreationCount, m_resourceCreationAdapter->m_numD3dDeviceCreationCalls);
            Assert::AreEqual(CanvasHardwareAcceleration::Off, m_resourceCreationAdapter->m_retrievableHarwareAcceleration);
            Assert::AreEqual(CanvasHardwareAcceleration::Auto, GetRequestedHardwareAcceleration(recoveredSoftwareDevice));

            CanvasHardwareAcceleration actualHardwareAcceleration;
            recoveredSoftwareDevice->get_HardwareAcceleration(&actualHardwareAcceleration);
            Assert::AreEqual(CanvasHardwareAcceleration::Off, actualHardwareAcceleration);
        }

        // Re-enable the hardware path, and do another RecoverLostDevice. 
        m_resourceCreationAdapter->SetHardwareEnabled(true);
        {
            ComPtr<ICanvasDevice> recoveredHardwareDevice;
            ThrowIfFailed(canvasDevice->RecoverLostDevice(&recoveredHardwareDevice));
            d3dDeviceCreationCount++;

            // Internally, this device should also be created against Requested CanvasHardwareAcceleration::Auto.
            // Ensure that this is the case.
            Assert::AreEqual(CanvasHardwareAcceleration::Auto, GetRequestedHardwareAcceleration(recoveredHardwareDevice));

            // This causes the retrievable option to, then, be CanvasHardwareAcceleration::On.
            Assert::AreEqual(d3dDeviceCreationCount, m_resourceCreationAdapter->m_numD3dDeviceCreationCalls);
            Assert::AreEqual(CanvasHardwareAcceleration::On, m_resourceCreationAdapter->m_retrievableHarwareAcceleration);

            CanvasHardwareAcceleration actualHardwareAcceleration;
            recoveredHardwareDevice->get_HardwareAcceleration(&actualHardwareAcceleration);
            Assert::AreEqual(CanvasHardwareAcceleration::On, actualHardwareAcceleration);
        }

        {
            // Re-create another whole device with the hardware path on, ensuring there isn't some weird statefulness problem.
            canvasDevice = m_deviceManager->Create(
                CanvasDebugLevel::None,
                CanvasHardwareAcceleration::Auto);
            d3dDeviceCreationCount++;

            // Ensure the hardware path was used.
            Assert::AreEqual(CanvasHardwareAcceleration::On, m_resourceCreationAdapter->m_retrievableHarwareAcceleration);
            Assert::AreEqual(d3dDeviceCreationCount, m_resourceCreationAdapter->m_numD3dDeviceCreationCalls);
            Assert::AreEqual(CanvasHardwareAcceleration::Auto, GetRequestedHardwareAcceleration(canvasDevice));

            // Ensure the HardwareAcceleration property getter returns HW again.
            CanvasHardwareAcceleration actualHardwareAcceleration;
            canvasDevice->get_HardwareAcceleration(&actualHardwareAcceleration);
            Assert::AreEqual(CanvasHardwareAcceleration::On, actualHardwareAcceleration);
        }

        {
            // Ensure RecoverLostDevice will now yield a HW device, too.
            ComPtr<ICanvasDevice> recoveredHardwareDevice;
            ThrowIfFailed(canvasDevice->RecoverLostDevice(&recoveredHardwareDevice));
            d3dDeviceCreationCount++;
            Assert::AreEqual(d3dDeviceCreationCount, m_resourceCreationAdapter->m_numD3dDeviceCreationCalls);
            Assert::AreEqual(CanvasHardwareAcceleration::On, m_resourceCreationAdapter->m_retrievableHarwareAcceleration);

            CanvasHardwareAcceleration actualHardwareAcceleration;
            recoveredHardwareDevice->get_HardwareAcceleration(&actualHardwareAcceleration);
            Assert::AreEqual(CanvasHardwareAcceleration::On, actualHardwareAcceleration);
        }
    }

    TEST_METHOD(CanvasDeviceManager_Create_GetOrCreate_Returns_Same_Instance)
    {
        auto expectedCanvasDevice = m_deviceManager->Create(CanvasDebugLevel::None, CanvasHardwareAcceleration::On);

        //
        // Create, followed by GetOrCreate on the same d2d device should give
        // back the same CanvasDevice.
        //

        auto d2dDevice = expectedCanvasDevice->GetD2DDevice();

        auto actualCanvasDevice = m_deviceManager->GetOrCreate(d2dDevice.Get());

        Assert::AreEqual<ICanvasDevice*>(expectedCanvasDevice.Get(), actualCanvasDevice.Get());

        //
        // Destroying these original CanvasDevices, and the GetOrCreate using
        // the same d2d device should give back a new, different, CanvasDevice.
        //

        WeakRef weakExpectedCanvasDevice;
        ThrowIfFailed(AsWeak(expectedCanvasDevice.Get(), &weakExpectedCanvasDevice));
        expectedCanvasDevice.Reset();
        actualCanvasDevice.Reset();

        actualCanvasDevice = m_deviceManager->GetOrCreate(d2dDevice.Get());

        ComPtr<ICanvasDevice> unexpectedCanvasDevice;
        weakExpectedCanvasDevice.As(&unexpectedCanvasDevice);

        Assert::AreNotEqual<ICanvasDevice*>(unexpectedCanvasDevice.Get(), actualCanvasDevice.Get());
    }
};

TEST_CLASS(DefaultDeviceResourceCreationAdapterTests)
{
    //
    // This tests GetDxgiDevice against real-live D3D/D2D instances since it
    // relies on non-trivial interaction with these to behave as we expect.
    //
    TEST_METHOD(GetDxgiDevice)
    {
        //
        // Set up
        //
        DefaultDeviceResourceCreationAdapter adapter;

        ComPtr<ID3D11Device> d3dDevice;
        if (!adapter.TryCreateD3DDevice(CanvasHardwareAcceleration::Off, &d3dDevice))
        {
            Assert::Fail(L"Failed to create d3d device");
        }

        ComPtr<IDXGIDevice> dxgiDevice;
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
