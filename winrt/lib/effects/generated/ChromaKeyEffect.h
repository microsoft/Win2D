// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

// This file was automatically generated. Please do not edit it manually.

#pragma once

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects 
{
    using namespace ::Microsoft::WRL;
    using namespace ABI::Microsoft::Graphics::Canvas;

    class ChromaKeyEffect : public RuntimeClass<
        IChromaKeyEffect,
        MixIn<ChromaKeyEffect, CanvasEffect>>,
        public CanvasEffect
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_Effects_ChromaKeyEffect, BaseTrust);

    public:
        ChromaKeyEffect(ICanvasDevice* device = nullptr, ID2D1Effect* effect = nullptr);

        static IID const& EffectId() { return CLSID_D2D1ChromaKey; }

        EFFECT_PROPERTY(Color, Color);
        EFFECT_PROPERTY(Tolerance, float);
        EFFECT_PROPERTY(InvertAlpha, boolean);
        EFFECT_PROPERTY(Feather, boolean);
        EFFECT_PROPERTY(ColorHdr, Numerics::Vector4);
        EFFECT_PROPERTY(Source, IGraphicsEffectSource*);

        EFFECT_PROPERTY_MAPPING();
    };
}}}}}
