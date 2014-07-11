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

#pragma once

// This device derives from MockCanvasDevice, but it allows creation of stroke styles.
class StubD2DFactoryWithCreateStrokeStyle : public MockD2DFactory
{
public:
    StubD2DFactoryWithCreateStrokeStyle()
        : m_numCallsToCreateStrokeStyle(0)
        , m_startCap(D2D1_CAP_STYLE_FLAT)
        , m_endCap(D2D1_CAP_STYLE_FLAT)
        , m_dashCap(D2D1_CAP_STYLE_FLAT)
        , m_lineJoin(D2D1_LINE_JOIN_MITER)
        , m_miterLimit(10.0f)
        , m_dashStyle(D2D1_DASH_STYLE_SOLID)
        , m_dashOffset(0)
        , m_transformBehavior(D2D1_STROKE_TRANSFORM_TYPE_NORMAL)
    {}


    STDMETHOD(CreateStrokeStyle)(
        _In_ CONST D2D1_STROKE_STYLE_PROPERTIES1 *strokeStyleProperties,
        _In_reads_opt_(dashesCount) CONST FLOAT *dashes,
        UINT32 dashesCount,
        _Outptr_ ID2D1StrokeStyle1 **strokeStyle
        );

    int m_numCallsToCreateStrokeStyle;
    D2D1_CAP_STYLE m_startCap;
    D2D1_CAP_STYLE m_endCap;
    D2D1_CAP_STYLE m_dashCap;
    D2D1_LINE_JOIN m_lineJoin;
    float m_miterLimit;
    D2D1_DASH_STYLE m_dashStyle;
    float m_dashOffset;
    std::vector<float> m_customDashElements;
    D2D1_STROKE_TRANSFORM_TYPE m_transformBehavior;
};
