// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

// This file was automatically generated. Please do not edit it manually.

#pragma once

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects 
{
    using namespace ::Microsoft::WRL;
    using namespace ABI::Microsoft::Graphics::Canvas;

    class SpotDiffuseEffect : public RuntimeClass<
        ISpotDiffuseEffect,
        MixIn<SpotDiffuseEffect, CanvasEffect>>,
        public CanvasEffect
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_Effects_SpotDiffuseEffect, BaseTrust);

    public:
        SpotDiffuseEffect(ID2D1Effect* effect = nullptr);

        static IID const& EffectId() { return CLSID_D2D1SpotDiffuse; }

        EFFECT_PROPERTY(LightPosition, Numerics::Vector3);
        EFFECT_PROPERTY(LightTarget, Numerics::Vector3);
        EFFECT_PROPERTY(Focus, float);
        EFFECT_PROPERTY(LimitingConeAngle, float);
        EFFECT_PROPERTY(DiffuseAmount, float);
        EFFECT_PROPERTY(HeightMapScale, float);
        EFFECT_PROPERTY(LightColor, Color);
        EFFECT_PROPERTY(HeightMapKernelSize, Numerics::Vector2);
        EFFECT_PROPERTY(HeightMapInterpolationMode, CanvasImageInterpolation);
        EFFECT_PROPERTY(Source, IGraphicsEffectSource*);

        EFFECT_PROPERTY_MAPPING();
    };
}}}}}
