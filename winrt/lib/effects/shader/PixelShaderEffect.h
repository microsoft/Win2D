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

    protected:
        virtual bool Realize(GetImageFlags flags, float targetDpi, ID2D1DeviceContext* deviceContext) override;
        virtual void Unrealize(unsigned int skipSourceIndex, bool skipAllSources) override;

        IFACEMETHOD(GetSource)(unsigned int index, IGraphicsEffectSource** source) override;
        void SetSource(unsigned int index, IGraphicsEffectSource* source);

        void SetProperty(HSTRING name, IInspectable* boxedValue);
        void SetD2DConstants(ID2D1Effect* d2dEffect);
    };

}}}}}
