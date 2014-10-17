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

    class ConvolveMatrixEffect : public RuntimeClass<
        IConvolveMatrixEffect,
        MixIn<ConvolveMatrixEffect, CanvasEffect>>,
        public CanvasEffect
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_Effects_ConvolveMatrixEffect, BaseTrust);

    public:
        ConvolveMatrixEffect();

        PROPERTY(KernelScale, Numerics::Vector2);
        PROPERTY(InterpolationMode, CanvasImageInterpolation);
        PROPERTY(KernelWidth, uint32_t);
        PROPERTY(KernelHeight, uint32_t);
        ARRAY_PROPERTY(KernelMatrix, float);
        PROPERTY(Divisor, float);
        PROPERTY(Offset, float);
        PROPERTY(KernelOffset, Numerics::Vector2);
        PROPERTY(PreserveAlpha, boolean);
        PROPERTY(BorderMode, EffectBorderMode);
        PROPERTY(ClampOutput, boolean);
        PROPERTY(Source, IEffectInput*);
    };
}}}}}
