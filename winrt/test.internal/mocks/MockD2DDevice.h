// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#include "stubs\StubDxgiDevice.h"

namespace canvas
{
    // Forward declaration to break circular dependency (defined in MockD2DFactory.h)
    ComPtr<ID2D1Factory2> MakeMockD2DFactory();


    [uuid(D5B2FFD5-882E-4CB5-98FA-2342E52FC6F2)]
    class ID2DDeviceWithDxgiDevice : public IUnknown
    {
    public:
        virtual ComPtr<IDXGIDevice3> GetDxgiDevice() = 0;
    };
    

    class MockD2DDevice : public RuntimeClass<
        RuntimeClassFlags<ClassicCom>,
        ChainInterfaces<
#if WINVER > _WIN32_WINNT_WINBLUE
        ID2D1Device2,
#endif
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

        IFACEMETHODIMP_(void) SetMaximumTextureMemory(uint64_t) override
        {
            Assert::Fail(L"Unexpected call to SetMaximumTextureMemory");
        }

        IFACEMETHODIMP_(uint64_t) GetMaximumTextureMemory() const override
        {
            Assert::Fail(L"Unexpected call to GetMaximumTextureMemory");
            return 0L;
        }

        IFACEMETHODIMP_(void) ClearResources(uint32_t) override
        {
            Assert::Fail(L"Unexpected call to ClearResources");
        }

        //
        // ID2D1Device1
        //

        IFACEMETHODIMP_(D2D1_RENDERING_PRIORITY) GetRenderingPriority() override
        {
            Assert::Fail(L"Unexpected call to GetRenderingPriority");
            return (D2D1_RENDERING_PRIORITY)0;
        }

        IFACEMETHODIMP_(void) SetRenderingPriority(D2D1_RENDERING_PRIORITY) override
        {
            Assert::Fail(L"Unexpected call to SetRenderingPriority");
        }

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

#if WINVER > _WIN32_WINNT_WINBLUE
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
            IDXGIDevice **dxgiDevice)
        {
            Assert::Fail(L"Unexpected call to GetDxgiDevice");
            return E_NOTIMPL;
        }
#endif

        //
        // ID2DDeviceWithDxgiDevice
        //

        virtual ComPtr<IDXGIDevice3> GetDxgiDevice() override
        {
            return m_dxgiDevice;
        }
    };
}
