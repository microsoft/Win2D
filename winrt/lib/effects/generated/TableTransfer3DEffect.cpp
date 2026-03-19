// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

// This file was automatically generated. Please do not edit it manually.

#include "pch.h"
#include "TableTransfer3DEffect.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects
{
    TableTransfer3DEffect::TableTransfer3DEffect(ICanvasDevice* device, ID2D1Effect* effect)
        : CanvasEffect(EffectId(), 2, 1, true, device, effect, static_cast<ITableTransfer3DEffect*>(this))
    {
        if (!effect)
        {
            // Set default values
            SetBoxedProperty<IEffectTransferTable3D*>(D2D1_LOOKUPTABLE3D_PROP_LUT, static_cast<IEffectTransferTable3D*>(nullptr));
            SetBoxedProperty<uint32_t>(D2D1_LOOKUPTABLE3D_PROP_ALPHA_MODE, D2D1_COLORMANAGEMENT_ALPHA_MODE_PREMULTIPLIED);
        }
    }

    IMPLEMENT_EFFECT_PROPERTY(TableTransfer3DEffect,
        Table,
        IEffectTransferTable3D*,
        IEffectTransferTable3D*,
        D2D1_LOOKUPTABLE3D_PROP_LUT)

    IMPLEMENT_EFFECT_PROPERTY(TableTransfer3DEffect,
        AlphaMode,
        ConvertAlphaMode,
        CanvasAlphaMode,
        D2D1_LOOKUPTABLE3D_PROP_ALPHA_MODE)

    IMPLEMENT_EFFECT_SOURCE_PROPERTY(TableTransfer3DEffect,
        Source,
        0)

    IMPLEMENT_EFFECT_PROPERTY_MAPPING(TableTransfer3DEffect,
        { L"Table",     D2D1_LOOKUPTABLE3D_PROP_LUT,        GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT                 },
        { L"AlphaMode", D2D1_LOOKUPTABLE3D_PROP_ALPHA_MODE, GRAPHICS_EFFECT_PROPERTY_MAPPING_COLORMATRIX_ALPHA_MODE })

    ActivatableClassWithFactory(TableTransfer3DEffect, ::SimpleAgileActivationFactory<TableTransfer3DEffect>);
}}}}}
