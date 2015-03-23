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
#include "TileEffect.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects
{
    TileEffect::TileEffect()
        : CanvasEffect(CLSID_D2D1Tile, 1, 1, true)
    {
        // Set default values
        SetBoxedProperty<float[4]>(D2D1_TILE_PROP_RECT, Rect{ 0, 0, 100, 100 });
    }

    IMPLEMENT_EFFECT_PROPERTY(TileEffect,
        SourceRectangle,
        float[4],
        Rect,
        D2D1_TILE_PROP_RECT)

    IMPLEMENT_EFFECT_SOURCE_PROPERTY(TileEffect,
        Source,
        0)

    ActivatableClass(TileEffect);
}}}}}
