// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

// This file was automatically generated. Please do not edit it manually.

#include "pch.h"
#include "CrossFadeEffect.h"

#if (defined _WIN32_WINNT_WIN10) && (WINVER >= _WIN32_WINNT_WIN10)

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects
{
    CrossFadeEffect::CrossFadeEffect(ICanvasDevice* device, ID2D1Effect* effect)
        : CanvasEffect(EffectId(), 1, 2, true, device, effect, static_cast<ICrossFadeEffect*>(this))
    {
        if (!SharedDeviceState::GetInstance()->IsID2D1Factory5Supported())
            ThrowHR(E_NOTIMPL, Strings::NotSupportedOnThisVersionOfWindows);

        if (!effect)
        {
            // Set default values
            SetBoxedProperty<float>(D2D1_CROSSFADE_PROP_WEIGHT, 0.5f);
        }
    }

    IMPLEMENT_EFFECT_PROPERTY_WITH_VALIDATION(CrossFadeEffect,
        CrossFade,
        float,
        float,
        D2D1_CROSSFADE_PROP_WEIGHT,
        (value >= 0.0f) && (value <= 1.0f))

    IMPLEMENT_EFFECT_SOURCE_PROPERTY(CrossFadeEffect,
        Source2,
        0)

    IMPLEMENT_EFFECT_SOURCE_PROPERTY(CrossFadeEffect,
        Source1,
        1)

    IMPLEMENT_EFFECT_PROPERTY_MAPPING(CrossFadeEffect,
        { L"CrossFade", D2D1_CROSSFADE_PROP_WEIGHT, GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT })

    IFACEMETHODIMP CrossFadeEffectFactory::ActivateInstance(IInspectable** instance)
    {
        return ExceptionBoundary([&]
        {
            auto effect = Make<CrossFadeEffect>();
            CheckMakeResult(effect);

            ThrowIfFailed(effect.CopyTo(instance));
        });
    }

    IFACEMETHODIMP CrossFadeEffectFactory::get_IsSupported(_Out_ boolean* result)
    {
        return ExceptionBoundary([&]
        {
            CheckInPointer(result);
            *result = SharedDeviceState::GetInstance()->IsID2D1Factory5Supported();
        });
    }

    ActivatableClassWithFactory(CrossFadeEffect, CrossFadeEffectFactory);
}}}}}

#endif // _WIN32_WINNT_WIN10
