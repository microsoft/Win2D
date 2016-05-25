// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

// This file was automatically generated. Please do not edit it manually.

#include "pch.h"
#include "VignetteEffect.h"

#if (defined _WIN32_WINNT_WIN10) && (WINVER >= _WIN32_WINNT_WIN10)

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects
{
    VignetteEffect::VignetteEffect(ICanvasDevice* device, ID2D1Effect* effect)
        : CanvasEffect(EffectId(), 3, 1, true, device, effect, static_cast<IVignetteEffect*>(this))
    {
        if (!effect)
        {
            // Set default values
            SetBoxedProperty<float[4]>(D2D1_VIGNETTE_PROP_COLOR, Color{ 255, 0, 0, 0 });
            SetBoxedProperty<float>(D2D1_VIGNETTE_PROP_TRANSITION_SIZE, 0.1f);
            SetBoxedProperty<float>(D2D1_VIGNETTE_PROP_STRENGTH, 0.5f);
        }
    }

    IMPLEMENT_EFFECT_PROPERTY(VignetteEffect,
        Color,
        float[4],
        Color,
        D2D1_VIGNETTE_PROP_COLOR)

    IMPLEMENT_EFFECT_PROPERTY_WITH_VALIDATION(VignetteEffect,
        Amount,
        float,
        float,
        D2D1_VIGNETTE_PROP_TRANSITION_SIZE,
        (value >= 0.0f) && (value <= 1.0f))

    IMPLEMENT_EFFECT_PROPERTY_WITH_VALIDATION(VignetteEffect,
        Curve,
        float,
        float,
        D2D1_VIGNETTE_PROP_STRENGTH,
        (value >= 0.0f) && (value <= 1.0f))

    IMPLEMENT_EFFECT_PROPERTY(VignetteEffect,
        ColorHdr,
        float[4],
        Numerics::Vector4,
        D2D1_VIGNETTE_PROP_COLOR)

    IMPLEMENT_EFFECT_SOURCE_PROPERTY(VignetteEffect,
        Source,
        0)

    IMPLEMENT_EFFECT_PROPERTY_MAPPING(VignetteEffect,
        { L"Color",    D2D1_VIGNETTE_PROP_COLOR,           GRAPHICS_EFFECT_PROPERTY_MAPPING_COLOR_TO_VECTOR4 },
        { L"Amount",   D2D1_VIGNETTE_PROP_TRANSITION_SIZE, GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT           },
        { L"Curve",    D2D1_VIGNETTE_PROP_STRENGTH,        GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT           },
        { L"ColorHdr", D2D1_VIGNETTE_PROP_COLOR,           GRAPHICS_EFFECT_PROPERTY_MAPPING_UNKNOWN          })

    ActivatableClassWithFactory(VignetteEffect, SimpleAgileActivationFactory<VignetteEffect>);
}}}}}

#endif // _WIN32_WINNT_WIN10
