// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

// This file was automatically generated. Please do not edit it manually.

#include "pch.h"
#include "DpiCompensationEffect.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects
{
    DpiCompensationEffect::DpiCompensationEffect(ICanvasDevice* device, ID2D1Effect* effect)
        : CanvasEffect(EffectId(), 3, 1, true, device, effect, static_cast<IDpiCompensationEffect*>(this))
    {
        if (!effect)
        {
            // Set default values
            SetBoxedProperty<uint32_t>(D2D1_DPICOMPENSATION_PROP_INTERPOLATION_MODE, D2D1_DPICOMPENSATION_INTERPOLATION_MODE_LINEAR);
            SetBoxedProperty<uint32_t>(D2D1_DPICOMPENSATION_PROP_BORDER_MODE, D2D1_BORDER_MODE_HARD);
            SetBoxedProperty<float[2]>(D2D1_DPICOMPENSATION_PROP_INPUT_DPI, Numerics::Vector2{ 96, 96 });
        }
    }

    IMPLEMENT_EFFECT_PROPERTY(DpiCompensationEffect,
        InterpolationMode,
        uint32_t,
        CanvasImageInterpolation,
        D2D1_DPICOMPENSATION_PROP_INTERPOLATION_MODE)

    IMPLEMENT_EFFECT_PROPERTY(DpiCompensationEffect,
        BorderMode,
        uint32_t,
        EffectBorderMode,
        D2D1_DPICOMPENSATION_PROP_BORDER_MODE)

    IMPLEMENT_EFFECT_PROPERTY(DpiCompensationEffect,
        SourceDpi,
        float[2],
        Numerics::Vector2,
        D2D1_DPICOMPENSATION_PROP_INPUT_DPI)

    IMPLEMENT_EFFECT_SOURCE_PROPERTY(DpiCompensationEffect,
        Source,
        0)

    IMPLEMENT_EFFECT_PROPERTY_MAPPING(DpiCompensationEffect,
        { L"InterpolationMode", D2D1_DPICOMPENSATION_PROP_INTERPOLATION_MODE, GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
        { L"BorderMode",        D2D1_DPICOMPENSATION_PROP_BORDER_MODE,        GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
        { L"SourceDpi",         D2D1_DPICOMPENSATION_PROP_INPUT_DPI,          GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT })

    ActivatableClassWithFactory(DpiCompensationEffect, ::SimpleAgileActivationFactory<DpiCompensationEffect>);
}}}}}
