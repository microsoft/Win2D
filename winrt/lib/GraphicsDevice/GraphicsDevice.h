// Copyright (c) Microsoft Corporation.  All rights reserved

#pragma once

namespace canvas
{
    using namespace Microsoft::WRL;
    using namespace ABI::Microsoft::Graphics::Canvas;

    [uuid(0A55F0AC-0BDD-4CFA-A9E7-8B2743AD33B7)]
    class IDirectX11DeviceInternal : public IUnknown
    {
    public:
        virtual void GetDXGIInterface(REFIID iid, void** p) = 0;
    };

    class DirectX11Device : public RuntimeClass<
        RuntimeClassFlags<WinRtClassicComMix>,
        IDirectX11Device,
        ABI::Windows::Foundation::IClosable,
        IDirectX11DeviceInternal>
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_DirectX11Device, BaseTrust);

        ClosablePtr<IDXGIDevice3> m_dxgiDevice;

    public:
        DirectX11Device(IDXGIDevice* device);

        // IClosable
        IFACEMETHOD(Close)() override;

        // IDirectX11Device
        IFACEMETHOD(Trim)() override;

        // IDirectX11DeviceInternal
        virtual void GetDXGIInterface(REFIID iid, void** p) override;
    };
}
