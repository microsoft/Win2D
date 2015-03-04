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
#include "OpacityMetadataEffect.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects
{
    OpacityMetadataEffect::OpacityMetadataEffect()
        : CanvasEffect(CLSID_D2D1OpacityMetadata, 1, 1, true)
    {
        // Set default values
        SetProperty<float[4]>(D2D1_OPACITYMETADATA_PROP_INPUT_OPAQUE_RECT, Rect{ -std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity() });
    }

    IMPLEMENT_EFFECT_PROPERTY(OpacityMetadataEffect,
        OpaqueRectangle,
        float[4],
        Rect,
        D2D1_OPACITYMETADATA_PROP_INPUT_OPAQUE_RECT)

    IMPLEMENT_EFFECT_INPUT_PROPERTY(OpacityMetadataEffect,
        Source,
        0)

    ActivatableClass(OpacityMetadataEffect);
}}}}}
