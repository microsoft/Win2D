// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

// This file was automatically generated. Please do not edit it manually.

#include "pch.h"
#include "EdgeDetectionEffect.h"

#if (defined _WIN32_WINNT_WIN10) && (WINVER >= _WIN32_WINNT_WIN10)

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects
{
    EdgeDetectionEffect::EdgeDetectionEffect(ICanvasDevice* device, ID2D1Effect* effect)
        : CanvasEffect(EffectId(), 5, 1, true, device, effect, static_cast<IEdgeDetectionEffect*>(this))
    {
        if (!effect)
        {
            // Set default values
            SetBoxedProperty<float>(D2D1_EDGEDETECTION_PROP_STRENGTH, 0.5f);
            SetBoxedProperty<float>(D2D1_EDGEDETECTION_PROP_BLUR_RADIUS, 0.0f);
            SetBoxedProperty<uint32_t>(D2D1_EDGEDETECTION_PROP_MODE, EdgeDetectionEffectMode::Sobel);
            SetBoxedProperty<boolean>(D2D1_EDGEDETECTION_PROP_OVERLAY_EDGES, static_cast<boolean>(false));
            SetBoxedProperty<uint32_t>(D2D1_EDGEDETECTION_PROP_ALPHA_MODE, D2D1_COLORMANAGEMENT_ALPHA_MODE_PREMULTIPLIED);
        }
    }

    IMPLEMENT_EFFECT_PROPERTY_WITH_VALIDATION(EdgeDetectionEffect,
        Amount,
        float,
        float,
        D2D1_EDGEDETECTION_PROP_STRENGTH,
        (value >= 0.0f) && (value <= 1.0f))

    IMPLEMENT_EFFECT_PROPERTY_WITH_VALIDATION(EdgeDetectionEffect,
        BlurAmount,
        float,
        float,
        D2D1_EDGEDETECTION_PROP_BLUR_RADIUS,
        (value >= 0.0f) && (value <= 10.0f))

    IMPLEMENT_EFFECT_PROPERTY(EdgeDetectionEffect,
        Mode,
        uint32_t,
        EdgeDetectionEffectMode,
        D2D1_EDGEDETECTION_PROP_MODE)

    IMPLEMENT_EFFECT_PROPERTY(EdgeDetectionEffect,
        OverlayEdges,
        boolean,
        boolean,
        D2D1_EDGEDETECTION_PROP_OVERLAY_EDGES)

    IMPLEMENT_EFFECT_PROPERTY(EdgeDetectionEffect,
        AlphaMode,
        ConvertAlphaMode,
        CanvasAlphaMode,
        D2D1_EDGEDETECTION_PROP_ALPHA_MODE)

    IMPLEMENT_EFFECT_SOURCE_PROPERTY(EdgeDetectionEffect,
        Source,
        0)

    IMPLEMENT_EFFECT_PROPERTY_MAPPING(EdgeDetectionEffect,
        { L"Amount",       D2D1_EDGEDETECTION_PROP_STRENGTH,      GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT                 },
        { L"BlurAmount",   D2D1_EDGEDETECTION_PROP_BLUR_RADIUS,   GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT                 },
        { L"Mode",         D2D1_EDGEDETECTION_PROP_MODE,          GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT                 },
        { L"OverlayEdges", D2D1_EDGEDETECTION_PROP_OVERLAY_EDGES, GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT                 },
        { L"AlphaMode",    D2D1_EDGEDETECTION_PROP_ALPHA_MODE,    GRAPHICS_EFFECT_PROPERTY_MAPPING_COLORMATRIX_ALPHA_MODE })

    ActivatableClass(EdgeDetectionEffect);
}}}}}

#endif // _WIN32_WINNT_WIN10
