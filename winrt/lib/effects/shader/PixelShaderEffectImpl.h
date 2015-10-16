// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects 
{
    class ISharedShaderState;
    class PixelShaderTransform;
    struct CoordinateMappingState;

    DEFINE_GUID(CLSID_PixelShaderEffect, 0x8db3047a, 0x84cc, 0x4152, 0xaf, 0x92, 0x50, 0xe4, 0xac, 0xb9, 0xd1, 0xfc);


    // Our custom Direct2D image effect.
    class PixelShaderEffectImpl : public RuntimeClass<RuntimeClassFlags<ClassicCom>, ID2D1EffectImpl>
                                , private LifespanTracker<PixelShaderEffectImpl>
    {
        ComPtr<ISharedShaderState> m_sharedState;
        ComPtr<PixelShaderTransform> m_shaderTransform;
        ComPtr<ID2D1EffectContext> m_effectContext;
        ComPtr<ID2D1TransformGraph> m_transformGraph;

        std::vector<BYTE> m_constants;
        bool m_constantsDirty;

        std::shared_ptr<CoordinateMappingState> m_coordinateMapping;

    public:
        PixelShaderEffectImpl();

        static void Register(ID2D1Factory1* factory);

        IFACEMETHOD(Initialize)(ID2D1EffectContext* effectContext, ID2D1TransformGraph* transformGraph) override;
        IFACEMETHOD(SetGraph)(ID2D1TransformGraph* transformGraph) override;
        IFACEMETHOD(PrepareForRender)(D2D1_CHANGE_TYPE changeType) override;

    private:
        HRESULT SetSharedStateProperty(IUnknown* sharedState);
        IUnknown* GetSharedStateProperty() const;

        HRESULT SetConstantsProperty(BYTE const* data, UINT32 dataSize);
        HRESULT GetConstantsProperty(BYTE* data, UINT32 dataSize, UINT32 *actualSize) const;

        HRESULT SetCoordinateMappingProperty(BYTE const* data, UINT32 dataSize);
        HRESULT GetCoordinateMappingProperty(BYTE* data, UINT32 dataSize, UINT32 *actualSize) const;
    };


    // Direct2D effect property indices (must match effectXml from PixelShaderEffectImpl.cpp).
    enum class PixelShaderEffectProperty
    {
        SharedState,
        Constants,
        CoordinateMapping,
    };

}}}}}
