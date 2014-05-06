// Copyright (c) Microsoft Corporation.  All rights reserved

#pragma once

namespace dxrt
{
    using namespace Microsoft::WRL;
    using namespace ABI::Microsoft::DirectX;

    [uuid(0A55F0AC-0BDD-4CFA-A9E7-8B2743AD33B7)]
    class IGraphicsDeviceInternal : public IUnknown
    {
    public:
        virtual void GetDXGIInterface(REFIID iid, void** p) = 0;
    };

    class GraphicsDevice : public RuntimeClass<
        RuntimeClassFlags<WinRtClassicComMix>,
        IGraphicsDevice,
        ABI::Windows::Foundation::IClosable,
        IGraphicsDeviceInternal>
    {
        InspectableClass(RuntimeClass_Microsoft_DirectX_GraphicsDevice, BaseTrust);

        ClosablePtr<IDXGIDevice3> m_DxgiDevice;

    public:
        GraphicsDevice(IDXGIDevice* device);

        // IClosable
        IFACEMETHOD(Close)() override;

        // IGraphicsDevice
        IFACEMETHOD(Trim)() override;

        // IGraphicsDeviceInternal
        virtual void GetDXGIInterface(REFIID iid, void** p) override;
    };
}
