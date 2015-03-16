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

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas
{
    class GeometrySink : public RuntimeClass<RuntimeClassFlags<ClassicCom>, ID2D1GeometrySink>,
        private LifespanTracker<GeometrySink>
    {
        ComPtr<ICanvasPathReceiver> m_streamReader;
        HRESULT m_result;

    public:
        GeometrySink(ComPtr<ICanvasPathReceiver> const& streamReader)
            : m_streamReader(streamReader)
            , m_result(S_OK)
        {}

        IFACEMETHODIMP_(void) BeginFigure(
            D2D1_POINT_2F startPoint,
            D2D1_FIGURE_BEGIN figureBegin) override
        {
            if (FAILED(m_result))
                return;

            m_result = m_streamReader->BeginFigure(
                FromD2DPoint(startPoint),
                static_cast<CanvasFigureFill>(figureBegin));
        }

        IFACEMETHODIMP_(void) AddLine(
            D2D1_POINT_2F point) override
        {
            if (FAILED(m_result))
                return;

            m_result = m_streamReader->AddLine(FromD2DPoint(point));
        }

        IFACEMETHODIMP_(void) AddLines(
            _In_reads_(pointsCount) CONST D2D1_POINT_2F *points,
            UINT32 pointsCount) override
        {
            for (uint32_t i = 0; i < pointsCount; ++i)
            {
                if (FAILED(m_result))
                    return;

                m_result = m_streamReader->AddLine(FromD2DPoint(points[i]));
            }
        }

        IFACEMETHODIMP_(void) AddBezier(
            _In_ CONST D2D1_BEZIER_SEGMENT *bezier) override
        {
            if (FAILED(m_result))
                return;

            m_result = m_streamReader->AddCubicBezier(
                FromD2DPoint(bezier->point1),
                FromD2DPoint(bezier->point2),
                FromD2DPoint(bezier->point3));
        }

        IFACEMETHODIMP_(void) AddBeziers(
            CONST D2D1_BEZIER_SEGMENT *beziers,
            UINT32 beziersCount) override
        {
            for (uint32_t i = 0; i < beziersCount; ++i)
            {
                if (FAILED(m_result))
                    return;

                m_result = m_streamReader->AddCubicBezier(
                    FromD2DPoint(beziers[i].point1),
                    FromD2DPoint(beziers[i].point2),
                    FromD2DPoint(beziers[i].point3));
            }
        }

        IFACEMETHODIMP_(void) AddQuadraticBezier(
            _In_ CONST D2D1_QUADRATIC_BEZIER_SEGMENT *bezier) override
        {
            if (FAILED(m_result))
                return;

            m_result = m_streamReader->AddQuadraticBezier(
                FromD2DPoint(bezier->point1),
                FromD2DPoint(bezier->point2));
        }

        IFACEMETHODIMP_(void) AddQuadraticBeziers(
            _In_reads_(beziersCount) CONST D2D1_QUADRATIC_BEZIER_SEGMENT *beziers,
            uint32_t beziersCount) override
        {
            for (uint32_t i = 0; i < beziersCount; ++i)
            {
                if (FAILED(m_result))
                    return;

                m_result = m_streamReader->AddQuadraticBezier(
                    FromD2DPoint(beziers[i].point1),
                    FromD2DPoint(beziers[i].point2));
            }
        }

        IFACEMETHODIMP_(void) AddArc(
            _In_ CONST D2D1_ARC_SEGMENT *arc) override
        {
            if (FAILED(m_result))
                return;

            m_result = m_streamReader->AddArc(
                FromD2DPoint(arc->point),
                arc->size.width,
                arc->size.height,
                ::DirectX::XMConvertToRadians(arc->rotationAngle),
                static_cast<CanvasSweepDirection>(arc->sweepDirection),
                static_cast<CanvasArcSize>(arc->arcSize));
        }

        IFACEMETHODIMP_(void) SetFillMode(
            D2D1_FILL_MODE fillMode) override
        {
            if (FAILED(m_result))
                return;

            m_result = m_streamReader->SetFilledRegionDetermination(
                static_cast<CanvasFilledRegionDetermination>(fillMode));
        }

        IFACEMETHODIMP_(void) SetSegmentFlags(
            D2D1_PATH_SEGMENT vertexFlags) override
        {
            if (FAILED(m_result))
                return;

            m_result = m_streamReader->SetSegmentOptions(
                static_cast<CanvasFigureSegmentOptions>(vertexFlags));
        }

        IFACEMETHODIMP_(void) EndFigure(
            D2D1_FIGURE_END figureEnd) override
        {
            if (FAILED(m_result))
                return;

            m_result = m_streamReader->EndFigure(
                static_cast<CanvasFigureLoop>(figureEnd));
        }

        IFACEMETHODIMP Close()
        {
            return m_result;
        }
    };
}}}}