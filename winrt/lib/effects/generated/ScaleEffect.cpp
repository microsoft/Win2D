// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

// This file was automatically generated. Please do not edit it manually.

#include "pch.h"
#include "ScaleEffect.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects
{
    ScaleEffect::ScaleEffect(ICanvasDevice* device, ID2D1Effect* effect)
        : CanvasEffect(EffectId(), 5, 1, true, device, effect, static_cast<IScaleEffect*>(this))
    {
        if (!effect)
        {
            // Set default values
            SetBoxedProperty<float[2]>(D2D1_SCALE_PROP_SCALE, Numerics::Vector2{ 1, 1 });
            SetBoxedProperty<float[2]>(D2D1_SCALE_PROP_CENTER_POINT, Numerics::Vector2{ 0, 0 });
            SetBoxedProperty<uint32_t>(D2D1_SCALE_PROP_INTERPOLATION_MODE, D2D1_CONVOLVEMATRIX_SCALE_MODE_LINEAR);
            SetBoxedProperty<uint32_t>(D2D1_SCALE_PROP_BORDER_MODE, D2D1_BORDER_MODE_SOFT);
            SetBoxedProperty<float>(D2D1_SCALE_PROP_SHARPNESS, 0.0f);
        }
    }

    IMPLEMENT_EFFECT_PROPERTY_WITH_VALIDATION(ScaleEffect,
        Scale,
        float[2],
        Numerics::Vector2,
        D2D1_SCALE_PROP_SCALE,
        (value.X >= 0.01f) && (value.Y >= 0.01f))

    IMPLEMENT_EFFECT_PROPERTY(ScaleEffect,
        CenterPoint,
        float[2],
        Numerics::Vector2,
        D2D1_SCALE_PROP_CENTER_POINT)

    IMPLEMENT_EFFECT_PROPERTY(ScaleEffect,
        InterpolationMode,
        uint32_t,
        CanvasImageInterpolation,
        D2D1_SCALE_PROP_INTERPOLATION_MODE)

    IMPLEMENT_EFFECT_PROPERTY(ScaleEffect,
        BorderMode,
        uint32_t,
        EffectBorderMode,
        D2D1_SCALE_PROP_BORDER_MODE)

    IMPLEMENT_EFFECT_PROPERTY_WITH_VALIDATION(ScaleEffect,
        Sharpness,
        float,
        float,
        D2D1_SCALE_PROP_SHARPNESS,
        (value >= 0.0f) && (value <= 1.0f))

    IMPLEMENT_EFFECT_SOURCE_PROPERTY(ScaleEffect,
        Source,
        0)

    IMPLEMENT_EFFECT_PROPERTY_MAPPING(ScaleEffect,
        { L"Scale",             D2D1_SCALE_PROP_SCALE,              GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
        { L"CenterPoint",       D2D1_SCALE_PROP_CENTER_POINT,       GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
        { L"InterpolationMode", D2D1_SCALE_PROP_INTERPOLATION_MODE, GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
        { L"BorderMode",        D2D1_SCALE_PROP_BORDER_MODE,        GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
        { L"Sharpness",         D2D1_SCALE_PROP_SHARPNESS,          GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT })

    ActivatableClassWithFactory(ScaleEffect, SimpleAgileActivationFactory<ScaleEffect>);
}}}}}
