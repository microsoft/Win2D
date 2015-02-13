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

namespace canvas
{
    class MockD2DGeometrySink : public RuntimeClass<
        RuntimeClassFlags<ClassicCom>,
        ChainInterfaces<ID2D1GeometrySink, ID2D1SimplifiedGeometrySink>>
    {
    public:
        CALL_COUNTER_WITH_MOCK(AddLineMethod, void(D2D1_POINT_2F));
        CALL_COUNTER_WITH_MOCK(AddBezierMethod, void(CONST D2D1_BEZIER_SEGMENT*));
        CALL_COUNTER_WITH_MOCK(AddQuadraticBezierMethod, void(CONST D2D1_QUADRATIC_BEZIER_SEGMENT*));
        CALL_COUNTER_WITH_MOCK(AddQuadraticBeziersMethod, void(CONST D2D1_QUADRATIC_BEZIER_SEGMENT*, UINT32));
        CALL_COUNTER_WITH_MOCK(AddArcMethod, void(CONST D2D1_ARC_SEGMENT*));

        CALL_COUNTER_WITH_MOCK(SetFillModeMethod, void(D2D1_FILL_MODE));
        CALL_COUNTER_WITH_MOCK(SetSegmentFlagsMethod, void(D2D1_PATH_SEGMENT));
        CALL_COUNTER_WITH_MOCK(BeginFigureMethod, void(D2D1_POINT_2F, D2D1_FIGURE_BEGIN));
        CALL_COUNTER_WITH_MOCK(AddLinesMethod, void(CONST D2D1_POINT_2F*, UINT32));
        CALL_COUNTER_WITH_MOCK(AddBeziersMethod, void(CONST D2D1_BEZIER_SEGMENT*, UINT32));
        CALL_COUNTER_WITH_MOCK(EndFigureMethod, void(D2D1_FIGURE_END));
        CALL_COUNTER_WITH_MOCK(CloseMethod, HRESULT());

        //
        // ID2D1GeometrySink
        //

        STDMETHOD_(void, AddLine)(
            D2D1_POINT_2F point) override
        {
            AddLineMethod.WasCalled(point);
        }

        STDMETHOD_(void, AddBezier)(
            CONST D2D1_BEZIER_SEGMENT* bezier) override
        {
            AddBezierMethod.WasCalled(bezier);
        }

        STDMETHOD_(void, AddQuadraticBezier)(
            CONST D2D1_QUADRATIC_BEZIER_SEGMENT* bezier) override
        {
            AddQuadraticBezierMethod.WasCalled(bezier);
        }

        STDMETHOD_(void, AddQuadraticBeziers)(
            CONST D2D1_QUADRATIC_BEZIER_SEGMENT* beziers,
            UINT32 beziersCount) override
        {
            AddQuadraticBeziersMethod.WasCalled(beziers, beziersCount);
        }

        STDMETHOD_(void, AddArc)(
            CONST D2D1_ARC_SEGMENT* arc) override
        {
            AddArcMethod.WasCalled(arc);
        }

        //
        // ID2D1SimplifiedGeometrySink
        //

        STDMETHOD_(void, SetFillMode)(
            D2D1_FILL_MODE fillMode) override
        {
            SetFillModeMethod.WasCalled(fillMode);
        }

        STDMETHOD_(void, SetSegmentFlags)(
            D2D1_PATH_SEGMENT vertexFlags) override
        {
            SetSegmentFlagsMethod.WasCalled(vertexFlags);
        }

        STDMETHOD_(void, BeginFigure)(
            D2D1_POINT_2F startPoint,
            D2D1_FIGURE_BEGIN figureBegin) override
        {
            BeginFigureMethod.WasCalled(startPoint, figureBegin);
        }

        STDMETHOD_(void, AddLines)(
            CONST D2D1_POINT_2F* points,
            UINT32 pointsCount) override
        {
            AddLinesMethod.WasCalled(points, pointsCount);
        }

        STDMETHOD_(void, AddBeziers)(
            CONST D2D1_BEZIER_SEGMENT* beziers,
            UINT32 beziersCount) override
        {
            AddBeziersMethod.WasCalled(beziers, beziersCount);
        }

        STDMETHOD_(void, EndFigure)(
            D2D1_FIGURE_END figureEnd) override
        {
            EndFigureMethod.WasCalled(figureEnd);
        }

        STDMETHOD(Close)()
        {
            return CloseMethod.WasCalled();
        }

    };
}
