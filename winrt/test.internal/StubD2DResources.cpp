// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may
// not use these files except in compliance with the License. You may obtain
// a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
// License for the specific language governing permissions and limitations
// under the License.

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