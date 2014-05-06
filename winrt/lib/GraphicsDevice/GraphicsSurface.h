// Copyright (c) Microsoft Corporation.  All rights reserved

#pragma once

namespace dxrt
{
    using namespace Microsoft::WRL;
    using namespace ABI::Microsoft::DirectX;

    [uuid(4D1CE3D8-3EED-4D43-8CDA-1C4A1190844F)]
    class IGraphicsSurfaceInternal : public IUnknown
    {
    public:
        virtual void GetDXGIInterface(REFIID iid, void** p) = 0;
    };

    class GraphicsSurface : public RuntimeClass<
        RuntimeClassFlags<WinRtClassicComMix>,
        IGraphicsSurface,
        ABI::Windows::Foundation::IClosable,
        IGraphicsSurfaceInternal>
    {
        InspectableClass(RuntimeClass_Microsoft_DirectX_GraphicsSurface, BaseTrust);

        ClosablePtr<IDXGISurface> m_DxgiSurface;

    public:
        GraphicsSurface(IDXGISurface* surface);

        // IClosable
        IFACEMETHOD(Close)() override;

        // IGraphicsSurface
        IFACEMETHOD(get_Description)(
            _Out_ GraphicsSurfaceDescription* value) override;

        IFACEMETHOD(get_EvictionPriority)(
            _Out_ uint32_t* value) override;

        IFACEMETHOD(put_EvictionPriority)(
            _In_ uint32_t value) override;

        // IGraphicsSurfaceInternal
        virtual void GetDXGIInterface(REFIID iid, void** p) override;
    };
}
