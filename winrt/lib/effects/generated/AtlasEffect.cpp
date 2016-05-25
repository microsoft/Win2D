// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

// This file was automatically generated. Please do not edit it manually.

#include "pch.h"
#include "AtlasEffect.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects
{
    AtlasEffect::AtlasEffect(ICanvasDevice* device, ID2D1Effect* effect)
        : CanvasEffect(EffectId(), 2, 1, true, device, effect, static_cast<IAtlasEffect*>(this))
    {
        if (!effect)
        {
            // Set default values
            SetBoxedProperty<float[4]>(D2D1_ATLAS_PROP_INPUT_RECT, Rect{ 0, 0, std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity() });
            SetBoxedProperty<float[4]>(D2D1_ATLAS_PROP_INPUT_PADDING_RECT, Rect{ 0, 0, std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity() });
        }
    }

    IMPLEMENT_EFFECT_PROPERTY(AtlasEffect,
        SourceRectangle,
        float[4],
        Rect,
        D2D1_ATLAS_PROP_INPUT_RECT)

    IMPLEMENT_EFFECT_PROPERTY(AtlasEffect,
        PaddingRectangle,
        float[4],
        Rect,
        D2D1_ATLAS_PROP_INPUT_PADDING_RECT)

    IMPLEMENT_EFFECT_SOURCE_PROPERTY(AtlasEffect,
        Source,
        0)

    IMPLEMENT_EFFECT_PROPERTY_MAPPING(AtlasEffect,
        { L"SourceRectangle",  D2D1_ATLAS_PROP_INPUT_RECT,         GRAPHICS_EFFECT_PROPERTY_MAPPING_RECT_TO_VECTOR4 },
        { L"PaddingRectangle", D2D1_ATLAS_PROP_INPUT_PADDING_RECT, GRAPHICS_EFFECT_PROPERTY_MAPPING_RECT_TO_VECTOR4 })

    ActivatableClassWithFactory(AtlasEffect, SimpleAgileActivationFactory<AtlasEffect>);
}}}}}
