// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

// This file was automatically generated. Please do not edit it manually.

#include "pch.h"
#include "OpacityEffect.h"

#if (defined _WIN32_WINNT_WIN10) && (WINVER >= _WIN32_WINNT_WIN10)

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects
{
    OpacityEffect::OpacityEffect(ICanvasDevice* device, ID2D1Effect* effect)
        : CanvasEffect(EffectId(), 1, 1, true, device, effect, static_cast<IOpacityEffect*>(this))
    {
        if (!SharedDeviceState::GetInstance()->IsID2D1Factory5Supported())
            ThrowHR(E_NOTIMPL, Strings::NotSupportedOnThisVersionOfWindows);

        if (!effect)
        {
            // Set default values
            SetBoxedProperty<float>(D2D1_OPACITY_PROP_OPACITY, 1.0f);
        }
    }

    IMPLEMENT_EFFECT_PROPERTY_WITH_VALIDATION(OpacityEffect,
        Opacity,
        float,
        float,
        D2D1_OPACITY_PROP_OPACITY,
        (value >= 0.0f) && (value <= 1.0f))

    IMPLEMENT_EFFECT_SOURCE_PROPERTY(OpacityEffect,
        Source,
        0)

    IMPLEMENT_EFFECT_PROPERTY_MAPPING(OpacityEffect,
        { L"Opacity", D2D1_OPACITY_PROP_OPACITY, GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT })

    IFACEMETHODIMP OpacityEffectFactory::ActivateInstance(IInspectable** instance)
    {
        return ExceptionBoundary([&]
        {
            auto effect = Make<OpacityEffect>();
            CheckMakeResult(effect);

            ThrowIfFailed(effect.CopyTo(instance));
        });
    }

    IFACEMETHODIMP OpacityEffectFactory::get_IsSupported(_Out_ boolean* result)
    {
        return ExceptionBoundary([&]
        {
            CheckInPointer(result);
            *result = SharedDeviceState::GetInstance()->IsID2D1Factory5Supported();
        });
    }

    ActivatableClassWithFactory(OpacityEffect, OpacityEffectFactory);
}}}}}

#endif // _WIN32_WINNT_WIN10
