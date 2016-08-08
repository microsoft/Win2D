// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

// This file was automatically generated. Please do not edit it manually.

#include "pch.h"
#include "AlphaMaskEffect.h"

#if (defined _WIN32_WINNT_WIN10) && (WINVER >= _WIN32_WINNT_WIN10)

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects
{
    AlphaMaskEffect::AlphaMaskEffect(ICanvasDevice* device, ID2D1Effect* effect)
        : CanvasEffect(EffectId(), 0, 2, true, device, effect, static_cast<IAlphaMaskEffect*>(this))
    {
        if (!SharedDeviceState::GetInstance()->IsID2D1Factory5Supported())
            ThrowHR(E_NOTIMPL, Strings::NotSupportedOnThisVersionOfWindows);

        if (!effect)
        {
            // Set default values
        }
    }

    IMPLEMENT_EFFECT_SOURCE_PROPERTY(AlphaMaskEffect,
        Source,
        0)

    IMPLEMENT_EFFECT_SOURCE_PROPERTY(AlphaMaskEffect,
        AlphaMask,
        1)

    IFACEMETHODIMP AlphaMaskEffectFactory::ActivateInstance(IInspectable** instance)
    {
        return ExceptionBoundary([&]
        {
            auto effect = Make<AlphaMaskEffect>();
            CheckMakeResult(effect);

            ThrowIfFailed(effect.CopyTo(instance));
        });
    }

    IFACEMETHODIMP AlphaMaskEffectFactory::get_IsSupported(_Out_ boolean* result)
    {
        return ExceptionBoundary([&]
        {
            CheckInPointer(result);
            *result = SharedDeviceState::GetInstance()->IsID2D1Factory5Supported();
        });
    }

    ActivatableClassWithFactory(AlphaMaskEffect, AlphaMaskEffectFactory);
}}}}}

#endif // _WIN32_WINNT_WIN10
