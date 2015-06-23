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
#include "ContrastEffect.h"

#if (defined _WIN32_WINNT_WIN10) && (WINVER >= _WIN32_WINNT_WIN10)

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects
{
    ContrastEffect::ContrastEffect()
        : CanvasEffect(CLSID_D2D1Contrast, 2, 1, true)
    {
        // Set default values
        SetBoxedProperty<float>(D2D1_CONTRAST_PROP_CONTRAST, 0.0f);
        SetBoxedProperty<boolean>(D2D1_CONTRAST_PROP_CLAMP_INPUT, static_cast<boolean>(false));
    }

    IMPLEMENT_EFFECT_PROPERTY_WITH_VALIDATION(ContrastEffect,
        Contrast,
        float,
        float,
        D2D1_CONTRAST_PROP_CONTRAST,
        (value >= -1.0f) && (value <= 1.0f))

    IMPLEMENT_EFFECT_PROPERTY(ContrastEffect,
        ClampSource,
        boolean,
        boolean,
        D2D1_CONTRAST_PROP_CLAMP_INPUT)

    IMPLEMENT_EFFECT_SOURCE_PROPERTY(ContrastEffect,
        Source,
        0)

    IMPLEMENT_EFFECT_PROPERTY_MAPPING(ContrastEffect,
        { L"Contrast",    D2D1_CONTRAST_PROP_CONTRAST,    GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
        { L"ClampSource", D2D1_CONTRAST_PROP_CLAMP_INPUT, GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT })

    ActivatableClass(ContrastEffect);
}}}}}

#endif // _WIN32_WINNT_WIN10
