// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

// This file was automatically generated. Please do not edit it manually.

#include "pch.h"
#include "ColorSourceEffect.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects
{
    ColorSourceEffect::ColorSourceEffect(ICanvasDevice* device, ID2D1Effect* effect)
        : CanvasEffect(EffectId(), 1, 0, true, device, effect, static_cast<IColorSourceEffect*>(this))
    {
        if (!effect)
        {
            // Set default values
            SetBoxedProperty<float[4]>(D2D1_FLOOD_PROP_COLOR, Color{ 255, 0, 0, 0 });
        }
    }

    IMPLEMENT_EFFECT_PROPERTY(ColorSourceEffect,
        Color,
        float[4],
        Color,
        D2D1_FLOOD_PROP_COLOR)

    IMPLEMENT_EFFECT_PROPERTY(ColorSourceEffect,
        ColorHdr,
        float[4],
        Numerics::Vector4,
        D2D1_FLOOD_PROP_COLOR)

    IMPLEMENT_EFFECT_PROPERTY_MAPPING(ColorSourceEffect,
        { L"Color",    D2D1_FLOOD_PROP_COLOR, GRAPHICS_EFFECT_PROPERTY_MAPPING_COLOR_TO_VECTOR4 },
        { L"ColorHdr", D2D1_FLOOD_PROP_COLOR, GRAPHICS_EFFECT_PROPERTY_MAPPING_UNKNOWN          })

    ActivatableClassWithFactory(ColorSourceEffect, ::SimpleAgileActivationFactory<ColorSourceEffect>);
}}}}}
