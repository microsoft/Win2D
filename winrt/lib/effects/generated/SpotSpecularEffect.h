// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may
// not use these files except in compliance with the License. You may obtain
// a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
// License for the specific language governing permissions and limitations
// under the License.

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
        SpotSpecularEffect();

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
        EFFECT_PROPERTY(Source, IGraphicsEffectSource*);
    };
}}}}}
