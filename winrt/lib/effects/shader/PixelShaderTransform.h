// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects 
{
    class ISharedShaderState;


    // Custom Direct2D draw transform.
    class PixelShaderTransform : public RuntimeClass<RuntimeClassFlags<ClassicCom>, ID2D1DrawTransform, ID2D1Transform, ID2D1TransformNode>
                               , private LifespanTracker<PixelShaderTransform>
    {
        ComPtr<ISharedShaderState> m_sharedState;

        ComPtr<ID2D1DrawInfo> m_drawInfo;

    public:
        PixelShaderTransform(ISharedShaderState* sharedState);

        IFACEMETHOD_(UINT32, GetInputCount)() const override;

        IFACEMETHOD(MapInputRectsToOutputRect)(D2D1_RECT_L const* inputRects, D2D1_RECT_L const* inputOpaqueSubRects, UINT32 inputRectCount, D2D1_RECT_L* outputRect, D2D1_RECT_L* outputOpaqueSubRect) override;
        IFACEMETHOD(MapOutputRectToInputRects)(D2D1_RECT_L const* outputRect, D2D1_RECT_L* inputRects, UINT32 inputRectsCount) const override;
        IFACEMETHOD(MapInvalidRect)(UINT32 inputIndex, D2D1_RECT_L invalidInputRect, D2D1_RECT_L* invalidOutputRect) const override;

        IFACEMETHOD(SetDrawInfo)(ID2D1DrawInfo* drawInfo) override;

        void SetConstants(std::vector<BYTE> const& constants);
    };

}}}}}
