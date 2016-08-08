// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

// This file was automatically generated. Please do not edit it manually.

#pragma once

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects 
{
    using namespace ::Microsoft::WRL;
    using namespace ABI::Microsoft::Graphics::Canvas;

    class ColorManagementEffect : public RuntimeClass<
        IColorManagementEffect,
        MixIn<ColorManagementEffect, CanvasEffect>>,
        public CanvasEffect
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_Effects_ColorManagementEffect, BaseTrust);

    public:
        ColorManagementEffect(ICanvasDevice* device = nullptr, ID2D1Effect* effect = nullptr);

        static IID const& EffectId() { return CLSID_D2D1ColorManagement; }

        EFFECT_PROPERTY(SourceColorProfile, IColorManagementProfile*);
        EFFECT_PROPERTY(SourceRenderingIntent, ColorManagementRenderingIntent);
        EFFECT_PROPERTY(OutputColorProfile, IColorManagementProfile*);
        EFFECT_PROPERTY(OutputRenderingIntent, ColorManagementRenderingIntent);
        EFFECT_PROPERTY(AlphaMode, CanvasAlphaMode);
        EFFECT_PROPERTY(Quality, ColorManagementEffectQuality);
        EFFECT_PROPERTY(Source, IGraphicsEffectSource*);

        EFFECT_PROPERTY_MAPPING();
    };

    class ColorManagementEffectFactory
        : public AgileActivationFactory<IColorManagementEffectStatics>
        , private LifespanTracker<ColorManagementEffectFactory>
    {
        InspectableClassStatic(RuntimeClass_Microsoft_Graphics_Canvas_Effects_ColorManagementEffect, BaseTrust);

    public:
        IFACEMETHODIMP ActivateInstance(IInspectable**) override;
        IFACEMETHODIMP IsBestQualitySupported(ICanvasDevice* device, boolean* result) override;
    };
}}}}}
