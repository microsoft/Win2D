// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may
// not use these files except in compliance with the License. You may obtain
// a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
// License for the specific language governing permissions and limitations
// under the License.

// This file was automatically generated. Please do not edit it manually.

#include "pch.h"
#include "TemperatureAndTintEffect.h"

#if (defined _WIN32_WINNT_WIN10) && (WINVER >= _WIN32_WINNT_WIN10)

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects
{
    TemperatureAndTintEffect::TemperatureAndTintEffect()
        : CanvasEffect(CLSID_D2D1TemperatureTint, 2, 1, true)
    {
        // Set default values
        SetBoxedProperty<float>(D2D1_TEMPERATUREANDTINT_PROP_TEMPERATURE, 0.0f);
        SetBoxedProperty<float>(D2D1_TEMPERATUREANDTINT_PROP_TINT, 0.0f);
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
