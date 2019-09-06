// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

// This file was automatically generated. Please do not edit it manually.

#include "pch.h"
#include "HueRotationEffect.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects
{
    HueRotationEffect::HueRotationEffect(ICanvasDevice* device, ID2D1Effect* effect)
        : CanvasEffect(EffectId(), 1, 1, true, device, effect, static_cast<IHueRotationEffect*>(this))
    {
        if (!effect)
        {
            // Set default values
            SetBoxedProperty<float>(D2D1_HUEROTATION_PROP_ANGLE, 0.0f);
        }
    }

    IMPLEMENT_EFFECT_PROPERTY(HueRotationEffect,
        Angle,
        ConvertRadiansToDegrees,
        float,
        D2D1_HUEROTATION_PROP_ANGLE)

    IMPLEMENT_EFFECT_SOURCE_PROPERTY(HueRotationEffect,
        Source,
        0)

    IMPLEMENT_EFFECT_PROPERTY_MAPPING(HueRotationEffect,
        { L"Angle", D2D1_HUEROTATION_PROP_ANGLE, GRAPHICS_EFFECT_PROPERTY_MAPPING_RADIANS_TO_DEGREES })

    ActivatableClassWithFactory(HueRotationEffect, ::SimpleAgileActivationFactory<HueRotationEffect>);
}}}}}
