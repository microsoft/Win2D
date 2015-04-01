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
#include "ShadowEffect.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects
{
    ShadowEffect::ShadowEffect()
        : CanvasEffect(CLSID_D2D1Shadow, 3, 1, true)
    {
        // Set default values
        SetBoxedProperty<float>(D2D1_SHADOW_PROP_BLUR_STANDARD_DEVIATION, 3.0f);
        SetBoxedProperty<float[4]>(D2D1_SHADOW_PROP_COLOR, Color{ 255, 0, 0, 0 });
        SetBoxedProperty<uint32_t>(D2D1_SHADOW_PROP_OPTIMIZATION, D2D1_SHADOW_OPTIMIZATION_BALANCED);
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

    IMPLEMENT_EFFECT_SOURCE_PROPERTY(ShadowEffect,
        Source,
        0)

    IMPLEMENT_EFFECT_PROPERTY_MAPPING(ShadowEffect,
        { L"BlurAmount",   D2D1_SHADOW_PROP_BLUR_STANDARD_DEVIATION, GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT           },
        { L"ShadowColor",  D2D1_SHADOW_PROP_COLOR,                   GRAPHICS_EFFECT_PROPERTY_MAPPING_COLOR_TO_VECTOR4 },
        { L"Optimization", D2D1_SHADOW_PROP_OPTIMIZATION,            GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT           })

    ActivatableClass(ShadowEffect);
}}}}}
