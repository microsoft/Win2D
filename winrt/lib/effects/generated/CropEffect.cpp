// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

// This file was automatically generated. Please do not edit it manually.

#include "pch.h"
#include "CropEffect.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects
{
    CropEffect::CropEffect(ICanvasDevice* device, ID2D1Effect* effect)
        : CanvasEffect(EffectId(), 2, 1, true, device, effect, static_cast<ICropEffect*>(this))
    {
        if (!effect)
        {
            // Set default values
            SetBoxedProperty<float[4]>(D2D1_CROP_PROP_RECT, Rect{ -std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity() });
            SetBoxedProperty<uint32_t>(D2D1_CROP_PROP_BORDER_MODE, D2D1_BORDER_MODE_SOFT);
        }
    }

    IMPLEMENT_EFFECT_PROPERTY(CropEffect,
        SourceRectangle,
        float[4],
        Rect,
        D2D1_CROP_PROP_RECT)

    IMPLEMENT_EFFECT_PROPERTY(CropEffect,
        BorderMode,
        uint32_t,
        EffectBorderMode,
        D2D1_CROP_PROP_BORDER_MODE)

    IMPLEMENT_EFFECT_SOURCE_PROPERTY(CropEffect,
        Source,
        0)

    IMPLEMENT_EFFECT_PROPERTY_MAPPING(CropEffect,
        { L"SourceRectangle", D2D1_CROP_PROP_RECT,        GRAPHICS_EFFECT_PROPERTY_MAPPING_RECT_TO_VECTOR4 },
        { L"BorderMode",      D2D1_CROP_PROP_BORDER_MODE, GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT          })

    ActivatableClass(CropEffect);
}}}}}
