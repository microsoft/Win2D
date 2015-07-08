// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

#include "StubD2DStrokeStyle.h"

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
        memcpy_s(&m_customDashElements[0], sizeof(float)*dashesCount, dashes, sizeof(float)*dashesCount);
    }
    m_transformBehavior = strokeStyleProperties->transformType;

    auto newStrokeStyle = Make<StubD2DStrokeStyleWithGetFactory>(this);

    CheckMakeResult(newStrokeStyle);

    ThrowIfFailed(newStrokeStyle.CopyTo(strokeStyle));

    return S_OK;
}

IFACEMETHODIMP StubD2DDeviceContextWithGetFactory::CreateBitmap(
    D2D1_SIZE_U size,
    const void *data,
    UINT32 dataSize,
    const D2D1_BITMAP_PROPERTIES1* properties,
    ID2D1Bitmap1** out)
{
    ComPtr<ID2D1Bitmap> bitmap = Make<StubD2DBitmap>();

    return bitmap.CopyTo(out);
}
