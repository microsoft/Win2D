// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may
// not use these files except in compliance with the License. You may obtain
// a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
// License for the specific language governing permissions and limitations
// under the License.

// This file was automatically generated. Please do not edit it manually.

#include "pch.h"
#include "GammaTransferEffect.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects
{
    GammaTransferEffect::GammaTransferEffect()
        : CanvasEffect(CLSID_D2D1GammaTransfer, 17, 1, true)
    {
        // Set default values
        SetProperty<float>(D2D1_GAMMATRANSFER_PROP_RED_AMPLITUDE, 1.0f);
        SetProperty<float>(D2D1_GAMMATRANSFER_PROP_RED_EXPONENT, 1.0f);
        SetProperty<float>(D2D1_GAMMATRANSFER_PROP_RED_OFFSET, 0.0f);
        SetProperty<boolean>(D2D1_GAMMATRANSFER_PROP_RED_DISABLE, static_cast<boolean>(false));
        SetProperty<float>(D2D1_GAMMATRANSFER_PROP_GREEN_AMPLITUDE, 1.0f);
        SetProperty<float>(D2D1_GAMMATRANSFER_PROP_GREEN_EXPONENT, 1.0f);
        SetProperty<float>(D2D1_GAMMATRANSFER_PROP_GREEN_OFFSET, 0.0f);
        SetProperty<boolean>(D2D1_GAMMATRANSFER_PROP_GREEN_DISABLE, static_cast<boolean>(false));
        SetProperty<float>(D2D1_GAMMATRANSFER_PROP_BLUE_AMPLITUDE, 1.0f);
        SetProperty<float>(D2D1_GAMMATRANSFER_PROP_BLUE_EXPONENT, 1.0f);
        SetProperty<float>(D2D1_GAMMATRANSFER_PROP_BLUE_OFFSET, 0.0f);
        SetProperty<boolean>(D2D1_GAMMATRANSFER_PROP_BLUE_DISABLE, static_cast<boolean>(false));
        SetProperty<float>(D2D1_GAMMATRANSFER_PROP_ALPHA_AMPLITUDE, 1.0f);
        SetProperty<float>(D2D1_GAMMATRANSFER_PROP_ALPHA_EXPONENT, 1.0f);
        SetProperty<float>(D2D1_GAMMATRANSFER_PROP_ALPHA_OFFSET, 0.0f);
        SetProperty<boolean>(D2D1_GAMMATRANSFER_PROP_ALPHA_DISABLE, static_cast<boolean>(false));
        SetProperty<boolean>(D2D1_GAMMATRANSFER_PROP_CLAMP_OUTPUT, static_cast<boolean>(false));
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

    IMPLEMENT_EFFECT_INPUT_PROPERTY(GammaTransferEffect,
        Source,
        0)

    ActivatableClass(GammaTransferEffect);
}}}}}
