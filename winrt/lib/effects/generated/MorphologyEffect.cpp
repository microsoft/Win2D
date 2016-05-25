// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

// This file was automatically generated. Please do not edit it manually.

#include "pch.h"
#include "MorphologyEffect.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects
{
    MorphologyEffect::MorphologyEffect(ICanvasDevice* device, ID2D1Effect* effect)
        : CanvasEffect(EffectId(), 3, 1, true, device, effect, static_cast<IMorphologyEffect*>(this))
    {
        if (!effect)
        {
            // Set default values
            SetBoxedProperty<uint32_t>(D2D1_MORPHOLOGY_PROP_MODE, D2D1_MORPHOLOGY_MODE_ERODE);
            SetBoxedProperty<int32_t>(D2D1_MORPHOLOGY_PROP_WIDTH, 1);
            SetBoxedProperty<int32_t>(D2D1_MORPHOLOGY_PROP_HEIGHT, 1);
        }
    }

    IMPLEMENT_EFFECT_PROPERTY(MorphologyEffect,
        Mode,
        uint32_t,
        MorphologyEffectMode,
        D2D1_MORPHOLOGY_PROP_MODE)

    IMPLEMENT_EFFECT_PROPERTY_WITH_VALIDATION(MorphologyEffect,
        Width,
        int32_t,
        int32_t,
        D2D1_MORPHOLOGY_PROP_WIDTH,
        (value >= 1) && (value <= 100))

    IMPLEMENT_EFFECT_PROPERTY_WITH_VALIDATION(MorphologyEffect,
        Height,
        int32_t,
        int32_t,
        D2D1_MORPHOLOGY_PROP_HEIGHT,
        (value >= 1) && (value <= 100))

    IMPLEMENT_EFFECT_SOURCE_PROPERTY(MorphologyEffect,
        Source,
        0)

    IMPLEMENT_EFFECT_PROPERTY_MAPPING(MorphologyEffect,
        { L"Mode",   D2D1_MORPHOLOGY_PROP_MODE,   GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
        { L"Width",  D2D1_MORPHOLOGY_PROP_WIDTH,  GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
        { L"Height", D2D1_MORPHOLOGY_PROP_HEIGHT, GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT })

    ActivatableClassWithFactory(MorphologyEffect, SimpleAgileActivationFactory<MorphologyEffect>);
}}}}}
