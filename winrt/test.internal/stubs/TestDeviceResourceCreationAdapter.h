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

#pragma once

#include "stubs/StubD3D11Device.h"

// This device is used for creating mock devices which include some basic functionality, but do not call any actual D2D/D3D.
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
        , m_retrievableHarwareAcceleration(CanvasHardwareAcceleration::Auto)
    {}

    virtual ComPtr<ID2D1Factory2> CreateD2DFactory(
        CanvasDebugLevel debugLevel) override
    {
        m_numD2DFactoryCreationCalls++;
        m_debugLevel = debugLevel;

        // Mock factory is used here, because, in execution of these tests, product code will 
        // actually call methods on the factory and expect them to succeed.
        return Make<MockD2DFactory>();
    }

    virtual bool TryCreateD3DDevice(
        CanvasHardwareAcceleration hardwareAcceleration,
        ComPtr<ID3D11Device>* device) override
    {
        if (!device)
        {
            Assert::Fail(L"Unexpected null device parameter in TryCreateD3DDevice");
            return false;
        }

        m_retrievableHarwareAcceleration = hardwareAcceleration;

        if (hardwareAcceleration == CanvasHardwareAcceleration::On && !m_allowHardware)
        {
            return false;
        }
        else
        {
            m_numD3dDeviceCreationCalls++;

            // Stub device is used here, because, in execution of these tests, product code will 
            // actually call methods on the factory and expect them to succeed.
            *device = Make<StubD3D11Device>();
            return true;
        }
    }

    virtual ComPtr<IDXGIDevice3> GetDxgiDevice(ID2D1Device1* d2dDevice) override
    {
        ComPtr<ID2DDeviceWithDxgiDevice> d2dDeviceWithDxgiDevice;
        ThrowIfFailed(d2dDevice->QueryInterface(d2dDeviceWithDxgiDevice.GetAddressOf()));
        
        return d2dDeviceWithDxgiDevice->GetDxgiDevice();
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
