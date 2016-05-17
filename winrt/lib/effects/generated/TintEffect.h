// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

// This file was automatically generated. Please do not edit it manually.

#pragma once

#if (defined _WIN32_WINNT_WIN10) && (WINVER >= _WIN32_WINNT_WIN10)

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects 
{
    using namespace ::Microsoft::WRL;
    using namespace ABI::Microsoft::Graphics::Canvas;

    class TintEffect : public RuntimeClass<
        ITintEffect,
        MixIn<TintEffect, CanvasEffect>>,
        public CanvasEffect
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_Effects_TintEffect, BaseTrust);

    public:
        TintEffect(ICanvasDevice* device = nullptr, ID2D1Effect* effect = nullptr);

        static IID const& EffectId() { return CLSID_D2D1Tint; }

        EFFECT_PROPERTY(Color, Color);
        EFFECT_PROPERTY(ClampOutput, boolean);
        EFFECT_PROPERTY(ColorHdr, Numerics::Vector4);
        EFFECT_PROPERTY(Source, IGraphicsEffectSource*);

        EFFECT_PROPERTY_MAPPING();
    };

    class TintEffectFactory
        : public AgileActivationFactory<ITintEffectStatics>
        , private LifespanTracker<TintEffectFactory>
    {
        InspectableClassStatic(RuntimeClass_Microsoft_Graphics_Canvas_Effects_TintEffect, BaseTrust);

    public:
        IFACEMETHODIMP ActivateInstance(IInspectable**) override;
        IFACEMETHOD(get_IsSupported)(boolean* value) override;
    };
}}}}}

#endif // _WIN32_WINNT_WIN10
