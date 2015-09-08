// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

// This file was automatically generated. Please do not edit it manually.

#include "pch.h"
#include "PointDiffuseEffect.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects
{
    PointDiffuseEffect::PointDiffuseEffect(ID2D1Effect* effect)
        : CanvasEffect(effect, EffectId(), 6, 1, true)
    {
        if (!effect)
        {
            // Set default values
            SetBoxedProperty<float[3]>(D2D1_POINTDIFFUSE_PROP_LIGHT_POSITION, Numerics::Vector3{ 0.0f, 0.0f, 0.0f });
            SetBoxedProperty<float>(D2D1_POINTDIFFUSE_PROP_DIFFUSE_CONSTANT, 1.0f);
            SetBoxedProperty<float>(D2D1_POINTDIFFUSE_PROP_SURFACE_SCALE, 1.0f);
            SetBoxedProperty<float[3]>(D2D1_POINTDIFFUSE_PROP_COLOR, Color{ 255, 255, 255, 255 });
            SetBoxedProperty<float[2]>(D2D1_POINTDIFFUSE_PROP_KERNEL_UNIT_LENGTH, Numerics::Vector2{ 1.0f, 1.0f });
            SetBoxedProperty<uint32_t>(D2D1_POINTDIFFUSE_PROP_SCALE_MODE, D2D1_POINTDIFFUSE_SCALE_MODE_LINEAR);
        }
    }

    IMPLEMENT_EFFECT_PROPERTY(PointDiffuseEffect,
        LightPosition,
        float[3],
        Numerics::Vector3,
        D2D1_POINTDIFFUSE_PROP_LIGHT_POSITION)

    IMPLEMENT_EFFECT_PROPERTY_WITH_VALIDATION(PointDiffuseEffect,
        DiffuseAmount,
        float,
        float,
        D2D1_POINTDIFFUSE_PROP_DIFFUSE_CONSTANT,
        (value >= 0.0f) && (value <= 10000.0f))

    IMPLEMENT_EFFECT_PROPERTY_WITH_VALIDATION(PointDiffuseEffect,
        HeightMapScale,
        float,
        float,
        D2D1_POINTDIFFUSE_PROP_SURFACE_SCALE,
        (value >= -10000.0f) && (value <= 10000.0f))

    IMPLEMENT_EFFECT_PROPERTY(PointDiffuseEffect,
        LightColor,
        float[3],
        Color,
        D2D1_POINTDIFFUSE_PROP_COLOR)

    IMPLEMENT_EFFECT_PROPERTY_WITH_VALIDATION(PointDiffuseEffect,
        HeightMapKernelSize,
        float[2],
        Numerics::Vector2,
        D2D1_POINTDIFFUSE_PROP_KERNEL_UNIT_LENGTH,
        (value.X >= 0.01f) && (value.Y >= 0.01f) && (value.X <= 100.0f) && (value.Y <= 100.0f))

    IMPLEMENT_EFFECT_PROPERTY(PointDiffuseEffect,
        HeightMapInterpolationMode,
        uint32_t,
        CanvasImageInterpolation,
        D2D1_POINTDIFFUSE_PROP_SCALE_MODE)

    IMPLEMENT_EFFECT_SOURCE_PROPERTY(PointDiffuseEffect,
        Source,
        0)

    IMPLEMENT_EFFECT_PROPERTY_MAPPING(PointDiffuseEffect,
        { L"LightPosition",              D2D1_POINTDIFFUSE_PROP_LIGHT_POSITION,     GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT           },
        { L"DiffuseAmount",              D2D1_POINTDIFFUSE_PROP_DIFFUSE_CONSTANT,   GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT           },
        { L"HeightMapScale",             D2D1_POINTDIFFUSE_PROP_SURFACE_SCALE,      GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT           },
        { L"LightColor",                 D2D1_POINTDIFFUSE_PROP_COLOR,              GRAPHICS_EFFECT_PROPERTY_MAPPING_COLOR_TO_VECTOR3 },
        { L"HeightMapKernelSize",        D2D1_POINTDIFFUSE_PROP_KERNEL_UNIT_LENGTH, GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT           },
        { L"HeightMapInterpolationMode", D2D1_POINTDIFFUSE_PROP_SCALE_MODE,         GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT           })

    ActivatableClass(PointDiffuseEffect);
}}}}}
