// Copyright (c) Microsoft Corporation.  All rights reserved

#include "pch.h"

#include "GraphicsSurface.h"

namespace dxrt
{
    GraphicsSurface::GraphicsSurface(IDXGISurface* dxgiSurface)
    {
        if (dxgiSurface == nullptr)
            throw ComException(E_INVALIDARG);

        m_DxgiSurface = dxgiSurface;
    }


    IFACEMETHODIMP GraphicsSurface::Close()
    {
        m_DxgiSurface.Close();
        return S_OK;
    }


    _Use_decl_annotations_
    IFACEMETHODIMP GraphicsSurface::get_Description(GraphicsSurfaceDescription* desc)
    {
        return ExceptionBoundary(
            [&]()
            {
                if (desc == nullptr)
                    throw ComException(E_INVALIDARG);

                auto& surface = m_DxgiSurface.EnsureNotClosed();

                //
                // IDXGISurface::GetDesc() expects a DXGI_SURFACE_DESC.  We've
                // cunningly arranged it so that GraphicsSurfaceDescription has
                // exactly the same binary layout as DXGI_SURFACE_DESC, so we
                // can pass one in directly and so avoid an unnecessary copy.
                //
                // These static asserts verify that the binary layout really
                // does match.
                //
                static_assert(sizeof(DXGI_SURFACE_DESC) == sizeof(GraphicsSurfaceDescription), "GraphicsSurfaceDescription layout must match DXGI_SURFACE_DESC layout");
                static_assert(offsetof(DXGI_SURFACE_DESC, Width) == offsetof(GraphicsSurfaceDescription, Width), "GraphicsSurfaceDescription layout must match DXGI_SURFACE_DESC layout");
                static_assert(offsetof(DXGI_SURFACE_DESC, Height) == offsetof(GraphicsSurfaceDescription, Height), "GraphicsSurfaceDescription layout must match DXGI_SURFACE_DESC layout");
                static_assert(offsetof(DXGI_SURFACE_DESC, Format) == offsetof(GraphicsSurfaceDescription, Format), "GraphicsSurfaceDescription layout must match DXGI_SURFACE_DESC layout");
                static_assert(offsetof(DXGI_SURFACE_DESC, SampleDesc) == offsetof(GraphicsSurfaceDescription, SampleDesc), "GraphicsSurfaceDescription layout must match DXGI_SURFACE_DESC layout");
                static_assert(offsetof(DXGI_SAMPLE_DESC, Count) == offsetof(GraphicsMultisampleDescription, Count), "GraphicsMultisampleDescription layout must match DXGI_SAMPLE_DESC layout");
                static_assert(offsetof(DXGI_SAMPLE_DESC, Quality) == offsetof(GraphicsMultisampleDescription, Quality), "GraphicsMultisampleDescription layout must match DXGI_SAMPLE_DESC layout");

                ThrowIfFailed(surface->GetDesc(reinterpret_cast<DXGI_SURFACE_DESC*>(desc)));
            });
    }

    _Use_decl_annotations_
    IFACEMETHODIMP GraphicsSurface::get_EvictionPriority(uint32_t* value)
    {
        return ExceptionBoundary(
            [&]()
            {
                auto& surface = m_DxgiSurface.EnsureNotClosed();

                ComPtr<IDXGIResource> resource;
                ThrowIfFailed(surface.As(&resource));
                ThrowIfFailed(resource->GetEvictionPriority(value));
            });
    }


    _Use_decl_annotations_
    IFACEMETHODIMP GraphicsSurface::put_EvictionPriority(uint32_t value)
    {
        return ExceptionBoundary(
            [&]()
            {
                auto& surface = m_DxgiSurface.EnsureNotClosed();

                ComPtr<IDXGIResource> resource;
                ThrowIfFailed(surface.As(&resource));
                ThrowIfFailed(resource->SetEvictionPriority(value));
            });
    }


    _Use_decl_annotations_
    void GraphicsSurface::GetDXGIInterface(REFIID iid, void** p)
    {
        auto& surface = m_DxgiSurface.EnsureNotClosed();
        ThrowIfFailed(surface.CopyTo(iid, p));
    }


    //
    // Graphics surface statics
    //

    class GraphicsSurfaceStatics : public ActivationFactory<IGraphicsSurfaceStatics>
    {
        InspectableClassStatic(RuntimeClass_Microsoft_DirectX_GraphicsSurface, BaseTrust);

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

    ActivatableStaticOnlyFactory(GraphicsSurfaceStatics);
}
