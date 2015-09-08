// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

// This file was automatically generated. Please do not edit it manually.

#pragma once

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects 
{
    using namespace ::Microsoft::WRL;
    using namespace ABI::Microsoft::Graphics::Canvas;

    class ArithmeticCompositeEffect : public RuntimeClass<
        IArithmeticCompositeEffect,
        MixIn<ArithmeticCompositeEffect, CanvasEffect>>,
        public CanvasEffect
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_Effects_ArithmeticCompositeEffect, BaseTrust);

    public:
        ArithmeticCompositeEffect(ID2D1Effect* effect = nullptr);

        static IID const& EffectId() { return CLSID_D2D1ArithmeticComposite; }

        EFFECT_PROPERTY(ClampOutput, boolean);
        EFFECT_PROPERTY(MultiplyAmount, float);
        EFFECT_PROPERTY(Source1Amount, float);
        EFFECT_PROPERTY(Source2Amount, float);
        EFFECT_PROPERTY(Offset, float);
        EFFECT_PROPERTY(Source1, IGraphicsEffectSource*);
        EFFECT_PROPERTY(Source2, IGraphicsEffectSource*);

        EFFECT_PROPERTY_MAPPING();
        EFFECT_PROPERTY_MAPPING_HANDCODED();
    };
}}}}}
