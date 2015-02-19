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
#include "ArithmeticCompositeEffect.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects
{
    ArithmeticCompositeEffect::ArithmeticCompositeEffect()
        : CanvasEffect(CLSID_D2D1ArithmeticComposite, 2, 2, true)
    {
        // Set default values
        SetProperty<float[4]>(D2D1_ARITHMETICCOMPOSITE_PROP_COEFFICIENTS, Numerics::Vector4{ 1.0f, 0.0f, 0.0f, 0.0f });
        SetProperty<boolean>(D2D1_ARITHMETICCOMPOSITE_PROP_CLAMP_OUTPUT, static_cast<boolean>(false));
    }

    IMPLEMENT_EFFECT_PROPERTY(ArithmeticCompositeEffect,
        ClampOutput,
        boolean,
        boolean,
        D2D1_ARITHMETICCOMPOSITE_PROP_CLAMP_OUTPUT)

    IMPLEMENT_EFFECT_INPUT_PROPERTY(ArithmeticCompositeEffect,
        Source1,
        0)

    IMPLEMENT_EFFECT_INPUT_PROPERTY(ArithmeticCompositeEffect,
        Source2,
        1)

    ActivatableClass(ArithmeticCompositeEffect);
}}}}}
