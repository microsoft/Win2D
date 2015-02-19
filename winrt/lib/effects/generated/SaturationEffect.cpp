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
#include "SaturationEffect.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects
{
    SaturationEffect::SaturationEffect()
        : CanvasEffect(CLSID_D2D1Saturation, 1, 1, true)
    {
        // Set default values
        SetProperty<float>(D2D1_SATURATION_PROP_SATURATION, 0.5f);
    }

    IMPLEMENT_EFFECT_PROPERTY_WITH_VALIDATION(SaturationEffect,
        Saturation,
        float,
        float,
        D2D1_SATURATION_PROP_SATURATION,
        (value >= 0.0f) && (value <= 2.0f))

    IMPLEMENT_EFFECT_INPUT_PROPERTY(SaturationEffect,
        Source,
        0)

    ActivatableClass(SaturationEffect);
}}}}}
