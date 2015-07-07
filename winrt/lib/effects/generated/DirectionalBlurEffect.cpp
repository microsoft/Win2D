// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

// This file was automatically generated. Please do not edit it manually.

#include "pch.h"
#include "DirectionalBlurEffect.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects
{
    DirectionalBlurEffect::DirectionalBlurEffect()
        : CanvasEffect(CLSID_D2D1DirectionalBlur, 4, 1, true)
    {
        // Set default values
        SetBoxedProperty<float>(D2D1_DIRECTIONALBLUR_PROP_STANDARD_DEVIATION, 3.0f);
        SetBoxedProperty<float>(D2D1_DIRECTIONALBLUR_PROP_ANGLE, 0.0f);
        SetBoxedProperty<uint32_t>(D2D1_DIRECTIONALBLUR_PROP_OPTIMIZATION, D2D1_DIRECTIONALBLUR_OPTIMIZATION_BALANCED);
        SetBoxedProperty<uint32_t>(D2D1_DIRECTIONALBLUR_PROP_BORDER_MODE, D2D1_BORDER_MODE_SOFT);
    }

    IMPLEMENT_EFFECT_PROPERTY_WITH_VALIDATION(DirectionalBlurEffect,
        BlurAmount,
        float,
        float,
        D2D1_DIRECTIONALBLUR_PROP_STANDARD_DEVIATION,
        (value >= 0.0f) && (value <= 250.0f))

    IMPLEMENT_EFFECT_PROPERTY(DirectionalBlurEffect,
        Angle,
        ConvertRadiansToDegrees,
        float,
        D2D1_DIRECTIONALBLUR_PROP_ANGLE)

    IMPLEMENT_EFFECT_PROPERTY(DirectionalBlurEffect,
        Optimization,
        uint32_t,
        EffectOptimization,
        D2D1_DIRECTIONALBLUR_PROP_OPTIMIZATION)

    IMPLEMENT_EFFECT_PROPERTY(DirectionalBlurEffect,
        BorderMode,
        uint32_t,
        EffectBorderMode,
        D2D1_DIRECTIONALBLUR_PROP_BORDER_MODE)

    IMPLEMENT_EFFECT_SOURCE_PROPERTY(DirectionalBlurEffect,
        Source,
        0)

    IMPLEMENT_EFFECT_PROPERTY_MAPPING(DirectionalBlurEffect,
        { L"BlurAmount",   D2D1_DIRECTIONALBLUR_PROP_STANDARD_DEVIATION, GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT             },
        { L"Angle",        D2D1_DIRECTIONALBLUR_PROP_ANGLE,              GRAPHICS_EFFECT_PROPERTY_MAPPING_RADIANS_TO_DEGREES },
        { L"Optimization", D2D1_DIRECTIONALBLUR_PROP_OPTIMIZATION,       GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT             },
        { L"BorderMode",   D2D1_DIRECTIONALBLUR_PROP_BORDER_MODE,        GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT             })

    ActivatableClass(DirectionalBlurEffect);
}}}}}
