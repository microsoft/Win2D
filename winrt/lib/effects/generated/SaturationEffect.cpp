// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

// This file was automatically generated. Please do not edit it manually.

#include "pch.h"
#include "SaturationEffect.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects
{
    SaturationEffect::SaturationEffect(ID2D1Effect* effect)
        : CanvasEffect(effect, EffectId(), 1, 1, true)
    {
        if (!effect)
        {
            // Set default values
            SetBoxedProperty<float>(D2D1_SATURATION_PROP_SATURATION, 0.5f);
        }
    }

    IMPLEMENT_EFFECT_PROPERTY_WITH_VALIDATION(SaturationEffect,
        Saturation,
        float,
        float,
        D2D1_SATURATION_PROP_SATURATION,
        (value >= 0.0f) && (value <= 2.0f))

    IMPLEMENT_EFFECT_SOURCE_PROPERTY(SaturationEffect,
        Source,
        0)

    IMPLEMENT_EFFECT_PROPERTY_MAPPING(SaturationEffect,
        { L"Saturation", D2D1_SATURATION_PROP_SATURATION, GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT })

    ActivatableClass(SaturationEffect);
}}}}}
