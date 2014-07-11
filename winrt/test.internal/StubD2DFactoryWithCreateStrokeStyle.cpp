// Copyright (c) Microsoft Corporation.  All rights reserved

#include "pch.h"

STDMETHODIMP StubD2DFactoryWithCreateStrokeStyle::CreateStrokeStyle(
    _In_ CONST D2D1_STROKE_STYLE_PROPERTIES1 *strokeStyleProperties,
    _In_reads_opt_(dashesCount) CONST FLOAT *dashes,
    UINT32 dashesCount,
    _Outptr_ ID2D1StrokeStyle1 **strokeStyle
    )
{
    m_numCallsToCreateStrokeStyle++;
    m_startCap = strokeStyleProperties->startCap;
    m_endCap = strokeStyleProperties->endCap;
    m_dashCap = strokeStyleProperties->dashCap;
    m_lineJoin = strokeStyleProperties->lineJoin;
    m_miterLimit = strokeStyleProperties->miterLimit;
    m_dashStyle = strokeStyleProperties->dashStyle;
    m_dashOffset = strokeStyleProperties->dashOffset;
    m_customDashElements.resize(dashesCount);
    if (dashesCount > 0)
    {
        memcpy(&(m_customDashElements[0]), dashes, sizeof(float)*dashesCount);
    }
    m_transformBehavior = strokeStyleProperties->transformType;

    auto newStrokeStyle = Make<StubD2DStrokeStyleWithGetFactory>(this);

    CheckMakeResult(newStrokeStyle);

    ThrowIfFailed(newStrokeStyle.CopyTo(strokeStyle));

    return S_OK;
}