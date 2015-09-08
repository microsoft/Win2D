// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

// This file was automatically generated. Please do not edit it manually.

#include "pch.h"
#include "HighlightsAndShadowsEffect.h"

#if (defined _WIN32_WINNT_WIN10) && (WINVER >= _WIN32_WINNT_WIN10)

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects
{
    HighlightsAndShadowsEffect::HighlightsAndShadowsEffect(ID2D1Effect* effect)
        : CanvasEffect(effect, EffectId(), 5, 1, true)
    {
        if (!effect)
        {
            // Set default values
            SetBoxedProperty<float>(D2D1_HIGHLIGHTSANDSHADOWS_PROP_HIGHLIGHTS, 0.0f);
            SetBoxedProperty<float>(D2D1_HIGHLIGHTSANDSHADOWS_PROP_SHADOWS, 0.0f);
            SetBoxedProperty<float>(D2D1_HIGHLIGHTSANDSHADOWS_PROP_CLARITY, 0.0f);
            SetBoxedProperty<uint32_t>(D2D1_HIGHLIGHTSANDSHADOWS_PROP_INPUT_GAMMA, D2D1_HIGHLIGHTSANDSHADOWS_INPUT_GAMMA_SRGB);
            SetBoxedProperty<float>(D2D1_HIGHLIGHTSANDSHADOWS_PROP_MASK_BLUR_RADIUS, 1.25f);
        }
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
