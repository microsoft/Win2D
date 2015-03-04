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
#include "DistantDiffuseEffect.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects
{
    DistantDiffuseEffect::DistantDiffuseEffect()
        : CanvasEffect(CLSID_D2D1DistantDiffuse, 7, 1, true)
    {
        // Set default values
        SetProperty<float>(D2D1_DISTANTDIFFUSE_PROP_AZIMUTH, 0.0f);
        SetProperty<float>(D2D1_DISTANTDIFFUSE_PROP_ELEVATION, 0.0f);
        SetProperty<float>(D2D1_DISTANTDIFFUSE_PROP_DIFFUSE_CONSTANT, 1.0f);
        SetProperty<float>(D2D1_DISTANTDIFFUSE_PROP_SURFACE_SCALE, 1.0f);
        SetProperty<float[3]>(D2D1_DISTANTDIFFUSE_PROP_COLOR, Color{ 255, 255, 255, 255 });
        SetProperty<float[2]>(D2D1_DISTANTDIFFUSE_PROP_KERNEL_UNIT_LENGTH, Numerics::Vector2{ 1.0f, 1.0f });
        SetProperty<uint32_t>(D2D1_DISTANTDIFFUSE_PROP_SCALE_MODE, D2D1_DISTANTDIFFUSE_SCALE_MODE_LINEAR);
    }

    IMPLEMENT_EFFECT_PROPERTY(DistantDiffuseEffect,
        Azimuth,
        ConvertRadiansToDegrees,
        float,
        D2D1_DISTANTDIFFUSE_PROP_AZIMUTH)

    IMPLEMENT_EFFECT_PROPERTY(DistantDiffuseEffect,
        Elevation,
        ConvertRadiansToDegrees,
        float,
        D2D1_DISTANTDIFFUSE_PROP_ELEVATION)

    IMPLEMENT_EFFECT_PROPERTY_WITH_VALIDATION(DistantDiffuseEffect,
        DiffuseAmount,
        float,
        float,
        D2D1_DISTANTDIFFUSE_PROP_DIFFUSE_CONSTANT,
        (value >= 0.0f) && (value <= 10000.0f))

    IMPLEMENT_EFFECT_PROPERTY_WITH_VALIDATION(DistantDiffuseEffect,
        HeightMapScale,
        float,
        float,
        D2D1_DISTANTDIFFUSE_PROP_SURFACE_SCALE,
        (value >= -10000.0f) && (value <= 10000.0f))

    IMPLEMENT_EFFECT_PROPERTY(DistantDiffuseEffect,
        LightColor,
        float[3],
        Color,
        D2D1_DISTANTDIFFUSE_PROP_COLOR)

    IMPLEMENT_EFFECT_PROPERTY_WITH_VALIDATION(DistantDiffuseEffect,
        HeightMapKernelSize,
        float[2],
        Numerics::Vector2,
        D2D1_DISTANTDIFFUSE_PROP_KERNEL_UNIT_LENGTH,
        (value.X >= 0.01f) && (value.Y >= 0.01f) && (value.X <= 100.0f) && (value.Y <= 100.0f))

    IMPLEMENT_EFFECT_PROPERTY(DistantDiffuseEffect,
        HeightMapInterpolationMode,
        uint32_t,
        CanvasImageInterpolation,
        D2D1_DISTANTDIFFUSE_PROP_SCALE_MODE)

    IMPLEMENT_EFFECT_INPUT_PROPERTY(DistantDiffuseEffect,
        Source,
        0)

    ActivatableClass(DistantDiffuseEffect);
}}}}}
