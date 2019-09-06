// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

// This file was automatically generated. Please do not edit it manually.

#include "pch.h"
#include "GammaTransferEffect.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects
{
    GammaTransferEffect::GammaTransferEffect(ICanvasDevice* device, ID2D1Effect* effect)
        : CanvasEffect(EffectId(), 17, 1, true, device, effect, static_cast<IGammaTransferEffect*>(this))
    {
        if (!effect)
        {
            // Set default values
            SetBoxedProperty<float>(D2D1_GAMMATRANSFER_PROP_RED_AMPLITUDE, 1.0f);
            SetBoxedProperty<float>(D2D1_GAMMATRANSFER_PROP_RED_EXPONENT, 1.0f);
            SetBoxedProperty<float>(D2D1_GAMMATRANSFER_PROP_RED_OFFSET, 0.0f);
            SetBoxedProperty<boolean>(D2D1_GAMMATRANSFER_PROP_RED_DISABLE, static_cast<boolean>(false));
            SetBoxedProperty<float>(D2D1_GAMMATRANSFER_PROP_GREEN_AMPLITUDE, 1.0f);
            SetBoxedProperty<float>(D2D1_GAMMATRANSFER_PROP_GREEN_EXPONENT, 1.0f);
            SetBoxedProperty<float>(D2D1_GAMMATRANSFER_PROP_GREEN_OFFSET, 0.0f);
            SetBoxedProperty<boolean>(D2D1_GAMMATRANSFER_PROP_GREEN_DISABLE, static_cast<boolean>(false));
            SetBoxedProperty<float>(D2D1_GAMMATRANSFER_PROP_BLUE_AMPLITUDE, 1.0f);
            SetBoxedProperty<float>(D2D1_GAMMATRANSFER_PROP_BLUE_EXPONENT, 1.0f);
            SetBoxedProperty<float>(D2D1_GAMMATRANSFER_PROP_BLUE_OFFSET, 0.0f);
            SetBoxedProperty<boolean>(D2D1_GAMMATRANSFER_PROP_BLUE_DISABLE, static_cast<boolean>(false));
            SetBoxedProperty<float>(D2D1_GAMMATRANSFER_PROP_ALPHA_AMPLITUDE, 1.0f);
            SetBoxedProperty<float>(D2D1_GAMMATRANSFER_PROP_ALPHA_EXPONENT, 1.0f);
            SetBoxedProperty<float>(D2D1_GAMMATRANSFER_PROP_ALPHA_OFFSET, 0.0f);
            SetBoxedProperty<boolean>(D2D1_GAMMATRANSFER_PROP_ALPHA_DISABLE, static_cast<boolean>(false));
            SetBoxedProperty<boolean>(D2D1_GAMMATRANSFER_PROP_CLAMP_OUTPUT, static_cast<boolean>(false));
        }
    }

    IMPLEMENT_EFFECT_PROPERTY(GammaTransferEffect,
        RedAmplitude,
        float,
        float,
        D2D1_GAMMATRANSFER_PROP_RED_AMPLITUDE)

    IMPLEMENT_EFFECT_PROPERTY(GammaTransferEffect,
        RedExponent,
        float,
        float,
        D2D1_GAMMATRANSFER_PROP_RED_EXPONENT)

    IMPLEMENT_EFFECT_PROPERTY(GammaTransferEffect,
        RedOffset,
        float,
        float,
        D2D1_GAMMATRANSFER_PROP_RED_OFFSET)

    IMPLEMENT_EFFECT_PROPERTY(GammaTransferEffect,
        RedDisable,
        boolean,
        boolean,
        D2D1_GAMMATRANSFER_PROP_RED_DISABLE)

    IMPLEMENT_EFFECT_PROPERTY(GammaTransferEffect,
        GreenAmplitude,
        float,
        float,
        D2D1_GAMMATRANSFER_PROP_GREEN_AMPLITUDE)

    IMPLEMENT_EFFECT_PROPERTY(GammaTransferEffect,
        GreenExponent,
        float,
        float,
        D2D1_GAMMATRANSFER_PROP_GREEN_EXPONENT)

    IMPLEMENT_EFFECT_PROPERTY(GammaTransferEffect,
        GreenOffset,
        float,
        float,
        D2D1_GAMMATRANSFER_PROP_GREEN_OFFSET)

    IMPLEMENT_EFFECT_PROPERTY(GammaTransferEffect,
        GreenDisable,
        boolean,
        boolean,
        D2D1_GAMMATRANSFER_PROP_GREEN_DISABLE)

    IMPLEMENT_EFFECT_PROPERTY(GammaTransferEffect,
        BlueAmplitude,
        float,
        float,
        D2D1_GAMMATRANSFER_PROP_BLUE_AMPLITUDE)

    IMPLEMENT_EFFECT_PROPERTY(GammaTransferEffect,
        BlueExponent,
        float,
        float,
        D2D1_GAMMATRANSFER_PROP_BLUE_EXPONENT)

    IMPLEMENT_EFFECT_PROPERTY(GammaTransferEffect,
        BlueOffset,
        float,
        float,
        D2D1_GAMMATRANSFER_PROP_BLUE_OFFSET)

    IMPLEMENT_EFFECT_PROPERTY(GammaTransferEffect,
        BlueDisable,
        boolean,
        boolean,
        D2D1_GAMMATRANSFER_PROP_BLUE_DISABLE)

    IMPLEMENT_EFFECT_PROPERTY(GammaTransferEffect,
        AlphaAmplitude,
        float,
        float,
        D2D1_GAMMATRANSFER_PROP_ALPHA_AMPLITUDE)

    IMPLEMENT_EFFECT_PROPERTY(GammaTransferEffect,
        AlphaExponent,
        float,
        float,
        D2D1_GAMMATRANSFER_PROP_ALPHA_EXPONENT)

    IMPLEMENT_EFFECT_PROPERTY(GammaTransferEffect,
        AlphaOffset,
        float,
        float,
        D2D1_GAMMATRANSFER_PROP_ALPHA_OFFSET)

    IMPLEMENT_EFFECT_PROPERTY(GammaTransferEffect,
        AlphaDisable,
        boolean,
        boolean,
        D2D1_GAMMATRANSFER_PROP_ALPHA_DISABLE)

    IMPLEMENT_EFFECT_PROPERTY(GammaTransferEffect,
        ClampOutput,
        boolean,
        boolean,
        D2D1_GAMMATRANSFER_PROP_CLAMP_OUTPUT)

    IMPLEMENT_EFFECT_SOURCE_PROPERTY(GammaTransferEffect,
        Source,
        0)

    IMPLEMENT_EFFECT_PROPERTY_MAPPING(GammaTransferEffect,
        { L"RedAmplitude",   D2D1_GAMMATRANSFER_PROP_RED_AMPLITUDE,   GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
        { L"RedExponent",    D2D1_GAMMATRANSFER_PROP_RED_EXPONENT,    GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
        { L"RedOffset",      D2D1_GAMMATRANSFER_PROP_RED_OFFSET,      GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
        { L"RedDisable",     D2D1_GAMMATRANSFER_PROP_RED_DISABLE,     GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
        { L"GreenAmplitude", D2D1_GAMMATRANSFER_PROP_GREEN_AMPLITUDE, GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
        { L"GreenExponent",  D2D1_GAMMATRANSFER_PROP_GREEN_EXPONENT,  GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
        { L"GreenOffset",    D2D1_GAMMATRANSFER_PROP_GREEN_OFFSET,    GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
        { L"GreenDisable",   D2D1_GAMMATRANSFER_PROP_GREEN_DISABLE,   GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
        { L"BlueAmplitude",  D2D1_GAMMATRANSFER_PROP_BLUE_AMPLITUDE,  GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
        { L"BlueExponent",   D2D1_GAMMATRANSFER_PROP_BLUE_EXPONENT,   GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
        { L"BlueOffset",     D2D1_GAMMATRANSFER_PROP_BLUE_OFFSET,     GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
        { L"BlueDisable",    D2D1_GAMMATRANSFER_PROP_BLUE_DISABLE,    GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
        { L"AlphaAmplitude", D2D1_GAMMATRANSFER_PROP_ALPHA_AMPLITUDE, GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
        { L"AlphaExponent",  D2D1_GAMMATRANSFER_PROP_ALPHA_EXPONENT,  GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
        { L"AlphaOffset",    D2D1_GAMMATRANSFER_PROP_ALPHA_OFFSET,    GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
        { L"AlphaDisable",   D2D1_GAMMATRANSFER_PROP_ALPHA_DISABLE,   GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
        { L"ClampOutput",    D2D1_GAMMATRANSFER_PROP_CLAMP_OUTPUT,    GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT })

    ActivatableClassWithFactory(GammaTransferEffect, ::SimpleAgileActivationFactory<GammaTransferEffect>);
}}}}}
