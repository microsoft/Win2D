// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#include "stubs\StubDxgiDevice.h"

namespace canvas
{
    // Forward declaration to break circular dependency (defined in MockD2DFactory.h)
    ComPtr<ID2D1Factory2> MakeMockD2DFactory();


    class __declspec(uuid("D5B2FFD5-882E-4CB5-98FA-2342E52FC6F2"))
    ID2DDeviceWithDxgiDevice : public IUnknown
    {
    public:
        virtual ComPtr<IDXGIDevice3> GetDxgiDevice() = 0;
    };
    

    class MockD2DDevice : public RuntimeClass<
        RuntimeClassFlags<ClassicCom>,
        ChainInterfaces<
        ID2D1Device2,
        ID2D1Device1, ID2D1Device, ID2D1Resource>,
        ID2DDeviceWithDxgiDevice>
    {
        mutable ComPtr<ID2D1Factory2> m_parentD2DFactory;

    protected:
        ComPtr<IDXGIDevice3> m_dxgiDevice;

    public:
        std::function<void(D2D1_DEVICE_CONTEXT_OPTIONS,ID2D1DeviceContext1**)> MockCreateDeviceContext;

        MockD2DDevice(ID2D1Factory2* parentD2DFactory = nullptr)
            : m_parentD2DFactory(parentD2DFactory)
            , m_dxgiDevice(Make<StubDxgiDevice>())
        {            
        }

        MockD2DDevice(IDXGIDevice3* dxgiDevice)
            : m_parentD2DFactory(nullptr)
            , m_dxgiDevice(dxgiDevice)
        {
        }
        
        //
        // ID2D1Resource
        //
        
        IFACEMETHODIMP_(void) GetFactory(ID2D1Factory **factory) const override
        {
            if (!m_parentD2DFactory)
                m_parentD2DFactory = MakeMockD2DFactory();

            m_parentD2DFactory.CopyTo(factory);
        }

        //
        // ID2D1Device
        //

        IFACEMETHODIMP CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS deviceContextOptions, ID2D1DeviceContext** deviceContext) override
        {
            Assert::Fail(L"Unexpected call to CreateDeviceContext");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP CreatePrintControl(IWICImagingFactory *,IPrintDocumentPackageTarget *,const D2D1_PRINT_CONTROL_PROPERTIES *,ID2D1PrintControl **) override
        {
            Assert::Fail(L"Unexpected call to PrintDocumentPackageTarget");
            return E_NOTIMPL;
        }

        MOCK_METHOD1(SetMaximumTextureMemory, void(uint64_t));
        MOCK_METHOD0_CONST(GetMaximumTextureMemory, uint64_t());

        MOCK_METHOD1(ClearResources, void(uint32_t));

        //
        // ID2D1Device1
        //

        MOCK_METHOD1(SetRenderingPriority, void(D2D1_RENDERING_PRIORITY));
        MOCK_METHOD0(GetRenderingPriority, D2D1_RENDERING_PRIORITY());

        IFACEMETHODIMP CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS options, ID2D1DeviceContext1** deviceContext) override
        {
            if (MockCreateDeviceContext)
            {
                return ExceptionBoundary(
                    [&] { MockCreateDeviceContext(options, deviceContext); });
            }

            auto mockDeviceContext = Make<MockD2DDeviceContext>();
            return mockDeviceContext.CopyTo(deviceContext);
        }

        //
        // ID2D1Device2
        //
        IFACEMETHODIMP  CreateDeviceContext(
            D2D1_DEVICE_CONTEXT_OPTIONS options,
            ID2D1DeviceContext2 **deviceContext2)
        {
            Assert::Fail(L"Unexpected call to CreateDeviceContext");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP_(void) FlushDeviceContexts(
            ID2D1Bitmap *bitmap)
        {
            Assert::Fail(L"Unexpected call to FlushDeviceContexts");
        }


        IFACEMETHODIMP GetDxgiDevice(
            IDXGIDevice **dxgiDevice) override
        {
            return m_dxgiDevice.CopyTo(dxgiDevice);
        }

        //
        // ID2DDeviceWithDxgiDevice
        //

        virtual ComPtr<IDXGIDevice3> GetDxgiDevice() override
        {
            return m_dxgiDevice;
        }
    };
}
