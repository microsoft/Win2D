// Copyright (c) Microsoft Corporation.  All rights reserved

#pragma once

namespace canvas
{
    using namespace ABI::Microsoft::Graphics::Canvas;

    class MockCanvasDevice : public RuntimeClass<
        RuntimeClassFlags<WinRtClassicComMix>,
        ICanvasDevice,
        CloakedIid<ICanvasDeviceInternal>>
    {
    public:        
        std::function<ComPtr<ID2D1Device1>()> MockGetD2DDevice;

        //
        // ICanvasDevice
        //
        
        IFACEMETHODIMP RecoverLostDevice(ICanvasDevice **) override
        {
            Assert::Fail(L"Unexpected call to RecoverLostDevice");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP CreateCompatibleDeviceWithDebugLevel(CanvasDebugLevel,ICanvasDevice **) override
        {
            Assert::Fail(L"Unexpected call to CreateCompatibleDeviceWithDebugLevel");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP CreateCompatibleDeviceWithDirectX11Device(IDirectX11Device *,ICanvasDevice **) override
        {
            Assert::Fail(L"Unexpected call to CreateCompatibleDeviceWithDirectX11Device");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP CreateCompatibleDeviceWithDebugLevelAndDirectX11Device(CanvasDebugLevel,IDirectX11Device *,ICanvasDevice **) override
        {
            Assert::Fail(L"Unexpected call to CreateCompatibleDeviceWithDebugLevelAndDirectX11Device");
            return E_NOTIMPL;
        }


        IFACEMETHODIMP get_HardwareAcceleration(CanvasHardwareAcceleration *) override
        {
            Assert::Fail(L"Unexpected call to get_HardwareAcceleration");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP get_DirectX11Device(IDirectX11Device **) override
        {
            Assert::Fail(L"Unexpected call to get_DirectX11Device");
            return E_NOTIMPL;
        }

        //
        // ICanvasDeviceInternal
        //

        virtual ComPtr<ID2D1Device1> GetD2DDevice() override
        {
            if (!MockGetD2DDevice)
            {
                Assert::Fail(L"Unexpected call to GetDevice");
                throw ComException(E_NOTIMPL);
            }

            return MockGetD2DDevice();
        }
    };
}

