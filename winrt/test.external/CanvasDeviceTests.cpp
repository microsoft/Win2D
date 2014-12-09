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
        Assert::IsNotNull(GetDXGIDevice(canvasDevice).Get());

        canvasDevice = ref new CanvasDevice(CanvasDebugLevel::Information);
        Assert::AreEqual(CanvasHardwareAcceleration::On, canvasDevice->HardwareAcceleration);
        Assert::IsNotNull(GetDXGIDevice(canvasDevice).Get());

        canvasDevice = ref new CanvasDevice(CanvasDebugLevel::Warning, CanvasHardwareAcceleration::Off);
        Assert::AreEqual(CanvasHardwareAcceleration::Off, canvasDevice->HardwareAcceleration);
        Assert::IsNotNull(GetDXGIDevice(canvasDevice).Get());

        IDirect3DDevice^ direct3DDevice = canvasDevice;
        canvasDevice = CanvasDevice::CreateFromDirect3D11Device(
            direct3DDevice,
            CanvasDebugLevel::None);
        Assert::AreEqual(CanvasHardwareAcceleration::Unknown, canvasDevice->HardwareAcceleration);

        delete canvasDevice;
            
        ExpectObjectClosed([&]{ canvasDevice->HardwareAcceleration; });
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
};
