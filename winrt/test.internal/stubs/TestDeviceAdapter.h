// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#include "stubs/StubD3D11Device.h"
#include "mocks/MockProperties.h"

class StubDebugProperty : public MockPropertyValue
{
    uint32_t m_value;

public:
    StubDebugProperty(CanvasDebugLevel value)
        : m_value(static_cast<uint32_t>(value))
    {}

    StubDebugProperty(uint32_t value)
        : m_value(value)
    {}

    IFACEMETHODIMP GetUInt32(UINT32* out) override
    {
        *out = m_value;
        return S_OK;
    }
};

class StubPropertyValueStatics : public MockPropertyValueStatics
{
public:
    IFACEMETHODIMP CreateUInt32(
        UINT32 value,
        IInspectable** out) override
    {
        auto propertyValue = Make<StubDebugProperty>(value);
        return propertyValue.CopyTo(out);
    }
};

// This device is used for creating mock devices which include some basic functionality, but do not call any actual D2D/D3D.
class TestDeviceAdapter : public CanvasDeviceAdapter
{
    // If not set, fail all attempts at creating a device with CanvasHardwareAcceleration::On.
    bool m_allowHardware;

public:
    CALL_COUNTER_WITH_MOCK(GetCoreApplicationMethod, ComPtr<ICoreApplication>());

    TestDeviceAdapter()
        : m_numD2DFactoryCreationCalls(0)
        , m_debugLevel(static_cast<CanvasDebugLevel>(-1))
        , m_numD3dDeviceCreationCalls(0)
        , m_allowHardware(true)
        , m_retrievableForceSoftwareRenderer(false)
        , m_retrievableUseDebugD3DDevice(false)
    {
        GetCoreApplicationMethod.AllowAnyCall(
            [&]
            {
                auto coreApplication = Make<MockCoreApplication>();                
                coreApplication->get_PropertiesMethod.AllowAnyCall(
                    [&](IPropertySet** out)
                    { 
                        auto mockProperties = Make<MockProperties>();
                        mockProperties->LookupMethod.AllowAnyCall(
                            [&](HSTRING, IInspectable** out)
                            {
                                auto debugPropertyHolder = Make<StubDebugProperty>(CanvasDebugLevel::None);
                                debugPropertyHolder.CopyTo(out);
                                return S_OK;
                            });
                        mockProperties->InsertMethod.AllowAnyCall();
                        return mockProperties.CopyTo(out);
                    });

                return coreApplication;
            });
    }

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
        bool useSoftwareRenderer,
        bool useDebugD3DDevice,
        ComPtr<ID3D11Device>* device) override
    {
        if (!device)
        {
            Assert::Fail(L"Unexpected null device parameter in TryCreateD3DDevice");
            return false;
        }

        m_retrievableForceSoftwareRenderer = useSoftwareRenderer;
        m_retrievableUseDebugD3DDevice = useDebugD3DDevice;

        if (!useSoftwareRenderer && !m_allowHardware)
        {
            return false;
        }
        else
        {
            m_numD3dDeviceCreationCalls++;

            // Stub device is used here, because, in execution of these tests, product code will 
            // actually call methods on the factory and expect them to succeed.
            *device = CreateStubD3D11Device();
            return true;
        }
    }

    virtual ComPtr<IDXGIDevice3> GetDxgiDevice(ID2D1Device1* d2dDevice) override
    {
        ComPtr<ID2DDeviceWithDxgiDevice> d2dDeviceWithDxgiDevice;
        ThrowIfFailed(d2dDevice->QueryInterface(d2dDeviceWithDxgiDevice.GetAddressOf()));
        
        return d2dDeviceWithDxgiDevice->GetDxgiDevice();
    }

    virtual ComPtr<ICoreApplication> GetCoreApplication() override
    {
        return GetCoreApplicationMethod.WasCalled();
    }

    virtual ComPtr<IPropertyValueStatics> GetPropertyValueStatics() override
    {
        return Make<StubPropertyValueStatics>();
    }

    void SetHardwareEnabled(bool enable)
    {
        m_allowHardware = enable;
    }

    virtual ComPtr<StubD3D11Device> CreateStubD3D11Device()
    {
        return Make<StubD3D11Device>();
    }

    // These are left public because it's test code and it's convenient to do so.
    int m_numD2DFactoryCreationCalls;
    CanvasDebugLevel m_debugLevel;

    int m_numD3dDeviceCreationCalls;
    bool m_retrievableForceSoftwareRenderer;
    bool m_retrievableUseDebugD3DDevice;
};
