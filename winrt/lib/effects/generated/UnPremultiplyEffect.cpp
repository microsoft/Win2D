// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

// This file was automatically generated. Please do not edit it manually.

#include "pch.h"
#include "UnPremultiplyEffect.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects
{
    UnPremultiplyEffect::UnPremultiplyEffect(ID2D1Effect* effect)
        : CanvasEffect(effect, EffectId(), 0, 1, true)
    {
        if (!effect)
        {
            // Set default values
        }
    }

    IMPLEMENT_EFFECT_SOURCE_PROPERTY(UnPremultiplyEffect,
        Source,
        0)

    ActivatableClass(UnPremultiplyEffect);
}}}}}
