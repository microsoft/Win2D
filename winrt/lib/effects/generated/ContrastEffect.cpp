// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

// This file was automatically generated. Please do not edit it manually.

#include "pch.h"
#include "ContrastEffect.h"

#if (defined _WIN32_WINNT_WIN10) && (WINVER >= _WIN32_WINNT_WIN10)

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects
{
    ContrastEffect::ContrastEffect(ICanvasDevice* device, ID2D1Effect* effect)
        : CanvasEffect(EffectId(), 2, 1, true, device, effect, static_cast<IContrastEffect*>(this))
    {
        if (!effect)
        {
            // Set default values
            SetBoxedProperty<float>(D2D1_CONTRAST_PROP_CONTRAST, 0.0f);
            SetBoxedProperty<boolean>(D2D1_CONTRAST_PROP_CLAMP_INPUT, static_cast<boolean>(false));
        }
    }

    IMPLEMENT_EFFECT_PROPERTY_WITH_VALIDATION(ContrastEffect,
        Contrast,
        float,
        float,
        D2D1_CONTRAST_PROP_CONTRAST,
        (value >= -1.0f) && (value <= 1.0f))

    IMPLEMENT_EFFECT_PROPERTY(ContrastEffect,
        ClampSource,
        boolean,
        boolean,
        D2D1_CONTRAST_PROP_CLAMP_INPUT)

    IMPLEMENT_EFFECT_SOURCE_PROPERTY(ContrastEffect,
        Source,
        0)

    IMPLEMENT_EFFECT_PROPERTY_MAPPING(ContrastEffect,
        { L"Contrast",    D2D1_CONTRAST_PROP_CONTRAST,    GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
        { L"ClampSource", D2D1_CONTRAST_PROP_CLAMP_INPUT, GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT })

    ActivatableClassWithFactory(ContrastEffect, ::SimpleAgileActivationFactory<ContrastEffect>);
}}}}}

#endif // _WIN32_WINNT_WIN10
