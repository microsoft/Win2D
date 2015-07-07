// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

// This file was automatically generated. Please do not edit it manually.

#pragma once

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects 
{
    using namespace ::Microsoft::WRL;
    using namespace ABI::Microsoft::Graphics::Canvas;

    class ColorSourceEffect : public RuntimeClass<
        IColorSourceEffect,
        MixIn<ColorSourceEffect, CanvasEffect>>,
        public CanvasEffect
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_Effects_ColorSourceEffect, BaseTrust);

    public:
        ColorSourceEffect();

        EFFECT_PROPERTY(Color, Color);

        EFFECT_PROPERTY_MAPPING();
    };
}}}}}
