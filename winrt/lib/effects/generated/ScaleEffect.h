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
    };
}}}}}
