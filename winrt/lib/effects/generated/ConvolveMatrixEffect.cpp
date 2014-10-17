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
#include "ConvolveMatrixEffect.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects
{
    ConvolveMatrixEffect::ConvolveMatrixEffect()
        : CanvasEffect(CLSID_D2D1ConvolveMatrix, 11, 1, true)
    {
        // Set default values
        SetProperty<float[2]>(D2D1_CONVOLVEMATRIX_PROP_KERNEL_UNIT_LENGTH, Numerics::Vector2{ 1.0f, 1.0f });
        SetProperty<uint32_t>(D2D1_CONVOLVEMATRIX_PROP_SCALE_MODE, D2D1_CONVOLVEMATRIX_SCALE_MODE_LINEAR);
        SetProperty<uint32_t>(D2D1_CONVOLVEMATRIX_PROP_KERNEL_SIZE_X, 3u);
        SetProperty<uint32_t>(D2D1_CONVOLVEMATRIX_PROP_KERNEL_SIZE_Y, 3u);
        SetArrayProperty<float>(D2D1_CONVOLVEMATRIX_PROP_KERNEL_MATRIX, { 0, 0, 0, 0, 1, 0, 0, 0, 0 });
        SetProperty<float>(D2D1_CONVOLVEMATRIX_PROP_DIVISOR, 1.0f);
        SetProperty<float>(D2D1_CONVOLVEMATRIX_PROP_BIAS, 0.0f);
        SetProperty<float[2]>(D2D1_CONVOLVEMATRIX_PROP_KERNEL_OFFSET, Numerics::Vector2{ 0.0f, 0.0f });
        SetProperty<boolean>(D2D1_CONVOLVEMATRIX_PROP_PRESERVE_ALPHA, static_cast<boolean>(false));
        SetProperty<uint32_t>(D2D1_CONVOLVEMATRIX_PROP_BORDER_MODE, D2D1_BORDER_MODE_SOFT);
        SetProperty<boolean>(D2D1_CONVOLVEMATRIX_PROP_CLAMP_OUTPUT, static_cast<boolean>(false));
    }

    IMPLEMENT_PROPERTY_WITH_VALIDATION(ConvolveMatrixEffect,
        KernelScale,
        float[2],
        Numerics::Vector2,
        D2D1_CONVOLVEMATRIX_PROP_KERNEL_UNIT_LENGTH,
        (value.X >= 0.01f) && (value.Y >= 0.01f) && (value.X <= 100.0f) && (value.Y <= 100.0f))

    IMPLEMENT_PROPERTY(ConvolveMatrixEffect,
        InterpolationMode,
        uint32_t,
        CanvasImageInterpolation,
        D2D1_CONVOLVEMATRIX_PROP_SCALE_MODE)

    IMPLEMENT_PROPERTY_WITH_VALIDATION(ConvolveMatrixEffect,
        KernelWidth,
        uint32_t,
        uint32_t,
        D2D1_CONVOLVEMATRIX_PROP_KERNEL_SIZE_X,
        (value >= 1u) && (value <= 100u))

    IMPLEMENT_PROPERTY_WITH_VALIDATION(ConvolveMatrixEffect,
        KernelHeight,
        uint32_t,
        uint32_t,
        D2D1_CONVOLVEMATRIX_PROP_KERNEL_SIZE_Y,
        (value >= 1u) && (value <= 100u))

    IMPLEMENT_ARRAY_PROPERTY(ConvolveMatrixEffect,
        KernelMatrix,
        float,
        D2D1_CONVOLVEMATRIX_PROP_KERNEL_MATRIX)

    IMPLEMENT_PROPERTY(ConvolveMatrixEffect,
        Divisor,
        float,
        float,
        D2D1_CONVOLVEMATRIX_PROP_DIVISOR)

    IMPLEMENT_PROPERTY(ConvolveMatrixEffect,
        Offset,
        float,
        float,
        D2D1_CONVOLVEMATRIX_PROP_BIAS)

    IMPLEMENT_PROPERTY_WITH_VALIDATION(ConvolveMatrixEffect,
        KernelOffset,
        float[2],
        Numerics::Vector2,
        D2D1_CONVOLVEMATRIX_PROP_KERNEL_OFFSET,
        (value.X >= -50.0f) && (value.Y >= -50.0f) && (value.X <= 50.0f) && (value.Y <= 50.0f))

    IMPLEMENT_PROPERTY(ConvolveMatrixEffect,
        PreserveAlpha,
        boolean,
        boolean,
        D2D1_CONVOLVEMATRIX_PROP_PRESERVE_ALPHA)

    IMPLEMENT_PROPERTY(ConvolveMatrixEffect,
        BorderMode,
        uint32_t,
        EffectBorderMode,
        D2D1_CONVOLVEMATRIX_PROP_BORDER_MODE)

    IMPLEMENT_PROPERTY(ConvolveMatrixEffect,
        ClampOutput,
        boolean,
        boolean,
        D2D1_CONVOLVEMATRIX_PROP_CLAMP_OUTPUT)

    IMPLEMENT_INPUT_PROPERTY(ConvolveMatrixEffect,
        Source,
        0)

    ActivatableClass(ConvolveMatrixEffect);
}}}}}
