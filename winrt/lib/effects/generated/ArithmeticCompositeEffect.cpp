// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

// This file was automatically generated. Please do not edit it manually.

#include "pch.h"
#include "ArithmeticCompositeEffect.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects
{
    ArithmeticCompositeEffect::ArithmeticCompositeEffect(ICanvasDevice* device, ID2D1Effect* effect)
        : CanvasEffect(EffectId(), 2, 2, true, device, effect, static_cast<IArithmeticCompositeEffect*>(this))
    {
        if (!effect)
        {
            // Set default values
            SetBoxedProperty<float[4]>(D2D1_ARITHMETICCOMPOSITE_PROP_COEFFICIENTS, Numerics::Vector4{ 1.0f, 0.0f, 0.0f, 0.0f });
            SetBoxedProperty<boolean>(D2D1_ARITHMETICCOMPOSITE_PROP_CLAMP_OUTPUT, static_cast<boolean>(false));
        }
    }

    IMPLEMENT_EFFECT_PROPERTY(ArithmeticCompositeEffect,
        ClampOutput,
        boolean,
        boolean,
        D2D1_ARITHMETICCOMPOSITE_PROP_CLAMP_OUTPUT)

    IMPLEMENT_EFFECT_SOURCE_PROPERTY(ArithmeticCompositeEffect,
        Source1,
        0)

    IMPLEMENT_EFFECT_SOURCE_PROPERTY(ArithmeticCompositeEffect,
        Source2,
        1)

    IMPLEMENT_EFFECT_PROPERTY_MAPPING(ArithmeticCompositeEffect,
        { L"ClampOutput", D2D1_ARITHMETICCOMPOSITE_PROP_CLAMP_OUTPUT, GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT })

    ActivatableClassWithFactory(ArithmeticCompositeEffect, ::SimpleAgileActivationFactory<ArithmeticCompositeEffect>);
}}}}}
