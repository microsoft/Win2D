// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

// This file was automatically generated. Please do not edit it manually.

#pragma once

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects 
{
    using namespace ::Microsoft::WRL;
    using namespace ABI::Microsoft::Graphics::Canvas;

    class LinearTransferEffect : public RuntimeClass<
        ILinearTransferEffect,
        MixIn<LinearTransferEffect, CanvasEffect>>,
        public CanvasEffect
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_Effects_LinearTransferEffect, BaseTrust);

    public:
        LinearTransferEffect(ID2D1Effect* effect = nullptr);

        static IID const& EffectId() { return CLSID_D2D1LinearTransfer; }

        EFFECT_PROPERTY(RedOffset, float);
        EFFECT_PROPERTY(RedSlope, float);
        EFFECT_PROPERTY(RedDisable, boolean);
        EFFECT_PROPERTY(GreenOffset, float);
        EFFECT_PROPERTY(GreenSlope, float);
        EFFECT_PROPERTY(GreenDisable, boolean);
        EFFECT_PROPERTY(BlueOffset, float);
        EFFECT_PROPERTY(BlueSlope, float);
        EFFECT_PROPERTY(BlueDisable, boolean);
        EFFECT_PROPERTY(AlphaOffset, float);
        EFFECT_PROPERTY(AlphaSlope, float);
        EFFECT_PROPERTY(AlphaDisable, boolean);
        EFFECT_PROPERTY(ClampOutput, boolean);
        EFFECT_PROPERTY(Source, IGraphicsEffectSource*);

        EFFECT_PROPERTY_MAPPING();
    };
}}}}}
