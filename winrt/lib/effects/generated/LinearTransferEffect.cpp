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
#include "LinearTransferEffect.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects
{
    LinearTransferEffect::LinearTransferEffect()
        : CanvasEffect(CLSID_D2D1LinearTransfer, 13, 1, true)
    {
        // Set default values
        SetProperty<float>(D2D1_LINEARTRANSFER_PROP_RED_Y_INTERCEPT, 0.0f);
        SetProperty<float>(D2D1_LINEARTRANSFER_PROP_RED_SLOPE, 1.0f);
        SetProperty<boolean>(D2D1_LINEARTRANSFER_PROP_RED_DISABLE, static_cast<boolean>(false));
        SetProperty<float>(D2D1_LINEARTRANSFER_PROP_GREEN_Y_INTERCEPT, 0.0f);
        SetProperty<float>(D2D1_LINEARTRANSFER_PROP_GREEN_SLOPE, 1.0f);
        SetProperty<boolean>(D2D1_LINEARTRANSFER_PROP_GREEN_DISABLE, static_cast<boolean>(false));
        SetProperty<float>(D2D1_LINEARTRANSFER_PROP_BLUE_Y_INTERCEPT, 0.0f);
        SetProperty<float>(D2D1_LINEARTRANSFER_PROP_BLUE_SLOPE, 1.0f);
        SetProperty<boolean>(D2D1_LINEARTRANSFER_PROP_BLUE_DISABLE, static_cast<boolean>(false));
        SetProperty<float>(D2D1_LINEARTRANSFER_PROP_ALPHA_Y_INTERCEPT, 0.0f);
        SetProperty<float>(D2D1_LINEARTRANSFER_PROP_ALPHA_SLOPE, 1.0f);
        SetProperty<boolean>(D2D1_LINEARTRANSFER_PROP_ALPHA_DISABLE, static_cast<boolean>(false));
        SetProperty<boolean>(D2D1_LINEARTRANSFER_PROP_CLAMP_OUTPUT, static_cast<boolean>(false));
    }

    IMPLEMENT_EFFECT_PROPERTY(LinearTransferEffect,
        RedOffset,
        float,
        float,
        D2D1_LINEARTRANSFER_PROP_RED_Y_INTERCEPT)

    IMPLEMENT_EFFECT_PROPERTY(LinearTransferEffect,
        RedSlope,
        float,
        float,
        D2D1_LINEARTRANSFER_PROP_RED_SLOPE)

    IMPLEMENT_EFFECT_PROPERTY(LinearTransferEffect,
        RedDisable,
        boolean,
        boolean,
        D2D1_LINEARTRANSFER_PROP_RED_DISABLE)

    IMPLEMENT_EFFECT_PROPERTY(LinearTransferEffect,
        GreenOffset,
        float,
        float,
        D2D1_LINEARTRANSFER_PROP_GREEN_Y_INTERCEPT)

    IMPLEMENT_EFFECT_PROPERTY(LinearTransferEffect,
        GreenSlope,
        float,
        float,
        D2D1_LINEARTRANSFER_PROP_GREEN_SLOPE)

    IMPLEMENT_EFFECT_PROPERTY(LinearTransferEffect,
        GreenDisable,
        boolean,
        boolean,
        D2D1_LINEARTRANSFER_PROP_GREEN_DISABLE)

    IMPLEMENT_EFFECT_PROPERTY(LinearTransferEffect,
        BlueOffset,
        float,
        float,
        D2D1_LINEARTRANSFER_PROP_BLUE_Y_INTERCEPT)

    IMPLEMENT_EFFECT_PROPERTY(LinearTransferEffect,
        BlueSlope,
        float,
        float,
        D2D1_LINEARTRANSFER_PROP_BLUE_SLOPE)

    IMPLEMENT_EFFECT_PROPERTY(LinearTransferEffect,
        BlueDisable,
        boolean,
        boolean,
        D2D1_LINEARTRANSFER_PROP_BLUE_DISABLE)

    IMPLEMENT_EFFECT_PROPERTY(LinearTransferEffect,
        AlphaOffset,
        float,
        float,
        D2D1_LINEARTRANSFER_PROP_ALPHA_Y_INTERCEPT)

    IMPLEMENT_EFFECT_PROPERTY(LinearTransferEffect,
        AlphaSlope,
        float,
        float,
        D2D1_LINEARTRANSFER_PROP_ALPHA_SLOPE)

    IMPLEMENT_EFFECT_PROPERTY(LinearTransferEffect,
        AlphaDisable,
        boolean,
        boolean,
        D2D1_LINEARTRANSFER_PROP_ALPHA_DISABLE)

    IMPLEMENT_EFFECT_PROPERTY(LinearTransferEffect,
        ClampOutput,
        boolean,
        boolean,
        D2D1_LINEARTRANSFER_PROP_CLAMP_OUTPUT)

    IMPLEMENT_EFFECT_INPUT_PROPERTY(LinearTransferEffect,
        Source,
        0)

    ActivatableClass(LinearTransferEffect);
}}}}}
