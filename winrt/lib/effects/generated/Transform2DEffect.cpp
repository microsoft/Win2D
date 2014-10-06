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
#include "Transform2DEffect.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects
{
    Transform2DEffect::Transform2DEffect()
        : CanvasEffect(CLSID_D2D12DAffineTransform, 4, 1, true)
    {
        // Set default values
        SetProperty<uint32_t>(D2D1_2DAFFINETRANSFORM_PROP_INTERPOLATION_MODE, D2D1_2DAFFINETRANSFORM_INTERPOLATION_MODE_LINEAR);
        SetProperty<uint32_t>(D2D1_2DAFFINETRANSFORM_PROP_BORDER_MODE, D2D1_BORDER_MODE_SOFT);
        SetProperty<float[6]>(D2D1_2DAFFINETRANSFORM_PROP_TRANSFORM_MATRIX, Numerics::Matrix3x2{ 1, 0, 0, 1, 0, 0 });
        SetProperty<float>(D2D1_2DAFFINETRANSFORM_PROP_SHARPNESS, 0.0f);
    }

    IMPLEMENT_PROPERTY(Transform2DEffect,
        InterpolationMode,
        uint32_t,
        CanvasImageInterpolation,
        D2D1_2DAFFINETRANSFORM_PROP_INTERPOLATION_MODE)

    IMPLEMENT_PROPERTY(Transform2DEffect,
        BorderMode,
        uint32_t,
        EffectBorderMode,
        D2D1_2DAFFINETRANSFORM_PROP_BORDER_MODE)

    IMPLEMENT_PROPERTY(Transform2DEffect,
        TransformMatrix,
        float[6],
        Numerics::Matrix3x2,
        D2D1_2DAFFINETRANSFORM_PROP_TRANSFORM_MATRIX)

    IMPLEMENT_PROPERTY_WITH_VALIDATION(Transform2DEffect,
        Sharpness,
        float,
        float,
        D2D1_2DAFFINETRANSFORM_PROP_SHARPNESS,
        (value >= 0.0f) && (value <= 1.0f))

    IMPLEMENT_INPUT_PROPERTY(Transform2DEffect,
        Source,
        0)

    ActivatableClass(Transform2DEffect);
}}}}}
