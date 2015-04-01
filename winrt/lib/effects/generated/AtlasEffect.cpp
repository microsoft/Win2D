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
#include "AtlasEffect.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects
{
    AtlasEffect::AtlasEffect()
        : CanvasEffect(CLSID_D2D1Atlas, 2, 1, true)
    {
        // Set default values
        SetBoxedProperty<float[4]>(D2D1_ATLAS_PROP_INPUT_RECT, Rect{ 0, 0, std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity() });
        SetBoxedProperty<float[4]>(D2D1_ATLAS_PROP_INPUT_PADDING_RECT, Rect{ 0, 0, std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity() });
    }

    IMPLEMENT_EFFECT_PROPERTY(AtlasEffect,
        SourceRectangle,
        float[4],
        Rect,
        D2D1_ATLAS_PROP_INPUT_RECT)

    IMPLEMENT_EFFECT_PROPERTY(AtlasEffect,
        PaddingRectangle,
        float[4],
        Rect,
        D2D1_ATLAS_PROP_INPUT_PADDING_RECT)

    IMPLEMENT_EFFECT_SOURCE_PROPERTY(AtlasEffect,
        Source,
        0)

    IMPLEMENT_EFFECT_PROPERTY_MAPPING(AtlasEffect,
        { L"SourceRectangle",  D2D1_ATLAS_PROP_INPUT_RECT,         GRAPHICS_EFFECT_PROPERTY_MAPPING_RECT_TO_VECTOR4 },
        { L"PaddingRectangle", D2D1_ATLAS_PROP_INPUT_PADDING_RECT, GRAPHICS_EFFECT_PROPERTY_MAPPING_RECT_TO_VECTOR4 })

    ActivatableClass(AtlasEffect);
}}}}}
