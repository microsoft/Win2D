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
#include "ColorSourceEffect.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects
{
    ColorSourceEffect::ColorSourceEffect()
        : CanvasEffect(CLSID_D2D1Flood, 1, 0, true)
    {
        // Set default values
        SetBoxedProperty<float[4]>(D2D1_FLOOD_PROP_COLOR, Color{ 255, 0, 0, 0 });
    }

    IMPLEMENT_EFFECT_PROPERTY(ColorSourceEffect,
        Color,
        float[4],
        Color,
        D2D1_FLOOD_PROP_COLOR)

    ActivatableClass(ColorSourceEffect);
}}}}}
