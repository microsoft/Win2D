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

#include "MockD2DCommandList.h"
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

    TEST_METHOD_EX(CanvasDevice_Implements_Expected_Interfaces)
    {
        auto canvasDevice = m_deviceManager->Create(CanvasDebugLevel::None, CanvasHardwareAcceleration::Auto);

        ASSERT_IMPLEMENTS_INTERFACE(canvasDevice, ICanvasDevice);
        ASSERT_IMPLEMENTS_INTERFACE(canvasDevice, ABI::Windows::Foundation::IClosable);
        ASSERT_IMPLEMENTS_INTERFACE(canvasDevice, ICanvasResourceWrapperNative);
        ASSERT_IMPLEMENTS_INTERFACE(canvasDevice, ICanvasDeviceInternal);
    }

    TEST_METHOD_EX(CanvasDevice_Defaults_Roundtrip)
    {
        auto canvasDevice = m_deviceManager->Create(CanvasDebugLevel::None, CanvasHardwareAcceleration::Auto);
        Assert::IsNotNull(canvasDevice.Get());

        Assert::AreEqual(1, m_resourceCreationAdapter->m_numD2DFactoryCreationCalls);
        Assert::AreEqual(CanvasDebugLevel::None, m_resourceCreationAdapter->m_debugLevel);
        Assert::AreEqual(1, m_resourceCreationAdapter->m_numD3dDeviceCreationCalls);
        Assert::AreEqual(CanvasHardwareAcceleration::On, m_resourceCreationAdapter->m_retrievableHarwareAcceleration); // Hardware is the default, and should be used here.

        AssertDeviceManagerRoundtrip(canvasDevice.Get());
    }

    TEST_METHOD_EX(CanvasDevice_DebugLevels)
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
        ExpectHResultException(E_INVALIDARG,
            [&]
            {
                m_deviceManager->Create(
                    static_cast<CanvasDebugLevel>(1234),
                    CanvasHardwareAcceleration::Auto);
            });
    }

    TEST_METHOD_EX(CanvasDevice_HardwareAcceleration)
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
            ExpectHResultException(E_INVALIDARG,
                [&] 
                { 
                    m_deviceManager->Create(
                        CanvasDebugLevel::None,
                        invalidCase);
                });
        }
    }

    TEST_METHOD_EX(CanvasDevice_Create_With_Specific_Direct3DDevice)
    {
        ComPtr<MockD3D11Device> mockD3D11Device = Make<MockD3D11Device>();

        ComPtr<IDirect3DDevice> stubDirect3DDevice;
        ThrowIfFailed(CreateDirect3D11DeviceFromDXGIDevice(mockD3D11Device.Get(), &stubDirect3DDevice));

        auto canvasDevice = m_deviceManager->Create(
            CanvasDebugLevel::None, 
            stubDirect3DDevice.Get());
        Assert::IsNotNull(canvasDevice.Get());

        // A D2D device should still have been created
        Assert::AreEqual(1, m_resourceCreationAdapter->m_numD2DFactoryCreationCalls);
        Assert::AreEqual(CanvasDebugLevel::None, m_resourceCreationAdapter->m_debugLevel);

        // But not a D3D device.
        Assert::AreEqual(0, m_resourceCreationAdapter->m_numD3dDeviceCreationCalls);

        AssertDeviceManagerRoundtrip(canvasDevice.Get());

        CanvasHardwareAcceleration hardwareAcceleration{};
        ThrowIfFailed(canvasDevice->get_HardwareAcceleration(&hardwareAcceleration));
        Assert::AreEqual(CanvasHardwareAcceleration::Unknown, hardwareAcceleration);

        // Try a NULL Direct3DDevice. 
        ExpectHResultException(E_INVALIDARG,
            [&] { m_deviceManager->Create(CanvasDebugLevel::None, nullptr); });
    }

    TEST_METHOD_EX(CanvasDevice_Create_From_D2DDevice)
    {
        auto d2dDevice = Make<MockD2DDevice>(Make<MockD2DFactory>().Get());

        auto canvasDevice = m_deviceManager->GetOrCreate(d2dDevice.Get());
        Assert::IsNotNull(canvasDevice.Get());

        // Nothing should have been created
        Assert::AreEqual(0, m_resourceCreationAdapter->m_numD2DFactoryCreationCalls);
        Assert::AreEqual(0, m_resourceCreationAdapter->m_numD3dDeviceCreationCalls);

        AssertDeviceManagerRoundtrip(canvasDevice.Get());

        CanvasHardwareAcceleration hardwareAcceleration{};
        ThrowIfFailed(canvasDevice->get_HardwareAcceleration(&hardwareAcceleration));
        Assert::AreEqual(CanvasHardwareAcceleration::Unknown, hardwareAcceleration);
    }

    TEST_METHOD_EX(CanvasDevice_Closed)
    {
        auto canvasDevice = m_deviceManager->Create(CanvasDebugLevel::None, CanvasHardwareAcceleration::On);
        Assert::IsNotNull(canvasDevice.Get());

        Assert::AreEqual(S_OK, canvasDevice->Close());

        ComPtr<IDXGIDevice> dxgiDevice;
        Assert::AreEqual(RO_E_CLOSED, canvasDevice->GetInterface(IID_PPV_ARGS(&dxgiDevice)));

        CanvasHardwareAcceleration hardwareAccelerationActual = static_cast<CanvasHardwareAcceleration>(1);
        Assert::AreEqual(RO_E_CLOSED, canvasDevice->get_HardwareAcceleration(&hardwareAccelerationActual));
    }

    ComPtr<ID2D1Device1> GetD2DDevice(ComPtr<ICanvasDevice> const& canvasDevice)
    {
        ComPtr<ICanvasDeviceInternal> canvasDeviceInternal;
        Assert::AreEqual(S_OK, canvasDevice.As(&canvasDeviceInternal));
        return canvasDeviceInternal->GetD2DDevice();
    }

    TEST_METHOD_EX(CanvasDevice_HwSwFallback)
    {
        Reset();

        int d3dDeviceCreationCount = 0;

        // Default canvas device should be hardware.
        auto canvasDevice = m_deviceManager->Create(
            CanvasDebugLevel::None, 
            CanvasHardwareAcceleration::Auto);
        d3dDeviceCreationCount++;

        Assert::AreEqual(CanvasHardwareAcceleration::On, m_resourceCreationAdapter->m_retrievableHarwareAcceleration);
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
            Assert::AreEqual(d3dDeviceCreationCount, m_resourceCreationAdapter->m_numD3dDeviceCreationCalls);

            // Ensure the HardwareAcceleration property getter returns the right thing.
            CanvasHardwareAcceleration hardwareAcceleration;
            canvasDevice->get_HardwareAcceleration(&hardwareAcceleration);
            Assert::AreEqual(CanvasHardwareAcceleration::Off, hardwareAcceleration);
        }
        {
            // Re-create another whole device with the hardware path on, ensuring there isn't some weird statefulness problem.
            m_resourceCreationAdapter->SetHardwareEnabled(true);
            canvasDevice = m_deviceManager->Create(
                CanvasDebugLevel::None,
                CanvasHardwareAcceleration::Auto);
            d3dDeviceCreationCount++;

            // Ensure the hardware path was used.
            Assert::AreEqual(CanvasHardwareAcceleration::On, m_resourceCreationAdapter->m_retrievableHarwareAcceleration);
            Assert::AreEqual(d3dDeviceCreationCount, m_resourceCreationAdapter->m_numD3dDeviceCreationCalls);

            // Ensure the HardwareAcceleration property getter returns HW again.
            CanvasHardwareAcceleration hardwareAcceleration;
            canvasDevice->get_HardwareAcceleration(&hardwareAcceleration);
            Assert::AreEqual(CanvasHardwareAcceleration::On, hardwareAcceleration);
        }
    }

    TEST_METHOD_EX(CanvasDeviceManager_Create_GetOrCreate_Returns_Same_Instance)
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

    TEST_METHOD_EX(CanvasDevice_DeviceProperty)
    {
        auto device = m_deviceManager->Create(CanvasDebugLevel::None, CanvasHardwareAcceleration::On);

        Assert::AreEqual(E_INVALIDARG, device->get_Device(nullptr));

        ComPtr<ICanvasDevice> deviceVerify;
        ThrowIfFailed(device->get_Device(&deviceVerify));
        Assert::AreEqual(static_cast<ICanvasDevice*>(device.Get()), deviceVerify.Get());
    }
    
    TEST_METHOD_EX(CanvasDevice_MaximumBitmapSize_NullArg)
    {
        auto canvasDevice = m_deviceManager->Create(CanvasDebugLevel::None, CanvasHardwareAcceleration::On);

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

        auto canvasDevice = m_deviceManager->GetOrCreate(d2dDevice.Get());

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

        auto canvasDevice = m_deviceManager->GetOrCreate(d2dDevice.Get());
        auto actualD2DCommandList = canvasDevice->CreateCommandList();

        Assert::IsTrue(IsSameInstance(d2dCommandList.Get(), actualD2DCommandList.Get()));
    }

    TEST_METHOD_EX(CanvasDevice_CreateRenderTarget_ReturnsBitmapCreatedWithCorrectProperties)
    {
        auto d2dDevice = Make<MockD2DDevice>();
        auto d2dBitmap = Make<MockD2DBitmap>();

        float anyWidth = 1.0f;
        float anyHeight = 2.0f;
        auto anyFormat = DirectXPixelFormat::R16G16B16A16UIntNormalized;
        auto anyAlphaMode = CanvasAlphaMode::Ignore;
        float anyDpi = 3.0f;

        auto deviceContext = Make<StubD2DDeviceContext>(d2dDevice.Get());
        deviceContext->CreateBitmapMethod.SetExpectedCalls(1,
            [&] (D2D1_SIZE_U size, void const* sourceData, UINT32 pitch, D2D1_BITMAP_PROPERTIES1 const* bitmapProperties, ID2D1Bitmap1** bitmap)
            {
                Assert::AreEqual<int>(DipsToPixels(anyWidth, anyDpi), size.width);
                Assert::AreEqual<int>(DipsToPixels(anyHeight, anyDpi), size.height);
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

        auto canvasDevice = m_deviceManager->GetOrCreate(d2dDevice.Get());
        auto actualBitmap = canvasDevice->CreateRenderTargetBitmap(anyWidth, anyHeight, anyFormat, anyAlphaMode, anyDpi);

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
        if (!adapter.TryCreateD3DDevice(CanvasHardwareAcceleration::Off, &d3dDevice))
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
