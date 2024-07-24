// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

// This file was automatically generated. Please do not edit it manually.

#pragma once

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects 
{
    using namespace ::Microsoft::WRL;
    using namespace ABI::Microsoft::Graphics::Canvas;

    class PosterizeEffect : public RuntimeClass<
        IPosterizeEffect,
        MixIn<PosterizeEffect, CanvasEffect>>,
        public CanvasEffect
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_Effects_PosterizeEffect, BaseTrust);

    public:
        PosterizeEffect(ICanvasDevice* device = nullptr, ID2D1Effect* effect = nullptr);

        static IID const& EffectId() { return CLSID_D2D1Posterize; }

        EFFECT_PROPERTY(RedValueCount, int32_t);
        EFFECT_PROPERTY(GreenValueCount, int32_t);
        EFFECT_PROPERTY(BlueValueCount, int32_t);
        EFFECT_PROPERTY(Source, IGraphicsEffectSource*);

        EFFECT_PROPERTY_MAPPING();
    };
}}}}}
