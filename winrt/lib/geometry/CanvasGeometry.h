// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#include "drawing/CanvasStrokeStyle.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Geometry
{
    using namespace ::Microsoft::WRL;
    using namespace Numerics;

    class GeometryAdapter;
    class DefaultGeometryAdapter;


    // When geometry is used without an associated CanvasDevice, this singleton provides
    // the necessary D2D factory. Thus all deviceless geometry will share the same factory.
    class StandaloneGeometryFactory : public Singleton<StandaloneGeometryFactory>
    {
        ComPtr<ID2D1Factory2> m_d2dFactory;

    public:
        StandaloneGeometryFactory();

        ComPtr<ID2D1Factory2> GetD2DFactory() const { return m_d2dFactory; }
    };


    // Abstracts the details of how CanvasGeometry and CanvasPathBuilder track which device they
    // are associated with. This can be either a ComPtr to a CanvasDevice, or a StandaloneGeometryFactory
    // if the Win2D geometry objects were created without an associated device.
    class GeometryDevicePtr
    {
        // These fields are alternatives: one or the other of them will always be null.
        ComPtr<ICanvasDevice> m_canvasDevice;
        std::shared_ptr<StandaloneGeometryFactory> m_standaloneFactory;

    public:
        explicit GeometryDevicePtr(ICanvasResourceCreator* resourceCreator);
        explicit GeometryDevicePtr(ICanvasDevice* canvasDevice);

        void Close();
        GeometryDevicePtr const& EnsureNotClosed();

        ComPtr<ICanvasDevice> GetCanvasDevice() const;
        ComPtr<ID2D1Factory2> GetD2DFactory() const;
    };


    // Adapter allows unit tests to intercept how D2D geometry objects are created (for inserting mocks).
    class GeometryAdapter : public Singleton<GeometryAdapter, DefaultGeometryAdapter>
    {
    public:
        virtual ~GeometryAdapter() = default;

        virtual ComPtr<ID2D1RectangleGeometry> CreateRectangleGeometry(GeometryDevicePtr const& device, D2D1_RECT_F const& rectangle) = 0;
        virtual ComPtr<ID2D1EllipseGeometry> CreateEllipseGeometry(GeometryDevicePtr const& device, D2D1_ELLIPSE const& ellipse) = 0;
        virtual ComPtr<ID2D1RoundedRectangleGeometry> CreateRoundedRectangleGeometry(GeometryDevicePtr const& device, D2D1_ROUNDED_RECT const& roundedRect) = 0;
        virtual ComPtr<ID2D1PathGeometry1> CreatePathGeometry(GeometryDevicePtr const& device) = 0;
        virtual ComPtr<ID2D1GeometryGroup> CreateGeometryGroup(GeometryDevicePtr const& device, D2D1_FILL_MODE fillMode, ID2D1Geometry** d2dGeometries, uint32_t geometryCount) = 0;
        virtual ComPtr<ID2D1TransformedGeometry> CreateTransformedGeometry(GeometryDevicePtr const& device, ID2D1Geometry* d2dGeometry, D2D1_MATRIX_3X2_F* transform) = 0;
    };


    class DefaultGeometryAdapter : public GeometryAdapter
    {
    public:
        virtual ComPtr<ID2D1RectangleGeometry> CreateRectangleGeometry(GeometryDevicePtr const& device, D2D1_RECT_F const& rectangle) override;
        virtual ComPtr<ID2D1EllipseGeometry> CreateEllipseGeometry(GeometryDevicePtr const& device, D2D1_ELLIPSE const& ellipse) override;
        virtual ComPtr<ID2D1RoundedRectangleGeometry> CreateRoundedRectangleGeometry(GeometryDevicePtr const& device, D2D1_ROUNDED_RECT const& roundedRect) override;
        virtual ComPtr<ID2D1PathGeometry1> CreatePathGeometry(GeometryDevicePtr const& device) override;
        virtual ComPtr<ID2D1GeometryGroup> CreateGeometryGroup(GeometryDevicePtr const& device, D2D1_FILL_MODE fillMode, ID2D1Geometry** d2dGeometries, uint32_t geometryCount) override;
        virtual ComPtr<ID2D1TransformedGeometry> CreateTransformedGeometry(GeometryDevicePtr const& device, ID2D1Geometry* d2dGeometry, D2D1_MATRIX_3X2_F* transform) override;
    };


    class CanvasGeometry : RESOURCE_WRAPPER_RUNTIME_CLASS(
        ID2D1Geometry,
        CanvasGeometry,
        ICanvasGeometry,
        ABI::Windows::Graphics::IGeometrySource2D,
        ABI::Windows::Graphics::IGeometrySource2DInterop,
        CloakedIid<ICanvasResourceWrapperWithDevice>)
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_Geometry_CanvasGeometry, BaseTrust);

        GeometryDevicePtr m_device;

    public:
        static ComPtr<CanvasGeometry> CreateNew(
            ICanvasResourceCreator* device,
            Rect rect);

        static ComPtr<CanvasGeometry> CreateNew(
            ICanvasResourceCreator* device,
            Vector2 center,
            float radiusX,
            float radiusY);

        static ComPtr<CanvasGeometry> CreateNew(
            ICanvasResourceCreator* device,
            Rect rect,
            float radiusX,
            float radiusY);

        static ComPtr<CanvasGeometry> CreateNew(
            ICanvasPathBuilder* pathBuilder);

        static ComPtr<CanvasGeometry> CreateNew(
            ICanvasResourceCreator* resourceCreator,
            uint32_t pointCount,
            Vector2* points);

        static ComPtr<CanvasGeometry> CreateNew(
            ICanvasResourceCreator* resourceCreator,
            uint32_t geometryCount,
            ICanvasGeometry** geometryElements,
            CanvasFilledRegionDetermination filledRegionDetermination);

        static ComPtr<CanvasGeometry> CreateNew(
            ICanvasTextLayout* textLayout);

        static ComPtr<CanvasGeometry> CreateNew(
            ICanvasResourceCreator* resourceCreator,
            Vector2 point,
            ICanvasFontFace* fontFace,
            float fontSize,
            uint32_t glyphCount,
            CanvasGlyph* glyphs,
            boolean isSideways,
            uint32_t bidiLevel,
            CanvasTextMeasuringMode measuringMode,
            CanvasGlyphOrientation glyphOrientation);

#if WINVER > _WIN32_WINNT_WINBLUE && WINUI3_SUPPORTS_INKING
        static ComPtr<CanvasGeometry> CreateNew(
            ICanvasResourceCreator* resourceCreator,
            IIterable<InkStroke*>* inkStrokes,
            Matrix3x2 transform,
            float flatteningTolerance);
#endif

        CanvasGeometry(GeometryDevicePtr const& device, ID2D1Geometry* d2dGeometry);
        CanvasGeometry(ICanvasDevice* device, ID2D1Geometry* d2dGeometry);

        IFACEMETHOD(Close)();

        IFACEMETHOD(get_Device)(ICanvasDevice** device);

        IFACEMETHOD(CombineWith)(
            ICanvasGeometry* otherGeometry,
            Matrix3x2 otherGeometryTransform,
            CanvasGeometryCombine combine,
            ICanvasGeometry** geometry) override;

        IFACEMETHOD(CombineWithUsingFlatteningTolerance)(
            ICanvasGeometry* otherGeometry,
            Matrix3x2 otherGeometryTransform,
            CanvasGeometryCombine combine,
            float flatteningTolerance,
            ICanvasGeometry** geometry) override;

        IFACEMETHOD(Stroke)(
            float strokeWidth,
            ICanvasGeometry** geometry) override;

        IFACEMETHOD(StrokeWithStrokeStyle)(
            float strokeWidth,
            ICanvasStrokeStyle* strokeStyle,
            ICanvasGeometry** geometry) override;

        IFACEMETHOD(StrokeWithAllOptions)(
            float strokeWidth,
            ICanvasStrokeStyle* strokeStyle,
            Matrix3x2 transform,
            float flatteningTolerance,
            ICanvasGeometry** geometry) override;

        IFACEMETHOD(Outline)(
            ICanvasGeometry** geometry) override;

        IFACEMETHOD(OutlineWithTransformAndFlatteningTolerance)(
            Matrix3x2 transform,
            float flatteningTolerance,
            ICanvasGeometry** geometry) override;

        IFACEMETHOD(Simplify)(
            CanvasGeometrySimplification simplification,
            ICanvasGeometry** geometry) override;

        IFACEMETHOD(SimplifyWithTransformAndFlatteningTolerance)(
            CanvasGeometrySimplification simplification,
            Matrix3x2 transform,
            float flatteningTolerance,
            ICanvasGeometry** geometry) override;

        IFACEMETHOD(Transform)(
            Numerics::Matrix3x2 transform,
            ICanvasGeometry** geometry) override;

        IFACEMETHOD(CompareWith)(
            ICanvasGeometry* otherGeometry,
            CanvasGeometryRelation* relation) override;

        IFACEMETHOD(CompareWithUsingTransformAndFlatteningTolerance)(
            ICanvasGeometry* otherGeometry,
            Matrix3x2 otherGeometryTransform,
            float flatteningTolerance,
            CanvasGeometryRelation* relation) override;

        IFACEMETHOD(ComputeArea)(
            float* area) override;

        IFACEMETHOD(ComputeAreaWithTransformAndFlatteningTolerance)(
            Matrix3x2 transform,
            float flatteningTolerance,
            float* area) override;

        IFACEMETHOD(ComputePathLength)(
            float* area) override;

        IFACEMETHOD(ComputePathLengthWithTransformAndFlatteningTolerance)(
            Matrix3x2 transform,
            float flatteningTolerance,
            float* area) override;

        IFACEMETHOD(ComputePointOnPath)(
            float distance,
            Vector2* point) override;

        IFACEMETHOD(ComputePointOnPathWithTangent)(
            float distance,
            Vector2* tangent,
            Vector2* point) override;

        IFACEMETHOD(ComputePointOnPathWithTransformAndFlatteningToleranceAndTangent)(
            float distance,
            Matrix3x2 transform,
            float flatteningTolerance,
            Vector2* tangent,
            Vector2* point) override;

        IFACEMETHOD(FillContainsPoint)(
            Vector2 point,
            boolean* containsPoint) override;

        IFACEMETHOD(FillContainsPointWithTransformAndFlatteningTolerance)(
            Vector2 point,
            Matrix3x2 transform,
            float flatteningTolerance,
            boolean* containsPoint) override;

        IFACEMETHOD(ComputeBounds)(
            Rect* bounds) override;

        IFACEMETHOD(ComputeBoundsWithTransform)(
            Matrix3x2 transform,
            Rect* bounds) override;

        IFACEMETHOD(ComputeStrokeBounds)(
            float strokeWidth,
            Rect* bounds) override;

        IFACEMETHOD(ComputeStrokeBoundsWithStrokeStyle)(
            float strokeWidth,
            ICanvasStrokeStyle* strokeStyle,
            Rect* bounds) override;

        IFACEMETHOD(ComputeStrokeBoundsWithAllOptions)(
            float strokeWidth,
            ICanvasStrokeStyle* strokeStyle,
            Matrix3x2 transform,
            float flatteningTolerance,
            Rect* bounds) override;

        IFACEMETHOD(StrokeContainsPoint)(
            Vector2 point,
            float strokeWidth,
            boolean* containsPoint) override;

        IFACEMETHOD(StrokeContainsPointWithStrokeStyle)(
            Vector2 point,
            float strokeWidth,
            ICanvasStrokeStyle* strokeStyle,
            boolean* containsPoint) override;

        IFACEMETHOD(StrokeContainsPointWithAllOptions)(
            Vector2 point,
            float strokeWidth,
            ICanvasStrokeStyle* strokeStyle,
            Matrix3x2 transform,
            float flatteningTolerance,
            boolean* containsPoint) override;

        IFACEMETHOD(Tessellate)(
            UINT32* trianglesCount,
            CanvasTriangleVertices** triangles) override;

        IFACEMETHOD(TessellateWithTransformAndFlatteningTolerance)(
            Matrix3x2 transform,
            float flatteningTolerance,
            UINT32* trianglesCount,
            CanvasTriangleVertices** triangles) override;

        IFACEMETHOD(SendPathTo)(
            ICanvasPathReceiver* streamReader) override;

        // IGeometrySource2DInterop
        IFACEMETHOD(GetGeometry)(
            ID2D1Geometry** geometry) override;

        IFACEMETHOD(TryGetGeometryUsingFactory)(
            ID2D1Factory* factory,
            ID2D1Geometry** geometry) override;

    private:
        void StrokeImpl(
            float strokeWidth,
            ICanvasStrokeStyle* strokeStyle,
            Matrix3x2* transform,
            float flatteningTolerance,
            ICanvasGeometry** geometry);

        void StrokeImpl(
            float strokeWidth,
            ICanvasStrokeStyle* strokeStyle,
            Matrix3x2* transform,
            float flatteningTolerance,
            ICanvasPathBuilder* targetPathBuilder);

        void StrokeContainsPointImpl(
            Vector2 point,
            float strokeWidth,
            ICanvasStrokeStyle* strokeStyle,
            Matrix3x2* transform,
            float flatteningTolerance,
            boolean* containsPoint);

        void ComputeStrokeBoundsImpl(
            float strokeWidth,
            ICanvasStrokeStyle* strokeStyle,
            Matrix3x2* transform,
            float flatteningTolerance,
            Rect* bounds);

        void ComputePointOnPathImpl(
            float distance,
            Matrix3x2* transform,
            float flatteningTolerance,
            Vector2* tangent,
            Vector2* point);
    };


    class CanvasGeometryFactory
        : public AgileActivationFactory<ICanvasGeometryStatics>
        , private LifespanTracker<CanvasGeometryFactory>
    {
        InspectableClassStatic(RuntimeClass_Microsoft_Graphics_Canvas_Geometry_CanvasGeometry, BaseTrust);

    public:
        IFACEMETHOD(CreateRectangle)(
            ICanvasResourceCreator* resourceCreator,
            Rect rect,
            ICanvasGeometry** geometry) override;

        IFACEMETHOD(CreateRectangleAtCoords)(
            ICanvasResourceCreator* resourceCreator,
            float x,
            float y,
            float w,
            float h,
            ICanvasGeometry** geometry) override;

        IFACEMETHOD(CreateRoundedRectangle)(
            ICanvasResourceCreator* resourceCreator,
            Rect rect,
            float radiusX,
            float radiusY,
            ICanvasGeometry** geometry) override;

        IFACEMETHOD(CreateRoundedRectangleAtCoords)(
            ICanvasResourceCreator* resourceCreator,
            float x,
            float y,
            float w,
            float h,
            float radiusX,
            float radiusY,
            ICanvasGeometry** geometry) override;

        IFACEMETHOD(CreateEllipse)(
            ICanvasResourceCreator* resourceCreator,
            Numerics::Vector2 centerPoint,
            float radiusX,
            float radiusY,
            ICanvasGeometry** geometry) override;

        IFACEMETHOD(CreateEllipseAtCoords)(
            ICanvasResourceCreator* resourceCreator,
            float x,
            float y,
            float radiusX,
            float radiusY,
            ICanvasGeometry** geometry) override;

        IFACEMETHOD(CreateCircle)(
            ICanvasResourceCreator* resourceCreator,
            Numerics::Vector2 centerPoint,
            float radius,
            ICanvasGeometry** geometry) override;

        IFACEMETHOD(CreateCircleAtCoords)(
            ICanvasResourceCreator* resourceCreator,
            float x,
            float y,
            float radius,
            ICanvasGeometry** geometry) override;

        IFACEMETHOD(CreatePath)(
            ICanvasPathBuilder* pathBuilder,
            ICanvasGeometry** geometry) override;

        IFACEMETHOD(CreatePolygon)(
            ICanvasResourceCreator* resourceCreator,
            uint32_t pointCount,
            Numerics::Vector2* points,
            ICanvasGeometry** geometry) override;

        IFACEMETHOD(CreateGroup)(
            ICanvasResourceCreator* resourceCreator,
            uint32_t geometryCount,
            ICanvasGeometry** geometryElements,
            ICanvasGeometry** geometry) override;

        IFACEMETHOD(CreateGroupWithFilledRegionDetermination)(
            ICanvasResourceCreator* resourceCreator,
            uint32_t geometryCount,
            ICanvasGeometry** geometryElements,
            CanvasFilledRegionDetermination filledRegionDetermination,
            ICanvasGeometry** geometry) override;

        IFACEMETHOD(CreateText)(
            ICanvasTextLayout* textLayout,
            ICanvasGeometry** geometry) override;

        IFACEMETHOD(CreateGlyphRun)(
            ICanvasResourceCreator* resourceCreator,
            Vector2 point,
            ICanvasFontFace* fontFace,
            float fontSize,
            uint32_t glyphCount,
            CanvasGlyph* glyphs,
            boolean isSideways,
            uint32_t bidiLevel,
            CanvasTextMeasuringMode measuringMode,
            CanvasGlyphOrientation glyphOrientation,
            ICanvasGeometry** geometry) override;

#if WINVER > _WIN32_WINNT_WINBLUE

#ifdef WINUI3_SUPPORTS_INKING

        IFACEMETHOD(CreateInk)(
            ICanvasResourceCreator* resourceCreator,
            IIterable<InkStroke*>* inkStrokes,
            ICanvasGeometry** geometry) override;

        IFACEMETHOD(CreateInkWithTransformAndFlatteningTolerance)(
            ICanvasResourceCreator* resourceCreator,
            IIterable<InkStroke*>* inkStrokes,
            Matrix3x2 transform,
            float flatteningTolerance,
            ICanvasGeometry** geometry) override;

#endif

#endif

        IFACEMETHOD(ComputeFlatteningTolerance)(
            float dpi,
            float maximumZoomFactor,
            float* flatteningTolerance) override;

        IFACEMETHOD(ComputeFlatteningToleranceWithTransform)(
            float dpi,
            float maximumZoomFactor,
            Matrix3x2 expectedGeometryTransform,
            float* flatteningTolerance) override;

        IFACEMETHOD(get_DefaultFlatteningTolerance)(float* theValue) override;
    };

    inline ComPtr<ID2D1StrokeStyle> MaybeGetStrokeStyleResource(
        ID2D1Resource* factoryOwner,
        ICanvasStrokeStyle* strokeStyle)
    {
        ComPtr<ID2D1StrokeStyle> d2dStrokeStyle;
        if (strokeStyle)
        {
            auto strokeStyleInternal = As<ICanvasStrokeStyleInternal>(strokeStyle);
            ComPtr<ID2D1Factory> d2dFactory;
            factoryOwner->GetFactory(&d2dFactory);
            d2dStrokeStyle = strokeStyleInternal->GetRealizedD2DStrokeStyle(d2dFactory.Get());
        }
        return d2dStrokeStyle;
    }
}}}}}
