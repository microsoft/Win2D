// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

// This file was automatically generated. Please do not edit it manually.

#pragma once

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects 
{
    using namespace ::Microsoft::WRL;
    using namespace ABI::Microsoft::Graphics::Canvas;

    class ScaleEffect : public RuntimeClass<
        IScaleEffect,
        MixIn<ScaleEffect, CanvasEffect>>,
        public CanvasEffect
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_Effects_ScaleEffect, BaseTrust);

    public:
        ScaleEffect();

        EFFECT_PROPERTY(Scale, Numerics::Vector2);
        EFFECT_PROPERTY(CenterPoint, Numerics::Vector2);
        EFFECT_PROPERTY(InterpolationMode, CanvasImageInterpolation);
        EFFECT_PROPERTY(BorderMode, EffectBorderMode);
        EFFECT_PROPERTY(Sharpness, float);
        EFFECT_PROPERTY(Source, IGraphicsEffectSource*);

        EFFECT_PROPERTY_MAPPING();
    };
}}}}}
