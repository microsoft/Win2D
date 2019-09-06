// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

// This file was automatically generated. Please do not edit it manually.

#include "pch.h"
#include "Transform3DEffect.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects
{
    Transform3DEffect::Transform3DEffect(ICanvasDevice* device, ID2D1Effect* effect)
        : CanvasEffect(EffectId(), 3, 1, true, device, effect, static_cast<ITransform3DEffect*>(this))
    {
        if (!effect)
        {
            // Set default values
            SetBoxedProperty<uint32_t>(D2D1_3DTRANSFORM_PROP_INTERPOLATION_MODE, D2D1_INTERPOLATION_MODE_LINEAR);
            SetBoxedProperty<uint32_t>(D2D1_3DTRANSFORM_PROP_BORDER_MODE, D2D1_BORDER_MODE_SOFT);
            SetBoxedProperty<float[16]>(D2D1_3DTRANSFORM_PROP_TRANSFORM_MATRIX, Numerics::Matrix4x4{ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 });
        }
    }

    IMPLEMENT_EFFECT_PROPERTY_WITH_VALIDATION(Transform3DEffect,
        InterpolationMode,
        uint32_t,
        CanvasImageInterpolation,
        D2D1_3DTRANSFORM_PROP_INTERPOLATION_MODE,
        (value != static_cast<CanvasImageInterpolation>(5)))

    IMPLEMENT_EFFECT_PROPERTY(Transform3DEffect,
        BorderMode,
        uint32_t,
        EffectBorderMode,
        D2D1_3DTRANSFORM_PROP_BORDER_MODE)

    IMPLEMENT_EFFECT_PROPERTY(Transform3DEffect,
        TransformMatrix,
        float[16],
        Numerics::Matrix4x4,
        D2D1_3DTRANSFORM_PROP_TRANSFORM_MATRIX)

    IMPLEMENT_EFFECT_SOURCE_PROPERTY(Transform3DEffect,
        Source,
        0)

    IMPLEMENT_EFFECT_PROPERTY_MAPPING(Transform3DEffect,
        { L"InterpolationMode", D2D1_3DTRANSFORM_PROP_INTERPOLATION_MODE, GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
        { L"BorderMode",        D2D1_3DTRANSFORM_PROP_BORDER_MODE,        GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
        { L"TransformMatrix",   D2D1_3DTRANSFORM_PROP_TRANSFORM_MATRIX,   GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT })

    ActivatableClassWithFactory(Transform3DEffect, ::SimpleAgileActivationFactory<Transform3DEffect>);
}}}}}
