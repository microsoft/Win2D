// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

// This file was automatically generated. Please do not edit it manually.

#include "pch.h"
#include "BorderEffect.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects
{
    BorderEffect::BorderEffect(ID2D1Effect* effect)
        : CanvasEffect(effect, EffectId(), 2, 1, true)
    {
        if (!effect)
        {
            // Set default values
            SetBoxedProperty<uint32_t>(D2D1_BORDER_PROP_EDGE_MODE_X, D2D1_BORDER_EDGE_MODE_CLAMP);
            SetBoxedProperty<uint32_t>(D2D1_BORDER_PROP_EDGE_MODE_Y, D2D1_BORDER_EDGE_MODE_CLAMP);
        }
    }

    IMPLEMENT_EFFECT_PROPERTY(BorderEffect,
        ExtendX,
        uint32_t,
        CanvasEdgeBehavior,
        D2D1_BORDER_PROP_EDGE_MODE_X)

    IMPLEMENT_EFFECT_PROPERTY(BorderEffect,
        ExtendY,
        uint32_t,
        CanvasEdgeBehavior,
        D2D1_BORDER_PROP_EDGE_MODE_Y)

    IMPLEMENT_EFFECT_SOURCE_PROPERTY(BorderEffect,
        Source,
        0)

    IMPLEMENT_EFFECT_PROPERTY_MAPPING(BorderEffect,
        { L"ExtendX", D2D1_BORDER_PROP_EDGE_MODE_X, GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
        { L"ExtendY", D2D1_BORDER_PROP_EDGE_MODE_Y, GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT })

    ActivatableClass(BorderEffect);
}}}}}
