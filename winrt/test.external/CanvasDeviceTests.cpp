// Copyright (c) Microsoft Corporation.  All rights reserved

#include "pch.h"

using namespace Microsoft::Graphics::Canvas;
using namespace Microsoft::Graphics::Canvas::DirectX::Direct3D11;

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
        Assert::IsNotNull(canvasDevice->Direct3DDevice);

        canvasDevice = ref new CanvasDevice(CanvasDebugLevel::Information);
        Assert::AreEqual(CanvasHardwareAcceleration::On, canvasDevice->HardwareAcceleration);
        Assert::IsNotNull(canvasDevice->Direct3DDevice);

        canvasDevice = ref new CanvasDevice(CanvasDebugLevel::Warning, CanvasHardwareAcceleration::Off);
        Assert::AreEqual(CanvasHardwareAcceleration::Off, canvasDevice->HardwareAcceleration);
        Assert::IsNotNull(canvasDevice->Direct3DDevice);

        auto direct3DDevice = canvasDevice->Direct3DDevice;
        canvasDevice = CanvasDevice::CreateFromDirect3D11Device(
            CanvasDebugLevel::None,
            direct3DDevice);
        Assert::AreEqual(CanvasHardwareAcceleration::Unknown, canvasDevice->HardwareAcceleration);

        delete canvasDevice;
            
        ExpectObjectClosed([&](){ canvasDevice->HardwareAcceleration; });
        ExpectObjectClosed([&](){ canvasDevice->Direct3DDevice; });
    }

    TEST_METHOD(CanvasDevice_RecoverDevice)
    {
        CanvasDevice^ canvasDevice = ref new CanvasDevice();

        CanvasDevice^ recoveredDevice = canvasDevice->RecoverLostDevice();

        Assert::AreNotEqual(canvasDevice->Direct3DDevice, recoveredDevice->Direct3DDevice);

        delete canvasDevice;

        ExpectObjectClosed([&](){ canvasDevice->RecoverLostDevice(); });
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
        Direct3DDevice^ direct3DDevice = CreateDirect3DDevice(dxgiDevice.Get());

        CanvasDevice^ compatibleDevice = canvasDevice->CreateCompatibleDevice(direct3DDevice);

        delete canvasDevice;

        ExpectObjectClosed([&](){ canvasDevice->CreateCompatibleDevice(direct3DDevice); });
    }

    TEST_METHOD(CanvasDevice_NativeInterop)
    {
        auto originalCanvasDevice = ref new CanvasDevice();

        auto originalD2DDevice = GetWrappedResource<ID2D1Device1>(originalCanvasDevice);

        auto newCanvasDevice = GetOrCreate<CanvasDevice>(originalD2DDevice.Get());

        auto newD2DDevice = GetWrappedResource<ID2D1Device1>(newCanvasDevice);

        Assert::AreEqual(originalCanvasDevice, newCanvasDevice);
        Assert::AreEqual(originalD2DDevice.Get(), newD2DDevice.Get());
    }
};
