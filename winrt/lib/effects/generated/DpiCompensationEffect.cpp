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
#include "DpiCompensationEffect.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects
{
    DpiCompensationEffect::DpiCompensationEffect()
        : CanvasEffect(CLSID_D2D1DpiCompensation, 3, 1, true)
    {
        // Set default values
        SetProperty<uint32_t>(D2D1_DPICOMPENSATION_PROP_INTERPOLATION_MODE, D2D1_DPICOMPENSATION_INTERPOLATION_MODE_LINEAR);
        SetProperty<uint32_t>(D2D1_DPICOMPENSATION_PROP_BORDER_MODE, D2D1_BORDER_MODE_HARD);
        SetProperty<float[2]>(D2D1_DPICOMPENSATION_PROP_INPUT_DPI, Numerics::Vector2{ 96, 96 });
    }

    IMPLEMENT_PROPERTY(DpiCompensationEffect,
        InterpolationMode,
        uint32_t,
        CanvasImageInterpolation,
        D2D1_DPICOMPENSATION_PROP_INTERPOLATION_MODE)

    IMPLEMENT_PROPERTY(DpiCompensationEffect,
        BorderMode,
        uint32_t,
        EffectBorderMode,
        D2D1_DPICOMPENSATION_PROP_BORDER_MODE)

    IMPLEMENT_PROPERTY(DpiCompensationEffect,
        SourceDpi,
        float[2],
        Numerics::Vector2,
        D2D1_DPICOMPENSATION_PROP_INPUT_DPI)

    IMPLEMENT_INPUT_PROPERTY(DpiCompensationEffect,
        Source,
        0)

    ActivatableClass(DpiCompensationEffect);
}}}}}
