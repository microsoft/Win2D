// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

// This file was automatically generated. Please do not edit it manually.

#include "pch.h"
#include "EmbossEffect.h"

#if (defined _WIN32_WINNT_WIN10) && (WINVER >= _WIN32_WINNT_WIN10)

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects
{
    EmbossEffect::EmbossEffect(ICanvasDevice* device, ID2D1Effect* effect)
        : CanvasEffect(EffectId(), 2, 1, true, device, effect, static_cast<IEmbossEffect*>(this))
    {
        if (!effect)
        {
            // Set default values
            SetBoxedProperty<float>(D2D1_EMBOSS_PROP_HEIGHT, 1.0f);
            SetBoxedProperty<float>(D2D1_EMBOSS_PROP_DIRECTION, 0.0f);
        }
    }

    IMPLEMENT_EFFECT_PROPERTY_WITH_VALIDATION(EmbossEffect,
        Amount,
        float,
        float,
        D2D1_EMBOSS_PROP_HEIGHT,
        (value >= 0.0f) && (value <= 10.0f))

    IMPLEMENT_EFFECT_PROPERTY(EmbossEffect,
        Angle,
        ConvertRadiansToDegrees,
        float,
        D2D1_EMBOSS_PROP_DIRECTION)

    IMPLEMENT_EFFECT_SOURCE_PROPERTY(EmbossEffect,
        Source,
        0)

    IMPLEMENT_EFFECT_PROPERTY_MAPPING(EmbossEffect,
        { L"Amount", D2D1_EMBOSS_PROP_HEIGHT,    GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT             },
        { L"Angle",  D2D1_EMBOSS_PROP_DIRECTION, GRAPHICS_EFFECT_PROPERTY_MAPPING_RADIANS_TO_DEGREES })

    ActivatableClassWithFactory(EmbossEffect, ::SimpleAgileActivationFactory<EmbossEffect>);
}}}}}

#endif // _WIN32_WINNT_WIN10
