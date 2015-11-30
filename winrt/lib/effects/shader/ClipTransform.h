// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects 
{
    class ISharedShaderState;
    struct CoordinateMappingState;


    // Custom Direct2D draw transform clips the output image back down to the desired size.
    // This avoids it becoming infinite due to intermediate border transforms.
    class ClipTransform : public RuntimeClass<RuntimeClassFlags<ClassicCom>, ID2D1DrawTransform, ID2D1Transform, ID2D1TransformNode>
                        , private LifespanTracker<ClipTransform>
    {
        ComPtr<ISharedShaderState> m_sharedState;
        std::shared_ptr<CoordinateMappingState> m_coordinateMapping;

    public:
        ClipTransform(ISharedShaderState* sharedState, std::shared_ptr<CoordinateMappingState> const& coordinateMapping);

        IFACEMETHOD_(UINT32, GetInputCount)() const override;

        IFACEMETHOD(MapInputRectsToOutputRect)(D2D1_RECT_L const* inputRects, D2D1_RECT_L const* inputOpaqueSubRects, UINT32 inputRectCount, D2D1_RECT_L* outputRect, D2D1_RECT_L* outputOpaqueSubRect) override;
        IFACEMETHOD(MapOutputRectToInputRects)(D2D1_RECT_L const* outputRect, D2D1_RECT_L* inputRects, UINT32 inputRectsCount) const override;
        IFACEMETHOD(MapInvalidRect)(UINT32 inputIndex, D2D1_RECT_L invalidInputRect, D2D1_RECT_L* invalidOutputRect) const override;

        IFACEMETHOD(SetDrawInfo)(ID2D1DrawInfo* drawInfo) override;
    };

}}}}}
