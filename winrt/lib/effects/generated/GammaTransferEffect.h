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

    class GammaTransferEffect : public RuntimeClass<
        IGammaTransferEffect,
        MixIn<GammaTransferEffect, CanvasEffect>>,
        public CanvasEffect
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_Effects_GammaTransferEffect, BaseTrust);

    public:
        GammaTransferEffect();

        PROPERTY(RedAmplitude, float);
        PROPERTY(RedExponent, float);
        PROPERTY(RedOffset, float);
        PROPERTY(RedDisable, boolean);
        PROPERTY(GreenAmplitude, float);
        PROPERTY(GreenExponent, float);
        PROPERTY(GreenOffset, float);
        PROPERTY(GreenDisable, boolean);
        PROPERTY(BlueAmplitude, float);
        PROPERTY(BlueExponent, float);
        PROPERTY(BlueOffset, float);
        PROPERTY(BlueDisable, boolean);
        PROPERTY(AlphaAmplitude, float);
        PROPERTY(AlphaExponent, float);
        PROPERTY(AlphaOffset, float);
        PROPERTY(AlphaDisable, boolean);
        PROPERTY(ClampOutput, boolean);
        PROPERTY(Source, IEffectInput*);
    };
}}}}}
