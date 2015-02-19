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
#include "CanvasPathBuilder.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas
{
    IFACEMETHODIMP CanvasPathBuilderFactory::Create(
        ICanvasResourceCreator* resourceAllocator,
        ICanvasPathBuilder** canvasPathBuilder)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(resourceAllocator);
                CheckAndClearOutPointer(canvasPathBuilder);

                ComPtr<ICanvasDevice> canvasDevice;
                ThrowIfFailed(resourceAllocator->get_Device(&canvasDevice));

                auto newCanvasPathBuilder = Make<CanvasPathBuilder>(canvasDevice.Get());
                CheckMakeResult(newCanvasPathBuilder);

                ThrowIfFailed(newCanvasPathBuilder.CopyTo(canvasPathBuilder));
            });
    }

    CanvasPathBuilder::CanvasPathBuilder(
        ICanvasDevice* canvasDevice)
        : m_canvasDevice(canvasDevice)
        , m_isInFigure(false)
        , m_beginFigureOccurred(false)
    {
        auto deviceInternal = As<ICanvasDeviceInternal>(canvasDevice);

        auto d2dPathGeometry = deviceInternal->CreatePathGeometry();

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

            m_canvasDevice.Close();
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
                    ThrowHR(E_INVALIDARG, HStringReference(Strings::TwoBeginFigures).Get());
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

    IFACEMETHODIMP CanvasPathBuilder::AddArc(
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
                    ThrowHR(E_INVALIDARG, HStringReference(Strings::PathBuilderAddGeometryMidFigure).Get());
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
                    ThrowHR(E_INVALIDARG, HStringReference(Strings::SetFilledRegionDeterminationAfterBeginFigure).Get());
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
                    ThrowHR(E_INVALIDARG, HStringReference(Strings::EndFigureWithoutBeginFigure).Get());
                }

                d2dGeometrySink->EndFigure(static_cast<D2D1_FIGURE_END>(figureLoop));

                m_isInFigure = false;
            });
    }

    ComPtr<ICanvasDevice> CanvasPathBuilder::GetDevice()
    {
        auto& canvasDevice = m_canvasDevice.EnsureNotClosed();

        return canvasDevice;
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
            ThrowHR(E_INVALIDARG, HStringReference(Strings::PathBuilderClosedMidFigure).Get());
        }

        ThrowIfFailed(Close());

        return returnedPathGeometry;
    }

    void CanvasPathBuilder::ValidateIsInFigure()
    {
        if (!m_isInFigure)
        {
            ThrowHR(E_INVALIDARG, HStringReference(Strings::CanOnlyAddPathDataWhileInFigure).Get());
        }
    }


    ActivatableClassWithFactory(CanvasPathBuilder, CanvasPathBuilderFactory);

}}}}
