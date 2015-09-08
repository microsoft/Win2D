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

    class InvertEffect : public RuntimeClass<
        IInvertEffect,
        MixIn<InvertEffect, CanvasEffect>>,
        public CanvasEffect
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_Effects_InvertEffect, BaseTrust);

    public:
        InvertEffect(ID2D1Effect* effect = nullptr);

        static IID const& EffectId() { return CLSID_D2D1Invert; }

        EFFECT_PROPERTY(Source, IGraphicsEffectSource*);
    };
}}}}}

#endif // _WIN32_WINNT_WIN10
