// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

// This file was automatically generated. Please do not edit it manually.

#pragma once

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects 
{
    using namespace ::Microsoft::WRL;
    using namespace ABI::Microsoft::Graphics::Canvas;

    class DiscreteTransferEffect : public RuntimeClass<
        IDiscreteTransferEffect,
        MixIn<DiscreteTransferEffect, CanvasEffect>>,
        public CanvasEffect
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_Effects_DiscreteTransferEffect, BaseTrust);

    public:
        DiscreteTransferEffect();

        EFFECT_ARRAY_PROPERTY(RedTable, float);
        EFFECT_PROPERTY(RedDisable, boolean);
        EFFECT_ARRAY_PROPERTY(GreenTable, float);
        EFFECT_PROPERTY(GreenDisable, boolean);
        EFFECT_ARRAY_PROPERTY(BlueTable, float);
        EFFECT_PROPERTY(BlueDisable, boolean);
        EFFECT_ARRAY_PROPERTY(AlphaTable, float);
        EFFECT_PROPERTY(AlphaDisable, boolean);
        EFFECT_PROPERTY(ClampOutput, boolean);
        EFFECT_PROPERTY(Source, IGraphicsEffectSource*);

        EFFECT_PROPERTY_MAPPING();
    };
}}}}}
