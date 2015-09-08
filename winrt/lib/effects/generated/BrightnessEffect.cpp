// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

// This file was automatically generated. Please do not edit it manually.

#include "pch.h"
#include "BrightnessEffect.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects
{
    BrightnessEffect::BrightnessEffect(ID2D1Effect* effect)
        : CanvasEffect(effect, EffectId(), 2, 1, true)
    {
        if (!effect)
        {
            // Set default values
            SetBoxedProperty<float[2]>(D2D1_BRIGHTNESS_PROP_WHITE_POINT, Numerics::Vector2{ 1.0f, 1.0f });
            SetBoxedProperty<float[2]>(D2D1_BRIGHTNESS_PROP_BLACK_POINT, Numerics::Vector2{ 0.0f, 0.0f });
        }
    }

    IMPLEMENT_EFFECT_PROPERTY_WITH_VALIDATION(BrightnessEffect,
        WhitePoint,
        float[2],
        Numerics::Vector2,
        D2D1_BRIGHTNESS_PROP_WHITE_POINT,
        (value.X >= 0.0f) && (value.Y >= 0.0f) && (value.X <= 1.0f) && (value.Y <= 1.0f))

    IMPLEMENT_EFFECT_PROPERTY_WITH_VALIDATION(BrightnessEffect,
        BlackPoint,
        float[2],
        Numerics::Vector2,
        D2D1_BRIGHTNESS_PROP_BLACK_POINT,
        (value.X >= 0.0f) && (value.Y >= 0.0f) && (value.X <= 1.0f) && (value.Y <= 1.0f))

    IMPLEMENT_EFFECT_SOURCE_PROPERTY(BrightnessEffect,
        Source,
        0)

    IMPLEMENT_EFFECT_PROPERTY_MAPPING(BrightnessEffect,
        { L"WhitePoint", D2D1_BRIGHTNESS_PROP_WHITE_POINT, GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
        { L"BlackPoint", D2D1_BRIGHTNESS_PROP_BLACK_POINT, GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT })

    ActivatableClass(BrightnessEffect);
}}}}}
