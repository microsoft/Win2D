// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"
#include "PixelShaderTransform.h"
#include "SharedShaderState.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects
{
    PixelShaderTransform::PixelShaderTransform(ISharedShaderState* sharedState, std::shared_ptr<CoordinateMappingState> const& coordinateMapping)
        : m_sharedState(sharedState)
        , m_coordinateMapping(coordinateMapping)
    { }


    IFACEMETHODIMP_(UINT32) PixelShaderTransform::GetInputCount() const
    {
        return m_sharedState->Shader().InputCount;
    }


    IFACEMETHODIMP PixelShaderTransform::MapInputRectsToOutputRect(D2D1_RECT_L const* inputRects, D2D1_RECT_L const* inputOpaqueSubRects, UINT32 inputRectCount, D2D1_RECT_L* outputRect, D2D1_RECT_L* outputOpaqueSubRect)
    {
        UNREFERENCED_PARAMETER(inputRects);
        UNREFERENCED_PARAMETER(inputOpaqueSubRects);
        UNREFERENCED_PARAMETER(inputRectCount);

        // The proper output rect calculation is carried out by ClipTransform, which is inserted after the PixelShaderTransform.
        *outputRect = D2D1_RECT_L{ INT_MIN, INT_MIN, INT_MAX, INT_MAX };
        *outputOpaqueSubRect = D2D1_RECT_L{ 0, 0, 0, 0 };

        return S_OK;
    }


    IFACEMETHODIMP PixelShaderTransform::MapOutputRectToInputRects(D2D1_RECT_L const* outputRect, D2D1_RECT_L* inputRects, UINT32 inputRectsCount) const
    {
        return ExceptionBoundary([&]
        {
            for (unsigned i = 0; i < inputRectsCount; i++)
            {
                switch (m_coordinateMapping->Mapping[i])
                {
                case SamplerCoordinateMapping::Unknown:
                    // Due to unknown coordinate mapping, we must request access to an infinite input area.
                    inputRects[i] = D2D_RECT_L{ INT_MIN, INT_MIN, INT_MAX, INT_MAX };
                    break;

                case SamplerCoordinateMapping::OneToOne:
                    // This output rectangle maps directly to the input.
                    inputRects[i] = *outputRect;
                    break;

                case SamplerCoordinateMapping::Offset:
                    // This rectangle must be expanded due to the use of offset texture coordinates.
                    inputRects[i] = ExpandRectangle(*outputRect, m_coordinateMapping->MaxOffset);
                    break;

                default:
                    ThrowHR(E_INVALIDARG);
                }
            }
        });
    }


    IFACEMETHODIMP PixelShaderTransform::MapInvalidRect(UINT32 inputIndex, D2D1_RECT_L invalidInputRect, D2D1_RECT_L* invalidOutputRect) const
    {
        UNREFERENCED_PARAMETER(inputIndex);
        UNREFERENCED_PARAMETER(invalidInputRect);

        // Mark the entire output as invalid.
        *invalidOutputRect = D2D_RECT_L{ INT_MIN, INT_MIN, INT_MAX, INT_MAX };

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


    void PixelShaderTransform::SetSourceInterpolation(SourceInterpolationState const* sourceInterpolation)
    {
        auto inputCount = m_sharedState->Shader().InputCount;

        for (unsigned i = 0; i < inputCount; i++)
        {
            ThrowIfFailed(m_drawInfo->SetInputDescription(i, D2D1_INPUT_DESCRIPTION{ sourceInterpolation->Filter[i], 0 }));
        }
    }

}}}}}
