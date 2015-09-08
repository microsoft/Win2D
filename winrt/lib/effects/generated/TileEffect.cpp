// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

// This file was automatically generated. Please do not edit it manually.

#include "pch.h"
#include "TileEffect.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects
{
    TileEffect::TileEffect(ICanvasDevice* device, ID2D1Effect* effect)
        : CanvasEffect(EffectId(), 1, 1, true, device, effect, static_cast<ITileEffect*>(this))
    {
        if (!effect)
        {
            // Set default values
            SetBoxedProperty<float[4]>(D2D1_TILE_PROP_RECT, Rect{ 0, 0, 100, 100 });
        }
    }

    IMPLEMENT_EFFECT_PROPERTY(TileEffect,
        SourceRectangle,
        float[4],
        Rect,
        D2D1_TILE_PROP_RECT)

    IMPLEMENT_EFFECT_SOURCE_PROPERTY(TileEffect,
        Source,
        0)

    IMPLEMENT_EFFECT_PROPERTY_MAPPING(TileEffect,
        { L"SourceRectangle", D2D1_TILE_PROP_RECT, GRAPHICS_EFFECT_PROPERTY_MAPPING_RECT_TO_VECTOR4 })

    ActivatableClass(TileEffect);
}}}}}
