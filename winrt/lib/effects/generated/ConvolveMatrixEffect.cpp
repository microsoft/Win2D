// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

// This file was automatically generated. Please do not edit it manually.

#include "pch.h"
#include "ConvolveMatrixEffect.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects
{
    ConvolveMatrixEffect::ConvolveMatrixEffect(ICanvasDevice* device, ID2D1Effect* effect)
        : CanvasEffect(EffectId(), 11, 1, true, device, effect, static_cast<IConvolveMatrixEffect*>(this))
    {
        if (!effect)
        {
            // Set default values
            SetBoxedProperty<float[2]>(D2D1_CONVOLVEMATRIX_PROP_KERNEL_UNIT_LENGTH, Numerics::Vector2{ 1.0f, 1.0f });
            SetBoxedProperty<uint32_t>(D2D1_CONVOLVEMATRIX_PROP_SCALE_MODE, D2D1_CONVOLVEMATRIX_SCALE_MODE_LINEAR);
            SetBoxedProperty<int32_t>(D2D1_CONVOLVEMATRIX_PROP_KERNEL_SIZE_X, 3);
            SetBoxedProperty<int32_t>(D2D1_CONVOLVEMATRIX_PROP_KERNEL_SIZE_Y, 3);
            SetArrayProperty<float>(D2D1_CONVOLVEMATRIX_PROP_KERNEL_MATRIX, { 0, 0, 0, 0, 1, 0, 0, 0, 0 });
            SetBoxedProperty<float>(D2D1_CONVOLVEMATRIX_PROP_DIVISOR, 1.0f);
            SetBoxedProperty<float>(D2D1_CONVOLVEMATRIX_PROP_BIAS, 0.0f);
            SetBoxedProperty<float[2]>(D2D1_CONVOLVEMATRIX_PROP_KERNEL_OFFSET, Numerics::Vector2{ 0.0f, 0.0f });
            SetBoxedProperty<boolean>(D2D1_CONVOLVEMATRIX_PROP_PRESERVE_ALPHA, static_cast<boolean>(false));
            SetBoxedProperty<uint32_t>(D2D1_CONVOLVEMATRIX_PROP_BORDER_MODE, D2D1_BORDER_MODE_SOFT);
            SetBoxedProperty<boolean>(D2D1_CONVOLVEMATRIX_PROP_CLAMP_OUTPUT, static_cast<boolean>(false));
        }
    }

    IMPLEMENT_EFFECT_PROPERTY_WITH_VALIDATION(ConvolveMatrixEffect,
        KernelScale,
        float[2],
        Numerics::Vector2,
        D2D1_CONVOLVEMATRIX_PROP_KERNEL_UNIT_LENGTH,
        (value.X >= 0.01f) && (value.Y >= 0.01f) && (value.X <= 100.0f) && (value.Y <= 100.0f))

    IMPLEMENT_EFFECT_PROPERTY(ConvolveMatrixEffect,
        InterpolationMode,
        uint32_t,
        CanvasImageInterpolation,
        D2D1_CONVOLVEMATRIX_PROP_SCALE_MODE)

    IMPLEMENT_EFFECT_PROPERTY_WITH_VALIDATION(ConvolveMatrixEffect,
        KernelWidth,
        int32_t,
        int32_t,
        D2D1_CONVOLVEMATRIX_PROP_KERNEL_SIZE_X,
        (value >= 1) && (value <= 100))

    IMPLEMENT_EFFECT_PROPERTY_WITH_VALIDATION(ConvolveMatrixEffect,
        KernelHeight,
        int32_t,
        int32_t,
        D2D1_CONVOLVEMATRIX_PROP_KERNEL_SIZE_Y,
        (value >= 1) && (value <= 100))

    IMPLEMENT_EFFECT_ARRAY_PROPERTY(ConvolveMatrixEffect,
        KernelMatrix,
        float,
        D2D1_CONVOLVEMATRIX_PROP_KERNEL_MATRIX)

    IMPLEMENT_EFFECT_PROPERTY(ConvolveMatrixEffect,
        Divisor,
        float,
        float,
        D2D1_CONVOLVEMATRIX_PROP_DIVISOR)

    IMPLEMENT_EFFECT_PROPERTY(ConvolveMatrixEffect,
        Offset,
        float,
        float,
        D2D1_CONVOLVEMATRIX_PROP_BIAS)

    IMPLEMENT_EFFECT_PROPERTY_WITH_VALIDATION(ConvolveMatrixEffect,
        KernelOffset,
        float[2],
        Numerics::Vector2,
        D2D1_CONVOLVEMATRIX_PROP_KERNEL_OFFSET,
        (value.X >= -50.0f) && (value.Y >= -50.0f) && (value.X <= 50.0f) && (value.Y <= 50.0f))

    IMPLEMENT_EFFECT_PROPERTY(ConvolveMatrixEffect,
        PreserveAlpha,
        boolean,
        boolean,
        D2D1_CONVOLVEMATRIX_PROP_PRESERVE_ALPHA)

    IMPLEMENT_EFFECT_PROPERTY(ConvolveMatrixEffect,
        BorderMode,
        uint32_t,
        EffectBorderMode,
        D2D1_CONVOLVEMATRIX_PROP_BORDER_MODE)

    IMPLEMENT_EFFECT_PROPERTY(ConvolveMatrixEffect,
        ClampOutput,
        boolean,
        boolean,
        D2D1_CONVOLVEMATRIX_PROP_CLAMP_OUTPUT)

    IMPLEMENT_EFFECT_SOURCE_PROPERTY(ConvolveMatrixEffect,
        Source,
        0)

    IMPLEMENT_EFFECT_PROPERTY_MAPPING(ConvolveMatrixEffect,
        { L"KernelScale",       D2D1_CONVOLVEMATRIX_PROP_KERNEL_UNIT_LENGTH, GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
        { L"InterpolationMode", D2D1_CONVOLVEMATRIX_PROP_SCALE_MODE,         GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
        { L"KernelWidth",       D2D1_CONVOLVEMATRIX_PROP_KERNEL_SIZE_X,      GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
        { L"KernelHeight",      D2D1_CONVOLVEMATRIX_PROP_KERNEL_SIZE_Y,      GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
        { L"KernelMatrix",      D2D1_CONVOLVEMATRIX_PROP_KERNEL_MATRIX,      GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
        { L"Divisor",           D2D1_CONVOLVEMATRIX_PROP_DIVISOR,            GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
        { L"Offset",            D2D1_CONVOLVEMATRIX_PROP_BIAS,               GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
        { L"KernelOffset",      D2D1_CONVOLVEMATRIX_PROP_KERNEL_OFFSET,      GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
        { L"PreserveAlpha",     D2D1_CONVOLVEMATRIX_PROP_PRESERVE_ALPHA,     GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
        { L"BorderMode",        D2D1_CONVOLVEMATRIX_PROP_BORDER_MODE,        GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
        { L"ClampOutput",       D2D1_CONVOLVEMATRIX_PROP_CLAMP_OUTPUT,       GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT })

    ActivatableClassWithFactory(ConvolveMatrixEffect, ::SimpleAgileActivationFactory<ConvolveMatrixEffect>);
}}}}}
