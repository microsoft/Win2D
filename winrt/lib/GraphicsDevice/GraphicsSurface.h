// Copyright (c) Microsoft Corporation.  All rights reserved

#pragma once

namespace dxrt
{
    using namespace Microsoft::WRL;
    using namespace ABI::Microsoft::DirectX;

    [uuid(4D1CE3D8-3EED-4D43-8CDA-1C4A1190844F)]
    class IDirectX11SurfaceInternal : public IUnknown
    {
    public:
        virtual void GetDXGIInterface(REFIID iid, void** p) = 0;
    };

    class DirectX11Surface : public RuntimeClass<
        RuntimeClassFlags<WinRtClassicComMix>,
        IDirectX11Surface,
        ABI::Windows::Foundation::IClosable,
        IDirectX11SurfaceInternal>
    {
        InspectableClass(RuntimeClass_Microsoft_DirectX_DirectX11Surface, BaseTrust);

        ClosablePtr<IDXGISurface> m_DxgiSurface;

    public:
        DirectX11Surface(IDXGISurface* surface);

        // IClosable
        IFACEMETHOD(Close)() override;

        // IDirectX11Surface
        IFACEMETHOD(get_Description)(
            _Out_ DirectX11SurfaceDescription* value) override;

        IFACEMETHOD(get_EvictionPriority)(
            _Out_ uint32_t* value) override;

        IFACEMETHOD(put_EvictionPriority)(
            _In_ uint32_t value) override;

        // IDirectX11SurfaceInternal
        virtual void GetDXGIInterface(REFIID iid, void** p) override;
    };
}
