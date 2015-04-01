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
#include "GaussianBlurEffect.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects
{
    GaussianBlurEffect::GaussianBlurEffect()
        : CanvasEffect(CLSID_D2D1GaussianBlur, 3, 1, true)
    {
        // Set default values
        SetBoxedProperty<float>(D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION, 3.0f);
        SetBoxedProperty<uint32_t>(D2D1_GAUSSIANBLUR_PROP_OPTIMIZATION, D2D1_GAUSSIANBLUR_OPTIMIZATION_BALANCED);
        SetBoxedProperty<uint32_t>(D2D1_GAUSSIANBLUR_PROP_BORDER_MODE, D2D1_BORDER_MODE_SOFT);
    }

    IMPLEMENT_EFFECT_PROPERTY_WITH_VALIDATION(GaussianBlurEffect,
        BlurAmount,
        float,
        float,
        D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION,
        (value >= 0.0f) && (value <= 250.0f))

    IMPLEMENT_EFFECT_PROPERTY(GaussianBlurEffect,
        Optimization,
        uint32_t,
        EffectOptimization,
        D2D1_GAUSSIANBLUR_PROP_OPTIMIZATION)

    IMPLEMENT_EFFECT_PROPERTY(GaussianBlurEffect,
        BorderMode,
        uint32_t,
        EffectBorderMode,
        D2D1_GAUSSIANBLUR_PROP_BORDER_MODE)

    IMPLEMENT_EFFECT_SOURCE_PROPERTY(GaussianBlurEffect,
        Source,
        0)

    IMPLEMENT_EFFECT_PROPERTY_MAPPING(GaussianBlurEffect,
        { L"BlurAmount",   D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION, GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
        { L"Optimization", D2D1_GAUSSIANBLUR_PROP_OPTIMIZATION,       GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
        { L"BorderMode",   D2D1_GAUSSIANBLUR_PROP_BORDER_MODE,        GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT })

    ActivatableClass(GaussianBlurEffect);
}}}}}
