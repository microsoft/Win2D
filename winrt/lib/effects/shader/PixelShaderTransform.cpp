// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"
#include "PixelShaderTransform.h"
#include "SharedShaderState.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects
{
    PixelShaderTransform::PixelShaderTransform(ISharedShaderState* sharedState)
        : m_sharedState(sharedState)
    { }


    IFACEMETHODIMP_(UINT32) PixelShaderTransform::GetInputCount() const
    {
        return m_sharedState->Shader().InputCount;
    }


    static D2D1_RECT_L RectangleUnion(D2D1_RECT_L const* rectangles, unsigned count)
    {
        assert(count);

        D2D1_RECT_L result = rectangles[0];

        for (unsigned i = 1; i < count; i++)
        {
            result.left   = std::min(result.left,   rectangles[i].left);
            result.right  = std::max(result.right,  rectangles[i].right);
            result.top    = std::min(result.top,    rectangles[i].top);
            result.bottom = std::max(result.bottom, rectangles[i].bottom);
        }

        return result;
    }


    IFACEMETHODIMP PixelShaderTransform::MapInputRectsToOutputRect(D2D1_RECT_L const* inputRects, D2D1_RECT_L const* inputOpaqueSubRects, UINT32 inputRectCount, D2D1_RECT_L* outputRect, D2D1_RECT_L* outputOpaqueSubRect)
    {
        UNREFERENCED_PARAMETER(inputOpaqueSubRects);

        return ExceptionBoundary([&]
        {
            // For now, we assume 1:1 mapping.
            *outputRect = inputRectCount ? RectangleUnion(inputRects, inputRectCount)
                                         : D2D_RECT_L{ INT_MIN, INT_MIN, INT_MAX, INT_MAX };

            // We don't know how this shader handles opacity, so always just report an empty opaque subrect.
            *outputOpaqueSubRect = D2D1_RECT_L{ 0, 0, 0, 0 };
        });
    }


    IFACEMETHODIMP PixelShaderTransform::MapOutputRectToInputRects(D2D1_RECT_L const* outputRect, D2D1_RECT_L* inputRects, UINT32 inputRectsCount) const
    {
        // For now, we assume 1:1 mapping.
        for (unsigned i = 0; i < inputRectsCount; i++)
        {
            inputRects[i] = *outputRect;
        }

        return S_OK;
    }


    IFACEMETHODIMP PixelShaderTransform::MapInvalidRect(UINT32 inputIndex, D2D1_RECT_L invalidInputRect, D2D1_RECT_L* invalidOutputRect) const
    {
        UNREFERENCED_PARAMETER(inputIndex);

        // For now, we assume 1:1 mapping.
        *invalidOutputRect = invalidInputRect;

        return S_OK;
    }


    IFACEMETHODIMP PixelShaderTransform::SetDrawInfo(ID2D1DrawInfo* drawInfo)
    {
        return ExceptionBoundary([&]
        {
            m_drawInfo = drawInfo;

            // Tell D2D to use our pixel shader.
            auto& shader = m_sharedState->Shader();

            ThrowIfFailed(drawInfo->SetPixelShader(shader.Hash));

            drawInfo->SetInstructionCountHint(shader.InstructionCount);
        });
    }


    void PixelShaderTransform::SetConstants(std::vector<BYTE> const& constants)
    {
        ThrowIfFailed(m_drawInfo->SetPixelShaderConstantBuffer(constants.data(), static_cast<UINT32>(constants.size())));
    }

}}}}}
