// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

typedef IDWriteRenderingParams3 DWriteTextParamsType;

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Text
{
    class CanvasTextRenderingParameters : RESOURCE_WRAPPER_RUNTIME_CLASS(
        DWriteTextParamsType,
        CanvasTextRenderingParameters,
        ICanvasTextRenderingParameters)
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_Text_CanvasTextRenderingParameters, BaseTrust);

    public:
        CanvasTextRenderingParameters(DWriteTextParamsType* renderingParams);

        IFACEMETHOD(get_RenderingMode)(CanvasTextRenderingMode* value) override;

        IFACEMETHOD(get_GridFit)(CanvasTextGridFit* value) override;

        IFACEMETHOD(Close)() override;

        static ComPtr<CanvasTextRenderingParameters> CreateNew(
            CanvasTextRenderingMode textRenderingMode,
            CanvasTextGridFit textGridFit);
    };


    //
    // CanvasTextRenderingParametersFactory
    //

    class CanvasTextRenderingParametersFactory
        : public AgileActivationFactory<ICanvasTextRenderingParametersFactory>
        , private LifespanTracker<CanvasTextRenderingParametersFactory>
    {
        InspectableClassStatic(RuntimeClass_Microsoft_Graphics_Canvas_Text_CanvasTextRenderingParameters, BaseTrust);

    public:
        IFACEMETHOD(Create)(
            CanvasTextRenderingMode textRenderingMode,
            CanvasTextGridFit gridFit,
            ICanvasTextRenderingParameters** textRenderingParameters);
    };
    
}}}}}
