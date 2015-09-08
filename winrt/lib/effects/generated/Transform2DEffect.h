// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

// This file was automatically generated. Please do not edit it manually.

#pragma once

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects 
{
    using namespace ::Microsoft::WRL;
    using namespace ABI::Microsoft::Graphics::Canvas;

    class Transform2DEffect : public RuntimeClass<
        ITransform2DEffect,
        MixIn<Transform2DEffect, CanvasEffect>>,
        public CanvasEffect
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_Effects_Transform2DEffect, BaseTrust);

    public:
        Transform2DEffect(ICanvasDevice* device = nullptr, ID2D1Effect* effect = nullptr);

        static IID const& EffectId() { return CLSID_D2D12DAffineTransform; }

        EFFECT_PROPERTY(InterpolationMode, CanvasImageInterpolation);
        EFFECT_PROPERTY(BorderMode, EffectBorderMode);
        EFFECT_PROPERTY(TransformMatrix, Numerics::Matrix3x2);
        EFFECT_PROPERTY(Sharpness, float);
        EFFECT_PROPERTY(Source, IGraphicsEffectSource*);

        EFFECT_PROPERTY_MAPPING();
    };
}}}}}
