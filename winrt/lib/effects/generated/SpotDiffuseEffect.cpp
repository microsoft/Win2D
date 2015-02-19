// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may
// not use these files except in compliance with the License. You may obtain
// a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
// License for the specific language governing permissions and limitations
// under the License.

// This file was automatically generated. Please do not edit it manually.

#include "pch.h"
#include "SpotDiffuseEffect.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects
{
    SpotDiffuseEffect::SpotDiffuseEffect()
        : CanvasEffect(CLSID_D2D1SpotDiffuse, 9, 1, true)
    {
        // Set default values
        SetProperty<float[3]>(D2D1_SPOTDIFFUSE_PROP_LIGHT_POSITION, Numerics::Vector3{ 0.0f, 0.0f, 0.0f });
        SetProperty<float[3]>(D2D1_SPOTDIFFUSE_PROP_POINTS_AT, Numerics::Vector3{ 0.0f, 0.0f, 0.0f });
        SetProperty<float>(D2D1_SPOTDIFFUSE_PROP_FOCUS, 1.0f);
        SetProperty<float>(D2D1_SPOTDIFFUSE_PROP_LIMITING_CONE_ANGLE, 90.0f);
        SetProperty<float>(D2D1_SPOTDIFFUSE_PROP_DIFFUSE_CONSTANT, 1.0f);
        SetProperty<float>(D2D1_SPOTDIFFUSE_PROP_SURFACE_SCALE, 1.0f);
        SetProperty<float[3]>(D2D1_SPOTDIFFUSE_PROP_COLOR, Color{ 255, 255, 255, 255 });
        SetProperty<float[2]>(D2D1_SPOTDIFFUSE_PROP_KERNEL_UNIT_LENGTH, Numerics::Vector2{ 1.0f, 1.0f });
        SetProperty<uint32_t>(D2D1_SPOTDIFFUSE_PROP_SCALE_MODE, D2D1_SPOTDIFFUSE_SCALE_MODE_LINEAR);
    }

    IMPLEMENT_EFFECT_PROPERTY(SpotDiffuseEffect,
        LightPosition,
        float[3],
        Numerics::Vector3,
        D2D1_SPOTDIFFUSE_PROP_LIGHT_POSITION)

    IMPLEMENT_EFFECT_PROPERTY(SpotDiffuseEffect,
        LightTarget,
        float[3],
        Numerics::Vector3,
        D2D1_SPOTDIFFUSE_PROP_POINTS_AT)

    IMPLEMENT_EFFECT_PROPERTY_WITH_VALIDATION(SpotDiffuseEffect,
        Focus,
        float,
        float,
        D2D1_SPOTDIFFUSE_PROP_FOCUS,
        (value >= -10000.0f) && (value <= 10000.0f))

    IMPLEMENT_EFFECT_PROPERTY(SpotDiffuseEffect,
        LimitingConeAngle,
        ConvertRadiansToDegrees,
        float,
        D2D1_SPOTDIFFUSE_PROP_LIMITING_CONE_ANGLE)

    IMPLEMENT_EFFECT_PROPERTY_WITH_VALIDATION(SpotDiffuseEffect,
        DiffuseAmount,
        float,
        float,
        D2D1_SPOTDIFFUSE_PROP_DIFFUSE_CONSTANT,
        (value >= 0.0f) && (value <= 10000.0f))

    IMPLEMENT_EFFECT_PROPERTY_WITH_VALIDATION(SpotDiffuseEffect,
        HeightMapScale,
        float,
        float,
        D2D1_SPOTDIFFUSE_PROP_SURFACE_SCALE,
        (value >= -10000.0f) && (value <= 10000.0f))

    IMPLEMENT_EFFECT_PROPERTY(SpotDiffuseEffect,
        LightColor,
        float[3],
        Color,
        D2D1_SPOTDIFFUSE_PROP_COLOR)

    IMPLEMENT_EFFECT_PROPERTY_WITH_VALIDATION(SpotDiffuseEffect,
        HeightMapKernelSize,
        float[2],
        Numerics::Vector2,
        D2D1_SPOTDIFFUSE_PROP_KERNEL_UNIT_LENGTH,
        (value.X >= 0.01f) && (value.Y >= 0.01f) && (value.X <= 100.0f) && (value.Y <= 100.0f))

    IMPLEMENT_EFFECT_PROPERTY(SpotDiffuseEffect,
        HeightMapInterpolationMode,
        uint32_t,
        CanvasImageInterpolation,
        D2D1_SPOTDIFFUSE_PROP_SCALE_MODE)

    IMPLEMENT_EFFECT_INPUT_PROPERTY(SpotDiffuseEffect,
        Source,
        0)

    ActivatableClass(SpotDiffuseEffect);
}}}}}
