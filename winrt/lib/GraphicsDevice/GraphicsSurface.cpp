// Copyright (c) Microsoft Corporation.  All rights reserved

#include "pch.h"

#include "GraphicsSurface.h"

namespace canvas
{
    DirectX11Surface::DirectX11Surface(IDXGISurface* dxgiSurface)
    {
        CheckInPointer(dxgiSurface);
        m_dxgiSurface = dxgiSurface;
    }


    IFACEMETHODIMP DirectX11Surface::Close()
    {
        m_dxgiSurface.Close();
        return S_OK;
    }


    _Use_decl_annotations_
    IFACEMETHODIMP DirectX11Surface::get_Description(DirectX11SurfaceDescription* desc)
    {
        return ExceptionBoundary(
            [&]()
            {
                CheckInPointer(desc);

                auto& surface = m_dxgiSurface.EnsureNotClosed();

                //
                // IDXGISurface::GetDesc() expects a DXGI_SURFACE_DESC.  We've
                // cunningly arranged it so that DirectX11SurfaceDescription has
                // exactly the same binary layout as DXGI_SURFACE_DESC, so we
                // can pass one in directly and so avoid an unnecessary copy.
                //
                // These static asserts verify that the binary layout really
                // does match.
                //
                static_assert(sizeof(DXGI_SURFACE_DESC) == sizeof(DirectX11SurfaceDescription), "DirectX11SurfaceDescription layout must match DXGI_SURFACE_DESC layout");
                static_assert(offsetof(DXGI_SURFACE_DESC, Width) == offsetof(DirectX11SurfaceDescription, Width), "DirectX11SurfaceDescription layout must match DXGI_SURFACE_DESC layout");
                static_assert(offsetof(DXGI_SURFACE_DESC, Height) == offsetof(DirectX11SurfaceDescription, Height), "DirectX11SurfaceDescription layout must match DXGI_SURFACE_DESC layout");
                static_assert(offsetof(DXGI_SURFACE_DESC, Format) == offsetof(DirectX11SurfaceDescription, Format), "DirectX11SurfaceDescription layout must match DXGI_SURFACE_DESC layout");
                static_assert(offsetof(DXGI_SURFACE_DESC, SampleDesc) == offsetof(DirectX11SurfaceDescription, MultisampleDescription), "DirectX11SurfaceDescription layout must match DXGI_SURFACE_DESC layout");
                static_assert(offsetof(DXGI_SAMPLE_DESC, Count) == offsetof(DirectX11MultisampleDescription, Count), "GraphicsMultisampleDescription layout must match DXGI_SAMPLE_DESC layout");
                static_assert(offsetof(DXGI_SAMPLE_DESC, Quality) == offsetof(DirectX11MultisampleDescription, Quality), "GraphicsMultisampleDescription layout must match DXGI_SAMPLE_DESC layout");

                ThrowIfFailed(surface->GetDesc(reinterpret_cast<DXGI_SURFACE_DESC*>(desc)));
            });
    }

    _Use_decl_annotations_
    IFACEMETHODIMP DirectX11Surface::get_EvictionPriority(uint32_t* value)
    {
        return ExceptionBoundary(
            [&]()
            {
                CheckInPointer(value);

                auto& surface = m_dxgiSurface.EnsureNotClosed();

                ComPtr<IDXGIResource> resource;
                ThrowIfFailed(surface.As(&resource));
                ThrowIfFailed(resource->GetEvictionPriority(value));
            });
    }


    _Use_decl_annotations_
    IFACEMETHODIMP DirectX11Surface::put_EvictionPriority(uint32_t value)
    {
        return ExceptionBoundary(
            [&]()
            {
                auto& surface = m_dxgiSurface.EnsureNotClosed();

                ComPtr<IDXGIResource> resource;
                ThrowIfFailed(surface.As(&resource));
                ThrowIfFailed(resource->SetEvictionPriority(value));
            });
    }


    _Use_decl_annotations_
    void DirectX11Surface::GetDXGIInterface(REFIID iid, void** p)
    {
        CheckAndClearOutPointer(p);
        auto& surface = m_dxgiSurface.EnsureNotClosed();
        ThrowIfFailed(surface.CopyTo(iid, p));
    }


    //
    // Graphics surface statics
    //

    class DirectX11SurfaceStatics : public ActivationFactory<IDirectX11SurfaceStatics>
    {
        InspectableClassStatic(RuntimeClass_Microsoft_Graphics_Canvas_DirectX11Surface, BaseTrust);

    public:
        IFACEMETHODIMP get_ResourcePriorityMinimum(
            _Out_ uint32_t* value) override
        {
            if (value == nullptr)
                return E_INVALIDARG;

            *value = DXGI_RESOURCE_PRIORITY_MINIMUM;
            return S_OK;
        }

        IFACEMETHODIMP get_ResourcePriorityLow(
            _Out_ uint32_t* value) override
        {
            if (value == nullptr)
                return E_INVALIDARG;

            *value = DXGI_RESOURCE_PRIORITY_LOW;
            return S_OK;
        }

        IFACEMETHODIMP get_ResourcePriorityNormal(
            _Out_ uint32_t* value) override
        {
            if (value == nullptr)
                return E_INVALIDARG;

            *value = DXGI_RESOURCE_PRIORITY_NORMAL;
            return S_OK;
        }

        IFACEMETHODIMP get_ResourcePriorityHigh(
            _Out_ uint32_t* value) override
        {
            if (value == nullptr)
                return E_INVALIDARG;

            *value = DXGI_RESOURCE_PRIORITY_HIGH;
            return S_OK;
        }

        IFACEMETHODIMP get_ResourcePriorityMaximum(
            _Out_ uint32_t* value) override
        {
            if (value == nullptr)
                return E_INVALIDARG;

            *value = DXGI_RESOURCE_PRIORITY_MAXIMUM;
            return S_OK;
        }
    };

    ActivatableStaticOnlyFactory(DirectX11SurfaceStatics);
}
