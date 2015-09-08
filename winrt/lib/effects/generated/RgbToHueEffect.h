// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

// This file was automatically generated. Please do not edit it manually.

#pragma once

#if (defined _WIN32_WINNT_WIN10) && (WINVER >= _WIN32_WINNT_WIN10)

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects 
{
    using namespace ::Microsoft::WRL;
    using namespace ABI::Microsoft::Graphics::Canvas;

    class RgbToHueEffect : public RuntimeClass<
        IRgbToHueEffect,
        MixIn<RgbToHueEffect, CanvasEffect>>,
        public CanvasEffect
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_Effects_RgbToHueEffect, BaseTrust);

    public:
        RgbToHueEffect(ID2D1Effect* effect = nullptr);

        static IID const& EffectId() { return CLSID_D2D1RgbToHue; }

        EFFECT_PROPERTY(OutputColorSpace, EffectHueColorSpace);
        EFFECT_PROPERTY(Source, IGraphicsEffectSource*);

        EFFECT_PROPERTY_MAPPING();
    };
}}}}}

#endif // _WIN32_WINNT_WIN10
