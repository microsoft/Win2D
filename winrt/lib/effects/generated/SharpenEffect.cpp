// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

// This file was automatically generated. Please do not edit it manually.

#include "pch.h"
#include "SharpenEffect.h"

#if (defined _WIN32_WINNT_WIN10) && (WINVER >= _WIN32_WINNT_WIN10)

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects
{
    SharpenEffect::SharpenEffect(ICanvasDevice* device, ID2D1Effect* effect)
        : CanvasEffect(EffectId(), 2, 1, true, device, effect, static_cast<ISharpenEffect*>(this))
    {
        if (!effect)
        {
            // Set default values
            SetBoxedProperty<float>(D2D1_SHARPEN_PROP_SHARPNESS, 0.0f);
            SetBoxedProperty<float>(D2D1_SHARPEN_PROP_THRESHOLD, 0.0f);
        }
    }

    IMPLEMENT_EFFECT_PROPERTY_WITH_VALIDATION(SharpenEffect,
        Amount,
        float,
        float,
        D2D1_SHARPEN_PROP_SHARPNESS,
        (value >= 0.0f) && (value <= 10.0f))

    IMPLEMENT_EFFECT_PROPERTY_WITH_VALIDATION(SharpenEffect,
        Threshold,
        float,
        float,
        D2D1_SHARPEN_PROP_THRESHOLD,
        (value >= 0.0f) && (value <= 10.0f))

    IMPLEMENT_EFFECT_SOURCE_PROPERTY(SharpenEffect,
        Source,
        0)

    IMPLEMENT_EFFECT_PROPERTY_MAPPING(SharpenEffect,
        { L"Amount",    D2D1_SHARPEN_PROP_SHARPNESS, GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
        { L"Threshold", D2D1_SHARPEN_PROP_THRESHOLD, GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT })

    ActivatableClass(SharpenEffect);
}}}}}

#endif // _WIN32_WINNT_WIN10
