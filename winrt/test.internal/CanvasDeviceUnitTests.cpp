// Copyright (c) Microsoft Corporation.  All rights reserved

#include "pch.h"

TEST_CLASS(CanvasDeviceTests)
{
public:

    class TestDeviceResourceCreationAdapter : public ICanvasDeviceResourceCreationAdapter
    {
        // If not set, fail all attempts at creating a device with CanvasHardwareAcceleration::On.
        bool m_allowHardware;

    public:

        TestDeviceResourceCreationAdapter()
            : m_numD2DFactoryCreationCalls(0)
            , m_debugLevel(static_cast<CanvasDebugLevel>(-1))
            , m_numD3dDeviceCreationCalls(0)
            , m_allowHardware(true)
        {}

        ComPtr<ID2D1Factory2> CreateD2DFactory(
            CanvasDebugLevel debugLevel
            )
        {
            m_numD2DFactoryCreationCalls++;
            m_debugLevel = debugLevel;

            // Mock factory is used here, because, in execution of these tests, product code will 
            // actually call methods on the factory and expect them to succeed.
            return Make<MockD2DFactory>();
        }

        virtual bool TryCreateD3DDevice(CanvasHardwareAcceleration hardwareAcceleration, ComPtr<ID3D11Device>* device)
        {
            m_retrievableHarwareAcceleration = hardwareAcceleration;

            if (hardwareAcceleration == CanvasHardwareAcceleration::On && !m_allowHardware)
            {
                return false;
            }
            else
            {
                m_numD3dDeviceCreationCalls++;

                // Mock factory is used here, because, in execution of these tests, product code will 
                // actually call methods on the factory and expect them to succeed.
                *device = Make<MockD3D11Device>();
                return true;
            }
        }

        void SetHardwareEnabled(bool enable)
        {
            m_allowHardware = enable;
        }
        
        // These are left public because it's test code and it's convenient to do so.
        int m_numD2DFactoryCreationCalls;
        CanvasDebugLevel m_debugLevel;

        int m_numD3dDeviceCreationCalls;
        CanvasHardwareAcceleration m_retrievableHarwareAcceleration;
    };

    CanvasHardwareAcceleration GetRoundTripHardwareAcceleration(const ComPtr<ICanvasDevice>& canvasDevice)
    {
        ComPtr<ICanvasDeviceInternal> canvasDeviceInternal;
        Assert::AreEqual(S_OK, canvasDevice.As(&canvasDeviceInternal));
        return canvasDeviceInternal->GetRoundTripHardwareAcceleration();
    }

    // Test that the default constructor constructs something. Ensure it passes the correct parameters down.
    TEST_METHOD(CanvasDevice_DefaultConstructor)
    {
        using canvas::CanvasDevice;

        std::shared_ptr<TestDeviceResourceCreationAdapter> resourceCreationAdapter = std::make_shared<TestDeviceResourceCreationAdapter>();

        auto canvasDevice = Make<CanvasDevice>(resourceCreationAdapter);
        Assert::IsNotNull(canvasDevice.Get());

        Assert::AreEqual(1, resourceCreationAdapter->m_numD2DFactoryCreationCalls);
        Assert::AreEqual(CanvasDebugLevel::None, resourceCreationAdapter->m_debugLevel);
        Assert::AreEqual(1, resourceCreationAdapter->m_numD3dDeviceCreationCalls);
        Assert::AreEqual(CanvasHardwareAcceleration::On, resourceCreationAdapter->m_retrievableHarwareAcceleration); // Hardware is the default, and should be used here.
        Assert::AreEqual(CanvasHardwareAcceleration::Auto, GetRoundTripHardwareAcceleration(canvasDevice));
    }

    TEST_METHOD(CanvasDevice_DebugLevels)
    {
        using canvas::CanvasDevice;

        CanvasDebugLevel cases[] = { CanvasDebugLevel::None, CanvasDebugLevel::Error, CanvasDebugLevel::Warning, CanvasDebugLevel::Information };
        for (int i = 0; i < _countof(cases); ++i)
        {
            std::shared_ptr<TestDeviceResourceCreationAdapter> resourceCreationAdapter = std::make_shared<TestDeviceResourceCreationAdapter>();

            ComPtr<ICanvasDevice> canvasDevice = Make<CanvasDevice>(cases[i], CanvasHardwareAcceleration::Auto, resourceCreationAdapter);
            Assert::IsNotNull(canvasDevice.Get());

            Assert::AreEqual(1, resourceCreationAdapter->m_numD3dDeviceCreationCalls);
            Assert::AreEqual(cases[i], resourceCreationAdapter->m_debugLevel);
        }

        // Try an invalid debug level
        {
            std::shared_ptr<TestDeviceResourceCreationAdapter> resourceCreationAdapter = std::make_shared<TestDeviceResourceCreationAdapter>();
            
            Assert::ExpectException<InvalidArgException>(
                [&]()
                {
                    ComPtr<ICanvasDevice> canvasDevice = Make<CanvasDevice>(
                        static_cast<CanvasDebugLevel>(1234),
                        CanvasHardwareAcceleration::Auto,
                        resourceCreationAdapter
                        );
                });
        }
    }

    TEST_METHOD(CanvasDevice_HardwareAcceleration)
    {
        using canvas::CanvasDevice;
        
        CanvasHardwareAcceleration cases[] = { CanvasHardwareAcceleration::On, CanvasHardwareAcceleration::Off };
        for (int i = 0; i < _countof(cases); ++i)
        {
            std::shared_ptr<TestDeviceResourceCreationAdapter> resourceCreationAdapter = std::make_shared<TestDeviceResourceCreationAdapter>();
            
            auto canvasDevice = Make<CanvasDevice>(
                CanvasDebugLevel::Information, 
                cases[i], 
                resourceCreationAdapter);

            Assert::IsNotNull(canvasDevice.Get());

            // Verify HW acceleration property getter returns the right thing
            CanvasHardwareAcceleration hardwareAccelerationActual;
            canvasDevice->get_HardwareAcceleration(&hardwareAccelerationActual);
            Assert::IsTrue(cases[i] == hardwareAccelerationActual);

            // Verify the roundtrip field was stored correctly
            Assert::AreEqual(cases[i], GetRoundTripHardwareAcceleration(canvasDevice));

            // Ensure that the getter returns E_INVALIDARG with null ptr.
            Assert::AreEqual(E_INVALIDARG, canvasDevice->get_HardwareAcceleration(NULL));

            Assert::AreEqual(1, resourceCreationAdapter->m_numD2DFactoryCreationCalls);
            Assert::AreEqual(CanvasDebugLevel::Information, resourceCreationAdapter->m_debugLevel);
        }

        // Try an invalid option
        {
            std::shared_ptr<TestDeviceResourceCreationAdapter> resourceCreationAdapter = std::make_shared<TestDeviceResourceCreationAdapter>();

            ComPtr<ICanvasDevice> canvasDevice;

            Assert::ExpectException<InvalidArgException>(
                [&]() 
                { 
                    ComPtr<ICanvasDevice> canvasDevice = Make<CanvasDevice>(
                        CanvasDebugLevel::None,
                        static_cast<CanvasHardwareAcceleration>(0x5678),
                        resourceCreationAdapter);
                });

            Assert::IsNull(canvasDevice.Get());
        }

    }

    TEST_METHOD(CanvasDevice_DirectX11DeviceInterop)
    {
        using canvas::CanvasDevice;

        // Verify correct behavior with the 'interop' path constructors; i.e.
        // wrapping a CanvasDevice around a given DirectX11Device.
        std::shared_ptr<TestDeviceResourceCreationAdapter> resourceCreationAdapter = std::make_shared<TestDeviceResourceCreationAdapter>();
        
        ComPtr<MockD3D11Device> mockD3D11Device = Make<MockD3D11Device>();
        ComPtr<IDirectX11Device> stubDirectX11Device = Make<canvas::DirectX11Device>(mockD3D11Device.Get());

        auto canvasDevice = Make<CanvasDevice>(CanvasDebugLevel::None, stubDirectX11Device.Get(), resourceCreationAdapter);
        Assert::IsNotNull(canvasDevice.Get());

        // A D2D device should still have been created
        Assert::AreEqual(1, resourceCreationAdapter->m_numD2DFactoryCreationCalls);
        Assert::AreEqual(CanvasDebugLevel::None, resourceCreationAdapter->m_debugLevel);

        // But not a D3D device.
        Assert::AreEqual(0, resourceCreationAdapter->m_numD3dDeviceCreationCalls);

        // Try a NULL DirectX11Device. 
        Assert::ExpectException<InvalidArgException>(
            [&]() { Make<CanvasDevice>(CanvasDebugLevel::None, nullptr, resourceCreationAdapter); });
    }

    TEST_METHOD(CanvasDevice_Closed)
    {
        using canvas::CanvasDevice;

        std::shared_ptr<TestDeviceResourceCreationAdapter> resourceCreationAdapter = std::make_shared<TestDeviceResourceCreationAdapter>();
        auto canvasDevice = Make<CanvasDevice>(resourceCreationAdapter);
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
        using canvas::CanvasDevice;

        std::shared_ptr<TestDeviceResourceCreationAdapter> resourceCreationAdapter = std::make_shared<TestDeviceResourceCreationAdapter>();

        ComPtr<CanvasDevice> canvasDevice = Make<CanvasDevice>(resourceCreationAdapter);
        ComPtr<ICanvasDevice> recoveredDevice;
        Assert::AreEqual(S_OK, canvasDevice->RecoverLostDevice(&recoveredDevice));

        VerifyCompatibleDevices(canvasDevice, recoveredDevice);
    }

    TEST_METHOD(CanvasDevice_CreateCompatibleDevice)
    {
        using canvas::CanvasDevice;

        std::shared_ptr<TestDeviceResourceCreationAdapter> resourceCreationAdapter = std::make_shared<TestDeviceResourceCreationAdapter>();

        ComPtr<CanvasDevice> canvasDevice = Make<CanvasDevice>(resourceCreationAdapter);
        ComPtr<ICanvasDevice> compatibleDevice;
            
        ComPtr<MockD3D11Device> mockD3D11Device = Make<MockD3D11Device>();
        ComPtr<IDirectX11Device> stubDirectX11Device = Make<canvas::DirectX11Device>(mockD3D11Device.Get());

        Assert::AreEqual(S_OK, canvasDevice->CreateCompatibleDevice(stubDirectX11Device.Get(), &compatibleDevice));

        VerifyCompatibleDevices(canvasDevice, compatibleDevice);

        // Additionally, verify that the compatible device has the correct original DirectX11Device.
        ComPtr<IDirectX11Device> deviceActual;
        Assert::AreEqual(S_OK, compatibleDevice->get_DirectX11Device(&deviceActual));
        Assert::AreEqual(static_cast<IDirectX11Device*>(stubDirectX11Device.Get()), deviceActual.Get());
    }

    TEST_METHOD(CanvasDevice_HwSwFallback)
    {
        using canvas::CanvasDevice;

        std::shared_ptr<TestDeviceResourceCreationAdapter> resourceCreationAdapter = std::make_shared<TestDeviceResourceCreationAdapter>();

        int d3dDeviceCreationCount = 0;

        // Default canvas device should be hardware.
        ComPtr<CanvasDevice> canvasDevice = Make<CanvasDevice>(
            CanvasDebugLevel::None, 
            CanvasHardwareAcceleration::Auto, 
            resourceCreationAdapter);
        d3dDeviceCreationCount++;

        Assert::AreEqual(CanvasHardwareAcceleration::On, resourceCreationAdapter->m_retrievableHarwareAcceleration);
        Assert::AreEqual(CanvasHardwareAcceleration::Auto, GetRoundTripHardwareAcceleration(canvasDevice));
        Assert::AreEqual(d3dDeviceCreationCount, resourceCreationAdapter->m_numD3dDeviceCreationCalls);

        // Now disable the hardware path.
        resourceCreationAdapter->SetHardwareEnabled(false);

        {
            // Ensure the fallback works.
            ComPtr<CanvasDevice> canvasDevice = Make<CanvasDevice>(
                CanvasDebugLevel::None,
                CanvasHardwareAcceleration::Auto,
                resourceCreationAdapter);
            d3dDeviceCreationCount++;

            // Ensure the software path was used.
            Assert::AreEqual(CanvasHardwareAcceleration::Off, resourceCreationAdapter->m_retrievableHarwareAcceleration);
            Assert::AreEqual(CanvasHardwareAcceleration::Auto, GetRoundTripHardwareAcceleration(canvasDevice));
            Assert::AreEqual(d3dDeviceCreationCount, resourceCreationAdapter->m_numD3dDeviceCreationCalls);

            // Ensure the HardwareAcceleration property getter returns the right thing.
            CanvasHardwareAcceleration actualHardwareAcceleration;
            canvasDevice->get_HardwareAcceleration(&actualHardwareAcceleration);
            Assert::AreEqual(CanvasHardwareAcceleration::Off, actualHardwareAcceleration);
        }

        // Ensure RecoverLostDevice will yield another software device, and that it has the correct properties.
        {
            ComPtr<ICanvasDevice> recoveredSoftwareDevice;
            canvasDevice->RecoverLostDevice(&recoveredSoftwareDevice);
            d3dDeviceCreationCount++;
            Assert::AreEqual(d3dDeviceCreationCount, resourceCreationAdapter->m_numD3dDeviceCreationCalls);
            Assert::AreEqual(CanvasHardwareAcceleration::Off, resourceCreationAdapter->m_retrievableHarwareAcceleration);
            Assert::AreEqual(CanvasHardwareAcceleration::Auto, GetRoundTripHardwareAcceleration(recoveredSoftwareDevice));

            CanvasHardwareAcceleration actualHardwareAcceleration;
            recoveredSoftwareDevice->get_HardwareAcceleration(&actualHardwareAcceleration);
            Assert::AreEqual(CanvasHardwareAcceleration::Off, actualHardwareAcceleration);
        }

        // Re-enable the hardware path, and do another RecoverLostDevice. 
        resourceCreationAdapter->SetHardwareEnabled(true);
        {
            ComPtr<ICanvasDevice> recoveredHardwareDevice;
            canvasDevice->RecoverLostDevice(&recoveredHardwareDevice);
            d3dDeviceCreationCount++;

            // Internally, this device should also be created against roundtrip CanvasHardwareAcceleration::Auto.
            // Ensure that this is the case.
            Assert::AreEqual(CanvasHardwareAcceleration::Auto, GetRoundTripHardwareAcceleration(recoveredHardwareDevice));

            // This causes the retrievable option to, then, be CanvasHardwareAcceleration::On.
            Assert::AreEqual(d3dDeviceCreationCount, resourceCreationAdapter->m_numD3dDeviceCreationCalls);
            Assert::AreEqual(CanvasHardwareAcceleration::On, resourceCreationAdapter->m_retrievableHarwareAcceleration);

            CanvasHardwareAcceleration actualHardwareAcceleration;
            recoveredHardwareDevice->get_HardwareAcceleration(&actualHardwareAcceleration);
            Assert::AreEqual(CanvasHardwareAcceleration::On, actualHardwareAcceleration);
        }

        {
            // Re-create another whole device with the hardware path on, ensuring there isn't some weird statefulness problem.
            canvasDevice = Make<CanvasDevice>(
                CanvasDebugLevel::None,
                CanvasHardwareAcceleration::Auto,
                resourceCreationAdapter);
            d3dDeviceCreationCount++;

            // Ensure the hardware path was used.
            Assert::AreEqual(CanvasHardwareAcceleration::On, resourceCreationAdapter->m_retrievableHarwareAcceleration);
            Assert::AreEqual(d3dDeviceCreationCount, resourceCreationAdapter->m_numD3dDeviceCreationCalls);
            Assert::AreEqual(CanvasHardwareAcceleration::Auto, GetRoundTripHardwareAcceleration(canvasDevice));

            // Ensure the HardwareAcceleration property getter returns HW again.
            CanvasHardwareAcceleration actualHardwareAcceleration;
            canvasDevice->get_HardwareAcceleration(&actualHardwareAcceleration);
            Assert::AreEqual(CanvasHardwareAcceleration::On, actualHardwareAcceleration);
        }

        {
            // Ensure RecoverLostDevice will now yield a HW device, too.
            ComPtr<ICanvasDevice> recoveredHardwareDevice;
            canvasDevice->RecoverLostDevice(&recoveredHardwareDevice);
            d3dDeviceCreationCount++;
            Assert::AreEqual(d3dDeviceCreationCount, resourceCreationAdapter->m_numD3dDeviceCreationCalls);
            Assert::AreEqual(CanvasHardwareAcceleration::On, resourceCreationAdapter->m_retrievableHarwareAcceleration);

            CanvasHardwareAcceleration actualHardwareAcceleration;
            recoveredHardwareDevice->get_HardwareAcceleration(&actualHardwareAcceleration);
            Assert::AreEqual(CanvasHardwareAcceleration::On, actualHardwareAcceleration);
        }
    }
};