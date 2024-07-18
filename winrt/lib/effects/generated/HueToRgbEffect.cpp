// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

// This file was automatically generated. Please do not edit it manually.

#include "pch.h"
#include "HueToRgbEffect.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects
{
    HueToRgbEffect::HueToRgbEffect(ICanvasDevice* device, ID2D1Effect* effect)
        : CanvasEffect(EffectId(), 1, 1, true, device, effect, static_cast<IHueToRgbEffect*>(this))
    {
        if (!effect)
        {
            // Set default values
            SetBoxedProperty<uint32_t>(D2D1_HUETORGB_PROP_INPUT_COLOR_SPACE, EffectHueColorSpace::Hsv);
        }
    }

    IMPLEMENT_EFFECT_PROPERTY(HueToRgbEffect,
        SourceColorSpace,
        uint32_t,
        EffectHueColorSpace,
        D2D1_HUETORGB_PROP_INPUT_COLOR_SPACE)

    IMPLEMENT_EFFECT_SOURCE_PROPERTY(HueToRgbEffect,
        Source,
        0)

    IMPLEMENT_EFFECT_PROPERTY_MAPPING(HueToRgbEffect,
        { L"SourceColorSpace", D2D1_HUETORGB_PROP_INPUT_COLOR_SPACE, GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT })

    ActivatableClassWithFactory(HueToRgbEffect, ::SimpleAgileActivationFactory<HueToRgbEffect>);
}}}}}
