// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Text
{
    using namespace ::Microsoft::WRL;

    class CanvasTypographyFactory
        : public ActivationFactory<>
        , private LifespanTracker<CanvasTypographyFactory>
    {
    public:
        IFACEMETHOD(ActivateInstance)(IInspectable** ppvObject) override;
    };

    class CanvasTypography : RESOURCE_WRAPPER_RUNTIME_CLASS(
        IDWriteTypography,
        CanvasTypography,
        ICanvasTypography)
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_Text_CanvasTypography, BaseTrust);
        
    public:
        CanvasTypography(IDWriteTypography* typography);

        IFACEMETHOD(AddFeature)(CanvasTypographyFeature feature) override;

        IFACEMETHOD(AddFeatureWithNameAndParameter)(CanvasTypographyFeatureName name, uint32_t parameter) override;

        IFACEMETHOD(GetFeatures)(
            uint32_t* positionCount,
            CanvasTypographyFeature** positions) override;
    };

}}}}}
