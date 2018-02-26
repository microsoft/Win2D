// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

#include "CanvasPathBuilder.h"

using namespace ABI::Microsoft::Graphics::Canvas::Geometry;
using namespace ABI::Microsoft::Graphics::Canvas;

IFACEMETHODIMP CanvasPathBuilderFactory::Create(
    ICanvasResourceCreator* resourceAllocator,
    ICanvasPathBuilder** canvasPathBuilder)
{
    return ExceptionBoundary(
        [&]
        {
            CheckAndClearOutPointer(canvasPathBuilder);

            GeometryDevicePtr device(resourceAllocator);

            auto newCanvasPathBuilder = Make<CanvasPathBuilder>(device);
            CheckMakeResult(newCanvasPathBuilder);

            ThrowIfFailed(newCanvasPathBuilder.CopyTo(canvasPathBuilder));
        });
}

CanvasPathBuilder::CanvasPathBuilder(GeometryDevicePtr const& device)
    : m_device(device)
    , m_isInFigure(false)
    , m_beginFigureOccurred(false)
{
    auto d2dPathGeometry = GeometryAdapter::GetInstance()->CreatePathGeometry(m_device);

    ComPtr<ID2D1GeometrySink> d2dGeometrySink;
    ThrowIfFailed(d2dPathGeometry->Open(&d2dGeometrySink));

    m_d2dGeometrySink = d2dGeometrySink;

    m_d2dPathGeometry = d2dPathGeometry;

}

IFACEMETHODIMP CanvasPathBuilder::Close()
{
    if (m_d2dGeometrySink)
    {
        auto d2dGeometrySink = m_d2dGeometrySink.Close();

        //
        // The word 'Close' is overloaded here.
        // ID2D1GeometrySink::Close is required to make the sink usable by the 
        // path geometry. This is different from simply closing the smart pointer. 
        //
        d2dGeometrySink->Close();

        m_d2dPathGeometry.Close();

        m_device.Close();
    }

    return S_OK;
}

IFACEMETHODIMP CanvasPathBuilder::BeginFigureWithFigureFill(
    Vector2 startPoint,
    CanvasFigureFill figureFill)
{
    return ExceptionBoundary(
        [&]
        {
            auto& d2dGeometrySink = m_d2dGeometrySink.EnsureNotClosed();

            if (m_isInFigure)
            {
                ThrowHR(E_INVALIDARG, Strings::TwoBeginFigures);
            }

            d2dGeometrySink->BeginFigure(ToD2DPoint(startPoint), static_cast<D2D1_FIGURE_BEGIN>(figureFill));

            m_isInFigure = true;

            m_beginFigureOccurred = true;
        });
}

IFACEMETHODIMP CanvasPathBuilder::BeginFigure(
    Vector2 startPoint)
{
    return BeginFigureWithFigureFill(startPoint, CanvasFigureFill::Default);
}

IFACEMETHODIMP CanvasPathBuilder::BeginFigureAtCoordsWithFigureFill(
    float startX,
    float startY,
    CanvasFigureFill figureFill)
{
    return BeginFigureWithFigureFill(Vector2{ startX, startY }, figureFill);
}

IFACEMETHODIMP CanvasPathBuilder::BeginFigureAtCoords(
    float startX,
    float startY)
{
    return BeginFigureWithFigureFill(Vector2{ startX, startY }, CanvasFigureFill::Default);
}

IFACEMETHODIMP CanvasPathBuilder::AddArcToPoint(
    Vector2 endPoint,
    float xRadius,
    float yRadius,
    float rotationAngle,
    CanvasSweepDirection sweepDirection,
    CanvasArcSize arcSize)
{
    return ExceptionBoundary(
        [&]
        {
            auto& d2dGeometrySink = m_d2dGeometrySink.EnsureNotClosed();

            ValidateIsInFigure();

            d2dGeometrySink->AddArc(
                D2D1::ArcSegment(
                    ToD2DPoint(endPoint), 
                    D2D1::SizeF(xRadius, yRadius), 
                    ::DirectX::XMConvertToDegrees(rotationAngle),
                    static_cast<D2D1_SWEEP_DIRECTION>(sweepDirection),
                    static_cast<D2D1_ARC_SIZE>(arcSize)));
        });
}

IFACEMETHODIMP CanvasPathBuilder::AddArcAroundEllipse(
    Vector2 centerPoint,
    float radiusX,
    float radiusY,
    float startAngle,
    float sweepAngle)
{
    using namespace ::DirectX;

    return ExceptionBoundary(
        [&]
        {
            auto& d2dGeometrySink = m_d2dGeometrySink.EnsureNotClosed();

            ValidateIsInFigure();

            // If the arc sweep covers a full 360, its start and end points will be the same. That
            // does not provide enough info to fully specify an arc to D2D (there are an infinite
            // number of possible ellipses passing through a single point!) so we must number of split
            // up such requests into a pair of 180 degree arcs. Sweeps greater than 360 are clamped.
            bool isFullCircle = fabs(sweepAngle) >= XM_2PI - FLT_EPSILON;

            if (isFullCircle)
            {
                sweepAngle = (sweepAngle < 0) ? -XM_PI : XM_PI;
            }

            // Compute the arc start and end positions.
            D2D1_POINT_2F startPoint
            {
                centerPoint.X + cosf(startAngle) * radiusX,
                centerPoint.Y + sinf(startAngle) * radiusY,
            };

            D2D1_POINT_2F endPoint
            {
                centerPoint.X + cosf(startAngle + sweepAngle) * radiusX,
                centerPoint.Y + sinf(startAngle + sweepAngle) * radiusY,
            };

            // Convert the arc description to D2D format.
            D2D1_ARC_SEGMENT arc
            {
                endPoint,
                D2D1_SIZE_F{ radiusX, radiusY },
                0,
                (sweepAngle >= 0) ? D2D1_SWEEP_DIRECTION_CLOCKWISE : D2D1_SWEEP_DIRECTION_COUNTER_CLOCKWISE,
                (fabs(sweepAngle) > XM_PI) ? D2D1_ARC_SIZE_LARGE : D2D1_ARC_SIZE_SMALL,
            };

            // Insert a line to move the current path location to the arc start point.
            d2dGeometrySink->AddLine(startPoint);

            // Add the arc.
            d2dGeometrySink->AddArc(arc);

            // If necessary, add a second arc to complete a full circle.
            if (isFullCircle)
            {
                arc.point = startPoint;
                d2dGeometrySink->AddArc(arc);
            }
        });
}

IFACEMETHODIMP CanvasPathBuilder::AddCubicBezier(
    Vector2 controlPoint1,
    Vector2 controlPoint2,
    Vector2 endPoint)
{
    return ExceptionBoundary(
        [&]
        {
            auto& d2dGeometrySink = m_d2dGeometrySink.EnsureNotClosed();

            ValidateIsInFigure();

            d2dGeometrySink->AddBezier(D2D1::BezierSegment(ToD2DPoint(controlPoint1), ToD2DPoint(controlPoint2), ToD2DPoint(endPoint)));
        });
}

IFACEMETHODIMP CanvasPathBuilder::AddLine(
    Vector2 endPoint)
{
    return ExceptionBoundary(
        [&]
        {
            auto& d2dGeometrySink = m_d2dGeometrySink.EnsureNotClosed();

            ValidateIsInFigure();

            d2dGeometrySink->AddLine(ToD2DPoint(endPoint));
        });
}

IFACEMETHODIMP CanvasPathBuilder::AddLineWithCoords(
    float endPointX,
    float endPointY)
{
    return AddLine(Vector2{ endPointX, endPointY });
}

IFACEMETHODIMP CanvasPathBuilder::AddQuadraticBezier(
    Vector2 controlPoint,
    Vector2 endPoint)
{
    return ExceptionBoundary(
        [&]
        {
            auto& d2dGeometrySink = m_d2dGeometrySink.EnsureNotClosed();

            ValidateIsInFigure();

            d2dGeometrySink->AddQuadraticBezier(D2D1::QuadraticBezierSegment(ToD2DPoint(controlPoint), ToD2DPoint(endPoint)));
        });
}

IFACEMETHODIMP CanvasPathBuilder::AddGeometry(
    ICanvasGeometry* geometry)
{        
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(geometry);

            auto& d2dGeometrySink = m_d2dGeometrySink.EnsureNotClosed();

            auto otherD2DGeometry = GetWrappedResource<ID2D1Geometry>(geometry);

            if (m_isInFigure)
            {
                ThrowHR(E_INVALIDARG, Strings::PathBuilderAddGeometryMidFigure);
            }

            auto otherD2DPathGeometry = MaybeAs<ID2D1PathGeometry>(otherD2DGeometry);

            if (otherD2DPathGeometry)
            {
                ThrowIfFailed(otherD2DPathGeometry->Stream(d2dGeometrySink.Get()));
            }
            else
            {
                ThrowIfFailed(otherD2DGeometry->Simplify(
                    D2D1_GEOMETRY_SIMPLIFICATION_OPTION_CUBICS_AND_LINES,
                    nullptr,
                    d2dGeometrySink.Get()));
            }
        });
}

IFACEMETHODIMP CanvasPathBuilder::SetSegmentOptions(
    CanvasFigureSegmentOptions figureSegmentOptions)
{
    return ExceptionBoundary(
        [&]
        {
            auto& d2dGeometrySink = m_d2dGeometrySink.EnsureNotClosed();

            d2dGeometrySink->SetSegmentFlags(static_cast<D2D1_PATH_SEGMENT>(figureSegmentOptions));
        });
}

IFACEMETHODIMP CanvasPathBuilder::SetFilledRegionDetermination(
    CanvasFilledRegionDetermination filledRegionDetermination)
{
    return ExceptionBoundary(
        [&]
        {
            auto& d2dGeometrySink = m_d2dGeometrySink.EnsureNotClosed();

            if (m_beginFigureOccurred)
            {
                ThrowHR(E_INVALIDARG, Strings::SetFilledRegionDeterminationAfterBeginFigure);
            }

            d2dGeometrySink->SetFillMode(static_cast<D2D1_FILL_MODE>(filledRegionDetermination));
        });
}

IFACEMETHODIMP CanvasPathBuilder::EndFigure(
    CanvasFigureLoop figureLoop)
{
    return ExceptionBoundary(
        [&]
        {
            auto& d2dGeometrySink = m_d2dGeometrySink.EnsureNotClosed();

            if (!m_isInFigure)
            {
                ThrowHR(E_INVALIDARG, Strings::EndFigureWithoutBeginFigure);
            }

            d2dGeometrySink->EndFigure(static_cast<D2D1_FIGURE_END>(figureLoop));

            m_isInFigure = false;
        });
}

GeometryDevicePtr CanvasPathBuilder::GetGeometryDevice()
{
    auto& device = m_device.EnsureNotClosed();

    return device;
}

ComPtr<ID2D1GeometrySink> CanvasPathBuilder::GetGeometrySink()
{
    auto& geometrySink = m_d2dGeometrySink.EnsureNotClosed();

    return geometrySink;
}

ComPtr<ID2D1PathGeometry1> CanvasPathBuilder::CloseAndReturnPath()
{
    //
    // The call to Close() below will actually close out m_d2dPathGeometry,
    // so it is necessary to transfer ownership of it here.
    //
    ComPtr<ID2D1PathGeometry1> returnedPathGeometry = m_d2dPathGeometry.EnsureNotClosed();

    if (m_isInFigure)
    {
        ThrowHR(E_INVALIDARG, Strings::PathBuilderClosedMidFigure);
    }

    ThrowIfFailed(Close());

    return returnedPathGeometry;
}

void CanvasPathBuilder::ValidateIsInFigure()
{
    if (!m_isInFigure)
    {
        ThrowHR(E_INVALIDARG, Strings::CanOnlyAddPathDataWhileInFigure);
    }
}


ActivatableClassWithFactory(CanvasPathBuilder, CanvasPathBuilderFactory);
