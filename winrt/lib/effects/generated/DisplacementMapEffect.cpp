// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

// This file was automatically generated. Please do not edit it manually.

#include "pch.h"
#include "DisplacementMapEffect.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects
{
    DisplacementMapEffect::DisplacementMapEffect(ID2D1Effect* effect)
        : CanvasEffect(effect, EffectId(), 3, 2, true)
    {
        if (!effect)
        {
            // Set default values
            SetBoxedProperty<float>(D2D1_DISPLACEMENTMAP_PROP_SCALE, 0.0f);
            SetBoxedProperty<uint32_t>(D2D1_DISPLACEMENTMAP_PROP_X_CHANNEL_SELECT, EffectChannelSelect::Alpha);
            SetBoxedProperty<uint32_t>(D2D1_DISPLACEMENTMAP_PROP_Y_CHANNEL_SELECT, EffectChannelSelect::Alpha);
        }
    }

    IMPLEMENT_EFFECT_PROPERTY(DisplacementMapEffect,
        Amount,
        float,
        float,
        D2D1_DISPLACEMENTMAP_PROP_SCALE)

    IMPLEMENT_EFFECT_PROPERTY(DisplacementMapEffect,
        XChannelSelect,
        uint32_t,
        EffectChannelSelect,
        D2D1_DISPLACEMENTMAP_PROP_X_CHANNEL_SELECT)

    IMPLEMENT_EFFECT_PROPERTY(DisplacementMapEffect,
        YChannelSelect,
        uint32_t,
        EffectChannelSelect,
        D2D1_DISPLACEMENTMAP_PROP_Y_CHANNEL_SELECT)

    IMPLEMENT_EFFECT_SOURCE_PROPERTY(DisplacementMapEffect,
        Source,
        0)

    IMPLEMENT_EFFECT_SOURCE_PROPERTY(DisplacementMapEffect,
        Displacement,
        1)

    IMPLEMENT_EFFECT_PROPERTY_MAPPING(DisplacementMapEffect,
        { L"Amount",         D2D1_DISPLACEMENTMAP_PROP_SCALE,            GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
        { L"XChannelSelect", D2D1_DISPLACEMENTMAP_PROP_X_CHANNEL_SELECT, GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
        { L"YChannelSelect", D2D1_DISPLACEMENTMAP_PROP_Y_CHANNEL_SELECT, GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT })

    ActivatableClass(DisplacementMapEffect);
}}}}}
