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
#include "SepiaEffect.h"

#if (defined _WIN32_WINNT_WIN10) && (WINVER >= _WIN32_WINNT_WIN10)

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects
{
    SepiaEffect::SepiaEffect()
        : CanvasEffect(CLSID_D2D1Sepia, 2, 1, true)
    {
        // Set default values
        SetBoxedProperty<float>(D2D1_SEPIA_PROP_INTENSITY, 0.5f);
        SetBoxedProperty<uint32_t>(D2D1_SEPIA_PROP_ALPHA_MODE, D2D1_COLORMANAGEMENT_ALPHA_MODE_PREMULTIPLIED);
    }

    IMPLEMENT_EFFECT_PROPERTY_WITH_VALIDATION(SepiaEffect,
        Intensity,
        float,
        float,
        D2D1_SEPIA_PROP_INTENSITY,
        (value >= 0.0f) && (value <= 1.0f))

    IMPLEMENT_EFFECT_PROPERTY(SepiaEffect,
        AlphaMode,
        ConvertAlphaMode,
        CanvasAlphaMode,
        D2D1_SEPIA_PROP_ALPHA_MODE)

    IMPLEMENT_EFFECT_SOURCE_PROPERTY(SepiaEffect,
        Source,
        0)

    IMPLEMENT_EFFECT_PROPERTY_MAPPING(SepiaEffect,
        { L"Intensity", D2D1_SEPIA_PROP_INTENSITY,  GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT                 },
        { L"AlphaMode", D2D1_SEPIA_PROP_ALPHA_MODE, GRAPHICS_EFFECT_PROPERTY_MAPPING_COLORMATRIX_ALPHA_MODE })

    ActivatableClass(SepiaEffect);
}}}}}

#endif // _WIN32_WINNT_WIN10
