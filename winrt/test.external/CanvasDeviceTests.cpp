// Copyright (c) Microsoft Corporation.  All rights reserved

#include "pch.h"

using namespace Microsoft::Graphics::Canvas;

#define EXPECT_OBJECT_CLOSED(CODE) Assert::ExpectException<Platform::ObjectDisposedException^>([&]() { CODE; })

TEST_CLASS(CanvasDeviceTests)
{
    //
    // While there are unit tests to more closely verify that these functions
    // work correctly, this test verifies projected constructors and all
    // projected methods.
    //
    // Close() is exercised on the respective tests' methods. 
    //

    TEST_METHOD(CanvasDevice_ConstructorsAndProperties)
    {
        //
        // Unlike the unit tests, this uses actual D2D/D3D device creation. 
        // Therefore, we want to cover each of {D2D debug layer on, off} and {D3D HW, SW}
        //

        CanvasDevice^ canvasDevice = ref new CanvasDevice();
        Assert::AreEqual(CanvasHardwareAcceleration::On, canvasDevice->HardwareAcceleration);
        Assert::IsNotNull(canvasDevice->DirectX11Device);

        canvasDevice = ref new CanvasDevice(CanvasDebugLevel::Information);
        Assert::AreEqual(CanvasHardwareAcceleration::On, canvasDevice->HardwareAcceleration);
        Assert::IsNotNull(canvasDevice->DirectX11Device);

        canvasDevice = ref new CanvasDevice(CanvasDebugLevel::Warning, CanvasHardwareAcceleration::Off);
        Assert::AreEqual(CanvasHardwareAcceleration::Off, canvasDevice->HardwareAcceleration);
        Assert::IsNotNull(canvasDevice->DirectX11Device);

        delete canvasDevice;
            
        EXPECT_OBJECT_CLOSED(canvasDevice->HardwareAcceleration);
        EXPECT_OBJECT_CLOSED(canvasDevice->DirectX11Device);
    }

    TEST_METHOD(CanvasDevice_RecoverDevice)
    {
        CanvasDevice^ canvasDevice = ref new CanvasDevice();

        CanvasDevice^ recoveredDevice = canvasDevice->RecoverLostDevice();

        Assert::AreNotEqual(canvasDevice->DirectX11Device, recoveredDevice->DirectX11Device);

        delete canvasDevice;

        EXPECT_OBJECT_CLOSED(canvasDevice->RecoverLostDevice());
    }

    TEST_METHOD(CanvasDevice_CreateCompatibleDevice)
    {
        CanvasDevice^ canvasDevice = ref new CanvasDevice(CanvasDebugLevel::Information);
        using namespace Microsoft::WRL;

        ComPtr<ID3D11Device> d3dDevice;

        ThrowIfFailed(D3D11CreateDevice(
            nullptr,            // adapter
            D3D_DRIVER_TYPE_WARP,
            nullptr,            // software
            D3D11_CREATE_DEVICE_DEBUG | D3D11_CREATE_DEVICE_BGRA_SUPPORT, // BGRA required for D2D
            nullptr,            // feature levels
            0,                  // feature levels count
            D3D11_SDK_VERSION,
            &d3dDevice,
            nullptr,        // feature level
            nullptr));      // immediate context

        ComPtr<IDXGIDevice> dxgiDevice;
        ThrowIfFailed(d3dDevice.As(&dxgiDevice));
        DirectX11Device^ directX11Device = CreateDirectX11Device(dxgiDevice.Get());

        CanvasDevice^ compatibleDevice = canvasDevice->CreateCompatibleDevice(directX11Device);

        delete canvasDevice;

        EXPECT_OBJECT_CLOSED(canvasDevice->CreateCompatibleDevice(directX11Device));
    }
};

#undef EXPECT_OBJECT_CLOSED