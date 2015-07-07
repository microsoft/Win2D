// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace canvas
{
    class StubGeometrySink : public RuntimeClass<
        RuntimeClassFlags<WinRtClassicComMix>,
        ICanvasPathReceiver>
    {
    public:
        CALL_COUNTER_WITH_MOCK(BeginFigureMethod, HRESULT(Vector2, CanvasFigureFill));
        CALL_COUNTER_WITH_MOCK(AddArcMethod, HRESULT(Vector2, float, float, float, CanvasSweepDirection, CanvasArcSize));
        CALL_COUNTER_WITH_MOCK(AddCubicBezierMethod, HRESULT(Vector2, Vector2, Vector2));
        CALL_COUNTER_WITH_MOCK(AddQuadraticBezierMethod, HRESULT(Vector2, Vector2));
        CALL_COUNTER_WITH_MOCK(AddLineMethod, HRESULT(Vector2));
        CALL_COUNTER_WITH_MOCK(SetFilledRegionDeterminationMethod, HRESULT(CanvasFilledRegionDetermination));
        CALL_COUNTER_WITH_MOCK(SetSegmentOptionsMethod, HRESULT(CanvasFigureSegmentOptions));
        CALL_COUNTER_WITH_MOCK(EndFigureMethod, HRESULT(CanvasFigureLoop));

        IFACEMETHODIMP BeginFigure(
            Vector2 startPoint,
            CanvasFigureFill figureFill)
        {
            return BeginFigureMethod.WasCalled(startPoint, figureFill);
        }

        IFACEMETHODIMP AddArc(
            Vector2 endPoint,
            float radiusX,
            float radiusY,
            float rotationAngle,
            CanvasSweepDirection sweepDirection,
            CanvasArcSize arcSize)
        {
            return AddArcMethod.WasCalled(endPoint, radiusX, radiusY, rotationAngle, sweepDirection, arcSize);
        }

        IFACEMETHODIMP AddCubicBezier(
            Vector2 controlPoint1,
            Vector2 controlPoint2,
            Vector2 endPoint)
        {
            return AddCubicBezierMethod.WasCalled(controlPoint1, controlPoint2, endPoint);
        }

        IFACEMETHODIMP AddLine(
            Vector2 endPoint)
        {
            return AddLineMethod.WasCalled(endPoint);
        }

        IFACEMETHODIMP AddQuadraticBezier(
            Vector2 controlPoint,
            Vector2 endPoint)
        {
            return AddQuadraticBezierMethod.WasCalled(controlPoint, endPoint);
        }

        IFACEMETHODIMP SetFilledRegionDetermination(
            CanvasFilledRegionDetermination filledRegionDetermination)
        {
            return SetFilledRegionDeterminationMethod.WasCalled(filledRegionDetermination);
        }

        IFACEMETHODIMP SetSegmentOptions(
            CanvasFigureSegmentOptions figureSegmentOptions)
        {
            return SetSegmentOptionsMethod.WasCalled(figureSegmentOptions);
        }

        IFACEMETHODIMP EndFigure(
            CanvasFigureLoop figureLoop)
        {
            return EndFigureMethod.WasCalled(figureLoop);
        }

    };

}
