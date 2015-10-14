// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects 
{
    class ISharedShaderState;

    template<typename TKey, typename TValue> struct PixelShaderEffectPropertyMapTraits;


    // WinRT activation factory.
    class PixelShaderEffectFactory : public ActivationFactory<IPixelShaderEffectFactory>
                                   , private LifespanTracker<PixelShaderEffectFactory>
    {
        InspectableClassStatic(RuntimeClass_Microsoft_Graphics_Canvas_Effects_PixelShaderEffect, BaseTrust);

    public:
        IFACEMETHOD(Create)(uint32_t shaderCodeCount, BYTE* shaderCode, IPixelShaderEffect** effect) override;
    };


    // Public Win2D API surface for using custom effects.
    class PixelShaderEffect : public RuntimeClass<IPixelShaderEffect, MixIn<PixelShaderEffect, CanvasEffect>>
                            , public CanvasEffect
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_Effects_PixelShaderEffect, BaseTrust);

        ComPtr<ISharedShaderState> m_sharedState;

        // Traits describe how to expose a view of our constant buffer as an IMap<> collection.
        friend PixelShaderEffectPropertyMapTraits<HSTRING, IInspectable*>;
        typedef Map<HSTRING, IInspectable*, PixelShaderEffectPropertyMapTraits> PropertyMap;
        ComPtr<PropertyMap> m_propertyMap;

    public:
        PixelShaderEffect(ICanvasDevice* device, ID2D1Effect* effect);
        PixelShaderEffect(ICanvasDevice* device, ID2D1Effect* effect, ISharedShaderState* sharedState);

        virtual ~PixelShaderEffect();

        IFACEMETHOD(Close)() override;

        IFACEMETHOD(get_Properties)(IMap<HSTRING, IInspectable*>** value) override;

        EFFECT_PROPERTY(Source1, IGraphicsEffectSource*);
        EFFECT_PROPERTY(Source2, IGraphicsEffectSource*);
        EFFECT_PROPERTY(Source3, IGraphicsEffectSource*);
        EFFECT_PROPERTY(Source4, IGraphicsEffectSource*);
        EFFECT_PROPERTY(Source5, IGraphicsEffectSource*);
        EFFECT_PROPERTY(Source6, IGraphicsEffectSource*);
        EFFECT_PROPERTY(Source7, IGraphicsEffectSource*);
        EFFECT_PROPERTY(Source8, IGraphicsEffectSource*);

        EFFECT_PROPERTY(Source1Mapping, SamplerCoordinateMapping);
        EFFECT_PROPERTY(Source2Mapping, SamplerCoordinateMapping);
        EFFECT_PROPERTY(Source3Mapping, SamplerCoordinateMapping);
        EFFECT_PROPERTY(Source4Mapping, SamplerCoordinateMapping);
        EFFECT_PROPERTY(Source5Mapping, SamplerCoordinateMapping);
        EFFECT_PROPERTY(Source6Mapping, SamplerCoordinateMapping);
        EFFECT_PROPERTY(Source7Mapping, SamplerCoordinateMapping);
        EFFECT_PROPERTY(Source8Mapping, SamplerCoordinateMapping);

        EFFECT_PROPERTY(MaxSamplerOffset, int);

        IFACEMETHOD(IsSupported)(ICanvasDevice* device, boolean* result) override;

    protected:
        bool IsSupported(ICanvasDevice* device);

        virtual bool Realize(GetImageFlags flags, float targetDpi, ID2D1DeviceContext* deviceContext) override;
        virtual void Unrealize(unsigned int skipSourceIndex, bool skipAllSources) override;

        IFACEMETHOD(GetSource)(unsigned int index, IGraphicsEffectSource** source) override;
        void SetSource(unsigned int index, IGraphicsEffectSource* source);

        void SetProperty(HSTRING name, IInspectable* boxedValue);

        HRESULT GetCoordinateMapping(unsigned index, SamplerCoordinateMapping* value);
        HRESULT SetCoordinateMapping(unsigned index, SamplerCoordinateMapping value);

        void SetD2DConstants();
        void SetD2DCoordinateMapping();
    };

}}}}}
