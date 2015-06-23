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
#include "PosterizeEffect.h"

#if (defined _WIN32_WINNT_WIN10) && (WINVER >= _WIN32_WINNT_WIN10)

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects
{
    PosterizeEffect::PosterizeEffect()
        : CanvasEffect(CLSID_D2D1Posterize, 3, 1, true)
    {
        // Set default values
        SetBoxedProperty<int32_t>(D2D1_POSTERIZE_PROP_RED_VALUE_COUNT, 4);
        SetBoxedProperty<int32_t>(D2D1_POSTERIZE_PROP_GREEN_VALUE_COUNT, 4);
        SetBoxedProperty<int32_t>(D2D1_POSTERIZE_PROP_BLUE_VALUE_COUNT, 4);
    }

    IMPLEMENT_EFFECT_PROPERTY_WITH_VALIDATION(PosterizeEffect,
        RedValueCount,
        int32_t,
        int32_t,
        D2D1_POSTERIZE_PROP_RED_VALUE_COUNT,
        (value >= 2) && (value <= 16))

    IMPLEMENT_EFFECT_PROPERTY_WITH_VALIDATION(PosterizeEffect,
        GreenValueCount,
        int32_t,
        int32_t,
        D2D1_POSTERIZE_PROP_GREEN_VALUE_COUNT,
        (value >= 2) && (value <= 16))

    IMPLEMENT_EFFECT_PROPERTY_WITH_VALIDATION(PosterizeEffect,
        BlueValueCount,
        int32_t,
        int32_t,
        D2D1_POSTERIZE_PROP_BLUE_VALUE_COUNT,
        (value >= 2) && (value <= 16))

    IMPLEMENT_EFFECT_SOURCE_PROPERTY(PosterizeEffect,
        Source,
        0)

    IMPLEMENT_EFFECT_PROPERTY_MAPPING(PosterizeEffect,
        { L"RedValueCount",   D2D1_POSTERIZE_PROP_RED_VALUE_COUNT,   GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
        { L"GreenValueCount", D2D1_POSTERIZE_PROP_GREEN_VALUE_COUNT, GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
        { L"BlueValueCount",  D2D1_POSTERIZE_PROP_BLUE_VALUE_COUNT,  GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT })

    ActivatableClass(PosterizeEffect);
}}}}}

#endif // _WIN32_WINNT_WIN10
