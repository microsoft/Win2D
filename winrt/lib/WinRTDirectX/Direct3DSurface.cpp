// Copyright (c) Microsoft Corporation.  All rights reserved

#include "pch.h"

using namespace Microsoft::WRL;

namespace canvas
{
    [uuid(4D1CE3D8-3EED-4D43-8CDA-1C4A1190844F)]
    class IDirect3DSurfaceInternal : public IUnknown
    {
    public:
        virtual void GetDXGIInterface(REFIID iid, void** p) = 0;
    };

    class Direct3DSurface : public RuntimeClass<
        RuntimeClassFlags<WinRtClassicComMix>,
        ABI::Microsoft::Graphics::Canvas::DirectX::Direct3D11::IDirect3DSurface,
        ABI::Windows::Foundation::IClosable,
        IDirect3DSurfaceInternal>
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_DirectX_Direct3D11_Direct3DSurface, BaseTrust);

        ClosablePtr<IDXGISurface> m_DxgiSurface;

    public:
        Direct3DSurface(IDXGISurface* surface)
        {
            if (surface == nullptr)
                throw InvalidArgException();

            m_DxgiSurface = surface;
        }

        // IClosable
        IFACEMETHOD(Close)() override
        {
            m_DxgiSurface.Close();
            return S_OK;
        }

        // IDirect3DSurface
        IFACEMETHOD(get_Description)(
            _Out_ ABI::Microsoft::Graphics::Canvas::DirectX::Direct3D11::Direct3DSurfaceDescription* value) override
        {
            return ExceptionBoundary(
                [&]()
                {
                    using ABI::Microsoft::Graphics::Canvas::DirectX::Direct3D11::Direct3DSurfaceDescription;
                    using ABI::Microsoft::Graphics::Canvas::DirectX::Direct3D11::Direct3DMultisampleDescription;

                    if (value == nullptr)
                        throw InvalidArgException();

                    auto& surface = m_DxgiSurface.EnsureNotClosed();

                    //
                    // IDXGISurface::GetDesc() expects a DXGI_SURFACE_DESC.
                    // We've cunningly arranged it so that
                    // Direct3DSurfaceDescription has exactly the same binary
                    // layout as DXGI_SURFACE_DESC, so we can pass one in
                    // directly and so avoid an unnecessary copy.
                    //
                    // These static asserts verify that the binary layout really
                    // does match.
                    //
                    static_assert(sizeof(DXGI_SURFACE_DESC) == sizeof(Direct3DSurfaceDescription), "Direct3DSurfaceDescription layout must match DXGI_SURFACE_DESC layout");
                    static_assert(offsetof(DXGI_SURFACE_DESC, Width) == offsetof(Direct3DSurfaceDescription, Width), "Direct3DSurfaceDescription layout must match DXGI_SURFACE_DESC layout");
                    static_assert(offsetof(DXGI_SURFACE_DESC, Height) == offsetof(Direct3DSurfaceDescription, Height), "Direct3DSurfaceDescription layout must match DXGI_SURFACE_DESC layout");
                    static_assert(offsetof(DXGI_SURFACE_DESC, Format) == offsetof(Direct3DSurfaceDescription, Format), "Direct3DSurfaceDescription layout must match DXGI_SURFACE_DESC layout");
                    static_assert(offsetof(DXGI_SURFACE_DESC, SampleDesc) == offsetof(Direct3DSurfaceDescription, MultisampleDescription), "Direct3DSurfaceDescription layout must match DXGI_SURFACE_DESC layout");
                    static_assert(offsetof(DXGI_SAMPLE_DESC, Count) == offsetof(Direct3DMultisampleDescription, Count), "GraphicsMultisampleDescription layout must match DXGI_SAMPLE_DESC layout");
                    static_assert(offsetof(DXGI_SAMPLE_DESC, Quality) == offsetof(Direct3DMultisampleDescription, Quality), "GraphicsMultisampleDescription layout must match DXGI_SAMPLE_DESC layout");

                    ThrowIfFailed(surface->GetDesc(reinterpret_cast<DXGI_SURFACE_DESC*>(value)));
                });
        }

        // IDirect3DSurfaceInternal
        virtual void GetDXGIInterface(REFIID iid, void** p) override
        {
            auto& surface = m_DxgiSurface.EnsureNotClosed();
            ThrowIfFailed(surface.CopyTo(iid, p));
        }        
    };
}


_Use_decl_annotations_
STDAPI CreateDirect3D11SurfaceFromDXGISurface(
    IDXGISurface* dxgiSurface,
    IInspectable** inspectableDirect3DSurface)
{
    return ExceptionBoundary(
        [&]()
        {
            ComPtr<canvas::Direct3DSurface> direct3DSurface = Make<canvas::Direct3DSurface>(dxgiSurface);
            if (!direct3DSurface)
                ThrowIfFailed(E_OUTOFMEMORY);

            ThrowIfFailed(direct3DSurface.CopyTo(inspectableDirect3DSurface));
        });
}


_Use_decl_annotations_
STDAPI GetDXGIInterfaceFromDirect3D11Surface(
    IInspectable* direct3DSurface,
    REFIID iid,
    void** p)
{
    return ExceptionBoundary(
        [&]()
        {
            ComPtr<canvas::IDirect3DSurfaceInternal> surfaceInternal;
            ThrowIfFailed(direct3DSurface->QueryInterface(surfaceInternal.GetAddressOf()));
            surfaceInternal->GetDXGIInterface(iid, p);
        });
}
