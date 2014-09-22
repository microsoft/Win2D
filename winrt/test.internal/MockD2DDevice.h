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

#include "../test.external/MockDxgiDevice.h" // TODO #997/#1429: move this file
#include "MockD2DDeviceContext.h"

namespace canvas
{
    [uuid(D5B2FFD5-882E-4CB5-98FA-2342E52FC6F2)]
    class ID2DDeviceWithDxgiDevice : public IUnknown
    {
    public:
        virtual ComPtr<IDXGIDevice3> GetDxgiDevice() = 0;
    };
    
    class MockD2DDevice : public RuntimeClass<
        RuntimeClassFlags<ClassicCom>,
        ChainInterfaces<ID2D1Device1, ID2D1Device, ID2D1Resource>,
        ID2DDeviceWithDxgiDevice>
    {
        ComPtr<ID2D1Factory2> m_parentD2DFactory;
        ComPtr<IDXGIDevice3> m_dxgiDevice;

    public:
        std::function<void(D2D1_DEVICE_CONTEXT_OPTIONS,ID2D1DeviceContext1**)> MockCreateDeviceContext;

        MockD2DDevice(ID2D1Factory2* parentD2DFactory = nullptr)
            : m_parentD2DFactory(parentD2DFactory)
            , m_dxgiDevice(Make<MockDxgiDevice>())
        {            
        }
        
        //
        // ID2D1Resource
        //
        
        IFACEMETHODIMP_(void) GetFactory(ID2D1Factory **factory) const override
        {
            if (!m_parentD2DFactory)
                Assert::Fail(L"Unexpected call to GetFactory");
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

        //
        // ID2DDeviceWithDxgiDevice
        //

        virtual ComPtr<IDXGIDevice3> GetDxgiDevice() override
        {
            return m_dxgiDevice;
        }
    };
}
