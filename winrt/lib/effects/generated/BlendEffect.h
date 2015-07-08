// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

// This file was automatically generated. Please do not edit it manually.

#pragma once

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects 
{
    using namespace ::Microsoft::WRL;
    using namespace ABI::Microsoft::Graphics::Canvas;

    class BlendEffect : public RuntimeClass<
        IBlendEffect,
        MixIn<BlendEffect, CanvasEffect>>,
        public CanvasEffect
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_Effects_BlendEffect, BaseTrust);

    public:
        BlendEffect();

        EFFECT_PROPERTY(Mode, BlendEffectMode);
        EFFECT_PROPERTY(Background, IGraphicsEffectSource*);
        EFFECT_PROPERTY(Foreground, IGraphicsEffectSource*);

        EFFECT_PROPERTY_MAPPING();
    };
}}}}}
