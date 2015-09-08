// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

// This file was automatically generated. Please do not edit it manually.

#include "pch.h"
#include "TableTransferEffect.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects
{
    TableTransferEffect::TableTransferEffect(ICanvasDevice* device, ID2D1Effect* effect)
        : CanvasEffect(EffectId(), 9, 1, true, device, effect, static_cast<ITableTransferEffect*>(this))
    {
        if (!effect)
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

    IMPLEMENT_EFFECT_PROPERTY_MAPPING(TableTransferEffect,
        { L"RedTable",     D2D1_TABLETRANSFER_PROP_RED_TABLE,     GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
        { L"RedDisable",   D2D1_TABLETRANSFER_PROP_RED_DISABLE,   GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
        { L"GreenTable",   D2D1_TABLETRANSFER_PROP_GREEN_TABLE,   GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
        { L"GreenDisable", D2D1_TABLETRANSFER_PROP_GREEN_DISABLE, GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
        { L"BlueTable",    D2D1_TABLETRANSFER_PROP_BLUE_TABLE,    GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
        { L"BlueDisable",  D2D1_TABLETRANSFER_PROP_BLUE_DISABLE,  GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
        { L"AlphaTable",   D2D1_TABLETRANSFER_PROP_ALPHA_TABLE,   GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
        { L"AlphaDisable", D2D1_TABLETRANSFER_PROP_ALPHA_DISABLE, GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT },
        { L"ClampOutput",  D2D1_TABLETRANSFER_PROP_CLAMP_OUTPUT,  GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT })

    ActivatableClass(TableTransferEffect);
}}}}}
