// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

// This file was automatically generated. Please do not edit it manually.

#include "pch.h"
#include "ColorManagementEffect.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects
{
    ColorManagementEffect::ColorManagementEffect(ICanvasDevice* device, ID2D1Effect* effect)
        : CanvasEffect(EffectId(), 6, 1, true, device, effect, static_cast<IColorManagementEffect*>(this))
    {
        if (!effect)
        {
            // Set default values
            SetBoxedProperty<IColorManagementProfile*>(D2D1_COLORMANAGEMENT_PROP_SOURCE_COLOR_CONTEXT, static_cast<IColorManagementProfile*>(nullptr));
            SetBoxedProperty<uint32_t>(D2D1_COLORMANAGEMENT_PROP_SOURCE_RENDERING_INTENT, D2D1_COLORMANAGEMENT_RENDERING_INTENT_PERCEPTUAL);
            SetBoxedProperty<IColorManagementProfile*>(D2D1_COLORMANAGEMENT_PROP_DESTINATION_COLOR_CONTEXT, static_cast<IColorManagementProfile*>(nullptr));
            SetBoxedProperty<uint32_t>(D2D1_COLORMANAGEMENT_PROP_DESTINATION_RENDERING_INTENT, D2D1_COLORMANAGEMENT_RENDERING_INTENT_PERCEPTUAL);
            SetBoxedProperty<uint32_t>(D2D1_COLORMANAGEMENT_PROP_ALPHA_MODE, D2D1_COLORMANAGEMENT_ALPHA_MODE_PREMULTIPLIED);
            SetBoxedProperty<uint32_t>(D2D1_COLORMANAGEMENT_PROP_QUALITY, D2D1_COLORMANAGEMENT_QUALITY_NORMAL);
        }
    }

    IMPLEMENT_EFFECT_PROPERTY(ColorManagementEffect,
        SourceColorProfile,
        IColorManagementProfile*,
        IColorManagementProfile*,
        D2D1_COLORMANAGEMENT_PROP_SOURCE_COLOR_CONTEXT)

    IMPLEMENT_EFFECT_PROPERTY(ColorManagementEffect,
        SourceRenderingIntent,
        uint32_t,
        ColorManagementRenderingIntent,
        D2D1_COLORMANAGEMENT_PROP_SOURCE_RENDERING_INTENT)

    IMPLEMENT_EFFECT_PROPERTY(ColorManagementEffect,
        OutputColorProfile,
        IColorManagementProfile*,
        IColorManagementProfile*,
        D2D1_COLORMANAGEMENT_PROP_DESTINATION_COLOR_CONTEXT)

    IMPLEMENT_EFFECT_PROPERTY(ColorManagementEffect,
        OutputRenderingIntent,
        uint32_t,
        ColorManagementRenderingIntent,
        D2D1_COLORMANAGEMENT_PROP_DESTINATION_RENDERING_INTENT)

    IMPLEMENT_EFFECT_PROPERTY(ColorManagementEffect,
        AlphaMode,
        ConvertAlphaMode,
        CanvasAlphaMode,
        D2D1_COLORMANAGEMENT_PROP_ALPHA_MODE)

    IMPLEMENT_EFFECT_PROPERTY(ColorManagementEffect,
        Quality,
        uint32_t,
        ColorManagementEffectQuality,
        D2D1_COLORMANAGEMENT_PROP_QUALITY)

    IMPLEMENT_EFFECT_SOURCE_PROPERTY(ColorManagementEffect,
        Source,
        0)

    IMPLEMENT_EFFECT_PROPERTY_MAPPING(ColorManagementEffect,
        { L"SourceColorProfile",    D2D1_COLORMANAGEMENT_PROP_SOURCE_COLOR_CONTEXT,         GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT                 },
        { L"SourceRenderingIntent", D2D1_COLORMANAGEMENT_PROP_SOURCE_RENDERING_INTENT,      GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT                 },
        { L"OutputColorProfile",    D2D1_COLORMANAGEMENT_PROP_DESTINATION_COLOR_CONTEXT,    GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT                 },
        { L"OutputRenderingIntent", D2D1_COLORMANAGEMENT_PROP_DESTINATION_RENDERING_INTENT, GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT                 },
        { L"AlphaMode",             D2D1_COLORMANAGEMENT_PROP_ALPHA_MODE,                   GRAPHICS_EFFECT_PROPERTY_MAPPING_COLORMATRIX_ALPHA_MODE },
        { L"Quality",               D2D1_COLORMANAGEMENT_PROP_QUALITY,                      GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT                 })

    IFACEMETHODIMP ColorManagementEffectFactory::ActivateInstance(IInspectable** instance)
    {
        return ExceptionBoundary([&]
        {
            auto effect = Make<ColorManagementEffect>();
            CheckMakeResult(effect);

            ThrowIfFailed(effect.CopyTo(instance));
        });
    }

    ActivatableClassWithFactory(ColorManagementEffect, ColorManagementEffectFactory);
}}}}}
