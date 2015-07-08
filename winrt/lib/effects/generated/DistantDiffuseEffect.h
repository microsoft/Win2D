// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

// This file was automatically generated. Please do not edit it manually.

#pragma once

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects 
{
    using namespace ::Microsoft::WRL;
    using namespace ABI::Microsoft::Graphics::Canvas;

    class DistantDiffuseEffect : public RuntimeClass<
        IDistantDiffuseEffect,
        MixIn<DistantDiffuseEffect, CanvasEffect>>,
        public CanvasEffect
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_Effects_DistantDiffuseEffect, BaseTrust);

    public:
        DistantDiffuseEffect();

        EFFECT_PROPERTY(Azimuth, float);
        EFFECT_PROPERTY(Elevation, float);
        EFFECT_PROPERTY(DiffuseAmount, float);
        EFFECT_PROPERTY(HeightMapScale, float);
        EFFECT_PROPERTY(LightColor, Color);
        EFFECT_PROPERTY(HeightMapKernelSize, Numerics::Vector2);
        EFFECT_PROPERTY(HeightMapInterpolationMode, CanvasImageInterpolation);
        EFFECT_PROPERTY(Source, IGraphicsEffectSource*);

        EFFECT_PROPERTY_MAPPING();
    };
}}}}}
