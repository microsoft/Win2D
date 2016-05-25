// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

// This file was automatically generated. Please do not edit it manually.

#include "pch.h"
#include "ChromaKeyEffect.h"

#if (defined _WIN32_WINNT_WIN10) && (WINVER >= _WIN32_WINNT_WIN10)

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects
{
    ChromaKeyEffect::ChromaKeyEffect(ICanvasDevice* device, ID2D1Effect* effect)
        : CanvasEffect(EffectId(), 4, 1, true, device, effect, static_cast<IChromaKeyEffect*>(this))
    {
        if (!effect)
        {
            // Set default values
            SetBoxedProperty<float[3]>(D2D1_CHROMAKEY_PROP_COLOR, Color{ 255, 0, 0, 0 });
            SetBoxedProperty<float>(D2D1_CHROMAKEY_PROP_TOLERANCE, 0.1f);
            SetBoxedProperty<boolean>(D2D1_CHROMAKEY_PROP_INVERT_ALPHA, static_cast<boolean>(false));
            SetBoxedProperty<boolean>(D2D1_CHROMAKEY_PROP_FEATHER, static_cast<boolean>(false));
        }
    }

    IMPLEMENT_EFFECT_PROPERTY(ChromaKeyEffect,
        Color,
        float[3],
        Color,
        D2D1_CHROMAKEY_PROP_COLOR)

    IMPLEMENT_EFFECT_PROPERTY_WITH_VALIDATION(ChromaKeyEffect,
        Tolerance,
        float,
        float,
        D2D1_CHROMAKEY_PROP_TOLERANCE,
        (value >= 0.0f) && (value <= 1.0f))

    IMPLEMENT_EFFECT_PROPERTY(ChromaKeyEffect,
        InvertAlpha,
        boolean,
        boolean,
        D2D1_CHROMAKEY_PROP_INVERT_ALPHA)

    IMPLEMENT_EFFECT_PROPERTY(ChromaKeyEffect,
        Feather,
        boolean,
        boolean,
        D2D1_CHROMAKEY_PROP_FEATHER)

    IMPLEMENT_EFFECT_PROPERTY(ChromaKeyEffect,
        ColorHdr,
        ConvertColorHdrToVector3,
        Numerics::Vector4,
        D2D1_CHROMAKEY_PROP_COLOR)

    IMPLEMENT_EFFECT_SOURCE_PROPERTY(ChromaKeyEffect,
        Source,
        0)

    IMPLEMENT_EFFECT_PROPERTY_MAPPING(ChromaKeyEffect,
        { L"Color",       D2D1_CHROMAKEY_PROP_COLOR,        GRAPHICS_EFFECT_PROPERTY_MAPPING_COLOR_TO_VECTOR3 },
        { L"Tolerance",   D2D1_CHROMAKEY_PROP_TOLERANCE,    GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT           },
        { L"InvertAlpha", D2D1_CHROMAKEY_PROP_INVERT_ALPHA, GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT           },
        { L"Feather",     D2D1_CHROMAKEY_PROP_FEATHER,      GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT           },
        { L"ColorHdr",    D2D1_CHROMAKEY_PROP_COLOR,        GRAPHICS_EFFECT_PROPERTY_MAPPING_UNKNOWN          })

    ActivatableClassWithFactory(ChromaKeyEffect, SimpleAgileActivationFactory<ChromaKeyEffect>);
}}}}}

#endif // _WIN32_WINNT_WIN10
