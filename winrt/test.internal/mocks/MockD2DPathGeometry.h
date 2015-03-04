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

// This file was automatically generated. Please do not edit it manually.

#pragma once

namespace canvas
{
    class MockD2DPathGeometry : public RuntimeClass<
        RuntimeClassFlags<ClassicCom>,
        ChainInterfaces<ID2D1PathGeometry1, ID2D1PathGeometry, ID2D1Geometry, ID2D1Resource>>
    {
    public:
        CALL_COUNTER_WITH_MOCK(ComputePointAndSegmentAtLengthMethod, HRESULT(FLOAT, UINT32, CONST D2D1_MATRIX_3X2_F*, FLOAT, D2D1_POINT_DESCRIPTION*));

        CALL_COUNTER_WITH_MOCK(OpenMethod, HRESULT(ID2D1GeometrySink**));
        CALL_COUNTER_WITH_MOCK(StreamMethod, HRESULT(ID2D1GeometrySink*));
        CALL_COUNTER_WITH_MOCK(GetSegmentCountMethod, HRESULT(UINT32*));
        CALL_COUNTER_WITH_MOCK(GetFigureCountMethod, HRESULT(UINT32*));

        CALL_COUNTER_WITH_MOCK(GetBoundsMethod, HRESULT(CONST D2D1_MATRIX_3X2_F*, D2D1_RECT_F*));
        CALL_COUNTER_WITH_MOCK(GetWidenedBoundsMethod, HRESULT(FLOAT, ID2D1StrokeStyle*, CONST D2D1_MATRIX_3X2_F*, FLOAT, D2D1_RECT_F*));
        CALL_COUNTER_WITH_MOCK(StrokeContainsPointMethod, HRESULT(D2D1_POINT_2F, FLOAT, ID2D1StrokeStyle*, CONST D2D1_MATRIX_3X2_F*, FLOAT, BOOL*));
        CALL_COUNTER_WITH_MOCK(FillContainsPointMethod, HRESULT(D2D1_POINT_2F, CONST D2D1_MATRIX_3X2_F*, FLOAT, BOOL*));
        CALL_COUNTER_WITH_MOCK(CompareWithGeometryMethod, HRESULT(ID2D1Geometry*, CONST D2D1_MATRIX_3X2_F*, FLOAT, D2D1_GEOMETRY_RELATION*));
        CALL_COUNTER_WITH_MOCK(SimplifyMethod, HRESULT(D2D1_GEOMETRY_SIMPLIFICATION_OPTION, CONST D2D1_MATRIX_3X2_F*, FLOAT, ID2D1SimplifiedGeometrySink*));
        CALL_COUNTER_WITH_MOCK(TessellateMethod, HRESULT(CONST D2D1_MATRIX_3X2_F*, FLOAT, ID2D1TessellationSink*));
        CALL_COUNTER_WITH_MOCK(CombineWithGeometryMethod, HRESULT(ID2D1Geometry*, D2D1_COMBINE_MODE, CONST D2D1_MATRIX_3X2_F*, FLOAT, ID2D1SimplifiedGeometrySink*));
        CALL_COUNTER_WITH_MOCK(OutlineMethod, HRESULT(CONST D2D1_MATRIX_3X2_F*, FLOAT, ID2D1SimplifiedGeometrySink*));
        CALL_COUNTER_WITH_MOCK(ComputeAreaMethod, HRESULT(CONST D2D1_MATRIX_3X2_F*, FLOAT, FLOAT*));
        CALL_COUNTER_WITH_MOCK(ComputeLengthMethod, HRESULT(CONST D2D1_MATRIX_3X2_F*, FLOAT, FLOAT*));
        CALL_COUNTER_WITH_MOCK(ComputePointAtLengthMethod, HRESULT(FLOAT, CONST D2D1_MATRIX_3X2_F*, FLOAT, D2D1_POINT_2F*, D2D1_POINT_2F*));
        CALL_COUNTER_WITH_MOCK(WidenMethod, HRESULT(FLOAT, ID2D1StrokeStyle*, CONST D2D1_MATRIX_3X2_F*, FLOAT, ID2D1SimplifiedGeometrySink*));

        CALL_COUNTER_WITH_MOCK(GetFactoryMethod, void(ID2D1Factory**));

        //
        // ID2D1PathGeometry1
        //

        STDMETHOD(ComputePointAndSegmentAtLength)(
            FLOAT length,
            UINT32 startSegment,
            CONST D2D1_MATRIX_3X2_F* worldTransform,
            FLOAT flatteningTolerance,
            D2D1_POINT_DESCRIPTION* pointDescription) const override
        {
            return ComputePointAndSegmentAtLengthMethod.WasCalled(length, startSegment, worldTransform, flatteningTolerance, pointDescription);
        }

        //
        // ID2D1PathGeometry
        //

        STDMETHOD(Open)(
            ID2D1GeometrySink** geometrySink) override
        {
            return OpenMethod.WasCalled(geometrySink);
        }

        STDMETHOD(Stream)(
            ID2D1GeometrySink* geometrySink) const override
        {
            return StreamMethod.WasCalled(geometrySink);
        }

        STDMETHOD(GetSegmentCount)(
            UINT32* count) const override
        {
            return GetSegmentCountMethod.WasCalled(count);
        }

        STDMETHOD(GetFigureCount)(
            UINT32* count) const override
        {
            return GetFigureCountMethod.WasCalled(count);
        }

        //
        // ID2D1Geometry
        //

        STDMETHOD(GetBounds)(
            CONST D2D1_MATRIX_3X2_F* worldTransform,
            D2D1_RECT_F* bounds) const override
        {
            return GetBoundsMethod.WasCalled(worldTransform, bounds);
        }

        STDMETHOD(GetWidenedBounds)(
            FLOAT strokeWidth,
            ID2D1StrokeStyle* strokeStyle,
            CONST D2D1_MATRIX_3X2_F* worldTransform,
            FLOAT flatteningTolerance,
            D2D1_RECT_F* bounds) const override
        {
            return GetWidenedBoundsMethod.WasCalled(strokeWidth, strokeStyle, worldTransform, flatteningTolerance, bounds);
        }

        STDMETHOD(StrokeContainsPoint)(
            D2D1_POINT_2F point,
            FLOAT strokeWidth,
            ID2D1StrokeStyle* strokeStyle,
            CONST D2D1_MATRIX_3X2_F* worldTransform,
            FLOAT flatteningTolerance,
            BOOL* contains) const override
        {
            return StrokeContainsPointMethod.WasCalled(point, strokeWidth, strokeStyle, worldTransform, flatteningTolerance, contains);
        }

        STDMETHOD(FillContainsPoint)(
            D2D1_POINT_2F point,
            CONST D2D1_MATRIX_3X2_F* worldTransform,
            FLOAT flatteningTolerance,
            BOOL* contains) const override
        {
            return FillContainsPointMethod.WasCalled(point, worldTransform, flatteningTolerance, contains);
        }

        STDMETHOD(CompareWithGeometry)(
            ID2D1Geometry* inputGeometry,
            CONST D2D1_MATRIX_3X2_F* inputGeometryTransform,
            FLOAT flatteningTolerance,
            D2D1_GEOMETRY_RELATION* relation) const override
        {
            return CompareWithGeometryMethod.WasCalled(inputGeometry, inputGeometryTransform, flatteningTolerance, relation);
        }

        STDMETHOD(Simplify)(
            D2D1_GEOMETRY_SIMPLIFICATION_OPTION simplificationOption,
            CONST D2D1_MATRIX_3X2_F* worldTransform,
            FLOAT flatteningTolerance,
            ID2D1SimplifiedGeometrySink* geometrySink) const override
        {
            return SimplifyMethod.WasCalled(simplificationOption, worldTransform, flatteningTolerance, geometrySink);
        }

        STDMETHOD(Tessellate)(
            CONST D2D1_MATRIX_3X2_F* worldTransform,
            FLOAT flatteningTolerance,
            ID2D1TessellationSink* tessellationSink) const override
        {
            return TessellateMethod.WasCalled(worldTransform, flatteningTolerance, tessellationSink);
        }

        STDMETHOD(CombineWithGeometry)(
            ID2D1Geometry* inputGeometry,
            D2D1_COMBINE_MODE combineMode,
            CONST D2D1_MATRIX_3X2_F* inputGeometryTransform,
            FLOAT flatteningTolerance,
            ID2D1SimplifiedGeometrySink* geometrySink) const override
        {
            return CombineWithGeometryMethod.WasCalled(inputGeometry, combineMode, inputGeometryTransform, flatteningTolerance, geometrySink);
        }

        STDMETHOD(Outline)(
            CONST D2D1_MATRIX_3X2_F* worldTransform,
            FLOAT flatteningTolerance,
            ID2D1SimplifiedGeometrySink* geometrySink) const override
        {
            return OutlineMethod.WasCalled(worldTransform, flatteningTolerance, geometrySink);
        }

        STDMETHOD(ComputeArea)(
            CONST D2D1_MATRIX_3X2_F* worldTransform,
            FLOAT flatteningTolerance,
            FLOAT* area) const override
        {
            return ComputeAreaMethod.WasCalled(worldTransform, flatteningTolerance, area);
        }

        STDMETHOD(ComputeLength)(
            CONST D2D1_MATRIX_3X2_F* worldTransform,
            FLOAT flatteningTolerance,
            FLOAT* length) const override
        {
            return ComputeLengthMethod.WasCalled(worldTransform, flatteningTolerance, length);
        }

        STDMETHOD(ComputePointAtLength)(
            FLOAT length,
            CONST D2D1_MATRIX_3X2_F* worldTransform,
            FLOAT flatteningTolerance,
            D2D1_POINT_2F* point,
            D2D1_POINT_2F* unitTangentVector) const override
        {
            return ComputePointAtLengthMethod.WasCalled(length, worldTransform, flatteningTolerance, point, unitTangentVector);
        }

        STDMETHOD(Widen)(
            FLOAT strokeWidth,
            ID2D1StrokeStyle* strokeStyle,
            CONST D2D1_MATRIX_3X2_F* worldTransform,
            FLOAT flatteningTolerance,
            ID2D1SimplifiedGeometrySink* geometrySink) const override
        {
            return WidenMethod.WasCalled(strokeWidth, strokeStyle, worldTransform, flatteningTolerance, geometrySink);
        }

        //
        // ID2D1Resource
        //

        STDMETHOD_(void, GetFactory)(
            ID2D1Factory** factory) const override
        {
            GetFactoryMethod.WasCalled(factory);
        }

    };
}
