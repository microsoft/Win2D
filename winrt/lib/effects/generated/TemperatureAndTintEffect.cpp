// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

// This file was automatically generated. Please do not edit it manually.

#include "pch.h"
#include "TemperatureAndTintEffect.h"

#if (defined _WIN32_WINNT_WIN10) && (WINVER >= _WIN32_WINNT_WIN10)

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects
{
    TemperatureAndTintEffect::TemperatureAndTintEffect(ID2D1Effect* effect)
        : CanvasEffect(effect, EffectId(), 2, 1, true)
    {
        if (!effect)
        {
            // Set default values
            SetBoxedProperty<float>(D2D1_TEMPERATUREANDTINT_PROP_TEMPERATURE, 0.0f);
            SetBoxedProperty<float>(D2D1_TEMPERATUREANDTINT_PROP_TINT, 0.0f);
        }
    }

    IMPLEMENT_EFFECT_PROPERTY_WITH_VALIDATION(TemperatureAndTintEffect,
        Temperature,
        float,
        float,
        D2D1_TEMPERATUREANDTINT_PROP_TEMPERATURE,
        (value >= -1.0f) && (value <= 1.0f))

    IMPLEMENT_EFFECT_PROPERTY_WITH_VALIDATION(TemperatureAndTintEffect,
        Tint,
        float,
        float,
        D2D1_TEMPERATUREANDTINT_PROP_TINT,
        (value >= -1.0f) && (value <= 1.0f))

    IMPLEMENT_EFFECT_SOURCE_PROPERTY(TemperatureAndTintEffect,
        Source,
        0)

    IMPLEMENT_EFFECT_PROPERTY_MAPPING(TemperatureAndTintEffect,
        { L"Temperature", D2D1_TEMPERATUREANDTINT_PROP_TEMPERATURE, GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
        { L"Tint",        D2D1_TEMPERATUREANDTINT_PROP_TINT,        GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT })

    ActivatableClass(TemperatureAndTintEffect);
}}}}}

#endif // _WIN32_WINNT_WIN10
