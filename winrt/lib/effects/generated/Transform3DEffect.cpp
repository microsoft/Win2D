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
#include "..\CanvasEffect.h"
#include "Transform3DEffect.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects
{
    Transform3DEffect::Transform3DEffect()
        : CanvasEffect(CLSID_D2D13DTransform, 3, 1, true)
    {
        // Set default values
        SetProperty(D2D1_3DTRANSFORM_PROP_INTERPOLATION_MODE, static_cast<uint32_t>(D2D1_3DTRANSFORM_INTERPOLATION_MODE_LINEAR), true);
        SetProperty(D2D1_3DTRANSFORM_PROP_BORDER_MODE, static_cast<uint32_t>(D2D1_BORDER_MODE_SOFT), true);
        Numerics::Matrix4x4 defaultTransformMatrix = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
        SetProperty(D2D1_3DTRANSFORM_PROP_TRANSFORM_MATRIX,  defaultTransformMatrix, true);
    }

    IMPLEMENT_ENUM_PROPERTY_WITH_UNSUPPORTED(Transform3DEffect,
        InterpolationMode,
        CanvasImageInterpolation,
        D2D1_3DTRANSFORM_PROP_INTERPOLATION_MODE,
        5U)

    IMPLEMENT_ENUM_PROPERTY(Transform3DEffect,
        BorderMode,
        EffectBorderMode,
        D2D1_3DTRANSFORM_PROP_BORDER_MODE)

    IMPLEMENT_PROPERTY(Transform3DEffect,
        TransformMatrix,
        Numerics::Matrix4x4,
        D2D1_3DTRANSFORM_PROP_TRANSFORM_MATRIX)

    IMPLEMENT_INPUT_PROPERTY(Transform3DEffect,
        Source,
        0)

    ActivatableClass(Transform3DEffect);
}}}}}
