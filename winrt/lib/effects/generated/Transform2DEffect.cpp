// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

// This file was automatically generated. Please do not edit it manually.

#include "pch.h"
#include "Transform2DEffect.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects
{
    Transform2DEffect::Transform2DEffect(ICanvasDevice* device, ID2D1Effect* effect)
        : CanvasEffect(EffectId(), 4, 1, true, device, effect, static_cast<ITransform2DEffect*>(this))
    {
        if (!effect)
        {
            // Set default values
            SetBoxedProperty<uint32_t>(D2D1_2DAFFINETRANSFORM_PROP_INTERPOLATION_MODE, D2D1_2DAFFINETRANSFORM_INTERPOLATION_MODE_LINEAR);
            SetBoxedProperty<uint32_t>(D2D1_2DAFFINETRANSFORM_PROP_BORDER_MODE, D2D1_BORDER_MODE_SOFT);
            SetBoxedProperty<float[6]>(D2D1_2DAFFINETRANSFORM_PROP_TRANSFORM_MATRIX, Numerics::Matrix3x2{ 1, 0, 0, 1, 0, 0 });
            SetBoxedProperty<float>(D2D1_2DAFFINETRANSFORM_PROP_SHARPNESS, 0.0f);
        }
    }

    IMPLEMENT_EFFECT_PROPERTY(Transform2DEffect,
        InterpolationMode,
        uint32_t,
        CanvasImageInterpolation,
        D2D1_2DAFFINETRANSFORM_PROP_INTERPOLATION_MODE)

    IMPLEMENT_EFFECT_PROPERTY(Transform2DEffect,
        BorderMode,
        uint32_t,
        EffectBorderMode,
        D2D1_2DAFFINETRANSFORM_PROP_BORDER_MODE)

    IMPLEMENT_EFFECT_PROPERTY(Transform2DEffect,
        TransformMatrix,
        float[6],
        Numerics::Matrix3x2,
        D2D1_2DAFFINETRANSFORM_PROP_TRANSFORM_MATRIX)

    IMPLEMENT_EFFECT_PROPERTY_WITH_VALIDATION(Transform2DEffect,
        Sharpness,
        float,
        float,
        D2D1_2DAFFINETRANSFORM_PROP_SHARPNESS,
        (value >= 0.0f) && (value <= 1.0f))

    IMPLEMENT_EFFECT_SOURCE_PROPERTY(Transform2DEffect,
        Source,
        0)

    IMPLEMENT_EFFECT_PROPERTY_MAPPING(Transform2DEffect,
        { L"InterpolationMode", D2D1_2DAFFINETRANSFORM_PROP_INTERPOLATION_MODE, GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
        { L"BorderMode",        D2D1_2DAFFINETRANSFORM_PROP_BORDER_MODE,        GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
        { L"TransformMatrix",   D2D1_2DAFFINETRANSFORM_PROP_TRANSFORM_MATRIX,   GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
        { L"Sharpness",         D2D1_2DAFFINETRANSFORM_PROP_SHARPNESS,          GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT })

    ActivatableClass(Transform2DEffect);
}}}}}
