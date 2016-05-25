// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

// This file was automatically generated. Please do not edit it manually.

#include "pch.h"
#include "TurbulenceEffect.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects
{
    TurbulenceEffect::TurbulenceEffect(ICanvasDevice* device, ID2D1Effect* effect)
        : CanvasEffect(EffectId(), 7, 0, true, device, effect, static_cast<ITurbulenceEffect*>(this))
    {
        if (!effect)
        {
            // Set default values
            SetBoxedProperty<float[2]>(D2D1_TURBULENCE_PROP_OFFSET, Numerics::Vector2{ 0, 0 });
            SetBoxedProperty<float[2]>(D2D1_TURBULENCE_PROP_SIZE, Numerics::Vector2{ 512, 512 });
            SetBoxedProperty<float[2]>(D2D1_TURBULENCE_PROP_BASE_FREQUENCY, Numerics::Vector2{ 0.01f, 0.01f });
            SetBoxedProperty<int32_t>(D2D1_TURBULENCE_PROP_NUM_OCTAVES, 1);
            SetBoxedProperty<int32_t>(D2D1_TURBULENCE_PROP_SEED, 0);
            SetBoxedProperty<uint32_t>(D2D1_TURBULENCE_PROP_NOISE, D2D1_TURBULENCE_NOISE_FRACTAL_SUM);
            SetBoxedProperty<boolean>(D2D1_TURBULENCE_PROP_STITCHABLE, static_cast<boolean>(false));
        }
    }

    IMPLEMENT_EFFECT_PROPERTY(TurbulenceEffect,
        Offset,
        float[2],
        Numerics::Vector2,
        D2D1_TURBULENCE_PROP_OFFSET)

    IMPLEMENT_EFFECT_PROPERTY_WITH_VALIDATION(TurbulenceEffect,
        Size,
        float[2],
        Numerics::Vector2,
        D2D1_TURBULENCE_PROP_SIZE,
        (value.X >= 0) && (value.Y >= 0))

    IMPLEMENT_EFFECT_PROPERTY_WITH_VALIDATION(TurbulenceEffect,
        Frequency,
        float[2],
        Numerics::Vector2,
        D2D1_TURBULENCE_PROP_BASE_FREQUENCY,
        (value.X >= 0) && (value.Y >= 0) && (value.X <= 1000) && (value.Y <= 1000))

    IMPLEMENT_EFFECT_PROPERTY_WITH_VALIDATION(TurbulenceEffect,
        Octaves,
        int32_t,
        int32_t,
        D2D1_TURBULENCE_PROP_NUM_OCTAVES,
        (value >= 1) && (value <= 15))

    IMPLEMENT_EFFECT_PROPERTY_WITH_VALIDATION(TurbulenceEffect,
        Seed,
        int32_t,
        int32_t,
        D2D1_TURBULENCE_PROP_SEED,
        (value >= -10000) && (value <= 10000))

    IMPLEMENT_EFFECT_PROPERTY(TurbulenceEffect,
        Noise,
        uint32_t,
        TurbulenceEffectNoise,
        D2D1_TURBULENCE_PROP_NOISE)

    IMPLEMENT_EFFECT_PROPERTY(TurbulenceEffect,
        Tileable,
        boolean,
        boolean,
        D2D1_TURBULENCE_PROP_STITCHABLE)

    IMPLEMENT_EFFECT_PROPERTY_MAPPING(TurbulenceEffect,
        { L"Offset",    D2D1_TURBULENCE_PROP_OFFSET,         GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
        { L"Size",      D2D1_TURBULENCE_PROP_SIZE,           GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
        { L"Frequency", D2D1_TURBULENCE_PROP_BASE_FREQUENCY, GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
        { L"Octaves",   D2D1_TURBULENCE_PROP_NUM_OCTAVES,    GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
        { L"Seed",      D2D1_TURBULENCE_PROP_SEED,           GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
        { L"Noise",     D2D1_TURBULENCE_PROP_NOISE,          GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
        { L"Tileable",  D2D1_TURBULENCE_PROP_STITCHABLE,     GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT })

    ActivatableClassWithFactory(TurbulenceEffect, SimpleAgileActivationFactory<TurbulenceEffect>);
}}}}}
