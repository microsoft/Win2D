// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

// This file was automatically generated. Please do not edit it manually.

#include "pch.h"
#include "HdrToneMapEffect.h"

#if (defined _WIN32_WINNT_WIN10) && (WINVER >= _WIN32_WINNT_WIN10)

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects
{
    HdrToneMapEffect::HdrToneMapEffect(ICanvasDevice* device, ID2D1Effect* effect)
        : CanvasEffect(EffectId(), 3, 1, true, device, effect, static_cast<IHdrToneMapEffect*>(this))
    {
        if (!SharedDeviceState::GetInstance()->IsEffectRegistered(HdrToneMapEffect::EffectId(), true))
            ThrowHR(E_NOTIMPL, Strings::NotSupportedOnThisVersionOfWindows);

        if (!effect)
        {
            // Set default values
            SetBoxedProperty<float>(D2D1_HDRTONEMAP_PROP_INPUT_MAX_LUMINANCE, 4000.0f);
            SetBoxedProperty<float>(D2D1_HDRTONEMAP_PROP_OUTPUT_MAX_LUMINANCE, 300.0f);
            SetBoxedProperty<uint32_t>(D2D1_HDRTONEMAP_PROP_DISPLAY_MODE, HdrToneMapEffectDisplayMode::Sdr);
        }
    }

    IMPLEMENT_EFFECT_PROPERTY(HdrToneMapEffect,
        InputMaxLuminance,
        float,
        float,
        D2D1_HDRTONEMAP_PROP_INPUT_MAX_LUMINANCE)

    IMPLEMENT_EFFECT_PROPERTY(HdrToneMapEffect,
        OutputMaxLuminance,
        float,
        float,
        D2D1_HDRTONEMAP_PROP_OUTPUT_MAX_LUMINANCE)

    IMPLEMENT_EFFECT_PROPERTY(HdrToneMapEffect,
        DisplayMode,
        uint32_t,
        HdrToneMapEffectDisplayMode,
        D2D1_HDRTONEMAP_PROP_DISPLAY_MODE)

    IMPLEMENT_EFFECT_SOURCE_PROPERTY(HdrToneMapEffect,
        Source,
        0)

    IMPLEMENT_EFFECT_PROPERTY_MAPPING(HdrToneMapEffect,
        { L"InputMaxLuminance",  D2D1_HDRTONEMAP_PROP_INPUT_MAX_LUMINANCE,  GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
        { L"OutputMaxLuminance", D2D1_HDRTONEMAP_PROP_OUTPUT_MAX_LUMINANCE, GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
        { L"DisplayMode",        D2D1_HDRTONEMAP_PROP_DISPLAY_MODE,         GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT })

    IFACEMETHODIMP HdrToneMapEffectFactory::ActivateInstance(IInspectable** instance)
    {
        return ExceptionBoundary([&]
        {
            auto effect = Make<HdrToneMapEffect>();
            CheckMakeResult(effect);

            ThrowIfFailed(effect.CopyTo(instance));
        });
    }

    IFACEMETHODIMP HdrToneMapEffectFactory::get_IsSupported(_Out_ boolean* result)
    {
        return ExceptionBoundary([&]
        {
            CheckInPointer(result);
            *result = SharedDeviceState::GetInstance()->IsEffectRegistered(HdrToneMapEffect::EffectId(), true);
        });
    }

    ActivatableClassWithFactory(HdrToneMapEffect, HdrToneMapEffectFactory);
}}}}}

#endif // _WIN32_WINNT_WIN10
