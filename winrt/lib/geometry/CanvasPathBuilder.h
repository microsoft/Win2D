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

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Geometry
{
    using namespace ::Microsoft::WRL;
    using namespace ABI::Microsoft::Graphics::Canvas::Numerics;

    class CanvasPathBuilderFactory
        : public ActivationFactory<ICanvasPathBuilderFactory>,
          private LifespanTracker<CanvasPathBuilderFactory>
    {
        InspectableClassStatic(RuntimeClass_Microsoft_Graphics_Canvas_Geometry_CanvasPathBuilder, BaseTrust);

    public:
        IFACEMETHOD(Create)(
            ICanvasResourceCreator* resourceAllocator,
            ICanvasPathBuilder** canvasPathBuilder) override;
    };
    
    [uuid(DC532EB2-A103-4EEA-914A-FC1B99E3F657)]
    class ICanvasPathBuilderInternal : public IUnknown
    {
    public:
        virtual ComPtr<ICanvasDevice> GetDevice() = 0;

        virtual ComPtr<ID2D1GeometrySink> GetGeometrySink() = 0;

        virtual ComPtr<ID2D1PathGeometry1> CloseAndReturnPath() = 0;
    };

    class CanvasPathBuilder : public RuntimeClass<
        RuntimeClassFlags<WinRtClassicComMix>,
        ICanvasPathBuilder,
        IClosable,
        CloakedIid<ICanvasPathBuilderInternal>>,
        private LifespanTracker<CanvasPathBuilder>
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_Geometry_CanvasPathBuilder, BaseTrust);

        ClosablePtr<ICanvasDevice> m_canvasDevice;
        ClosablePtr<ID2D1GeometrySink> m_d2dGeometrySink;
        ClosablePtr<ID2D1PathGeometry1> m_d2dPathGeometry;
        bool m_isInFigure;
        bool m_beginFigureOccurred;

    public:
        CanvasPathBuilder(
            ICanvasDevice* canvasDevice);

        IFACEMETHOD(Close)();

        IFACEMETHOD(BeginFigureWithFigureFill)(
            Vector2 startPoint,
            CanvasFigureFill figureFill) override;

        IFACEMETHOD(BeginFigure)(
            Vector2 startPoint) override;

        IFACEMETHOD(BeginFigureAtCoordsWithFigureFill)(
            float startX,
            float startY,
            CanvasFigureFill figureFill) override;

        IFACEMETHOD(BeginFigureAtCoords)(
            float startX,
            float startY) override;

        IFACEMETHOD(AddArcToPoint)(
            Vector2 endPoint,
            float xRadius,
            float yRadius,
            float rotationAngle,
            CanvasSweepDirection sweepDirection,
            CanvasArcSize arcSize) override;

        IFACEMETHOD(AddArcAroundEllipse)(
            Vector2 centerPoint,
            float radiusX,
            float radiusY,
            float startAngle,
            float sweepAngle) override;

        IFACEMETHOD(AddCubicBezier)(
            Vector2 controlPoint1,
            Vector2 controlPoint2,
            Vector2 endPoint) override;

        IFACEMETHOD(AddLine)(
            Vector2 endPoint) override;

        IFACEMETHOD(AddLineWithCoords)(
            float endPointX,
            float endPointY) override;

        IFACEMETHOD(AddQuadraticBezier)(
            Vector2 controlPoint,
            Vector2 endPoint) override;

        IFACEMETHOD(AddGeometry)(
            ICanvasGeometry* geometry) override;

        IFACEMETHOD(SetSegmentOptions)(
            CanvasFigureSegmentOptions figureSegmentOptions) override;

        IFACEMETHOD(SetFilledRegionDetermination)(
            CanvasFilledRegionDetermination filledRegionDetermination) override;

        IFACEMETHOD(EndFigure)(
            CanvasFigureLoop figureLoop) override;

        //
        // ICanvasPathBuilderInternal
        //

        virtual ComPtr<ICanvasDevice> GetDevice() override;

        virtual ComPtr<ID2D1GeometrySink> GetGeometrySink() override;

        virtual ComPtr<ID2D1PathGeometry1> CloseAndReturnPath() override;

    private:
        void ValidateIsInFigure();
    };
}}}}}
