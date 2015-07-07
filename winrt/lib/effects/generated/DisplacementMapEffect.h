// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

// This file was automatically generated. Please do not edit it manually.

#pragma once

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects 
{
    using namespace ::Microsoft::WRL;
    using namespace ABI::Microsoft::Graphics::Canvas;

    class DisplacementMapEffect : public RuntimeClass<
        IDisplacementMapEffect,
        MixIn<DisplacementMapEffect, CanvasEffect>>,
        public CanvasEffect
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_Effects_DisplacementMapEffect, BaseTrust);

    public:
        DisplacementMapEffect();

        EFFECT_PROPERTY(Amount, float);
        EFFECT_PROPERTY(XChannelSelect, EffectChannelSelect);
        EFFECT_PROPERTY(YChannelSelect, EffectChannelSelect);
        EFFECT_PROPERTY(Source, IGraphicsEffectSource*);
        EFFECT_PROPERTY(Displacement, IGraphicsEffectSource*);

        EFFECT_PROPERTY_MAPPING();
    };
}}}}}
