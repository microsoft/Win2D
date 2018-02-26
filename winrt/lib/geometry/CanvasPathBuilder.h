// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Geometry
{
    using namespace ::Microsoft::WRL;
    using namespace ABI::Microsoft::Graphics::Canvas::Numerics;

    class CanvasPathBuilderFactory
        : public AgileActivationFactory<ICanvasPathBuilderFactory>,
          private LifespanTracker<CanvasPathBuilderFactory>
    {
        InspectableClassStatic(RuntimeClass_Microsoft_Graphics_Canvas_Geometry_CanvasPathBuilder, BaseTrust);

    public:
        IFACEMETHOD(Create)(
            ICanvasResourceCreator* resourceAllocator,
            ICanvasPathBuilder** canvasPathBuilder) override;
    };
    
    class __declspec(uuid("DC532EB2-A103-4EEA-914A-FC1B99E3F657"))
    ICanvasPathBuilderInternal : public IUnknown
    {
    public:
        virtual GeometryDevicePtr GetGeometryDevice() = 0;

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

        //
        // Although this ties together several bits of state, no attempt is made
        // to make CanvasPathBuilder thread safe.  It is hard to imagine any
        // valid scenario where a path may be meaningfully built simultaneously
        // from multiple threads.
        //
        
        GeometryDevicePtr m_device;
        ClosablePtr<ID2D1GeometrySink> m_d2dGeometrySink;
        ClosablePtr<ID2D1PathGeometry1> m_d2dPathGeometry;
        bool m_isInFigure;
        bool m_beginFigureOccurred;

    public:
        CanvasPathBuilder(GeometryDevicePtr const& device);

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

        virtual GeometryDevicePtr GetGeometryDevice() override;

        virtual ComPtr<ID2D1GeometrySink> GetGeometrySink() override;

        virtual ComPtr<ID2D1PathGeometry1> CloseAndReturnPath() override;

    private:
        void ValidateIsInFigure();
    };
}}}}}
