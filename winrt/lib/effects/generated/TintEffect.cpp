// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

// This file was automatically generated. Please do not edit it manually.

#include "pch.h"
#include "TintEffect.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects
{
    TintEffect::TintEffect(ICanvasDevice* device, ID2D1Effect* effect)
        : CanvasEffect(EffectId(), 2, 1, true, device, effect, static_cast<ITintEffect*>(this))
    {
        if (!SharedDeviceState::GetInstance()->IsID2D1Factory5Supported())
            ThrowHR(E_NOTIMPL, Strings::NotSupportedOnThisVersionOfWindows);

        if (!effect)
        {
            // Set default values
            SetBoxedProperty<float[4]>(D2D1_TINT_PROP_COLOR, Color{ 255, 255, 255, 255 });
            SetBoxedProperty<boolean>(D2D1_TINT_PROP_CLAMP_OUTPUT, static_cast<boolean>(false));
        }
    }

    IMPLEMENT_EFFECT_PROPERTY(TintEffect,
        Color,
        float[4],
        Color,
        D2D1_TINT_PROP_COLOR)

    IMPLEMENT_EFFECT_PROPERTY(TintEffect,
        ClampOutput,
        boolean,
        boolean,
        D2D1_TINT_PROP_CLAMP_OUTPUT)

    IMPLEMENT_EFFECT_PROPERTY(TintEffect,
        ColorHdr,
        float[4],
        Numerics::Vector4,
        D2D1_TINT_PROP_COLOR)

    IMPLEMENT_EFFECT_SOURCE_PROPERTY(TintEffect,
        Source,
        0)

    IMPLEMENT_EFFECT_PROPERTY_MAPPING(TintEffect,
        { L"Color",       D2D1_TINT_PROP_COLOR,        GRAPHICS_EFFECT_PROPERTY_MAPPING_COLOR_TO_VECTOR4 },
        { L"ClampOutput", D2D1_TINT_PROP_CLAMP_OUTPUT, GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT           },
        { L"ColorHdr",    D2D1_TINT_PROP_COLOR,        GRAPHICS_EFFECT_PROPERTY_MAPPING_UNKNOWN          })

    IFACEMETHODIMP TintEffectFactory::ActivateInstance(IInspectable** instance)
    {
        return ExceptionBoundary([&]
        {
            auto effect = Make<TintEffect>();
            CheckMakeResult(effect);

            ThrowIfFailed(effect.CopyTo(instance));
        });
    }

    IFACEMETHODIMP TintEffectFactory::get_IsSupported(_Out_ boolean* result)
    {
        return ExceptionBoundary([&]
        {
            CheckInPointer(result);
            *result = SharedDeviceState::GetInstance()->IsID2D1Factory5Supported();
        });
    }

    ActivatableClassWithFactory(TintEffect, TintEffectFactory);
}}}}}
