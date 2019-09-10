// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

// This file was automatically generated. Please do not edit it manually.

#include "pch.h"
#include "PosterizeEffect.h"

#if (defined _WIN32_WINNT_WIN10) && (WINVER >= _WIN32_WINNT_WIN10)

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects
{
    PosterizeEffect::PosterizeEffect(ICanvasDevice* device, ID2D1Effect* effect)
        : CanvasEffect(EffectId(), 3, 1, true, device, effect, static_cast<IPosterizeEffect*>(this))
    {
        if (!effect)
        {
            // Set default values
            SetBoxedProperty<int32_t>(D2D1_POSTERIZE_PROP_RED_VALUE_COUNT, 4);
            SetBoxedProperty<int32_t>(D2D1_POSTERIZE_PROP_GREEN_VALUE_COUNT, 4);
            SetBoxedProperty<int32_t>(D2D1_POSTERIZE_PROP_BLUE_VALUE_COUNT, 4);
        }
    }

    IMPLEMENT_EFFECT_PROPERTY_WITH_VALIDATION(PosterizeEffect,
        RedValueCount,
        int32_t,
        int32_t,
        D2D1_POSTERIZE_PROP_RED_VALUE_COUNT,
        (value >= 2) && (value <= 16))

    IMPLEMENT_EFFECT_PROPERTY_WITH_VALIDATION(PosterizeEffect,
        GreenValueCount,
        int32_t,
        int32_t,
        D2D1_POSTERIZE_PROP_GREEN_VALUE_COUNT,
        (value >= 2) && (value <= 16))

    IMPLEMENT_EFFECT_PROPERTY_WITH_VALIDATION(PosterizeEffect,
        BlueValueCount,
        int32_t,
        int32_t,
        D2D1_POSTERIZE_PROP_BLUE_VALUE_COUNT,
        (value >= 2) && (value <= 16))

    IMPLEMENT_EFFECT_SOURCE_PROPERTY(PosterizeEffect,
        Source,
        0)

    IMPLEMENT_EFFECT_PROPERTY_MAPPING(PosterizeEffect,
        { L"RedValueCount",   D2D1_POSTERIZE_PROP_RED_VALUE_COUNT,   GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
        { L"GreenValueCount", D2D1_POSTERIZE_PROP_GREEN_VALUE_COUNT, GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
        { L"BlueValueCount",  D2D1_POSTERIZE_PROP_BLUE_VALUE_COUNT,  GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT })

    ActivatableClassWithFactory(PosterizeEffect, ::SimpleAgileActivationFactory<PosterizeEffect>);
}}}}}

#endif // _WIN32_WINNT_WIN10
