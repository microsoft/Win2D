// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

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
        GammaTransferEffect(ICanvasDevice* device = nullptr, ID2D1Effect* effect = nullptr);

        static IID const& EffectId() { return CLSID_D2D1GammaTransfer; }

        EFFECT_PROPERTY(RedAmplitude, float);
        EFFECT_PROPERTY(RedExponent, float);
        EFFECT_PROPERTY(RedOffset, float);
        EFFECT_PROPERTY(RedDisable, boolean);
        EFFECT_PROPERTY(GreenAmplitude, float);
        EFFECT_PROPERTY(GreenExponent, float);
        EFFECT_PROPERTY(GreenOffset, float);
        EFFECT_PROPERTY(GreenDisable, boolean);
        EFFECT_PROPERTY(BlueAmplitude, float);
        EFFECT_PROPERTY(BlueExponent, float);
        EFFECT_PROPERTY(BlueOffset, float);
        EFFECT_PROPERTY(BlueDisable, boolean);
        EFFECT_PROPERTY(AlphaAmplitude, float);
        EFFECT_PROPERTY(AlphaExponent, float);
        EFFECT_PROPERTY(AlphaOffset, float);
        EFFECT_PROPERTY(AlphaDisable, boolean);
        EFFECT_PROPERTY(ClampOutput, boolean);
        EFFECT_PROPERTY(Source, IGraphicsEffectSource*);

        EFFECT_PROPERTY_MAPPING();
    };
}}}}}
