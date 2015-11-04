// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"
#include "ClipTransform.h"
#include "SharedShaderState.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects
{
    ClipTransform::ClipTransform(ISharedShaderState* sharedState, std::shared_ptr<CoordinateMappingState> const& coordinateMapping)
        : m_sharedState(sharedState)
        , m_coordinateMapping(coordinateMapping)
    { }


    IFACEMETHODIMP_(UINT32) ClipTransform::GetInputCount() const
    {
        // Our first input is the output from PixelShaderTransform. We also get copies of all the
        // PixelShaderTransform inputs, before any intermediate border transforms were applied to them.
        // This lets us compute a non infinite output rect even in the presence of hard borders.

        return 1 + m_sharedState->Shader().InputCount;
    }


    IFACEMETHODIMP ClipTransform::MapInputRectsToOutputRect(D2D1_RECT_L const* inputRects, D2D1_RECT_L const* inputOpaqueSubRects, UINT32 inputRectCount, D2D1_RECT_L* outputRect, D2D1_RECT_L* outputOpaqueSubRect)
    {
        UNREFERENCED_PARAMETER(inputOpaqueSubRects);

        return ExceptionBoundary([&]
        {
            D2D_RECT_L accumulatedRect = { INT_MIN, INT_MIN, INT_MAX, INT_MAX };
            bool gotRect = false;
            bool gotOffset = false;

            // Start at 1 to skip the output from PixelShaderTransform - we only care about its original input images.
            for (unsigned i = 1; i < inputRectCount; i++)
            {
                unsigned inputIndex = i - 1;

                if (inputIndex >= MaxShaderInputs)
                    ThrowHR(E_INVALIDARG);

                D2D_RECT_L rect;

                switch (m_coordinateMapping->Mapping[inputIndex])
                {
                    case SamplerCoordinateMapping::Unknown:
                        // Due to unknown coordinate mapping, this input does not contribute to the output rectangle.
                        continue;

                    case SamplerCoordinateMapping::OneToOne:
                        // This input rectangle maps directly to the output.
                        rect = inputRects[i];
                        break;

                    case SamplerCoordinateMapping::Offset:
                        // This rectangle may need to be expanded due to the use of offset texture coordinates.
                        if (!m_coordinateMapping->MaxOffset)
                        {
                            WinStringBuilder message;
                            message.Format(Strings::CustomEffectOffsetMappingWithoutMaxOffset, i);
                            ThrowHR(E_INVALIDARG, message.Get());
                        }

                        switch (m_coordinateMapping->BorderMode[inputIndex])
                        {
                            case EffectBorderMode::Soft:
                                // In soft border mode, expand the input rectangle.
                                rect = ExpandRectangle(inputRects[i], m_coordinateMapping->MaxOffset);
                                break;

                            case EffectBorderMode::Hard:
                                // For hard borders, pass the input through unchanged.
                                rect = inputRects[i];
                                break;

                            default:
                                ThrowHR(E_INVALIDARG);
                        }

                        gotOffset = true;
                        break;

                    default:
                        ThrowHR(E_INVALIDARG);
                }

                if (!gotRect)
                {
                    // This is the first output rectangle we have seen, so store it directly.
                    accumulatedRect = rect;
                    gotRect = true;
                }
                else
                {
                    // Subsequent rectangles are combined with the existing region.
                    accumulatedRect = RectangleUnion(accumulatedRect, rect);
                }
            }

            // Validate that if MaxOffset is set, at least one input should be using SamplerCoordinateMapping::Offset.
            if (m_coordinateMapping->MaxOffset && !gotOffset)
            {
                ThrowHR(E_INVALIDARG, Strings::CustomEffectMaxOffsetWithoutOffsetMapping);
            }

            // Return the accumulated output rectangle.
            *outputRect = accumulatedRect;

            // We don't know how this shader handles opacity, so always just report an empty opaque subrect.
            *outputOpaqueSubRect = D2D1_RECT_L{ 0, 0, 0, 0 };
        });
    }


    IFACEMETHODIMP ClipTransform::MapOutputRectToInputRects(D2D1_RECT_L const* outputRect, D2D1_RECT_L* inputRects, UINT32 inputRectsCount) const
    {
        for (unsigned i = 0; i < inputRectsCount; i++)
        {
            if (i == 0)
            {
                // Our first input is the PixelShaderTransform.
                // This gets the output rect passed through without modification.
                inputRects[i] = *outputRect;
            }
            else
            {
                // Don't care about the other inputs.
                // These are only connected to give MapInputRectsToOutputRect access to their bounds.
                inputRects[i] = D2D_RECT_L{ 0, 0, 0, 0 };
            }
        }

        return S_OK;
    }


    IFACEMETHODIMP ClipTransform::MapInvalidRect(UINT32 inputIndex, D2D1_RECT_L invalidInputRect, D2D1_RECT_L* invalidOutputRect) const
    {
        UNREFERENCED_PARAMETER(inputIndex);
        UNREFERENCED_PARAMETER(invalidInputRect);

        // Mark the entire output as invalid.
        *invalidOutputRect = D2D_RECT_L{ INT_MIN, INT_MIN, INT_MAX, INT_MAX };

        return S_OK;
    }


    IFACEMETHODIMP ClipTransform::SetDrawInfo(ID2D1DrawInfo*)
    {
        return S_OK;
    }

}}}}}
