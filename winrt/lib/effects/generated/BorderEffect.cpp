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
#include "BorderEffect.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects
{
    BorderEffect::BorderEffect()
        : CanvasEffect(CLSID_D2D1Border, 2, 1, true)
    {
        // Set default values
        SetBoxedProperty<uint32_t>(D2D1_BORDER_PROP_EDGE_MODE_X, D2D1_BORDER_EDGE_MODE_CLAMP);
        SetBoxedProperty<uint32_t>(D2D1_BORDER_PROP_EDGE_MODE_Y, D2D1_BORDER_EDGE_MODE_CLAMP);
    }

    IMPLEMENT_EFFECT_PROPERTY(BorderEffect,
        ExtendX,
        uint32_t,
        CanvasEdgeBehavior,
        D2D1_BORDER_PROP_EDGE_MODE_X)

    IMPLEMENT_EFFECT_PROPERTY(BorderEffect,
        ExtendY,
        uint32_t,
        CanvasEdgeBehavior,
        D2D1_BORDER_PROP_EDGE_MODE_Y)

    IMPLEMENT_EFFECT_SOURCE_PROPERTY(BorderEffect,
        Source,
        0)

    ActivatableClass(BorderEffect);
}}}}}
