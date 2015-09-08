// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

// This file was automatically generated. Please do not edit it manually.

#pragma once

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects 
{
    using namespace ::Microsoft::WRL;
    using namespace ABI::Microsoft::Graphics::Canvas;

    class CompositeEffect : public RuntimeClass<
        ICompositeEffect,
        MixIn<CompositeEffect, CanvasEffect>>,
        public CanvasEffect
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_Effects_CompositeEffect, BaseTrust);

    public:
        CompositeEffect(ID2D1Effect* effect = nullptr);

        static IID const& EffectId() { return CLSID_D2D1Composite; }

        EFFECT_PROPERTY(Mode, CanvasComposite);
        EFFECT_SOURCES_PROPERTY();

        EFFECT_PROPERTY_MAPPING();
    };
}}}}}
