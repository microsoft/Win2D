// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

// This file was automatically generated. Please do not edit it manually.

#include "pch.h"
#include "WhiteLevelAdjustmentEffect.h"

#if (defined _WIN32_WINNT_WIN10) && (WINVER >= _WIN32_WINNT_WIN10)

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects
{
    WhiteLevelAdjustmentEffect::WhiteLevelAdjustmentEffect(ICanvasDevice* device, ID2D1Effect* effect)
        : CanvasEffect(EffectId(), 2, 1, true, device, effect, static_cast<IWhiteLevelAdjustmentEffect*>(this))
    {
        if (!SharedDeviceState::GetInstance()->IsEffectRegistered(WhiteLevelAdjustmentEffect::EffectId(), true))
            ThrowHR(E_NOTIMPL, Strings::NotSupportedOnThisVersionOfWindows);

        if (!effect)
        {
            // Set default values
            SetBoxedProperty<float>(D2D1_WHITELEVELADJUSTMENT_PROP_INPUT_WHITE_LEVEL, 80.0f);
            SetBoxedProperty<float>(D2D1_WHITELEVELADJUSTMENT_PROP_OUTPUT_WHITE_LEVEL, 80.0f);
        }
    }

    IMPLEMENT_EFFECT_PROPERTY(WhiteLevelAdjustmentEffect,
        InputWhiteLevel,
        float,
        float,
        D2D1_WHITELEVELADJUSTMENT_PROP_INPUT_WHITE_LEVEL)

    IMPLEMENT_EFFECT_PROPERTY(WhiteLevelAdjustmentEffect,
        OutputWhiteLevel,
        float,
        float,
        D2D1_WHITELEVELADJUSTMENT_PROP_OUTPUT_WHITE_LEVEL)

    IMPLEMENT_EFFECT_SOURCE_PROPERTY(WhiteLevelAdjustmentEffect,
        Source,
        0)

    IMPLEMENT_EFFECT_PROPERTY_MAPPING(WhiteLevelAdjustmentEffect,
        { L"InputWhiteLevel",  D2D1_WHITELEVELADJUSTMENT_PROP_INPUT_WHITE_LEVEL,  GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
        { L"OutputWhiteLevel", D2D1_WHITELEVELADJUSTMENT_PROP_OUTPUT_WHITE_LEVEL, GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT })

    IFACEMETHODIMP WhiteLevelAdjustmentEffectFactory::ActivateInstance(IInspectable** instance)
    {
        return ExceptionBoundary([&]
        {
            auto effect = Make<WhiteLevelAdjustmentEffect>();
            CheckMakeResult(effect);

            ThrowIfFailed(effect.CopyTo(instance));
        });
    }

    IFACEMETHODIMP WhiteLevelAdjustmentEffectFactory::get_IsSupported(_Out_ boolean* result)
    {
        return ExceptionBoundary([&]
        {
            CheckInPointer(result);
            *result = SharedDeviceState::GetInstance()->IsEffectRegistered(WhiteLevelAdjustmentEffect::EffectId(), true);
        });
    }

    ActivatableClassWithFactory(WhiteLevelAdjustmentEffect, WhiteLevelAdjustmentEffectFactory);
}}}}}

#endif // _WIN32_WINNT_WIN10
