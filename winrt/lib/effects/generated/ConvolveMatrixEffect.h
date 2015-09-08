// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

// This file was automatically generated. Please do not edit it manually.

#pragma once

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects 
{
    using namespace ::Microsoft::WRL;
    using namespace ABI::Microsoft::Graphics::Canvas;

    class ConvolveMatrixEffect : public RuntimeClass<
        IConvolveMatrixEffect,
        MixIn<ConvolveMatrixEffect, CanvasEffect>>,
        public CanvasEffect
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_Effects_ConvolveMatrixEffect, BaseTrust);

    public:
        ConvolveMatrixEffect(ID2D1Effect* effect = nullptr);

        static IID const& EffectId() { return CLSID_D2D1ConvolveMatrix; }

        EFFECT_PROPERTY(KernelScale, Numerics::Vector2);
        EFFECT_PROPERTY(InterpolationMode, CanvasImageInterpolation);
        EFFECT_PROPERTY(KernelWidth, int32_t);
        EFFECT_PROPERTY(KernelHeight, int32_t);
        EFFECT_ARRAY_PROPERTY(KernelMatrix, float);
        EFFECT_PROPERTY(Divisor, float);
        EFFECT_PROPERTY(Offset, float);
        EFFECT_PROPERTY(KernelOffset, Numerics::Vector2);
        EFFECT_PROPERTY(PreserveAlpha, boolean);
        EFFECT_PROPERTY(BorderMode, EffectBorderMode);
        EFFECT_PROPERTY(ClampOutput, boolean);
        EFFECT_PROPERTY(Source, IGraphicsEffectSource*);

        EFFECT_PROPERTY_MAPPING();
    };
}}}}}
