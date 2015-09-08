// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

// This file was automatically generated. Please do not edit it manually.

#include "pch.h"
#include "CompositeEffect.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects
{
    CompositeEffect::CompositeEffect(ID2D1Effect* effect)
        : CanvasEffect(effect, EffectId(), 1, 0, false)
    {
        if (!effect)
        {
            // Set default values
            SetBoxedProperty<uint32_t>(D2D1_COMPOSITE_PROP_MODE, D2D1_COMPOSITE_MODE_SOURCE_OVER);
        }
    }

    IMPLEMENT_EFFECT_PROPERTY(CompositeEffect,
        Mode,
        uint32_t,
        CanvasComposite,
        D2D1_COMPOSITE_PROP_MODE)

    IMPLEMENT_EFFECT_SOURCES_PROPERTY(CompositeEffect)

    IMPLEMENT_EFFECT_PROPERTY_MAPPING(CompositeEffect,
        { L"Mode", D2D1_COMPOSITE_PROP_MODE, GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT })

    ActivatableClass(CompositeEffect);
}}}}}
