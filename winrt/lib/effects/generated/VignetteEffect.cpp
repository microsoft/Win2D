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
#include "VignetteEffect.h"

#if (defined _WIN32_WINNT_WIN10) && (WINVER >= _WIN32_WINNT_WIN10)

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects
{
    VignetteEffect::VignetteEffect()
        : CanvasEffect(CLSID_D2D1Vignette, 3, 1, true)
    {
        // Set default values
        SetBoxedProperty<float[4]>(D2D1_VIGNETTE_PROP_COLOR, Color{ 255, 0, 0, 0 });
        SetBoxedProperty<float>(D2D1_VIGNETTE_PROP_TRANSITION_SIZE, 0.1f);
        SetBoxedProperty<float>(D2D1_VIGNETTE_PROP_STRENGTH, 0.5f);
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

    IMPLEMENT_EFFECT_SOURCE_PROPERTY(VignetteEffect,
        Source,
        0)

    IMPLEMENT_EFFECT_PROPERTY_MAPPING(VignetteEffect,
        { L"Color",  D2D1_VIGNETTE_PROP_COLOR,           GRAPHICS_EFFECT_PROPERTY_MAPPING_COLOR_TO_VECTOR4 },
        { L"Amount", D2D1_VIGNETTE_PROP_TRANSITION_SIZE, GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT           },
        { L"Curve",  D2D1_VIGNETTE_PROP_STRENGTH,        GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT           })

    ActivatableClass(VignetteEffect);
}}}}}

#endif // _WIN32_WINNT_WIN10
