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

    TEST_METHOD(CanvasDevice_NativeInterop_DeviceContextLease)
    {
        auto canvasDevice = ref new CanvasDevice();

        // CanvasDevice should be castable to ID2D1DeviceContextPool
        auto deviceContextPool = As<ABI::Microsoft::Graphics::Canvas::ID2D1DeviceContextPool>(canvasDevice);

        // Getting a device context lease should always succeed
        ComPtr<ABI::Microsoft::Graphics::Canvas::ID2D1DeviceContextLease> deviceContextLease;
        ThrowIfFailed(deviceContextPool->GetDeviceContextLease(&deviceContextLease));
        Assert::IsNotNull(deviceContextLease.Get());

        // Nobody else should have a reference to this lease
        deviceContextLease->AddRef();
        Assert::AreEqual(deviceContextLease->Release(), 1ul);

        // Getting a device context from a lease should also always succeed
        ComPtr<ID2D1DeviceContext> deviceContext;
        ThrowIfFailed(deviceContextLease->GetD2DDeviceContext(&deviceContext));
        Assert::IsNotNull(deviceContext.Get());

        // Getting a device context from a lease again should always return the same instance.
        // That is, a device context lease is just a thin wrapper around a single rented context.
        ComPtr<ID2D1DeviceContext> deviceContext2;
        ThrowIfFailed(deviceContextLease->GetD2DDeviceContext(&deviceContext2));
        Assert::IsNotNull(deviceContext2.Get());
        Assert::IsTrue(As<IUnknown>(deviceContext.Get()).Get() == As<IUnknown>(deviceContext2.Get()).Get());

        // Get the device from the canvas device
        ComPtr<ID2D1Device1> d2D1Device;
        ThrowIfFailed(As<ABI::Microsoft::Graphics::Canvas::ICanvasResourceWrapperNative>(canvasDevice)->GetNativeResource(
            As<ABI::Microsoft::Graphics::Canvas::ICanvasDevice>(canvasDevice).Get(),
            0,
            IID_PPV_ARGS(&d2D1Device)));

        // Also get the device from the rented device context
        ComPtr<ID2D1Device> d2D1DeviceFromDeviceContext;
        deviceContext->GetDevice(&d2D1DeviceFromDeviceContext);

        // The underlying device from the canvas device should be the same one as the one from the context
        Assert::IsTrue(As<IUnknown>(d2D1Device.Get()).Get() == As<IUnknown>(d2D1DeviceFromDeviceContext.Get()).Get());
    }

    TEST_METHOD(CanvasDevice_GetSharedDevice_ReturnsExisting)
    {
        auto d1 = CanvasDevice::GetSharedDevice(false);

        auto d2 = CanvasDevice::GetSharedDevice(false);

        Assert::AreEqual(d1, d2);
    }

    TEST_METHOD(CanvasDevice_GetSharedDevice_WhenDeviceClosed_ReturnsNew)
    {
        auto d1 = CanvasDevice::GetSharedDevice(false);

        delete d1;

        auto d2 = CanvasDevice::GetSharedDevice(false);

        Assert::AreNotEqual(d1, d2);
    }

    TEST_METHOD(CanvasDevice_GetSharedDevice_WhenDebugLevelChanged_Fails)
    {
        DisableDebugLayer disableDebug;

        auto d1 = CanvasDevice::GetSharedDevice(false);

        CanvasDevice::DebugLevel = CanvasDebugLevel::Error;

        ExpectCOMException(
            E_FAIL,
            L"CanvasDevice.DebugLevel has changed since this shared device was created. The debug level must be set before the first call to GetSharedDevice.",
            [] { CanvasDevice::GetSharedDevice(false); });
    }

    TEST_METHOD(CanvasDevice_DefaultDebugLevel)
    {
#ifdef NDEBUG
        Assert::AreEqual(CanvasDebugLevel::None, CanvasDevice::DebugLevel);
#else
        // In debug builds, ModuleInitialize (see DebugLayer.cpp) may have overridden the default
        // debug level to specify CanvasDebugLevel::Warning. Or it may have left this set to
        // CanvasDebugLevel::None (if the debug layer is not installed on the current machine).
        Assert::IsTrue(CanvasDevice::DebugLevel == CanvasDebugLevel::Warning ||
                       CanvasDevice::DebugLevel == CanvasDebugLevel::None);
#endif
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
