// Copyright (c) Microsoft Corporation.  All rights reserved

#pragma once

namespace canvas
{
    class MockD2DDevice : public RuntimeClass<
        RuntimeClassFlags<ClassicCom>,
        ChainInterfaces<ID2D1Device1, ID2D1Device, ID2D1Resource>>
    {
    public:
        //
        // ID2D1Resource
        //

        IFACEMETHODIMP_(void) GetFactory(ID2D1Factory **) const override
        {
            Assert::Fail(L"Unexpected call to GetFactory");
        }

        //
        // ID2D1Device
        //

        IFACEMETHODIMP CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS,ID2D1DeviceContext **) override
        {
            Assert::Fail(L"Unexpected call to D2D1DeviceContext");
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

        IFACEMETHODIMP CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS,ID2D1DeviceContext1 **) override
        {
            Assert::Fail(L"Unexpected call to D2D1DeviceContext1");
            return E_NOTIMPL;
        }
    };
}
