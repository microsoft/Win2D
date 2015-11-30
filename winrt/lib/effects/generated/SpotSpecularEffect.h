// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

// This file was automatically generated. Please do not edit it manually.

#pragma once

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects 
{
    using namespace ::Microsoft::WRL;
    using namespace ABI::Microsoft::Graphics::Canvas;

    class SpotSpecularEffect : public RuntimeClass<
        ISpotSpecularEffect,
        MixIn<SpotSpecularEffect, CanvasEffect>>,
        public CanvasEffect
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_Effects_SpotSpecularEffect, BaseTrust);

    public:
        SpotSpecularEffect(ICanvasDevice* device = nullptr, ID2D1Effect* effect = nullptr);

        static IID const& EffectId() { return CLSID_D2D1SpotSpecular; }

        EFFECT_PROPERTY(LightPosition, Numerics::Vector3);
        EFFECT_PROPERTY(LightTarget, Numerics::Vector3);
        EFFECT_PROPERTY(Focus, float);
        EFFECT_PROPERTY(LimitingConeAngle, float);
        EFFECT_PROPERTY(SpecularExponent, float);
        EFFECT_PROPERTY(SpecularAmount, float);
        EFFECT_PROPERTY(HeightMapScale, float);
        EFFECT_PROPERTY(LightColor, Color);
        EFFECT_PROPERTY(HeightMapKernelSize, Numerics::Vector2);
        EFFECT_PROPERTY(HeightMapInterpolationMode, CanvasImageInterpolation);
        EFFECT_PROPERTY(LightColorHdr, Numerics::Vector4);
        EFFECT_PROPERTY(Source, IGraphicsEffectSource*);

        EFFECT_PROPERTY_MAPPING();
    };
}}}}}
