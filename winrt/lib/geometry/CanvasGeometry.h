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

#include "CanvasStrokeStyle.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas
{
    using namespace ::Microsoft::WRL;
    using namespace Numerics;

    class CanvasGeometry;
    class CanvasGeometryManager;

    struct CanvasGeometryTraits
    {
        typedef ID2D1Geometry resource_t;
        typedef CanvasGeometry wrapper_t;
        typedef ICanvasGeometry wrapper_interface_t;
        typedef CanvasGeometryManager manager_t;
    };

    class CanvasGeometry : RESOURCE_WRAPPER_RUNTIME_CLASS(
        CanvasGeometryTraits,
        IClosable)
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_CanvasGeometry, BaseTrust);

        ClosablePtr<ICanvasDevice> m_canvasDevice;

    public:
        CanvasGeometry(
            std::shared_ptr<CanvasGeometryManager> manager,
            ID2D1Geometry* d2dGeometry,
            ICanvasDevice* canvasDevice);

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

    class CanvasGeometryManager : public ResourceManager<CanvasGeometryTraits>
    {
    public:
        ComPtr<CanvasGeometry> CreateNew(
            ICanvasResourceCreator* device,
            Rect rect);

        ComPtr<CanvasGeometry> CreateNew(
            ICanvasResourceCreator* device,
            Vector2 center,
            float radiusX,
            float radiusY);

        ComPtr<CanvasGeometry> CreateNew(
            ICanvasResourceCreator* device,
            Rect rect,
            float radiusX,
            float radiusY);

        ComPtr<CanvasGeometry> CreateNew(
            ICanvasPathBuilder* pathBuilder);

        ComPtr<CanvasGeometry> CreateWrapper(
            ICanvasDevice* device,
            ID2D1Geometry* resource);

    };

    class CanvasGeometryFactory
        : public ActivationFactory<
            ICanvasGeometryStatics,
            CloakedIid<ICanvasDeviceResourceFactoryNative>> ,
            public PerApplicationManager<CanvasGeometryFactory, CanvasGeometryManager>
    {
        InspectableClassStatic(RuntimeClass_Microsoft_Graphics_Canvas_CanvasGeometry, BaseTrust);

    public:
        IMPLEMENT_DEFAULT_ICANVASDEVICERESOURCEFACTORYNATIVE();

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

        IFACEMETHOD(get_DefaultFlatteningTolerance)(float* value) override;
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
}}}}
