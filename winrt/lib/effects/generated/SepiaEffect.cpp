// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

// This file was automatically generated. Please do not edit it manually.

#include "pch.h"
#include "SepiaEffect.h"

#if (defined _WIN32_WINNT_WIN10) && (WINVER >= _WIN32_WINNT_WIN10)

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects
{
    SepiaEffect::SepiaEffect(ICanvasDevice* device, ID2D1Effect* effect)
        : CanvasEffect(EffectId(), 2, 1, true, device, effect, static_cast<ISepiaEffect*>(this))
    {
        if (!effect)
        {
            // Set default values
            SetBoxedProperty<float>(D2D1_SEPIA_PROP_INTENSITY, 0.5f);
            SetBoxedProperty<uint32_t>(D2D1_SEPIA_PROP_ALPHA_MODE, D2D1_COLORMANAGEMENT_ALPHA_MODE_PREMULTIPLIED);
        }
    }

    IMPLEMENT_EFFECT_PROPERTY_WITH_VALIDATION(SepiaEffect,
        Intensity,
        float,
        float,
        D2D1_SEPIA_PROP_INTENSITY,
        (value >= 0.0f) && (value <= 1.0f))

    IMPLEMENT_EFFECT_PROPERTY(SepiaEffect,
        AlphaMode,
        ConvertAlphaMode,
        CanvasAlphaMode,
        D2D1_SEPIA_PROP_ALPHA_MODE)

    IMPLEMENT_EFFECT_SOURCE_PROPERTY(SepiaEffect,
        Source,
        0)

    IMPLEMENT_EFFECT_PROPERTY_MAPPING(SepiaEffect,
        { L"Intensity", D2D1_SEPIA_PROP_INTENSITY,  GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT                 },
        { L"AlphaMode", D2D1_SEPIA_PROP_ALPHA_MODE, GRAPHICS_EFFECT_PROPERTY_MAPPING_COLORMATRIX_ALPHA_MODE })

    ActivatableClass(SepiaEffect);
}}}}}

#endif // _WIN32_WINNT_WIN10
