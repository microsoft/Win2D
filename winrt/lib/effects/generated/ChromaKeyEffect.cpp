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
#include "ChromaKeyEffect.h"

#if (defined _WIN32_WINNT_WIN10) && (WINVER >= _WIN32_WINNT_WIN10)

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects
{
    ChromaKeyEffect::ChromaKeyEffect()
        : CanvasEffect(CLSID_D2D1ChromaKey, 4, 1, true)
    {
        // Set default values
        SetBoxedProperty<float[3]>(D2D1_CHROMAKEY_PROP_COLOR, Color{ 255, 0, 0, 0 });
        SetBoxedProperty<float>(D2D1_CHROMAKEY_PROP_TOLERANCE, 0.1f);
        SetBoxedProperty<boolean>(D2D1_CHROMAKEY_PROP_INVERT_ALPHA, static_cast<boolean>(false));
        SetBoxedProperty<boolean>(D2D1_CHROMAKEY_PROP_FEATHER, static_cast<boolean>(false));
    }

    IMPLEMENT_EFFECT_PROPERTY(ChromaKeyEffect,
        Color,
        float[3],
        Color,
        D2D1_CHROMAKEY_PROP_COLOR)

    IMPLEMENT_EFFECT_PROPERTY_WITH_VALIDATION(ChromaKeyEffect,
        Tolerance,
        float,
        float,
        D2D1_CHROMAKEY_PROP_TOLERANCE,
        (value >= 0.0f) && (value <= 1.0f))

    IMPLEMENT_EFFECT_PROPERTY(ChromaKeyEffect,
        InvertAlpha,
        boolean,
        boolean,
        D2D1_CHROMAKEY_PROP_INVERT_ALPHA)

    IMPLEMENT_EFFECT_PROPERTY(ChromaKeyEffect,
        Feather,
        boolean,
        boolean,
        D2D1_CHROMAKEY_PROP_FEATHER)

    IMPLEMENT_EFFECT_SOURCE_PROPERTY(ChromaKeyEffect,
        Source,
        0)

    IMPLEMENT_EFFECT_PROPERTY_MAPPING(ChromaKeyEffect,
        { L"Color",       D2D1_CHROMAKEY_PROP_COLOR,        GRAPHICS_EFFECT_PROPERTY_MAPPING_COLOR_TO_VECTOR3 },
        { L"Tolerance",   D2D1_CHROMAKEY_PROP_TOLERANCE,    GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT           },
        { L"InvertAlpha", D2D1_CHROMAKEY_PROP_INVERT_ALPHA, GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT           },
        { L"Feather",     D2D1_CHROMAKEY_PROP_FEATHER,      GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT           })

    ActivatableClass(ChromaKeyEffect);
}}}}}

#endif // _WIN32_WINNT_WIN10
