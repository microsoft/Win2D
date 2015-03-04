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
#include "CropEffect.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects
{
    CropEffect::CropEffect()
        : CanvasEffect(CLSID_D2D1Crop, 2, 1, true)
    {
        // Set default values
        SetProperty<float[4]>(D2D1_CROP_PROP_RECT, Rect{ -std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity() });
        SetProperty<uint32_t>(D2D1_CROP_PROP_BORDER_MODE, D2D1_BORDER_MODE_SOFT);
    }

    IMPLEMENT_EFFECT_PROPERTY(CropEffect,
        SourceRectangle,
        float[4],
        Rect,
        D2D1_CROP_PROP_RECT)

    IMPLEMENT_EFFECT_PROPERTY(CropEffect,
        BorderMode,
        uint32_t,
        EffectBorderMode,
        D2D1_CROP_PROP_BORDER_MODE)

    IMPLEMENT_EFFECT_INPUT_PROPERTY(CropEffect,
        Source,
        0)

    ActivatableClass(CropEffect);
}}}}}
