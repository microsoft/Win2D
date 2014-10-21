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
#include "TurbulenceEffect.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects
{
    TurbulenceEffect::TurbulenceEffect()
        : CanvasEffect(CLSID_D2D1Turbulence, 7, 0, true)
    {
        // Set default values
        SetProperty<float[2]>(D2D1_TURBULENCE_PROP_OFFSET, Numerics::Vector2{ 0, 0 });
        SetProperty<float[2]>(D2D1_TURBULENCE_PROP_SIZE, Numerics::Vector2{ 512, 512 });
        SetProperty<float[2]>(D2D1_TURBULENCE_PROP_BASE_FREQUENCY, Numerics::Vector2{ 0.01f, 0.01f });
        SetProperty<int32_t>(D2D1_TURBULENCE_PROP_NUM_OCTAVES, 1);
        SetProperty<int32_t>(D2D1_TURBULENCE_PROP_SEED, 0);
        SetProperty<uint32_t>(D2D1_TURBULENCE_PROP_NOISE, D2D1_TURBULENCE_NOISE_FRACTAL_SUM);
        SetProperty<boolean>(D2D1_TURBULENCE_PROP_STITCHABLE, static_cast<boolean>(false));
    }

    IMPLEMENT_PROPERTY(TurbulenceEffect,
        Offset,
        float[2],
        Numerics::Vector2,
        D2D1_TURBULENCE_PROP_OFFSET)

    IMPLEMENT_PROPERTY_WITH_VALIDATION(TurbulenceEffect,
        Size,
        float[2],
        Numerics::Vector2,
        D2D1_TURBULENCE_PROP_SIZE,
        (value.X >= 0) && (value.Y >= 0))

    IMPLEMENT_PROPERTY_WITH_VALIDATION(TurbulenceEffect,
        Frequency,
        float[2],
        Numerics::Vector2,
        D2D1_TURBULENCE_PROP_BASE_FREQUENCY,
        (value.X >= 0) && (value.Y >= 0) && (value.X <= 1000) && (value.Y <= 1000))

    IMPLEMENT_PROPERTY_WITH_VALIDATION(TurbulenceEffect,
        Octaves,
        int32_t,
        int32_t,
        D2D1_TURBULENCE_PROP_NUM_OCTAVES,
        (value >= 1) && (value <= 15))

    IMPLEMENT_PROPERTY_WITH_VALIDATION(TurbulenceEffect,
        Seed,
        int32_t,
        int32_t,
        D2D1_TURBULENCE_PROP_SEED,
        (value >= -10000) && (value <= 10000))

    IMPLEMENT_PROPERTY(TurbulenceEffect,
        Noise,
        uint32_t,
        TurbulenceEffectNoise,
        D2D1_TURBULENCE_PROP_NOISE)

    IMPLEMENT_PROPERTY(TurbulenceEffect,
        Tileable,
        boolean,
        boolean,
        D2D1_TURBULENCE_PROP_STITCHABLE)

    ActivatableClass(TurbulenceEffect);
}}}}}
