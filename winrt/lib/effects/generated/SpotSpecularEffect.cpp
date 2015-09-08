// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

// This file was automatically generated. Please do not edit it manually.

#include "pch.h"
#include "SpotSpecularEffect.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects
{
    SpotSpecularEffect::SpotSpecularEffect(ICanvasDevice* device, ID2D1Effect* effect)
        : CanvasEffect(EffectId(), 10, 1, true, device, effect, static_cast<ISpotSpecularEffect*>(this))
    {
        if (!effect)
        {
            // Set default values
            SetBoxedProperty<float[3]>(D2D1_SPOTSPECULAR_PROP_LIGHT_POSITION, Numerics::Vector3{ 0.0f, 0.0f, 0.0f });
            SetBoxedProperty<float[3]>(D2D1_SPOTSPECULAR_PROP_POINTS_AT, Numerics::Vector3{ 0.0f, 0.0f, 0.0f });
            SetBoxedProperty<float>(D2D1_SPOTSPECULAR_PROP_FOCUS, 1.0f);
            SetBoxedProperty<float>(D2D1_SPOTSPECULAR_PROP_LIMITING_CONE_ANGLE, 90.0f);
            SetBoxedProperty<float>(D2D1_SPOTSPECULAR_PROP_SPECULAR_EXPONENT, 1.0f);
            SetBoxedProperty<float>(D2D1_SPOTSPECULAR_PROP_SPECULAR_CONSTANT, 1.0f);
            SetBoxedProperty<float>(D2D1_SPOTSPECULAR_PROP_SURFACE_SCALE, 1.0f);
            SetBoxedProperty<float[3]>(D2D1_SPOTSPECULAR_PROP_COLOR, Color{ 255, 255, 255, 255 });
            SetBoxedProperty<float[2]>(D2D1_SPOTSPECULAR_PROP_KERNEL_UNIT_LENGTH, Numerics::Vector2{ 1.0f, 1.0f });
            SetBoxedProperty<uint32_t>(D2D1_SPOTSPECULAR_PROP_SCALE_MODE, D2D1_SPOTSPECULAR_SCALE_MODE_LINEAR);
        }
    }

    IMPLEMENT_EFFECT_PROPERTY(SpotSpecularEffect,
        LightPosition,
        float[3],
        Numerics::Vector3,
        D2D1_SPOTSPECULAR_PROP_LIGHT_POSITION)

    IMPLEMENT_EFFECT_PROPERTY(SpotSpecularEffect,
        LightTarget,
        float[3],
        Numerics::Vector3,
        D2D1_SPOTSPECULAR_PROP_POINTS_AT)

    IMPLEMENT_EFFECT_PROPERTY_WITH_VALIDATION(SpotSpecularEffect,
        Focus,
        float,
        float,
        D2D1_SPOTSPECULAR_PROP_FOCUS,
        (value >= -10000.0f) && (value <= 10000.0f))

    IMPLEMENT_EFFECT_PROPERTY(SpotSpecularEffect,
        LimitingConeAngle,
        ConvertRadiansToDegrees,
        float,
        D2D1_SPOTSPECULAR_PROP_LIMITING_CONE_ANGLE)

    IMPLEMENT_EFFECT_PROPERTY_WITH_VALIDATION(SpotSpecularEffect,
        SpecularExponent,
        float,
        float,
        D2D1_SPOTSPECULAR_PROP_SPECULAR_EXPONENT,
        (value >= -10000.0f) && (value <= 10000.0f))

    IMPLEMENT_EFFECT_PROPERTY_WITH_VALIDATION(SpotSpecularEffect,
        SpecularAmount,
        float,
        float,
        D2D1_SPOTSPECULAR_PROP_SPECULAR_CONSTANT,
        (value >= 0.0f) && (value <= 10000.0f))

    IMPLEMENT_EFFECT_PROPERTY_WITH_VALIDATION(SpotSpecularEffect,
        HeightMapScale,
        float,
        float,
        D2D1_SPOTSPECULAR_PROP_SURFACE_SCALE,
        (value >= -10000.0f) && (value <= 10000.0f))

    IMPLEMENT_EFFECT_PROPERTY(SpotSpecularEffect,
        LightColor,
        float[3],
        Color,
        D2D1_SPOTSPECULAR_PROP_COLOR)

    IMPLEMENT_EFFECT_PROPERTY_WITH_VALIDATION(SpotSpecularEffect,
        HeightMapKernelSize,
        float[2],
        Numerics::Vector2,
        D2D1_SPOTSPECULAR_PROP_KERNEL_UNIT_LENGTH,
        (value.X >= 0.01f) && (value.Y >= 0.01f) && (value.X <= 100.0f) && (value.Y <= 100.0f))

    IMPLEMENT_EFFECT_PROPERTY(SpotSpecularEffect,
        HeightMapInterpolationMode,
        uint32_t,
        CanvasImageInterpolation,
        D2D1_SPOTSPECULAR_PROP_SCALE_MODE)

    IMPLEMENT_EFFECT_SOURCE_PROPERTY(SpotSpecularEffect,
        Source,
        0)

    IMPLEMENT_EFFECT_PROPERTY_MAPPING(SpotSpecularEffect,
        { L"LightPosition",              D2D1_SPOTSPECULAR_PROP_LIGHT_POSITION,      GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT             },
        { L"LightTarget",                D2D1_SPOTSPECULAR_PROP_POINTS_AT,           GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT             },
        { L"Focus",                      D2D1_SPOTSPECULAR_PROP_FOCUS,               GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT             },
        { L"LimitingConeAngle",          D2D1_SPOTSPECULAR_PROP_LIMITING_CONE_ANGLE, GRAPHICS_EFFECT_PROPERTY_MAPPING_RADIANS_TO_DEGREES },
        { L"SpecularExponent",           D2D1_SPOTSPECULAR_PROP_SPECULAR_EXPONENT,   GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT             },
        { L"SpecularAmount",             D2D1_SPOTSPECULAR_PROP_SPECULAR_CONSTANT,   GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT             },
        { L"HeightMapScale",             D2D1_SPOTSPECULAR_PROP_SURFACE_SCALE,       GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT             },
        { L"LightColor",                 D2D1_SPOTSPECULAR_PROP_COLOR,               GRAPHICS_EFFECT_PROPERTY_MAPPING_COLOR_TO_VECTOR3   },
        { L"HeightMapKernelSize",        D2D1_SPOTSPECULAR_PROP_KERNEL_UNIT_LENGTH,  GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT             },
        { L"HeightMapInterpolationMode", D2D1_SPOTSPECULAR_PROP_SCALE_MODE,          GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT             })

    ActivatableClass(SpotSpecularEffect);
}}}}}
