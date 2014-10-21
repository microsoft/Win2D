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
#include "MorphologyEffect.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects
{
    MorphologyEffect::MorphologyEffect()
        : CanvasEffect(CLSID_D2D1Morphology, 3, 1, true)
    {
        // Set default values
        SetProperty<uint32_t>(D2D1_MORPHOLOGY_PROP_MODE, D2D1_MORPHOLOGY_MODE_ERODE);
        SetProperty<int32_t>(D2D1_MORPHOLOGY_PROP_WIDTH, 1);
        SetProperty<int32_t>(D2D1_MORPHOLOGY_PROP_HEIGHT, 1);
    }

    IMPLEMENT_PROPERTY(MorphologyEffect,
        Mode,
        uint32_t,
        MorphologyEffectMode,
        D2D1_MORPHOLOGY_PROP_MODE)

    IMPLEMENT_PROPERTY_WITH_VALIDATION(MorphologyEffect,
        Width,
        int32_t,
        int32_t,
        D2D1_MORPHOLOGY_PROP_WIDTH,
        (value >= 1) && (value <= 100))

    IMPLEMENT_PROPERTY_WITH_VALIDATION(MorphologyEffect,
        Height,
        int32_t,
        int32_t,
        D2D1_MORPHOLOGY_PROP_HEIGHT,
        (value >= 1) && (value <= 100))

    IMPLEMENT_INPUT_PROPERTY(MorphologyEffect,
        Source,
        0)

    ActivatableClass(MorphologyEffect);
}}}}}
