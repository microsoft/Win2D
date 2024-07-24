// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

// This file was automatically generated. Please do not edit it manually.

#pragma once

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects 
{
    using namespace ::Microsoft::WRL;
    using namespace ABI::Microsoft::Graphics::Canvas;

    class HighlightsAndShadowsEffect : public RuntimeClass<
        IHighlightsAndShadowsEffect,
        MixIn<HighlightsAndShadowsEffect, CanvasEffect>>,
        public CanvasEffect
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_Effects_HighlightsAndShadowsEffect, BaseTrust);

    public:
        HighlightsAndShadowsEffect(ICanvasDevice* device = nullptr, ID2D1Effect* effect = nullptr);

        static IID const& EffectId() { return CLSID_D2D1HighlightsShadows; }

        EFFECT_PROPERTY(Highlights, float);
        EFFECT_PROPERTY(Shadows, float);
        EFFECT_PROPERTY(Clarity, float);
        EFFECT_PROPERTY(MaskBlurAmount, float);
        EFFECT_PROPERTY(SourceIsLinearGamma, boolean);
        EFFECT_PROPERTY(Source, IGraphicsEffectSource*);

        EFFECT_PROPERTY_MAPPING();
        EFFECT_PROPERTY_MAPPING_HANDCODED();
    };
}}}}}
