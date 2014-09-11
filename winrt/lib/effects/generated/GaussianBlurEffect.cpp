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
#include "..\CanvasEffect.h"
#include "GaussianBlurEffect.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects
{
    GaussianBlurEffect::GaussianBlurEffect()
        : CanvasEffect(CLSID_D2D1GaussianBlur, 3, 1, true)
    {
        // Set default values
        SetProperty(D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION, 3.0f, true);
        SetProperty(D2D1_GAUSSIANBLUR_PROP_OPTIMIZATION, static_cast<uint32_t>(D2D1_GAUSSIANBLUR_OPTIMIZATION_BALANCED), true);
        SetProperty(D2D1_GAUSSIANBLUR_PROP_BORDER_MODE, static_cast<uint32_t>(D2D1_BORDER_MODE_SOFT), true);
    }

    IMPLEMENT_PROPERTY_WITH_VALIDATION(GaussianBlurEffect,
        StandardDeviation,
        float,
        D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION,
        0.0,
        250.0)

    IMPLEMENT_ENUM_PROPERTY(GaussianBlurEffect,
        Optimization,
        EffectOptimization,
        D2D1_GAUSSIANBLUR_PROP_OPTIMIZATION)

    IMPLEMENT_ENUM_PROPERTY(GaussianBlurEffect,
        BorderMode,
        EffectBorderMode,
        D2D1_GAUSSIANBLUR_PROP_BORDER_MODE)

    IMPLEMENT_INPUT_PROPERTY(GaussianBlurEffect,
        Source,
        0)

    ActivatableClass(GaussianBlurEffect);
}}}}}
