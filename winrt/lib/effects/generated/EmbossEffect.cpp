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
#include "EmbossEffect.h"

#if (defined _WIN32_WINNT_WIN10) && (WINVER >= _WIN32_WINNT_WIN10)

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects
{
    EmbossEffect::EmbossEffect()
        : CanvasEffect(CLSID_D2D1Emboss, 2, 1, true)
    {
        // Set default values
        SetBoxedProperty<float>(D2D1_EMBOSS_PROP_HEIGHT, 1.0f);
        SetBoxedProperty<float>(D2D1_EMBOSS_PROP_DIRECTION, 0.0f);
    }

    IMPLEMENT_EFFECT_PROPERTY_WITH_VALIDATION(EmbossEffect,
        Amount,
        float,
        float,
        D2D1_EMBOSS_PROP_HEIGHT,
        (value >= 0.0f) && (value <= 10.0f))

    IMPLEMENT_EFFECT_PROPERTY(EmbossEffect,
        Angle,
        ConvertRadiansToDegrees,
        float,
        D2D1_EMBOSS_PROP_DIRECTION)

    IMPLEMENT_EFFECT_SOURCE_PROPERTY(EmbossEffect,
        Source,
        0)

    IMPLEMENT_EFFECT_PROPERTY_MAPPING(EmbossEffect,
        { L"Amount", D2D1_EMBOSS_PROP_HEIGHT,    GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT             },
        { L"Angle",  D2D1_EMBOSS_PROP_DIRECTION, GRAPHICS_EFFECT_PROPERTY_MAPPING_RADIANS_TO_DEGREES })

    ActivatableClass(EmbossEffect);
}}}}}

#endif // _WIN32_WINNT_WIN10
