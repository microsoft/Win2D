// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

// This file was automatically generated. Please do not edit it manually.

#include "pch.h"
#include "ShadowEffect.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects
{
    ShadowEffect::ShadowEffect(ICanvasDevice* device, ID2D1Effect* effect)
        : CanvasEffect(EffectId(), 3, 1, true, device, effect, static_cast<IShadowEffect*>(this))
    {
        if (!effect)
        {
            // Set default values
            SetBoxedProperty<float>(D2D1_SHADOW_PROP_BLUR_STANDARD_DEVIATION, 3.0f);
            SetBoxedProperty<float[4]>(D2D1_SHADOW_PROP_COLOR, Color{ 255, 0, 0, 0 });
            SetBoxedProperty<uint32_t>(D2D1_SHADOW_PROP_OPTIMIZATION, D2D1_SHADOW_OPTIMIZATION_BALANCED);
        }
    }

    IMPLEMENT_EFFECT_PROPERTY_WITH_VALIDATION(ShadowEffect,
        BlurAmount,
        float,
        float,
        D2D1_SHADOW_PROP_BLUR_STANDARD_DEVIATION,
        (value >= 0.0f) && (value <= 250.0f))

    IMPLEMENT_EFFECT_PROPERTY(ShadowEffect,
        ShadowColor,
        float[4],
        Color,
        D2D1_SHADOW_PROP_COLOR)

    IMPLEMENT_EFFECT_PROPERTY(ShadowEffect,
        Optimization,
        uint32_t,
        EffectOptimization,
        D2D1_SHADOW_PROP_OPTIMIZATION)

    IMPLEMENT_EFFECT_PROPERTY(ShadowEffect,
        ShadowColorHdr,
        float[4],
        Numerics::Vector4,
        D2D1_SHADOW_PROP_COLOR)

    IMPLEMENT_EFFECT_SOURCE_PROPERTY(ShadowEffect,
        Source,
        0)

    IMPLEMENT_EFFECT_PROPERTY_MAPPING(ShadowEffect,
        { L"BlurAmount",     D2D1_SHADOW_PROP_BLUR_STANDARD_DEVIATION, GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT           },
        { L"ShadowColor",    D2D1_SHADOW_PROP_COLOR,                   GRAPHICS_EFFECT_PROPERTY_MAPPING_COLOR_TO_VECTOR4 },
        { L"Optimization",   D2D1_SHADOW_PROP_OPTIMIZATION,            GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT           },
        { L"ShadowColorHdr", D2D1_SHADOW_PROP_COLOR,                   GRAPHICS_EFFECT_PROPERTY_MAPPING_UNKNOWN          })

    ActivatableClassWithFactory(ShadowEffect, ::SimpleAgileActivationFactory<ShadowEffect>);
}}}}}
