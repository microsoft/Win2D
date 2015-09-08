// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

// This file was automatically generated. Please do not edit it manually.

#pragma once

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects 
{
    using namespace ::Microsoft::WRL;
    using namespace ABI::Microsoft::Graphics::Canvas;

    class TurbulenceEffect : public RuntimeClass<
        ITurbulenceEffect,
        MixIn<TurbulenceEffect, CanvasEffect>>,
        public CanvasEffect
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_Effects_TurbulenceEffect, BaseTrust);

    public:
        TurbulenceEffect(ID2D1Effect* effect = nullptr);

        static IID const& EffectId() { return CLSID_D2D1Turbulence; }

        EFFECT_PROPERTY(Offset, Numerics::Vector2);
        EFFECT_PROPERTY(Size, Numerics::Vector2);
        EFFECT_PROPERTY(Frequency, Numerics::Vector2);
        EFFECT_PROPERTY(Octaves, int32_t);
        EFFECT_PROPERTY(Seed, int32_t);
        EFFECT_PROPERTY(Noise, TurbulenceEffectNoise);
        EFFECT_PROPERTY(Tileable, boolean);

        EFFECT_PROPERTY_MAPPING();
    };
}}}}}
