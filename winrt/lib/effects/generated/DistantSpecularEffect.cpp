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
#include "DistantSpecularEffect.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects
{
    DistantSpecularEffect::DistantSpecularEffect()
        : CanvasEffect(CLSID_D2D1DistantSpecular, 8, 1, true)
    {
        // Set default values
        SetBoxedProperty<float>(D2D1_DISTANTSPECULAR_PROP_AZIMUTH, 0.0f);
        SetBoxedProperty<float>(D2D1_DISTANTSPECULAR_PROP_ELEVATION, 0.0f);
        SetBoxedProperty<float>(D2D1_DISTANTSPECULAR_PROP_SPECULAR_EXPONENT, 1.0f);
        SetBoxedProperty<float>(D2D1_DISTANTSPECULAR_PROP_SPECULAR_CONSTANT, 1.0f);
        SetBoxedProperty<float>(D2D1_DISTANTSPECULAR_PROP_SURFACE_SCALE, 1.0f);
        SetBoxedProperty<float[3]>(D2D1_DISTANTSPECULAR_PROP_COLOR, Color{ 255, 255, 255, 255 });
        SetBoxedProperty<float[2]>(D2D1_DISTANTSPECULAR_PROP_KERNEL_UNIT_LENGTH, Numerics::Vector2{ 1.0f, 1.0f });
        SetBoxedProperty<uint32_t>(D2D1_DISTANTSPECULAR_PROP_SCALE_MODE, D2D1_DISTANTSPECULAR_SCALE_MODE_LINEAR);
    }

    IMPLEMENT_EFFECT_PROPERTY(DistantSpecularEffect,
        Azimuth,
        ConvertRadiansToDegrees,
        float,
        D2D1_DISTANTSPECULAR_PROP_AZIMUTH)

    IMPLEMENT_EFFECT_PROPERTY(DistantSpecularEffect,
        Elevation,
        ConvertRadiansToDegrees,
        float,
        D2D1_DISTANTSPECULAR_PROP_ELEVATION)

    IMPLEMENT_EFFECT_PROPERTY_WITH_VALIDATION(DistantSpecularEffect,
        SpecularExponent,
        float,
        float,
        D2D1_DISTANTSPECULAR_PROP_SPECULAR_EXPONENT,
        (value >= -10000.0f) && (value <= 10000.0f))

    IMPLEMENT_EFFECT_PROPERTY_WITH_VALIDATION(DistantSpecularEffect,
        SpecularAmount,
        float,
        float,
        D2D1_DISTANTSPECULAR_PROP_SPECULAR_CONSTANT,
        (value >= 0.0f) && (value <= 10000.0f))

    IMPLEMENT_EFFECT_PROPERTY_WITH_VALIDATION(DistantSpecularEffect,
        HeightMapScale,
        float,
        float,
        D2D1_DISTANTSPECULAR_PROP_SURFACE_SCALE,
        (value >= -10000.0f) && (value <= 10000.0f))

    IMPLEMENT_EFFECT_PROPERTY(DistantSpecularEffect,
        LightColor,
        float[3],
        Color,
        D2D1_DISTANTSPECULAR_PROP_COLOR)

    IMPLEMENT_EFFECT_PROPERTY_WITH_VALIDATION(DistantSpecularEffect,
        HeightMapKernelSize,
        float[2],
        Numerics::Vector2,
        D2D1_DISTANTSPECULAR_PROP_KERNEL_UNIT_LENGTH,
        (value.X >= 0.01f) && (value.Y >= 0.01f) && (value.X <= 100.0f) && (value.Y <= 100.0f))

    IMPLEMENT_EFFECT_PROPERTY(DistantSpecularEffect,
        HeightMapInterpolationMode,
        uint32_t,
        CanvasImageInterpolation,
        D2D1_DISTANTSPECULAR_PROP_SCALE_MODE)

    IMPLEMENT_EFFECT_SOURCE_PROPERTY(DistantSpecularEffect,
        Source,
        0)

    IMPLEMENT_EFFECT_PROPERTY_MAPPING(DistantSpecularEffect,
        { L"Azimuth",                    D2D1_DISTANTSPECULAR_PROP_AZIMUTH,            GRAPHICS_EFFECT_PROPERTY_MAPPING_RADIANS_TO_DEGREES },
        { L"Elevation",                  D2D1_DISTANTSPECULAR_PROP_ELEVATION,          GRAPHICS_EFFECT_PROPERTY_MAPPING_RADIANS_TO_DEGREES },
        { L"SpecularExponent",           D2D1_DISTANTSPECULAR_PROP_SPECULAR_EXPONENT,  GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT             },
        { L"SpecularAmount",             D2D1_DISTANTSPECULAR_PROP_SPECULAR_CONSTANT,  GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT             },
        { L"HeightMapScale",             D2D1_DISTANTSPECULAR_PROP_SURFACE_SCALE,      GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT             },
        { L"LightColor",                 D2D1_DISTANTSPECULAR_PROP_COLOR,              GRAPHICS_EFFECT_PROPERTY_MAPPING_COLOR_TO_VECTOR3   },
        { L"HeightMapKernelSize",        D2D1_DISTANTSPECULAR_PROP_KERNEL_UNIT_LENGTH, GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT             },
        { L"HeightMapInterpolationMode", D2D1_DISTANTSPECULAR_PROP_SCALE_MODE,         GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT             })

    ActivatableClass(DistantSpecularEffect);
}}}}}
