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
#include "HueToRgbEffect.h"

#if (defined _WIN32_WINNT_WIN10) && (WINVER >= _WIN32_WINNT_WIN10)

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects
{
    HueToRgbEffect::HueToRgbEffect()
        : CanvasEffect(CLSID_D2D1HueToRgb, 1, 1, true)
    {
        // Set default values
        SetBoxedProperty<uint32_t>(D2D1_HUETORGB_PROP_INPUT_COLOR_SPACE, EffectHueColorSpace::Hsv);
    }

    IMPLEMENT_EFFECT_PROPERTY(HueToRgbEffect,
        SourceColorSpace,
        uint32_t,
        EffectHueColorSpace,
        D2D1_HUETORGB_PROP_INPUT_COLOR_SPACE)

    IMPLEMENT_EFFECT_SOURCE_PROPERTY(HueToRgbEffect,
        Source,
        0)

    IMPLEMENT_EFFECT_PROPERTY_MAPPING(HueToRgbEffect,
        { L"SourceColorSpace", D2D1_HUETORGB_PROP_INPUT_COLOR_SPACE, GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT })

    ActivatableClass(HueToRgbEffect);
}}}}}

#endif // _WIN32_WINNT_WIN10
