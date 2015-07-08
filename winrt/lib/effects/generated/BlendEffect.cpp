// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

// This file was automatically generated. Please do not edit it manually.

#include "pch.h"
#include "BlendEffect.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects
{
    BlendEffect::BlendEffect()
        : CanvasEffect(CLSID_D2D1Blend, 1, 2, true)
    {
        // Set default values
        SetBoxedProperty<uint32_t>(D2D1_BLEND_PROP_MODE, D2D1_BLEND_MODE_MULTIPLY);
    }

    IMPLEMENT_EFFECT_PROPERTY(BlendEffect,
        Mode,
        uint32_t,
        BlendEffectMode,
        D2D1_BLEND_PROP_MODE)

    IMPLEMENT_EFFECT_SOURCE_PROPERTY(BlendEffect,
        Background,
        0)

    IMPLEMENT_EFFECT_SOURCE_PROPERTY(BlendEffect,
        Foreground,
        1)

    IMPLEMENT_EFFECT_PROPERTY_MAPPING(BlendEffect,
        { L"Mode", D2D1_BLEND_PROP_MODE, GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT })

    ActivatableClass(BlendEffect);
}}}}}
