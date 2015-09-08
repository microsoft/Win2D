// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

// This file was automatically generated. Please do not edit it manually.

#include "pch.h"
#include "StraightenEffect.h"

#if (defined _WIN32_WINNT_WIN10) && (WINVER >= _WIN32_WINNT_WIN10)

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects
{
    StraightenEffect::StraightenEffect(ICanvasDevice* device, ID2D1Effect* effect)
        : CanvasEffect(EffectId(), 3, 1, true, device, effect, static_cast<IStraightenEffect*>(this))
    {
        if (!effect)
        {
            // Set default values
            SetBoxedProperty<float>(D2D1_STRAIGHTEN_PROP_ANGLE, 0.0f);
            SetBoxedProperty<boolean>(D2D1_STRAIGHTEN_PROP_MAINTAIN_SIZE, static_cast<boolean>(false));
            SetBoxedProperty<uint32_t>(D2D1_STRAIGHTEN_PROP_SCALE_MODE, D2D1_INTERPOLATION_MODE_LINEAR);
        }
    }

    IMPLEMENT_EFFECT_PROPERTY(StraightenEffect,
        Angle,
        ConvertRadiansToDegrees,
        float,
        D2D1_STRAIGHTEN_PROP_ANGLE)

    IMPLEMENT_EFFECT_PROPERTY(StraightenEffect,
        MaintainSize,
        boolean,
        boolean,
        D2D1_STRAIGHTEN_PROP_MAINTAIN_SIZE)

    IMPLEMENT_EFFECT_PROPERTY_WITH_VALIDATION(StraightenEffect,
        InterpolationMode,
        uint32_t,
        CanvasImageInterpolation,
        D2D1_STRAIGHTEN_PROP_SCALE_MODE,
        (value != static_cast<CanvasImageInterpolation>(5)))

    IMPLEMENT_EFFECT_SOURCE_PROPERTY(StraightenEffect,
        Source,
        0)

    IMPLEMENT_EFFECT_PROPERTY_MAPPING(StraightenEffect,
        { L"Angle",             D2D1_STRAIGHTEN_PROP_ANGLE,         GRAPHICS_EFFECT_PROPERTY_MAPPING_RADIANS_TO_DEGREES },
        { L"MaintainSize",      D2D1_STRAIGHTEN_PROP_MAINTAIN_SIZE, GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT             },
        { L"InterpolationMode", D2D1_STRAIGHTEN_PROP_SCALE_MODE,    GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT             })

    ActivatableClass(StraightenEffect);
}}}}}

#endif // _WIN32_WINNT_WIN10
