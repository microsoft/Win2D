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
#include "HighlightsAndShadowsEffect.h"

#if (defined _WIN32_WINNT_WIN10) && (WINVER >= _WIN32_WINNT_WIN10)

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects
{
    HighlightsAndShadowsEffect::HighlightsAndShadowsEffect()
        : CanvasEffect(CLSID_D2D1HighlightsShadows, 5, 1, true)
    {
        // Set default values
        SetBoxedProperty<float>(D2D1_HIGHLIGHTSANDSHADOWS_PROP_HIGHLIGHTS, 0.0f);
        SetBoxedProperty<float>(D2D1_HIGHLIGHTSANDSHADOWS_PROP_SHADOWS, 0.0f);
        SetBoxedProperty<float>(D2D1_HIGHLIGHTSANDSHADOWS_PROP_CLARITY, 0.0f);
        SetBoxedProperty<uint32_t>(D2D1_HIGHLIGHTSANDSHADOWS_PROP_INPUT_GAMMA, D2D1_HIGHLIGHTSANDSHADOWS_INPUT_GAMMA_SRGB);
        SetBoxedProperty<float>(D2D1_HIGHLIGHTSANDSHADOWS_PROP_MASK_BLUR_RADIUS, 1.25f);
    }

    IMPLEMENT_EFFECT_PROPERTY_WITH_VALIDATION(HighlightsAndShadowsEffect,
        Highlights,
        float,
        float,
        D2D1_HIGHLIGHTSANDSHADOWS_PROP_HIGHLIGHTS,
        (value >= -1.0f) && (value <= 1.0f))

    IMPLEMENT_EFFECT_PROPERTY_WITH_VALIDATION(HighlightsAndShadowsEffect,
        Shadows,
        float,
        float,
        D2D1_HIGHLIGHTSANDSHADOWS_PROP_SHADOWS,
        (value >= -1.0f) && (value <= 1.0f))

    IMPLEMENT_EFFECT_PROPERTY_WITH_VALIDATION(HighlightsAndShadowsEffect,
        Clarity,
        float,
        float,
        D2D1_HIGHLIGHTSANDSHADOWS_PROP_CLARITY,
        (value >= -1.0f) && (value <= 1.0f))

    IMPLEMENT_EFFECT_PROPERTY_WITH_VALIDATION(HighlightsAndShadowsEffect,
        MaskBlurAmount,
        float,
        float,
        D2D1_HIGHLIGHTSANDSHADOWS_PROP_MASK_BLUR_RADIUS,
        (value >= 0.0f) && (value <= 10.0f))

    IMPLEMENT_EFFECT_SOURCE_PROPERTY(HighlightsAndShadowsEffect,
        Source,
        0)

    IMPLEMENT_EFFECT_PROPERTY_MAPPING(HighlightsAndShadowsEffect,
        { L"Highlights",     D2D1_HIGHLIGHTSANDSHADOWS_PROP_HIGHLIGHTS,       GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
        { L"Shadows",        D2D1_HIGHLIGHTSANDSHADOWS_PROP_SHADOWS,          GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
        { L"Clarity",        D2D1_HIGHLIGHTSANDSHADOWS_PROP_CLARITY,          GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
        { L"MaskBlurAmount", D2D1_HIGHLIGHTSANDSHADOWS_PROP_MASK_BLUR_RADIUS, GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT })

    ActivatableClass(HighlightsAndShadowsEffect);
}}}}}

#endif // _WIN32_WINNT_WIN10
