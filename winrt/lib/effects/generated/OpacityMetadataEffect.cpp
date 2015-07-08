// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

// This file was automatically generated. Please do not edit it manually.

#include "pch.h"
#include "OpacityMetadataEffect.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects
{
    OpacityMetadataEffect::OpacityMetadataEffect()
        : CanvasEffect(CLSID_D2D1OpacityMetadata, 1, 1, true)
    {
        // Set default values
        SetBoxedProperty<float[4]>(D2D1_OPACITYMETADATA_PROP_INPUT_OPAQUE_RECT, Rect{ -std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity() });
    }

    IMPLEMENT_EFFECT_PROPERTY(OpacityMetadataEffect,
        OpaqueRectangle,
        float[4],
        Rect,
        D2D1_OPACITYMETADATA_PROP_INPUT_OPAQUE_RECT)

    IMPLEMENT_EFFECT_SOURCE_PROPERTY(OpacityMetadataEffect,
        Source,
        0)

    IMPLEMENT_EFFECT_PROPERTY_MAPPING(OpacityMetadataEffect,
        { L"OpaqueRectangle", D2D1_OPACITYMETADATA_PROP_INPUT_OPAQUE_RECT, GRAPHICS_EFFECT_PROPERTY_MAPPING_RECT_TO_VECTOR4 })

    ActivatableClass(OpacityMetadataEffect);
}}}}}
