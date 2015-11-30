// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

// This file was automatically generated. Please do not edit it manually.

#pragma once

namespace canvas
{
    class MockD2DInk : public RuntimeClass<
        RuntimeClassFlags<ClassicCom>,
        ChainInterfaces<ID2D1Ink, ID2D1Resource>>
    {
    public:
        CALL_COUNTER_WITH_MOCK(SetStartPointMethod, void(CONST D2D1_INK_POINT*));
        CALL_COUNTER_WITH_MOCK(GetStartPointMethod, D2D1_INK_POINT());
        CALL_COUNTER_WITH_MOCK(AddSegmentsMethod, HRESULT(CONST D2D1_INK_BEZIER_SEGMENT*, UINT32));
        CALL_COUNTER_WITH_MOCK(RemoveSegmentsAtEndMethod, HRESULT(UINT32));
        CALL_COUNTER_WITH_MOCK(SetSegmentsMethod, HRESULT(UINT32, CONST D2D1_INK_BEZIER_SEGMENT*, UINT32));
        CALL_COUNTER_WITH_MOCK(SetSegmentAtEndMethod, HRESULT(CONST D2D1_INK_BEZIER_SEGMENT*));
        CALL_COUNTER_WITH_MOCK(GetSegmentCountMethod, UINT32());
        CALL_COUNTER_WITH_MOCK(GetSegmentsMethod, HRESULT(UINT32, D2D1_INK_BEZIER_SEGMENT*, UINT32));
        CALL_COUNTER_WITH_MOCK(StreamAsGeometryMethod, HRESULT(ID2D1InkStyle*, CONST D2D1_MATRIX_3X2_F*, FLOAT, ID2D1SimplifiedGeometrySink*));
        CALL_COUNTER_WITH_MOCK(GetBoundsMethod, HRESULT(ID2D1InkStyle*, CONST D2D1_MATRIX_3X2_F*, D2D1_RECT_F*));

        CALL_COUNTER_WITH_MOCK(GetFactoryMethod, void(ID2D1Factory**));

        //
        // ID2D1Ink
        //

        STDMETHOD_(void, SetStartPoint)(
            CONST D2D1_INK_POINT* startPoint) override
        {
            SetStartPointMethod.WasCalled(startPoint);
        }

        STDMETHOD_(D2D1_INK_POINT, GetStartPoint)() const override
        {
            return GetStartPointMethod.WasCalled();
        }

        STDMETHOD(AddSegments)(
            CONST D2D1_INK_BEZIER_SEGMENT* segments,
            UINT32 segmentsCount) override
        {
            return AddSegmentsMethod.WasCalled(segments, segmentsCount);
        }

        STDMETHOD(RemoveSegmentsAtEnd)(
            UINT32 segmentsCount) override
        {
            return RemoveSegmentsAtEndMethod.WasCalled(segmentsCount);
        }

        STDMETHOD(SetSegments)(
            UINT32 startSegment,
            CONST D2D1_INK_BEZIER_SEGMENT* segments,
            UINT32 segmentsCount) override
        {
            return SetSegmentsMethod.WasCalled(startSegment, segments, segmentsCount);
        }

        STDMETHOD(SetSegmentAtEnd)(
            CONST D2D1_INK_BEZIER_SEGMENT* segment) override
        {
            return SetSegmentAtEndMethod.WasCalled(segment);
        }

        STDMETHOD_(UINT32, GetSegmentCount)() const override
        {
            return GetSegmentCountMethod.WasCalled();
        }

        STDMETHOD(GetSegments)(
            UINT32 startSegment,
            D2D1_INK_BEZIER_SEGMENT* segments,
            UINT32 segmentsCount) const override
        {
            return GetSegmentsMethod.WasCalled(startSegment, segments, segmentsCount);
        }

        STDMETHOD(StreamAsGeometry)(
            ID2D1InkStyle* inkStyle,
            CONST D2D1_MATRIX_3X2_F* worldTransform,
            FLOAT flatteningTolerance,
            ID2D1SimplifiedGeometrySink* geometrySink) const override
        {
            return StreamAsGeometryMethod.WasCalled(inkStyle, worldTransform, flatteningTolerance, geometrySink);
        }

        STDMETHOD(GetBounds)(
            ID2D1InkStyle* inkStyle,
            CONST D2D1_MATRIX_3X2_F* worldTransform,
            D2D1_RECT_F* bounds) const override
        {
            return GetBoundsMethod.WasCalled(inkStyle, worldTransform, bounds);
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
