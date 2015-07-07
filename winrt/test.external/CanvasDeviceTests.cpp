// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

using namespace Microsoft::Graphics::Canvas;
using namespace WinRTDirectX;

CanvasDebugLevel allDebugLevels[] = {
    CanvasDebugLevel::Error,
    CanvasDebugLevel::Warning,
    CanvasDebugLevel::Information,
    CanvasDebugLevel::None };

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
        // Therefore, we want to cover each of D3D HW and SW.
        //

        CanvasDevice^ canvasDevice = ref new CanvasDevice();
        Assert::IsFalse(canvasDevice->ForceSoftwareRenderer);
        Assert::IsNotNull(GetDXGIDevice(canvasDevice).Get());

        canvasDevice = ref new CanvasDevice(true);
        Assert::IsTrue(canvasDevice->ForceSoftwareRenderer);
        Assert::IsNotNull(GetDXGIDevice(canvasDevice).Get());

        IDirect3DDevice^ direct3DDevice = canvasDevice;
        canvasDevice = CanvasDevice::CreateFromDirect3D11Device(
            direct3DDevice);
        //
        // Devices created using Direct3D interop have the convention of always
        // ForceSoftwareRenderer == false.
        //
        Assert::IsFalse(canvasDevice->ForceSoftwareRenderer);

        delete canvasDevice;
            
        ExpectObjectClosed([&]{ canvasDevice->ForceSoftwareRenderer; });
        ExpectObjectClosed([&]{ canvasDevice->MaximumBitmapSizeInPixels; });
        ExpectObjectClosed([&]{ GetDXGIDevice(canvasDevice); });
    }

    static ComPtr<IDXGIDevice> GetDXGIDevice(CanvasDevice^ device)
    {
        ComPtr<IDXGIDevice> dxgiDevice;
        __abi_ThrowIfFailed(GetDXGIInterface<IDXGIDevice>(device, &dxgiDevice));
        return dxgiDevice;
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

    TEST_METHOD(CanvasDevice_GetSharedDevice_ReturnsExisting)
    {
        auto d1 = CanvasDevice::GetSharedDevice(false);

        auto d2 = CanvasDevice::GetSharedDevice(false);

        Assert::AreEqual(d1, d2);
    }

    TEST_METHOD(CanvasDevice_DefaultDebugLevel)
    {
        Assert::AreEqual(CanvasDebugLevel::None, CanvasDevice::DebugLevel);
    }

    TEST_METHOD(CanvasDevice_SetAndGetDebugLevels)
    {
        for (auto debugLevel : allDebugLevels)
        {
            CanvasDevice::DebugLevel = debugLevel;
            Assert::AreEqual(debugLevel, CanvasDevice::DebugLevel);
        }
    }
};
