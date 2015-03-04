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
#include "DisplacementMapEffect.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects
{
    DisplacementMapEffect::DisplacementMapEffect()
        : CanvasEffect(CLSID_D2D1DisplacementMap, 3, 2, true)
    {
        // Set default values
        SetProperty<float>(D2D1_DISPLACEMENTMAP_PROP_SCALE, 0.0f);
        SetProperty<uint32_t>(D2D1_DISPLACEMENTMAP_PROP_X_CHANNEL_SELECT, EffectChannelSelect::Alpha);
        SetProperty<uint32_t>(D2D1_DISPLACEMENTMAP_PROP_Y_CHANNEL_SELECT, EffectChannelSelect::Alpha);
    }

    IMPLEMENT_EFFECT_PROPERTY(DisplacementMapEffect,
        Amount,
        float,
        float,
        D2D1_DISPLACEMENTMAP_PROP_SCALE)

    IMPLEMENT_EFFECT_PROPERTY(DisplacementMapEffect,
        XChannelSelect,
        uint32_t,
        EffectChannelSelect,
        D2D1_DISPLACEMENTMAP_PROP_X_CHANNEL_SELECT)

    IMPLEMENT_EFFECT_PROPERTY(DisplacementMapEffect,
        YChannelSelect,
        uint32_t,
        EffectChannelSelect,
        D2D1_DISPLACEMENTMAP_PROP_Y_CHANNEL_SELECT)

    IMPLEMENT_EFFECT_INPUT_PROPERTY(DisplacementMapEffect,
        Source,
        0)

    IMPLEMENT_EFFECT_INPUT_PROPERTY(DisplacementMapEffect,
        Displacement,
        1)

    ActivatableClass(DisplacementMapEffect);
}}}}}
