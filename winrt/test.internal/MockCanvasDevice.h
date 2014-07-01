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

        IFACEMETHODIMP get_HardwareAcceleration(CanvasHardwareAcceleration *) override
        {
            Assert::Fail(L"Unexpected call to get_HardwareAcceleration");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP get_Direct3DDevice(IDirect3DDevice **) override
        {
            Assert::Fail(L"Unexpected call to get_Direct3DDevice");
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
                ThrowHR(E_NOTIMPL);
            }

            return MockGetD2DDevice();
        }

        virtual ComPtr<ID2D1SolidColorBrush> CreateSolidColorBrush(const D2D1_COLOR_F& color) override
        {
            Assert::Fail(L"Unexpected call to CreateSolidColorBrush");
            return nullptr;
        }
    };
}

