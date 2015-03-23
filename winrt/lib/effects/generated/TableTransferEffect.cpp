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
#include "TableTransferEffect.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects
{
    TableTransferEffect::TableTransferEffect()
        : CanvasEffect(CLSID_D2D1TableTransfer, 9, 1, true)
    {
        // Set default values
        SetArrayProperty<float>(D2D1_TABLETRANSFER_PROP_RED_TABLE, { 0.0, 1.0 });
        SetBoxedProperty<boolean>(D2D1_TABLETRANSFER_PROP_RED_DISABLE, static_cast<boolean>(false));
        SetArrayProperty<float>(D2D1_TABLETRANSFER_PROP_GREEN_TABLE, { 0.0, 1.0 });
        SetBoxedProperty<boolean>(D2D1_TABLETRANSFER_PROP_GREEN_DISABLE, static_cast<boolean>(false));
        SetArrayProperty<float>(D2D1_TABLETRANSFER_PROP_BLUE_TABLE, { 0.0, 1.0 });
        SetBoxedProperty<boolean>(D2D1_TABLETRANSFER_PROP_BLUE_DISABLE, static_cast<boolean>(false));
        SetArrayProperty<float>(D2D1_TABLETRANSFER_PROP_ALPHA_TABLE, { 0.0, 1.0 });
        SetBoxedProperty<boolean>(D2D1_TABLETRANSFER_PROP_ALPHA_DISABLE, static_cast<boolean>(false));
        SetBoxedProperty<boolean>(D2D1_TABLETRANSFER_PROP_CLAMP_OUTPUT, static_cast<boolean>(false));
    }

    IMPLEMENT_EFFECT_ARRAY_PROPERTY(TableTransferEffect,
        RedTable,
        float,
        D2D1_TABLETRANSFER_PROP_RED_TABLE)

    IMPLEMENT_EFFECT_PROPERTY(TableTransferEffect,
        RedDisable,
        boolean,
        boolean,
        D2D1_TABLETRANSFER_PROP_RED_DISABLE)

    IMPLEMENT_EFFECT_ARRAY_PROPERTY(TableTransferEffect,
        GreenTable,
        float,
        D2D1_TABLETRANSFER_PROP_GREEN_TABLE)

    IMPLEMENT_EFFECT_PROPERTY(TableTransferEffect,
        GreenDisable,
        boolean,
        boolean,
        D2D1_TABLETRANSFER_PROP_GREEN_DISABLE)

    IMPLEMENT_EFFECT_ARRAY_PROPERTY(TableTransferEffect,
        BlueTable,
        float,
        D2D1_TABLETRANSFER_PROP_BLUE_TABLE)

    IMPLEMENT_EFFECT_PROPERTY(TableTransferEffect,
        BlueDisable,
        boolean,
        boolean,
        D2D1_TABLETRANSFER_PROP_BLUE_DISABLE)

    IMPLEMENT_EFFECT_ARRAY_PROPERTY(TableTransferEffect,
        AlphaTable,
        float,
        D2D1_TABLETRANSFER_PROP_ALPHA_TABLE)

    IMPLEMENT_EFFECT_PROPERTY(TableTransferEffect,
        AlphaDisable,
        boolean,
        boolean,
        D2D1_TABLETRANSFER_PROP_ALPHA_DISABLE)

    IMPLEMENT_EFFECT_PROPERTY(TableTransferEffect,
        ClampOutput,
        boolean,
        boolean,
        D2D1_TABLETRANSFER_PROP_CLAMP_OUTPUT)

    IMPLEMENT_EFFECT_SOURCE_PROPERTY(TableTransferEffect,
        Source,
        0)

    ActivatableClass(TableTransferEffect);
}}}}}
